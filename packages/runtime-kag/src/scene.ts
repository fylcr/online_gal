import {
  CommandTracker,
  type RuntimeHost,
} from '@online-gal/engine-api';
import {
  type AssetManager,
  type Stage,
  TextRenderer,
  type TextStyle,
  parseInlineRuns,
} from '@online-gal/player-host';
import {
  type KagStatement,
  type KagTag,
  parseKagBoolean,
  parseKagNumber,
  resolveStoragePath,
} from './script.js';

/**
 * A blocking condition the scenario is currently suspended on.
 *
 * The interpreter is a generator, so blocking commands read as straight-line
 * code, but the host frame loop still owns the clock. Every suspension is
 * expressed as one of these predicates and re-tested once per tick.
 */
export interface WaitCondition {
  readonly label: string;
  isSatisfied(now: number): boolean;
}

export function waitForMs(durationMs: number, label = 'wait'): WaitCondition {
  const deadline = performance.now() + durationMs;
  return { label, isSatisfied: () => performance.now() >= deadline };
}

/** A wait satisfied only by player input. */
export function waitForInput(label = 'input'): WaitCondition {
  return { label, isSatisfied: () => false };
}

export function combineWaits(
  conditions: readonly WaitCondition[],
  label = 'wait',
): WaitCondition {
  return {
    label,
    isSatisfied: (now) => conditions.every((condition) => condition.isSatisfied(now)),
  };
}

/** KAG layer names, in the stack order the native engine uses. */
export const KAG_LAYERS = {
  base: 'base',
  layer0: 'layer0',
  layer1: 'layer1',
  layer2: 'layer2',
  message0: 'message0',
  message1: 'message1',
  message2: 'message2',
  message3: 'message3',
  fore: 'fore',
} as const;

/** Requests the scene emits to the runtime for host-owned services. */
export type SceneRequest =
  | {
      readonly kind: 'audio-play';
      readonly slot: string;
      readonly channel: 'bgm' | 'se' | 'voice';
      readonly storage: string;
      readonly loop: boolean;
      readonly volume: number;
      readonly fadeMs: number;
    }
  | { readonly kind: 'audio-stop'; readonly slot: string; readonly fadeMs: number }
  | { readonly kind: 'save-request'; readonly slot: number }
  | { readonly kind: 'load-request'; readonly slot: number }
  | { readonly kind: 'session-end'; readonly reason: string };

/** One recorded line of dialogue, for the backlog. */
export interface HistoryEntry {
  readonly speaker: string;
  readonly text: string;
}

export interface KagSceneDeps {
  readonly host: RuntimeHost;
  readonly stage: Stage;
  readonly assets: AssetManager;
  readonly text: TextRenderer;
  /** Tallies honoured and unsupported tags for the compatibility report. */
  readonly compat: CommandTracker;
  /**
   * Loads another script, or returns null when the path does not exist.
   *
   * Null rather than a throw, because a storage name written without a directory
   * is legitimately ambiguous — the same name resolves to different paths in
   * different titles — so callers try several candidates and a miss is expected
   * rather than exceptional.
   */
  readonly loadScript: (
    path: string,
  ) => Promise<{
    path: string;
    statements: readonly KagStatement[];
    labels: ReadonlyMap<string, number>;
  } | null>;
  /** Evaluates a TJS expression from a `%[...]` attribute or `[eval]`. */
  readonly evaluateExpression: (source: string) => unknown;
  /** Receives audio, save/load and lifecycle requests. */
  readonly request: (request: SceneRequest) => void;
  /** Called when a `[title]` tag sets the window title. */
  readonly onTitle?: (title: string) => void;
}

interface SelectOption {
  readonly label: string;
  readonly target: string;
}

/**
 * Executes one KAG3 scenario.
 *
 * Responsibilities are deliberately narrow: read the parsed statement list,
 * mutate the stage, emit requests for services the host owns (audio, saves),
 * and suspend on blocking commands. Everything visual goes through `Stage`;
 * everything time-related goes through the host frame loop.
 */
export class KagScene {
  private program: readonly KagStatement[];
  private labels: ReadonlyMap<string, number>;
  private pc = 0;
  private generator: Generator<WaitCondition | null, void, void> | null = null;
  private pendingWait: WaitCondition | null = null;
  /**
   * Saved positions for `[call]`, innermost last.
   *
   * A frame holds the entire program rather than just the counter, because a
   * call may cross into a different script file; `[return]` has to restore the
   * caller's file as well as its position.
   */
  private readonly callStack: {
    program: readonly KagStatement[];
    labels: ReadonlyMap<string, number>;
    storage: string;
    pc: number;
  }[] = [];
  /**
   * Work the scenario is awaiting that cannot be expressed as a time predicate —
   * decoding an image before the next statement runs, for example. Generators
   * cannot `await`, so the generator yields the promise and finishes its step;
   * the host settles it when the promise resolves.
   */
  private pendingWork: { promise: Promise<void>; label: string } | null = null;
  private finished = false;
  private fatalError: string | null = null;

  /** Per-frame budget so a runaway loop cannot lock the tab. */
  private static readonly MAX_STEPS_PER_TICK = 4000;

  /** KAG's `f.` map — per-playthrough flags. */
  readonly flags = new Map<string, unknown>();
  /** KAG's `sf.` map — values that survive into a save slot. */
  readonly systemFlags = new Map<string, unknown>();
  /**
   * KAG's `mp.` map — macro parameters.
   *
   * Kept as real storage rather than a fresh object per expression because
   * transition macros read `mp.storage` to decide whether the background has
   * actually changed, and that value has to survive from the tag that set it to
   * the guard that reads it.
   */
  readonly macroParameters = new Map<string, unknown>();

  private readonly history: HistoryEntry[] = [];
  private historyVisible = false;

  /** Typewriter state for the current message. */
  private reveal: {
    layout: ReturnType<TextRenderer['layout']>;
    total: number;
    revealed: number;
    charsPerSecond: number;
    startedAt: number;
    completed: boolean;
  } | null = null;

  private messageText = '';
  private messageSpeaker = '';
  private messagePadding = { left: 48, top: 32, right: 48, bottom: 24 };

  private selection: {
    options: readonly SelectOption[];
    x: number;
    y: number;
    vertical: boolean;
    hovered: number;
    width: number;
    rowHeight: number;
  } | null = null;

  private currentStorage = '';

  constructor(
    private readonly deps: KagSceneDeps,
    program: readonly KagStatement[],
    labels: ReadonlyMap<string, number>,
  ) {
    this.program = program;
    this.labels = labels;
  }

  /* ---------------------------------------------------------------- */
  /* Lifecycle                                                        */
  /* ---------------------------------------------------------------- */

  /** Starts execution at `label`, or at the top of the script. */
  start(label?: string): void {
    this.pc = label ? this.resolveLabel(label) : 0;
    this.finished = false;
    this.fatalError = null;
    this.generator = this.run();
    // Pump immediately so the first frame is not blank.
    this.pump();
  }

  isFinished(): boolean {
    return this.finished;
  }

  error(): string | null {
    return this.fatalError;
  }

  get waitLabel(): string | null {
    return this.pendingWait?.label ?? null;
  }

  get isSelecting(): boolean {
    return this.selection !== null;
  }

  get isHistoryVisible(): boolean {
    return this.historyVisible;
  }

  get backlog(): readonly HistoryEntry[] {
    return this.history;
  }

  get currentMessage(): { speaker: string; text: string } {
    return { speaker: this.messageSpeaker, text: this.messageText };
  }

  /** Advances the scene by one host frame. */
  update(deltaMs: number): void {
    this.deps.stage.update(deltaMs);
    this.updateReveal();

    if (!this.generator) return;
    // An in-flight promise (an image decode) gates progress until it settles;
    // `settlePendingWork` restarts the pump when it does.
    if (this.pendingWork) return;
    if (this.pendingWait === null || this.pendingWait.isSatisfied(performance.now())) {
      this.pendingWait = null;
      this.pump();
    }
  }

  /**
   * Suspends the scenario until `work` completes.
   *
   * Generators cannot `await`, so the promise is tracked in `pendingWork`. The
   * host stops pumping while it is set and resumes via `settlePendingWork`.
   * Attaching a rejection handler immediately means the promise can never
   * surface as an unhandled rejection while it is parked here.
   */
  private *awaitWork(
    work: Promise<void>,
    label: string,
  ): Generator<WaitCondition | null, void, void> {
    let failure: unknown = null;
    let settled = false;
    const tracked = work.then(
      () => {
        settled = true;
      },
      (error: unknown) => {
        settled = true;
        failure = error;
      },
    );
    this.pendingWork = { promise: tracked, label };
    // Resume as soon as the work settles, without waiting for the next frame.
    void tracked.then(() => this.settlePendingWork());
    yield waitForInput(label);
    this.pendingWork = null;
    if (failure !== null) throw failure;
  }

  /** Called when a tracked promise resolves; restarts the pump. */
  private settlePendingWork(): void {
    if (!this.pendingWork) return;
    // The generator itself clears `pendingWork` when resumed.
    queueMicrotask(() => {
      if (!this.generator) return;
      this.pendingWait = null;
      this.pump();
    });
    this.deps.host.requestFrame();
  }

  private pump(): void {
    if (!this.generator) return;
    let steps = 0;
    for (;;) {
      if (steps >= KagScene.MAX_STEPS_PER_TICK) {
        // Yield the rest to the next frame instead of blocking this one.
        this.deps.host.requestFrame();
        return;
      }
      steps += 1;

      let result: IteratorResult<WaitCondition | null, void>;
      try {
        result = this.generator.next();
      } catch (error) {
        this.fatalError = error instanceof Error ? error.message : String(error);
        this.deps.host.log('error', `scenario aborted: ${this.fatalError}`, error);
        this.generator = null;
        this.finished = true;
        return;
      }

      if (result.done) {
        this.generator = null;
        this.finished = true;
        this.deps.request({ kind: 'session-end', reason: 'scenario-end' });
        return;
      }
      if (result.value) {
        this.pendingWait = result.value;
        return;
      }
    }
  }

  /** Releases a pending input wait. */
  private resumeWait(): void {
    if (!this.pendingWait) return;
    this.pendingWait = null;
    this.pump();
  }

  /* ---------------------------------------------------------------- */
  /* Rendering                                                        */
  /* ---------------------------------------------------------------- */

  render(context: CanvasRenderingContext2D): void {
    this.deps.stage.render(context);
    this.renderMessage(context);
    if (this.selection) this.renderSelection(context);
    if (this.historyVisible) this.renderHistory(context);
  }

  private renderMessage(context: CanvasRenderingContext2D): void {
    const reveal = this.reveal;
    if (!reveal) return;
    const messageLayer = this.deps.stage.get(KAG_LAYERS.message0);
    if (messageLayer && !messageLayer.visible) return;
    this.deps.text.draw(
      context,
      reveal.layout,
      (messageLayer?.x ?? 0) + this.messagePadding.left,
      (messageLayer?.y ?? 0) + this.messagePadding.top,
      reveal.revealed,
      1,
    );
  }

  private renderSelection(context: CanvasRenderingContext2D): void {
    const selection = this.selection;
    if (!selection) return;
    const style = this.deps.text.style;
    context.save();
    for (let i = 0; i < selection.options.length; i += 1) {
      const option = selection.options[i]!;
      const x = selection.x + (selection.vertical ? 0 : i * (selection.width + 12));
      const y = selection.y + (selection.vertical ? i * selection.rowHeight : 0);
      const hovered = selection.hovered === i;

      context.globalAlpha = hovered ? 0.94 : 0.7;
      context.fillStyle = hovered ? 'rgba(44, 84, 152, 0.94)' : 'rgba(12, 16, 26, 0.78)';
      context.fillRect(x, y, selection.width, selection.rowHeight);
      context.globalAlpha = 1;
      context.strokeStyle = hovered ? 'rgba(186, 216, 255, 0.95)' : 'rgba(255, 255, 255, 0.38)';
      context.lineWidth = 1.5;
      context.strokeRect(x + 0.5, y + 0.5, selection.width - 1, selection.rowHeight - 1);
      this.deps.text.paintText(
        context,
        option.label,
        x + 20,
        y + style.fontSize * 1.35,
        style,
      );
    }
    context.restore();
  }

  private renderHistory(context: CanvasRenderingContext2D): void {
    const style = this.deps.text.style;
    const width = this.deps.stage.width;
    const height = this.deps.stage.height;
    context.save();
    context.fillStyle = 'rgba(4, 6, 12, 0.88)';
    context.fillRect(0, 0, width, height);

    const rowHeight = style.fontSize * style.lineSpacing;
    const maxRows = Math.floor((height - 80) / rowHeight);
    const entries = this.history.slice(-maxRows);
    let y = 40;
    for (const entry of entries) {
      const label = entry.speaker ? `${entry.speaker}: ` : '';
      this.deps.text.paintText(context, `${label}${entry.text}`, 40, y + style.fontSize, style);
      y += rowHeight;
    }
    context.restore();
  }

  /* ---------------------------------------------------------------- */
  /* Input                                                            */
  /* ---------------------------------------------------------------- */

  /** Returns true when the scene consumed the event. */
  handlePointer(x: number, y: number, phase: 'down' | 'move' | 'up'): boolean {
    if (this.historyVisible) {
      if (phase === 'up') this.historyVisible = false;
      return true;
    }

    if (this.selection) {
      const selection = this.selection;
      for (let i = 0; i < selection.options.length; i += 1) {
        const optionX = selection.x + (selection.vertical ? 0 : i * (selection.width + 12));
        const optionY = selection.y + (selection.vertical ? i * selection.rowHeight : 0);
        const inside =
          x >= optionX &&
          x <= optionX + selection.width &&
          y >= optionY &&
          y <= optionY + selection.rowHeight;
        if (inside) {
          selection.hovered = i;
          if (phase === 'up') this.chooseOption(i);
          return true;
        }
      }
      return true;
    }

    if (phase === 'down' || phase === 'up') {
      if (this.reveal && !this.reveal.completed) {
        this.completeReveal();
        return true;
      }
      if (this.pendingWait && ['click', 'input'].includes(this.pendingWait.label)) {
        this.resumeWait();
        return true;
      }
    }
    return false;
  }

  handleWheel(deltaY: number): boolean {
    if (!this.reveal) return false;
    // Scrolling adjusts reading speed, the convention in the native players.
    this.reveal.charsPerSecond = Math.min(
      2000,
      Math.max(0, this.reveal.charsPerSecond + (deltaY < 0 ? 60 : -60)),
    );
    return true;
  }

  handleKey(key: string, phase: 'down' | 'up'): boolean {
    if (this.historyVisible) {
      if (phase === 'down' && (key === 'Escape' || key === 'Enter' || key === ' ')) {
        this.historyVisible = false;
      }
      return true;
    }

    if (this.selection) {
      const selection = this.selection;
      const count = selection.options.length;
      const previous = selection.vertical ? 'ArrowUp' : 'ArrowLeft';
      const next = selection.vertical ? 'ArrowDown' : 'ArrowRight';
      if (key === previous) {
        selection.hovered = (selection.hovered - 1 + count) % count;
        return true;
      }
      if (key === next) {
        selection.hovered = (selection.hovered + 1) % count;
        return true;
      }
      if (phase === 'down' && (key === 'Enter' || key === ' ')) {
        this.chooseOption(selection.hovered);
        return true;
      }
      return true;
    }

    if (phase === 'down') {
      if (key === 'Escape') {
        this.historyVisible = true;
        return true;
      }
      if (key === 'Enter' || key === ' ' || key === 'Control') {
        if (this.reveal && !this.reveal.completed) {
          this.completeReveal();
          return true;
        }
        if (this.pendingWait && ['click', 'input'].includes(this.pendingWait.label)) {
          this.resumeWait();
          return true;
        }
      }
    }
    return false;
  }

  private chooseOption(index: number): void {
    const selection = this.selection;
    if (!selection) return;
    const option = selection.options[index];
    if (!option) return;
    this.selection = null;
    // KAG exposes the choice through `f.selected`/`f.target` so scripts and
    // TJS handlers can branch on it after the `[select]` returns.
    this.flags.set('selected', option.label);
    this.flags.set('selected_index', index);
    if (option.target) this.flags.set('target', option.target);
    this.resumeWait();
  }

  /* ---------------------------------------------------------------- */
  /* Scenario execution                                               */
  /* ---------------------------------------------------------------- */

  private *run(): Generator<WaitCondition | null, void, void> {
    let guard = 0;
    while (this.pc < this.program.length) {
      if ((guard += 1) > 10_000_000) {
        throw new Error('scenario exceeded the instruction guard');
      }
      const statement = this.program[this.pc]!;

      if (statement.kind === 'text') {
        this.pc += 1;
        this.appendMessage(statement.text);
        continue;
      }
      if (statement.kind === 'comment' || statement.kind === 'label') {
        this.pc += 1;
        continue;
      }

      const outcome = yield* this.executeTag(statement.tag);
      if (outcome === 'halt') return;
      if (outcome === 'jumped') continue;
      this.pc += 1;
    }
  }

  /* ---------------------------------------------------------------- */
  /* Tag dispatch                                                     */
  /* ---------------------------------------------------------------- */

  /**
   * Runs one KAG tag and tallies the outcome.
   *
   * The tally is what produces the session compatibility report, so it is
   * applied here rather than in each branch: a tag the dispatch does not know
   * about is recorded as skipped, and everything else counts as honoured. That
   * keeps the two accounting rules impossible to forget in a new branch.
   */
  private *executeTag(
    tag: KagTag,
  ): Generator<WaitCondition | null, 'handled' | 'halt' | 'jumped', void> {
    const outcome = yield* this.dispatchTag(tag);
    if (outcome === 'unhandled') {
      this.deps.compat.unsupported(tag.name.toLowerCase(), 'skipped', {
        line: tag.line,
      });
      return 'handled';
    }
    if (outcome === 'handled') this.deps.compat.honoured(tag.name.toLowerCase());
    return outcome;
  }

  private *dispatchTag(
    tag: KagTag,
  ): Generator<WaitCondition | null, 'handled' | 'halt' | 'jumped' | 'unhandled', void> {
    switch (tag.name.toLowerCase()) {
      /* --- message flow --- */
      case 'cm':
      case 'ct':
        this.clearMessage();
        return 'handled';
      case 'r':
      case 'reset':
        yield* this.waitForReveal();
        return 'handled';
      case 'er':
        this.completeReveal();
        return 'handled';
      case 'br':
        this.appendMessage('\n');
        return 'handled';
      case 'p':
        this.completeReveal();
        yield waitForInput('click');
        return 'handled';
      case 'l':
      case 'line':
      case 'position_message':
      case 'deffont':
        return 'handled';
      case 'wait':
        yield waitForMs(numberAttr(tag, 'time', 500), 'wait');
        return 'handled';
      case 'waitclick':
      case 'click':
        yield waitForInput('click');
        return 'handled';

      /* --- control flow --- */
      case 'end':
        // `[end]` inside a *called* script returns to the caller; only the
        // outermost script actually ends the game. KAG3 relies on this for
        // library scripts: the bundled demo opens with `@call storage="snow.ks"`,
        // a plugin definition that finishes with `[end]`. Treating that as a
        // session end stopped the game before its first line of dialogue.
        if (this.callStack.length > 0) {
          this.returnFromCall();
          return 'jumped';
        }
        return 'halt';
      case 'jump':
        yield* this.jumpTo(stringAttr(tag, 'target') ?? stringAttr(tag, 'storage') ?? '');
        return 'jumped';
      case 'call': {
        const target = stringAttr(tag, 'target') ?? stringAttr(tag, 'storage') ?? '';
        // Remember where to come back to before switching scripts. The frame
        // captures the whole program, not just the counter, because `@call` can
        // cross into another file.
        this.callStack.push({
          program: this.program,
          labels: this.labels,
          storage: this.currentStorage,
          pc: this.pc + 1,
        });
        yield* this.jumpTo(target);
        return 'jumped';
      }
      case 'return':
        if (this.callStack.length > 0) {
          this.returnFromCall();
          return 'jumped';
        }
        // A `[return]` with nothing to return to ends the script, matching the
        // native engine's behaviour for a stray return at the top level.
        return 'halt';
      case 'gotostart':
        this.pc = 0;
        return 'jumped';
      case 'if':
        yield* this.executeConditional(tag);
        return 'handled';
      case 'elseif':
      case 'else':
      case 'endif':
        // Reached only when the preceding branch ran to its end; the matching
        // `[endif]` was already consumed by `executeConditional`.
        return 'handled';
      case 'eval':
        this.evaluateExpressionAttr(tag);
        return 'handled';
      case 'clearvar':
        this.flags.clear();
        return 'handled';
      case 'macro':
      case 'endmacro':
        // Macros are expanded by the loader, not at run time; see `MacroExpander`.
        return 'handled';

      /* --- layers --- */
      case 'bg':
      case 'background':
        yield* this.setBackground(tag);
        return 'handled';
      case 'image':
      case 'ld':
        yield* this.applyLayerImage(tag);
        return 'handled';
      case 'layopt':
        this.applyLayerOptions(tag);
        return 'handled';
      case 'position':
        this.applyPosition(tag);
        return 'handled';
      case 'cl':
      case 'freeimage':
        this.deps.stage.clear(stringAttr(tag, 'layer') ?? KAG_LAYERS.layer0);
        return 'handled';
      case 'trans':
        yield* this.applyTransition(tag);
        return 'handled';
      case 'quake':
        this.deps.stage.startShake(numberAttr(tag, 'count', 3), numberAttr(tag, 'time', 400));
        yield waitForMs(numberAttr(tag, 'time', 400), 'quake');
        return 'handled';
      case 'msgoff':
        this.deps.stage.layer(KAG_LAYERS.message0, 40).visible = false;
        return 'handled';
      case 'msgon':
        this.deps.stage.layer(KAG_LAYERS.message0, 40).visible = true;
        return 'handled';

      /* --- text formatting --- */
      case 'locate':
        this.applyLocate(tag);
        return 'handled';
      case 'font':
        this.applyFont(tag);
        return 'handled';
      case 'ruby':
        this.appendMessage(`[ruby ${stringAttr(tag, 'text') ?? ''}]`);
        return 'handled';
      case 'color':
      case 'colour':
        this.appendMessage(`[color ${stringAttr(tag, 'color') ?? '#ffffff'}]`);
        return 'handled';
      case 'size':
        this.appendMessage(`[size ${stringAttr(tag, 'size') ?? '26'}]`);
        return 'handled';
      case 'bold':
        this.appendMessage('[b]');
        return 'handled';
      case 'italic':
        this.appendMessage('[i]');
        return 'handled';

      /* --- audio --- */
      case 'playbgm':
      case 'bgm':
      case 'playse':
      case 'se':
      case 'playvoice':
      case 'voice':
      case 'stopbgm':
      case 'stopsound':
      case 'stopse':
      case 'fadebgm':
        this.handleAudio(tag);
        return 'handled';

      /* --- interaction --- */
      case 'select':
        yield* this.runSelect(tag);
        return 'handled';
      case 'history':
      case 'backlog':
        // `[history]` shows the backlog and waits for the player to dismiss it;
        // in the native engine that dismissal is the same tap that advances
        // dialogue. Waiting on a *distinct* label here would be unsatisfiable —
        // nothing else in the runtime can clear it — so the scenario would hang
        // on the tag forever. That is exactly what happened against the demo,
        // whose first substantive tag is `[history]`.
        this.historyVisible = true;
        yield waitForInput('click');
        this.historyVisible = false;
        return 'handled';

      /* --- saves --- */
      case 'save':
        this.clearMessage();
        this.deps.request({ kind: 'save-request', slot: numberAttr(tag, 'slot', 0) });
        return 'handled';
      case 'load':
        this.deps.request({ kind: 'load-request', slot: numberAttr(tag, 'slot', 0) });
        return 'handled';

      /* --- engine --- */
      case 'title':
        this.deps.onTitle?.(stringAttr(tag, 'name') ?? '');
        return 'handled';
      case 'graph':
        return 'handled';
      case 'loadplugin':
      case 'plugin':
        // A native plugin cannot be loaded in a browser: there is no `.dll` or
        // `.so` to dlopen. The dispatch below records it as unsupported so the
        // session's compatibility report names the missing feature.
        return 'handled';

      default:
        return 'unhandled';
    }
  }

  private evaluateExpressionAttr(tag: KagTag): void {
    const expression = stringAttr(tag, 'exp');
    if (expression === undefined) return;
    try {
      this.deps.evaluateExpression(expression);
    } catch (error) {
      this.deps.host.log('warn', `[eval] failed: ${String(error)}`);
    }
  }

  /* ---------------------------------------------------------------- */
  /* Conditionals                                                     */
  /* ---------------------------------------------------------------- */

  /**
   * Evaluates an `[if]`/`[elseif]` condition.
   *
   * KAG spells the expression either as `exp=` (a TJS2 expression) or as a bare
   * attribute whose presence means true. A failing expression counts as false,
   * matching the native engine's tolerance of not-yet-defined flags.
   */
  private evaluateCondition(tag: KagTag): boolean {
    const expression =
      stringAttr(tag, 'exp') ?? stringAttr(tag, 'condition') ?? stringAttr(tag, 'cond');
    if (expression === undefined) {
      // `[if]` with no expression is not a valid test; treat it as false rather
      // than running a branch the script did not select.
      return false;
    }
    try {
      return Boolean(this.deps.evaluateExpression(expression));
    } catch (error) {
      this.deps.host.log('warn', `[if] expression failed: ${String(error)}`);
      return false;
    }
  }

  /**
   * Executes an `[if]` group and leaves `this.pc` at the matching `[endif]`.
   *
   * KAG's conditional syntax is `[if]` / `[elseif]` / `[else]` / `[endif]` with
   * optional `exp` attributes. The interpreter evaluates each condition in turn,
   * runs the body of the first branch that holds, and then skips to the end of
   * the group — so a group is entered at most once and never falls through into
   * a later branch.
   */
  private *executeConditional(tag: KagTag): Generator<WaitCondition | null, void, void> {
    let taken = this.evaluateCondition(tag);
    let branchStart = this.pc + 1;

    for (;;) {
      const branchEnd = this.findBranchEnd(branchStart);
      if (taken) {
        // Run this branch, then jump past the whole group.
        yield* this.runBranch(branchStart, branchEnd);
        this.pc = this.findGroupEnd(this.pc);
        return;
      }

      // The branch was false: resume scanning at its terminator.
      const terminator = this.program[branchEnd];
      if (!terminator || terminator.kind !== 'tag') {
        this.pc = branchEnd;
        return;
      }
      const name = terminator.tag.name.toLowerCase();
      if (name === 'endif') {
        this.pc = branchEnd;
        return;
      }
      if (name === 'elseif') {
        taken = this.evaluateCondition(terminator.tag);
        branchStart = branchEnd + 1;
        continue;
      }
      // `[else]`: the remaining branch is unconditional.
      taken = true;
      branchStart = branchEnd + 1;
    }
  }

  /** Runs the statements of one branch, without touching `this.pc`. */
  private *runBranch(
    start: number,
    end: number,
  ): Generator<WaitCondition | null, void, void> {
    const savedPc = this.pc;
    let index = start;
    while (index < end) {
      this.pc = index;
      const statement = this.program[index]!;
      if (statement.kind === 'text') {
        this.appendMessage(statement.text);
        index += 1;
        continue;
      }
      if (statement.kind === 'tag') {
        const name = statement.tag.name.toLowerCase();
        if (name === 'if') {
          // A nested group is handled by the same routine.
          yield* this.executeConditional(statement.tag);
          index = this.pc + 1;
          continue;
        }
        const outcome = yield* this.executeTag(statement.tag);
        if (outcome === 'halt') return;
        if (outcome === 'jumped') {
          // The branch transferred control away; stop running this body.
          return;
        }
      }
      index += 1;
    }
    // Leave `this.pc` on the branch terminator so the caller can continue.
    this.pc = end;
    void savedPc;
  }

  /** Index of the branch terminator at the same nesting depth as `start`. */
  private findBranchEnd(start: number): number {
    let depth = 0;
    for (let index = start; index < this.program.length; index += 1) {
      const statement = this.program[index]!;
      if (statement.kind !== 'tag') continue;
      const name = statement.tag.name.toLowerCase();
      if (name === 'if') depth += 1;
      else if (name === 'endif') {
        if (depth === 0) return index;
        depth -= 1;
      } else if ((name === 'elseif' || name === 'else') && depth === 0) {
        return index;
      }
    }
    // Unterminated group: treat the end of the script as the terminator.
    return this.program.length;
  }

  /** Index of the `[endif]` that closes the group starting at `from`. */
  private findGroupEnd(from: number): number {
    let depth = 0;
    for (let index = from; index < this.program.length; index += 1) {
      const statement = this.program[index]!;
      if (statement.kind !== 'tag') continue;
      const name = statement.tag.name.toLowerCase();
      if (name === 'if') depth += 1;
      else if (name === 'endif') {
        depth -= 1;
        if (depth <= 0) return index;
      }
    }
    return this.program.length;
  }

  /* ---------------------------------------------------------------- */
  /* Messages                                                         */
  /* ---------------------------------------------------------------- */

  /** Appends visible text to the current message and re-lays it out. */
  private appendMessage(text: string): void {
    if (!this.messageText) this.messageSpeaker = this.flags.has('speaker')
      ? String(this.flags.get('speaker'))
      : '';
    this.messageText += this.expandInlineExpressions(text);
    this.relayoutMessage();
  }

  /** Appends text without resetting the reveal, used by macros. */
  appendRaw(text: string): void {
    this.appendMessage(text);
  }

  private relayoutMessage(): void {
    const runs = parseInlineRuns(this.messageText);
    const layout = this.deps.text.layout(runs, { maxWidth: this.messageBoxWidth });
    const previousRevealed = this.reveal?.revealed ?? 0;
    this.reveal = {
      layout,
      total: layout.totalCharacters,
      revealed: Math.min(previousRevealed, layout.totalCharacters),
      charsPerSecond: this.deps.host.setting('textSpeed', 45),
      startedAt: performance.now(),
      completed: false,
    };
    if (this.reveal.revealed >= this.reveal.total) this.reveal.completed = true;
  }

  /** Substitutes `%[expr]` inside prose. */
  private expandInlineExpressions(text: string): string {
    if (!text.includes('%[')) return text;
    return text.replace(/%\[([^\]]*)\]/g, (_match, expression: string) => {
      try {
        const value = this.deps.evaluateExpression(expression);
        return value === undefined || value === null ? '' : String(value);
      } catch (error) {
        this.deps.host.log('trace', `inline expression failed: ${String(error)}`);
        return '';
      }
    });
  }

  private updateReveal(): void {
    const reveal = this.reveal;
    if (!reveal || reveal.completed) return;
    const elapsed = (performance.now() - reveal.startedAt) / 1000;
    reveal.revealed = Math.min(
      reveal.total,
      Math.max(reveal.revealed, elapsed * reveal.charsPerSecond),
    );
    if (reveal.revealed >= reveal.total) reveal.completed = true;
  }

  private completeReveal(): void {
    if (!this.reveal) return;
    this.reveal.revealed = this.reveal.total;
    this.reveal.completed = true;
  }

  private *waitForReveal(): Generator<WaitCondition | null, void, void> {
    while (this.reveal && !this.reveal.completed) {
      yield waitForMs(16, 'reveal');
    }
  }

  private clearMessage(): void {
    if (this.messageText || this.messageSpeaker) {
      this.history.push({ speaker: this.messageSpeaker, text: this.messageText });
      if (this.history.length > 500) this.history.shift();
    }
    this.messageText = '';
    this.messageSpeaker = '';
    this.reveal = null;
  }

  private get messageBoxWidth(): number {
    const layer = this.deps.stage.get(KAG_LAYERS.message0);
    const width = layer && layer.width > 0 ? layer.width : this.deps.stage.width;
    return Math.max(
      80,
      width - this.messagePadding.left - this.messagePadding.right,
    );
  }

  /* ---------------------------------------------------------------- */
  /* Layers                                                           */
  /* ---------------------------------------------------------------- */

  private *setBackground(tag: KagTag): Generator<WaitCondition | null, void, void> {
    const storage = stringAttr(tag, 'storage');
    const layer = this.deps.stage.layer(KAG_LAYERS.base, zIndexForLayer(KAG_LAYERS.base));
    const duration = numberAttr(tag, 'time', 0);
    const method = stringAttr(tag, 'method') ?? 'crossfade';

    if (storage) {
      const holder: { image: Awaited<ReturnType<KagSceneDeps['assets']['tryLoadImage']>> } = {
        image: null,
      };
      yield* this.awaitWork(
        this.loadImage(storage).then((image) => {
          holder.image = image;
        }),
        'image',
      );
      const decoded = holder.image;
      if (decoded) {
        layer.image = decoded;
        layer.src = null;
        layer.width = decoded.width;
        layer.height = decoded.height;
        layer.x = numberAttr(tag, 'x', 0);
        layer.y = numberAttr(tag, 'y', 0);
        layer.opacity = duration > 0 && method !== 'fadein' ? 0 : 1;
      } else {
        this.deps.host.log('warn', `[bg] could not load "${storage}"`);
      }
    }

    if (duration > 0) {
      this.deps.stage.animate(layer, { opacity: 1 }, duration, method);
      yield* this.waitForStage(duration);
    } else {
      layer.opacity = 1;
    }
  }

  private *applyLayerImage(tag: KagTag): Generator<WaitCondition | null, void, void> {
    const layerName = stringAttr(tag, 'layer') ?? KAG_LAYERS.layer0;
    const layer = this.deps.stage.layer(layerName, zIndexForLayer(layerName));
    const storage = stringAttr(tag, 'storage');

    if (storage) {
      const holder: { image: Awaited<ReturnType<KagSceneDeps['assets']['tryLoadImage']>> } = {
        image: null,
      };
      yield* this.awaitWork(
        this.loadImage(storage).then((image) => {
          holder.image = image;
        }),
        'image',
      );
      const decoded = holder.image;
      if (decoded) {
        layer.image = decoded;
        layer.src = null;
        const left = tag.attrs.has('left') ? numberAttr(tag, 'left', 0) : null;
        const top = tag.attrs.has('top') ? numberAttr(tag, 'top', 0) : null;
        const width = tag.attrs.has('width') ? numberAttr(tag, 'width', decoded.width) : null;
        const height = tag.attrs.has('height') ? numberAttr(tag, 'height', decoded.height) : null;
        if (left !== null || top !== null || width !== null || height !== null) {
          const source = {
            x: left ?? 0,
            y: top ?? 0,
            width: width ?? decoded.width - (left ?? 0),
            height: height ?? decoded.height - (top ?? 0),
          };
          layer.src = source;
          layer.width = source.width;
          layer.height = source.height;
        } else {
          layer.width = decoded.width;
          layer.height = decoded.height;
        }
        layer.visible = true;
        if (tag.attrs.has('opacity')) layer.opacity = numberAttr(tag, 'opacity', 255) / 255;
      } else {
        this.deps.host.log('warn', `[image] could not load "${storage}"`);
      }
    }

    const duration = numberAttr(tag, 'time', 0);
    const animated: { x?: number; y?: number; opacity?: number } = {};
    if (tag.attrs.has('x')) animated.x = numberAttr(tag, 'x', layer.x);
    if (tag.attrs.has('y')) animated.y = numberAttr(tag, 'y', layer.y);
    if (duration > 0 && Object.keys(animated).length > 0) {
      this.deps.stage.animate(layer, animated, duration, stringAttr(tag, 'method') ?? 'easeInOut');
      yield* this.waitForStage(duration);
    } else {
      if (animated.x !== undefined) layer.x = animated.x;
      if (animated.y !== undefined) layer.y = animated.y;
    }
  }

  private applyLayerOptions(tag: KagTag): void {
    const layerName = stringAttr(tag, 'layer') ?? KAG_LAYERS.layer0;
    const layer = this.deps.stage.layer(layerName, zIndexForLayer(layerName));
    if (tag.attrs.has('visible')) layer.visible = booleanAttr(tag, 'visible', true);
    if (tag.attrs.has('opacity')) layer.opacity = numberAttr(tag, 'opacity', 255) / 255;
    if (tag.attrs.has('left')) layer.x = numberAttr(tag, 'left', layer.x);
    if (tag.attrs.has('top')) layer.y = numberAttr(tag, 'top', layer.y);
    if (tag.attrs.has('width')) layer.width = numberAttr(tag, 'width', layer.width);
    if (tag.attrs.has('height')) layer.height = numberAttr(tag, 'height', layer.height);
  }

  private applyPosition(tag: KagTag): void {
    const layerName = stringAttr(tag, 'layer') ?? KAG_LAYERS.layer0;
    const layer = this.deps.stage.layer(layerName, zIndexForLayer(layerName));
    if (tag.attrs.has('x')) layer.x = numberAttr(tag, 'x', layer.x);
    if (tag.attrs.has('y')) layer.y = numberAttr(tag, 'y', layer.y);
    if (tag.attrs.has('width')) layer.width = numberAttr(tag, 'width', layer.width);
    if (tag.attrs.has('height')) layer.height = numberAttr(tag, 'height', layer.height);
    if (tag.attrs.has('opacity')) layer.opacity = numberAttr(tag, 'opacity', 255) / 255;
    if (tag.attrs.has('visible')) layer.visible = booleanAttr(tag, 'visible', true);

    // Sprite-sheet frame selection, as KAG's `[position frame=N]` does.
    if (tag.attrs.has('frame') && layer.image) {
      const frame = numberAttr(tag, 'frame', 0);
      const columns = Math.max(1, numberAttr(tag, 'columns', 1));
      const rows = Math.max(1, numberAttr(tag, 'rows', 1));
      const cellWidth = layer.image.width / columns;
      const cellHeight = layer.image.height / rows;
      const column = columns > 0 ? frame % columns : 0;
      const row = Math.floor(frame / columns) % rows;
      layer.src = {
        x: column * cellWidth,
        y: row * cellHeight,
        width: cellWidth,
        height: cellHeight,
      };
      layer.width = cellWidth;
      layer.height = cellHeight;
    }
  }

  private applyLocate(tag: KagTag): void {
    this.messagePadding = {
      left: numberAttr(tag, 'left', this.messagePadding.left),
      top: numberAttr(tag, 'top', this.messagePadding.top),
      right: this.messagePadding.right,
      bottom: this.messagePadding.bottom,
    };
  }

  private applyFont(tag: KagTag): void {
    const style: Partial<TextStyle> = {};
    const size = tag.attrs.get('size');
    if (size !== undefined) style.fontSize = parseKagNumber(size, this.deps.text.style.fontSize);
    const color = stringAttr(tag, 'color');
    if (color) style.color = normalizeColour(color);
    const face = stringAttr(tag, 'face');
    if (face) style.fontFamily = face;
    if (tag.attrs.has('bold')) style.fontWeight = booleanAttr(tag, 'bold', false) ? '700' : '400';
    if (Object.keys(style).length > 0) this.deps.text.setBaseStyle(style);
  }

  /** Loads an image, retrying with common extensions when none is given. */
  private async loadImage(storage: string) {
    const path = resolveStoragePath(storage);
    const image = await this.deps.assets.tryLoadImage(path);
    if (image) return image;
    if (!/\.[a-z0-9]{2,5}$/i.test(path)) {
      for (const extension of ['.png', '.jpg', '.webp', '.tga']) {
        const candidate = await this.deps.assets.tryLoadImage(`${path}${extension}`);
        if (candidate) return candidate;
      }
    }
    return null;
  }

  /* ---------------------------------------------------------------- */
  /* Transitions                                                      */
  /* ---------------------------------------------------------------- */

  private *applyTransition(tag: KagTag): Generator<WaitCondition | null, void, void> {
    const duration = numberAttr(tag, 'time', 300);
    const method = stringAttr(tag, 'method') ?? 'crossfade';
    const layerName = stringAttr(tag, 'layer');
    const rule = stringAttr(tag, 'rule');

    if (rule) {
      // The mask art is not applied; a cross-fade preserves the timing and
      // readability, so this is an approximation rather than a failure.
      this.deps.compat.unsupported('trans:rule', 'approximated', {
        line: tag.line,
        note: `rule="${rule}" rendered as a cross-fade`,
      });
    }

    if (layerName) {
      const layer = this.deps.stage.layer(layerName, zIndexForLayer(layerName));
      const target = numberAttr(tag, 'opacity', 255) / 255;
      if (method === 'fadein') layer.opacity = 0;
      this.deps.stage.animate(layer, { opacity: target }, duration, method);
    } else if (method === 'fadein') {
      for (const layer of this.deps.stage.visibleLayers()) {
        layer.opacity = 0;
        this.deps.stage.animate(layer, { opacity: 1 }, duration, method);
      }
    }

    if (booleanAttr(tag, 'wait', true)) yield* this.waitForStage(duration);
  }

  /** Waits for the stage transitions to finish, bounded by `durationMs`. */
  private *waitForStage(durationMs: number): Generator<WaitCondition | null, void, void> {
    const deadline = performance.now() + durationMs + 32;
    while (performance.now() < deadline) {
      yield waitForMs(16, 'trans');
      if (!this.deps.stage.isAnimating) return;
    }
  }

  /* ---------------------------------------------------------------- */
  /* Audio                                                            */
  /* ---------------------------------------------------------------- */

  private handleAudio(tag: KagTag): void {
    const command = tag.name.toLowerCase();
    const slot = stringAttr(tag, 'buf') ?? '0';
    const channel: 'bgm' | 'se' | 'voice' = command.includes('bgm')
      ? 'bgm'
      : command.includes('voice')
        ? 'voice'
        : 'se';
    const storage = stringAttr(tag, 'storage') ?? stringAttr(tag, 'file');

    if (command.startsWith('stop') || !storage) {
      this.deps.request({
        kind: 'audio-stop',
        slot: `${channel}${slot}`,
        fadeMs: numberAttr(tag, 'fade', 0),
      });
      return;
    }

    this.deps.request({
      kind: 'audio-play',
      slot: `${channel}${slot}`,
      channel,
      storage: resolveStoragePath(storage),
      loop: booleanAttr(tag, 'loop', channel === 'bgm'),
      volume: numberAttr(tag, 'volume', 255) / 255,
      fadeMs: numberAttr(tag, 'fade', 0),
    });
  }

  /* ---------------------------------------------------------------- */
  /* Selection                                                        */
  /* ---------------------------------------------------------------- */

  private *runSelect(tag: KagTag): Generator<WaitCondition | null, void, void> {
    const options: SelectOption[] = [];
    for (let i = 1; i <= 32; i += 1) {
      const label = stringAttr(tag, `text${i}`) ?? stringAttr(tag, `name${i}`);
      if (label === undefined) continue;
      options.push({ label, target: stringAttr(tag, `target${i}`) ?? '' });
    }
    if (options.length === 0) {
      this.deps.host.log('warn', `[select] at line ${tag.line} declared no options`);
      return;
    }

    const style = this.deps.text.style;
    const widest = options.reduce(
      (max, option) => Math.max(max, this.deps.text.measureText(option.label, style)),
      0,
    );
    this.selection = {
      options,
      width: Math.max(220, widest + 56),
      rowHeight: style.fontSize * 2.1,
      x: numberAttr(tag, 'x', Math.round(this.deps.stage.width / 2 - Math.max(110, widest / 2 + 28))),
      y: numberAttr(
        tag,
        'y',
        Math.round(this.deps.stage.height / 2 - (options.length * style.fontSize * 2.1) / 2),
      ),
      vertical: booleanAttr(tag, 'vertical', true),
      hovered: 0,
    };
    this.completeReveal();
    yield waitForInput('select');

    const target = this.flags.get('target');
    if (typeof target === 'string' && target) yield* this.jumpTo(target);
  }

  /* ---------------------------------------------------------------- */
  /* Control flow and persistence                                     */
  /* ---------------------------------------------------------------- */

  /**
   * Loads the first storage candidate that parses.
   *
   * Script storage is named relative to the scenario directory in real titles,
   * but the same name is written root-relative in others, so each candidate is
   * tried in order. The loader caches both hits and misses, so a failed attempt
   * costs one cached lookup after the first time.
   */
  private *loadStorageCandidates(
    candidates: readonly string[],
  ): Generator<WaitCondition | null, { path: string; statements: readonly KagStatement[]; labels: ReadonlyMap<string, number> } | null, void> {
    for (const candidate of candidates) {
      const holder: {
        loaded: {
          path: string;
          statements: readonly KagStatement[];
          labels: ReadonlyMap<string, number>;
        } | null;
      } = { loaded: null };
      yield* this.awaitWork(
        this.deps.loadScript(candidate).then((result) => {
          holder.loaded = result;
        }),
        'script',
      );
      if (holder.loaded) return holder.loaded;
    }
    return null;
  }

  private *jumpTo(target: string): Generator<WaitCondition | null, void, void> {
    if (!target) return;
    const separator = target.indexOf('|');
    if (separator !== -1) {
      const storage = target.slice(0, separator);
      const label = target.slice(separator + 1).replace(/^\*/, '');
      if (storage && storage !== this.currentStorage) {
        // Switching scripts means parsing a file, which is asynchronous; the
        // generator parks on the promise and resumes with the result held in a
        // mutable holder (TypeScript cannot narrow a closure-assigned local).
        const loaded = yield* this.loadStorageCandidates(this.storageCandidates(storage));
        if (loaded) {
          this.program = loaded.statements;
          this.labels = loaded.labels;
          this.currentStorage = loaded.path;
        } else {
          this.deps.host.log('warn', `could not load scenario "${storage}"`);
          this.pc += 1;
          return;
        }
      }
      this.pc = this.resolveLabel(label);
      return;
    }

    const label = target.replace(/^\*/, '');
    const index = this.labels.get(label);
    if (index !== undefined) {
      this.pc = index;
      return;
    }

    // A name that is not a label in the current script is frequently a *script*
    // reference: KAG3 titles write `@jump target="snow.ks"` as readily as
    // `@jump target="snow.ks|*start"`, and the file then boots at its own first
    // statement. Falling straight to a warning loses the branch entirely, which
    // is how a call that should have initialised the game instead leaves state
    // undefined for everything that follows.
    const loaded = yield* this.loadStorageCandidates(this.storageCandidates(label));
    if (loaded) {
      this.program = loaded.statements;
      this.labels = loaded.labels;
      this.currentStorage = loaded.path;
      // A script entered without a label starts at its first statement.
      this.pc = 0;
      return;
    }

    this.deps.host.log('warn', `jump target "${label}" was not found`);
    this.pc += 1;
  }

  private resolveLabel(name: string): number {
    const direct = this.labels.get(name);
    if (direct !== undefined) return direct;
    const stripped = name.replace(/^.*\|/, '').replace(/^\*/, '');
    const index = this.labels.get(stripped);
    if (index !== undefined) return index;
    throw new Error(`unknown label "${name}"`);
  }

  setCurrentStorage(storage: string): void {
    this.currentStorage = storage;
  }

  /** Restores the caller's program, file and position after a `[call]`. */
  private returnFromCall(): void {
    const frame = this.callStack.pop();
    if (!frame) return;
    this.program = frame.program;
    this.labels = frame.labels;
    this.currentStorage = frame.storage;
    this.pc = frame.pc;
  }

  /** Directory part of a normalised storage path, without a trailing slash. */
  private static directoryOf(path: string): string {
    const slash = path.lastIndexOf('/');
    return slash === -1 ? '' : path.slice(0, slash);
  }

  /**
   * Candidate paths for a storage name, most likely first.
   *
   * KAG3 resolves a bare script name relative to the **current scenario's
   * directory** before the game root. That is not a detail: a real title writes
   *
   *     @call storage="snow.ks"
   *
   * from `scenario/first.ks`, and the file lives at `scenario/snow.ks`. Treating
   * the name as a root-relative path finds nothing, and because the failed call
   * also skips the script's initialisation, the failure surfaces later and
   * elsewhere — as an undefined-variable error inside an unrelated `@eval`.
   *
   * Returns candidates rather than one answer because existence can only be
   * checked asynchronously; the caller tries them in order and the script cache
   * makes a miss cheap.
   */
  private storageCandidates(value: string): readonly string[] {
    const base = resolveStoragePath(value);
    if (!base) return [];

    // A name that already carries a path is taken at face value.
    if (base.includes('/')) return [base];

    const directory = KagScene.directoryOf(this.currentStorage);
    const candidates: string[] = [];
    if (directory) candidates.push(`${directory}/${base}`);
    candidates.push(base);
    if (directory !== 'scenario') candidates.push(`scenario/${base}`);
    return candidates;
  }

  /**
   * Swaps in a different scenario before restoring a save, so that a slot
   * recorded in another script file resumes in the right place.
   */
  async replaceProgramForRestore(
    statements: readonly KagStatement[],
    labels: ReadonlyMap<string, number>,
  ): Promise<void> {
    this.program = statements;
    this.labels = labels;
    this.currentStorage = '';
  }

  get storageName(): string {
    return this.currentStorage;
  }

  /** Serializable state for a save slot. */
  snapshot(): KagSceneSnapshot {
    return {
      pc: this.pc,
      storage: this.currentStorage,
      flags: Object.fromEntries(this.flags),
      systemFlags: Object.fromEntries(this.systemFlags),
      macroParameters: Object.fromEntries(this.macroParameters),
      message: this.messageText,
      speaker: this.messageSpeaker,
      history: this.history.slice(-100),
    };
  }

  /**
   * Restores a save slot.
   *
   * The scenario generator is recreated from the restored program counter, so
   * a load behaves exactly like a fresh start at that point.
   */
  restoreSnapshot(state: KagSceneSnapshot): void {
    this.pc = Number.isFinite(state.pc) ? state.pc : 0;
    this.flags.clear();
    for (const [key, value] of Object.entries(state.flags ?? {})) this.flags.set(key, value);
    this.systemFlags.clear();
    for (const [key, value] of Object.entries(state.systemFlags ?? {})) {
      this.systemFlags.set(key, value);
    }
    this.macroParameters.clear();
    for (const [key, value] of Object.entries(state.macroParameters ?? {})) {
      this.macroParameters.set(key, value);
    }
    this.messageText = state.message ?? '';
    this.messageSpeaker = state.speaker ?? '';
    this.history.length = 0;
    for (const entry of state.history ?? []) this.history.push(entry);
    this.selection = null;
    this.pendingWait = null;
    this.finished = false;
    this.fatalError = null;
    this.currentStorage = state.storage ?? this.currentStorage;
    this.relayoutMessage();
    this.generator = this.run();
  }

  /** Runtime descriptors surfaced in the engine panel. */
  describe(): Readonly<Record<string, unknown>> {
    return {
      pc: this.pc,
      statements: this.program.length,
      labels: this.labels.size,
      flags: this.flags.size,
      waitingOn: this.pendingWait?.label ?? null,
      selecting: this.selection !== null,
      /** Which script is running, since `@call`/`@jump` can switch files. */
      storage: this.currentStorage,
      ended: this.finished,
      /** Present only after an unrecoverable script error. */
      ...(this.fatalError ? { fatalError: this.fatalError } : {}),
      layers: this.deps.stage.describe(),
    };
  }
}

export interface KagSceneSnapshot {
  pc: number;
  storage?: string;
  flags?: Record<string, unknown>;
  systemFlags?: Record<string, unknown>;
  macroParameters?: Record<string, unknown>;
  message?: string;
  speaker?: string;
  history?: readonly HistoryEntry[];
}

function zIndexForLayer(name: string): number {
  const order: string[] = [
    KAG_LAYERS.base,
    KAG_LAYERS.layer0,
    KAG_LAYERS.layer1,
    KAG_LAYERS.layer2,
    KAG_LAYERS.message0,
    KAG_LAYERS.message1,
    KAG_LAYERS.message2,
    KAG_LAYERS.message3,
    KAG_LAYERS.fore,
  ];
  const index = order.indexOf(name);
  return index === -1 ? 45 : index * 10;
}

function numberAttr(tag: KagTag, name: string, fallback: number): number {
  const raw = tag.attrs.get(name);
  if (raw === undefined) return fallback;
  // Attributes may carry a `%[expr]` form; those are resolved before dispatch.
  return parseKagNumber(raw, fallback);
}

function booleanAttr(tag: KagTag, name: string, fallback: boolean): boolean {
  return parseKagBoolean(tag.attrs.get(name), fallback);
}

function stringAttr(tag: KagTag, name: string): string | undefined {
  return tag.attrs.get(name);
}

/** Normalises the several colour spellings KAG scripts use. */
export function normalizeColour(value: string): string {
  const trimmed = value.trim();
  if (trimmed.startsWith('#')) return trimmed;
  if (/^0x[0-9a-fA-F]{6}$/.test(trimmed)) return `#${trimmed.slice(2)}`;
  if (/^[0-9a-fA-F]{6}$/.test(trimmed)) return `#${trimmed}`;
  const parts = trimmed.split(',').map((part) => Number(part.trim()));
  if (parts.length >= 3 && parts.slice(0, 3).every((part) => Number.isFinite(part))) {
    return `rgb(${parts[0]}, ${parts[1]}, ${parts[2]})`;
  }
  return '#ffffff';
}
