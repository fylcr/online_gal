<script lang="ts">
  import { onMount, tick } from 'svelte';
  import GameCard from './GameCard.svelte';
  import { getPlatform, playBlockReason, type GameRecord } from '../../lib/platform';

  let games = $state<readonly GameRecord[] | null>(null);
  let loadError = $state('');
  let selected = $state<GameRecord | null>(null);
  let menu = $state<{ game: GameRecord; x: number; y: number } | null>(null);
  let menuButton = $state<HTMLButtonElement>();
  let deleteDialog: HTMLDialogElement;
  let actionOrigin: HTMLElement | null = null;

  onMount(() => {
    void reload();
  });

  async function reload(): Promise<void> {
    loadError = '';
    try {
      games = await getPlatform().library.list();
    } catch (error) {
      loadError = `无法读取游戏库：${String(error)}`;
    }
  }

  // Right-click (and later long-press) opens the card menu. The menu itself is
  // kept as-is for now; its redesign is deferred.
  async function openMenu(game: GameRecord, event: MouseEvent): Promise<void> {
    event.preventDefault();
    actionOrigin = (event.currentTarget as HTMLElement).querySelector('button');
    menu = { game, x: event.clientX, y: event.clientY };
    await tick();
    if (!menu || !menuButton) return;
    const { width, height } = menuButton.getBoundingClientRect();
    menu.x = Math.max(8, Math.min(menu.x, window.innerWidth - width - 8));
    menu.y = Math.max(8, Math.min(menu.y, window.innerHeight - height - 8));
    menuButton.focus({ preventScroll: true });
  }

  function closeMenu(event?: Event): void {
    if (!menu || (event?.type === 'pointerdown' && menuButton?.contains(event.target as Node))) return;
    menu = null;
  }

  function onKeydown(event: KeyboardEvent): void {
    if (!menu) return;
    if (event.key === 'Escape') {
      event.preventDefault();
      closeMenu();
      actionOrigin?.focus({ preventScroll: true });
    } else if (event.key === 'Tab') {
      closeMenu();
    }
  }

  function confirmDelete(game: GameRecord): void {
    menu = null;
    selected = game;
    deleteDialog.showModal();
  }
</script>

<svelte:window onpageshow={(event) => { if (event.persisted) void reload(); }} onpointerdown={closeMenu} onkeydown={onKeydown} onresize={closeMenu} onscroll={closeMenu} />

<section aria-label="本地游戏" class="@container">
  {#if loadError}
    <p role="alert" class="alert alert-error mb-4">{loadError}</p>
    <button class="btn" type="button" onclick={reload}>重试</button>
  {:else if games === null}
    <p class="text-base-content/65" role="status">正在读取游戏库…</p>
  {:else if games.length === 0}
    <div class="grid min-h-[50vh] place-items-center text-center">
      <div>
        <h2 class="text-xl font-semibold">暂无游戏</h2>
        <p class="mt-2 text-base-content/60">游戏库为空</p>
      </div>
    </div>
  {:else}
    <ul class="grid grid-cols-2 gap-3 @lg:grid-cols-3 @2xl:grid-cols-4 @4xl:grid-cols-5 @5xl:grid-cols-6 @7xl:grid-cols-7">
      {#each games as game (game.id)}
        <GameCard {game} blocked={playBlockReason(game)} onMenu={(event) => openMenu(game, event)} />
      {/each}
    </ul>
  {/if}
</section>

{#if menu}
  <button bind:this={menuButton} type="button" data-card-context-action class="btn btn-sm fixed z-40 shadow-lg" style:left="{menu.x}px" style:top="{menu.y}px" onclick={() => menu && confirmDelete(menu.game)}>
    从游戏库删除
  </button>
{/if}

<dialog bind:this={deleteDialog} class="modal" aria-label="从游戏库删除">
  <div class="modal-box">
    <h2 class="text-lg font-bold">从游戏库删除</h2>
    <p class="py-4 text-base-content/70">弹窗待重新设计{selected ? `：「${selected.title || selected.name}」` : ''}。</p>
    <div class="modal-action">
      <form method="dialog"><button class="btn">关闭</button></form>
    </div>
  </div>
  <form method="dialog" class="modal-backdrop"><button aria-label="关闭">关闭</button></form>
</dialog>
