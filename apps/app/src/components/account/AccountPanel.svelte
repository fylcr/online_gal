<script lang="ts">
  import DeviceLogin from './DeviceLogin.svelte';

  function clock(at: number): string {
    return new Date(at).toLocaleString('zh-CN', { hour12: false });
  }
</script>

<DeviceLogin>
  {#snippet signedIn(status, refresh, logout, busy)}
    <div class="rounded-2xl bg-base-200 px-6 py-5">
      <div class="flex items-start justify-between gap-4">
        <div class="min-w-0">
          <p class="font-bold">Cloudflare</p>
          <p class="mt-1 truncate text-sm">{status.user?.email || status.user?.id || '已登录'}</p>
        </div>
        <button type="button" class="btn btn-sm" disabled={busy} onclick={logout}>
          {#if busy}<span class="loading loading-spinner loading-xs"></span>{/if}
          退出
        </button>
      </div>
      <dl class="mt-4 grid grid-cols-[auto_1fr] gap-x-6 gap-y-2 text-sm">
        <dt class="text-base-content/60">权限</dt>
        <dd>{status.scopes.length} 项</dd>
        <dt class="text-base-content/60">访问令牌到期</dt>
        <dd>{clock(status.expiresAt)}（到期前自动续期）</dd>
      </dl>
    </div>

    <div class="rounded-2xl bg-base-200 px-6 py-5">
      <p class="font-bold">站点管理</p>
      {#if status.siteAdmin}
        <p class="mt-2 text-sm text-base-content/60">
          此账号拥有本站的 Pages 项目 <span class="font-mono">{status.siteAdmin.projectName}</span>（账号 {status.siteAdmin.accountName}）。
        </p>
        <a class="btn btn-primary btn-sm mt-4" href="/admin">进入管理页</a>
      {:else if status.siteAdminError}
        <p class="mt-2 text-sm text-warning break-all">无法确认是否为本站管理员：{status.siteAdminError}</p>
        <button type="button" class="btn btn-sm mt-4" onclick={refresh}>重试</button>
      {:else}
        <p class="mt-2 text-sm text-base-content/60">此账号下没有提供本站的 Pages 项目，不能进入管理页。</p>
      {/if}
    </div>
  {/snippet}
</DeviceLogin>
