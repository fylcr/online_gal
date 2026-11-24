<script lang="ts">
  /**
   * The account and project at the top of /admin. The section list below it is static; each
   * section page runs the same check again before it shows anything.
   */
  import { onMount } from 'svelte';
  import { checkAdmin, importAdminEntry, type AdminGate } from '../../lib/admin';
  import AdminDenied from './AdminDenied.svelte';

  let gate = $state<AdminGate | null>(null);
  let checking = false;
  let checkAgain = false;
  let mounted = false;

  async function check(): Promise<void> {
    if (checking) {
      checkAgain = true;
      return;
    }
    checking = true;
    gate = null;
    try {
      gate = await importAdminEntry() ?? await checkAdmin();
    } finally {
      checking = false;
      if (mounted && checkAgain) {
        checkAgain = false;
        void check();
      }
    }
  }

  onMount(() => {
    mounted = true;
    const reevaluate = () => { void check(); };
    navigator.serviceWorker?.addEventListener('controllerchange', reevaluate);
    window.addEventListener('hashchange', reevaluate);
    reevaluate();
    return () => {
      mounted = false;
      navigator.serviceWorker?.removeEventListener('controllerchange', reevaluate);
      window.removeEventListener('hashchange', reevaluate);
    };
  });
</script>

{#if gate === null}
  <div class="flex items-center gap-3 rounded-2xl bg-base-200 px-6 py-5 text-sm text-base-content/60">
    <span class="loading loading-spinner loading-sm"></span>
    正在确认管理员身份
  </div>
{:else if gate.kind === 'denied'}
  <AdminDenied {gate} onretry={check} />
{:else}
  <div class="rounded-2xl bg-base-200 px-6 py-5">
    <div class="flex items-start justify-between gap-4">
      <div class="min-w-0">
        <p class="font-bold">{gate.admin.projectName}</p>
        <p class="mt-1 truncate text-sm text-base-content/60">
          {gate.user?.email || gate.user?.id || '已登录'} · 账号 {gate.admin.accountName}
        </p>
      </div>
      <a class="btn btn-sm" href="/settings/account">账号设置</a>
    </div>
  </div>
{/if}
