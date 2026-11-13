import { PluginError } from './errors.js';
import type { PluginProvenance } from './manifest.js';

/**
 * What one plugin may spend.
 *
 * `memoryBytes` is an allocation budget for data the host hands to the plugin
 * (bytes it reads, bytes it writes). It is not a live heap measurement: the host
 * cannot see when a plugin drops a reference, so it only counts what it gave
 * away. A handle-returning API would add a release path; there is none yet.
 *
 * `timeMs` is the window for one activation or one hook call.
 */
export interface ResourceBudget {
  readonly memoryBytes: number;
  readonly timeMs: number;
}

/**
 * Defaults, kept modest and different per provenance.
 *
 * These are starting numbers, not a tuned policy: the final values belong with
 * the plugin spec that is still being written. They are overridable per host
 * (`PluginHostOptions.budgets`) and per plugin, so changing them later is not a
 * code change in the gate.
 */
export const DEFAULT_BUDGETS: Readonly<Record<PluginProvenance, ResourceBudget>> = {
  official: { memoryBytes: 32 * 1024 * 1024, timeMs: 5000 },
  'third-party': { memoryBytes: 8 * 1024 * 1024, timeMs: 1000 },
};

export interface BudgetUsage {
  readonly memoryBytes: number;
  /** Milliseconds consumed by the current window, or 0 before it starts. */
  readonly elapsedMs: number;
}

function over(pluginId: string, detail: string): PluginError {
  return new PluginError('budget-exceeded', `plugin "${pluginId}" exceeded its ${detail}`, pluginId);
}

/**
 * Cooperative budget accounting for in-process plugins.
 *
 * The word "cooperative" is the honest part: a synchronous `while (true)` inside
 * a plugin cannot be interrupted from here, so exceeding the time budget is only
 * detected when control comes back to the host. That is why every host-mediated
 * call checks the budget, and why the spec requires isolation before anything
 * stronger can be claimed.
 */
export class BudgetMeter {
  private windowStart: number | null = null;
  private memory = 0;

  constructor(
    readonly pluginId: string,
    readonly limits: ResourceBudget,
    private readonly clock: () => number = () => Date.now(),
  ) {}

  get usage(): BudgetUsage {
    return {
      memoryBytes: this.memory,
      elapsedMs: this.windowStart === null ? 0 : this.clock() - this.windowStart,
    };
  }

  /** Bytes still available to this plugin in the current session. */
  get remainingMemory(): number {
    return Math.max(0, this.limits.memoryBytes - this.memory);
  }

  /** Opens a fresh time window for one activation or one hook call. */
  startWindow(): void {
    this.windowStart = this.clock();
  }

  /** Throws when the current window is over. Called at every observable boundary. */
  checkpoint(): void {
    if (this.windowStart === null) return;
    const elapsed = this.clock() - this.windowStart;
    if (elapsed > this.limits.timeMs) {
      throw over(this.pluginId, `time budget (${elapsed}ms > ${this.limits.timeMs}ms)`);
    }
  }

  /**
   * Charges `bytes` to the plugin, refusing before the allocation happens.
   *
   * The check runs first so a refused call cannot leave the accounting inflated
   * and turn one rejected read into an unrecoverable plugin.
   */
  chargeMemory(bytes: number): void {
    if (!Number.isFinite(bytes) || bytes < 0) {
      throw new RangeError('memory charge must be a finite, non-negative number');
    }
    const next = this.memory + bytes;
    if (next > this.limits.memoryBytes) {
      throw over(this.pluginId, `memory budget (${next}B > ${this.limits.memoryBytes}B)`);
    }
    this.memory = next;
  }

  /** Only for a future handle-based API that can observe a release. */
  releaseMemory(bytes: number): void {
    this.memory = Math.max(0, this.memory - bytes);
  }
}
