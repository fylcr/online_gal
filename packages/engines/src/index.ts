/**
 * `@online-gal/engines` — the aggregate simulator entry point.
 *
 * The platform already ships one provider per engine family; what was missing
 * was a single package that knows about all of them, so that anything wanting to
 * "just run whatever this game is" — the library, the launcher, a CI smoke
 * test, a future native shell — has one import instead of seven and one honest
 * answer about what can actually run.
 *
 * Two rules this package exists to enforce:
 *
 *  - **Identification is not delivery.** `selectEngine` reports a game as
 *    `unavailable` when the winning provider declares it so, instead of handing
 *    back a provider whose `create()` would fail. Artemis and SiglusEngine
 *    recognise their titles today and still have no browser build, so that gap
 *    is stated, not smoothed over.
 *  - **The registry is a set, not a list.** `ENGINE_PACKAGES` is validated for
 *    duplicate runtime ids at module load, because two providers claiming one
 *    id would make dispatch depend on import order.
 */
import {
  DefaultProbeContext,
  RuntimeError,
  RuntimeRegistry,
  UnsupportedEngineError,
  type GameSource,
  type ProbeContext,
  type ProbeResult,
  type RuntimeAvailability,
  type RuntimeHost,
  type RuntimeId,
  type RuntimeInstance,
  type RuntimeOptions,
  type RuntimeProvider,
} from '@online-gal/engine-api';
import { artemisProvider } from '@online-gal/runtime-artemis';
import { kirikiriProvider, kirikiriWasmProvider } from '@online-gal/runtime-kag';
import { onscripterProvider, onscripterWasmProvider } from '@online-gal/runtime-ons';
import { siglusProvider } from '@online-gal/runtime-siglus';
import { tyranoProvider } from '@online-gal/runtime-tyrano';
import { webProvider } from '@online-gal/runtime-web';

export * from '@online-gal/engine-api';

/**
 * Where a provider comes from, and what it is for.
 *
 * `kind` separates the two things a provider can be, which is the distinction a
 * consumer actually needs: `engine` is a real build of the engine, `interpreter`
 * is a reimplementation of the script interpreter, and `shell` is a provider
 * that only identifies the engine because execution has nowhere to go yet.
 */
export type EngineKind = 'engine' | 'interpreter' | 'shell';

/** One registered engine, as reported by `listEngines`. */
export interface EngineDescriptor {
  readonly runtimeId: RuntimeId;
  readonly displayName: string;
  /** The npm package that exports the provider. */
  readonly packageName: string;
  readonly kind: EngineKind;
  /**
   * Whether this provider can start a game here.
   *
   * Read from the provider itself, never computed: a provider knows whether its
   * backend is wired up, and guessing would produce exactly the false green
   * this platform refuses to ship.
   */
  readonly availability: RuntimeAvailability;
  /** True when the game can actually be played: runnable and confidently probed. */
  readonly playable: boolean;
  readonly priority: number;
  readonly description: string;
  /** Markers the provider recognises, for the engine panel. */
  readonly accepts: readonly string[];
  /** Archive/script containers it can read. */
  readonly containers: readonly string[];
  /** Option keys the provider honours, for a settings panel. */
  readonly optionKeys: readonly string[];
  readonly provider: RuntimeProvider;
}

/** A provider plus the metadata the aggregate layer knows about it. */
export interface EngineRegistration {
  readonly runtimeId: RuntimeId;
  readonly packageName: string;
  readonly kind: EngineKind;
  readonly provider: RuntimeProvider;
}

/**
 * Every engine the platform ships, in registration (tie-break) order.
 *
 * The order matters and is the same one `packages/core/src/platform.ts` uses:
 * real engines first, then interpreters, then the recognise-only providers, then
 * the broad `web` provider last so a bare `index.html` can never outrank a
 * specific engine marker.
 */
export const ENGINE_PACKAGES: readonly EngineRegistration[] = [
  {
    runtimeId: 'krkr2-wasm',
    packageName: '@online-gal/runtime-kag',
    kind: 'engine',
    provider: kirikiriWasmProvider,
  },
  // The interpreter stays behind the real build: both recognise the same games,
  // and a faithful compilation cannot drift the way a reimplementation can.
  {
    runtimeId: 'krkr2',
    packageName: '@online-gal/runtime-kag',
    kind: 'interpreter',
    provider: kirikiriProvider,
  },
  {
    runtimeId: 'onscripter-wasm',
    packageName: '@online-gal/runtime-ons',
    kind: 'engine',
    provider: onscripterWasmProvider,
  },
  {
    runtimeId: 'onscripter',
    packageName: '@online-gal/runtime-ons',
    kind: 'interpreter',
    provider: onscripterProvider,
  },
  {
    runtimeId: 'siglus',
    packageName: '@online-gal/runtime-siglus',
    kind: 'shell',
    provider: siglusProvider,
  },
  {
    runtimeId: 'artemis',
    packageName: '@online-gal/runtime-artemis',
    kind: 'engine',
    provider: artemisProvider,
  },
  {
    runtimeId: 'tyrano',
    packageName: '@online-gal/runtime-tyrano',
    kind: 'engine',
    provider: tyranoProvider,
  },
  {
    runtimeId: 'web',
    packageName: '@online-gal/runtime-web',
    kind: 'engine',
    provider: webProvider,
  },
];

/**
 * Fails loudly at import time rather than silently at dispatch time.
 *
 * A duplicate id would let the first registration win and make engine choice
 * depend on module evaluation order — the one bug class the registry's
 * tie-break chain is supposed to make impossible.
 */
function assertNoDuplicateIds(registrations: readonly EngineRegistration[]): void {
  const seen = new Set<RuntimeId>();
  for (const entry of registrations) {
    if (entry.provider.runtimeId !== entry.runtimeId) {
      throw new RuntimeError(
        `engine "${entry.runtimeId}" declares runtimeId "${entry.provider.runtimeId}"`,
        entry.runtimeId,
      );
    }
    if (seen.has(entry.runtimeId)) {
      throw new RuntimeError(`engine "${entry.runtimeId}" is registered twice`, entry.runtimeId);
    }
    seen.add(entry.runtimeId);
  }
}

assertNoDuplicateIds(ENGINE_PACKAGES);

/** The providers, in registration order. */
export function engineProviders(): readonly RuntimeProvider[] {
  return ENGINE_PACKAGES.map((entry) => entry.provider);
}

/**
 * Every registered engine, with its honest availability.
 *
 * `availability` is copied from the provider at call time rather than stored,
 * because a provider's availability is a property of the running host (a
 * WebAssembly build may have been registered since the module loaded) and a
 * cached copy would go stale.
 */
export function listEngines(): EngineDescriptor[] {
  return ENGINE_PACKAGES.map((entry) => {
    const { provider } = entry;
    const availability = provider.availability ?? 'runnable';
    return {
      runtimeId: provider.runtimeId,
      displayName: provider.displayName,
      packageName: entry.packageName,
      kind: entry.kind,
      availability,
      playable: availability === 'runnable',
      priority: provider.priority,
      description: provider.description ?? '',
      accepts: provider.accepts ?? [],
      containers: provider.containers ?? [],
      optionKeys: provider.optionKeys ?? [],
      provider,
    };
  });
}

/** Describes one engine by id, or null when it is not registered. */
export function engineInfo(runtimeId: RuntimeId): EngineDescriptor | null {
  return listEngines().find((entry) => entry.runtimeId === runtimeId) ?? null;
}

/** A fresh registry with every shipped engine registered. */
export function createEngineRegistry(
  extra: readonly RuntimeProvider[] = [],
): RuntimeRegistry {
  return new RuntimeRegistry().registerAll(extra).registerAll(engineProviders());
}

/** A probe context for `source`, sharing one cache across all probes. */
function contextFor(source: GameSource, ctx?: ProbeContext): ProbeContext {
  return ctx ?? new DefaultProbeContext(source);
}

/** One engine's answer for one game directory. */
export interface EngineProbe {
  readonly runtimeId: RuntimeId;
  readonly displayName: string;
  /** The probe score; 0 means the engine did not recognise the game. */
  readonly score: number;
  readonly confidence: ProbeResult['confidence'];
  /** What the engine's own provider says about being able to run it. */
  readonly availability: RuntimeAvailability;
  /** True only when recognised *and* runnable. */
  readonly playable: boolean;
  /** Human-readable reason when the engine was recognised but cannot run. */
  readonly reason: string | null;
  readonly startup?: string;
  readonly evidence: readonly ProbeResult['evidence'][number][];
  /**
   * The provider that claimed the game. Look it up with `engineInfo` to get its
   * package, kind and marker list — those describe the *engine*, not this match.
   */
  readonly provider: RuntimeProvider;
}

/** The outcome of probing a game against every engine. */
export interface ProbeReport {
  /** Engines that recognised the game, best first. */
  readonly matches: readonly EngineProbe[];
  /** The best playable match, or null when the best match cannot run. */
  readonly selected: EngineProbe | null;
  /** The highest-scoring match even when it cannot run — shown, not hidden. */
  readonly best: EngineProbe | null;
  /**
   * Why nothing can play this game: `null` when `selected` is set, otherwise a
   * short reason the UI can show verbatim.
   */
  readonly blockedReason: string | null;
  readonly source: GameSource;
}

function rankKey(
  a: { match: EngineProbe; index: number },
  b: { match: EngineProbe; index: number },
): number {
  if (b.match.score !== a.match.score) return b.match.score - a.match.score;
  if (b.match.provider.priority !== a.match.provider.priority) {
    return b.match.provider.priority - a.match.provider.priority;
  }
  return a.index - b.index;
}

function blocked(message: string, source: GameSource): ProbeReport {
  return { matches: [], selected: null, best: null, blockedReason: message, source };
}

/**
 * Probes one game against every registered engine.
 *
 * Ranking is score, then priority, then registration order — the registry's own
 * chain, so this package never invents a second way to pick an engine.
 *
 * `selected` is the honest answer rather than the highest score: a game whose
 * engine is recognised but not runnable is reported as `selected: null` with the
 * match still listed, because handing that match back as playable is the one
 * result this platform must not produce.
 *
 * Pass `providers` to probe a different set — `createRuntime` uses it to probe
 * exactly the engines it can create from, so a registry can never be asked for
 * an engine the probe did not see.
 */
export async function probeAll(
  source: GameSource,
  ctx?: ProbeContext,
  providers: readonly RuntimeProvider[] = engineProviders(),
): Promise<ProbeReport> {
  const context = contextFor(source, ctx);
  const descriptors = providers;

  const answers = await Promise.all(
    descriptors.map(async (provider): Promise<EngineProbe | null> => {
      let result: ProbeResult | null;
      try {
        result = await provider.probe(context);
      } catch {
        // A provider that throws loses its vote; it must not take the whole
        // scan down with it, which is the same contract `discover` has.
        return null;
      }
      if (!result || result.score <= 0) return null;

      const availability =
        provider.availability ??
        (result.confidence === 'none' ? 'unknown' : 'runnable');
      return {
        runtimeId: provider.runtimeId,
        displayName: result.displayName || provider.displayName,
        score: result.score,
        confidence: result.confidence,
        availability,
        playable: availability === 'runnable',
        reason:
          availability === 'runnable'
            ? null
            : `${provider.displayName} 识别成功，但此环境尚无可运行的浏览器实现。`,
        ...(result.startup === undefined ? {} : { startup: result.startup }),
        evidence: result.evidence,
        provider,
      };
    }),
  );

  const matches = answers
    .filter((answer): answer is EngineProbe => answer !== null)
    .map((match, index) => ({ match, index }))
    .sort(rankKey)
    .map((entry) => entry.match);

  const best = matches[0] ?? null;
  if (!best) {
    return blocked(`没有引擎识别出这个目录（已探测 ${descriptors.length} 个引擎）。`, source);
  }

  const selected = best.playable ? best : (matches.find((match) => match.playable) ?? null);
  if (selected) {
    return { matches, selected, best, blockedReason: null, source };
  }

  return {
    matches,
    selected: null,
    best,
    blockedReason:
      matches.length === 1
        ? `${best.reason}`
        : `${best.reason}（另有 ${matches.length - 1} 个更弱的候选同样无法运行）`,
    source,
  };
}

/**
 * Picks the engine for a game, or throws `UnsupportedEngineError`.
 *
 * Use this when the caller genuinely cannot proceed without an engine. Use
 * `probeAll` when it would rather show the user why not.
 */
export async function selectEngine(
  source: GameSource,
  ctx?: ProbeContext,
  providers?: readonly RuntimeProvider[],
): Promise<EngineProbe> {
  const report = await probeAll(source, ctx, providers);
  if (!report.selected) {
    throw new UnsupportedEngineError(
      report.blockedReason ?? `没有引擎能运行 ${source.name}`,
      report.best?.runtimeId ?? null,
      report.matches.map((match) => match.runtimeId),
    );
  }
  return report.selected;
}

export interface CreateRuntimeRequest {
  readonly source: GameSource;
  readonly host: RuntimeHost;
  /** `runtimeId` is filled in by `createRuntime` from the probe. */
  readonly options: Omit<RuntimeOptions, 'runtimeId'>;
  /** Forces a specific engine, skipping the probe. */
  readonly runtimeId?: RuntimeId;
  readonly registry?: RuntimeRegistry;
}

export interface StartedRuntime {
  readonly runtimeId: RuntimeId;
  /** The probe that chose the engine; absent when `runtimeId` was forced. */
  readonly match: EngineProbe | null;
  readonly instance: RuntimeInstance;
}

/**
 * Creates a session for the engine that matches `source`.
 *
 * The choice is made by probe score and priority, never by the caller naming an
 * engine: that is the whole point of the aggregate package. A caller that really
 * means a specific engine can pass `runtimeId`, which skips the probe — and is
 * refused, loudly, when that engine declares itself unavailable, so an override
 * cannot launder an unrunnable engine into a session.
 *
 * The engine is chosen from `registry`'s own providers, never from the shipped
 * list: a caller that registered a replacement or removed a default gets the
 * choice it actually registered, and can never be handed an id its registry
 * cannot create.
 */
export async function createRuntime(request: CreateRuntimeRequest): Promise<StartedRuntime> {
  const registry = request.registry ?? createEngineRegistry();
  const { host, source } = request;

  if (request.runtimeId) {
    const provider = registry.get(request.runtimeId);
    if (!provider) {
      throw new UnsupportedEngineError(
        `no runtime provider registered for "${request.runtimeId}"`,
        request.runtimeId,
        registry.list().map((entry) => entry.runtimeId),
      );
    }
    if (provider.availability === 'unavailable') {
      throw new UnsupportedEngineError(
        `${provider.displayName} 可以识别此游戏，但此环境没有可运行的实现。`,
        request.runtimeId,
      );
    }
    return {
      runtimeId: request.runtimeId,
      match: null,
      instance: registry.create(request.runtimeId, host, {
        ...request.options,
        runtimeId: request.runtimeId,
      }),
    };
  }

  const match = await selectEngine(source, undefined, registry.list());
  return {
    runtimeId: match.runtimeId,
    match,
    instance: registry.create(match.runtimeId, host, {
      ...request.options,
      runtimeId: match.runtimeId,
    }),
  };
}
