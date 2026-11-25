<script lang="ts">
  import { onMount } from 'svelte';
  import { DirectoryPermissionError, getPlatform, pickGameDirectory, playBlockReason, startGame, type GameRecord, type Session } from '../../lib/platform';

  let canvas: HTMLCanvasElement;
  let record = $state<GameRecord | null>(null);
  let status = $state('正在载入…');
  let error = $state('');
  let playing = $state(false);
  let leaving = $state(false);
  let needsDirectory = $state(false);
  let recovering = $state(false);
  let session: Session | null = null;
  let disposed = false;

  onMount(() => {
    void load();
    return dispose;
  });

  async function load(): Promise<void> {
    try {
      const id = new URLSearchParams(location.search).get('id');
      record = id ? await getPlatform().library.get(id) : null;
      if (!record) {
        error = '游戏不存在';
        return;
      }
      await play(record);
    } catch (cause) {
      error = `无法读取游戏：${String(cause)}`;
    }
  }

  async function play(game: GameRecord): Promise<void> {
    if (disposed) return;
    error = '';
    needsDirectory = false;
    status = '正在启动…';
    const blocked = playBlockReason(game);
    if (blocked) {
      error = blocked;
      return;
    }
    try {
      const instance = await startGame(game, canvas);
      if (disposed) {
        await instance.stop();
        return;
      }
      session = instance;
      playing = true;
      void getPlatform().library.patch(game.id, { lastPlayedAt: Date.now() }).catch(console.error);
    } catch (cause) {
      needsDirectory = cause instanceof DirectoryPermissionError;
      error = needsDirectory ? '无法读取原游戏文件夹。请重新授权或重选游戏根目录。' : `启动失败：${String(cause)}`;
    }
  }

  async function recoverDirectory(reselect: boolean): Promise<void> {
    if (!record || recovering) return;
    recovering = true;
    const game = record;
    try {
      const platform = getPlatform();
      if (reselect) {
        const selection = pickGameDirectory();
        if (!selection) {
          error = '此浏览器无法重选目录句柄。请在支持目录选择的浏览器中恢复授权。';
          return;
        }
        record = await platform.reselectDirectory(game.id, await selection);
      } else if (!(await platform.requestDirectoryPermission(game))) {
        error = '未获得读取权限。你可以重试或重选游戏根目录。';
        return;
      }
      await play(record);
    } catch (cause) {
      if (!(cause instanceof DOMException && cause.name === 'AbortError')) {
        error = `恢复目录失败：${String(cause)}`;
      }
    } finally {
      recovering = false;
    }
  }

  function dispose(): void {
    disposed = true;
    const instance = session;
    session = null;
    void instance?.stop().catch(console.error);
  }

  async function leave(event: MouseEvent): Promise<void> {
    if (event.button !== 0 || event.metaKey || event.ctrlKey || event.shiftKey || event.altKey) return;
    if (!session) return;
    event.preventDefault();
    if (leaving) return;
    leaving = true;
    const instance = session;
    session = null;
    try {
      await instance.stop();
      const library = getPlatform().library;
      const current = await library.get(instance.gameRecord.id);
      if (current) await library.patch(current.id, { playTimeMs: current.playTimeMs + instance.elapsedMs });
    } catch (cause) {
      console.error(cause);
    } finally {
      location.assign('/');
    }
  }
</script>

<svelte:window onpagehide={dispose} onpageshow={(event) => { if (event.persisted) location.reload(); }} />

<div class="flex h-dvh flex-col bg-neutral text-neutral-content">
  <header class="flex items-center justify-between gap-4 px-4 py-2">
    <a href="/" class="btn btn-sm" onclick={leave} aria-disabled={leaving}>返回游戏库</a>
    {#if record}<h1 class="truncate text-sm font-medium">{record.title || record.name}</h1>{/if}
  </header>
  <main class="relative isolate grid min-h-0 flex-1 place-items-center overflow-hidden bg-black text-white">
    <canvas bind:this={canvas} tabindex="0" aria-label="游戏画面" class="block max-h-full max-w-full object-contain" class:invisible={!playing}></canvas>
    {#if !playing}
      <div class="absolute inset-0 grid place-items-center px-4 text-center">
        {#if error}
          <div class="max-w-prose break-words">
            <p role="alert">{error}</p>
            {#if needsDirectory}
              <div class="mt-4 flex flex-wrap justify-center gap-2">
                <button type="button" class="btn btn-primary" disabled={recovering} onclick={() => recoverDirectory(false)}>重新授权</button>
                <button type="button" class="btn" disabled={recovering} onclick={() => recoverDirectory(true)}>重选文件夹</button>
              </div>
            {/if}
          </div>
        {:else}
          <p role="status">{status}</p>
        {/if}
      </div>
    {/if}
  </main>
</div>
