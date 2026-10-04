<script lang="ts">
  import { onDestroy, onMount } from 'svelte';
  import {
    defaultSettings,
    getPlatform,
    type AppSettings,
    type GameRecord,
    type SaveSlotRecord,
  } from '../lib/platform';
  import { launchSession, runtimeLabel, type Session } from '@online-gal/core';

  let canvas: HTMLCanvasElement;
  let shell: HTMLDivElement;
  let game = $state<GameRecord | null>(null);
  let session = $state<Session | null>(null);
  let settings = $state<AppSettings>(defaultSettings);
  let slots = $state<SaveSlotRecord[]>([]);
  let status = $state('正在载入…');
  let error = $state<string | null>(null);
  let showMenu = $state(false);
  let showLogs = $state(false);
  let showDiagnostics = $state(false);
  let diagnostics = $state<Record<string, unknown> | null>(null);
  let scaling = $state<'fit' | 'pixel' | 'stretch'>('fit');
  let logLines = $state<{ level: string; message: string; at: number }[]>([]);
  let fullscreen = $state(false);

  const platform = getPlatform();
  let diagnosticsTimer: number | undefined;

  onMount(async () => {
    const id = new URLSearchParams(location.search).get('id');
    if (!id) {
      error = '缺少游戏 id。';
      return;
    }
    settings = await platform.loadSettings();
    const record = await platform.library.get(id);
    if (!record) {
      error = '找不到这个游戏，可能已被删除。';
      return;
    }
    game = record;
    if (!record.detection.runtimeId) {
      error = '这个游戏没有识别出可运行的引擎。请回到游戏库手动指定引擎后重试。';
      return;
    }
    await start(record);
  });

  async function start(record: GameRecord): Promise<void> {
    status = `正在启动 ${runtimeLabel(record.detection.runtimeId)}…`;
    try {
      const source = await platform.library.openSource(record);
      const store = platform.saves.fileStore(record.id);
      const instance = await launchSession({
        registry: platform.registry,
        canvas,
        record,
        source,
        store,
        settings,
      });
      session = instance;
      shell.style.aspectRatio = `${canvas.width} / ${canvas.height}`;
      await recordPlayStart(record);
      slots = [...(await platform.saves.list(record.id))];
      status = '';
      diagnosticsTimer = window.setInterval(() => {
        diagnostics = instance.describe();
      }, 1000);
      // A handle for browser-side smoke testing: the repository's own
      // verification drives a session through this, and it costs nothing when
      // unused. It exposes only read-only diagnostics and the public session
      // API, never game content. `save` returns the **promise** — a caller that
      // wants the record must await it, and a call that resolves is a checkpoint
      // the engine actually acknowledged.
      (window as unknown as { __ogSession?: unknown }).__ogSession = {
        describe: () => instance.describe(),
        save: (slot: number) => instance.save(slot),
        stop: () => instance.stop(),
        record: record.id,
      };
    } catch (cause) {
      error = `启动失败：${cause instanceof Error ? cause.message : String(cause)}`;
      status = '';
    }
  }

  /** Records the play session in the library so the shelf reflects reality. */
  async function recordPlayStart(record: GameRecord): Promise<void> {
    if (record.lastPlayedAt === null) {
      await platform.library.patch(record.id, { lastPlayedAt: Date.now() });
    }
    // The duration is written on stop, not here, so a crashed tab cannot leave
    // an inflated play time behind.
    await platform.library.patch(record.id, { lastPlayedAt: Date.now() });
  }

  async function stopSession(): Promise<void> {
    const instance = session;
    const record = game;
    if (!instance) return;
    if (diagnosticsTimer !== undefined) window.clearInterval(diagnosticsTimer);
    const elapsed = instance.elapsedMs;
    session = null;
    await instance.stop();
    if (record) {
      await platform.library.patch(record.id, {
        playTimeMs: record.playTimeMs + Math.round(elapsed),
        lastPlayedAt: Date.now(),
      });
    }
  }

  onDestroy(() => {
    void stopSession();
  });

  async function saveTo(slot: number): Promise<void> {
    if (!session || !game) return;
    try {
      // `session.save` awaits the engine's own checkpoint before it returns a record, so a runtime
      // that cannot save rejects here and no slot is written. The success UI below therefore only
      // appears for a save that actually happened.
      const record = await session.save(slot);
      await platform.saves.put(record);
      slots = [...(await platform.saves.list(game.id))];
      showMenu = false;
    } catch (cause) {
      alert(`保存失败：${cause instanceof Error ? cause.message : String(cause)}`);
    }
  }

  async function loadFrom(slot: number): Promise<void> {
    if (!session || !game) return;
    const record = await platform.saves.get(game.id, slot);
    if (!record) return;
    try {
      await session.restore(record);
      showMenu = false;
    } catch (cause) {
      // A restore the engine refused ends the session rather than leaving a half-applied scene, so
      // this says so instead of silently returning to the menu.
      alert(`读取失败：${cause instanceof Error ? cause.message : String(cause)}`);
    }
  }

  async function deleteSlot(slot: number): Promise<void> {
    if (!game) return;
    if (!confirm(`删除存档 ${slot}？`)) return;
    await platform.saves.remove(game.id, slot);
    slots = [...(await platform.saves.list(game.id))];
  }

  async function exportSaves(): Promise<void> {
    if (!game) return;
    const json = await platform.saves.exportAll(game.id);
    const blob = new Blob([json], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const anchor = document.createElement('a');
    anchor.href = url;
    anchor.download = `${game.name}-saves.json`;
    anchor.click();
    URL.revokeObjectURL(url);
  }

  async function importSaves(event: Event): Promise<void> {
    if (!game) return;
    const input = event.currentTarget as HTMLInputElement;
    const file = input.files?.[0];
    if (!file) return;
    try {
      const count = await platform.saves.importAll(game.id, await file.text());
      slots = [...(await platform.saves.list(game.id))];
      alert(`已导入 ${count} 个存档。`);
    } catch (cause) {
      alert(`导入失败：${cause instanceof Error ? cause.message : String(cause)}`);
    }
    input.value = '';
  }

  async function updateSetting<K extends keyof AppSettings>(key: K, value: AppSettings[K]) {
    settings = await platform.settings.patch({ [key]: value } as Partial<AppSettings>);
    session?.applySettings(settings);
  }

  async function toggleFullscreen(): Promise<void> {
    if (document.fullscreenElement) {
      await document.exitFullscreen();
    } else {
      await shell?.requestFullscreen();
    }
  }

  function onFullscreenChange(): void {
    fullscreen = document.fullscreenElement === shell;
  }

  function togglePause(): void {
    if (!session) return;
    if (session.isPaused) session.resume();
    else session.pause();
  }

  /** Keyboard shortcuts that the canvas itself does not own. */
  function onKeydown(event: KeyboardEvent): void {
    if (event.key === 'Escape') {
      showMenu = !showMenu;
      return;
    }
    if (event.key === 'F11' || (event.key === 'f' && event.ctrlKey)) {
      event.preventDefault();
      void toggleFullscreen();
      return;
    }
    if (event.key === 'p' && event.altKey) {
      event.preventDefault();
      togglePause();
    }
  }

  const surfaceLabel = $derived(
    canvas ? `${canvas.width} × ${canvas.height}` : '—',
  );

  /**
   * Compatibility state, read from the runtime's own report.
   *
   * The summary lives in `describe()` because the diagnostics poll already runs
   * once a second; `report()` carries the per-command detail, so it is pulled
   * alongside and flattened for the template.
   */
  interface Summary {
    severity: 'clean' | 'degraded' | 'unsupported';
    unsupportedCommands: number;
    approximatedCommands: number;
    commandsHonoured: number;
  }

  const compatibility = $derived.by(() => {
    const summary = (diagnostics?.runtime as { compatibility?: Summary } | undefined)
      ?.compatibility;
    if (!summary) return null;
    return { ...summary, detail: session?.compatibilityDetail() ?? [] };
  });

  function severityLabel(severity: Summary['severity']): string {
    switch (severity) {
      case 'clean':
        return '完全支持';
      case 'degraded':
        return '降级可用';
      default:
        return '存在未支持命令';
    }
  }
</script>

<svelte:window onkeydown={onKeydown} onfullscreenchange={onFullscreenChange} />

<div class="player">
  <div class="player__bar">
    <a class="button button--ghost" href="/">← 返回游戏库</a>
    <div class="player__title">
      <strong>{game?.name ?? '载入中'}</strong>
      <span>{game ? runtimeLabel(game.detection.runtimeId) : ''}</span>
    </div>
    <div class="player__tools">
      <span class="surface" title="引擎渲染分辨率">{surfaceLabel}</span>
      {#if diagnostics && showDiagnostics}
        <span class="surface" title="帧率">{String(diagnostics.fps ?? 0)} fps</span>
      {/if}
      <button class="button button--sm" onclick={togglePause} disabled={!session}>
        {session?.isPaused ? '继续' : '暂停'}
      </button>
      <button class="button button--sm" onclick={() => (showMenu = !showMenu)}>菜单</button>
    </div>
  </div>

  {#if error}
    <div class="player__error">
      <p>{error}</p>
      <a class="button" href="/">返回游戏库</a>
    </div>
  {/if}

  <div
    class="stage stage--{scaling}"
    bind:this={shell}
    ondblclick={toggleFullscreen}
    role="presentation"
  >
    <canvas bind:this={canvas} tabindex="0" aria-label="游戏画面"></canvas>
    {#if status}
      <div class="stage__status">{status}</div>
    {/if}
  </div>

  <p class="player__hint">
    点击画面开始。双击进入全屏，<kbd>Esc</kbd> 打开菜单，<kbd>Alt</kbd>+<kbd>P</kbd> 暂停。
  </p>

  {#if showDiagnostics && diagnostics}
    <section class="panel">
      <header class="panel__header">
        <h2>引擎状态</h2>
        <button class="button button--ghost" onclick={() => (showDiagnostics = false)}>隐藏</button>
      </header>
      <pre>{JSON.stringify(diagnostics, null, 2)}</pre>
    </section>
  {/if}
</div>

{#if showMenu}
  <div
    class="modal"
    role="dialog"
    aria-modal="true"
    aria-label="游戏菜单"
    onclick={(event) => {
      if (event.target === event.currentTarget) showMenu = false;
    }}
  >
    <div class="modal__panel">
      <header class="modal__header">
        <h2>菜单</h2>
        <button class="button button--ghost" onclick={() => (showMenu = false)}>关闭</button>
      </header>

      <h3>存档</h3>
      <div class="slots">
        {#each [1, 2, 3, 4, 5, 6] as slot (slot)}
          {@const record = slots.find((entry) => entry.slot === slot)}
          <div class="slot">
            <div class="slot__thumb">
              {#if record?.thumbnail}
                <img src={record.thumbnail} alt={`存档 ${slot} 预览`} />
              {:else}
                <span>{slot}</span>
              {/if}
            </div>
            <div class="slot__body">
              <strong>存档 {slot}</strong>
              <span class="slot__caption">{record?.caption ?? '空'}</span>
              {#if record}
                <span class="slot__time">{new Date(record.createdAt).toLocaleString('zh-CN')}</span>
              {/if}
            </div>
            <div class="slot__actions">
              <button class="button button--sm" onclick={() => saveTo(slot)} disabled={!session}>
                保存
              </button>
              <button
                class="button button--sm"
                onclick={() => loadFrom(slot)}
                disabled={!record || !session}
              >
                读取
              </button>
              {#if record}
                <button class="button button--sm button--danger" onclick={() => deleteSlot(slot)}>
                  删除
                </button>
              {/if}
            </div>
          </div>
        {/each}
      </div>

      <h3>显示</h3>
      <div class="settings-row">
        <label for="scaling">缩放方式</label>
        <select id="scaling" bind:value={scaling}>
          <option value="fit">适应窗口（保持比例）</option>
          <option value="pixel">整数倍像素（清晰）</option>
          <option value="stretch">拉伸填满</option>
        </select>
      </div>

      <h3>文本与音量</h3>
      <div class="settings-row">
        <label for="speed">文字速度（字/秒）</label>
        <input
          id="speed"
          type="range"
          min="0"
          max="200"
          step="5"
          value={settings.textSpeed}
          oninput={(event) => updateSetting('textSpeed', Number(event.currentTarget.value))}
        />
        <output>{settings.textSpeed}</output>
      </div>
      <div class="settings-row">
        <label for="bgm">BGM</label>
        <input
          id="bgm"
          type="range"
          min="0"
          max="1"
          step="0.05"
          value={settings.volumes.bgm}
          oninput={(event) =>
            updateSetting('volumes', {
              ...settings.volumes,
              bgm: Number(event.currentTarget.value),
            })}
        />
      </div>
      <div class="settings-row">
        <label for="se">音效</label>
        <input
          id="se"
          type="range"
          min="0"
          max="1"
          step="0.05"
          value={settings.volumes.se}
          oninput={(event) =>
            updateSetting('volumes', {
              ...settings.volumes,
              se: Number(event.currentTarget.value),
            })}
        />
      </div>

      <h3>存档备份</h3>
      <div class="modal__footer">
        <button class="button" onclick={exportSaves}>导出存档</button>
        <label class="button">
          导入存档
          <input class="visually-hidden" type="file" accept="application/json" onchange={importSaves} />
        </label>
        <button class="button" onclick={() => (showLogs = !showLogs)}>
          {showLogs ? '隐藏日志' : '运行日志'}
        </button>
        <button class="button" onclick={() => (showDiagnostics = !showDiagnostics)}>
          {showDiagnostics ? '隐藏引擎状态' : '引擎状态'}
        </button>
        <button class="button" onclick={toggleFullscreen}>
          {fullscreen ? '退出全屏' : '进入全屏'}
        </button>
      </div>

      {#if compatibility}
        <h3>兼容性报告</h3>
        <p class="hint">
          引擎发出、但运行时无法完整处理的命令。严重程度越高，越可能出现卡住或表现异常。
        </p>
        <dl class="facts">
          <div>
            <dt>结论</dt>
            <dd class="sev sev--{compatibility.severity}">
              {severityLabel(compatibility.severity)}
            </dd>
          </div>
          <div><dt>已处理命令</dt><dd>{compatibility.commandsHonoured}</dd></div>
          <div><dt>跳过</dt><dd>{compatibility.unsupportedCommands}</dd></div>
          <div><dt>降级处理</dt><dd>{compatibility.approximatedCommands}</dd></div>
        </dl>
        {#if compatibility.detail.length > 0}
          <ul class="commands">
            {#each compatibility.detail as item (item.command)}
              <li>
                <code>{item.command}</code>
                <span class="tag tag--{item.handling}">
                  {item.handling === 'skipped' ? '跳过' : '降级'}
                </span>
                <span class="count">×{item.count}</span>
                {#if item.line !== undefined}<span class="line">行 {item.line}</span>{/if}
                {#if item.note}<span class="note">{item.note}</span>{/if}
              </li>
            {/each}
          </ul>
        {/if}
      {/if}

      {#if showLogs}
        <pre class="logs">{logLines.length === 0 ? '暂无日志' : logLines.map((line) => line.message).join('\n')}</pre>
      {/if}
    </div>
  </div>
{/if}

<style>
  .player {
    display: flex;
    flex-direction: column;
    gap: 12px;
    padding: 14px 20px 32px;
    min-height: 100vh;
  }

  .player__bar {
    display: flex;
    align-items: center;
    gap: 14px;
    flex-wrap: wrap;
  }

  .player__title {
    display: flex;
    flex-direction: column;
    line-height: 1.2;
    min-width: 0;
  }

  .player__title span {
    color: var(--text-dim);
    font-size: 0.78rem;
  }

  .player__tools {
    display: flex;
    align-items: center;
    gap: 8px;
    margin-left: auto;
  }

  .surface {
    color: var(--text-dim);
    font-size: 0.78rem;
    font-family: var(--font-mono);
    padding: 3px 8px;
    border: 1px solid var(--border);
    border-radius: var(--radius-sm);
  }

  .player__error {
    border: 1px solid rgba(224, 108, 117, 0.45);
    background: rgba(224, 108, 117, 0.1);
    border-radius: var(--radius);
    padding: 14px 16px;
    display: flex;
    align-items: center;
    justify-content: space-between;
    gap: 16px;
    flex-wrap: wrap;
  }

  .player__error p {
    margin: 0;
  }

  .stage {
    position: relative;
    width: 100%;
    max-height: calc(100vh - 190px);
    display: grid;
    place-items: center;
    background: #000;
    border: 1px solid var(--border);
    border-radius: var(--radius);
    overflow: hidden;
  }

  .stage :global(canvas) {
    display: block;
    width: 100%;
    height: 100%;
    outline: none;
    touch-action: none;
    background: #000;
  }

  .stage--fit :global(canvas) {
    object-fit: contain;
  }

  .stage--pixel :global(canvas) {
    object-fit: contain;
    image-rendering: pixelated;
  }

  .stage--stretch :global(canvas) {
    object-fit: fill;
  }

  .stage__status {
    position: absolute;
    bottom: 14px;
    left: 50%;
    transform: translateX(-50%);
    background: rgba(4, 8, 16, 0.82);
    border: 1px solid var(--border);
    border-radius: 999px;
    padding: 6px 16px;
    font-size: 0.85rem;
    color: var(--accent-text);
  }

  .player__hint {
    color: var(--text-dim);
    font-size: 0.82rem;
    margin: 0;
  }

  kbd {
    border: 1px solid var(--border-strong);
    border-radius: 4px;
    padding: 0 5px;
    font-size: 0.78rem;
    background: var(--bg-elevated);
  }

  .panel {
    border: 1px solid var(--border);
    border-radius: var(--radius);
    background: var(--bg-elevated);
    padding: 14px 16px;
  }

  .panel__header {
    display: flex;
    align-items: center;
    justify-content: space-between;
  }

  .panel h2 {
    margin: 0;
    font-size: 1rem;
  }

  .modal {
    position: fixed;
    inset: 0;
    z-index: 50;
    display: grid;
    place-items: center;
    padding: 24px;
    background: rgba(4, 6, 12, 0.74);
    backdrop-filter: blur(4px);
  }

  .modal__panel {
    width: min(760px, 100%);
    max-height: 88vh;
    overflow: auto;
    background: var(--bg-panel);
    border: 1px solid var(--border-strong);
    border-radius: var(--radius-lg);
    box-shadow: var(--shadow-lg);
    padding: 22px 24px 24px;
  }

  .modal__header {
    display: flex;
    align-items: center;
    justify-content: space-between;
    margin-bottom: 10px;
  }

  .modal__header h2 {
    margin: 0;
  }

  .slots {
    display: grid;
    gap: 8px;
    margin-bottom: 18px;
  }

  .slot {
    display: flex;
    align-items: center;
    gap: 12px;
    background: var(--bg-elevated);
    border: 1px solid var(--border);
    border-radius: var(--radius-sm);
    padding: 8px 10px;
  }

  .slot__thumb {
    width: 84px;
    height: 52px;
    flex: none;
    display: grid;
    place-items: center;
    background: var(--bg);
    border: 1px solid var(--border);
    border-radius: 4px;
    overflow: hidden;
    color: var(--text-dim);
  }

  .slot__thumb img {
    width: 100%;
    height: 100%;
    object-fit: cover;
  }

  .slot__body {
    display: flex;
    flex-direction: column;
    min-width: 0;
    flex: 1;
  }

  .slot__caption {
    color: var(--text-muted);
    font-size: 0.82rem;
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
  }

  .slot__time {
    color: var(--text-dim);
    font-size: 0.74rem;
  }

  .slot__actions {
    display: flex;
    gap: 6px;
    flex: none;
  }

  .settings-row {
    display: flex;
    align-items: center;
    gap: 12px;
    margin-bottom: 8px;
  }

  .settings-row label {
    min-width: 150px;
    color: var(--text-muted);
    font-size: 0.88rem;
  }

  .settings-row input[type='range'] {
    flex: 1;
    accent-color: var(--accent);
  }

  .settings-row output {
    width: 42px;
    text-align: right;
    font-family: var(--font-mono);
    color: var(--text-dim);
  }

  .modal__footer {
    display: flex;
    gap: 10px;
    flex-wrap: wrap;
    margin-top: 14px;
    padding-top: 14px;
    border-top: 1px solid var(--border);
  }

  .logs {
    max-height: 220px;
    margin-top: 12px;
    font-size: 0.78rem;
  }

  .sev--clean {
    color: var(--ok);
  }

  .sev--degraded {
    color: var(--warn);
  }

  .sev--unsupported {
    color: var(--danger);
  }

  .commands {
    list-style: none;
    margin: 0 0 16px;
    padding: 0;
    display: grid;
    gap: 4px;
    max-height: 220px;
    overflow: auto;
  }

  .commands li {
    display: flex;
    align-items: center;
    gap: 8px;
    flex-wrap: wrap;
    padding: 5px 10px;
    background: var(--bg);
    border: 1px solid var(--border);
    border-radius: var(--radius-sm);
    font-size: 0.82rem;
  }

  .commands .count {
    color: var(--text-dim);
    font-family: var(--font-mono);
  }

  .commands .line {
    color: var(--text-dim);
  }

  .commands .note {
    color: var(--text-muted);
    flex-basis: 100%;
    font-size: 0.78rem;
  }

  .tag {
    font-size: 0.7rem;
    padding: 1px 7px;
    border-radius: 999px;
    border: 1px solid var(--border-strong);
    color: var(--text-muted);
  }

  .tag--skipped {
    border-color: rgba(224, 108, 117, 0.5);
    color: var(--danger);
  }

  .tag--approximated {
    border-color: rgba(229, 181, 103, 0.5);
    color: var(--warn);
  }

  .button--sm {
    padding: 5px 12px;
    font-size: 0.85rem;
  }
</style>
