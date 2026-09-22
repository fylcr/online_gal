export interface GameLoopOptions {
  /** Called once per frame with the wall-clock delta in milliseconds. */
  readonly onTick: (deltaMs: number) => void;
  /** Called after `onTick`; put all drawing here. */
  readonly onRender: () => void;
  /** Clamp for long stalls (tab switch, debugger pause) so timers do not jump. */
  readonly maxDeltaMs?: number;
  /** Frames per second cap; 0 disables the cap. */
  readonly targetFps?: number;
}

/**
 * The host frame loop.
 *
 * A single `requestAnimationFrame` pump shared by every runtime keeps audio,
 * transitions and the typewriter effect in step with the display, and keeps the
 * runtime free of any scheduling concerns. Long stalls are clamped so that a
 * backgrounded tab does not fast-forward the game on return — the same guard
 * the native players apply to their tick clocks.
 */
export class GameLoop {
  private handle: number | null = null;
  private lastTime = 0;
  private accumulator = 0;
  private running = false;
  private readonly maxDelta: number;
  private readonly frameInterval: number;

  constructor(private readonly options: GameLoopOptions) {
    this.maxDelta = options.maxDeltaMs ?? 100;
    this.frameInterval = options.targetFps ? 1000 / options.targetFps : 0;
  }

  get isRunning(): boolean {
    return this.running;
  }

  start(): void {
    if (this.running) return;
    this.running = true;
    this.lastTime = performance.now();
    this.accumulator = 0;
    const step = (time: number) => {
      if (!this.running) return;
      this.handle = requestAnimationFrame(step);

      const delta = time - this.lastTime;
      if (this.frameInterval > 0) {
        this.accumulator += delta;
        if (this.accumulator < this.frameInterval) return;
        this.accumulator = 0;
      }
      this.lastTime = time;

      const clamped = Math.min(Math.max(0, delta), this.maxDelta);
      this.options.onTick(clamped);
      this.options.onRender();
    };
    this.handle = requestAnimationFrame(step);
  }

  stop(): void {
    this.running = false;
    if (this.handle !== null) {
      cancelAnimationFrame(this.handle);
      this.handle = null;
    }
  }

  dispose(): void {
    this.stop();
  }
}

/** Eases used by transitions, mirroring KAG3's `trans` methods. */
export const Easing = {
  linear: (t: number) => t,
  easeIn: (t: number) => t * t,
  easeOut: (t: number) => 1 - (1 - t) * (1 - t),
  easeInOut: (t: number) => (t < 0.5 ? 2 * t * t : 1 - 2 * (1 - t) * (1 - t)),
  /** KAG3's default "crossfade" hop: quick in, quick out. */
  crossFade: (t: number) => (t < 0.5 ? 2 * t * t : 1 - 2 * (1 - t) * (1 - t)),
} as const;

export type EasingName = keyof typeof Easing;

export function easingFor(name: string): (t: number) => number {
  if (name in Easing) return Easing[name as EasingName];
  // KAG spells several transitions differently from the canvas vocabulary.
  switch (name.toLowerCase()) {
    case 'crossfade':
    case 'cross':
      return Easing.easeInOut;
    case 'universal':
    case 'universalrule':
      return Easing.easeInOut;
    case 'fadein':
      return Easing.linear;
    case 'slide':
    case 'slidein':
      return Easing.easeOut;
    default:
      return Easing.easeInOut;
  }
}
