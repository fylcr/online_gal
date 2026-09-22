<script lang="ts">
  import { onMount } from 'svelte';
  import {
    browserSupport,
    defaultSettings,
    getPlatform,
    type AppSettings,
    type BrowserSupport,
  } from '../lib/platform';

  let settings = $state<AppSettings>(defaultSettings);
  let support = $state<BrowserSupport | null>(null);
  let saved = $state(false);
  let librarySize = $state(0);
  let saveSlotCount = $state(0);
  let storageEstimate = $state<string>('未知');

  const platform = getPlatform();

  onMount(async () => {
    settings = await platform.loadSettings();
    support = browserSupport();
    const games = await platform.library.list();
    librarySize = games.length;
    let slots = 0;
    for (const game of games) slots += (await platform.saves.list(game.id)).length;
    saveSlotCount = slots;
    const estimate = await navigator.storage?.estimate?.();
    if (estimate?.usage !== undefined) {
      const used = estimate.usage / 1024 / 1024;
      storageEstimate = `${used.toFixed(1)} MB`;
    }
  });

  async function patch<K extends keyof AppSettings>(key: K, value: AppSettings[K]): Promise<void> {
    settings = await platform.settings.patch({ [key]: value } as Partial<AppSettings>);
    saved = true;
    window.setTimeout(() => (saved = false), 1400);
  }

  async function reset(): Promise<void> {
    if (!confirm('恢复默认设置？')) return;
    settings = await platform.settings.save({ ...defaultSettings });
  }

  async function requestPersistence(): Promise<void> {
    if (!navigator.storage?.persist) {
      alert('此浏览器不支持申请持久化存储。');
      return;
    }
    const granted = await navigator.storage.persist();
    alert(granted ? '已获得持久化存储，浏览器不会自动清理存档。' : '浏览器拒绝了持久化申请。');
  }

  async function clearLibrary(): Promise<void> {
    if (!confirm('删除全部游戏与存档？此操作不可撤销。')) return;
    for (const game of await platform.library.list()) {
      await platform.library.remove(game.id);
    }
    librarySize = 0;
    saveSlotCount = 0;
  }

  const supportRows: { key: keyof BrowserSupport; label: string; hint: string }[] = [
    { key: 'indexedDb', label: 'IndexedDB', hint: '游戏库、存档与设置持久化' },
    { key: 'fileSystemAccess', label: 'File System Access', hint: '直接读取游戏文件夹，无需复制' },
    { key: 'createImageBitmap', label: 'createImageBitmap', hint: '高效解码 PNG/JPEG/WebP' },
    { key: 'decompressionStream', label: 'DecompressionStream', hint: 'XP3 与 ZIP 包解压' },
    { key: 'audioContext', label: 'Web Audio', hint: 'BGM / 音效 / 语音播放' },
    { key: 'offscreenCanvas', label: 'OffscreenCanvas', hint: '文本测量与后台绘制' },
    { key: 'webAssembly', label: 'WebAssembly', hint: '未来接入 wasm 引擎运行时所需' },
  ];
</script>

<section>
  <h1>设置</h1>
  <p class="lead">设置保存在本机 IndexedDB 中，并在每次启动游戏时应用。</p>

  <div class="card">
    <h2>文本</h2>
    <div class="row">
      <label for="speed">文字速度</label>
      <input
        id="speed"
        type="range"
        min="0"
        max="200"
        step="5"
        value={settings.textSpeed}
        oninput={(event) => patch('textSpeed', Number(event.currentTarget.value))}
      />
      <output>{settings.textSpeed} 字/秒</output>
    </div>
    <p class="hint">0 表示瞬间显示全部文字。游戏内滚动滚轮也可以临时调整速度。</p>
  </div>

  <div class="card">
    <h2>音量</h2>
    <div class="row">
      <label for="bgm">BGM</label>
      <input
        id="bgm"
        type="range"
        min="0"
        max="1"
        step="0.05"
        value={settings.volumes.bgm}
        oninput={(event) =>
          patch('volumes', { ...settings.volumes, bgm: Number(event.currentTarget.value) })}
      />
      <output>{Math.round(settings.volumes.bgm * 100)}%</output>
    </div>
    <div class="row">
      <label for="se">音效</label>
      <input
        id="se"
        type="range"
        min="0"
        max="1"
        step="0.05"
        value={settings.volumes.se}
        oninput={(event) =>
          patch('volumes', { ...settings.volumes, se: Number(event.currentTarget.value) })}
      />
      <output>{Math.round(settings.volumes.se * 100)}%</output>
    </div>
    <div class="row">
      <label for="voice">语音</label>
      <input
        id="voice"
        type="range"
        min="0"
        max="1"
        step="0.05"
        value={settings.volumes.voice}
        oninput={(event) =>
          patch('volumes', { ...settings.volumes, voice: Number(event.currentTarget.value) })}
      />
      <output>{Math.round(settings.volumes.voice * 100)}%</output>
    </div>
    <div class="row">
      <label for="mute">静音</label>
      <input
        id="mute"
        type="checkbox"
        checked={settings.mute}
        onchange={(event) => patch('mute', event.currentTarget.checked)}
      />
    </div>
  </div>

  <div class="card">
    <h2>显示</h2>
    <div class="row">
      <label for="width">默认渲染宽度</label>
      <input
        id="width"
        type="number"
        min="320"
        max="3840"
        step="16"
        value={settings.surface.width}
        onchange={(event) =>
          patch('surface', { ...settings.surface, width: Number(event.currentTarget.value) })}
      />
      <span>px</span>
    </div>
    <div class="row">
      <label for="height">默认渲染高度</label>
      <input
        id="height"
        type="number"
        min="240"
        max="2160"
        step="16"
        value={settings.surface.height}
        onchange={(event) =>
          patch('surface', { ...settings.surface, height: Number(event.currentTarget.value) })}
      />
      <span>px</span>
    </div>
    <p class="hint">
      只有在游戏自身没有声明分辨率时才会生效（KiriKiri 会从 Config.tjs 读取真实分辨率）。
    </p>
    <div class="row">
      <label for="diag">游玩时显示引擎状态</label>
      <input
        id="diag"
        type="checkbox"
        checked={settings.showDiagnostics}
        onchange={(event) => patch('showDiagnostics', event.currentTarget.checked)}
      />
    </div>
  </div>

  <div class="card">
    <h2>存储</h2>
    <dl class="facts">
      <div><dt>游戏数量</dt><dd>{librarySize}</dd></div>
      <div><dt>存档数量</dt><dd>{saveSlotCount}</dd></div>
      <div><dt>已用空间</dt><dd>{storageEstimate}</dd></div>
    </dl>
    <div class="row">
      <button class="button" onclick={requestPersistence}>申请持久化存储</button>
      <button class="button button--danger" onclick={clearLibrary}>删除全部数据</button>
    </div>
    <p class="hint">
      存档以「游戏 + 可写覆盖层」的形式保存：导入的游戏目录始终只读，引擎写入的存档进入独立的
      IndexedDB 命名空间。
    </p>
  </div>

  <div class="card">
    <h2>浏览器能力检测</h2>
    <ul class="support">
      {#each supportRows as row (row.key)}
        <li>
          <span class="dot" class:ok={support?.[row.key]} class:no={support && !support[row.key]}
          ></span>
          <div>
            <strong>{row.label}</strong>
            <span>{row.hint}</span>
          </div>
          <em>{support?.[row.key] ? '可用' : '不可用'}</em>
        </li>
      {/each}
    </ul>
  </div>

  <div class="row">
    <button class="button" onclick={reset}>恢复默认</button>
    {#if saved}<span class="saved">已保存</span>{/if}
  </div>
</section>

<style>
  .lead {
    color: var(--text-muted);
    margin-top: -6px;
  }

  .card {
    background: var(--bg-elevated);
    border: 1px solid var(--border);
    border-radius: var(--radius);
    padding: 16px 18px;
    margin-bottom: 16px;
  }

  .card h2 {
    margin: 0 0 12px;
    font-size: 1rem;
  }

  .row {
    display: flex;
    align-items: center;
    gap: 12px;
    flex-wrap: wrap;
    margin-bottom: 8px;
  }

  .row label {
    min-width: 160px;
    color: var(--text-muted);
    font-size: 0.9rem;
  }

  input[type='range'] {
    flex: 1;
    min-width: 180px;
    accent-color: var(--accent);
  }

  input[type='number'] {
    width: 110px;
    background: var(--bg);
    border: 1px solid var(--border-strong);
    border-radius: var(--radius-sm);
    padding: 6px 10px;
  }

  input[type='checkbox'] {
    width: 18px;
    height: 18px;
    accent-color: var(--accent);
  }

  output,
  .row span {
    color: var(--text-dim);
    font-family: var(--font-mono);
    font-size: 0.84rem;
  }

  .hint {
    color: var(--text-dim);
    font-size: 0.82rem;
    margin: 6px 0 0;
  }

  .facts {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(140px, 1fr));
    gap: 10px;
    margin: 0 0 14px;
  }

  .facts div {
    background: var(--bg);
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

  .support {
    list-style: none;
    margin: 0;
    padding: 0;
    display: grid;
    gap: 6px;
  }

  .support li {
    display: flex;
    align-items: center;
    gap: 10px;
    padding: 6px 10px;
    background: var(--bg);
    border: 1px solid var(--border);
    border-radius: var(--radius-sm);
    font-size: 0.86rem;
  }

  .support div {
    display: flex;
    flex-direction: column;
    flex: 1;
  }

  .support div span {
    color: var(--text-dim);
    font-size: 0.76rem;
  }

  .support em {
    font-style: normal;
    color: var(--text-dim);
    font-size: 0.8rem;
  }

  .dot {
    width: 9px;
    height: 9px;
    border-radius: 50%;
    background: var(--text-dim);
    flex: none;
  }

  .dot.ok {
    background: var(--ok);
  }

  .dot.no {
    background: var(--danger);
  }

  .saved {
    color: var(--ok);
    font-size: 0.85rem;
  }
</style>
