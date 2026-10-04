/**
 * The compat publication: which engine the provider runs, and where the pack bytes come from.
 *
 * Two independent decisions live here, and keeping them apart is the point:
 *
 * - {@link configureArtemisWasm} publishes the **legacy asyncify bridge**. After this batch it no
 *   longer routes: it is the explicit diagnostic input, so a session still runs on it only when the
 *   embedder chose it per game.
 * - {@link configureArtemisCompat} publishes the **compat source build** (the worker + `.wasm` pair
 *   `ports/artemis/compat-web` produces) and the optional per-game engine base.
 *
 * Neither is a router: the runtime reads them once, at `load()`.
 */

/**
 * The engine source a compat session runs on.
 *
 * Three URLs, not two, because the shipped engine is **three files** and two of them are not
 * interchangeable:
 *
 * - `workerUrl` — the classic worker that speaks this package's RPC protocol. In a deployment this
 *   is `dist/compat/compat-worker.js`. It is the script `new Worker()` is handed.
 * - `engineJsUrl` — the **generated Emscripten glue** (`artemis-compat.js`), which the worker pulls
 *   in with `importScripts`. A worker handed the glue instead of the RPC script loads a module that
 *   never answers a message, which is a hang, not an error.
 * - `wasmUrl` — the binary the glue fetches.
 *
 * Collapsing `workerUrl` and `engineJsUrl` into one field is the defect this shape exists to make
 * unrepresentable: the two files live in different directories and only one of them is a worker.
 */
export interface ArtemisCompatEngine {
  /** Classic-worker script URL (the RPC worker), already resolved against the page. */
  readonly workerUrl: string;
  /** The generated Emscripten glue the worker `importScripts`, resolved against the page. */
  readonly engineJsUrl: string;
  /** The `.wasm` the glue fetches. Resolved against the page too. */
  readonly wasmUrl: string;
}

/** How the worker script and the binary are supplied. */
export interface ArtemisCompatOptions {
  /**
   * Per-game engine directory, or `null`/omitted for the deployment default.
   *
   * A URL (or an absolute path) resolves `artemis-compat.js` and `artemis-compat.wasm` inside it.
   * Anything else is a **virtual directory name** — the object-URL shape, where the embedder has
   * already turned the pair into blob URLs and only the paths' identity is left.
   *
   * `null` is a real answer: it withdraws the engine for this game, which is what the provider's
   * availability then reports. A game that asked for an engine the deployment cannot serve must not
   * silently fall back to the one it can.
   */
  readonly engineBase?: string | URL | null;
  /** The page's own asset base, used when `engineBase` is absent. */
  readonly baseUrl?: string;
  /** Page-side override for the binary. */
  readonly wasmUrl?: string;
  /** Page-side override for the classic worker script. */
  readonly workerUrl?: string;
  /** Page-side override for the generated glue the worker `importScripts`. */
  readonly engineJsUrl?: string;
}

/**
 * What {@link configureArtemisCompat} publishes.
 *
 * `engineBase` is **absent** for the bundled default and `null` for a withdrawal. The two are
 * different facts and must not be collapsed: absent means "use what this package ships", `null`
 * means "this deployment has no engine".
 */
export interface ArtemisCompatConfig {
  readonly engineBase?: string | URL | null;
  readonly baseUrl?: string;
  readonly wasmUrl?: string;
  readonly workerUrl?: string;
  readonly engineJsUrl?: string;
}

let compatConfig: ArtemisCompatConfig | null = null;
let compatGeneration = 0;

/** The default names inside an engine directory. */
const COMPAT_WORKER_NAME = 'artemis-compat.js';
const COMPAT_WASM_NAME = 'artemis-compat.wasm';

/**
 * Where the three bundled files sit, relative to this module.
 *
 * The glue and the binary are the built engine and live beside the package's assets; the RPC worker
 * is copied into `dist/compat/` by `scripts/copy-compat-worker.mjs`. Both spellings are relative to
 * `import.meta.url`, so a bundler that rewrites it per chunk keeps the pair together and a
 * deployment that copies `dist/` gets both.
 */
const BUNDLED_GLUE = '../assets/artemis-compat.js';
const BUNDLED_WASM = '../assets/artemis-compat.wasm';
const BUNDLED_WORKER = './compat/compat-worker.js';

/** True when `value` names a location rather than an opaque handle. */
function isLocator(value: string | URL): boolean {
  if (value instanceof URL) return true;
  // A scheme (`blob:`, `https:`), a protocol-relative spelling or a path: all three can be resolved.
  // A bare name (`blob` in the object-URL shape) cannot, and is treated as a virtual directory.
  return /^[a-zA-Z][a-zA-Z0-9+.-]*:/.test(value) || value.startsWith('/') || value.startsWith('./') || value.startsWith('../');
}

/**
 * Turns a resolved engine directory into the three URLs a session needs.
 *
 * A named directory supplies the **glue and the binary**; the RPC worker is not in it and never was,
 * so it always comes from the package's own `dist/compat/`. The fallback pair — beside this module's
 * `import.meta.url` — is deliberately relative rather than in place of the configured path: a
 * bundler rewrites `import.meta.url` per chunk, so the relative trio survives being built and
 * copied, which is how the shipped files are found without the page knowing anything about them.
 *
 * The three URLs are asserted distinct. Two of them being equal is not a style problem: it means a
 * worker was handed a script that cannot answer it.
 */
export function resolveCompatEngine(config: ArtemisCompatConfig | null): ArtemisCompatEngine | null {
  if (config === null) return null;
  const base = config.engineBase;
  // Per-game withdrawal beats the page default: a game that named `null` asked for no engine.
  if (base === null) return null;
  const from = base ?? config.baseUrl;
  const workerUrl = config.workerUrl ?? new URL(BUNDLED_WORKER, import.meta.url).href;
  if (from === undefined) {
    return distinct({
      workerUrl,
      engineJsUrl: config.engineJsUrl ?? new URL(BUNDLED_GLUE, import.meta.url).href,
      wasmUrl: config.wasmUrl ?? new URL(BUNDLED_WASM, import.meta.url).href,
    });
  }
  if (!isLocator(from)) return null;
  const resolved = from instanceof URL ? from : new URL(from, baseDocumentBase());
  const directory = resolved.href.endsWith('/') ? resolved.href : `${resolved.href}/`;
  const worker = new URL(directory);
  worker.pathname = `${worker.pathname}${COMPAT_WORKER_NAME}`;
  const wasm = new URL(directory);
  wasm.pathname = `${wasm.pathname}${COMPAT_WASM_NAME}`;
  return distinct({
    workerUrl,
    engineJsUrl: config.engineJsUrl ?? worker.href,
    wasmUrl: config.wasmUrl ?? wasm.href,
  });
}

/**
 * Refuses an engine whose URLs are not three distinct locations.
 *
 * The check is the point of the shape: it turns "the worker was handed the glue" from a silent hang
 * into a `null` that `hasArtemisCompatEngine()` reports as unavailable. An explicit override that
 * makes two of them one file is a configuration the transport cannot serve, not a preference.
 */
function distinct(engine: ArtemisCompatEngine): ArtemisCompatEngine | null {
  const { workerUrl, engineJsUrl, wasmUrl } = engine;
  if (workerUrl === '' || engineJsUrl === '' || wasmUrl === '') return null;
  if (workerUrl === engineJsUrl || workerUrl === wasmUrl || engineJsUrl === wasmUrl) return null;
  return engine;
}

/**
 * The document base a relative engine directory resolves against.
 *
 * A runtime has no document of its own at configure time — the page does. `globalThis.location` is
 * the one spelling that exists in a page and is absent in Node, where the fallback is the module's
 * own URL: a test that configures `'engines/'` still gets an absolute pair back.
 */
function baseDocumentBase(): string {
  const scope = globalThis as { location?: { href?: string } };
  return scope.location?.href ?? import.meta.url;
}

/**
 * Publishes the compat engine every later Artemis session runs on.
 *
 * A bare `configureArtemisCompat()` — or `{}` — is a **real publication**: it restores the bundled
 * default after a game-level or deployment-level withdrawal, which is the only way back from
 * `engineBase: null`. It is not an empty call and must not be treated as a no-op, because
 * "publish the default" and "publish nothing" are different answers.
 *
 * `engineBase: null` (or a per-game `null`) withdraws the engine and returns the provider to
 * `unavailable`. Availability is read from this publication, so a deployment that takes its engine
 * away stops advertising a play button that would fail on click.
 *
 * A malformed **per-game** `engineBase` throws: a game that named something the transport cannot
 * resolve is a configuration error, and silently withdrawing it would look identical to the game
 * having asked for no engine. The deployment-level `null` stays a withdrawal.
 */
export function configureArtemisCompat(options: ArtemisCompatOptions = {}): void {
  const { engineBase, baseUrl, wasmUrl, workerUrl, engineJsUrl } = options;
  if (engineBase !== undefined && engineBase !== null && !(engineBase instanceof URL) && typeof engineBase !== 'string') {
    throw new TypeError(
      `configureArtemisCompat: engineBase must be a string, a URL or null, not ${typeof engineBase}`,
    );
  }
  if (typeof engineBase === 'string' && engineBase !== '' && !isLocator(engineBase)) {
    throw new TypeError(
      `configureArtemisCompat: engineBase ${JSON.stringify(engineBase)} is neither a URL nor a ` +
        'path; omit it to use the bundled engine, or pass null to withdraw it',
    );
  }
  compatConfig = Object.freeze({
    // `undefined` keeps `engineBase` absent (the bundled default); only an explicit `null` withdraws.
    // Writing `?? null` here would turn every bare configure() into a withdrawal.
    ...(engineBase === undefined ? {} : { engineBase }),
    ...(baseUrl === undefined ? {} : { baseUrl }),
    ...(wasmUrl === undefined ? {} : { wasmUrl }),
    ...(workerUrl === undefined ? {} : { workerUrl }),
    ...(engineJsUrl === undefined ? {} : { engineJsUrl }),
  });
  compatGeneration += 1;
}

/**
 * The published compat configuration. Read-only for the caller; the runtime never writes back.
 *
 * A **copy**, and a frozen one: the publication is a fact about the deployment, and an embedder that
 * mutated the object it was handed could move a later session onto a different engine without
 * bumping the generation that sessions record.
 */
export function readArtemisCompatConfig(): ArtemisCompatConfig | null {
  return compatConfig === null ? null : Object.freeze({ ...compatConfig });
}

/** Bumped by every publication, so a session can publish which engine it ran on. */
export function artemisCompatGeneration(): number {
  return compatGeneration;
}

/**
 * Whether a session created right now could start on the compat engine.
 *
 * Deliberately not a claim that the engine works: it answers "is an engine named", which is the
 * deployment's half. The engine's own half is the load, and `describe()` reports that.
 */
export function hasArtemisCompatEngine(): boolean {
  return resolveCompatEngine(compatConfig) !== null;
}
