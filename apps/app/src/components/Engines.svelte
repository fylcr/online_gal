<script lang="ts">
  import { onMount } from 'svelte';
  import { browserSupport, getPlatform, type BrowserSupport, type GameRecord } from '../lib/platform';
  import { runtimeLabel, unsupportedReason } from '@online-gal/core';

  interface EngineRow {
    runtimeId: string;
    displayName: string;
    priority: number;
    description: string;
    accepts: readonly string[];
    containers: readonly string[];
    gameCount: number;
    playable: boolean;
    reason: string | null;
  }

  let rows = $state<EngineRow[]>([]);
  let support = $state<BrowserSupport | null>(null);
  let games = $state<GameRecord[]>([]);
  let failureCount = $state(0);

  const platform = getPlatform();

  /**
   * Engines that have no registered runtime at all.
   *
   * SiglusEngine and ArtemisEngine are deliberately absent: both now have
   * providers that recognise them and declare themselves unavailable, so they
   * appear in the list above with a real reason instead of a hardcoded note.
   * Ren'Py has no probe either, so it can only be listed as future work.
   */
  const detectedOnly = [
    {
      displayName: "Ren'Py",
      markers: ['.rpa', 'game/script.rpy', 'renpy/'],
      note: "Ren'Py 依赖 Python 运行时，浏览器端需要额外移植，目前也没有识别规则。",
    },
  ];

  onMount(async () => {
    support = browserSupport();
    games = [...(await platform.library.list())];
    failureCount = platform.probeFailures.length;
    rows = platform.registry.list().map((provider) => {
      const reason = unsupportedReason(provider.runtimeId, support!);
      // A provider may declare itself unable to start a game (an engine with no
      // browser build). That is a stronger statement than a browser capability
      // check, so it wins, and it is reported as such rather than as a missing
      // feature of the browser.
      const declaredUnavailable = provider.availability === 'unavailable';
      return {
        runtimeId: provider.runtimeId,
        displayName: provider.displayName,
        priority: provider.priority,
        description: provider.description ?? '',
        accepts: provider.accepts ?? [],
        containers: provider.containers ?? [],
        gameCount: games.filter((game) => game.detection.runtimeId === provider.runtimeId).length,
        playable: reason === null && !declaredUnavailable,
        reason: declaredUnavailable
          ? '该引擎的运行时尚未接入浏览器：可以识别，但无法启动。'
          : reason,
      };
    });
  });
</script>

<section>
  <h1>引擎</h1>
  <p class="lead">
    平台以「运行时提供者（runtime provider）」的方式扩展。每个提供者独立完成引擎识别（probe）与运行
    （create），由调度器按匹配分值、优先级、注册顺序选出唯一运行时——这与
    <a href="https://github.com/AetherKiri/AetherKiri" target="_blank" rel="noreferrer noopener"
      >AetherKiri</a
    >
    的原生运行时调度一致，只是宿主从 Godot 换成了 Canvas 与 iframe。
  </p>

  <h2>已注册运行时</h2>
  <ul class="engines">
    {#each rows as row (row.runtimeId)}
      <li class="engine">
        <header>
          <div>
            <strong>{row.displayName}</strong>
            <code>{row.runtimeId}</code>
          </div>
          <div class="badges">
            <span class="badge">优先级 {row.priority}</span>
            <span class="badge" class:ok={row.playable} class:no={!row.playable}>
              {row.playable ? '可用' : '不可用'}
            </span>
          </div>
        </header>
        {#if row.description}<p>{row.description}</p>{/if}
        {#if row.reason}<p class="warn">{row.reason}</p>{/if}
        <div class="markers">
          {#each row.accepts as marker (marker)}
            <code>{marker}</code>
          {/each}
        </div>
        {#if row.containers.length > 0}
          <div class="markers markers--containers">
            <span>可读取归档：</span>
            {#each row.containers as container (container)}
              <code>{container}</code>
            {/each}
          </div>
        {/if}
        <footer>
          <span>库中游戏 {row.gameCount} 个</span>
          {#if row.gameCount > 0}
            <a href={`/?engine=${row.runtimeId}`}>在游戏库中查看</a>
          {/if}
        </footer>
      </li>
    {/each}
  </ul>

  <h2>仅识别，暂不运行</h2>
  <p class="lead">
    这些引擎的识别证据会被记录在游戏条目上，但浏览器端还没有对应运行时，导入后会明确提示而不是静默失败。
  </p>
  <ul class="engines">
    {#each detectedOnly as engine (engine.displayName)}
      <li class="engine engine--muted">
        <header>
          <div><strong>{engine.displayName}</strong></div>
          <div class="badges"><span class="badge no">未接入</span></div>
        </header>
        <p>{engine.note}</p>
        <div class="markers">
          {#each engine.markers as marker (marker)}
            <code>{marker}</code>
          {/each}
        </div>
      </li>
    {/each}
  </ul>

  <h2>运行环境</h2>
  <dl class="facts">
    <div><dt>浏览器能力</dt><dd>{support?.webAssembly ? '满足基础要求' : '缺少 WebAssembly'}</dd></div>
    <div><dt>引擎探测失败</dt><dd>{failureCount}</dd></div>
    <div><dt>库中已识别游戏</dt><dd>{games.filter((game) => game.detection.runtimeId).length}</dd></div>
    <div>
      <dt>未识别游戏</dt>
      <dd>{games.filter((game) => !game.detection.runtimeId).length}</dd>
    </div>
  </dl>

  {#if games.some((game) => game.detection.runtimeId && game.detection.confidence === 'low')}
    <div class="notice">
      <strong>有游戏是低置信度识别。</strong>
      打开游戏库中的「详情」可以查看识别依据并手动指定引擎。
    </div>
  {/if}

  <h2>目录识别规则</h2>
  <p class="lead">
    识别规则取自
    <a
      href="https://github.com/Weiss-UltimateSavior/Tyranor-Next"
      target="_blank"
      rel="noreferrer noopener">Tyranor-Next</a
    >
    的引擎表，并补充了权重，使同一目录同时命中多个引擎时能稳定地选出最合适的运行时。
  </p>
  <table>
    <thead>
      <tr><th>引擎</th><th>典型特征</th><th>分值</th></tr>
    </thead>
    <tbody>
      <tr><td>KiriKiri / KAG3</td><td>.xp3 包、startup.tjs、Config.tjs、first.ks</td><td>最高 180</td></tr>
      <tr><td>ONScripter</td><td>nscript.dat、nscr_sec.dat、onscript.nt2/nt3、.nsa</td><td>最高 130</td></tr>
      <tr><td>SiglusEngine</td><td>Scene.pck、Gameexe.ini/.dat</td><td>55–130（仅识别）</td></tr>
      <tr><td>ArtemisEngine</td><td>root.pfs、system.ini、system/first.iet、.pfs.NNN 补丁包</td><td>20–155（仅识别）</td></tr>
      <tr><td>TyranoBuilder</td><td>tyrano/、data/scenario/first.ks</td><td>最高 95</td></tr>
      <tr><td>RPG Maker MZ</td><td>js/rmmz_core.js、www/js/rmmz_core.js</td><td>95</td></tr>
      <tr><td>RPG Maker MV</td><td>js/rpg_core.js、www/js/rpg_core.js</td><td>90</td></tr>
      <tr><td>HTML5 / Web</td><td>index.html、app.asar</td><td>30–35</td></tr>
    </tbody>
  </table>
</section>

<style>
  .lead {
    color: var(--text-muted);
    margin-top: -6px;
    max-width: 90ch;
  }

  h2 {
    margin-top: 28px;
  }

  .engines {
    list-style: none;
    margin: 0;
    padding: 0;
    display: grid;
    gap: 12px;
  }

  .engine {
    background: var(--bg-elevated);
    border: 1px solid var(--border);
    border-radius: var(--radius);
    padding: 14px 16px;
  }

  .engine--muted {
    opacity: 0.72;
  }

  .engine header {
    display: flex;
    align-items: center;
    justify-content: space-between;
    gap: 12px;
    flex-wrap: wrap;
  }

  .engine header div {
    display: flex;
    align-items: baseline;
    gap: 10px;
  }

  .engine code {
    color: var(--text-dim);
  }

  .badges {
    display: flex;
    gap: 6px;
  }

  .badge {
    font-size: 0.72rem;
    padding: 2px 9px;
    border-radius: 999px;
    border: 1px solid var(--border-strong);
    color: var(--text-muted);
  }

  .badge.ok {
    border-color: rgba(78, 201, 160, 0.5);
    color: var(--ok);
  }

  .badge.no {
    border-color: rgba(224, 108, 117, 0.5);
    color: var(--danger);
  }

  .engine p {
    color: var(--text-muted);
    font-size: 0.88rem;
    margin: 8px 0;
  }

  .engine p.warn {
    color: var(--warn);
  }

  .markers {
    display: flex;
    flex-wrap: wrap;
    gap: 6px;
    margin-top: 8px;
  }

  .markers code {
    background: var(--bg);
    border: 1px solid var(--border);
    border-radius: 4px;
    padding: 2px 8px;
    font-size: 0.76rem;
    color: var(--text-muted);
  }

  .markers--containers {
    align-items: center;
    font-size: 0.76rem;
    color: var(--text-dim);
  }

  .engine footer {
    margin-top: 10px;
    padding-top: 8px;
    border-top: 1px solid var(--border);
    display: flex;
    justify-content: space-between;
    color: var(--text-dim);
    font-size: 0.8rem;
  }

  .facts {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(160px, 1fr));
    gap: 10px;
    margin: 12px 0 0;
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

  .notice {
    margin-top: 18px;
    border: 1px solid rgba(229, 181, 103, 0.4);
    background: rgba(229, 181, 103, 0.1);
    border-radius: var(--radius-sm);
    padding: 12px 14px;
    font-size: 0.88rem;
  }

  table {
    width: 100%;
    border-collapse: collapse;
    font-size: 0.86rem;
    margin-top: 10px;
  }

  th,
  td {
    text-align: left;
    padding: 8px 10px;
    border-bottom: 1px solid var(--border);
  }

  th {
    color: var(--text-dim);
    font-weight: 600;
  }
</style>
