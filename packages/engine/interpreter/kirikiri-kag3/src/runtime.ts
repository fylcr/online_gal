import {
  CommandTracker,
  DefaultProbeContext,
  evidence,
  OPTION_ARCHIVE_CACHE_COUNT,
  OPTION_COMPATIBILITY_REPORT,
  OPTION_FPS_LIMIT,
  OPTION_MEMORY_PROFILE,
  OPTION_SCRIPT_ENCODING,
  OPTION_STRICT_MODE,
  OPTION_TEXT_SPEED,
  OPTION_XP3_KEY,
  type CompatibilityReport,
  type GameSource,
  type InputEvent,
  type LogRecord,
  type ProbeContext,
  type ProbeResult,
  type RuntimeHost,
  type RuntimeHostEvent,
  type RuntimeInstance,
  type RuntimeOptions,
  type RuntimeProvider,
  type SaveState,
} from '@online-gal/engine-api';
import {
  AssetManager,
  AudioEngine,
  Presenter,
  Stage,
  TextRenderer,
  type TextStyle,
} from '@online-gal/player-host';
import { findUniqueFile, type CompositeGameSource } from '@online-gal/vfs';
import { compiledScriptKey, loadScriptBytes } from './compiled.js';

/**
 * True when a payload is TJS2 *bytecode* rather than decoded text.
 *
 * The check is inlined rather than imported from `@online-gal/tjs2-bytecode`,
 * because that package depends on this one for the script-container decoder —
 * importing back would be a cycle. The test is a stable eight-byte signature
 * (`"TJS2"` plus the `"100"` version tag), so duplicating it costs nothing and
 * keeps the dependency graph acyclic.
 */
function isCompiledTjs2Bytecode(bytes: Uint8Array): boolean {
  if (bytes.byteLength < 8) return false;
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  // 'T' | 'J'<<8 | 'S'<<16 | '2'<<24, then '1' | '0'<<8 | '0'<<16.
  return view.getUint32(0, true) === 0x32534a54 && view.getUint32(4, true) === 0x00303031;
}
import { parseKagScript, type KagStatement, type KagTag } from './script.js';
import { KagScene, type SceneRequest } from './scene.js';
import { TjsInterpreter, type TjsHost } from './tjs/interpreter.js';
import type { Environment, TjsValue } from './tjs/values.js';

/** Files that mark a directory as a KiriKiri/KAG3 game. */
export const KIRIKIRI_MARKERS = {
  archives: ['.xp3'],
  scripts: ['startup.tjs', 'first.ks', 'config.tjs'],
} as const;

const STARTUP_CANDIDATES = [
  'startup.tjs',
  'Startup.tjs',
  'main.tjs',
  'Main.tjs',
];

const SCENARIO_CANDIDATES = [
  'first.ks',
  'First.ks',
  'first.KS',
  'scenario/first.ks',
  'scenario/First.ks',
];

/** Default surface when the game does not declare one. */
const DEFAULT_SURFACE = { width: 1280, height: 720 };

export interface KagProbeSummary {
  readonly score: number;
  readonly evidence: readonly { path: string; detail: string }[];
  readonly startupScript: string | null;
  readonly scenario: string | null;
  readonly archives: readonly string[];
}

/**
 * Scores a directory as a KiriKiri/KAG3 title.
 *
 * The signals mirror what the native engines look for: an `.xp3` package is the
 * strongest indicator, followed by the TJS boot scripts and the `first.ks`
 * scenario entry point. Scoring is additive with explicit weights, so a game
 * that only ships loose files still ranks above "unknown".
 */
export async function probeKirikiri(game: ProbeContext): Promise<KagProbeSummary> {
  const evidenceList: { path: string; detail: string; weight: number }[] = [];
  let score = 0;

  const add = (path: string, detail: string, weight: number): void => {
    evidenceList.push({ path, detail, weight });
    score += weight;
  };

  // Package files: the strongest single signal.
  const rootEntries = await game.list('');
  for (const entry of rootEntries) {
    const lower = entry.toLowerCase();
    if (lower.endsWith('.xp3')) {
      add(entry, 'KiriKiri XP3 package', 70);
      break;
    }
  }

  let startupScript: string | null = null;
  for (const candidate of STARTUP_CANDIDATES) {
    if (await game.has(candidate)) {
      startupScript = candidate;
      add(candidate, 'TJS boot script', 60);
      break;
    }
  }

  for (const candidate of ['Config.tjs', 'config.tjs']) {
    if (await game.has(candidate)) {
      add(candidate, 'KAG configuration script', 35);
      break;
    }
  }

  let scenario: string | null = null;
  for (const candidate of SCENARIO_CANDIDATES) {
    if (await game.has(candidate)) {
      scenario = candidate;
      add(candidate, 'KAG scenario entry point', 50);
      break;
    }
  }

  // `System.tjs` accompanies every KAG3 template.
  if (await game.has('System.tjs')) add('System.tjs', 'KAG3 system library', 30);
  if (await game.has('system/Initialize.tjs')) {
    add('system/Initialize.tjs', 'KAG3 initialisation library', 25);
  }

  // A compiled scenario anywhere in the tree still identifies the engine.
  if (score === 0 && (await game.hasSuffix('.ks'))) {
    add('*.ks', 'KAG scenario files', 25);
  }
  if (score > 0 && score < 60 && (await game.hasSuffix('.tjs'))) {
    add('*.tjs', 'TJS scripts', 20);
  }

  return {
    score,
    evidence: evidenceList.map(({ path, detail }) => ({ path, detail })),
    startupScript,
    scenario,
    archives: rootEntries.filter((entry) => entry.toLowerCase().endsWith('.xp3')),
  };
}

/** Registers KiriKiri/KAG3 support with the runtime registry. */
export const kirikiriProvider: RuntimeProvider = {
  runtimeId: 'krkr2',
  displayName: 'KiriKiri / KAG3',
  priority: 100,
  description:
    'Interprets KAG3 scenarios with an embedded TJS2 engine, reading loose files and XP3 packages directly from the browser.',
  accepts: ['.xp3', 'startup.tjs', 'Config.tjs', 'first.ks', 'System.tjs'],
  containers: ['xp3', 'zip', 'asar'],
  optionKeys: [
    OPTION_TEXT_SPEED,
    OPTION_XP3_KEY,
    OPTION_SCRIPT_ENCODING,
    OPTION_FPS_LIMIT,
    OPTION_MEMORY_PROFILE,
    OPTION_ARCHIVE_CACHE_COUNT,
    OPTION_STRICT_MODE,
    OPTION_COMPATIBILITY_REPORT,
  ],

  async probe(game: ProbeContext): Promise<ProbeResult | null> {
    const summary = await probeKirikiri(game);
    if (summary.score <= 0) return null;
    return {
      runtimeId: 'krkr2',
      displayName: 'KiriKiri / KAG3',
      score: summary.score,
      confidence: summary.score >= 100 ? 'high' : summary.score >= 50 ? 'medium' : 'low',
      ...(summary.scenario ? { startup: summary.scenario } : {}),
      evidence: summary.evidence.map((entry) => {
        const weight = entry.detail.includes('XP3')
          ? 70
          : entry.detail.includes('scenario')
            ? 50
            : 30;
        return evidence(entry.path, entry.detail, weight);
      }),
    };
  },

  create(host: RuntimeHost, options: RuntimeOptions): RuntimeInstance {
    return new KagRuntime(host, options);
  },
};

interface KagConfig {
  surface: { width: number; height: number };
  title: string | null;
  /** Default text speed in characters per second. */
  textSpeed: number;
  encoding: readonly string[];
}

/**
 * The KiriKiri runtime instance.
 *
 * Owns the participant objects a KAG script expects — `kag`, `System`,
 * `Storages`, `Scripts` — the TJS interpreter, the scene, and the audio bridge.
 */
export class KagRuntime implements RuntimeInstance {
  readonly runtimeId = 'krkr2';

  private readonly presenter: Presenter;
  private readonly stage: Stage;
  private readonly assets: AssetManager;
  private readonly text: TextRenderer;
  private readonly audio: AudioEngine;
  /**
   * Tallies which KAG tags this session honoured.
   *
   * Built from the runtime options so `compatibility_report=false` disables the
   * accounting entirely, and `strict_mode=true` escalates an unsupported tag
   * from a warning to an error instead of letting it pass unnoticed.
   */
  private readonly compat: CommandTracker;
  private scene: KagScene | null = null;
  private interpreter: TjsInterpreter | null = null;
  private config: KagConfig = {
    surface: { ...DEFAULT_SURFACE },
    title: null,
    textSpeed: 45,
    encoding: ['utf-8', 'shift_jis', 'gbk'],
  };
  private scenarioPath = 'first.ks';
  private readonly logs: LogRecord[] = [];
  private loadedScripts = new Map<
    string,
    {
      path: string;
      statements: readonly KagStatement[];
      labels: ReadonlyMap<string, number>;
    }
  >();
  /** Compiled-script XOR keys discovered while booting, reused for XP3. */
  private discoveredKeys: bigint[] = [];

  constructor(
    private readonly host: RuntimeHost,
    private readonly options: RuntimeOptions,
  ) {
    this.presenter = new Presenter(host.canvas, {
      onLog: (record) => this.logs.push(record),
      onEvent: (event) => this.handleHostEvent(event),
    });
    this.presenter.setSurfaceSize(
      options.surface.width || DEFAULT_SURFACE.width,
      options.surface.height || DEFAULT_SURFACE.height,
    );
    this.stage = new Stage(
      this.presenter.surfaceSize.width,
      this.presenter.surfaceSize.height,
    );
    this.assets = new AssetManager(host.source, {
      onMissing: (path) => host.log('debug', `asset missing: ${path}`),
    });
    this.text = new TextRenderer();
    this.audio = new AudioEngine();
    this.compat = CommandTracker.fromRuntime(
      this.runtimeId,
      (key, fallback) => host.setting(key, fallback),
      (level, message) => host.log(level, message),
    );
    for (const channel of ['bgm', 'se', 'voice', 'system'] as const) {
      const volume = host.setting<number>(`volume.${channel}`, -1);
      if (volume >= 0) this.audio.setVolume(channel, volume);
    }
  }

  /* ---------------------------------------------------------------- */
  /* Loading                                                          */
  /* ---------------------------------------------------------------- */

  async load(): Promise<void> {
    this.host.log('info', 'loading KiriKiri game');
    await this.audio.unlock().catch(() => undefined);

    await this.readConfig();
    this.presenter.setSurfaceSize(this.config.surface.width, this.config.surface.height);
    this.stage.setCamera(0, 0, 1);

    this.interpreter = new TjsInterpreter(this.createTjsHost());
    this.installKiriKiriObjects(this.interpreter);

    // `Config.tjs` and `System.tjs` populate the runtime objects before the
    // scenario starts, matching the native boot order.
    await this.runBootScript('Config.tjs');
    await this.runBootScript('System.tjs');

    const scenario = await this.resolveScenario();
    this.scenarioPath = scenario;
    const loaded = await this.loadScript(scenario);
    if (!loaded) {
      // `resolveScenario` picked this path from the directory listing, so a
      // failure here means the file vanished or is unreadable — not that the
      // name was ambiguous. Say which, rather than starting an empty scene.
      throw new Error(
        `the scenario "${scenario}" could not be read, though it was found while probing the game`,
      );
    }

    this.scene = new KagScene(
      {
        host: this.host,
        stage: this.stage,
        assets: this.assets,
        text: this.text,
        compat: this.compat,
        loadScript: (path) => this.loadScript(path),
        evaluateExpression: (source) => this.evaluateExpression(source),
        request: (request) => this.handleSceneRequest(request),
        onTitle: (title) => {
          this.config.title = title;
          this.host.notify({ type: 'title', title });
        },
      },
      loaded.statements,
      loaded.labels,
    );
    this.scene.setCurrentStorage(scenario);
    this.scene.start();
    this.host.notify({ type: 'title', title: this.config.title ?? 'KiriKiri' });
    this.host.log('info', `scenario "${scenario}" started`);
  }

  private async readConfig(): Promise<void> {
    const bytes = await this.tryRead('Config.tjs');
    if (!bytes) {
      this.host.log('debug', 'no Config.tjs found; using defaults');
      return;
    }
    const decoded = await loadScriptBytes(bytes, 'Config.tjs');
    this.config.encoding = [decoded.encoding, 'shift_jis', 'gbk'];

    // Read the scalar settings with tolerant regexes rather than executing the
    // file: it references engine objects that do not exist in a browser. The
    // file is still executed later so scripts get their globals.
    const text = decoded.text;
    const width = matchNumber(text, /(?:ScrWidth|ScreenWidth|WindowWidth)\s*=\s*(\d+)/);
    const height = matchNumber(text, /(?:ScrHeight|ScreenHeight|WindowHeight)\s*=\s*(\d+)/);
    if (width && height) {
      this.config.surface = { width, height };
    } else {
      const size = text.match(/ScreenSize\s*=\s*"(\d+)\s*[x*,\s]\s*(\d+)"/);
      if (size) {
        this.config.surface = { width: Number(size[1]), height: Number(size[2]) };
      }
    }
    const title = text.match(/WindowTitle\s*=\s*"([^"]*)"/);
    if (title) this.config.title = title[1] ?? null;
    const speed = matchNumber(text, /(?:TextSpeed|CharsPerSecond)\s*=\s*(\d+)/);
    if (speed) this.config.textSpeed = speed;
  }

  private async runBootScript(path: string): Promise<void> {
    const bytes = await this.tryRead(path);
    if (!bytes) return;
    try {
      const decoded = await loadScriptBytes(bytes, path);
      const key = compiledScriptKey(bytes);
      if (key !== null) this.discoveredKeys.push(key);
      this.interpreter?.run(decoded.text, path);
      this.host.log('debug', `executed boot script ${path} (${decoded.encoding})`);
    } catch (error) {
      // A boot script referencing unavailable engine internals is expected and
      // must not abort the session; KAG tolerates it in the native engine too.
      this.host.log('warn', `boot script ${path} failed: ${String(error)}`);
    }
  }

  private async resolveScenario(): Promise<string> {
    for (const candidate of SCENARIO_CANDIDATES) {
      if (await this.host.source.exists(candidate)) return candidate;
    }
    // Fall back to any `.ks` in the root, or one found anywhere in the tree.
    const rootEntries = await this.host.source.list('');
    const local = rootEntries.find(
      (entry) => entry.toLowerCase().endsWith('.ks') && !entry.endsWith('/'),
    );
    if (local) return local;
    const found = await findUniqueFile(this.host.source, 'first.ks');
    if (found) return found;
    throw new Error(
      'this KiriKiri game ships no .ks scenario in a readable package; ' +
        'encrypted XP3 archives cannot be opened without the game key',
    );
  }

  private async tryRead(path: string): Promise<Uint8Array | null> {
    try {
      if (!(await this.host.source.exists(path))) return null;
      return await this.host.source.readFile(path);
    } catch (error) {
      this.host.log('debug', `cannot read ${path}: ${String(error)}`);
      return null;
    }
  }

  /**
   * Loads and parses a `.ks` scenario, expanding macros and conditionals.
   *
   * Returns null when the path does not exist, so a caller trying several
   * candidate locations can treat a miss as an ordinary outcome. A *malformed*
   * script still throws: silently treating an unreadable scenario as absent
   * would hide a real problem behind a "not found" branch.
   */
  private async loadScript(path: string): Promise<{
    path: string;
    statements: readonly KagStatement[];
    labels: ReadonlyMap<string, number>;
  } | null> {
    const cached = this.loadedScripts.get(path);
    if (cached) return cached;

    let bytes: Uint8Array;
    try {
      bytes = await this.host.source.readFile(path);
    } catch (error) {
      this.host.log('debug', `script "${path}" is not present: ${String(error)}`);
      return null;
    }

    // A compiled `.ks` carries either the original KAG source or TJS2 *bytecode*
    // behind the same container header, and the two are indistinguishable once
    // decoded as text. Running bytecode through the text parser would not fail:
    // the bytes decode to something, the tag scanner finds no `[tag]` it
    // recognises, and the scenario silently plays as an empty script. The check
    // is therefore made against the raw bytes, before any text decoding.
    if (isCompiledTjs2Bytecode(bytes)) {
      throw new Error(
        `"${path}" is compiled TJS2 bytecode, which this runtime cannot execute ` +
          '(it interprets TJS2 source and has no bytecode VM). ' +
          'The scenario would otherwise load as an empty script.',
      );
    }

    const decoded = await loadScriptBytes(bytes, path);
    const key = compiledScriptKey(bytes);
    if (key !== null) this.discoveredKeys.push(key);

    const parsed = parseKagScript(decoded.text);
    for (const diagnostic of parsed.diagnostics) {
      this.host.log('warn', `${path}: ${diagnostic}`);
    }
    const expanded = expandMacros(parsed.statements);
    const result = {
      path,
      statements: expanded.statements,
      labels: rebuildLabels(expanded.statements),
    };
    this.loadedScripts.set(path, result);
    this.host.log(
      'debug',
      `loaded ${path}: ${result.statements.length} statements, ${result.labels.size} labels (${decoded.encoding})`,
    );
    return result;
  }

  /* ---------------------------------------------------------------- */
  /* Host services                                                    */
  /* ---------------------------------------------------------------- */

  private createTjsHost(): TjsHost {
    return {
      globals: {
        window: globalThis,
        console: {
          log: (...args: TjsValue[]) => this.host.log('info', args.map(String).join(' ')),
        },
      },
      trace: (message) => this.host.log('debug', message),
    };
  }

  private evaluateExpression(source: string): unknown {
    if (!this.interpreter) return undefined;
    const scope: Environment = this.interpreter.global.child();
    // Expose the KAG maps so `%[f.flag]` and `%[sf.value]` resolve.
    scope.declare('f', Object.fromEntries(this.scene?.flags ?? []));
    scope.declare('sf', Object.fromEntries(this.scene?.systemFlags ?? []));
    // `mp` is KAG's per-message scratch map and is routinely read before anything
    // writes to it (`@if exp="mp.storage != f.bg"` is the idiomatic guard at the
    // top of a transition macro). Declaring it as `undefined` makes such a read
    // throw "cannot read property of void", which aborts the enclosing `@if` and
    // silently drops the branch — the branch that initialises the plugin.
    scope.declare('mp', { ...(this.scene?.macroParameters ?? {}) });
    // KAG3 scripts reach for the TJS global object by name: `snow.ks` opens with
    // `typeof(global.snow_object) == 'undefined'` to decide whether it has already
    // registered itself. Without `global` that expression throws, the guard
    // misfires, and the plugin never initialises.
    scope.declare('global', this.interpreter.globalObject());
    const value = this.interpreter.evaluateExpression(this.rewriteVariableSyntax(source), scope);
    // Write back any changes the expression made to `f`/`sf`.
    if (this.scene) {
      const flags = scope.get('f');
      if (flags && typeof flags === 'object') {
        for (const [key, entry] of Object.entries(flags as Record<string, unknown>)) {
          this.scene.flags.set(key, entry);
        }
      }
      const systemFlags = scope.get('sf');
      if (systemFlags && typeof systemFlags === 'object') {
        for (const [key, entry] of Object.entries(systemFlags as Record<string, unknown>)) {
          this.scene.systemFlags.set(key, entry);
        }
      }
      const macroParameters = scope.get('mp');
      if (macroParameters && typeof macroParameters === 'object') {
        for (const [key, entry] of Object.entries(macroParameters as Record<string, unknown>)) {
          this.scene.macroParameters.set(key, entry);
        }
      }
    }
    return value;
  }

  /**
   * Rewrites KAG's variable shorthands into the object form TJS2 evaluates.
   *
   * KAG exposes `f.flag` and `sf.flag` as properties of the `f`/`sf` objects,
   * which the interpreter resolves directly, so the only rewriting needed is
   * `%[...]` stripping and KAG's `tf.` alias for `f.`.
   */
  private rewriteVariableSyntax(source: string): string {
    return source.replace(/\btf\./g, 'f.').trim();
  }

  /**
   * Installs the objects a KAG script expects.
   *
   * These are the interoperability surface: a scenario that calls
   * `kag.getTitle()` or `System.getKeyState()` must work, so every method has a
   * browser-appropriate implementation rather than a stub that throws.
   */
  private installKiriKiriObjects(interpreter: TjsInterpreter): void {
    const scene = () => this.scene;
    const self = this;

    const kag = {
      getTitle: () => self.config.title ?? 'KiriKiri',
      setTitle: (title: string) => {
        self.config.title = title;
        self.host.notify({ type: 'title', title });
      },
      getLayer: (name: string) => {
        const layer = self.stage.layer(name);
        return {
          x: layer.x,
          y: layer.y,
          width: layer.width,
          height: layer.height,
          opacity: Math.round(layer.opacity * 255),
          visible: layer.visible,
        };
      },
      setLayerPosition: (name: string, x: number, y: number) => {
        const layer = self.stage.layer(name);
        layer.x = x;
        layer.y = y;
      },
      setLayerVisible: (name: string, visible: boolean) => {
        self.stage.layer(name).visible = visible;
      },
      getLayerVisible: (name: string) => self.stage.layer(name).visible,
      getLayerOpacity: (name: string) => Math.round(self.stage.layer(name).opacity * 255),
      setLayerOpacity: (name: string, opacity: number) => {
        self.stage.layer(name).opacity = Math.max(0, Math.min(1, opacity / 255));
      },
      getMessageText: () => scene()?.currentMessage.text ?? '',
      clearHistory: () => undefined,
      getHistory: () => scene()?.backlog ?? [],
      getSurfaceSize: () => ({ width: self.stage.width, height: self.stage.height }),
      getScreenWidth: () => self.stage.width,
      getScreenHeight: () => self.stage.height,
      getVariables: () => ({}),
      pushVariables: () => undefined,
      popVariables: () => undefined,
      setSkipMode: () => undefined,
      cancelSkip: () => undefined,
      getSkipMode: () => false,
      trigger: () => undefined,
      getCurrentLayer: () => 'message0',
      getLayerAlias: (name: string) => name,
    };

    const variablesProxy = () => {
      const target: Record<string, TjsValue> = {};
      for (const [key, value] of scene()?.flags ?? []) target[key] = value;
      return target;
    };

    const system = {
      getKeyState: () => false,
      getMousePosition: () => ({ x: 0, y: 0 }),
      getMouseButtonState: () => false,
      getTime: () => Date.now(),
      getTickCount: () => performance.now(),
      getVersionString: () => 'online-gal 0.1.0',
      getVersionInfo: () => 'online-gal 0.1.0 (browser)',
      inform: (message: string) => self.host.log('info', String(message)),
      exit: () => self.host.notify({ type: 'session-end', reason: 'script-exit' }),
      createAppPath: () => '/',
      getAppPath: () => '/',
      getDataPath: () => '/',
      readReg: () => undefined,
      writeReg: () => undefined,
      addContinuousHandler: () => undefined,
      removeContinuousHandler: () => undefined,
      doCompact: () => undefined,
      onActivate: () => undefined,
      setMouseCursor: () => undefined,
    };

    const storages = {
      isExistentStorage: async (path: string) => this.sourceExists(path),
      getFileList: async (dir: string) => this.host.source.list(dir),
      extractFile: async (path: string) => this.readTextFile(path),
      readFile: async (path: string) => this.readTextFile(path),
      getLocalName: (path: string) => path.replace(/^.*[\\/]/, ''),
      getExtension: (path: string) => {
        const match = /\.[^./\\]+$/.exec(path);
        return match ? match[0] : '';
      },
      getStorageName: (path: string) => path.replace(/\.[^.]*$/, ''),
    };

    // `Storages` methods are asynchronous in the browser, but KAG scripts call
    // them synchronously. The cache is primed at load time so the common calls
    // (`isExistentStorage`) can answer from memory.
    const storagesSync = {
      ...storages,
      isExistentStorage: (path: string) => this.existenceCache.get(path) ?? false,
    };

    const scripts = {
      eval: (source: string) => this.evaluateExpression(source),
      exec: (source: string) => interpreter.run(source, '<Scripts.exec>'),
      evalStorage: async (path: string) => {
        const bytes = await this.tryRead(path);
        if (!bytes) return undefined;
        const decoded = await loadScriptBytes(bytes, path);
        return interpreter.run(decoded.text, path);
      },
      getClass: () => undefined,
    };

    const timers: number[] = [];
    const setTimer = (callback: TjsValue, delayMs: number): number => {
      const id = setTimeout(() => {
        try {
          interpreter.callValue(callback, [], undefined, false);
        } catch (error) {
          self.host.log('warn', `timer callback failed: ${String(error)}`);
        }
      }, Math.max(0, delayMs)) as unknown as number;
      timers.push(id);
      return timers.length - 1;
    };
    const clearTimer = (id: number): void => {
      const handle = timers[id];
      if (handle !== undefined) clearTimeout(handle);
    };
    this.timerHandles = timers;

    interpreter.define('kag', kag);
    interpreter.define('System', system);
    interpreter.define('Storages', storagesSync);
    interpreter.define('Scripts', scripts);
    interpreter.define('setTimeout', setTimer);
    interpreter.define('clearTimeout', clearTimer);
    interpreter.define('kagVariables', variablesProxy);
    interpreter.define('Dictionary', class Dictionary {
      private readonly entries = new Map<string, TjsValue>();
      set(key: string, value: TjsValue): void {
        this.entries.set(key, value);
      }
      get(key: string): TjsValue {
        return this.entries.get(key);
      }
      has(key: string): boolean {
        return this.entries.has(key);
      }
      remove(key: string): void {
        this.entries.delete(key);
      }
      clear(): void {
        this.entries.clear();
      }
      get count(): number {
        return this.entries.size;
      }
    });
  }

  private timerHandles: number[] = [];
  /** Prime-time cache so synchronous `isExistentStorage` calls can answer. */
  private existenceCache = new Map<string, boolean>();

  private async sourceExists(path: string): Promise<boolean> {
    const cached = this.existenceCache.get(path);
    if (cached !== undefined) return cached;
    const exists = await this.host.source.exists(path);
    this.existenceCache.set(path, exists);
    return exists;
  }

  private async readTextFile(path: string): Promise<string> {
    const bytes = await this.tryRead(path);
    if (!bytes) return '';
    const decoded = await loadScriptBytes(bytes, path);
    return decoded.text;
  }

  private handleHostEvent(event: RuntimeHostEvent): void {
    if (event.type === 'fatal') {
      this.host.log('error', event.message);
    }
  }

  /** Executes the requests the scene raises for host-owned services. */
  private handleSceneRequest(request: SceneRequest): void {
    switch (request.kind) {
      case 'audio-play':
        void this.playAudio(request);
        break;
      case 'audio-stop':
        this.audio.stop(request.slot, request.fadeMs);
        break;
      case 'session-end':
        this.host.notify({ type: 'session-end', reason: request.reason });
        break;
      case 'save-request':
      case 'load-request':
        // Save and load are driven by the launcher's own UI; the runtime only
        // reports that the script asked, so the shell can open its dialog.
        this.host.notify({ type: 'request-menu', items: [] });
        this.host.log('debug', `${request.kind} slot ${request.slot}`);
        break;
      default:
        break;
    }
  }

  private async playAudio(request: Extract<SceneRequest, { kind: 'audio-play' }>): Promise<void> {
    try {
      const bytes = await this.host.source.readFile(request.storage);
      const buffer = await this.audio.decode(bytes, request.storage);
      this.audio.play(request.slot, buffer, {
        loop: request.loop,
        volume: request.volume,
        channel: request.channel,
      });
      if (request.fadeMs > 0 && request.channel === 'bgm') {
        this.audio.fade(request.slot, request.volume, request.fadeMs);
      }
    } catch (error) {
      // Audio codecs vary; a missing or undecodable track must not stop the game.
      this.host.log('warn', `cannot play "${request.storage}": ${String(error)}`);
    }
  }

  /* ---------------------------------------------------------------- */
  /* Frame loop                                                       */
  /* ---------------------------------------------------------------- */

  tick(deltaMs: number): void {
    if (!this.scene) return;
    this.scene.update(deltaMs);
  }

  render(): void {
    const context = this.presenter.context;
    context.save();
    context.fillStyle = '#000000';
    context.fillRect(0, 0, this.stage.width, this.stage.height);
    if (this.scene) {
      // The stage draws every layer, then the message text goes on top so that
      // message-layer transforms (a sliding box) can still be applied later.
      this.scene.render(context);
    }
    context.restore();
  }

  pause(): void {
    this.audio.pause();
  }

  resume(): void {
    void this.audio.resume();
  }

  sendInput(event: InputEvent): void {
    const scene = this.scene;
    if (!scene) return;
    switch (event.type) {
      case 'pointer-down':
        scene.handlePointer(event.x, event.y, 'down');
        break;
      case 'pointer-up':
        scene.handlePointer(event.x, event.y, 'up');
        break;
      case 'pointer-move':
        scene.handlePointer(event.x, event.y, 'move');
        break;
      case 'pointer-wheel':
        scene.handleWheel(event.deltaY);
        break;
      case 'key-down':
        scene.handleKey(event.key, 'down');
        break;
      case 'key-up':
        scene.handleKey(event.key, 'up');
        break;
      default:
        break;
    }
  }

  /* ---------------------------------------------------------------- */
  /* Saves                                                            */
  /* ---------------------------------------------------------------- */

  save(): SaveState {
    return {
      version: 1,
      data: {
        scenario: this.scenarioPath,
        scene: this.scene?.snapshot() ?? null,
      },
      ...(this.scene
        ? { caption: this.scene.currentMessage.text.slice(0, 80) }
        : {}),
    };
  }

  async restore(state: SaveState): Promise<void> {
    const data = state.data as
      | { scenario?: string; scene?: Parameters<KagScene['restoreSnapshot']>[0] }
      | undefined;
    if (!this.scene || !data?.scene) return;
    if (data.scenario && data.scenario !== this.scenarioPath) {
      const loaded = await this.loadScript(data.scenario);
      if (!loaded) {
        // The slot names a script this install no longer has. Restoring the
        // snapshot alone would leave the program counter pointing into a script
        // that is not loaded, so refuse rather than resume somewhere arbitrary.
        throw new Error(
          `the save refers to scenario "${data.scenario}", which is not present in this game`,
        );
      }
      this.scenarioPath = data.scenario;
      await this.scene.replaceProgramForRestore(loaded.statements, loaded.labels);
    }
    this.scene.restoreSnapshot(data.scene);
    this.scene.setCurrentStorage(data.scenario ?? this.scenarioPath);
    this.host.log('info', 'session restored from save state');
  }

  dispose(): void {
    for (const handle of this.timerHandles) clearTimeout(handle);
    this.timerHandles = [];
    this.audio.dispose();
    this.assets.clear();
    this.stage.clearAll();
    this.scene = null;
    this.interpreter = null;
  }

  describe(): Readonly<Record<string, unknown>> {
    const report = this.compat.report();
    return {
      scenario: this.scenarioPath,
      surface: this.stage.width + 'x' + this.stage.height,
      title: this.config.title,
      encoding: this.config.encoding.join('/'),
      audio: this.audio.state,
      cachedImages: this.assets.cachedImageCount,
      cachedImageBytes: this.assets.cachedBytes,
      discoveredXp3Keys: this.discoveredKeys.length,
      scene: this.scene?.describe() ?? null,
      scripts: this.loadedScripts.size,
      // Surfaced inline so the engine panel shows compatibility at a glance;
      // `report()` carries the per-command detail.
      compatibility: report
        ? {
            severity: report.severity,
            unsupportedCommands: report.unsupported.length,
            approximatedCommands: report.approximated.length,
            commandsHonoured: report.supportedCommandCount,
          }
        : null,
    };
  }

  /**
   * Which KAG tags this session could not honour.
   *
   * A game can run to completion while silently skipping commands it needs, so
   * this is the difference between "it started" and "it works".
   */
  report(): CompatibilityReport {
    return (
      this.compat.report() ?? {
        runtimeId: this.runtimeId,
        severity: 'clean',
        unsupported: [],
        approximated: [],
        supportedCommandCount: 0,
      }
    );
  }
}

function matchNumber(text: string, pattern: RegExp): number | null {
  const match = pattern.exec(text);
  if (!match) return null;
  const value = Number(match[1]);
  return Number.isFinite(value) ? value : null;
}

/* ------------------------------------------------------------------ */
/* Macro and conditional expansion                                     */
/* ------------------------------------------------------------------ */

interface MacroDefinition {
  readonly name: string;
  readonly params: readonly string[];
  readonly body: readonly KagStatement[];
}

/**
 * Expands `[macro]`/`[endmacro]` definitions and prunes `[if]` branches.
 *
 * KAG evaluates conditions at run time, but a scenario's structure (`[if]`
 * blocks, macro definitions) is knowable statically. Doing it here keeps the
 * hot interpreter loop free of block bookkeeping and lets macros be inlined
 * with their parameters substituted textually, exactly as the native engine
 * does when it loads a scenario.
 */
export function expandMacros(statements: readonly KagStatement[]): {
  statements: readonly KagStatement[];
  macros: ReadonlyMap<string, MacroDefinition>;
} {
  const macros = new Map<string, MacroDefinition>();
  const outside: KagStatement[] = [];

  for (let index = 0; index < statements.length; index += 1) {
    const statement = statements[index]!;
    if (statement.kind !== 'tag' || statement.tag.name.toLowerCase() !== 'macro') {
      outside.push(statement);
      continue;
    }

    const name = statement.tag.attrs.get('name') ?? '';
    const attributeNames = [...statement.tag.attrs.keys()].filter((key) => key !== 'name');
    const body: KagStatement[] = [];
    let depth = 1;
    index += 1;
    for (; index < statements.length; index += 1) {
      const candidate = statements[index]!;
      if (candidate.kind === 'tag') {
        const tagName = candidate.tag.name.toLowerCase();
        if (tagName === 'macro') depth += 1;
        if (tagName === 'endmacro') {
          depth -= 1;
          if (depth === 0) break;
        }
      }
      body.push(candidate);
    }
    if (name) macros.set(name, { name, params: attributeNames, body });
  }

  const expanded = inlineMacros(outside, macros, 0);
  return { statements: pruneConditionals(expanded), macros };
}

/** Replaces macro calls with their body, substituting `&param` references. */
function inlineMacros(
  statements: readonly KagStatement[],
  macros: ReadonlyMap<string, MacroDefinition>,
  depth: number,
): readonly KagStatement[] {
  if (depth > 16) return statements;
  const out: KagStatement[] = [];
  for (const statement of statements) {
    if (statement.kind !== 'tag') {
      out.push(statement);
      continue;
    }
    const definition = macros.get(statement.tag.name);
    if (!definition) {
      out.push(statement);
      continue;
    }
    const substituted: KagStatement[] = [];
    for (const bodyStatement of definition.body) {
      if (bodyStatement.kind === 'text') {
        substituted.push({
          kind: 'text',
          text: substituteText(bodyStatement.text, definition.params, statement.tag),
          line: statement.tag.line,
        });
      } else if (bodyStatement.kind === 'tag') {
        substituted.push({
          kind: 'tag',
          line: statement.tag.line,
          tag: substituteTag(bodyStatement.tag, definition.params, statement.tag),
        });
      } else {
        substituted.push(bodyStatement);
      }
    }
    out.push(...inlineMacros(substituted, macros, depth + 1));
  }
  return out;
}

function substituteText(
  text: string,
  params: readonly string[],
  call: KagTag,
): string {
  let result = text;
  for (const param of params) {
    const value = call.attrs.get(param) ?? '';
    result = result.split(`&${param}`).join(value);
  }
  return result;
}

function substituteTag(
  tag: KagTag,
  params: readonly string[],
  call: KagTag,
): KagTag {
  const attrs = new Map<string, string>();
  for (const [key, value] of tag.attrs) {
    let resolved = value;
    for (const param of params) {
      const argument = call.attrs.get(param) ?? '';
      resolved = resolved.split(`&${param}`).join(argument);
    }
    attrs.set(key, resolved);
  }
  return { name: tag.name, attrs, raw: tag.raw, line: call.line };
}

/**
 * Removes the statements inside `[if]` groups while preserving the group
 * structure itself.
 *
 * KAG conditions are evaluated at run time (they read `f.`/`sf.` flags the
 * scenario sets as it goes), so no branch can be chosen at load time. What can
 * be removed is the branch *bodies*: the interpreter evaluates the condition
 * when it reaches an `[if]`, and if it is false it scans forward for the
 * matching `[elseif]`/`[else]`/`[endif]`, so those statements never execute.
 *
 * The block markers themselves must survive — including nested groups — or the
 * interpreter's brace counter would lose track of depth when the outer
 * condition is true and it steps into the nested region. Markers are therefore
 * always emitted, and only non-marker statements inside a group are dropped.
 */
export function pruneConditionals(
  statements: readonly KagStatement[],
): readonly KagStatement[] {
  const markers = new Set(['if', 'elseif', 'else', 'endif']);
  const out: KagStatement[] = [];
  let depth = 0;

  for (const statement of statements) {
    if (statement.kind === 'tag') {
      const name = statement.tag.name.toLowerCase();
      if (name === 'if') {
        depth += 1;
        out.push(statement);
        continue;
      }
      if (name === 'endif') {
        depth = Math.max(0, depth - 1);
        out.push(statement);
        continue;
      }
      if (markers.has(name)) {
        out.push(statement);
        continue;
      }
      // A command inside a conditional group would only run when that group is
      // taken, which the interpreter decides; the group's body is dropped here
      // because the interpreter re-reads it from the block it skips to.
      if (depth > 0) continue;
      out.push(statement);
      continue;
    }

    if (depth === 0) out.push(statement);
  }

  return out;
}

/** Recomputes the label index after macro expansion changes indices. */
export function rebuildLabels(
  statements: readonly KagStatement[],
): ReadonlyMap<string, number> {
  const labels = new Map<string, number>();
  for (let index = 0; index < statements.length; index += 1) {
    const statement = statements[index]!;
    if (statement.kind === 'label' && !labels.has(statement.name)) {
      labels.set(statement.name, index);
    }
  }
  return labels;
}

/** Convenience probe helper for tests and diagnostics. */
export async function probeSource(source: GameSource): Promise<KagProbeSummary> {
  return probeKirikiri(new DefaultProbeContext(source));
}

export type { CompositeGameSource };
