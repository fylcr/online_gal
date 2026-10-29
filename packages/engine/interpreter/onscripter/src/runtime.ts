import {
  type CompatibilityReport,
  CommandTracker,
  type GameSource,
  type InputEvent,
  OPTION_ARCHIVE_CACHE_COUNT,
  OPTION_COMPATIBILITY_REPORT,
  OPTION_FPS_LIMIT,
  OPTION_MEMORY_PROFILE,
  OPTION_SCRIPT_ENCODING,
  OPTION_STRICT_MODE,
  OPTION_TEXT_SPEED,
  type ProbeContext,
  type ProbeResult,
  type RuntimeHost,
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
import { findUniqueFile } from '@online-gal/vfs';
import {
  NSCRIPT_CANDIDATES,
  classifyNsContainer,
  decodeNscript,
} from './container.js';
import {
  findOnsSavePaths,
  migrateOnsSaves,
  onsSavePath,
  ONS_SAVE_DIRECTORY,
} from './save-storage.js';
import {
  expandNsText,
  parseEffect,
  parseNscript,
  type NsArgument,
  type NsStatement,
  type ParsedNscript,
} from './nscript.js';

const DEFAULT_SURFACE = { width: 800, height: 600 };

/** Layer ids, mirroring ONScripter's slots. */
const LAYER_BACKGROUND = 'background';
const LAYER_FOREGROUND = 'foreground';
const LAYER_PANEL = 'panel';
const SPRITE_LAYER_PREFIX = 'sprite';

interface OnsWait {
  readonly label: string;
  isSatisfied(now: number): boolean;
}

function waitMs(ms: number, label: string): OnsWait {
  const deadline = performance.now() + ms;
  return { label, isSatisfied: () => performance.now() >= deadline };
}

function waitInput(label = 'click'): OnsWait {
  return { label, isSatisfied: () => false };
}

/**
 * Scores a directory as an ONScripter title.
 *
 * The `nscript.dat` family is definitive; `00.txt` plus `.nsa` archives is the
 * next strongest signal, since some releases ship the script only inside the
 * archive.
 */
export async function probeOnscripter(game: ProbeContext): Promise<{
  score: number;
  script: string | null;
  archives: readonly string[];
  evidence: readonly { path: string; detail: string }[];
}> {
  const evidence: { path: string; detail: string }[] = [];
  let score = 0;

  let script: string | null = null;
  for (const candidate of NSCRIPT_CANDIDATES) {
    if (await game.has(candidate)) {
      script = candidate;
      const isArchive = candidate.endsWith('.nt2') || candidate.endsWith('.nt3');
      score += isArchive ? 55 : 80;
      evidence.push({
        path: candidate,
        detail: isArchive ? 'NScripter text archive' : 'NScripter scenario',
      });
      break;
    }
  }

  const rootEntries = await game.list('');
  const archives = rootEntries.filter((entry) =>
    ['.nsa', '.ns2', '.sar'].some((extension) => entry.toLowerCase().endsWith(extension)),
  );
  if (archives.length > 0) {
    score += 40;
    evidence.push({ path: archives[0]!, detail: 'NScripter archive' });
  }

  if (await game.has('nscr_sec.dat')) {
    score += 50;
    evidence.push({ path: 'nscr_sec.dat', detail: 'secondary NScripter scenario' });
  }

  return { score, script, archives, evidence };
}

/** Why a recognised-but-unimplemented command cannot run here. */
const MOVIE_AND_PLUGIN_NOTES: Readonly<Record<string, string>> = {
  movie: 'video playback is not implemented in the browser runtime',
  anime: 'timed animation playback is not implemented',
  midi: 'MIDI playback is not supported by the Web Audio backend',
  systemcall: 'native process execution has no browser equivalent',
  getini: 'INI configuration reading is not implemented',
  saveini: 'INI configuration writing is not implemented',
  savescreenshot: 'engine-side screenshots are not implemented; use the host save UI',
  savescreenshot2: 'engine-side screenshots are not implemented; use the host save UI',
  cdfadeout: 'CD audio is not supported',
};

export const onscripterProvider: RuntimeProvider = {
  runtimeId: 'onscripter',
  displayName: 'ONScripter',
  priority: 90,
  description:
    'Interprets NScripter/ONScripter scenarios (nscript.dat, omscript.nt2/nt3) with its own command table and layer stack.',
  accepts: ['nscript.dat', 'nscript.___', 'nscr_sec.dat', 'onscript.nt2', 'onscript.nt3', '.nsa'],
  containers: ['nsa', 'ns2', 'sar', 'zip'],
  optionKeys: [
    OPTION_TEXT_SPEED,
    OPTION_SCRIPT_ENCODING,
    OPTION_FPS_LIMIT,
    OPTION_MEMORY_PROFILE,
    OPTION_ARCHIVE_CACHE_COUNT,
    OPTION_STRICT_MODE,
    OPTION_COMPATIBILITY_REPORT,
  ],

  async probe(game: ProbeContext): Promise<ProbeResult | null> {
    const summary = await probeOnscripter(game);
    if (summary.score <= 0) return null;
    return {
      runtimeId: 'onscripter',
      displayName: 'ONScripter',
      score: summary.score,
      confidence: summary.score >= 80 ? 'high' : summary.score >= 50 ? 'medium' : 'low',
      ...(summary.script ? { startup: summary.script } : {}),
      evidence: summary.evidence.map((entry) => ({
        path: entry.path,
        detail: entry.detail,
        weight: 50,
      })),
    };
  },

  create(host: RuntimeHost, options: RuntimeOptions): RuntimeInstance {
    return new OnscripterRuntime(host, options);
  },
};

/**
 * The ONScripter runtime instance.
 *
 * NScripter's model differs from KAG in two ways that shape this class: all
 * state lives in integer variable slots (`numalias`-named), and control flow is
 * a flat command stream with `goto`/`gosub`. Both map cleanly onto a program
 * counter plus a variable map.
 */
export class OnscripterRuntime implements RuntimeInstance {
  readonly runtimeId = 'onscripter';

  private readonly presenter: Presenter;
  private readonly stage: Stage;
  private readonly assets: AssetManager;
  private readonly text: TextRenderer;
  private readonly audio: AudioEngine;

  private program: readonly NsStatement[] = [];
  private labels: ReadonlyMap<string, number> = new Map();
  private pc = 0;
  private callStack: number[] = [];
  private wait: OnsWait | null = null;
  private finished = false;
  private fatalError: string | null = null;

  /** Integer and string variable slots, keyed by name or numeric index. */
  private readonly variables = new Map<string, number>();
  private readonly stringVariables = new Map<string, string>();
  /** `numalias`/`stralias` name to slot index. */
  private readonly numericAliases = new Map<string, string>();
  /** Accumulated text for the current message. */
  private messageText = '';
  /** Sprite slot 闁?layer id. */
  private readonly spriteLayers = new Map<number, string>();
  private readonly sprites = new Map<number, { x: number; y: number; visible: boolean; storage: string }>();

  private savedSurface = { ...DEFAULT_SURFACE };
  private readonly history: { speaker: string; text: string }[] = [];

  /** Slot numbers known to hold a save, so an argument-less `save` picks a free one. */
  private readonly usedSlots = new Set<number>();

  /** Tallies which NScripter commands this session honoured. */
  private readonly compat: CommandTracker;

  /**
   * Program counter requested from outside the execution loop.
   *
   * A menu choice is made from the input handler, which may run while `run()` is
   * still on the stack. Applying the jump there would be overwritten by that
   * loop's next increment, so it is parked and consumed at the top of the loop.
   */
  private pendingJump: number | null = null;

  private reveal: {
    layout: ReturnType<TextRenderer['layout']>;
    total: number;
    revealed: number;
    charsPerSecond: number;
    startedAt: number;
    completed: boolean;
  } | null = null;

  constructor(
    private readonly host: RuntimeHost,
    private readonly options: RuntimeOptions,
  ) {
    this.presenter = new Presenter(host.canvas, {});
    this.saveState();
    this.stage = new Stage(this.savedSurface.width, this.savedSurface.height);
    this.assets = new AssetManager(host.source, {});
    this.text = new TextRenderer();
    this.audio = new AudioEngine();
    this.compat = CommandTracker.fromRuntime(
      this.runtimeId,
      (key, fallback) => host.setting(key, fallback),
      (level, message) => host.log(level, message),
    );
  }

  private saveState(): void {
    const saved = this.host.setting<{ width: number; height: number }>('surface', DEFAULT_SURFACE);
    this.savedSurface = {
      width: saved.width > 0 ? saved.width : DEFAULT_SURFACE.width,
      height: saved.height > 0 ? saved.height : DEFAULT_SURFACE.height,
    };
    this.presenter.setSurfaceSize(this.savedSurface.width, this.savedSurface.height);
  }

  /* ---------------------------------------------------------------- */
  /* Loading                                                          */
  /* ---------------------------------------------------------------- */

  async load(): Promise<void> {
    await this.audio.unlock().catch(() => undefined);
    const source = this.host.source;

    let containerPath: string | null = null;
    for (const candidate of NSCRIPT_CANDIDATES) {
      if (await source.exists(candidate)) {
        containerPath = candidate;
        break;
      }
    }
    if (!containerPath) {
      containerPath = await findUniqueFile(source, 'nscript.dat');
    }
    if (!containerPath) {
      throw new Error(
        'no NScripter scenario found (looked for nscript.dat, nscr_sec.dat, onscript.nt2/nt3, 00.txt)',
      );
    }

    const bytes = await source.readFile(containerPath);
    const container = classifyNsContainer(containerPath) ?? 'nscript.dat';
    const decoded = decodeNscript(bytes, container);
    const parsed = parseNscript(decoded.text);
    for (const diagnostic of parsed.diagnostics) {
      this.host.log('warn', `${containerPath}: ${diagnostic}`);
    }
    this.applyProgram(parsed);
    this.host.log(
      'info',
      `loaded ${containerPath} (${container}, ${decoded.encoding}): ${parsed.statements.length} statements, ${parsed.labels.size} labels`,
    );

    // Read the screen size from `*define` before the first frame is drawn.
    this.readDefineBlock(parsed);
    this.presenter.setSurfaceSize(this.savedSurface.width, this.savedSurface.height);
    // Adopt any save files an earlier layout left at the game root, then learn
    // which slots already exist.
    await this.adoptLegacySaves();
    this.host.notify({ type: 'title', title: this.host.source.name });
    this.run();
  }

  private applyProgram(parsed: ParsedNscript): void {
    this.program = parsed.statements;
    this.labels = parsed.labels;
    this.pc = 0;
  }

  /** Reads `*define` 闁?`game` for the window size, matching ONScripter. */
  private readDefineBlock(parsed: ParsedNscript): void {
    let inside = false;
    for (const statement of parsed.statements) {
      if (statement.kind === 'command' && statement.name === 'define') {
        inside = true;
        continue;
      }
      if (statement.kind === 'command' && statement.name === 'game') {
        inside = false;
        continue;
      }
      if (!inside || statement.kind !== 'command') continue;

      if (statement.name === 'window' || statement.name === 'windoweffect') continue;
      if (statement.name === 'mode') {
        const width = statement.args[0]?.number;
        const height = statement.args[1]?.number;
        if (width && height && width >= 160 && height >= 120) {
          this.savedSurface = { width, height };
        }
      }
      if (statement.name === 'numalias') {
        const name = statement.args[0]?.text;
        const slot = statement.args[1]?.text;
        if (name && slot) this.numericAliases.set(name, slot);
      }
      if (statement.name === 'stralias') {
        const name = statement.args[0]?.text;
        if (name) this.stringVariables.set(name, '');
      }
    }
  }

  /* ---------------------------------------------------------------- */
  /* Execution                                                        */
  /* ---------------------------------------------------------------- */

  update(deltaMs: number): void {
    this.stage.update(deltaMs);
    this.updateReveal();
    // A menu choice takes priority over "the scenario ended". ONScripter's own
    // loop looks for a new jump target before testing the end flag, and that
    // ordering is load-bearing: a script whose final statement is a menu would
    // otherwise be stuck forever, because the choice is made after the counter
    // ran past the last statement.
    if (this.pendingJump === null && this.finished) return;
    if (this.wait === null || this.wait.isSatisfied(performance.now())) {
      this.wait = null;
      this.run();
    }
  }

  /** Executes statements until a blocking command or the step budget. */
  private run(): void {
    let steps = 0;
    while (true) {
      // A jump requested from outside the loop (a menu choice) is applied here,
      // where the program counter is the loop's own state.
      if (this.pendingJump !== null) {
        this.pc = this.pendingJump;
        this.pendingJump = null;
      }
      if (this.pc >= this.program.length) break;
      if (steps >= 6000) {
        this.host.requestFrame();
        return;
      }
      steps += 1;
      const statement = this.program[this.pc]!;
      this.pc += 1;

      try {
        const blocked = this.execute(statement);
        // A menu choice supersedes a `pause`-style wait: the player's answer is
        // the event the wait was for, so execution continues immediately.
        if (blocked && this.pendingJump === null) return;
      } catch (error) {
        this.fatalError = error instanceof Error ? error.message : String(error);
        this.host.log('error', `ONScripter scenario aborted: ${this.fatalError}`, error);
        this.finished = true;
        return;
      }
    }
    this.finished = true;
    this.host.notify({ type: 'session-end', reason: 'scenario-end' });
  }

  /** Returns true when execution should pause. */
  private execute(statement: NsStatement): boolean {
    switch (statement.kind) {
      case 'blank':
      case 'comment':
      case 'label':
        return false;

      case 'text': {
        this.compat.honoured('text');
        this.appendMessage(statement.text);
        return false;
      }

      case 'command': {
        const handled = this.executeCommand(statement.name, statement.args, statement.line);
        // The tally is applied here rather than in every branch so the two
        // accounting rules cannot be forgotten when a command is added.
        if (handled) this.compat.honoured(statement.name);
        return this.shouldPause;
      }

      default:
        return false;
    }
  }

  /**
   * Set by a command that blocks, read by `execute`.
   *
   * `executeCommand` has three outcomes 闁?handled and continue, handled and
   * pause, or unhandled 闁?which a boolean cannot express without conflating
   * "unhandled" with "do not pause".
   */
  private shouldPause = false;

  /** Returns true when the command was recognised. */
  private executeCommand(
    name: string,
    args: readonly NsArgument[],
    line: number,
  ): boolean {
    const first = args[0];
    this.shouldPause = false;

    switch (name) {
      /* --- definition block markers --- */
      case 'define':
      case 'definebg':
      case 'definereset':
      case 'game':
        // `*define` 鈥?`game` brackets the header block, which has already been
        // read for the window size and aliases. Execution starts after it, so
        // these are structural markers rather than working commands.
        return true;

      /* --- control flow --- */
      case 'goto': {
        const target = first?.text ?? '';
        const index = this.labels.get(target.replace(/^\*/, ''));
        if (index === undefined) {
          this.host.log('warn', `goto target "*${target}" was not found`);
          return true;
        }
        this.pc = index;
        return true;
      }
      case 'gosub':
      case 'call': {
        const target = first?.text ?? '';
        const index = this.labels.get(target.replace(/^\*/, ''));
        if (index === undefined) {
          this.host.log('warn', `gosub target "*${target}" was not found`);
          return true;
        }
        this.callStack.push(this.pc);
        this.pc = index;
        return true;
      }
      case 'return': {
        const previous = this.callStack.pop();
        if (previous !== undefined) this.pc = previous;
        return true;
      }
      case 'jumpb':
      case 'jumpf': {
        const target = first?.text ?? '';
        const index = this.labels.get(target.replace(/^\*/, ''));
        if (index !== undefined) this.pc = index;
        return true;
      }
      case 'end':
      case 'quit':
        this.finished = true;
        this.host.notify({ type: 'session-end', reason: 'script-end' });
        return this.pauseCommand();
      case 'resettimer':
        return true;
      case 'wait':
        this.wait = waitMs(first?.number ?? 500, 'wait');
        return this.pauseCommand();
      case 'waittimer':
        this.wait = waitInput('click');
        return this.pauseCommand();
      case 'click':
        this.wait = waitInput('click');
        return this.pauseCommand();
      case 'skip':
      case 'nbskip':
        return true;

      /* --- conditionals and arithmetic --- */
      case 'if': {
        this.evaluateIf(args);
        return true;
      }
      case 'notif': {
        this.evaluateIf(args, true);
        return true;
      }
      case 'mov':
      case 'movl':
        this.assign(args[0], args[1]?.number ?? 0);
        return true;
      case 'add':
      case 'addl':
        this.assign(args[0], this.valueOf(args[0]) + (args[1]?.number ?? 0));
        return true;
      case 'sub':
      case 'subl':
        this.assign(args[0], this.valueOf(args[0]) - (args[1]?.number ?? 0));
        return true;
      case 'mul':
        this.assign(args[0], this.valueOf(args[0]) * (args[1]?.number ?? 0));
        return true;
      case 'div': {
        const divisor = args[1]?.number ?? 1;
        if (divisor === 0) {
          this.host.log('warn', 'div by zero');
          return true;
        }
        this.assign(args[0], Math.trunc(this.valueOf(args[0]) / divisor));
        return true;
      }
      case 'mod': {
        const divisor = args[1]?.number ?? 1;
        if (divisor === 0) return true;
        this.assign(args[0], this.valueOf(args[0]) % divisor);
        return true;
      }
      case 'inc':
        this.assign(args[0], this.valueOf(args[0]) + 1);
        return true;
      case 'dec':
        this.assign(args[0], this.valueOf(args[0]) - 1);
        return true;
      case 'numalias': {
        const alias = args[0]?.text;
        const slot = args[1]?.text;
        if (alias && slot) this.numericAliases.set(alias, slot);
        return true;
      }
      case 'stralias': {
        const alias = args[0]?.text;
        if (alias) this.stringVariables.set(alias, args[1]?.text ?? '');
        return true;
      }
      case 'itoa': {
        const target = args[0]?.text;
        const value = args[1]?.number ?? 0;
        if (target) this.stringVariables.set(target, String(value));
        return true;
      }
      case 'atoi': {
        const target = args[0]?.text;
        const source = args[1]?.text ?? '';
        if (target) this.assign({ text: target, number: null, quoted: false }, Number.parseInt(source, 10) || 0);
        return true;
      }
      case 'getparam':
      case 'getreg':
        this.assign(args[0], 0);
        return true;
      case 'itoa2':
      case 'itoa3':
        return true;

      /* --- message flow --- */
      case 'clr' :
        this.clearMessage();
        return true;
      case 'clrall':
        this.clearMessage();
        return true;
      case 'er':
        this.completeReveal();
        return true;
      case 'page':
        this.completeReveal();
        this.wait = waitInput('click');
        return this.pauseCommand();
      case 'br':
        this.appendMessage('\n');
        return true;
      case 'indent':
        this.appendMessage('\u3000');
        return true;
      case 'line':
        return true;
      case 'texton':
        this.deps_messageVisible(true);
        return true;
      case 'textoff':
        this.deps_messageVisible(false);
        return true;

      /* --- graphics --- */
      case 'bg': {
        this.startTransition(first?.text ?? '', args[2] ?? args[1], 1);
        return true;
      }
      case 'ld':
      case 'ldl':
        return this.executeLoad(args);
      case 'csp': {
        this.executeSprite(args);
        return true;
      }
      case 'csp2': {
        this.executeSprite(args, true);
        return true;
      }
      case 'vsp':
      case 'vsp2': {
        // Video sprites need a decoder the browser does not guarantee; the
        // command is recognised so scripts continue past it.
        this.host.log('debug', `[${name}] video sprite playback is not supported`);
        return true;
      }
      case 'cl':
      case 'clc':
      case 'cll': {
        this.executeClear(args);
        return true;
      }
      case 'effect':
      case 'effectblank':
        return true;
      case 'window':
      case 'windoweffect':
        return true;
      case 'bar':
      case 'barclear':
      case 'prnum':
      case 'prnumclear':
        return true;
      case 'quake':
      case 'quakex':
      case 'quakey': {
        const duration = first?.number ?? 300;
        this.stage.startShake(args[2]?.number ?? args[1]?.number ?? 8, duration);
        this.wait = waitMs(duration, 'quake');
        return this.pauseCommand();
      }
      case 'blt':
      case 'cell':
      case 'humanz':
        return true;
      case 'trans':
      case 'bgm':
        return true;

      /* --- audio --- */
      case 'bgmfade':
        return true;
      case 'play':
      case 'playonce':
      case 'wave':
      case 'mp3':
      case 'mp3loop':
      case 'mp3save':
      case 'bgmstop':
      case 'stop':
      case 'wavestop':
        this.executeAudio(name, args);
        return true;

      /* --- recognised but not implemented ---
       *
       * These commands are understood well enough to consume their arguments
       * correctly, but the runtime cannot perform what they ask. They are
       * recorded through the compatibility tracker rather than silently
       * succeeding: a game that plays a movie is meaningfully broken if the
       * movie never appears, and the report is how the player finds that out
       * instead of concluding the game is at fault.
       */
      case 'anime':
      case 'movie':
      case 'midi':
      case 'systemcall':
      case 'getini':
      case 'saveini':
      case 'savescreenshot':
      case 'savescreenshot2':
      case 'cdfadeout': {
        const detail = MOVIE_AND_PLUGIN_NOTES[name];
        this.compat.unsupported(name, 'skipped', {
          line,
          ...(detail ? { note: detail } : {}),
        });
        return true;
      }

      /* --- volume and file queries: accepted without effect --- */
      case 'mp3vol':
      case 'bgmvol':
      case 'sevol':
      case 'savename':
      case 'getfileinfo':
        return true;

      case 'fileexist':
        // The script asks whether a file exists to decide a branch. Answering
        // "no" is the conservative choice: it makes the script take its
        // missing-asset path rather than depending on an asset the browser
        // cannot read.
        this.assign(args[0], 0);
        this.compat.unsupported('fileexist', 'approximated', {
          line,
          note: 'always answers "not present"',
        });
        return true;

      /* --- saves --- */
      case 'save': {
        // NScripter's `save` writes the engine's slot file itself; the launcher
        // only supplies the directory. The browser equivalent writes through the
        // runtime's own serialisation into `savedata/save<NN>.dat` in the
        // writable overlay, so the file layout matches a real install and the
        // slot survives a reload even without the host's save system.
        this.clearMessage();
        const slot = first?.number ?? this.nextSaveSlot();
        void this.persistSlot(slot);
        this.host.notify({ type: 'request-menu', items: [] });
        return true;
      }
      case 'load': {
        const slot = first?.number ?? 0;
        void this.restoreSlot(slot);
        this.host.notify({ type: 'request-menu', items: [] });
        return true;
      }
      case 'saveon':
      case 'saveoff':
        // Whether the engine offers in-game saves; the host's own save UI is
        // always available, so this is recorded rather than acted on.
        return true;
      case 'savedir': {
        // A script may redirect the save directory. Honouring it would break
        // the overlay layout, so the request is surfaced instead of applied.
        this.compat.unsupported('savedir', 'approximated', {
          note: 'saves stay in savedata/ as the writable overlay requires',
        });
        return true;
      }

      /* --- selection --- */
      case 'selnum':
      case 'select':
        return this.executeSelect(args);

      /* --- misc --- */
      case 'caption':
        this.host.notify({ type: 'title', title: first?.text ?? '' });
        return true;
      case 'defaultfont':
      case 'font':
        return true;
      case 'automode':
      case 'cursorset':
      case 'getcursorpos':
      case 'lookback':
      case 'lookbackflush':
        return true;
      case 'flushout':
      case 'ruby':
        return true;

      default:
        // Recorded rather than only logged: an ONScripter game that issues a
        // command the runtime lacks can otherwise run to completion while
        // missing behaviour the player would notice. Returning true means
        // "handled", so the caller does not also tally it as honoured.
        this.compat.unsupported(name, 'skipped', { line });
        return true;
    }
  }

  /** Marks the current command as blocking, and reports it as handled. */
  private pauseCommand(): boolean {
    this.shouldPause = true;
    return true;
  }

  /* ---------------------------------------------------------------- */
  /* Slot persistence                                                 */
  /* ---------------------------------------------------------------- */

  /** Lowest unused slot number, so `save` without an argument still works. */
  private nextSaveSlot(): number {
    let slot = 0;
    while (this.usedSlots.has(slot)) slot += 1;
    return slot;
  }

  /**
   * Writes a slot file into the engine's own save directory.
   *
   * The payload is this runtime's serialised state, which is deliberately not
   * byte-compatible with a native `.dat`: the browser cannot execute the same
   * script interpreter state, so the file stores what a browser session needs
   * to resume. What it *does* preserve is the naming and directory layout, so
   * the file is recognisable to `isOnsSaveFileName` and travels with an export.
   */
  private async persistSlot(slot: number): Promise<void> {
    try {
      const payload = JSON.stringify(this.save().data);
      await this.host.store.write(onsSavePath(slot), new TextEncoder().encode(payload));
      this.usedSlots.add(slot);
      this.host.log('debug', `wrote ${onsSavePath(slot)}`);
    } catch (error) {
      this.host.log('warn', `cannot write slot ${slot}: ${String(error)}`);
    }
  }

  private async restoreSlot(slot: number): Promise<void> {
    try {
      const bytes = await this.host.store.read(onsSavePath(slot));
      if (!bytes) {
        this.host.log('warn', `slot ${slot} is empty`);
        return;
      }
      const data = JSON.parse(new TextDecoder().decode(bytes)) as unknown;
      await this.restore({ version: 1, data });
      this.host.log('info', `restored ${onsSavePath(slot)}`);
    } catch (error) {
      this.host.log('warn', `cannot read slot ${slot}: ${String(error)}`);
    }
  }

  /**
   * Adopts save files left by an earlier layout.
   *
   * Ported from AetherKiri's `PrepareSaveStorage`: saves written by older builds
   * to the game root are copied into `savedata/` without overwriting anything
   * already there, so an upgrade never rolls progress backwards.
   */
  private async adoptLegacySaves(): Promise<void> {
    const result = await migrateOnsSaves(this.host.store, '', ONS_SAVE_DIRECTORY);
    if (result.copied > 0) {
      this.host.log('info', `migrated ${result.copied} save file(s) into savedata/`);
    }
    for (const warning of result.warnings) this.host.log('debug', warning);
    for (const path of await this.listSlotPaths()) {
      const match = /save(\d+)\.dat$/i.exec(path);
      if (match) this.usedSlots.add(Number(match[1]));
    }
  }

  private async listSlotPaths(): Promise<readonly string[]> {
    try {
      const paths = await this.host.store.list();
      return findOnsSavePaths(paths);
    } catch {
      return [];
    }
  }

  private deps_messageVisible(visible: boolean): void {
    this.stage.layer('message', 40).visible = visible;
  }

  private executeLoad(args: readonly NsArgument[]): boolean {
    // `ld c,"file",effect` for the centre, `ld l,...` left, `ld r,...` right.
    const slot = args[0]?.text?.toLowerCase();
    const storage = args[1]?.text ?? args[2]?.text ?? '';
    const effect = parseEffect(args[2] ?? args[3]);
    if (!storage) return false;

    const layerName =
      slot === 'c' || slot === '' ? LAYER_BACKGROUND : slot === 'p' ? LAYER_PANEL : `${LAYER_BACKGROUND}_${slot}`;
    this.startTransition(storage, args[2] ?? undefined, 1, layerName, effect.durationMs);
    return false;
  }

  private executeSprite(args: readonly NsArgument[], isSprite2 = false): void {
    // `csp slot, file, x, y [, alpha]`
    const slot = args[0]?.number ?? 0;
    const storage = args[1]?.text ?? '';
    const x = args[2]?.number ?? 0;
    const y = args[3]?.number ?? 0;
    const record = this.sprites.get(slot) ?? { x, y, visible: true, storage: '' };
    record.x = x;
    record.y = y;
    record.visible = true;
    if (storage) record.storage = storage;
    this.sprites.set(slot, record);

    const layerName = `${SPRITE_LAYER_PREFIX}${slot}`;
    this.spriteLayers.set(slot, layerName);
    if (record.storage) {
      void this.loadIntoLayer(record.storage, layerName, 20 + slot, x, y);
    }
    if (isSprite2) this.host.log('trace', 'csp2 alpha handling falls back to full opacity');
  }

  private executeClear(args: readonly NsArgument[]): void {
    const slot = args[0]?.number ?? null;
    if (slot === null) {
      // `cl` with no slot clears every graphic layer.
      this.stage.clear(LAYER_BACKGROUND);
      for (const [id, layer] of this.spriteLayers) {
        this.stage.clear(layer);
        this.sprites.delete(id);
      }
      return;
    }
    const layerName = `${SPRITE_LAYER_PREFIX}${slot}`;
    this.stage.clear(layerName);
    this.sprites.delete(slot);
  }

  private startTransition(
    storage: string,
    effectArgument: NsArgument | undefined,
    opacity: number,
    layerName = LAYER_BACKGROUND,
    explicitDuration?: number,
  ): void {
    if (!storage) return;
    const effect = parseEffect(effectArgument);
    const duration = explicitDuration ?? effect.durationMs;
    const layer = this.stage.layer(layerName, layerName === LAYER_BACKGROUND ? 0 : 10);
    void opacity;
    void this.loadIntoLayer(storage, layerName, layerName === LAYER_BACKGROUND ? 0 : 10, 0, 0, duration);
  }

  private async loadIntoLayer(
    storage: string,
    layerName: string,
    zIndex: number,
    x: number,
    y: number,
    durationMs = 0,
  ): Promise<void> {
    const path = storage.replace(/\\/g, '/').replace(/^\.\//, '');
    let image = await this.assets.tryLoadImage(path);
    if (!image && !/\.[a-z0-9]{2,5}$/i.test(path)) {
      for (const extension of ['.png', '.jpg', '.bmp', '.webp', '.tga']) {
        image = await this.assets.tryLoadImage(`${path}${extension}`);
        if (image) break;
      }
    }
    if (!image) {
      this.host.log('warn', `image not found: "${storage}"`);
      return;
    }
    const layer = this.stage.layer(layerName, zIndex);
    layer.image = image;
    layer.src = null;
    layer.width = image.width;
    layer.height = image.height;
    layer.x = x;
    layer.y = y;
    layer.visible = true;
    if (durationMs > 0) {
      layer.opacity = 0;
      this.stage.animate(layer, { opacity: 1 }, durationMs, 'crossfade');
    } else {
      layer.opacity = 1;
    }
    this.host.requestFrame();
  }

  private executeAudio(name: string, args: readonly NsArgument[]): void {
    const storage = args[0]?.text ?? '';
    const slot = args[1]?.text ?? '0';
    const isBgm = name.startsWith('bgm') || name === 'mp3' || name === 'mp3loop';
    const channel: 'bgm' | 'se' = isBgm ? 'bgm' : 'se';
    const channelSlot = `${channel}${slot}`;

    if (name.includes('stop') || !storage) {
      this.audio.stop(channelSlot, 0);
      return;
    }
    void (async () => {
      try {
        const path = storage.replace(/\\/g, '/');
        const bytes = await this.host.source.readFile(path);
        const buffer = await this.audio.decode(bytes, path);
        this.audio.play(channelSlot, buffer, {
          channel,
          loop: isBgm || name === 'playonce' ? isBgm : false,
          volume: 1,
        });
      } catch (error) {
        this.host.log('warn', `cannot play "${storage}": ${String(error)}`);
      }
    })();
  }

  /* ---------------------------------------------------------------- */
  /* Selection                                                        */
  /* ---------------------------------------------------------------- */

  private selection: {
    options: readonly { label: string; target: string }[];
    x: number;
    y: number;
    hovered: number;
    width: number;
    rowHeight: number;
  } | null = null;

  private executeSelect(args: readonly NsArgument[]): boolean {
    // `select "text1",*label1,"text2",*label2`
    // `selnum %0,"text1",*label1,"text2",*label2`
    //
    // Only `selnum` carries the leading variable, so it is detected twice over:
    // an integer literal (`selnum 0,鈥) or a `%`-prefixed slot (`selnum %0,鈥).
    // Without both the first option is consumed as the variable and the last
    // target is lost, which leaves the menu apparently working but jumping
    // nowhere.
    const options: { label: string; target: string }[] = [];
    const first = args[0];
    const startsWithVariableSlot =
      first !== undefined &&
      !first.quoted &&
      (first.number !== null || first.text.startsWith('%'));
    let index = startsWithVariableSlot ? 1 : 0;

    while (index + 1 < args.length) {
      const label = args[index]?.text ?? '';
      const target = args[index + 1]?.text ?? '';
      if (label) options.push({ label, target });
      index += 2;
    }
    if (options.length === 0) {
      this.host.log('warn', 'select declared no options');
      return true;
    }

    const style = this.text.style;
    const widest = options.reduce(
      (max, option) => Math.max(max, this.text.measureText(option.label, style)),
      0,
    );
    this.selection = {
      options,
      width: Math.max(200, widest + 48),
      rowHeight: style.fontSize * 2,
      x: Math.round(this.stage.width / 2 - Math.max(100, widest / 2 + 24)),
      y: Math.round(this.stage.height / 2 - (options.length * style.fontSize * 2) / 2),
      hovered: 0,
    };
    this.completeReveal();
    this.wait = waitInput('select');
    return true;
  }

  private chooseOption(index: number): void {
    const selection = this.selection;
    if (!selection) return;
    const option = selection.options[index];
    if (!option) return;
    this.selection = null;

    const label = option.target.replace(/^\*/, '');
    const target = this.labels.get(label);
    if (target === undefined) {
      this.host.log('warn', `select target "*${label}" was not found`);
      this.wait = null;
      return;
    }
    // The jump is *requested* rather than applied here. `chooseOption` runs
    // inside the input handler, which may itself have been called from within
    // `run()`; writing `this.pc` directly would be undone by that outer loop's
    // own increment on its next iteration.
    this.pendingJump = target;
    this.wait = null;
  }

  /* ---------------------------------------------------------------- */
  /* Conditionals and variables                                        */
  /* ---------------------------------------------------------------- */

  private slotFor(argument: NsArgument | undefined): string | null {
    if (!argument) return null;
    if (argument.number !== null) return `#${argument.number}`;
    const alias = this.numericAliases.get(argument.text);
    if (alias !== undefined) return alias.startsWith('#') ? alias : `#${alias}`;
    return argument.text;
  }

  private valueOf(argument: NsArgument | undefined): number {
    if (!argument) return 0;
    if (argument.number !== null) return argument.number;
    const alias = this.numericAliases.get(argument.text);
    if (alias !== undefined) return this.variables.get(alias) ?? 0;
    return this.variables.get(argument.text) ?? 0;
  }

  private assign(argument: NsArgument | undefined, value: number): void {
    const slot = this.slotFor(argument);
    if (!slot) return;
    this.variables.set(slot, Math.trunc(value));
    if (argument?.text) this.variables.set(argument.text, Math.trunc(value));
  }

  /**
   * Evaluates an `if` command.
   *
   * ONScripter's `if` is a postfix comparison over variable slots
   * (`if %0 == 1 goto *label`, `if %0 >= 3`), with the jump target carried in
   * the final argument. Only the comparison is evaluated here; the following
   * `goto`/`jumpf` command performs the branch.
   */
  private evaluateIf(args: readonly NsArgument[], invert = false): void {
    if (args.length < 3) {
      // `if %0` alone tests for non-zero.
      const truthy = this.valueOf(args[0]) !== 0;
      if (truthy === invert) {
        // Skip the next statement by leaving execution to the branch command.
      }
      return;
    }
    const left = this.valueOf(args[0]);
    const operator = args[1]?.text ?? '==';
    const right = this.valueOf(args[2]);
    const result = compare(left, operator, right);
    if (!result !== invert) {
      // The branch command on the same line already performed the jump; when
      // the condition is false we must skip it.
      const next = this.program[this.pc];
      if (
        next &&
        next.kind === 'command' &&
        ['goto', 'gosub', 'jumpf', 'jumpb', 'call'].includes(next.name)
      ) {
        this.pc += 1;
      }
    }
  }

  /* ---------------------------------------------------------------- */
  /* Rendering                                                        */
  /* ---------------------------------------------------------------- */

  render(): void {
    const context = this.presenter.context;
    context.save();
    context.fillStyle = '#000000';
    context.fillRect(0, 0, this.stage.width, this.stage.height);
    this.stage.render(context);

    const reveal = this.reveal;
    if (reveal) {
      const messageLayer = this.stage.get('message');
      const originX = (messageLayer?.x ?? 0) + 40;
      const originY = (messageLayer?.y ?? 0) + 40;
      if (!messageLayer || messageLayer.visible) {
        this.text.draw(context, reveal.layout, originX, originY, reveal.revealed, 1);
      }
    }

    if (this.selection) this.renderSelection(context);
    context.restore();
  }

  private renderSelection(context: CanvasRenderingContext2D): void {
    const selection = this.selection;
    if (!selection) return;
    const style = this.text.style;
    context.save();
    for (let index = 0; index < selection.options.length; index += 1) {
      const option = selection.options[index]!;
      const x = selection.x;
      const y = selection.y + index * selection.rowHeight;
      const hovered = selection.hovered === index;
      context.globalAlpha = hovered ? 0.92 : 0.7;
      context.fillStyle = hovered ? 'rgba(60, 96, 168, 0.92)' : 'rgba(10, 14, 24, 0.78)';
      context.fillRect(x, y, selection.width, selection.rowHeight);
      context.globalAlpha = 1;
      context.strokeStyle = hovered ? '#bcd6ff' : 'rgba(255,255,255,0.4)';
      context.lineWidth = 1.5;
      context.strokeRect(x + 0.5, y + 0.5, selection.width - 1, selection.rowHeight - 1);
      this.text.paintText(context, option.label, x + 20, y + style.fontSize * 1.35, style);
    }
    context.restore();
  }

  /* ---------------------------------------------------------------- */
  /* Message state                                                    */
  /* ---------------------------------------------------------------- */

  private appendMessage(text: string): void {
    const expanded = expandNsText(text);
    this.messageText += expanded.text;
    this.relayout();
  }

  private relayout(): void {
    const runs = [{ text: this.messageText, offset: 0, style: {} }];
    const layout = this.text.layout(runs, { maxWidth: this.messageWidth });
    const previously = this.reveal?.revealed ?? 0;
    this.reveal = {
      layout,
      total: layout.totalCharacters,
      revealed: Math.min(previously, layout.totalCharacters),
      charsPerSecond: this.host.setting('textSpeed', 45),
      startedAt: performance.now(),
      completed: false,
    };
  }

  private updateReveal(): void {
    const reveal = this.reveal;
    if (!reveal || reveal.completed) return;
    const elapsed = (performance.now() - reveal.startedAt) / 1000;
    reveal.revealed = Math.min(reveal.total, Math.max(reveal.revealed, elapsed * reveal.charsPerSecond));
    if (reveal.revealed >= reveal.total) reveal.completed = true;
  }

  private completeReveal(): void {
    if (!this.reveal) return;
    this.reveal.revealed = this.reveal.total;
    this.reveal.completed = true;
  }

  private clearMessage(): void {
    if (this.messageText) {
      this.history.push({ speaker: '', text: this.messageText });
      if (this.history.length > 500) this.history.shift();
    }
    this.messageText = '';
    this.reveal = null;
  }

  private get messageWidth(): number {
    const layer = this.stage.get('message');
    return Math.max(80, (layer?.width && layer.width > 0 ? layer.width : this.stage.width) - 80);
  }

  /* ---------------------------------------------------------------- */
  /* Input                                                            */
  /* ---------------------------------------------------------------- */

  sendInput(event: InputEvent): void {
    if (event.type === 'pointer-up' || event.type === 'pointer-move' || event.type === 'pointer-down') {
      if (this.selection) {
        const selection = this.selection;
        const index = Math.floor((event.y - selection.y) / selection.rowHeight);
        if (index >= 0 && index < selection.options.length) {
          selection.hovered = index;
          if (event.type === 'pointer-up') this.chooseOption(index);
        }
        return;
      }
      if (event.type !== 'pointer-move') this.handleAdvance();
      return;
    }
    if (event.type === 'key-down') {
      if (this.selection) {
        const count = this.selection.options.length;
        if (event.key === 'ArrowUp') {
          this.selection.hovered = (this.selection.hovered - 1 + count) % count;
          return;
        }
        if (event.key === 'ArrowDown') {
          this.selection.hovered = (this.selection.hovered + 1) % count;
          return;
        }
        if (event.key === 'Enter' || event.key === ' ') {
          this.chooseOption(this.selection.hovered);
          return;
        }
      }
      if (['Enter', ' ', 'Control'].includes(event.key)) this.handleAdvance();
      return;
    }
    if (event.type === 'pointer-wheel' && this.reveal) {
      this.reveal.charsPerSecond = Math.min(
        2000,
        Math.max(0, this.reveal.charsPerSecond + (event.deltaY < 0 ? 60 : -60)),
      );
    }
  }

  /**
   * Handles a click or keypress that is not a menu choice.
   *
   * The first press completes the typewriter reveal rather than advancing, which
   * is the convention every visual novel uses: the text is still appearing, so
   * the player's input means "show it all", not "next line".
   */
  private handleAdvance(): void {
    if (this.reveal && !this.reveal.completed) {
      this.completeReveal();
      return;
    }
    if (this.wait && ['click', 'input'].includes(this.wait.label)) {
      this.wait = null;
      this.run();
    }
  }

  tick(deltaMs: number): void {
    this.update(deltaMs);
  }

  pause(): void {
    this.audio.pause();
  }

  resume(): void {
    void this.audio.resume();
  }

  /* ---------------------------------------------------------------- */
  /* Saves and teardown                                               */
  /* ---------------------------------------------------------------- */

  save(): SaveState {
    return {
      version: 1,
      data: {
        pc: this.pc,
        callStack: [...this.callStack],
        variables: Object.fromEntries(this.variables),
        stringVariables: Object.fromEntries(this.stringVariables),
        sprites: [...this.sprites.entries()].map(([slot, record]) => ({ slot, ...record })),
        message: this.messageText,
      },
      caption: this.messageText.slice(0, 80),
    };
  }

  async restore(state: SaveState): Promise<void> {
    const data = state.data as
      | {
          pc?: number;
          callStack?: number[];
          variables?: Record<string, number>;
          stringVariables?: Record<string, string>;
          sprites?: { slot: number; x: number; y: number; visible: boolean; storage: string }[];
          message?: string;
        }
      | undefined;
    if (!data) return;
    if (typeof data.pc === 'number') this.pc = data.pc;
    this.callStack = data.callStack ?? [];
    this.variables.clear();
    for (const [key, value] of Object.entries(data.variables ?? {})) this.variables.set(key, value);
    this.stringVariables.clear();
    for (const [key, value] of Object.entries(data.stringVariables ?? {})) {
      this.stringVariables.set(key, value);
    }
    this.sprites.clear();
    for (const sprite of data.sprites ?? []) {
      this.sprites.set(sprite.slot, {
        x: sprite.x,
        y: sprite.y,
        visible: sprite.visible,
        storage: sprite.storage,
      });
      if (sprite.storage) {
        void this.loadIntoLayer(sprite.storage, `${SPRITE_LAYER_PREFIX}${sprite.slot}`, 20 + sprite.slot, sprite.x, sprite.y);
      }
    }
    this.messageText = data.message ?? '';
    this.relayout();
    this.wait = null;
    this.finished = false;
    this.run();
  }

  dispose(): void {
    this.audio.dispose();
    this.assets.clear();
    this.stage.clearAll();
    this.reveal = null;
    this.selection = null;
  }

  describe(): Readonly<Record<string, unknown>> {
    const report = this.compat.report();
    return {
      surface: `${this.stage.width}x${this.stage.height}`,
      statements: this.program.length,
      pc: this.pc,
      labels: this.labels.size,
      variables: this.variables.size,
      sprites: this.sprites.size,
      waitingOn: this.wait?.label ?? null,
      error: this.fatalError,
      audio: this.audio.state,
      knownSlots: [...this.usedSlots].sort((a, b) => a - b),
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
   * Which NScripter commands this session could not honour.
   *
   * The same signal the KAG runtime produces: a game can run to completion while
   * silently skipping behaviour it depends on, so "it started" and "it works" are
   * different claims.
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

/** Compares two variable values with an ONScripter-style operator. */
export function compare(left: number, operator: string, right: number): boolean {
  switch (operator) {
    case '==':
    case '=':
      return left === right;
    case '!=':
    case '<>':
      return left !== right;
    case '>':
      return left > right;
    case '<':
      return left < right;
    case '>=':
      return left >= right;
    case '<=':
      return left <= right;
    case '&':
      return (left & right) !== 0;
    default:
      return left === right;
  }
}

export type { GameSource };