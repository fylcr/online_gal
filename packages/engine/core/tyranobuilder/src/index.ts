import type { ProbeResult, RuntimeProvider } from '@online-gal/engine-api';
import { WebRuntimeProvider, probeWebGame } from '@online-gal/engine-core-web-html5';

/**
 * Compatibility shim injected ahead of a TyranoBuilder game.
 *
 * TyranoBuilder exports target a browser, but desktop exports commonly assume
 * NW.js. The shim supplies the globals those builds probe for and neutralises
 * the file-system calls that cannot work here, so the engine falls back to its
 * web storage path instead of throwing during boot.
 */
export const TYRANO_SHIM = `(function () {
  'use strict';

  // TyranoBuilder's desktop template checks these before choosing a storage
  // backend; presenting a browser identity keeps it on the web path.
  window.__ogfs_browser = true;
  if (typeof window.require === 'undefined') {
    window.require = undefined;
  }
  if (typeof window.nw === 'undefined') {
    window.nw = undefined;
  }
  if (typeof window.process === 'undefined') {
    window.process = { platform: 'browser', env: {}, versions: { node: '0.0.0' } };
  }

  // Silence the desktop-only config loader when it is not present.
  var originalError = console.error;
  console.error = function () {
    var message = Array.prototype.join.call(arguments, ' ');
    if (typeof message === 'string' && /node-webkit|nw\\.js|require is not defined/i.test(message)) {
      return;
    }
    return originalError.apply(console, arguments);
  };

  // Tyrano's audio layer probes for HTML5 support through this global.
  if (typeof window.__tyrano_audio_ready === 'undefined') {
    window.__tyrano_audio_ready = true;
  }
})();
`;

/**
 * TyranoBuilder provider.
 *
 * TyranoBuilder games are plain HTML5 applications, so they are executed by the
 * embedded web runtime rather than reimplemented. What this provider adds is
 * the identification logic — `tyrano/` plus `data/scenario/` markers — and the
 * desktop-to-browser shim, which is what makes an exported desktop build boot
 * in a page.
 */
export const tyranoProvider: RuntimeProvider = new (class extends WebRuntimeProvider {
  constructor() {
    super('tyrano', 'TyranoBuilder', 80, {
      description:
        'Runs TyranoBuilder titles with their bundled KAG/Tyrano engine, adding a browser shim for desktop exports.',
      accepts: ['tyrano/', 'data/scenario/first.ks', 'tyrano/plugins/kag/kag.js'],
      engines: ['tyrano'],
      webRoot: {
        entry: 'index.html',
        shimScripts: [{ path: '__ogfs_tyrano_shim__.js', source: TYRANO_SHIM }],
      },
    });
  }

  override async probe(game: Parameters<RuntimeProvider['probe']>[0]): Promise<ProbeResult | null> {
    const summary = await probeWebGame(game);
    if (summary.engine !== 'tyrano') return null;

    // Distinguish a TyranoBuilder build from a generic page that happens to
    // have a `tyrano` directory by looking for the scenario data.
    const evidence = [...summary.evidence];
    let score = summary.score;
    for (const marker of ['data/scenario/first.ks', 'tyrano/plugins/kag/kag.js', 'tyrano/kag.js']) {
      if (await game.has(marker)) {
        evidence.push({ path: marker, detail: 'TyranoBuilder data marker' });
        score += 10;
        break;
      }
    }

    return {
      runtimeId: 'tyrano',
      displayName: 'TyranoBuilder',
      score,
      confidence: score >= 60 ? 'high' : 'medium',
      startup: summary.entry,
      evidence: evidence.map((entry) => ({
        path: entry.path,
        detail: entry.detail,
        weight: score,
      })),
    };
  }
})();
