<script lang="ts">
  import { onMount } from 'svelte';
  import {
    browserSupport,
    defaultSettings,
    formatPlayTime,
    getPlatform,
    relativeTime,
    type AppSettings,
    type BrowserSupport,
    type GameRecord,
  } from '../lib/platform';
  import { runtimeLabel, unsupportedReason } from '@online-gal/core';

  let games = $state<GameRecord[]>([]);
  let settings = $state<AppSettings>(defaultSettings);
  let support = $state<BrowserSupport | null>(null);
  let loading = $state(true);
  let busy = $state<string | null>(null);
  let error = $state<string | null>(null);
  let notice = $state<string | null>(null);
  let dragging = $state(false);
  let query = $state('');
  let engineFilter = $state<string>('all');
  let sort = $state<'recent' | 'name' | 'added'>('recent');
  let detail = $state<GameRecord | null>(null);
  let fileInput: HTMLInputElement;
  let archiveInput: HTMLInputElement;

  const platform = getPlatform();

  const engines = $derived(
    [...new Set(games.map((game) => game.detection.runtimeId).filter(Boolean))].sort() as string[],
  );

  const visible = $derived(
    games
      .filter((game) => {
        if (engineFilter !== 'all' && game.detection.runtimeId !== engineFilter) return false;
        if (!query.trim()) return true;
        const needle = query.trim().toLowerCase();
        return (
          game.name.toLowerCase().includes(needle) ||
          (game.title ?? '').toLowerCase().includes(needle) ||
          game.tags.some((tag) => tag.toLowerCase().includes(needle))
        );
      })
      .sort((a, b) => {
        if (sort === 'name') return a.name.localeCompare(b.name, 'zh-CN');
        if (sort === 'added') return b.addedAt - a.addedAt;
        return (b.lastPlayedAt ?? 0) - (a.lastPlayedAt ?? 0) || b.addedAt - a.addedAt;
      }),
  );

  onMount(async () => {
    support = browserSupport();
    try {
      settings = await platform.loadSettings();
      games = [...(await platform.library.list())];
    } catch (cause) {
      error = `初始化失败：${describe(cause)}`;
    } finally {
      loading = false;
    }
  });

  function describe(cause: unknown): string {
    return cause instanceof Error ? cause.message : String(cause);
  }

  async function refresh(): Promise<void> {
    games = [...(await platform.library.list())];
  }

  /** Runs an import and reports the detected engine. */
  async function runImport(task: () => Promise<{ record: GameRecord; replaced: boolean }>) {
    error = null;
    notice = null;
    busy = '正在导入并识别引擎…';
    try {
      const result = await task();
      await refresh();
      notice = result.replaced
        ? `已更新《${result.record.name}》，识别为 ${runtimeLabel(result.record.detection.runtimeId)}。`
        : `已导入《${result.record.name}》，识别为 ${runtimeLabel(result.record.detection.runtimeId)}${
            result.record.detection.confidence === 'low' ? '（低置信度，可在详情中指定引擎）' : ''
          }。`;
    } catch (cause) {
      error = `导入失败：${describe(cause)}`;
    } finally {
      busy = null;
    }
  }

  async function pickDirectory(): Promise<void> {
    const picker = (
      globalThis as unknown as {
        showDirectoryPicker?: (options?: { mode?: string }) => Promise<FileSystemDirectoryHandle>;
      }
    ).showDirectoryPicker;
    if (!picker) {
      // Without the File System Access API, fall back to the directory input.
      fileInput?.click();
      return;
    }
    try {
      const handle = await picker({ mode: 'read' });
      await runImport(() => platform.importDirectory(handle));
    } catch (cause) {
      if (isAbort(cause)) return;
      error = `无法读取该文件夹：${describe(cause)}`;
    }
  }

  function isAbort(cause: unknown): boolean {
    return cause instanceof DOMException && cause.name === 'AbortError';
  }

  async function onFilesPicked(event: Event): Promise<void> {
    const input = event.currentTarget as HTMLInputElement;
    const files = input.files;
    if (!files || files.length === 0) return;
    await runImport(() => platform.importFileList(files));
    input.value = '';
  }

  async function onDrop(event: DragEvent): Promise<void> {
    event.preventDefault();
    dragging = false;
    const items = event.dataTransfer?.items;
    if (!items || items.length === 0) return;
    await runImport(() => platform.importDataTransfer(items));
  }

  async function remove(game: GameRecord): Promise<void> {
    if (!confirm(`删除《${game.name}》？其存档也会一并移除。`)) return;
    busy = `正在删除《${game.name}》…`;
    try {
      await platform.library.remove(game.id);
      await refresh();
      detail = null;
    } catch (cause) {
      error = `删除失败：${describe(cause)}`;
    } finally {
      busy = null;
    }
  }

  async function toggleFavorite(game: GameRecord): Promise<void> {
    await platform.library.patch(game.id, { favorite: !game.favorite });
    await refresh();
    if (detail?.id === game.id) detail = await platform.library.get(game.id);
  }

  async function forceEngine(game: GameRecord, runtimeId: string): Promise<void> {
    await platform.library.patch(game.id, {
      config: { ...game.config, ...(runtimeId ? { forcedRuntimeId: runtimeId } : {}) },
    });
    await refresh();
    detail = await platform.library.get(game.id);
  }

  async function redetect(game: GameRecord): Promise<void> {
    busy = `正在重新识别《${game.name}》…`;
    try {
      await platform.library.redetect(game.id);
      await refresh();
      detail = await platform.library.get(game.id);
    } finally {
      busy = null;
    }
  }

  function coverStyle(game: GameRecord): string | undefined {
    return game.cover ? `background-image: url("${game.cover}")` : undefined;
  }

  function initial(game: GameRecord): string {
    return [...game.name][0] ?? '?';
  }

  /** Machines without the directory picker must use the input fallback. */
  const canPickDirectory = $derived(support?.fileSystemAccess ?? false);
</script>

<svelte:window
  ondragover={(event) => {
    event.preventDefault();
    dragging = true;
  }}
  ondragleave={(event) => {
    if (event.relatedTarget === null) dragging = false;
  }}
  ondrop={onDrop}
/>

<section class="hero">
  <div class="hero__text">
    <h1>游戏库</h1>
    <p>
      导入本地游戏文件夹或压缩包，平台会在浏览器中识别引擎并直接运行。所有文件都留在本机，不会上传。
    </p>
  </div>
  <div class="hero__actions">
    {#if canPickDirectory}
      <button class="button button--primary" onclick={pickDirectory} disabled={!!busy}>
        导入文件夹
      </button>
    {:else}
      <button class="button button--primary" onclick={() => fileInput?.click()} disabled={!!busy}>
        选择游戏文件
      </button>
    {/if}
    <button class="button" onclick={() => archiveInput?.click()} disabled={!!busy}>选择压缩包</button>
  </div>
</section>

<input
  bind:this={fileInput}
  class="visually-hidden"
  type="file"
  multiple
  webkitdirectory
  onchange={onFilesPicked}
/>

<!--
  A separate input without `webkitdirectory` is what makes an archive selectable at
  all: a directory input cannot yield a `.zip`/`.xp3`, and the copy above promises
  both folders and archives. Keeping it distinct from the folder input also keeps
  `onFilesPicked` shared — the platform tells the two apart by `webkitRelativePath`,
  which only a directory selection carries.
-->
<input
  bind:this={archiveInput}
  class="visually-hidden"
  type="file"
  multiple
  accept=".zip,.xp3,.rar,.7z,.tar,.gz"
  onchange={onFilesPicked}
/>

{#if !canPickDirectory}
  <p class="hint">
    当前浏览器不支持目录选择器，导入时会读取所选文件夹中的全部文件并保存到本机数据库。
  </p>
{/if}

{#if busy}
  <div class="banner banner--busy">{busy}</div>
{/if}
{#if error}
  <div class="banner banner--error">
    {error}
    <button class="button button--ghost" onclick={() => (error = null)}>关闭</button>
  </div>
{/if}
{#if notice}
  <div class="banner banner--ok">
    {notice}
    <button class="button button--ghost" onclick={() => (notice = null)}>关闭</button>
  </div>
{/if}

<div class="toolbar">
  <label class="field">
    <span class="visually-hidden">搜索</span>
    <input type="search" placeholder="搜索名称或标签…" bind:value={query} />
  </label>
  <label class="field">
    <span class="visually-hidden">引擎</span>
    <select bind:value={engineFilter}>
      <option value="all">全部引擎</option>
      {#each engines as engine (engine)}
        <option value={engine}>{runtimeLabel(engine)}</option>
      {/each}
    </select>
  </label>
  <label class="field">
    <span class="visually-hidden">排序</span>
    <select bind:value={sort}>
      <option value="recent">最近游玩</option>
      <option value="added">最近添加</option>
      <option value="name">按名称</option>
    </select>
  </label>
  <span class="toolbar__count">{visible.length} / {games.length}</span>
</div>

{#if loading}
  <p class="empty">正在载入游戏库…</p>
{:else if games.length === 0}
  <div class="empty empty--drop">
    <p class="empty__title">还没有游戏</p>
    <p>把游戏文件夹拖到窗口中，或点击「导入文件夹」。</p>
    <p class="empty__hint">
      支持：KiriKiri/KAG3（.xp3、startup.tjs）、ONScripter（nscript.dat、.nsa）、TyranoBuilder、
      RPG Maker MV/MZ 与其它 HTML5 游戏。
    </p>
  </div>
{:else if visible.length === 0}
  <p class="empty">没有符合条件的游戏。</p>
{:else}
  <ul class="grid">
    {#each visible as game (game.id)}
      <li class="card">
        <button class="card__button" onclick={() => (detail = game)}>
          <span class="card__cover" style={coverStyle(game)}>
            {#if !game.cover}<span class="card__initial">{initial(game)}</span>{/if}
            <span class="card__engine">{runtimeLabel(game.detection.runtimeId)}</span>
            {#if game.favorite}<span class="card__favorite" aria-label="收藏">★</span>{/if}
          </span>
          <span class="card__body">
            <span class="card__name" title={game.name}>{game.name}</span>
            <span class="card__meta">
              {relativeTime(game.lastPlayedAt)} · {formatPlayTime(game.playTimeMs)}
            </span>
          </span>
        </button>
        <div class="card__actions">
          {#if game.detection.runtimeId && game.detection.availability !== 'unavailable'}
            <a class="button button--primary button--sm" href={`/play?id=${game.id}`}>开始</a>
          {:else if game.detection.availability === 'unavailable'}
            <span class="card__unsupported" title="该引擎当前无法在浏览器中运行">
              引擎不可用
            </span>
          {:else}
            <span class="card__unsupported">引擎未识别</span>
          {/if}
          <button class="button button--ghost button--sm" onclick={() => (detail = game)}>
            详情
          </button>
        </div>
      </li>
    {/each}
  </ul>
{/if}

{#if dragging}
  <div class="dropzone"><p>松开以导入游戏</p></div>
{/if}

{#if detail}
  {@const unsupported = unsupportedReason(detail.detection.runtimeId ?? '')}
  <div
    class="modal"
    role="dialog"
    aria-modal="true"
    aria-label={`${detail.name} 详情`}
    onclick={(event) => {
      if (event.target === event.currentTarget) detail = null;
    }}
  >
    <div class="modal__panel">
      <header class="modal__header">
        <h2>{detail.name}</h2>
        <button class="button button--ghost" onclick={() => (detail = null)}>关闭</button>
      </header>

      <dl class="facts">
        <div>
          <dt>识别引擎</dt>
          <dd>
            {runtimeLabel(detail.detection.runtimeId)}
            <span class="pill pill--{detail.detection.confidence}">
              {detail.detection.confidence}
            </span>
          </dd>
        </div>
        <div>
          <dt>匹配分值</dt>
          <dd>{detail.detection.score}</dd>
        </div>
        <div>
          <dt>来源</dt>
          <dd>
            {detail.source.kind === 'directory'
              ? '本地文件夹（授权访问）'
              : detail.source.kind === 'local'
                ? '开发服务器路径'
                : `已导入文件（${detail.source.fileCount ?? 0} 个）`}
          </dd>
        </div>
        <div>
          <dt>游玩时长</dt>
          <dd>{formatPlayTime(detail.playTimeMs)}</dd>
        </div>
        <div>
          <dt>最近游玩</dt>
          <dd>{relativeTime(detail.lastPlayedAt)}</dd>
        </div>
      </dl>

      {#if unsupported}
        <div class="banner banner--error">{unsupported}</div>
      {/if}

      {#if detail.detection.evidence.length > 0}
        <h3>识别依据</h3>
        <ul class="evidence">
          {#each detail.detection.evidence as item (item.path + item.detail)}
            <li><code>{item.path}</code><span>{item.detail}</span></li>
          {/each}
        </ul>
      {/if}

      {#if detail.detection.alternatives.length > 0}
        <h3>其它可能</h3>
        <ul class="evidence">
          {#each detail.detection.alternatives as item (item.runtimeId)}
            <li><code>{item.displayName}</code><span>分值 {item.score}</span></li>
          {/each}
        </ul>
      {/if}

      <h3>指定引擎</h3>
      <p class="hint">识别错误时可以手动指定；留空表示使用自动识别的结果。</p>
      <select
        value={detail.config.forcedRuntimeId ?? ''}
        onchange={(event) => forceEngine(detail!, event.currentTarget.value)}
      >
        <option value="">自动（{runtimeLabel(detail.detection.runtimeId)}）</option>
        {#each platform.registry.list() as provider (provider.runtimeId)}
          <option value={provider.runtimeId}>{provider.displayName}</option>
        {/each}
      </select>

      <footer class="modal__footer">
        <button class="button" onclick={() => toggleFavorite(detail!)}>
          {detail.favorite ? '取消收藏' : '加入收藏'}
        </button>
        <button class="button" onclick={() => redetect(detail!)} disabled={!!busy}>重新识别</button>
        <button class="button button--danger" onclick={() => remove(detail!)} disabled={!!busy}>
          删除
        </button>
        {#if detail.detection.runtimeId && detail.detection.availability !== 'unavailable'}
          <a class="button button--primary" href={`/play?id=${detail.id}`}>开始游戏</a>
        {:else if detail.detection.availability === 'unavailable'}
          <span class="card__unsupported">
            {runtimeLabel(detail.detection.runtimeId)} 运行时尚未接入浏览器
          </span>
        {/if}
      </footer>
    </div>
  </div>
{/if}

<style>
  .hero {
    display: flex;
    flex-wrap: wrap;
    align-items: flex-end;
    justify-content: space-between;
    gap: 20px;
    margin-bottom: 20px;
  }

  .hero__text p {
    margin: 0;
    color: var(--text-muted);
    max-width: 62ch;
  }

  .hero__actions {
    display: flex;
    gap: 10px;
    flex-wrap: wrap;
  }

  .hint {
    color: var(--text-dim);
    font-size: 0.85rem;
    margin: -6px 0 16px;
  }

  .banner {
    display: flex;
    align-items: center;
    justify-content: space-between;
    gap: 12px;
    padding: 10px 14px;
    border-radius: var(--radius-sm);
    border: 1px solid var(--border);
    background: var(--bg-elevated);
    margin-bottom: 14px;
    font-size: 0.9rem;
  }

  .banner--error {
    border-color: rgba(224, 108, 117, 0.45);
    background: rgba(224, 108, 117, 0.1);
    color: #ffd7da;
  }

  .banner--ok {
    border-color: rgba(78, 201, 160, 0.4);
    background: rgba(78, 201, 160, 0.1);
    color: #d3f6ea;
  }

  .banner--busy {
    border-color: rgba(91, 141, 239, 0.4);
    background: var(--accent-soft);
    color: var(--accent-text);
  }

  .toolbar {
    display: flex;
    gap: 10px;
    flex-wrap: wrap;
    align-items: center;
    margin-bottom: 18px;
  }

  .field input,
  .field select,
  select {
    background: var(--bg-elevated);
    border: 1px solid var(--border-strong);
    border-radius: var(--radius-sm);
    padding: 8px 12px;
    min-width: 150px;
  }

  .field input {
    min-width: 240px;
  }

  .toolbar__count {
    margin-left: auto;
    color: var(--text-dim);
    font-size: 0.85rem;
  }

  .grid {
    list-style: none;
    margin: 0;
    padding: 0;
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(220px, 1fr));
    gap: 18px;
  }

  .card {
    background: var(--bg-elevated);
    border: 1px solid var(--border);
    border-radius: var(--radius);
    overflow: hidden;
    display: flex;
    flex-direction: column;
    transition: border-color 0.15s ease, transform 0.15s ease;
  }

  .card:hover {
    border-color: var(--border-strong);
    transform: translateY(-2px);
  }

  .card__button {
    all: unset;
    cursor: pointer;
    display: block;
  }

  .card__cover {
    position: relative;
    display: grid;
    place-items: center;
    aspect-ratio: 3 / 4;
    background:
      linear-gradient(160deg, rgba(91, 141, 239, 0.22), rgba(125, 91, 239, 0.16)),
      var(--bg-panel);
    background-size: cover;
    background-position: center;
  }

  .card__initial {
    font-size: 3rem;
    font-weight: 700;
    color: rgba(255, 255, 255, 0.22);
  }

  .card__engine {
    position: absolute;
    left: 8px;
    bottom: 8px;
    background: rgba(4, 8, 16, 0.72);
    border: 1px solid var(--border);
    color: var(--text-muted);
    font-size: 0.7rem;
    padding: 2px 8px;
    border-radius: 999px;
    backdrop-filter: blur(4px);
  }

  .card__favorite {
    position: absolute;
    right: 8px;
    top: 6px;
    color: var(--warn);
    font-size: 1rem;
  }

  .card__body {
    display: block;
    padding: 10px 12px 4px;
  }

  .card__name {
    display: block;
    font-weight: 600;
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
  }

  .card__meta {
    display: block;
    color: var(--text-dim);
    font-size: 0.78rem;
    margin-top: 2px;
  }

  .card__actions {
    display: flex;
    gap: 8px;
    padding: 10px 12px 12px;
    margin-top: auto;
  }

  .card__unsupported {
    color: var(--text-dim);
    font-size: 0.8rem;
    align-self: center;
  }

  .button--sm {
    padding: 5px 12px;
    font-size: 0.85rem;
  }

  .empty {
    color: var(--text-muted);
    padding: 48px 0;
    text-align: center;
  }

  .empty--drop {
    border: 1px dashed var(--border-strong);
    border-radius: var(--radius);
    background: rgba(255, 255, 255, 0.015);
  }

  .empty__title {
    font-size: 1.1rem;
    color: var(--text);
    margin-bottom: 4px;
  }

  .empty__hint {
    font-size: 0.82rem;
    color: var(--text-dim);
    max-width: 60ch;
    margin: 10px auto 0;
  }

  .dropzone {
    position: fixed;
    inset: 0;
    z-index: 40;
    display: grid;
    place-items: center;
    background: rgba(6, 9, 16, 0.78);
    backdrop-filter: blur(3px);
    border: 3px dashed var(--accent);
    pointer-events: none;
  }

  .dropzone p {
    font-size: 1.3rem;
    color: var(--accent-text);
  }

  .modal {
    position: fixed;
    inset: 0;
    z-index: 50;
    display: grid;
    place-items: center;
    padding: 24px;
    background: rgba(4, 6, 12, 0.72);
    backdrop-filter: blur(4px);
  }

  .modal__panel {
    width: min(680px, 100%);
    max-height: 86vh;
    overflow: auto;
    background: var(--bg-panel);
    border: 1px solid var(--border-strong);
    border-radius: var(--radius-lg);
    box-shadow: var(--shadow-lg);
    padding: 22px 24px 24px;
  }

  .modal__header {
    display: flex;
    align-items: flex-start;
    justify-content: space-between;
    gap: 16px;
    margin-bottom: 12px;
  }

  .modal__header h2 {
    margin: 0;
    word-break: break-word;
  }

  .facts {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
    gap: 12px;
    margin: 0 0 18px;
  }

  .facts div {
    background: var(--bg-elevated);
    border: 1px solid var(--border);
    border-radius: var(--radius-sm);
    padding: 8px 12px;
  }

  .facts dt {
    color: var(--text-dim);
    font-size: 0.76rem;
  }

  .facts dd {
    margin: 2px 0 0;
    font-weight: 600;
  }

  .evidence {
    list-style: none;
    margin: 0 0 16px;
    padding: 0;
    display: grid;
    gap: 4px;
  }

  .evidence li {
    display: flex;
    justify-content: space-between;
    gap: 12px;
    font-size: 0.85rem;
    padding: 5px 10px;
    background: var(--bg);
    border: 1px solid var(--border);
    border-radius: var(--radius-sm);
  }

  .evidence span {
    color: var(--text-muted);
  }

  .pill {
    display: inline-block;
    margin-left: 8px;
    font-size: 0.7rem;
    padding: 1px 7px;
    border-radius: 999px;
    border: 1px solid var(--border-strong);
    color: var(--text-muted);
  }

  .pill--high {
    border-color: rgba(78, 201, 160, 0.5);
    color: var(--ok);
  }

  .pill--medium {
    border-color: rgba(229, 181, 103, 0.5);
    color: var(--warn);
  }

  .pill--low {
    border-color: rgba(224, 108, 117, 0.5);
    color: var(--danger);
  }

  .modal__footer {
    display: flex;
    gap: 10px;
    flex-wrap: wrap;
    justify-content: flex-end;
    margin-top: 20px;
    padding-top: 16px;
    border-top: 1px solid var(--border);
  }
</style>
