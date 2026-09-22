import {
  OPTION_COMPATIBILITY_REPORT,
  OPTION_STRICT_MODE,
} from './options.js';
import {
  summariseCompatibility,
  type CompatibilityReport,
  type RuntimeId,
  type UnsupportedCommand,
} from './types.js';

export interface CommandTrackerOptions {
  readonly runtimeId: RuntimeId;
  /** Reports an unsupported command; called once per distinct command. */
  readonly onUnsupported: (entry: UnsupportedCommand) => void;
  /** When true, a skipped command is escalated to an error-level log. */
  readonly strict: boolean;
  /** When false, tallies are dropped and `report()` returns null. */
  readonly enabled: boolean;
}

/**
 * Tallies how much of a game's script a runtime actually honoured.
 *
 * This is the browser analogue of AetherKiri's plugin debug info. A native
 * build substitutes mock objects for plugins it cannot load and reports them,
 * which is how its per-game compatibility notes are produced; with no plugins
 * to mock, the equivalent signal is the set of script commands an engine issued
 * that the runtime could not implement. Without it a game either "runs" or
 * throws, and a game that runs *wrong* is invisible.
 */
export class CommandTracker {
  private readonly entries = new Map<string, UnsupportedCommand>();
  private honouredCount = 0;
  private ignored = 0;

  private constructor(private readonly options: CommandTrackerOptions) {}

  /**
   * Builds a tracker from a runtime's option lookup.
   *
   * `strict` mirrors AetherKiri's `mock_enabled=0`: instead of absorbing an
   * unsupported command quietly, it is escalated to an error so a debugging
   * session surfaces it immediately.
   */
  static fromRuntime(
    runtimeId: RuntimeId,
    setting: <T>(key: string, fallback: T) => T,
    onLog: (level: 'warn' | 'error', message: string) => void,
  ): CommandTracker {
    const enabled = setting<boolean>(OPTION_COMPATIBILITY_REPORT, true);
    const strict = setting<boolean>(OPTION_STRICT_MODE, false);
    return new CommandTracker({
      runtimeId,
      enabled,
      strict,
      onUnsupported: (entry) => {
        if (entry.handling === 'approximated') {
          onLog(
            strict ? 'error' : 'warn',
            `command "${entry.command}" was approximated: ${entry.note ?? 'no note given'}`,
          );
          return;
        }
        const where = entry.line !== undefined ? ` at line ${entry.line}` : '';
        onLog(
          strict ? 'error' : 'warn',
          `unsupported command "${entry.command}"${where}`,
        );
      },
    });
  }

  /** Records a command the runtime honoured. */
  honoured(_command: string): void {
    if (!this.options.enabled) return;
    this.honouredCount += 1;
  }

  /**
   * Records a command the runtime could not honour.
   *
   * Only the first occurrence reaches the log: a script that issues a missing
   * command usually issues it thousands of times, and logging each one would
   * bury everything else.
   */
  unsupported(
    command: string,
    handling: 'skipped' | 'approximated',
    detail: { line?: number; note?: string } = {},
  ): void {
    if (!this.options.enabled) return;
    this.ignored += 1;
    const existing = this.entries.get(command);
    if (existing) {
      this.entries.set(command, {
        ...existing,
        count: existing.count + 1,
        ...(existing.line === undefined && detail.line !== undefined
          ? { line: detail.line }
          : {}),
      });
      return;
    }
    const entry: UnsupportedCommand = {
      command,
      count: 1,
      handling,
      ...(detail.line !== undefined ? { line: detail.line } : {}),
      ...(detail.note !== undefined ? { note: detail.note } : {}),
    };
    this.entries.set(command, entry);
    this.options.onUnsupported(entry);
  }

  /** True when at least one command was skipped outright. */
  get hasSkipped(): boolean {
    for (const entry of this.entries.values()) {
      if (entry.handling === 'skipped') return true;
    }
    return false;
  }

  /** Total command invocations the runtime did not fully honour. */
  get unsupportedCount(): number {
    return this.ignored;
  }

  /** Builds the report, or null when reporting is disabled. */
  report(): CompatibilityReport | null {
    if (!this.options.enabled) return null;
    return summariseCompatibility(
      this.options.runtimeId,
      [...this.entries.values()].sort((a, b) => b.count - a.count),
      this.honouredCount,
    );
  }
}
