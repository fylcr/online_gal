<script lang="ts">
  /**
   * /admin/domains: the project's custom domains and their verification state. Domains can be
   * added, re-checked and removed; removing asks first, and warns when it is the domain this page
   * was opened on.
   */
  import { onMount } from 'svelte';
  import type { SiteAdmin } from '../../lib/account';
  import {
    cfApi,
    domainState,
    errorText,
    projectPath,
    TONE_CLASS,
    when,
    type PagesDomain,
    type PagesProject,
  } from '../../lib/admin';

  interface Props {
    admin: SiteAdmin;
  }

  let { admin }: Props = $props();
  let domains = $state<PagesDomain[] | null>(null);
  let subdomain = $state<string | null>(null);
  let loading = $state(false);
  let error = $state<string | null>(null);
  let confirming = $state<string | null>(null);
  let working = $state<string | null>(null);
  let rowError = $state<{ name: string; message: string } | null>(null);
  let draft = $state('');
  let adding = $state(false);
  let addError = $state<string | null>(null);

  const here = typeof location === 'undefined' ? '' : location.hostname;
  const domainsPath = () => `${projectPath(admin)}/domains`;
  const domainPath = (name: string) => `${domainsPath()}/${encodeURIComponent(name)}`;

  async function load(): Promise<void> {
    loading = true;
    error = null;
    confirming = null;
    rowError = null;
    try {
      const [project, list] = await Promise.all([
        cfApi<PagesProject>(projectPath(admin)),
        cfApi<PagesDomain[]>(domainsPath()),
      ]);
      subdomain = project.result.subdomain ?? null;
      domains = list.result;
    } catch (cause) {
      error = errorText(cause);
    } finally {
      loading = false;
    }
  }

  async function act(name: string, method: 'PATCH' | 'DELETE'): Promise<void> {
    confirming = null;
    rowError = null;
    working = name;
    try {
      await cfApi(domainPath(name), { method });
      await load();
    } catch (cause) {
      rowError = { name, message: errorText(cause) };
    } finally {
      working = null;
    }
  }

  async function add(event: SubmitEvent): Promise<void> {
    event.preventDefault();
    const name = draft.trim().toLowerCase().replace(/\.$/, '');
    addError = null;
    if (!/^(?=.{1,253}$)(?:[a-z0-9](?:[a-z0-9-]{0,61}[a-z0-9])?\.)+[a-z0-9-]{2,63}$/.test(name)) {
      addError = '请输入域名，例如 example.com';
      return;
    }
    adding = true;
    try {
      await cfApi(domainsPath(), { method: 'POST', body: { name } });
      draft = '';
      await load();
    } catch (cause) {
      addError = errorText(cause);
    } finally {
      adding = false;
    }
  }

  const problem = (domain: PagesDomain): string | undefined =>
    domain.verification_data?.error_message || domain.validation_data?.error_message || undefined;

  onMount(() => {
    void load();
  });
</script>

<div class="space-y-4">
  {#if error !== null}
    <div class="rounded-2xl bg-base-200 px-6 py-5">
      <p class="font-bold text-error">读取域名失败</p>
      <p class="mt-2 text-sm break-all text-base-content/60">{error}</p>
      <button type="button" class="btn btn-sm mt-4" disabled={loading} onclick={load}>重试</button>
    </div>
  {:else if domains === null}
    <div class="flex items-center gap-3 rounded-2xl bg-base-200 px-6 py-5 text-sm text-base-content/60">
      <span class="loading loading-spinner loading-sm"></span>
      正在读取域名
    </div>
  {:else}
    <ul class="divide-y divide-base-300 overflow-hidden rounded-2xl bg-base-200">
      {#if subdomain}
        <li class="flex items-start gap-4 px-6 py-4">
          <div class="min-w-0 flex-1">
            <a class="link font-mono font-bold break-all" href={`https://${subdomain}`} target="_blank" rel="noopener noreferrer">{subdomain}</a>
            <p class="mt-1 text-sm text-base-content/60">Pages 默认域名{#if subdomain === here} · 当前站点{/if}</p>
          </div>
          <span class="flex shrink-0 items-center gap-2 text-sm">
            <span class={['size-2 rounded-full', TONE_CLASS.success]} aria-hidden="true"></span>
            已生效
          </span>
        </li>
      {/if}
      {#each domains as domain (domain.name)}
        {@const state = domainState(domain)}
        {@const issue = problem(domain)}
        <li class="px-6 py-4">
          <div class="flex items-start gap-4">
            <div class="min-w-0 flex-1">
              <a class="link font-mono font-bold break-all" href={`https://${domain.name}`} target="_blank" rel="noopener noreferrer">{domain.name}</a>
              <p class="mt-1 text-sm text-base-content/60">
                添加于 {when(domain.created_on)}{#if domain.name === here} · 当前站点{/if}
              </p>
              {#if issue}
                <p class="mt-1 text-sm break-all text-warning">{issue}</p>
              {/if}
            </div>
            <div class="flex shrink-0 flex-col items-end gap-2">
              <span class="flex items-center gap-2 text-sm">
                <span class={['size-2 rounded-full', TONE_CLASS[state.tone]]} aria-hidden="true"></span>
                {state.label}
              </span>
              {#if working === domain.name}
                <span class="loading loading-spinner loading-xs"></span>
              {:else if confirming !== domain.name}
                <div class="flex gap-1">
                  {#if domain.status !== 'active'}
                    <button type="button" class="btn btn-ghost btn-xs" disabled={working !== null} onclick={() => act(domain.name, 'PATCH')}>重新验证</button>
                  {/if}
                  <button type="button" class="btn btn-ghost btn-xs" disabled={working !== null} onclick={() => (confirming = domain.name)}>移除</button>
                </div>
              {/if}
            </div>
          </div>
          {#if confirming === domain.name}
            <div class="mt-3 flex flex-wrap items-center gap-2 text-sm">
              <span>
                {domain.name === here ? '这是你现在访问的域名，移除后本站将不能通过它访问。确认移除？' : `从项目中移除 ${domain.name}？`}
              </span>
              <button type="button" class="btn btn-error btn-xs" onclick={() => act(domain.name, 'DELETE')}>确认</button>
              <button type="button" class="btn btn-xs" onclick={() => (confirming = null)}>取消</button>
            </div>
          {/if}
          {#if rowError?.name === domain.name}
            <p class="mt-2 text-sm break-all text-error">{rowError.message}</p>
          {/if}
        </li>
      {/each}
    </ul>

    <form class="rounded-2xl bg-base-200 px-6 py-5" onsubmit={add}>
      <p class="font-bold">添加域名</p>
      <p class="mt-2 text-sm text-base-content/60">
        添加后按 Cloudflare 的提示配置 DNS{#if subdomain}（CNAME 指向 <span class="font-mono">{subdomain}</span>）{/if}，验证通过后生效。
      </p>
      <div class="mt-4 flex gap-2">
        <input
          class="input input-sm flex-1 font-mono"
          bind:value={draft}
          placeholder="example.com"
          autocomplete="off"
          autocapitalize="off"
          spellcheck="false"
          aria-label="域名"
        />
        <button type="submit" class="btn btn-primary btn-sm" disabled={adding}>
          {#if adding}<span class="loading loading-spinner loading-xs"></span>{/if}
          添加
        </button>
      </div>
      {#if addError}
        <p class="mt-2 text-sm break-all text-error">{addError}</p>
      {/if}
    </form>
  {/if}
</div>
