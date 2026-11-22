<script lang="ts">
  /**
   * One /admin section page: the admin check, then that section's panel
   * (openspec/changes/cf-account-admin, design.md A9).
   */
  import { onMount } from 'svelte';
  import { checkAdmin, type AdminGate } from '../../lib/admin';
  import type { AdminSectionId } from '../../lib/admin-sections';
  import AdminDenied from './AdminDenied.svelte';
  import CfConsole from './CfConsole.svelte';
  import DeploymentsPanel from './DeploymentsPanel.svelte';
  import DomainsPanel from './DomainsPanel.svelte';
  import ProjectPanel from './ProjectPanel.svelte';

  interface Props {
    section: AdminSectionId;
  }

  let { section }: Props = $props();
  let gate = $state<AdminGate | null>(null);

  async function check(): Promise<void> {
    gate = null;
    gate = await checkAdmin();
  }

  onMount(() => {
    void check();
  });
</script>

{#if gate === null}
  <div class="flex items-center gap-3 rounded-2xl bg-base-200 px-6 py-5 text-sm text-base-content/60">
    <span class="loading loading-spinner loading-sm"></span>
    正在确认管理员身份
  </div>
{:else if gate.kind === 'denied'}
  <AdminDenied {gate} onretry={check} />
{:else if section === 'project'}
  <ProjectPanel admin={gate.admin} />
{:else if section === 'deployments'}
  <DeploymentsPanel admin={gate.admin} />
{:else if section === 'domains'}
  <DomainsPanel admin={gate.admin} />
{:else}
  <CfConsole />
{/if}
