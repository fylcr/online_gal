import { PluginError } from './errors.js';
import type { PluginProvenance } from './manifest.js';

/**
 * Everything a plugin can ask the host for.
 *
 * The list is deliberately short: each entry is a surface the host has to
 * guard, audit and test. A capability that is not here does not exist — there
 * is no "raw access" fallback and no implicit grant.
 *
 * - `vfs.read`       read the game's read-only files, root-bounded.
 * - `vfs.write`      read/write a private overlay under `plugins/<pluginId>/`.
 * - `net.request`    call the host-injected proxy; the host owns allowlists.
 * - `ui.notify`      surface a message through the host's own channel.
 * - `plugins.manage` inspect, grant, revoke and unload other plugins (privileged).
 */
export const PLUGIN_CAPABILITIES = [
  'vfs.read',
  'vfs.write',
  'net.request',
  'ui.notify',
  'plugins.manage',
] as const;

export type PluginCapability = (typeof PLUGIN_CAPABILITIES)[number];

/**
 * Capabilities that hand over more than a guarded surface.
 *
 * `plugins.manage` gives control over every other plugin. That cannot be
 * contained by per-call checks, so it is trust, not sandboxing: only an
 * `official` manifest may declare it, it is never part of a default grant,
 * and the host has to approve it for a specific plugin id.
 */
export const PRIVILEGED_CAPABILITIES: readonly PluginCapability[] = ['plugins.manage'];

export function isPluginCapability(value: unknown): value is PluginCapability {
  return typeof value === 'string' && (PLUGIN_CAPABILITIES as readonly string[]).includes(value);
}

export function isPrivilegedCapability(capability: PluginCapability): boolean {
  return PRIVILEGED_CAPABILITIES.includes(capability);
}

/**
 * Default grants per provenance.
 *
 * Third-party plugins start with nothing, as the boundary spec requires. The
 * official set is not a trust shortcut: it still only applies to capabilities
 * the manifest declares, so an official plugin that does not declare `vfs.read`
 * never receives it.
 */
export const DEFAULT_GRANTS: Readonly<Record<PluginProvenance, readonly PluginCapability[]>> = {
  official: ['vfs.read', 'ui.notify'],
  'third-party': [],
};

export type GrantAction = 'default' | 'grant' | 'revoke' | 'deny';

export interface GrantEvent {
  readonly at: number;
  readonly action: GrantAction;
  readonly capability: PluginCapability;
  readonly reason?: string;
}

export interface GrantLedgerOptions {
  /** Host approvals beyond the provenance defaults. Must be declared first. */
  readonly approvals?: readonly PluginCapability[];
  readonly clock?: () => number;
}

/**
 * One plugin's capability ledger for one load session.
 *
 * Grants are checked on *every* host API call rather than copied into the
 * context, because a plugin can stash the context object in a closure: the
 * only revocation that works is one the next call actually observes.
 */
export class GrantLedger {
  private readonly granted = new Set<PluginCapability>();
  private readonly requested: ReadonlySet<PluginCapability>;
  private readonly events: GrantEvent[] = [];
  private readonly clock: () => number;
  private closed = false;

  constructor(
    readonly pluginId: string,
    provenance: PluginProvenance,
    requested: readonly PluginCapability[],
    options: GrantLedgerOptions = {},
  ) {
    this.requested = new Set(requested);
    this.clock = options.clock ?? (() => Date.now());
    for (const capability of DEFAULT_GRANTS[provenance]) {
      if (!this.requested.has(capability)) continue;
      this.granted.add(capability);
      this.record('default', capability);
    }
    for (const capability of options.approvals ?? []) this.grant(capability);
  }

  get capabilities(): readonly PluginCapability[] {
    return [...this.granted];
  }

  get history(): readonly GrantEvent[] {
    return [...this.events];
  }

  get denialCount(): number {
    return this.events.filter((event) => event.action === 'deny').length;
  }

  get isClosed(): boolean {
    return this.closed;
  }

  has(capability: PluginCapability): boolean {
    return !this.closed && this.granted.has(capability);
  }

  grant(capability: PluginCapability): void {
    this.assertOpen();
    if (!this.requested.has(capability)) {
      throw new PluginError(
        'capability-not-requested',
        `plugin "${this.pluginId}" did not declare capability "${capability}" in its manifest`,
        this.pluginId,
      );
    }
    if (this.granted.has(capability)) return;
    this.granted.add(capability);
    this.record('grant', capability);
  }

  revoke(capability: PluginCapability): void {
    if (!this.granted.delete(capability)) return;
    this.record('revoke', capability);
  }

  /**
   * Throws unless the capability is currently granted. Called by every host API
   * method, so revocation takes effect on the next call and denials are counted.
   */
  require(capability: PluginCapability): void {
    if (this.has(capability)) return;
    const reason = this.closed ? 'session ended' : 'not granted';
    this.record('deny', capability, reason);
    throw new PluginError(
      'capability-denied',
      `plugin "${this.pluginId}" cannot use "${capability}" (${reason})`,
      this.pluginId,
    );
  }

  /** Ends the session: every grant is dropped and cannot be re-granted. */
  close(): void {
    if (this.closed) return;
    for (const capability of this.granted) this.record('revoke', capability, 'session ended');
    this.granted.clear();
    this.closed = true;
  }

  private assertOpen(): void {
    if (this.closed) {
      throw new PluginError(
        'capability-denied',
        `plugin "${this.pluginId}" session has ended; grants are no longer possible`,
        this.pluginId,
      );
    }
  }

  private record(action: GrantAction, capability: PluginCapability, reason?: string): void {
    this.events.push({
      at: this.clock(),
      action,
      capability,
      ...(reason === undefined ? {} : { reason }),
    });
  }
}
