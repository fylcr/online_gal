import {
  type GameSource,
  type InputEvent,
  type ProbeContext,
  type ProbeResult,
  type RuntimeHost,
  type RuntimeInstance,
  type RuntimeOptions,
  type RuntimeProvider,
  type SaveState,
} from '@online-gal/engine-api';
import { findUniqueFile } from '@online-gal/vfs';
import { WebRoot, type WebRootOptions } from './root.js';

/**
 * The `postMessage` channel name the injected bootstrap uses.
 * Kept in sync with `WebRoot.bootstrapSource`.
 */
const CHANNEL = '__ogfs__';

/** Engines that ship an HTML5 bundle and therefore run as-is. */
export const WEB_ENGINE_MARKERS = {
  rpgMakerMv: ['www/js/rpg_core.js', 'js/rpg_core.js'],
  rpgMakerMz: ['www/js/rmmz_core.js', 'js/rmmz_core.js'],
  tyrano: ['tyrano/plugins/kag/kag.js', 'tyrano/kag.js', 'data/scenario/first.ks'],
  generic: ['index.html'],
} as const;

export interface WebProbeSummary {
  readonly score: number;
  readonly engine: 'rpgmaker-mv' | 'rpgmaker-mz' | 'tyrano' | 'web';
  readonly entry: string;
  readonly evidence: readonly { path: string; detail: string }[];
}

/**
 * Classifies a directory that contains an HTML5 game.
 *
 * The order matters: RPG Maker MZ shares the `www/` layout with MV and Tyrano
 * ships `.ks` data, so the most specific marker is tested first and the generic
 * `index.html` case scores lowest.
 */
export async function probeWebGame(game: ProbeContext): Promise<WebProbeSummary> {
  const evidence: { path: string; detail: string }[] = [];

  const rootEntries = await game.list('');
  const entry =
    rootEntries.find((candidate) => candidate.toLowerCase() === 'index.html') ??
    'index.html';

  for (const candidate of WEB_ENGINE_MARKERS.rpgMakerMz) {
    if (await game.has(candidate)) {
      evidence.push({ path: candidate, detail: 'RPG Maker MZ core script' });
      return { score: 95, engine: 'rpgmaker-mz', entry, evidence };
    }
  }

  for (const candidate of WEB_ENGINE_MARKERS.rpgMakerMv) {
    if (await game.has(candidate)) {
      evidence.push({ path: candidate, detail: 'RPG Maker MV core script' });
      return { score: 90, engine: 'rpgmaker-mv', entry, evidence };
    }
  }

  for (const candidate of ['js/rmmz_core.js', 'js/rpg_core.js']) {
    if (await game.has(candidate)) {
      evidence.push({ path: candidate, detail: 'RPG Maker core script' });
      return { score: 88, engine: 'rpgmaker-mv', entry, evidence };
    }
  }

  for (const candidate of WEB_ENGINE_MARKERS.tyrano) {
    if (await game.has(candidate)) {
      evidence.push({ path: candidate, detail: 'TyranoBuilder engine file' });
      return { score: 85, engine: 'tyrano', entry, evidence };
    }
  }

  if (await game.has('tyrano/tyrano.js')) {
    evidence.push({ path: 'tyrano/tyrano.js', detail: 'TyranoBuilder engine file' });
    return { score: 85, engine: 'tyrano', entry, evidence };
  }

  // A generic HTML5 game: recognised, but ranked below every native engine so
  // that a KiriKiri title which happens to bundle a help page still wins.
  for (const candidate of ['index.html', 'index.htm', 'game.html']) {
    if (await game.has(candidate)) {
      evidence.push({ path: candidate, detail: 'HTML5 entry document' });
      return { score: 30, engine: 'web', entry: candidate, evidence };
    }
  }

  // Some NW.js bundles keep everything inside `app.asar`.
  if (await game.has('app.asar')) {
    evidence.push({ path: 'app.asar', detail: 'NW.js application bundle' });
    return { score: 35, engine: 'web', entry, evidence };
  }

  if (await game.has('www/index.html')) {
    evidence.push({ path: 'www/index.html', detail: 'nested HTML5 entry document' });
    return { score: 32, engine: 'web', entry: 'www/index.html', evidence };
  }

  return { score: 0, engine: 'web', entry, evidence };
}

export interface WebRuntimeOptions extends WebRootOptions {
  /** Engine label surfaced in the engine panel. */
  readonly engine?: string;
  /** Extra globals injected before the game's own scripts run. */
  readonly prelude?: string;
}

/**
 * Runtime for HTML5 game engines (RPG Maker MV/MZ, TyranoBuilder, generic web
 * games).
 *
 * These engines already provide their own renderer and game loop, so the host
 * does not composite their frames. Instead the game runs inside an iframe that
 * the runtime feeds through a `postMessage` file bridge, and the canvas is left
 * black — the iframe is the presentation surface. This mirrors how
 * Tyranor-Next runs its "built-in Web runtime" titles.
 */
export class WebRuntime implements RuntimeInstance {
  readonly runtimeId: string;

  private readonly root: WebRoot;
  private frame: HTMLIFrameElement | null = null;
  private readonly listener: (event: MessageEvent) => void;
  private loaded = false;
  private fatalError: string | null = null;
  private readonly readCount = { value: 0 };

  constructor(
    readonly provider: WebRuntimeProvider,
    private readonly host: RuntimeHost,
    options: WebRuntimeOptions,
  ) {
    this.runtimeId = `${provider.runtimeId}`;
    this.root = new WebRoot(host.source, {
      entry: options.entry ?? 'index.html',
      shimScripts: options.shimScripts ?? [],
      onMissing: (path) => host.log('debug', `web asset missing: ${path}`),
    });

    this.listener = (event: MessageEvent) => {
      const data = event.data as
        | { channel?: string; kind?: string; id?: number; path?: string }
        | undefined;
      if (!data || data.channel !== CHANNEL || data.kind !== 'read') return;
      if (typeof data.id !== 'number' || typeof data.path !== 'string') return;
      void this.serve(data.id, data.path);
    };
    window.addEventListener('message', this.listener);
  }

  private async serve(id: number, path: string): Promise<void> {
    this.readCount.value += 1;
    try {
      const file = await this.root.resolve(path);
      if (!file) {
        this.host.log('debug', `web runtime: "${path}" not found`);
        this.frame?.contentWindow?.postMessage(
          { channel: CHANNEL, kind: 'data', id, body: null },
          '*',
        );
        return;
      }
      // The body is transferred as an ArrayBuffer so the iframe can wrap it
      // without a base64 round trip.
      const buffer = file.body.buffer.slice(
        file.body.byteOffset,
        file.body.byteOffset + file.body.byteLength,
      );
      this.frame?.contentWindow?.postMessage(
        { channel: CHANNEL, kind: 'data', id, body: buffer },
        '*',
        [buffer],
      );
    } catch (error) {
      this.host.log('warn', `web runtime failed to serve "${path}": ${String(error)}`);
      this.frame?.contentWindow?.postMessage(
        { channel: CHANNEL, kind: 'data', id, body: null },
        '*',
      );
    }
  }

  async load(): Promise<void> {
    const html = await this.root.renderEntry();
    const frame = document.createElement('iframe');
    frame.title = 'game';
    frame.setAttribute('sandbox', 'allow-scripts allow-same-origin allow-pointer-lock allow-popups');
    frame.style.cssText =
      'position:absolute;inset:0;width:100%;height:100%;border:0;background:#000;';
    frame.srcdoc = html;

    const container = this.host.canvas.parentElement ?? document.body;
    const position = getComputedStyle(container).position;
    if (position === 'static') container.style.position = 'relative';
    container.appendChild(frame);
    this.frame = frame;
    this.loaded = true;

    frame.addEventListener('error', () => {
      this.fatalError = 'the game page failed to load';
      this.host.notify({ type: 'fatal', message: this.fatalError });
    });

    this.host.log('info', `embedded web game started (${this.root.entry})`);
  }

  tick(): void {
    // The embedded engine drives its own rendering and timing.
  }

  render(): void {
    const context = this.host.canvas.getContext('2d');
    if (!context) return;
    // The iframe is the visible surface; paint a neutral backdrop behind it.
    if (!this.loaded) {
      context.fillStyle = '#000000';
      context.fillRect(0, 0, this.host.canvas.width, this.host.canvas.height);
    }
  }

  sendInput(): void {
    // Input goes to the iframe through the DOM, so nothing to forward.
  }

  pause(): void {
    this.frame?.contentWindow?.postMessage({ channel: CHANNEL, kind: 'pause' }, '*');
  }

  resume(): void {
    this.frame?.contentWindow?.postMessage({ channel: CHANNEL, kind: 'resume' }, '*');
  }

  save(): SaveState {
    // HTML5 engines keep their own save state; the host records the game
    // identity so a library entry can restore the session it belongs to.
    return {
      version: 1,
      data: { kind: 'web', entry: this.root.entry },
      caption: 'Web engine session',
    };
  }

  async restore(): Promise<void> {
    // Nothing to inject: the embedded engine reads its own save data from the
    // writable overlay the host mounted.
  }

  dispose(): void {
    window.removeEventListener('message', this.listener);
    if (this.frame?.parentElement) this.frame.parentElement.removeChild(this.frame);
    this.frame = null;
    this.root.dispose();
  }

  describe(): Readonly<Record<string, unknown>> {
    return {
      engine: this.provider.displayName,
      entry: this.root.entry,
      loaded: this.loaded,
      filesServed: this.readCount.value,
      error: this.fatalError,
    };
  }
}

/**
 * Provider base class for embedded web runtimes.
 *
 * `runtime-web` and `runtime-tyrano` share this implementation; only the
 * marker set, score and display name differ.
 */
export class WebRuntimeProvider implements RuntimeProvider {
  constructor(
    readonly runtimeId: string,
    readonly displayName: string,
    readonly priority: number,
    private readonly options: {
      readonly description: string;
      readonly accepts: readonly string[];
      readonly engines: readonly WebProbeSummary['engine'][];
      readonly webRoot?: WebRootOptions;
      readonly prelude?: string;
    },
  ) {}

  async probe(game: ProbeContext): Promise<ProbeResult | null> {
    const summary = await probeWebGame(game);
    if (summary.score <= 0) return null;
    if (!this.options.engines.includes(summary.engine)) return null;

    return {
      runtimeId: this.runtimeId,
      displayName: this.displayName,
      score: summary.score,
      // Scores are calibrated so that a bare `index.html` (30) is low
      // confidence — it identifies a web game but not which engine — while a
      // recognised engine banner (85+) is high.
      confidence: summary.score >= 60 ? 'high' : summary.score >= 50 ? 'medium' : 'low',
      startup: summary.entry,
      evidence: summary.evidence.map((entry) => ({
        path: entry.path,
        detail: entry.detail,
        weight: summary.score,
      })),
    };
  }

  create(host: RuntimeHost, options: RuntimeOptions): RuntimeInstance {
    return new WebRuntime(this, host, {
      entry: (options.gameConfig?.entry as string | undefined) ?? this.options.webRoot?.entry ?? 'index.html',
      shimScripts: this.options.webRoot?.shimScripts ?? [],
      ...(this.options.prelude ? { prelude: this.options.prelude } : {}),
    });
  }
}

export const webProvider: RuntimeProvider = new WebRuntimeProvider(
  'web',
  'HTML5 / Web game',
  20,
  {
    description:
      'Runs HTML5 games (including RPG Maker MV/MZ and NW.js bundles) in an isolated frame served from the imported directory.',
    accepts: ['index.html', 'js/rpg_core.js', 'js/rmmz_core.js'],
    engines: ['web', 'rpgmaker-mv', 'rpgmaker-mz'],
  },
);

export type { GameSource };
