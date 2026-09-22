import type {
  AudioChannel,
  InputEvent,
  LogLevel,
  LogRecord,
  RuntimeHost,
  RuntimeHostEvent,
  WritableStore,
} from '@online-gal/engine-api';

export interface PresenterOptions {
  /** Called for every log record the runtime emits. */
  readonly onLog?: (record: LogRecord) => void;
  /** Called for every host event the runtime emits. */
  readonly onEvent?: (event: RuntimeHostEvent) => void;
  /** Initial user settings. */
  readonly settings?: Readonly<Record<string, unknown>>;
}

/**
 * Owns the canvas, the input translation and the runtime host services.
 *
 * The canvas keeps a game-resolution coordinate system regardless of how the
 * element is laid out on screen: `width`/`height` attributes hold the game
 * surface, and CSS letterboxes it into the viewport. Runtime code therefore
 * never deals with device pixels, HiDPI ratios or window resizing — the same
 * separation AetherKiri keeps between its engine surface and its Godot
 * presentation layer.
 */
export class Presenter {
  private readonly logs: LogRecord[] = [];
  private readonly settings: Record<string, unknown>;
  private frameRequests = 0;

  constructor(
    readonly canvas: HTMLCanvasElement,
    private readonly options: PresenterOptions = {},
  ) {
    this.settings = { ...(options.settings ?? {}) };
  }

  /** Sizes the backing store to the game surface. */
  setSurfaceSize(width: number, height: number): void {
    if (this.canvas.width !== width) this.canvas.width = width;
    if (this.canvas.height !== height) this.canvas.height = height;
  }

  get surfaceSize(): { width: number; height: number } {
    return { width: this.canvas.width, height: this.canvas.height };
  }

  get context(): CanvasRenderingContext2D {
    const context = this.canvas.getContext('2d', { alpha: false });
    if (!context) throw new Error('2D canvas context is unavailable');
    return context;
  }

  /** Records a frame request; the host loop aggregates them per tick. */
  requestFrame(): void {
    this.frameRequests += 1;
  }

  /** Consumes and returns the number of frame requests since the last call. */
  takeFrameRequests(): number {
    const count = this.frameRequests;
    this.frameRequests = 0;
    return count;
  }

  setSetting(key: string, value: unknown): void {
    this.settings[key] = value;
  }

  getSetting<T>(key: string, fallback: T): T {
    const value = this.settings[key];
    return value === undefined ? fallback : (value as T);
  }

  record(level: LogLevel, runtimeId: string, message: string, data?: unknown): void {
    const record: LogRecord = { level, runtimeId, message, data, at: Date.now() };
    this.logs.push(record);
    if (this.logs.length > 500) this.logs.shift();
    this.options.onLog?.(record);
  }

  history(): readonly LogRecord[] {
    return this.logs;
  }

  emit(event: RuntimeHostEvent): void {
    this.options.onEvent?.(event);
  }
}

/** Console-backed logger used when no presenter is attached (tests, headless). */
export function consoleLogger(runtimeId: string) {
  return (level: LogLevel, message: string, data?: unknown): void => {
    const tag = `[${runtimeId}]`;
    if (level === 'error') console.error(tag, message, data ?? '');
    else if (level === 'warn') console.warn(tag, message, data ?? '');
    else if (level === 'debug' || level === 'trace') console.debug(tag, message, data ?? '');
    else console.info(tag, message, data ?? '');
  };
}

/**
 * Translates raw DOM events into the normalised `InputEvent` union.
 *
 * Coordinate mapping accounts for letterboxing, so a click on the black bar
 * beside the game is reported outside the surface bounds rather than being
 * silently clamped onto the edge — which is how the native players behave and
 * what the diagnostics panel needs to show.
 */
export class InputTranslator {
  constructor(private readonly canvas: HTMLCanvasElement) {}

  /** Maps client coordinates into game-surface coordinates. */
  toSurface(clientX: number, clientY: number): { x: number; y: number } {
    const rect = this.canvas.getBoundingClientRect();
    if (rect.width === 0 || rect.height === 0) return { x: 0, y: 0 };
    const x = ((clientX - rect.left) / rect.width) * this.canvas.width;
    const y = ((clientY - rect.top) / rect.height) * this.canvas.height;
    return { x: Math.round(x), y: Math.round(y) };
  }

  fromPointer(
    event: PointerEvent | MouseEvent,
    type: 'down' | 'up' | 'move',
  ): InputEvent {
    const { x, y } = this.toSurface(event.clientX, event.clientY);
    // Pointer identity is carried through so the gesture queue can pair a
    // press with its release; a mouse always reports 0, matching the DOM.
    const pointerId = (event as PointerEvent).pointerId ?? 0;
    if (type === 'move') {
      return {
        type: 'pointer-move',
        x,
        y,
        pointerId,
        pressed: ((event as PointerEvent).buttons ?? 0) !== 0,
      };
    }
    return type === 'down'
      ? { type: 'pointer-down', x, y, button: event.button, pointerId }
      : { type: 'pointer-up', x, y, button: event.button, pointerId };
  }

  fromWheel(event: WheelEvent): InputEvent {
    const { x, y } = this.toSurface(event.clientX, event.clientY);
    return { type: 'pointer-wheel', x, y, deltaY: event.deltaY };
  }

  static fromKeyboard(event: KeyboardEvent, type: 'down' | 'up'): InputEvent {
    return type === 'down'
      ? {
          type: 'key-down',
          key: event.key,
          code: event.code,
          repeat: event.repeat,
        }
      : { type: 'key-up', key: event.key, code: event.code };
  }
}

/** Attaches DOM listeners to a canvas and forwards normalised events. */
export function attachInput(
  canvas: HTMLCanvasElement,
  sink: (event: InputEvent) => void,
): () => void {
  const translator = new InputTranslator(canvas);

  const onPointerDown = (event: PointerEvent) => {
    canvas.focus();
    sink(translator.fromPointer(event, 'down'));
  };
  const onPointerUp = (event: PointerEvent) => sink(translator.fromPointer(event, 'up'));
  const onPointerMove = (event: PointerEvent) => sink(translator.fromPointer(event, 'move'));
  const onWheel = (event: WheelEvent) => {
    sink(translator.fromWheel(event));
    event.preventDefault();
  };
  const onKeyDown = (event: KeyboardEvent) => {
    sink(InputTranslator.fromKeyboard(event, 'down'));
    // Stop the page from scrolling while the game owns the keyboard.
    if (['ArrowUp', 'ArrowDown', 'ArrowLeft', 'ArrowRight', ' '].includes(event.key)) {
      event.preventDefault();
    }
  };
  const onKeyUp = (event: KeyboardEvent) => sink(InputTranslator.fromKeyboard(event, 'up'));
  const onBlur = () => sink({ type: 'blur' });
  const onContextMenu = (event: Event) => event.preventDefault();

  canvas.addEventListener('pointerdown', onPointerDown);
  canvas.addEventListener('pointerup', onPointerUp);
  canvas.addEventListener('pointermove', onPointerMove);
  canvas.addEventListener('wheel', onWheel, { passive: false });
  canvas.addEventListener('keydown', onKeyDown);
  canvas.addEventListener('keyup', onKeyUp);
  canvas.addEventListener('blur', onBlur);
  canvas.addEventListener('contextmenu', onContextMenu);

  return () => {
    canvas.removeEventListener('pointerdown', onPointerDown);
    canvas.removeEventListener('pointerup', onPointerUp);
    canvas.removeEventListener('pointermove', onPointerMove);
    canvas.removeEventListener('wheel', onWheel);
    canvas.removeEventListener('keydown', onKeyDown);
    canvas.removeEventListener('keyup', onKeyUp);
    canvas.removeEventListener('blur', onBlur);
    canvas.removeEventListener('contextmenu', onContextMenu);
  };
}

/** Builds a `RuntimeHost` for a runtime instance. */
export function createRuntimeHost(options: {
  presenter: Presenter;
  source: RuntimeHost['source'];
  store: WritableStore;
  canvas: HTMLCanvasElement;
  settings?: Readonly<Record<string, unknown>>;
  onEvent?: (event: RuntimeHostEvent) => void;
  runtimeId: string;
}): RuntimeHost {
  const { presenter } = options;
  return {
    source: options.source,
    store: options.store,
    canvas: options.canvas,
    get surface() {
      return presenter.surfaceSize;
    },
    requestFrame: () => presenter.requestFrame(),
    log: (level, message, data) =>
      presenter.record(level, options.runtimeId, message, data),
    now: () => performance.now(),
    notify: (event) => {
      presenter.emit(event);
      options.onEvent?.(event);
    },
    setting: <T,>(key: string, fallback: T): T => {
      const value = presenter.getSetting<unknown>(key, options.settings?.[key] ?? fallback);
      return (value ?? fallback) as T;
    },
  };
}

/** Convenience helper for channel volume defaults used across runtimes. */
export const DEFAULT_CHANNEL_VOLUMES: Readonly<Record<AudioChannel, number>> = {
  bgm: 0.8,
  se: 0.9,
  voice: 1,
  system: 0.7,
};
