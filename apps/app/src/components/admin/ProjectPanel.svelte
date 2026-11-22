<script lang="ts">
  /** /admin/project: the site's Pages project, where it builds from, and what is live now. */
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
    type PagesProject,
  } from '../../lib/admin';

  interface Props {
    admin: SiteAdmin;
  }

  let { admin }: Props = $props();
  let project = $state<PagesProject | null>(null);
  let error = $state<string | null>(null);
  let loading = $state(false);

  async function load(): Promise<void> {
    loading = true;
    error = null;
    try {
      project = (await cfApi<PagesProject>(projectPath(admin))).result;
    } catch (cause) {
      error = errorText(cause);
    } finally {
      loading = false;
    }
  }

  onMount(() => {
    void load();
  });

  const PREVIEW_SETTING: Record<string, string> = { all: '所有分支', none: '不部署', custom: '指定分支' };
</script>

<div class="space-y-4">
  {#if error !== null}
    <div class="rounded-2xl bg-base-200 px-6 py-5">
      <p class="font-bold text-error">读取项目失败</p>
      <p class="mt-2 text-sm break-all text-base-content/60">{error}</p>
      <button type="button" class="btn btn-sm mt-4" disabled={loading} onclick={load}>重试</button>
    </div>
  {:else if project === null}
    <div class="flex items-center gap-3 rounded-2xl bg-base-200 px-6 py-5 text-sm text-base-content/60">
      <span class="loading loading-spinner loading-sm"></span>
      正在读取项目
    </div>
  {:else}
    {@const source = project.source?.config}
    {@const build = project.build_config}
    {@const live = project.canonical_deployment ?? null}

    <div class="rounded-2xl bg-base-200 px-6 py-5">
      <div class="flex items-start justify-between gap-4">
        <div class="min-w-0">
          <p class="font-bold">{project.name}</p>
          <p class="mt-1 truncate text-sm text-base-content/60">账号 {admin.accountName}</p>
        </div>
        <button type="button" class="btn btn-sm" disabled={loading} onclick={load}>
          {#if loading}<span class="loading loading-spinner loading-xs"></span>{/if}
          刷新
        </button>
      </div>
      <dl class="mt-4 grid grid-cols-[auto_1fr] gap-x-6 gap-y-2 text-sm">
        <dt class="text-base-content/60">账号 ID</dt>
        <dd class="font-mono break-all">{admin.accountId}</dd>
        {#if project.subdomain}
          <dt class="text-base-content/60">默认域名</dt>
          <dd class="min-w-0">
            <a class="link font-mono break-all" href={`https://${project.subdomain}`} target="_blank" rel="noopener noreferrer">{project.subdomain}</a>
          </dd>
        {/if}
        <dt class="text-base-content/60">创建于</dt>
        <dd>{when(project.created_on)}</dd>
      </dl>
    </div>

    <div class="rounded-2xl bg-base-200 px-6 py-5">
      <p class="font-bold">当前生产部署</p>
      {#if live}
        {@const state = deploymentState(live)}
        {@const trigger = live.deployment_trigger?.metadata}
        <div class="mt-3 flex items-start gap-4">
          <div class="min-w-0 flex-1">
            <p class="truncate text-sm">{deploymentTitle(live)}</p>
            <p class="mt-1 text-sm text-base-content/60">
              {environmentLabel(live.environment)}{#if trigger?.branch} · {trigger.branch}{/if}{#if trigger?.commit_hash} · <span class="font-mono">{trigger.commit_hash.slice(0, 7)}</span>{/if} · {when(live.created_on)}
            </p>
          </div>
          <span class="flex shrink-0 items-center gap-2 text-sm">
            <span class={['size-2 rounded-full', TONE_CLASS[state.tone]]} aria-hidden="true"></span>
            {state.label}
          </span>
        </div>
      {:else}
        <p class="mt-2 text-sm text-base-content/60">还没有生产部署。</p>
      {/if}
      <a class="btn btn-sm mt-4" href="/admin/deployments">全部部署</a>
    </div>

    <div class="rounded-2xl bg-base-200 px-6 py-5">
      <p class="font-bold">来源</p>
      <dl class="mt-4 grid grid-cols-[auto_1fr] gap-x-6 gap-y-2 text-sm">
        <dt class="text-base-content/60">仓库</dt>
        <dd class="min-w-0 font-mono break-all">
          {#if source?.owner && source.repo_name}
            {#if project.source?.type === 'github'}
              <a class="link" href={`https://github.com/${source.owner}/${source.repo_name}`} target="_blank" rel="noopener noreferrer">{source.owner}/{source.repo_name}</a>
            {:else}
              {project.source?.type} · {source.owner}/{source.repo_name}
            {/if}
          {:else}
            直接上传
          {/if}
        </dd>
        <dt class="text-base-content/60">生产分支</dt>
        <dd class="font-mono">{project.production_branch ?? '—'}</dd>
        {#if source}
          <dt class="text-base-content/60">自动部署生产</dt>
          <dd>{source.production_deployments_enabled === false ? '关闭' : '开启'}</dd>
          <dt class="text-base-content/60">预览部署</dt>
          <dd>{PREVIEW_SETTING[source.preview_deployment_setting ?? ''] ?? source.preview_deployment_setting ?? '—'}</dd>
        {/if}
      </dl>
    </div>

    <div class="rounded-2xl bg-base-200 px-6 py-5">
      <p class="font-bold">构建</p>
      <dl class="mt-4 grid grid-cols-[auto_1fr] gap-x-6 gap-y-2 text-sm">
        <dt class="text-base-content/60">构建命令</dt>
        <dd class="font-mono break-all">{build?.build_command || '—'}</dd>
        <dt class="text-base-content/60">输出目录</dt>
        <dd class="font-mono break-all">{build?.destination_dir || '—'}</dd>
        <dt class="text-base-content/60">根目录</dt>
        <dd class="font-mono break-all">{build?.root_dir || '/'}</dd>
      </dl>
    </div>
  {/if}
</div>
