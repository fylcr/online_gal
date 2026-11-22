<script lang="ts">
  /** Why the visitor cannot manage the site, with the way out (settings) and a retry where useful. */
  import type { AdminGate } from '../../lib/admin';

  interface Props {
    gate: Extract<AdminGate, { kind: 'denied' }>;
    onretry: () => void;
  }

  let { gate, onretry }: Props = $props();
</script>

<div class="rounded-2xl bg-base-200 px-6 py-5">
  <p class="font-bold">{gate.title}</p>
  <p class="mt-2 text-sm break-all text-base-content/60">{gate.detail}</p>
  <div class="mt-4 flex gap-2">
    <a class="btn btn-primary btn-sm" href="/settings/account">账号设置</a>
    {#if gate.retry}
      <button type="button" class="btn btn-sm" onclick={onretry}>重试</button>
    {/if}
  </div>
</div>
