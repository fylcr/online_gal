<script lang="ts">
  import { onMount, tick } from 'svelte';
  import GameCard from './GameCard.svelte';
  import { getPlatform, pickGameDirectory, playBlockReason, type GameRecord } from '../../lib/platform';

  let games = $state<readonly GameRecord[] | null>(null);
  let loadError = $state('');
  let notice = $state('');
  let importError = $state('');
  let deleteError = $state('');
  let importing = $state(false);
  let deleting = $state(false);
  let selected = $state<GameRecord | null>(null);
  let menu = $state<{ game: GameRecord; x: number; y: number } | null>(null);
  let menuButton: HTMLButtonElement;
  let addButton: HTMLButtonElement;
  let folderInput: HTMLInputElement;
  let importDialog: HTMLDialogElement;
  let detailsDialog: HTMLDialogElement;
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

  function openImport(): void {
    importError = '';
    importDialog.showModal();
  }

  async function importFolder(files?: readonly File[]): Promise<void> {
    if (importing) return;
    notice = '';
    importError = '';
    importing = true;
    try {
      const platform = getPlatform();
      const selection = files ? null : pickGameDirectory();
      if (!files && !selection) {
        folderInput.click();
        return;
      }
      const { record, replaced } = selection
        ? await platform.importDirectory(await selection)
        : await platform.importFileList(files ?? []);
      const { runtimeName, confidence } = record.detection;
      const detection = runtimeName ? `识别为 ${runtimeName}${confidence === 'low' ? '（低置信度）' : ''}` : '未识别引擎';
      const blocked = runtimeName ? playBlockReason(record) : null;
      notice = `${replaced ? '已更新' : '已添加'}「${record.title || record.name}」。${detection}${blocked ? ` · ${blocked}` : ''}。`;
      await reload();
      importDialog.close();
    } catch (error) {
      if (!(error instanceof DOMException && error.name === 'AbortError')) {
        importError = `导入失败：${String(error)}`;
      }
    } finally {
      importing = false;
    }
  }

  function onFiles(event: Event): void {
    const input = event.currentTarget as HTMLInputElement;
    const files = Array.from(input.files ?? []);
    input.value = '';
    if (files.length) void importFolder(files);
  }

  async function openDetails(game: GameRecord, event: MouseEvent): Promise<void> {
    actionOrigin = event.currentTarget as HTMLElement;
    selected = game;
    await tick();
    detailsDialog.showModal();
  }

  async function openMenu(game: GameRecord, event: MouseEvent): Promise<void> {
    event.preventDefault();
    actionOrigin = (event.currentTarget as HTMLElement).querySelector('button');
    menu = { game, x: event.clientX, y: event.clientY };
    await tick();
    if (!menu) return;
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
    if (detailsDialog.open) detailsDialog.close();
    selected = game;
    deleteError = '';
    deleteDialog.showModal();
  }

  async function deleteGame(): Promise<void> {
    if (!selected || deleting) return;
    deleting = true;
    deleteError = '';
    const game = selected;
    try {
      await getPlatform().library.remove(game.id);
      games = games?.filter(({ id }) => id !== game.id) ?? [];
      notice = `已从游戏库删除「${game.title || game.name}」。`;
      deleteDialog.close();
    } catch (error) {
      deleteError = `删除失败：${String(error)}`;
    } finally {
      deleting = false;
    }
  }

  function restoreActionFocus(): void {
    (actionOrigin?.isConnected ? actionOrigin : addButton).focus({ preventScroll: true });
  }
</script>

<svelte:window onpageshow={(event) => { if (event.persisted) void reload(); }} onpointerdown={closeMenu} onkeydown={onKeydown} onresize={closeMenu} onscroll={closeMenu} />

<section aria-label="本地游戏" class="@container pb-16">
  <div role="status" aria-live="polite">
    {#if notice}<p class="alert alert-info mb-4">{notice}</p>{/if}
  </div>
  {#if loadError}
    <p role="alert" class="alert alert-error mb-4">{loadError}</p>
    <button class="btn" type="button" onclick={reload}>重试</button>
  {:else if games === null}
    <p class="text-base-content/65" role="status">正在读取游戏库…</p>
  {:else if games.length === 0}
    <div class="rounded-2xl bg-base-200 px-6 py-10">
      <h2 class="text-xl font-semibold">尚无游戏</h2>
      <p class="mt-2 text-base-content/70">选择一个已解压的本地游戏文件夹。</p>
      <button type="button" class="btn btn-primary mt-4" onclick={openImport}>导入游戏</button>
    </div>
  {:else}
    <ul class="grid grid-cols-2 gap-3 @lg:grid-cols-3 @2xl:grid-cols-4 @4xl:grid-cols-5 @5xl:grid-cols-6 @7xl:grid-cols-7">
      {#each games as game (game.id)}
        <GameCard {game} blocked={playBlockReason(game)} onDetails={(event) => openDetails(game, event)} onMenu={(event) => openMenu(game, event)} />
      {/each}
    </ul>
  {/if}
</section>

<button bind:this={addButton} type="button" aria-label="添加游戏" aria-haspopup="dialog" onclick={openImport}
  class="btn btn-circle btn-primary fixed right-[calc(1.5rem+env(safe-area-inset-right))] bottom-[calc(1.5rem+env(safe-area-inset-bottom))] z-30 size-14 text-3xl shadow-lg portrait:bottom-[calc(5rem+env(safe-area-inset-bottom))]">
  <span aria-hidden="true">+</span>
</button>

<dialog bind:this={importDialog} class="modal" aria-labelledby="import-heading" oncancel={(event) => { if (importing) event.preventDefault(); }}>
  <div class="modal-box">
    <h2 id="import-heading" class="text-xl font-semibold">添加游戏</h2>
    <p class="mt-2 text-base-content/70">选择已解压的游戏根目录。</p>
    <input bind:this={folderInput} type="file" webkitdirectory multiple hidden onchange={onFiles} />
    {#if importError}<p role="alert" class="alert alert-error mt-4">{importError}</p>{/if}
    <div class="modal-action">
      <button class="btn" type="button" disabled={importing} onclick={() => importDialog.close()}>关闭</button>
      <button class="btn btn-primary" type="button" disabled={importing} onclick={() => importFolder()}>{importing ? '正在导入…' : '本地文件夹'}</button>
    </div>
  </div>
</dialog>

{#if menu}
  <button bind:this={menuButton} type="button" data-card-context-action class="btn btn-sm fixed z-40 shadow-lg" style:left="{menu.x}px" style:top="{menu.y}px" onclick={() => menu && confirmDelete(menu.game)}>
    从游戏库删除
  </button>
{/if}

<dialog bind:this={detailsDialog} class="modal" aria-labelledby="details-heading" onclose={() => { if (!deleteDialog.open) restoreActionFocus(); }}>
  {#if selected}
    <div class="modal-box">
      <h2 id="details-heading" class="text-xl font-semibold">{selected.title || selected.name}</h2>
      <p class="mt-2">{selected.detection.runtimeName ?? '未识别引擎'}</p>
      <p class="text-base-content/70">{playBlockReason(selected) ?? '可开始'}</p>
      <div class="modal-action">
        <button class="btn" type="button" onclick={() => detailsDialog.close()}>关闭</button>
        <button class="btn btn-outline btn-error" type="button" onclick={() => selected && confirmDelete(selected)}>从游戏库删除</button>
      </div>
    </div>
  {/if}
</dialog>

<dialog bind:this={deleteDialog} class="modal" aria-labelledby="delete-heading" aria-describedby="delete-description" onclose={restoreActionFocus} oncancel={(event) => { if (deleting) event.preventDefault(); }}>
  <div class="modal-box">
    <h2 id="delete-heading" class="text-xl font-semibold">从游戏库删除</h2>
    <p id="delete-description" class="mt-2 break-words">删除「{selected?.title || selected?.name}」？本站保存的该游戏存档会一并删除。原始游戏文件将保留。</p>
    {#if deleteError}<p role="alert" class="alert alert-error mt-4">{deleteError}</p>{/if}
    <div class="modal-action">
      <button class="btn" type="button" disabled={deleting} onclick={() => deleteDialog.close()}>取消</button>
      <button class="btn btn-error" type="button" disabled={deleting} onclick={deleteGame}>{deleting ? '正在删除…' : '确认删除'}</button>
    </div>
  </div>
</dialog>
