/**
 * The WebAssembly KiriKiri provider.
 *
 * This runs the real engine, built to WebAssembly and mounted in a framed page. The
 * TypeScript interpreter in `runtime.ts` is kept because it is the fallback for games
 * the build cannot open — but wherever the build can run, it should: a faithful
 * compilation of KiriKiri2 cannot drift from the original's behaviour the way a
 * reimplementation can, and the host byte-source seam feeds its archives directly.
 *
 * `priority` is higher than the interpreter's 100 so the engine wins when both match.
 */
import {
  evidence,
  type ProbeContext,
  type ProbeResult,
  type RuntimeHost,
  type RuntimeInstance,
  type RuntimeOptions,
  type RuntimeProvider,
} from '@online-gal/engine-api';
import { probeKirikiri } from './runtime.js';
import { findKiriMount, kirikiriWasmPageUrl } from './wasm-config.js';
import { KiriWasmRuntime, type KiriWasmCreateOptions } from './wasm-runtime.js';

export const kirikiriWasmProvider: RuntimeProvider = {
  runtimeId: 'krkr2-wasm',
  displayName: 'KiriKiri (WebAssembly engine)',
  priority: 105,
  description:
    'Runs the real KiriKiri2 engine compiled to WebAssembly inside a framed build, ' +
    'streaming the game archives in through the host byte-source seam.',
  accepts: ['.xp3', 'startup.tjs', 'Config.tjs', 'first.ks', 'System.tjs'],
  containers: ['xp3', 'zip', 'asar'],
  availability: 'runnable',

  /**
   * Recognises the same games as the interpreter, since both target KiriKiri/KAG3 —
   * but only claims one when this deployment can actually run it.
   *
   * Two gates, and both are about the host rather than the game:
   *
   *  - **A build must be published.** Without a served page there is no engine, and
   *    claiming the game would turn a game the interpreter can run into a `load()`
   *    failure.
   *  - **The source must offer a mountable archive.** The framed shell mounts one
   *    file (`?xp3=`, or a whole-source `?game=` ZIP); a directory of loose files is
   *    the interpreter's job.
   *
   * The probe itself is reused rather than duplicated: which games these are is a
   * property of the engine family, not of the execution strategy, and two probes
   * would be two things to keep in sync.
   */
  async probe(game: ProbeContext): Promise<ProbeResult | null> {
    if (kirikiriWasmPageUrl() === null) return null;
    if ((await findKiriMount(game)) === null) return null;

    const summary = await probeKirikiri(game);
    if (summary.score <= 0) return null;
    return {
      runtimeId: 'krkr2-wasm',
      displayName: 'KiriKiri (WebAssembly engine)',
      score: summary.score,
      confidence: summary.score >= 100 ? 'high' : summary.score >= 50 ? 'medium' : 'low',
      ...(summary.scenario ? { startup: summary.scenario } : {}),
      evidence: summary.evidence.map((entry) => {
        const weight = entry.detail.includes('XP3')
          ? 70
          : entry.detail.includes('scenario')
            ? 50
            : 30;
        return evidence(entry.path, entry.detail, weight);
      }),
    };
  },

  create(host: RuntimeHost, options: RuntimeOptions): RuntimeInstance {
    return new KiriWasmRuntime(host, options as KiriWasmCreateOptions);
  },
};
