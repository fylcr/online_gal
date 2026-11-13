import type { GameSource, WritableStore } from '@online-gal/engine-api';
import { BudgetMeter, DEFAULT_BUDGETS, type BudgetUsage, type ResourceBudget } from './budget.js';
import {
  GrantLedger,
  isPrivilegedCapability,
  type PluginCapability,
} from './capabilities.js';
import { asPluginError, PluginError, type PluginErrorCode } from './errors.js';
import {
  HostApi,
  type HostNetworkProxy,
  type KernelBridge,
  type PluginContext,
  type PluginLogRecord,
  type PluginManager,
} from './host-api.js';
import {
  assertApiCompatible,
  parseManifest,
  type PluginManifest,
  type PluginProvenance,
} from './manifest.js';

export type PluginHookHandler = (
  payload: unknown,
  context: PluginContext,
) => unknown | Promise<unknown>;

/**
 * What `setup` may return: one implementation per service in `provides`.
 * Plugins that provide nothing return nothing.
 */
export type PluginServices = Readonly<Record<string, object>>;

/** The plugin's code, as `load()` resolves it. */
export interface PluginModule {
  readonly setup: (context: PluginContext) => void | PluginServices | Promise<void | PluginServices>;
  readonly dispose?: () => void | Promise<void>;
  readonly hooks?: Readonly<Record<string, PluginHookHandler>>;
}

/**
 * What the page hands the kernel.
 *
 * The manifest travels separately from the code on purpose: the kernel checks
 * the manifest, the API version, the id and the dependency graph first, and
 * only calls `load()` — typically a dynamic `import()` — for a plugin that has
 * passed all of it. A rejected plugin's code is never even fetched.
 */
export interface PluginEntry {
  /** Untrusted until the gate passes. */
  readonly manifest: unknown;
  readonly load: () => PluginModule | Promise<PluginModule>;
}

export type PluginState = 'loaded' | 'active' | 'failed' | 'unloaded';

export interface PluginFailure {
  readonly code: PluginErrorCode;
  readonly message: string;
}

export interface PluginRegistration {
  readonly pluginId: string;
  readonly status: 'active' | 'failed';
  readonly capabilities: readonly PluginCapability[];
  readonly failure?: PluginFailure;
}

export interface PluginRuntimeInfo {
  readonly pluginId: string;
  readonly name: string;
  readonly version: string;
  readonly provenance: PluginProvenance;
  readonly state: PluginState;
  readonly capabilities: readonly PluginCapability[];
  readonly provides: readonly string[];
  readonly requires: readonly string[];
  readonly denials: number;
  readonly budget: BudgetUsage;
  readonly failure?: PluginFailure;
}

export interface PluginHookResult {
  readonly pluginId: string;
  readonly ok: boolean;
  readonly value?: unknown;
  readonly failure?: PluginFailure;
}

export interface PluginHostOptions {
  /** The mounted game. Absent at page open; `vfs` reports unavailable until set. */
  readonly source?: GameSource;
  readonly store?: WritableStore;
  readonly notify?: (message: string, level: PluginLogRecord['level']) => void;
  readonly net?: HostNetworkProxy;
  readonly logger?: (record: PluginLogRecord) => void;
  readonly clock?: () => number;
  /** Per-plugin budget overrides, merged over the provenance defaults. */
  readonly budgets?: Readonly<Record<string, Partial<ResourceBudget>>>;
  /**
   * Host approvals beyond the provenance defaults, per plugin id. This is the
   * only way a privileged capability (`plugins.manage`) is granted
   * at boot. Undeclared entries are refused.
   */
  readonly grants?: Readonly<Record<string, readonly PluginCapability[]>>;
}

interface PluginRecord {
  readonly entry: PluginEntry;
  readonly manifest: PluginManifest;
  readonly ledger: GrantLedger;
  readonly meter: BudgetMeter;
  readonly context: PluginContext;
  /** Manager subscriptions owned by this plugin; dropped when it stops. */
  readonly listeners: Set<() => void>;
  module?: PluginModule;
  services: ReadonlyMap<string, object>;
  state: PluginState;
  /** Activation sequence; teardown runs in reverse. */
  order: number;
  stopping: boolean;
  failure?: PluginFailure;
}

type LoadOutcome = { readonly module: PluginModule } | { readonly error: unknown };

function toFailure(error: unknown, pluginId: string | null = null): PluginFailure {
  const pluginError = asPluginError(error, pluginId);
  return { code: pluginError.code, message: pluginError.message };
}

function idOf(entry: unknown): string {
  if (typeof entry === 'object' && entry !== null) {
    const manifest = (entry as { manifest?: unknown }).manifest;
    if (typeof manifest === 'object' && manifest !== null) {
      const id = (manifest as { id?: unknown }).id;
      if (typeof id === 'string' && id.length > 0) return id;
    }
  }
  return '(unknown)';
}

function isLive(record: PluginRecord): boolean {
  return record.state === 'active' || record.state === 'loaded';
}

/**
 * The plugin kernel.
 *
 * It is the first thing a page starts and owns nothing but the rules: every
 * feature — the manager (the page's interface), engines, the library — arrives
 * as a plugin through `boot`. Per plugin the order is fixed: manifest → API version
 * → duplicate id → grants → budget → dependency graph → `load()` → `setup`.
 * Nothing of a plugin is fetched or run before every check that can reject it
 * has passed, and a rejection leaves the kernel exactly as it was.
 */
export class PluginHost {
  private readonly records = new Map<string, PluginRecord>();
  private readonly refused: PluginRegistration[] = [];
  private readonly hostListeners = new Set<() => void>();
  private readonly options: PluginHostOptions;
  private readonly bridge: KernelBridge;
  private sequence = 0;
  private queue: Promise<unknown> = Promise.resolve();

  constructor(options: PluginHostOptions = {}) {
    this.options = options;
    this.bridge = {
      use: (pluginId, service) => this.serviceFor(pluginId, service),
      manager: (pluginId) => this.managerFor(pluginId),
    };
  }

  /** Registrations that never became a plugin record, e.g. bad manifests. */
  get rejections(): readonly PluginRegistration[] {
    return [...this.refused];
  }

  /** Every plugin the kernel knows about, including failed and unloaded ones. */
  get plugins(): readonly PluginRuntimeInfo[] {
    return [...this.records.values()].map((record) => ({
      pluginId: record.manifest.id,
      name: record.manifest.name,
      version: record.manifest.version,
      provenance: record.manifest.provenance,
      state: record.state,
      capabilities: record.ledger.capabilities,
      provides: record.manifest.provides ?? [],
      requires: record.manifest.requires ?? [],
      denials: record.ledger.denialCount,
      budget: record.meter.usage,
      ...(record.failure === undefined ? {} : { failure: record.failure }),
    }));
  }

  get(pluginId: string): PluginRuntimeInfo | undefined {
    return this.plugins.find((plugin) => plugin.pluginId === pluginId);
  }

  /** Called after any plugin changes state, for the page's fallback surface. */
  subscribe(listener: () => void): () => void {
    this.hostListeners.add(listener);
    return () => this.hostListeners.delete(listener);
  }

  /**
   * Gates, orders and activates a set of plugins, providers before consumers.
   *
   * Results come back in input order, one per entry, and never throw: the
   * page has to report every plugin individually, and an exception would turn
   * one bad entry into a page that never boots. Boots are serialised so two
   * batches cannot both claim the same service.
   *
   * Within a batch, plugins whose providers are already active fetch their
   * code together, one dependency level at a time; set-up still runs one
   * plugin at a time in dependency order, so the activation order does not
   * depend on which download finished first.
   */
  boot(entries: readonly PluginEntry[]): Promise<readonly PluginRegistration[]> {
    const run = this.queue.then(
      () => this.bootNow(entries),
      () => this.bootNow(entries),
    );
    this.queue = run.catch(() => undefined);
    return run;
  }

  /** Boots a single plugin; its required services must already be active. */
  async register(entry: PluginEntry): Promise<PluginRegistration> {
    const [registration] = await this.boot([entry]);
    return registration!;
  }

  grant(pluginId: string, capability: PluginCapability): void {
    this.require(pluginId).ledger.grant(capability);
    this.emit();
  }

  revoke(pluginId: string, capability: PluginCapability): void {
    this.require(pluginId).ledger.revoke(capability);
    this.emit();
  }

  /**
   * Forwards `hook` to every active plugin that registered a handler for it,
   * in activation order.
   *
   * One plugin's throw fails that plugin alone: it is stopped, its grants are
   * dropped and `dispose` runs, while the remaining plugins still get the
   * payload and their results still come back in the list.
   */
  async dispatch(hook: string, payload?: unknown): Promise<readonly PluginHookResult[]> {
    const results: PluginHookResult[] = [];
    for (const record of this.byOrder()) {
      if (record.state !== 'active') continue;
      const handler = record.module?.hooks?.[hook];
      if (handler === undefined) continue;

      record.meter.startWindow();
      try {
        const value = await handler(payload, record.context);
        // The only place an overrun is observable: control came back to us.
        record.meter.checkpoint();
        results.push({ pluginId: record.manifest.id, ok: true, value });
      } catch (error) {
        const failure = toFailure(error, record.manifest.id);
        await this.fail(record, failure);
        results.push({ pluginId: record.manifest.id, ok: false, failure });
      }
    }
    return results;
  }

  /**
   * Unloads a plugin. Plugins that require one of its services are failed
   * with `dependency-failed` first, while it can still answer their `dispose`.
   */
  async unload(pluginId: string): Promise<void> {
    const record = this.require(pluginId);
    if (!isLive(record)) return;
    await this.stop(record, 'unloaded');
  }

  /** Unloads everything, consumers before providers. The kernel stays usable. */
  async dispose(): Promise<void> {
    for (const record of this.byOrder().reverse()) {
      if (isLive(record)) await this.stop(record, 'unloaded');
    }
  }

  private async bootNow(entries: readonly PluginEntry[]): Promise<readonly PluginRegistration[]> {
    const results: (PluginRegistration | PluginRecord)[] = [];
    const candidates: PluginRecord[] = [];
    const batch = new Set<string>();

    // 1. Gate every manifest. Nothing here runs plugin code.
    for (const entry of entries) {
      let manifest: PluginManifest;
      try {
        manifest = parseManifest(entry?.manifest);
        assertApiCompatible(manifest);
      } catch (error) {
        results.push(this.refuse(entry, error));
        continue;
      }
      const existing = this.records.get(manifest.id);
      if (batch.has(manifest.id) || (existing !== undefined && isLive(existing))) {
        results.push(
          this.refuse(
            entry,
            new PluginError(
              'duplicate-plugin-id',
              `plugin "${manifest.id}" is already registered (${existing?.state ?? 'in this boot'})`,
              manifest.id,
            ),
          ),
        );
        continue;
      }
      let record: PluginRecord;
      try {
        record = this.createRecord(entry, manifest);
      } catch (error) {
        results.push(this.refuse(entry, error));
        continue;
      }
      batch.add(manifest.id);
      // Replace the previous, dead record only now that the new one is viable.
      this.records.set(manifest.id, record);
      candidates.push(record);
      results.push(record);
    }

    // 2. One provider per service: what is already active wins, then input order.
    const providers = new Map<string, PluginRecord>();
    for (const record of this.records.values()) {
      if (record.state !== 'active') continue;
      for (const service of record.manifest.provides ?? []) providers.set(service, record);
    }
    for (const record of candidates) {
      const taken = (record.manifest.provides ?? []).find((service) => providers.has(service));
      if (taken !== undefined) {
        await this.fail(record, {
          code: 'service-conflict',
          message: `service "${taken}" is already provided by "${providers.get(taken)!.manifest.id}"`,
        });
        continue;
      }
      for (const service of record.manifest.provides ?? []) providers.set(service, record);
    }

    // 3. Every requirement needs a provider that is, or may still become, active.
    for (let changed = true; changed; ) {
      changed = false;
      for (const record of candidates) {
        if (record.state !== 'loaded') continue;
        const failure = this.unmet(record, providers, false);
        if (failure !== undefined) {
          await this.fail(record, failure);
          changed = true;
        }
      }
    }

    // 4. Providers before consumers, input order otherwise. Leftovers are cycles.
    const ordered: PluginRecord[] = [];
    const placed = new Set<PluginRecord>();
    for (let progress = true; progress; ) {
      progress = false;
      for (const record of candidates) {
        if (record.state !== 'loaded' || placed.has(record)) continue;
        const ready = (record.manifest.requires ?? []).every((service) => {
          const provider = providers.get(service)!;
          return provider.state === 'active' || placed.has(provider);
        });
        if (ready) {
          ordered.push(record);
          placed.add(record);
          progress = true;
        }
      }
    }
    const cyclic = candidates.filter((record) => record.state === 'loaded' && !placed.has(record));
    for (const record of cyclic) {
      await this.fail(record, {
        code: 'dependency-missing',
        message: `dependency cycle among ${cyclic.map((entry) => `"${entry.manifest.id}"`).join(', ')}`,
      });
    }

    // 5. Load level by level. Everything whose providers are active fetches its
    //    code at once; a consumer's code is fetched only after its providers are
    //    active, so a provider that fails still takes its consumers with it
    //    unfetched. Topological order guarantees each pass makes progress.
    let pending = ordered;
    while (pending.length > 0) {
      const level: PluginRecord[] = [];
      const waiting: PluginRecord[] = [];
      for (const record of pending) {
        if (record.state !== 'loaded') continue;
        const requires = record.manifest.requires ?? [];
        if (requires.some((service) => providers.get(service)!.state === 'loaded')) {
          waiting.push(record);
          continue;
        }
        const failure = this.unmet(record, providers, true);
        if (failure !== undefined) await this.fail(record, failure);
        else level.push(record);
      }
      const fetched = level.map((record) => this.fetch(record));
      for (const [index, record] of level.entries()) {
        await this.activate(record, fetched[index]!, providers);
      }
      if (level.length === 0) {
        // Unreachable while `ordered` is topological; never spin if it is not.
        for (const record of waiting) {
          await this.fail(record, { code: 'dependency-missing', message: 'dependency order could not be resolved' });
        }
        break;
      }
      pending = waiting;
    }

    this.emit();
    return results.map((result) => ('entry' in result ? this.registrationOf(result) : result));
  }

  private unmet(
    record: PluginRecord,
    providers: ReadonlyMap<string, PluginRecord>,
    mustBeActive: boolean,
  ): PluginFailure | undefined {
    for (const service of record.manifest.requires ?? []) {
      const provider = providers.get(service);
      if (provider === undefined) {
        return { code: 'dependency-missing', message: `no plugin provides required service "${service}"` };
      }
      if (mustBeActive ? provider.state !== 'active' : !isLive(provider)) {
        return {
          code: 'dependency-failed',
          message: `required service "${service}" is unavailable: provider "${provider.manifest.id}" is ${provider.state}`,
        };
      }
    }
    return undefined;
  }

  /**
   * Starts fetching a plugin's code. It never rejects: a level starts every
   * fetch first and awaits them one by one, and a rejection nobody is awaiting
   * yet must not surface as an unhandled one.
   */
  private fetch(record: PluginRecord): Promise<LoadOutcome> {
    return (async () => record.entry.load())().then(
      (module) => ({ module }),
      (error: unknown) => ({ error }),
    );
  }

  private async activate(
    record: PluginRecord,
    fetched: Promise<LoadOutcome>,
    providers: ReadonlyMap<string, PluginRecord>,
  ): Promise<void> {
    const id = record.manifest.id;
    // Fetching code is not charged to the time budget: network latency is the
    // host's, not the plugin's.
    const outcome = await fetched;
    // An earlier set-up in this level may have stopped this plugin or a provider.
    if (record.state !== 'loaded') return;
    if ('error' in outcome) {
      await this.fail(record, toFailure(outcome.error, id));
      return;
    }
    if (typeof outcome.module?.setup !== 'function') {
      await this.fail(record, { code: 'plugin-failed', message: `plugin "${id}" module has no setup()` });
      return;
    }
    const unmet = this.unmet(record, providers, true);
    if (unmet !== undefined) {
      await this.fail(record, unmet);
      return;
    }
    record.module = outcome.module;

    try {
      record.meter.startWindow();
      const returned = await record.module.setup(record.context);
      record.meter.checkpoint();
      record.services = this.publish(record, returned);
      record.state = 'active';
      record.order = ++this.sequence;
    } catch (error) {
      await this.fail(record, toFailure(error, id));
    }
  }

  /** Checks `setup`'s return against `provides` and wraps each service. */
  private publish(record: PluginRecord, returned: unknown): ReadonlyMap<string, object> {
    const id = record.manifest.id;
    const declared = record.manifest.provides ?? [];
    const services = new Map<string, object>();
    if (declared.length === 0) return services;

    if (typeof returned !== 'object' || returned === null) {
      throw new PluginError(
        'service-invalid',
        `plugin "${id}" declares ${declared.join(', ')} but setup() returned no services`,
        id,
      );
    }
    const keys = Object.keys(returned);
    const missing = declared.filter((service) => !keys.includes(service));
    const extra = keys.filter((key) => !declared.includes(key));
    if (missing.length > 0 || extra.length > 0) {
      throw new PluginError(
        'service-invalid',
        `plugin "${id}" setup() must return exactly ${declared.join(', ')}` +
          (missing.length > 0 ? `; missing ${missing.join(', ')}` : '') +
          (extra.length > 0 ? `; undeclared ${extra.join(', ')}` : ''),
        id,
      );
    }
    for (const service of declared) {
      services.set(service, this.facade(record, service, (returned as Record<string, unknown>)[service]));
    }
    return services;
  }

  /**
   * A consumer gets the provider's methods and nothing else: no fields, no
   * prototype, no way to the provider's internals. Every call first checks
   * that the provider is still active, so a consumer that kept the facade
   * cannot reach a stopped plugin. Values a method returns are passed through
   * as they are; anything live inside them is the provider's responsibility.
   */
  private facade(record: PluginRecord, service: string, implementation: unknown): object {
    const id = record.manifest.id;
    if (typeof implementation !== 'object' || implementation === null) {
      throw new PluginError('service-invalid', `service "${service}" from "${id}" must be an object`, id);
    }
    const methods = new Map<string, (...args: readonly unknown[]) => unknown>();
    for (
      let level: object | null = implementation;
      level !== null && level !== Object.prototype;
      level = Object.getPrototypeOf(level) as object | null
    ) {
      for (const key of Object.getOwnPropertyNames(level)) {
        if (key === 'constructor' || methods.has(key)) continue;
        const method = Object.getOwnPropertyDescriptor(level, key)?.value as unknown;
        if (typeof method !== 'function') continue;
        methods.set(key, (...args) => {
          if (record.state !== 'active') {
            throw new PluginError(
              'dependency-failed',
              `service "${service}" from "${id}" is no longer available (${record.state})`,
              id,
            );
          }
          return (method as (...a: unknown[]) => unknown).apply(implementation, [...args]);
        });
      }
    }
    if (methods.size === 0) {
      throw new PluginError('service-invalid', `service "${service}" from "${id}" exposes no methods`, id);
    }
    return Object.freeze(Object.fromEntries(methods));
  }

  private serviceFor(pluginId: string, service: string): object {
    for (const record of this.records.values()) {
      if (record.state !== 'active') continue;
      const facade = record.services.get(service);
      if (facade !== undefined) return facade;
    }
    throw new PluginError(
      'dependency-failed',
      `service "${service}" required by "${pluginId}" has no active provider`,
      pluginId,
    );
  }

  private managerFor(pluginId: string): PluginManager {
    return {
      list: () => this.plugins,
      rejections: () => this.rejections,
      grant: (target, capability) => {
        // Privileged grants are the host's alone, by plugin id. A manager that
        // could hand them out would turn one trusted plugin into a way to
        // make any official plugin trusted.
        if (isPrivilegedCapability(capability)) {
          throw new PluginError(
            'capability-denied',
            `plugin "${pluginId}" cannot grant privileged "${capability}"; only the host approves it`,
            pluginId,
          );
        }
        this.grant(target, capability);
      },
      revoke: (target, capability) => this.revoke(target, capability),
      unload: (target) => this.unload(target),
      subscribe: (listener) => {
        const record = this.require(pluginId);
        if (!isLive(record)) return () => undefined;
        record.listeners.add(listener);
        return () => record.listeners.delete(listener);
      },
    };
  }

  private createRecord(entry: PluginEntry, manifest: PluginManifest): PluginRecord {
    const ledger = new GrantLedger(manifest.id, manifest.provenance, manifest.capabilities ?? [], {
      ...(this.options.grants?.[manifest.id] === undefined
        ? {}
        : { approvals: this.options.grants[manifest.id] }),
      ...(this.options.clock === undefined ? {} : { clock: this.options.clock }),
    });
    const meter = new BudgetMeter(
      manifest.id,
      { ...DEFAULT_BUDGETS[manifest.provenance], ...this.options.budgets?.[manifest.id] },
      this.options.clock,
    );
    const context = new HostApi(
      manifest.id,
      ledger,
      meter,
      {
        ...(this.options.source === undefined ? {} : { source: this.options.source }),
        ...(this.options.store === undefined ? {} : { store: this.options.store }),
        ...(this.options.notify === undefined ? {} : { notify: this.options.notify }),
        ...(this.options.net === undefined ? {} : { net: this.options.net }),
        ...(this.options.logger === undefined ? {} : { logger: this.options.logger }),
        ...(this.options.clock === undefined ? {} : { clock: this.options.clock }),
      },
      manifest.requires ?? [],
      this.bridge,
    ).context;
    return {
      entry,
      manifest,
      ledger,
      meter,
      context,
      listeners: new Set(),
      services: new Map(),
      state: 'loaded',
      order: 0,
      stopping: false,
    };
  }

  private refuse(entry: unknown, error: unknown): PluginRegistration {
    const id = idOf(entry);
    const registration: PluginRegistration = {
      pluginId: id,
      status: 'failed',
      capabilities: [],
      failure: toFailure(error, id),
    };
    this.refused.push(registration);
    return registration;
  }

  private registrationOf(record: PluginRecord): PluginRegistration {
    return {
      pluginId: record.manifest.id,
      status: record.state === 'active' ? 'active' : 'failed',
      capabilities: record.ledger.capabilities,
      ...(record.failure === undefined ? {} : { failure: record.failure }),
    };
  }

  private require(pluginId: string): PluginRecord {
    const record = this.records.get(pluginId);
    if (record === undefined) {
      throw new PluginError('unknown-plugin', `no plugin registered as "${pluginId}"`, pluginId);
    }
    return record;
  }

  private byOrder(): PluginRecord[] {
    return [...this.records.values()].sort((a, b) => a.order - b.order);
  }

  private async fail(record: PluginRecord, failure: PluginFailure): Promise<void> {
    if (!isLive(record) || record.stopping) return;
    record.failure = failure;
    await this.stop(record, 'failed');
  }

  private async stop(record: PluginRecord, state: 'failed' | 'unloaded'): Promise<void> {
    if (!isLive(record) || record.stopping) return;
    record.stopping = true;
    const id = record.manifest.id;

    // Consumers first, newest first, while this provider still answers them.
    const provides = record.manifest.provides ?? [];
    if (provides.length > 0) {
      const dependents = this.byOrder()
        .reverse()
        .filter(
          (other) =>
            other !== record &&
            other.state === 'active' &&
            (other.manifest.requires ?? []).some((service) => provides.includes(service)),
        );
      for (const dependent of dependents) {
        await this.fail(dependent, {
          code: 'dependency-failed',
          message: `provider "${id}" stopped (${state})`,
        });
      }
    }

    record.state = state;
    // Grants go next: anything the plugin still runs cannot reach the host.
    record.ledger.close();
    record.listeners.clear();
    try {
      await record.module?.dispose?.();
    } catch (error) {
      // A broken `dispose` is logged, not promoted: the failure that stopped the
      // plugin is already recorded, and it is the one worth showing.
      this.log(id, 'warn', `dispose failed: ${toFailure(error, id).message}`);
    }
    record.stopping = false;
    this.emit();
  }

  private emit(): void {
    const listeners = [
      ...this.hostListeners,
      ...[...this.records.values()].flatMap((record) => (isLive(record) ? [...record.listeners] : [])),
    ];
    for (const listener of listeners) {
      try {
        listener();
      } catch (error) {
        this.log('(kernel)', 'warn', `state listener threw: ${toFailure(error).message}`);
      }
    }
  }

  private log(pluginId: string, level: PluginLogRecord['level'], message: string): void {
    this.options.logger?.({ pluginId, level, message, at: this.options.clock?.() ?? Date.now() });
  }
}
