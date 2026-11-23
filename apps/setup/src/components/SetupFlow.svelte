<script lang="ts">
  import { onMount } from 'svelte';
  import { CloudflareAccount, Vault, type CloudflareStatus, type SiteAdmin } from '@online-gal/plugin-system/account';
  import DeviceLogin from './DeviceLogin.svelte';
  import { MemoryVaultStore } from '../lib/memory-vault';
  import { SETUP_ITEMS } from '../lib/setup-items';
  import {
    clearSession, clearSetupSession, isLocalHost, missingScopes, readCaFingerprint, RELOAD_KEY,
    readSession, safeNext, savedDeployment, SetupClient, unsupportedItems, type SetupConfig, WRITTEN_KEY,
  } from '../lib/setup';

  let { config }: { config: SetupConfig } = $props();
  type SignedIn = Extract<CloudflareStatus, { signedIn: true }>;
  type Step = 'status' | 'status-error' | 'stale' | 'login' | 'building' | 'build-error' | 'confirming'
    | 'confirmation-error' | 'local-preview' | 'complete';
  type ProjectState = 'idle' | 'checking' | 'confirm' | 'writing' | 'error';
  let step = $state<Step>('status');
  let project = $state<ProjectState>('idle');
  let account = $state.raw<CloudflareAccount>();
  let client: SetupClient;
  let lifetime: AbortController;
  let host = $state('');
  let next = '/';
  let errorMessage = $state('');
  let initialized = $state(false);
  let manualBuild = $state(false);
  let admin = $state.raw<SiteAdmin | null>(null);
  let productionBranch = '';
  let projectWrittenAt: string | undefined;
  let caFingerprint = $state<string>();
  let scopesMissing = $state<string[]>([]);
  /**
   * Items whose value would have to be typed in. Decision G removed the form, so the page stops
   * here instead of writing a partial configuration; no current item is one.
   */
  const blocked = unsupportedItems(SETUP_ITEMS);
  /** The write needs the Pages scopes and a configuration the page can produce by itself. */
  let allWritable = $derived(scopesMissing.length === 0 && blocked.length === 0);
  let inspection = 0;
  let redirectTimer: ReturnType<typeof setTimeout> | undefined;

  /** The account id is the owner's own; the page only has to be recognisable, not legible. */
  function maskId(id: string): string {
    return id.length <= 8 ? '•'.repeat(id.length) : `${id.slice(0, 4)}…${id.slice(-4)}`;
  }

  function complete(): void {
    account = undefined;
    clearSetupSession();
    step = 'complete';
    redirectTimer = setTimeout(() => location.replace(next), 1500);
  }

  async function checkStatus(): Promise<void> {
    step = 'status';
    try {
      const status = await client.readStatus(lifetime.signal);
      if (lifetime.signal.aborted) return;
      if (!status) {
        step = 'stale';
        if (readSession(RELOAD_KEY) !== '1') {
          writeSession(RELOAD_KEY, '1');
          if (readSession(RELOAD_KEY) === '1') location.reload();
        }
        return;
      }
      clearSession(RELOAD_KEY);
      initialized = status.initialized;
      step = 'login';
    } catch (error) {
      if (lifetime.signal.aborted) return;
      errorMessage = error instanceof Error ? error.message : '请重试。';
      step = 'status-error';
    }
  }

  async function checkSwitch(): Promise<void> {
    step = 'confirming';
    try {
      await client.waitForSwitch(lifetime.signal);
      complete();
    } catch {
      if (!lifetime.signal.aborted) step = 'confirmation-error';
    }
  }

  async function rebuild(owner: SiteAdmin, restart = false): Promise<void> {
    if (isLocalHost(host)) {
      step = 'local-preview';
      return;
    }
    step = 'building';
    errorMessage = '';
    manualBuild = false;
    const mode = config.rebuild;
    try {
      let id = restart ? null : savedDeployment(owner);
      if (mode.kind === 'pages-git') {
        id ??= await client.startPagesBuild(owner, productionBranch, lifetime.signal);
        if (!id) throw new Error('未能开始构建。');
        await client.waitForDeployment(owner, id, lifetime.signal);
      } else {
        // Decision G: there is no GitHub token, so `github-rerun` only shows the manual entry.
        manualBuild = true;
        await client.waitForNewProduction(owner, projectWrittenAt ?? readSession(WRITTEN_KEY) ?? undefined, lifetime.signal);
      }
      await checkSwitch();
    } catch (error) {
      if (lifetime.signal.aborted) return;
      errorMessage = error instanceof Error ? error.message : '构建请求失败，请重试。';
      step = 'build-error';
    }
  }

  async function inspectOwner(status: CloudflareStatus): Promise<void> {
    const run = ++inspection;
    project = 'idle';
    const missing = status.signedIn ? missingScopes(status.scopes) : [];
    scopesMissing = missing;
    admin = status.signedIn ? status.siteAdmin : null;
    if (!status.signedIn) caFingerprint = undefined;
    if (!admin || step !== 'login' || missing.length > 0) return;
    const owner = admin;
    project = 'checking';
    errorMessage = '';
    try {
      const state = await client.readProject(owner, lifetime.signal);
      if (lifetime.signal.aborted || run !== inspection || step !== 'login') return;
      productionBranch = state.branch ?? '';
      projectWrittenAt = state.writtenAt;
      if (state.written || initialized) {
        if (state.missing.length) throw new Error('项目缺少初始化配置，请在 Cloudflare 控制台检查。');
        caFingerprint = readCaFingerprint(owner, state.writtenAt);
        await rebuild(owner);
      } else {
        project = 'confirm';
      }
    } catch (error) {
      if (lifetime.signal.aborted || run !== inspection) return;
      errorMessage = error instanceof Error ? error.message : '请重试。';
      project = 'error';
    }
  }

  async function submit(status: SignedIn): Promise<void> {
    if (!admin || !allWritable || project !== 'confirm') return;
    const owner = admin;
    project = 'writing';
    errorMessage = '';
    try {
      const result = await client.write(owner, status.user, status.scopes, lifetime.signal);
      if (lifetime.signal.aborted) return;
      if (result === 'switched') complete();
      else if (result === 'already-written') await inspectOwner(status);
      else {
        if (typeof result === 'object') caFingerprint = result.fingerprint;
        void rebuild(owner);
      }
    } catch (error) {
      if (lifetime.signal.aborted) return;
      errorMessage = error instanceof Error ? error.message : '请重试。';
      project = 'confirm';
    }
  }

  onMount(() => {
    lifetime = new AbortController();
    host = location.hostname;
    next = safeNext(location.pathname + location.search);
    account = new CloudflareAccount({ origin: location.origin, siteHost: config.siteHost, fetch: fetch.bind(globalThis),
      vault: new Vault(new MemoryVaultStore()), now: Date.now });
    client = new SetupClient(location.origin, account);
    void checkStatus();
    return () => {
      lifetime.abort();
      account = undefined;
      clearTimeout(redirectTimer);
    };
  });
</script>

{#if caFingerprint}
  <p class="mb-4 text-sm" role="status">微型 CA 公钥指纹：<code class="block select-all break-all font-mono">{caFingerprint}</code></p>
{/if}
{#if initialized && step === 'login'}
  <p class="mb-4 text-sm text-base-content/70">初始化已写入。请登录后继续构建正式站点。</p>
{/if}
{#if step === 'login' && account}
  <p class="mb-4 text-sm text-base-content/70">请使用拥有本站 Pages 项目的 Cloudflare 账号登录。</p>
  <DeviceLogin {account} onStatus={(status) => void inspectOwner(status)}>
    {#snippet signedIn(status, refresh, logout, busy)}
      <div class="rounded-2xl bg-base-200 px-6 py-5">
        {#if scopesMissing.length > 0}
          <p class="font-bold">缺少 Cloudflare 权限</p>
          <p class="mt-2 text-sm text-base-content/70">
            这次授权没有包含 <span class="font-mono">{scopesMissing.join('、')}</span>。请退出，用同一个账号重新授权，并在授权页同意全部权限。
          </p>
        {:else if status.siteAdmin}
          <p class="font-bold">已确认站点所有者</p>
          <p class="mt-2 text-sm text-base-content/70">{status.siteAdmin.accountName} · <span class="font-mono">{status.siteAdmin.projectName}</span></p>
          {#if project === 'checking'}
            <p class="mt-4 text-sm" role="status">正在检查 Pages 项目配置</p>
          {:else if project === 'error'}
            <p class="mt-4 text-sm text-error" role="alert">{errorMessage}</p>
            <button class="btn btn-sm mt-4" type="button" onclick={() => void inspectOwner(status)}>重试</button>
          {:else if project === 'confirm' || project === 'writing'}
            {#if blocked.length > 0}
              <p class="mt-4 text-sm text-error" role="alert">
                该项需手动填写，初始化页暂不支持：<span class="font-mono">{blocked.map(({ name }) => name).join('、')}</span>。请在 Cloudflare 控制台添加后重新加载。
              </p>
            {:else}
              <p class="mt-4 text-sm">将一次写入下列配置。页面没有任何 token 输入框，登录凭据只留在当前页面的内存里。</p>
              <ul class="mt-3 space-y-1.5 text-sm">
                {#each SETUP_ITEMS as item (item.name)}
                  <li class="flex flex-wrap items-baseline justify-between gap-2">
                    <span>{item.label}</span>
                    <span class="font-mono text-xs break-all text-base-content/70">{item.source === 'account' ? maskId(status.siteAdmin.accountId) : '保存时生成'} · {item.name} · secret_text · production</span>
                  </li>
                {/each}
                <li class="flex flex-wrap items-baseline justify-between gap-2">
                  <span>站点初始化记录</span>
                  <span class="font-mono text-xs break-all text-base-content/70">ONLINE_GAL_SITE · plain_text · production, preview</span>
                </li>
              </ul>
              {#if errorMessage}<p role="alert" class="mt-3 text-sm text-error">{errorMessage}</p>{/if}
              <button class="btn btn-primary mt-4" type="button" disabled={busy || project === 'writing'} onclick={() => void submit(status)}>
                {project === 'writing' ? '正在写入' : '写入并构建'}
              </button>
            {/if}
          {/if}
        {:else}
          <p class="font-bold">无法确认站点所有者</p>
          <p class="mt-2 break-all text-sm text-base-content/70">{status.siteAdminError ?? `当前账号没有拥有本站（${host}）的 Pages 项目。`}</p>
          {#if status.siteAdminError}<button type="button" class="btn btn-sm mt-4" disabled={busy} onclick={refresh}>重试</button>{/if}
        {/if}
        <button type="button" class="btn btn-ghost btn-sm mt-4" disabled={busy || project === 'writing'} onclick={logout}>退出并更换账号</button>
      </div>
    {/snippet}
  </DeviceLogin>
{:else}
  <div class="rounded-2xl bg-base-200 px-6 py-5" aria-live="polite">
    {#if step === 'status'}
      <p>正在读取站点状态</p>
    {:else if step === 'status-error'}
      <p class="text-error">{errorMessage}</p>
      <button type="button" class="btn btn-sm mt-4" onclick={checkStatus}>重试</button>
    {:else if step === 'stale'}
      <p>当前页面已过期，请刷新。</p>
      <button type="button" class="btn btn-sm mt-4" onclick={() => location.reload()}>刷新页面</button>
    {:else if step === 'building' || step === 'build-error'}
      <p class="font-bold">{step === 'building' ? '等待正式站点构建' : '构建尚未完成'}</p>
      {#if step === 'build-error'}<p class="mt-2 text-sm text-error">{errorMessage}</p>{/if}
      {#if config.rebuild.kind === 'github-rerun' && (manualBuild || step === 'build-error')}
        <p class="mt-2 text-sm">请在 GitHub 重跑发布工作流，或推一个新的 <code>v*</code> 标签。</p>
        <a class="btn btn-sm mt-4" href={'https://github.com/' + config.rebuild.repo + '/actions/runs/' + config.rebuild.runId} target="_blank" rel="noopener noreferrer">打开发布工作流</a>
      {:else if manualBuild}
        <p class="mt-2 text-sm">设置 Pages 项目查询所需的环境变量后，可运行 <code>npm run build:pages</code> 检查构建产物。</p>
        <p class="mt-2 text-sm">Git 集成项目可用 <code>cf@1.0.0-rc.2</code> 触发生产构建：</p>
        <code class="mt-1 block break-all text-sm">cf pages deployments create {admin?.projectName ?? '&lt;project&gt;'} --branch {productionBranch || '&lt;production-branch&gt;'}</code>
        <p class="mt-2 text-sm">直接上传项目需要使用已配置的发布流程发布 <code>dist/site</code>。</p>
      {/if}
      {#if step === 'build-error' && admin}
        {@const owner = admin}
        <button type="button" class="btn btn-sm mt-4" onclick={() => void rebuild(owner)}>继续检查</button>
        {#if config.rebuild.kind === 'pages-git'}
          <button type="button" class="btn btn-sm mt-4" onclick={() => void rebuild(owner, true)}>重新构建</button>
          <p class="mt-2 text-sm">也可以在 Cloudflare Pages 中重新部署生产分支{productionBranch ? ` ${productionBranch}` : ''}。</p>
        {/if}
      {/if}
    {:else if step === 'confirming'}
      <p>正在确认正式站点已生效</p>
    {:else if step === 'confirmation-error'}
      <p>站点还未切换，请稍后刷新。</p>
      <button type="button" class="btn btn-sm mt-4" onclick={checkSwitch}>重新检查</button>
    {:else if step === 'complete'}
      <p>初始化已完成，正在返回原页面。</p>
    {:else if step === 'local-preview'}
      <p class="font-bold">本地预演</p>
      <ul class="mt-4 space-y-1 font-mono text-xs">
        {#each SETUP_ITEMS as item (item.name)}<li>{item.name} · production · secret_text · ••••</li>{/each}
        <li>ONLINE_GAL_SITE · production, preview · plain_text · ••••</li>
      </ul>
      <p class="mt-3 text-sm">构建方式：{config.rebuild.kind}</p>
    {/if}
  </div>
{/if}
