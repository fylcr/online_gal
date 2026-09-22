/**
 * Bounded input queue for gesture semantics.
 *
 * Ported from AetherKiri's `bridge/engine_api/src/engine_input_queue_gate.h`.
 *
 * The problem is the same in both hosts: a click can synchronously start an
 * expensive transition, and clicks arriving while the engine is busy must not
 * be lost — but they must also not accumulate without limit, or a burst during
 * a transition replays as a long stale backlog once the engine catches up and
 * the player sees clicks "firing on their own".
 *
 * AetherKiri's version additionally de-duplicates one physical press that Godot
 * reports twice (a global callback plus a Control callback). That hazard does
 * not exist here — `attachInput` binds one listener to one canvas, so a pointer
 * yields exactly one `pointerdown` — but the guard is kept generalised so that
 * a future second input source (a bridged iframe, a second canvas) inherits it.
 *
 * The decision API mirrors the original: `shouldQueue` answers "hold this back",
 * and the caller delivers everything else immediately.
 */

/** Upper bound on complete gestures held while the engine is busy. */
export const MAX_QUEUED_GESTURES = 8;

/** The subset of an input event this gate needs to reason about. */
export interface GateableEvent {
  readonly type: 'pointer-down' | 'pointer-up' | 'pointer-move' | string;
  /** Pointer identity; the DOM mouse defaults to 0. */
  readonly pointerId?: number;
  /** Mouse button; 0 is the primary button. */
  readonly button?: number;
}

export interface GestureQueueOptions {
  /**
   * True while the engine cannot process input promptly — a transition, a
   * blocking wait, a modal. The gate only holds events in that state, so
   * latency stays at zero while the game is idle.
   */
  readonly isBusy: () => boolean;
  /** Called when a gesture is dropped because the backlog was full. */
  readonly onOverflow?: (droppedTotal: number) => void;
}

/**
 * Decides which input events to hold back while the engine is busy.
 *
 * The unit of queueing is a *complete* gesture, never a lone event: a queued
 * `down` whose `up` never follows would leave the engine believing a button is
 * still held, which in a visual novel shows up as a permanently advancing
 * message. A gesture therefore enters the queue only once its `up` arrives, and
 * a gesture interrupted by `reset()` is discarded rather than replayed.
 */
export class InputGestureQueue {
  /** Gestures being accumulated, keyed by pointer id. */
  private readonly pending = new Map<number, GateableEvent[]>();
  /** Complete gestures awaiting replay, oldest first. */
  private readonly queued: GateableEvent[][] = [];
  private dropped = 0;

  constructor(private readonly options: GestureQueueOptions) {}

  /** Complete gestures waiting to be replayed. */
  get size(): number {
    return this.queued.length;
  }

  /** Gestures dropped because the backlog was full. */
  get droppedCount(): number {
    return this.dropped;
  }

  /**
   * Returns true when the caller must hold this event back.
   *
   * Events that cannot form part of a gesture (wheel, keys, text) are only held
   * when the engine is busy and there is a queue to preserve ordering in.
   */
  shouldQueue(event: GateableEvent): boolean {
    const pointerId = event.pointerId ?? 0;
    const busy = this.options.isBusy();

    switch (event.type) {
      case 'pointer-down': {
        // A second `down` for a pointer already down is a duplicate or a lost
        // `up`; accepting it would strand the earlier gesture. Swallow it.
        if (this.pending.has(pointerId)) return true;
        this.pending.set(pointerId, [event]);
        return busy;
      }

      case 'pointer-move': {
        const open = this.pending.get(pointerId);
        if (!open) {
          // Movement with no press in flight is hover/aim; never queued, and
          // not worth delivering while the engine is mid-transition.
          return busy;
        }
        open.push(event);
        return busy;
      }

      case 'pointer-up': {
        const open = this.pending.get(pointerId);
        if (!open) {
          // An `up` with no matching `down` cannot form a gesture.
          return true;
        }
        open.push(event);
        this.pending.delete(pointerId);

        if (!busy) return false;

        if (this.queued.length >= MAX_QUEUED_GESTURES) {
          // Drop the newest gesture rather than the oldest: the oldest carries
          // the player's earlier intent, and discarding it would replay a later
          // click first.
          this.dropped += 1;
          this.options.onOverflow?.(this.dropped);
          return true;
        }
        this.queued.push(open);
        return true;
      }

      default:
        // Wheel is idempotent and keys are already coalesced by the OS repeat
        // rules, so holding them only matters for ordering.
        return busy;
    }
  }

  /**
   * Takes every queued event in original order, for replay once idle.
   *
   * A recorded gesture is flattened back into its events, because the runtime's
   * input handler consumes one event at a time.
   */
  drain(): readonly GateableEvent[] {
    const flushed: GateableEvent[] = [];
    for (const gesture of this.queued.splice(0, this.queued.length)) {
      flushed.push(...gesture);
    }
    return flushed;
  }

  /**
   * Discards in-flight and queued input.
   *
   * Called on blur and on teardown: a `down` whose `up` will never arrive would
   * otherwise be replayed as a permanent press.
   */
  reset(): void {
    this.pending.clear();
    this.queued.length = 0;
    this.dropped = 0;
  }
}
