<script lang="ts">
  /**
   * /admin/deployments: recent deployments, newest first. A failed or cancelled deployment can be
   * retried, and production can be rolled back to an earlier successful production deployment.
   * Both ask for confirmation in the row first.
   */
  import { onMount } from 'svelte';
  import type { SiteAdmin } from '../../lib/account';
  import {
    cfApi,
    deploymentState,
    deploymentTitle,
    environmentLabel,
    errorText,
    projectPath,
    TONE_CLASS,
    when,
    type PagesDeployment,
    type PagesProject,
  } from '../../lib/admin';

  interface Props {
    admin: SiteAdmin;
  }

  type Filter = 'all' | 'production' | 'preview';
  type Action = 'retry' | 'rollback';

  const PER_PAGE = 20;
  const FILTERS: ReadonlyArray<{ id: Filter; label: string }> = [
    { id: 'all', label: '全部' },
    { id: 'production', label: '生产' },
    { id: 'preview', label: '预览' },
  ];
  const QUESTION: Record<Action, string> = {
    retry: '重新运行这个部署？',
    rollback: '把生产环境回滚到这个部署？',
  };

  let { admin }: Props = $props();
  let filter = $state<Filter>('all');
  let deployments = $state<PagesDeployment[]>([]);
  let liveId = $state<string | null>(null);
  let page = $state(0);
  let pages = $state(1);
  let loading = $state(false);
  let error = $state<string | null>(null);
  let confirming = $state<{ id: string; action: Action } | null>(null);
  let working = $state<string | null>(null);
  let rowError = $state<{ id: string; message: string } | null>(null);

  async function fetchPage(next: number): Promise<void> {
    const query = new URLSearchParams({ page: String(next), per_page: String(PER_PAGE) });
    if (filter !== 'all') query.set('env', filter);
    const { result, info } = await cfApi<PagesDeployment[]>(`${projectPath(admin)}/deployments?${query}`);
    deployments = next === 1 ? result : [...deployments, ...result];
    page = next;
    pages = info?.total_pages ?? (result.length < PER_PAGE ? next : next + 1);
  }

  async function load(): Promise<void> {
    loading = true;
    error = null;
    confirming = null;
    rowError = null;
    try {
      const [project] = await Promise.all([cfApi<PagesProject>(projectPath(admin)), fetchPage(1)]);
      liveId = project.result.canonical_deployment?.id ?? null;
    } catch (cause) {
      error = errorText(cause);
    } finally {
      loading = false;
    }
  }

  async function more(): Promise<void> {
    loading = true;
    try {
      await fetchPage(page + 1);
    } catch (cause) {
      error = errorText(cause);
    } finally {
      loading = false;
    }
  }

  function choose(next: Filter): void {
    if (next === filter) return;
    filter = next;
    deployments = [];
    void load();
  }

  async function run(deployment: PagesDeployment, action: Action): Promise<void> {
    confirming = null;
    rowError = null;
    working = deployment.id;
    try {
      await cfApi(`${projectPath(admin)}/deployments/${encodeURIComponent(deployment.id)}/${action}`, { method: 'POST' });
      await load();
    } catch (cause) {
      rowError = { id: deployment.id, message: errorText(cause) };
    } finally {
      working = null;
    }
  }

  const canRetry = (d: PagesDeployment) => d.latest_stage?.status === 'failure' || d.latest_stage?.status === 'canceled';
  const canRollback = (d: PagesDeployment) =>
    d.environment === 'production' &&
    d.id !== liveId &&
    d.latest_stage?.name === 'deploy' &&
    d.latest_stage.status === 'success';

  onMount(() => {
    void load();
  });
</script>

<div class="space-y-4">
  <div class="flex items-center justify-between gap-4">
    <div class="flex gap-1" role="group" aria-label="环境">
      {#each FILTERS as item}
        <button
          type="button"
          class={['btn btn-sm', filter === item.id ? 'btn-neutral' : 'btn-ghost']}
          aria-pressed={filter === item.id}
          onclick={() => choose(item.id)}
        >
          {item.label}
        </button>
      {/each}
    </div>
    <button type="button" class="btn btn-sm" disabled={loading} onclick={load}>
      {#if loading}<span class="loading loading-spinner loading-xs"></span>{/if}
      刷新
    </button>
  </div>

  {#if error !== null}
    <div class="rounded-2xl bg-base-200 px-6 py-5">
      <p class="font-bold text-error">读取部署失败</p>
      <p class="mt-2 text-sm break-all text-base-content/60">{error}</p>
      <button type="button" class="btn btn-sm mt-4" disabled={loading} onclick={load}>重试</button>
    </div>
  {:else if deployments.length === 0}
    <div class="flex items-center gap-3 rounded-2xl bg-base-200 px-6 py-5 text-sm text-base-content/60">
      {#if loading}
        <span class="loading loading-spinner loading-sm"></span>
        正在读取部署
      {:else}
        没有部署。
      {/if}
    </div>
  {:else}
    <ul class="divide-y divide-base-300 overflow-hidden rounded-2xl bg-base-200">
      {#each deployments as deployment (deployment.id)}
        {@const state = deploymentState(deployment)}
        {@const trigger = deployment.deployment_trigger?.metadata}
        <li class="px-6 py-4">
          <div class="flex items-start gap-4">
            <div class="min-w-0 flex-1">
              <p class="truncate font-bold">{deploymentTitle(deployment)}</p>
              <p class="mt-1 text-sm text-base-content/60">
                {environmentLabel(deployment.environment)}{#if trigger?.branch} · {trigger.branch}{/if}{#if trigger?.commit_hash} · <span class="font-mono">{trigger.commit_hash.slice(0, 7)}</span>{/if} · {when(deployment.created_on)}
              </p>
              {#if deployment.url}
                <a class="link mt-1 block truncate font-mono text-xs text-base-content/60" href={deployment.url} target="_blank" rel="noopener noreferrer">{deployment.url}</a>
              {/if}
            </div>
            <div class="flex shrink-0 flex-col items-end gap-2">
              <span class="flex items-center gap-2 text-sm">
                <span class={['size-2 rounded-full', TONE_CLASS[state.tone]]} aria-hidden="true"></span>
                {state.label}
              </span>
              {#if deployment.id === liveId}
                <span class="text-xs text-base-content/50">当前生产</span>
              {/if}
              {#if working === deployment.id}
                <span class="loading loading-spinner loading-xs"></span>
              {:else if confirming?.id !== deployment.id}
                <div class="flex gap-1">
                  {#if canRetry(deployment)}
                    <button type="button" class="btn btn-ghost btn-xs" disabled={working !== null} onclick={() => (confirming = { id: deployment.id, action: 'retry' })}>重试</button>
                  {/if}
                  {#if canRollback(deployment)}
                    <button type="button" class="btn btn-ghost btn-xs" disabled={working !== null} onclick={() => (confirming = { id: deployment.id, action: 'rollback' })}>回滚到此</button>
                  {/if}
                </div>
              {/if}
            </div>
          </div>
          {#if confirming?.id === deployment.id}
            {@const action = confirming.action}
            <div class="mt-3 flex flex-wrap items-center gap-2 text-sm">
              <span>{QUESTION[action]}</span>
              <button type="button" class="btn btn-error btn-xs" onclick={() => run(deployment, action)}>确认</button>
              <button type="button" class="btn btn-xs" onclick={() => (confirming = null)}>取消</button>
            </div>
          {/if}
          {#if rowError?.id === deployment.id}
            <p class="mt-2 text-sm break-all text-error">{rowError.message}</p>
          {/if}
        </li>
      {/each}
    </ul>
    {#if page < pages}
      <button type="button" class="btn btn-sm w-full" disabled={loading} onclick={more}>
        {#if loading}<span class="loading loading-spinner loading-xs"></span>{/if}
        加载更多
      </button>
    {/if}
  {/if}
</div>
