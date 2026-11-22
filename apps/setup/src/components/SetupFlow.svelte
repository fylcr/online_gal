<script lang="ts">
  import { onMount } from 'svelte';
  import { CloudflareAccount, Vault, type CloudflareStatus, type SiteAdmin } from '@online-gal/plugin-system/account';
  import DeviceLogin from './DeviceLogin.svelte';
  import { MemoryVaultStore } from '../lib/memory-vault';
  import { SETUP_ITEMS, type SetupItem } from '../lib/setup-items';
  import {
    buildEnvPatch, buildSiteRecord, clearSession, clearSetupSession, isLocalHost, RELOAD_KEY,
    readSession, safeNext, savedDeployment, SetupClient, type SetupConfig, type SetupVerification,
    writeSession, WRITTEN_KEY,
  } from '../lib/setup';

  let { config }: { config: SetupConfig } = $props();
  type SignedIn = Extract<CloudflareStatus, { signedIn: true }>;
  type Step = 'status' | 'status-error' | 'stale' | 'login' | 'building' | 'build-error' | 'confirming'
    | 'confirmation-error' | 'local-preview' | 'complete';
  type ProjectState = 'idle' | 'checking' | 'form' | 'writing' | 'error';
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
  let githubRerunError = $state('');
  let admin = $state.raw<SiteAdmin | null>(null);
  let productionBranch = '';
  let values = $state<Record<string, string>>({});
  let verification = $state<Record<string, SetupVerification | 'pending' | undefined>>({});
  let allValid = $derived(SETUP_ITEMS.every((item) => item.source === 'account'
    ? /^[0-9a-f]{32}$/.test(admin?.accountId ?? '')
    : values[item.name]?.trim() && verification[item.name] !== 'pending' && verification[item.name]?.ok));
  let inspection = 0;
  let redirectTimer: ReturnType<typeof setTimeout> | undefined;
  const verificationRuns: Record<string, number> = {};

  function clearInputs(): void {
    values = {};
    verification = {};
    for (const key of Object.keys(verificationRuns)) verificationRuns[key] = (verificationRuns[key] ?? 0) + 1;
  }

  function complete(): void {
    clearInputs();
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

  async function rebuild(owner: SiteAdmin, githubToken = '', restart = false): Promise<void> {
    if (isLocalHost(host)) {
      clearInputs();
      step = 'local-preview';
      return;
    }
    step = 'building';
    errorMessage = '';
    manualBuild = false;
    githubRerunError = '';
    const mode = config.rebuild;
    try {
      let id = restart ? null : savedDeployment(owner);
      if (mode.kind === 'pages-git') {
        id ??= await client.startPagesBuild(owner, productionBranch, lifetime.signal);
        if (!id) throw new Error('未能开始构建。');
        await client.waitForDeployment(owner, id, lifetime.signal);
      } else {
        manualBuild = true;
        if (mode.kind === 'github-rerun' && githubToken) {
          const result = await client.rerunGitHub(mode, githubToken, lifetime.signal);
          manualBuild = !result.started;
          if (!result.started) {
            githubRerunError = result.httpStatus
              ? `GitHub 未接受重跑请求（HTTP ${result.httpStatus}）。`
              : '无法连接 GitHub 重跑工作流。';
          }
        }
        githubToken = '';
        await client.waitForNewProduction(owner, readSession(WRITTEN_KEY) ?? config.builtAt, lifetime.signal);
      }
      await checkSwitch();
    } catch (error) {
      if (lifetime.signal.aborted) return;
      errorMessage = error instanceof Error ? error.message : '构建请求失败，请重试。';
      step = 'build-error';
    } finally {
      githubToken = '';
    }
  }

  async function inspectOwner(status: CloudflareStatus): Promise<void> {
    const run = ++inspection;
    clearInputs();
    project = 'idle';
    admin = status.signedIn ? status.siteAdmin : null;
    if (!admin || step !== 'login') return;
    const owner = admin;
    project = 'checking';
    errorMessage = '';
    try {
      const state = await client.readProject(owner, lifetime.signal);
      if (lifetime.signal.aborted || run !== inspection || step !== 'login') return;
      productionBranch = state.branch ?? '';
      if (state.written || initialized) {
        if (state.missing.length) throw new Error('项目缺少初始化配置，请在 Cloudflare 控制台检查。');
        await rebuild(owner);
      } else {
        project = 'form';
      }
    } catch (error) {
      if (lifetime.signal.aborted || run !== inspection) return;
      errorMessage = error instanceof Error ? error.message : '请重试。';
      project = 'error';
    }
  }

  function changeValue(item: SetupItem, value: string): void {
    values[item.name] = value;
    verification[item.name] = undefined;
    verificationRuns[item.name] = (verificationRuns[item.name] ?? 0) + 1;
  }

  async function checkItem(item: SetupItem): Promise<void> {
    const value = values[item.name];
    if (!admin || project !== 'form' || !value?.trim() || verification[item.name] !== undefined) return;
    const run = (verificationRuns[item.name] ?? 0) + 1;
    verificationRuns[item.name] = run;
    verification[item.name] = 'pending';
    let result: SetupVerification;
    try {
      result = await client.verifyItem(item, value, admin.accountId, lifetime.signal);
    } catch {
      result = { ok: false, reason: 'upstream' };
    }
    if (!lifetime.signal.aborted && verificationRuns[item.name] === run) verification[item.name] = result;
  }

  function recheckItem(item: SetupItem): void {
    verification[item.name] = undefined;
    void checkItem(item);
  }

  async function submit(status: SignedIn): Promise<void> {
    if (!admin || !allValid || project !== 'form') return;
    const owner = admin;
    project = 'writing';
    errorMessage = '';
    try {
      const result = await client.write(owner, buildEnvPatch(buildSiteRecord(owner, status.user), {
        ...values, CLOUDFLARE_ACCOUNT_ID: owner.accountId,
      }), lifetime.signal);
      if (lifetime.signal.aborted) return;
      const githubToken = result === 'written' && config.rebuild.kind === 'github-rerun' ? values.GITHUB_TOKEN : undefined;
      clearInputs();
      if (result === 'switched') complete();
      else if (result === 'already-written') await inspectOwner(status);
      else void rebuild(owner, githubToken);
    } catch (error) {
      if (lifetime.signal.aborted) return;
      errorMessage = error instanceof Error ? error.message : '请重试。';
      project = 'form';
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
      clearInputs();
      account = undefined;
      clearTimeout(redirectTimer);
    };
  });
</script>

{#if initialized && step === 'login'}
  <p class="mb-4 text-sm text-base-content/70">初始化已写入。请登录后继续构建正式站点。</p>
{/if}
{#if step === 'login' && account}
  <p class="mb-4 text-sm text-base-content/70">请使用拥有本站 Pages 项目的 Cloudflare 账号登录。</p>
  <DeviceLogin {account} onStatus={(status) => void inspectOwner(status)}>
    {#snippet signedIn(status, refresh, logout, busy)}
      <div class="rounded-2xl bg-base-200 px-6 py-5">
        {#if status.siteAdmin}
          <p class="font-bold">已确认站点所有者</p>
          <p class="mt-2 text-sm text-base-content/70">{status.siteAdmin.accountName} · <span class="font-mono">{status.siteAdmin.projectName}</span></p>
          {#if project === 'checking'}
            <p class="mt-4 text-sm" role="status">正在检查 Pages 项目配置</p>
          {:else if project === 'error'}
            <p class="mt-4 text-sm text-error" role="alert">{errorMessage}</p>
            <button class="btn btn-sm mt-4" type="button" onclick={() => void inspectOwner(status)}>重试</button>
          {:else if project === 'form' || project === 'writing'}
            <form class="mt-4 grid gap-4" autocomplete="off" onsubmit={(event) => { event.preventDefault(); void submit(status); }}>
              {#each SETUP_ITEMS as item (item.name)}
                <div class="grid gap-1 text-sm">
                  <label for={'setup-' + item.name}>{item.label}</label>
                  {#if item.source === 'account'}
                    <input id={'setup-' + item.name} class="input w-full font-mono" value={status.siteAdmin.accountId} readonly />
                  {:else}
                    {@const result = verification[item.name]}
                    <div class="flex gap-2">
                      <input
                        id={'setup-' + item.name} type="password" class="input min-w-0 flex-1"
                        value={values[item.name] ?? ''} autocomplete="off" autocapitalize="off" spellcheck={false}
                        disabled={project === 'writing' || busy} maxlength={512} required
                        aria-invalid={typeof result === 'object' && !result.ok}
                        oninput={(event) => changeValue(item, event.currentTarget.value)}
                        onblur={() => void checkItem(item)}
                      />
                      <button class="btn" type="button" disabled={project === 'writing' || busy || !values[item.name]?.trim() || result === 'pending'} onclick={() => recheckItem(item)}>校验</button>
                    </div>
                    <div aria-live="polite">
                      {#if result === 'pending'}<p>正在校验</p>
                      {:else if result?.ok}
                        <p class="text-success">校验通过{result.login ? ` · ${result.login}` : ''}{result.status ? ` · ${result.status}` : ''}{result.expiresAt || result.expiresOn ? ` · 到期 ${result.expiresAt ?? result.expiresOn}` : ''}</p>
                      {:else if result}
                        <p class="text-error">{result.reason === 'upstream' ? '暂时无法校验，请重试。' : '凭据无效，请检查后重试。'}</p>
                      {/if}
                    </div>
                  {/if}
                  <p class="text-base-content/70">{item.help} {#if item.verify === 'github' && config.rebuild.kind === 'github-rerun'}自动重新构建需要对 {config.rebuild.repo} 的 Actions 写权限。{/if} {#if item.helpUrl}<a class="link" href={item.helpUrl} target="_blank" rel="noopener noreferrer">创建 Token</a>{/if}</p>
                </div>
              {/each}
              {#if errorMessage}<p role="alert" class="text-sm text-error">{errorMessage}</p>{/if}
              <button class="btn btn-primary justify-self-start" disabled={!allValid || busy || project === 'writing'} type="submit">{project === 'writing' ? '正在写入' : '写入并构建'}</button>
            </form>
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
        <p class="mt-2 text-sm">{githubRerunError}请在 GitHub 重跑发布工作流，或发布新的版本标签。</p>
        <a class="btn btn-sm mt-4" href={'https://github.com/' + config.rebuild.repo + '/actions/runs/' + config.rebuild.runId} target="_blank" rel="noopener noreferrer">打开发布工作流</a>
      {:else if manualBuild}
        <p class="mt-2 text-sm">设置 Pages 项目查询所需的环境变量后，可运行 <code>npm run build:pages</code> 检查构建产物。</p>
        <p class="mt-2 text-sm">Git 集成项目可用以下命令触发生产构建：</p>
        <code class="mt-1 block break-all text-sm">cf pages deployments create {admin?.projectName ?? '&lt;project&gt;'} --branch {productionBranch || '&lt;production-branch&gt;'}</code>
        <p class="mt-2 text-sm">直接上传项目需要使用已配置的发布流程发布 <code>dist/site</code>。</p>
      {/if}
      {#if step === 'build-error' && admin}
        {@const owner = admin}
        <button type="button" class="btn btn-sm mt-4" onclick={() => void rebuild(owner)}>继续检查</button>
        {#if config.rebuild.kind === 'pages-git'}
          <button type="button" class="btn btn-sm mt-4" onclick={() => void rebuild(owner, '', true)}>重新构建</button>
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
