import type {
  GameSource,
  ProbeConfidence,
  ProbeContext,
  ProbeEvidence,
  ProbeResult,
  RuntimeHost,
  RuntimeInstance,
  RuntimeOptions,
  RuntimeProvider,
  RuntimeId,
} from './types.js';
import { RuntimeError, UnsupportedEngineError } from './types.js';

/** Shared no-op logger used when a probe context is not given one. */
export type ProbeLogger = (message: string) => void;

function joinPath(dir: string | undefined, name: string): string {
  if (!dir) return name;
  const base = dir.endsWith('/') ? dir.slice(0, -1) : dir;
  return base ? `${base}/${name}` : name;
}

function normalise(path: string): string {
  return path.replace(/^\.?\//, '').replace(/\/{2,}/g, '/');
}

/**
 * Default `ProbeContext` backed by a `GameSource`.
 *
 * Probing must stay cheap: results of `has`/`list` are memoised for the
 * lifetime of the context so that several providers can probe the same game
 * without re-walking the directory.
 */
export class DefaultProbeContext implements ProbeContext {
  private readonly fileCache = new Map<string, boolean>();
  private readonly dirCache = new Map<string, readonly string[]>();
  private readonly textCache = new Map<string, string | undefined>();

  constructor(
    readonly source: GameSource,
    private readonly onWarn: ProbeLogger = () => {},
  ) {}

  async has(path: string): Promise<boolean> {
    const key = normalise(path);
    const cached = this.fileCache.get(key);
    if (cached !== undefined) return cached;
    let result = false;
    try {
      result = await this.source.exists(key);
    } catch (error) {
      this.onWarn(`exists(${key}) failed: ${String(error)}`);
      result = false;
    }
    this.fileCache.set(key, result);
    return result;
  }

  async list(dir = ''): Promise<readonly string[]> {
    const key = normalise(dir);
    const cached = this.dirCache.get(key);
    if (cached) return cached;
    let result: readonly string[] = [];
    try {
      result = await this.source.list(key);
    } catch (error) {
      this.onWarn(`list(${key}) failed: ${String(error)}`);
      result = [];
    }
    this.dirCache.set(key, result);
    return result;
  }

  async hasSuffix(suffix: string): Promise<boolean> {
    const wanted = suffix.toLowerCase();
    const direct = normalise(suffix.replace(/^\*/, ''));
    if (!wanted.startsWith('/') && !wanted.includes('/')) {
      if (await this.has(direct)) return true;
    }
    return this.scanFor((path) => path.toLowerCase().endsWith(wanted), 512);
  }

  async hasEvery(paths: readonly string[]): Promise<boolean> {
    for (const path of paths) {
      if (!(await this.has(path))) return false;
    }
    return true;
  }

  async readText(path: string, maxBytes = 256 * 1024): Promise<string | undefined> {
    const key = normalise(path);
    if (this.textCache.has(key)) return this.textCache.get(key);
    let result: string | undefined;
    try {
      if (await this.has(key)) {
        const size = await this.source.size(key);
        if (size < 0 || size <= maxBytes) {
          const bytes = await this.source.readFile(key);
          if (bytes.byteLength <= maxBytes) {
            result = new TextDecoder('utf-8', { fatal: false }).decode(bytes);
          }
        }
      }
    } catch (error) {
      this.onWarn(`readText(${key}) failed: ${String(error)}`);
      result = undefined;
    }
    this.textCache.set(key, result);
    return result;
  }

  async dirContains(dir: string, pattern: RegExp): Promise<boolean> {
    const entries = await this.list(dir);
    return entries.some((entry) => pattern.test(entry));
  }

  /**
   * Breadth-first walk with a hard budget, so a probe against a huge game
   * directory cannot stall the library scan.
   */
  private async scanFor(
    predicate: (path: string) => boolean,
    budget: number,
  ): Promise<boolean> {
    const queue: string[] = [''];
    let visited = 0;
    while (queue.length > 0 && visited < budget) {
      const dir = queue.shift()!;
      const entries = await this.list(dir);
      for (const entry of entries) {
        visited += 1;
        if (visited > budget) return false;
        const path = joinPath(dir, entry);
        if (entry.endsWith('/')) {
          queue.push(path.replace(/\/$/, ''));
        } else if (predicate(path)) {
          return true;
        }
      }
    }
    return false;
  }
}

export function confidenceForScore(score: number): ProbeConfidence {
  if (score <= 0) return 'none';
  if (score >= 100) return 'high';
  if (score >= 50) return 'medium';
  return 'low';
}

export interface DiscoveryOutcome {
  /** Every runtime that recognised the game, best first. */
  readonly matches: readonly ProbeResult[];
  /** The winner, or null when nothing matched. */
  readonly selected: ProbeResult | null;
  /** The probed game, for error reporting. */
  readonly source: GameSource;
}

export interface RuntimeRegistryOptions {
  /** Called for every probe failure; probing never throws out of `discover`. */
  readonly onProbeError?: (runtimeId: RuntimeId, error: unknown) => void;
  readonly onLog?: (message: string) => void;
}

/**
 * The runtime dispatcher.
 *
 * Registration order is the final tie-break, exactly like AetherKiri's
 * provider registry, so built-in runtimes registered first act as the default
 * for their engine family while third-party providers can outrank them on
 * score or priority.
 */
export class RuntimeRegistry {
  private readonly providers: RuntimeProvider[] = [];
  private readonly byId = new Map<RuntimeId, RuntimeProvider>();

  constructor(private readonly options: RuntimeRegistryOptions = {}) {}

  register(provider: RuntimeProvider): this {
    if (this.byId.has(provider.runtimeId)) {
      throw new RuntimeError(
        `runtime "${provider.runtimeId}" is already registered`,
        provider.runtimeId,
      );
    }
    this.providers.push(provider);
    this.byId.set(provider.runtimeId, provider);
    return this;
  }

  registerAll(providers: readonly RuntimeProvider[]): this {
    for (const provider of providers) this.register(provider);
    return this;
  }

  unregister(runtimeId: RuntimeId): boolean {
    const provider = this.byId.get(runtimeId);
    if (!provider) return false;
    this.byId.delete(runtimeId);
    const index = this.providers.indexOf(provider);
    if (index >= 0) this.providers.splice(index, 1);
    return true;
  }

  get(runtimeId: RuntimeId): RuntimeProvider | undefined {
    return this.byId.get(runtimeId);
  }

  require(runtimeId: RuntimeId): RuntimeProvider {
    const provider = this.byId.get(runtimeId);
    if (!provider) {
      throw new UnsupportedEngineError(
        `no runtime provider registered for "${runtimeId}"`,
        runtimeId,
        this.list().map((entry) => entry.runtimeId),
      );
    }
    return provider;
  }

  list(): readonly RuntimeProvider[] {
    return this.providers;
  }

  get size(): number {
    return this.providers.length;
  }

  /** Probes one runtime in isolation, the direct analogue of the C ABI call. */
  async probeRuntime(
    runtimeId: RuntimeId,
    source: GameSource,
  ): Promise<ProbeResult | null> {
    const provider = this.byId.get(runtimeId);
    if (!provider) return null;
    return this.probeProvider(provider, source);
  }

  /**
   * Probes every registered runtime and returns the ranked matches.
   *
   * Ranking: score desc, priority desc, registration order asc.
   */
  async discover(source: GameSource): Promise<DiscoveryOutcome> {
    const context = new DefaultProbeContext(source, this.options.onLog);
    const results = await Promise.all(
      this.providers.map((provider) =>
        this.probeProvider(provider, source, context),
      ),
    );

    const matches = results
      .filter((result): result is ProbeResult => result !== null)
      .map((result, index) => ({ result, index }))
      .sort((a, b) => {
        if (b.result.score !== a.result.score) {
          return b.result.score - a.result.score;
        }
        const priorityA = this.byId.get(a.result.runtimeId)?.priority ?? 0;
        const priorityB = this.byId.get(b.result.runtimeId)?.priority ?? 0;
        if (priorityB !== priorityA) return priorityB - priorityA;
        return a.index - b.index;
      })
      .map((entry) => entry.result);

    return { matches, selected: matches[0] ?? null, source };
  }

  /** Creates a session for `runtimeId`, or throws `UnsupportedEngineError`. */
  create(
    runtimeId: RuntimeId,
    host: RuntimeHost,
    options: RuntimeOptions,
  ): RuntimeInstance {
    const provider = this.require(runtimeId);
    return provider.create(host, { ...options, runtimeId });
  }

  private async probeProvider(
    provider: RuntimeProvider,
    source: GameSource,
    shared?: ProbeContext,
  ): Promise<ProbeResult | null> {
    const context =
      shared ?? new DefaultProbeContext(source, this.options.onLog);
    try {
      const result = await provider.probe(context);
      if (!result || result.score <= 0) return null;
      return {
        ...result,
        runtimeId: provider.runtimeId,
        displayName: result.displayName || provider.displayName,
        confidence: result.confidence || confidenceForScore(result.score),
      };
    } catch (error) {
      this.options.onProbeError?.(provider.runtimeId, error);
      return null;
    }
  }
}

/** Convenience helper for providers building evidence lists. */
export function evidence(
  path: string,
  detail: string,
  weight: number,
): ProbeEvidence {
  return { path, detail, weight };
}
