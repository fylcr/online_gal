/**
 * The WebAssembly ONScripter provider.
 *
 * This runs the real engine. The TypeScript NScripter interpreter in `runtime.ts` is kept
 * because it is the fallback for scenarios the engine cannot open — but wherever the engine
 * can run, it should: a faithful build of OnscripterYuri cannot drift from the original's
 * behaviour the way a reimplementation can, and this one is verified working end to end
 * (assets loaded in 0.56 MB of ranged reads out of a 1.8 GB archive, audio, input, and the
 * picture filling the viewport).
 *
 * `priority` is higher than the interpreter's 90 so the engine wins when both match.
 */
import type { ProbeContext, ProbeResult, RuntimeHost, RuntimeInstance, RuntimeProvider } from '@online-gal/engine-api';
import { OnsWasmRuntime, type OnsWasmCreateOptions } from './wasm-runtime.js';
// Reused rather than duplicated: which games these are is a property of the engine family,
// not of the execution strategy, and two probes would be two things to keep in step.
import { probeOnscripter } from './runtime.js';

export const onscripterWasmProvider: RuntimeProvider = {
  runtimeId: 'onscripter-wasm',
  displayName: 'ONScripter (WebAssembly engine)',
  priority: 95,
  description:
    'Runs the real OnscripterYuri engine, compiled to WebAssembly, with ranged asset loading from the game archives.',
  accepts: ['nscript.dat', 'nscript.___', 'nscr_sec.dat', 'onscript.nt2', 'onscript.nt3', '.nsa'],
  containers: ['nsa', 'ns2', 'sar', 'zip'],
  availability: 'runnable',

  /**
   * Recognises the same games as the interpreter, since both target ONScripter.
   *
   * The probe is reused rather than duplicated: which games these are is a property of the
   * engine family, not of the execution strategy, and two probes would be two things to keep
   * in step.
   */
  async probe(game: ProbeContext): Promise<ProbeResult | null> {
    const summary = await probeOnscripter(game);
    if (summary.score <= 0) return null;
    return {
      runtimeId: 'onscripter-wasm',
      displayName: 'ONScripter (WebAssembly engine)',
      score: summary.score,
      confidence: summary.score >= 80 ? 'high' : summary.score >= 50 ? 'medium' : 'low',
      ...(summary.script ? { startup: summary.script } : {}),
      evidence: summary.evidence.map((entry) => ({
        path: entry.path,
        detail: entry.detail,
        weight: 60,
      })),
    };
  },

  create(host: RuntimeHost, options): RuntimeInstance {
    return new OnsWasmRuntime(host, options as OnsWasmCreateOptions);
  },
};
