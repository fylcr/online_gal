/**
 * Stable error codes for every way a plugin can be rejected, denied or stopped.
 *
 * The codes are the contract, not the messages: the UI and the tests branch on
 * `code`, while `message` stays free to get better. Everything a plugin (or the
 * developer loading one) can trip over has to land on one of these, because a
 * failure that cannot be named cannot be shown or tested.
 */
export type PluginErrorCode =
  | 'manifest-invalid'
  | 'api-version-mismatch'
  | 'duplicate-plugin-id'
  | 'unknown-plugin'
  | 'capability-denied'
  | 'capability-not-requested'
  | 'capability-unavailable'
  | 'path-outside-root'
  | 'budget-exceeded'
  | 'plugin-failed'
  /** A required service has no provider, or the requirements form a cycle. */
  | 'dependency-missing'
  /** A provider this plugin depends on failed or was unloaded. */
  | 'dependency-failed'
  /** Two plugins provide the same service. */
  | 'service-conflict'
  /** `setup` did not return exactly the services the manifest promised. */
  | 'service-invalid';

export class PluginError extends Error {
  constructor(
    readonly code: PluginErrorCode,
    message: string,
    readonly pluginId: string | null = null,
    cause?: unknown,
  ) {
    super(message, cause === undefined ? undefined : { cause });
    this.name = 'PluginError';
  }
}

/** Wraps an arbitrary thrown value so callers always get a coded error. */
export function asPluginError(error: unknown, pluginId: string | null = null): PluginError {
  if (error instanceof PluginError) return error;
  const message = error instanceof Error ? error.message : String(error);
  return new PluginError('plugin-failed', message, pluginId, error);
}
