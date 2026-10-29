/**
 * The Artemis compat engine as *static* app assets — three fixed URLs and nothing else.
 *
 * The engine is a build, not a package: `dist/artemis-compat/` carries the glue, the binary, the
 * GPL source offer and the manifest that ties them to the reviewed commit. Presence alone is not
 * enough (a manifest can describe a different build), so it is verified with Node's own sha256
 * before anything is served or copied. Absent or unverifiable means *withdrawn*, never a stub: an
 * engine that cannot start must not be advertised as one that can.
 *
 * Pure Node, no build step: it runs in `astro.config.mjs` (config time and build time) and in
 * `node --test` alike, and it never reads `.ref/`. A passing gate is a candidate snapshot, not a
 * released body: nothing here claims the manifest's provenance.
 */
import { createHash } from 'node:crypto';
import { closeSync, copyFileSync, createReadStream, existsSync, fstatSync, mkdirSync, openSync, readFileSync, statSync } from 'node:fs';
import { extname, join, resolve, sep } from 'node:path';
import { fileURLToPath } from 'node:url';

const REPO_ROOT = resolve(fileURLToPath(import.meta.url), '../../../..');

/** The one directory the app publishes the engine from. Three distinct URLs live inside it. */
export const MOUNT = '/engines/artemis-compat/';

/** What a session actually fetches: the RPC worker, the glue and the binary. */
export const SERVE = ['compat-worker.js', 'artemis-compat.js', 'artemis-compat.wasm'];

/** The manifest covers exactly these five; it never hashes itself. */
const HASHED = ['artemis-compat.js', 'artemis-compat.wasm', 'artemis-compat-source.tar.gz', 'LICENSE', 'THIRD_PARTY_NOTICES.md'];
const MANIFEST = 'artemis-compat-source.tar.gz.sha256';

/** What a production build emits: the three served files plus the licence/source offer. */
export const PUBLISH = [...new Set([...SERVE, ...HASHED, MANIFEST])];
const MIME = { '.js': 'text/javascript; charset=utf-8', '.wasm': 'application/wasm', '.gz': 'application/gzip' };
const sha256 = (file) => createHash('sha256').update(readFileSync(file)).digest('hex');
// The RPC worker is the package's built concatenation (helper + worker), not a raw source file.
const sourceOf = (state, name) => (name === SERVE[0] ? state.rpc : join(state.dir, name));

/** One `sha256sum` line: 64 hex, GNU two-space or binary ` *`, then a bare safe filename. */
const MANIFEST_LINE = /^([0-9a-fA-F]{64}) (?: |\*)([A-Za-z0-9._-]+)$/;

/**
 * The recorded map, or a string reason. An extra name, a second line for the same name, a path,
 * or a line that is not a hash all refuse — a shorter *or* a longer list is not this candidate.
 */
function recordedHashes(text) {
  const recorded = new Map();
  for (const raw of text.split(/\r?\n/)) {
    const line = raw.trim();
    if (line === '') continue;
    const match = MANIFEST_LINE.exec(line);
    if (!match) return 'the manifest has a line that is not a sha256 of a safe filename';
    const [, hash, name] = match;
    if (recorded.has(name)) return `the manifest lists ${name} twice`;
    recorded.set(name, hash.toLowerCase());
  }
  const extra = [...recorded.keys()].filter((name) => !HASHED.includes(name));
  if (extra.length > 0) return `the manifest names ${extra[0]}, which is not one of the five`;
  return recorded;
}

/**
 * One snapshot of "may this deployment publish the engine?", taken once per process and fed to the
 * compile-time flag, the dev middleware and the production copy, so those three cannot disagree.
 * An unreadable file withdraws the engine; it does not throw out of config load. `sha256` is the
 * map this snapshot recorded — a later copy compares against it instead of trusting the directory.
 */
export function inspectArtemisAssets({
  dir = join(REPO_ROOT, 'dist/artemis-compat'),
  rpc = join(REPO_ROOT, 'packages/engine/core/artemis-compat/dist/compat/compat-worker.js'),
} = {}) {
  const with_ = (reason) => ({ available: false, dir, rpc, reason });
  try {
    if (!existsSync(rpc) || !statSync(rpc).isFile()) return with_(`the RPC worker source is missing: ${rpc}`);
    if (!existsSync(dir)) return with_(`no candidate build in ${dir}`);
    if (!statSync(dir).isDirectory()) return with_(`unreadable: ${dir} is not a directory`);
    const missing = [...HASHED, MANIFEST].filter((name) => {
      const file = join(dir, name);
      return !existsSync(file) || !statSync(file).isFile();
    });
    if (missing.length > 0) return with_(`${missing.length} file(s) missing, e.g. ${missing[0]}`);
    const parsed = recordedHashes(readFileSync(join(dir, MANIFEST), 'utf8'));
    if (typeof parsed === 'string') return with_(parsed);
    for (const name of HASHED) {
      if (!parsed.has(name)) return with_(`the manifest does not cover ${name}`);
      if (parsed.get(name) !== sha256(join(dir, name))) return with_(`${name} does not match its manifest hash`);
    }
    return { available: true, dir, rpc, reason: 'verified', sha256: Object.fromEntries(parsed) };
  } catch (error) {
    return with_(`unreadable: ${error?.code ?? error?.message ?? 'unknown'}`);
  }
}

const ISOLATION = {
  'Cross-Origin-Opener-Policy': 'same-origin',
  'Cross-Origin-Embedder-Policy': 'credentialless',
};

/**
 * Streams one allowlisted file. The allowlist is the same seven names the production copy emits,
 * so dev can hand out the GPL offer (notices and the source archive) at those paths too.
 * Refused paths never reach `fs`. No per-request hash: a stat failure is a 503, and a candidate
 * whose bytes change under a dev server is re-checked only when a copy is asked to publish them.
 */
export function artemisAssetMiddleware(state) {
  return function serve(request, response, next) {
    if (!request || typeof request.url !== 'string') return typeof next === 'function' ? next() : undefined;
    const url = request.url ?? '';
    if (!url.startsWith(MOUNT)) return next();
    const deny = (code, why) => { response.statusCode = code; response.end(why); };
    if (request.method !== 'GET' && request.method !== 'HEAD') return deny(405, 'method not allowed');
    if (!state.available) return deny(404, 'Artemis withdrawn');
    let relative;
    try {
      relative = decodeURIComponent(url.slice(MOUNT.length).split('?')[0]);
    } catch {
      return deny(400, 'malformed percent-escape');
    }
    // A traversal is refused before the allowlist, so escaping the mount is a 403 rather than a 404
    // and never resolves against `fs` at all. The worker then resolves to package source, which is
    // a constant name, not the request.
    if (resolve(state.dir, relative) !== state.dir && !resolve(state.dir, relative).startsWith(state.dir + sep)) {
      return deny(403, 'forbidden');
    }
    if (!PUBLISH.includes(relative)) return deny(404, 'not found');
    const file = sourceOf(state, relative);
    let handle;
    try {
      handle = openSync(file, 'r');
      if (!fstatSync(handle).isFile()) {
        closeSync(handle);
        return deny(404, 'not found');
      }
    } catch {
      return deny(503, 'unavailable');
    }
    for (const [name, value] of Object.entries(ISOLATION)) response.setHeader(name, value);
    response.setHeader('Content-Type', MIME[extname(relative).toLowerCase()] ?? 'text/plain; charset=utf-8');
    response.setHeader('X-Content-Type-Options', 'nosniff');
    if (request.method === 'HEAD') { closeSync(handle); response.end(); return; }
    const stream = createReadStream(file, { fd: handle, autoClose: true });
    stream.on('error', () => {
      if (!response.headersSent) response.statusCode = 503;
      response.destroy();
    });
    stream.pipe(response);
  };
}

/**
 * Copies exactly the allowlist into a production `outDir`. A withdrawn snapshot is refused.
 * Immediately before the copy the directory is inspected again and each recorded sha256 is
 * compared to this snapshot's — the snapshot is not trusted across that window, and the file
 * bytes are not kept in memory to do it.
 */
export function copyArtemisAssets(state, outDir) {
  if (!state?.available) throw new Error(`refusing to copy a withdrawn engine: ${state?.reason ?? 'no snapshot'}`);
  const again = inspectArtemisAssets({ dir: state.dir, rpc: state.rpc });
  if (!again.available) throw new Error(`refusing to copy: ${again.reason}`);
  for (const name of HASHED) {
    if (again.sha256?.[name] !== state.sha256?.[name]) {
      throw new Error(`refusing to copy: ${name} is not the inspected sha256`);
    }
  }
  const target = join(outDir, ...MOUNT.split('/').filter(Boolean));
  mkdirSync(target, { recursive: true });
  for (const name of PUBLISH) copyFileSync(sourceOf(again, name), join(target, name));
  return PUBLISH;
}

/** Vite plugin: dev and preview. `configureServer` is not an Astro hook. */
export function artemisAssetPlugin(state) {
  const middleware = artemisAssetMiddleware(state);
  const serve = (server) => { server.middlewares.use(middleware); };
  return {
    name: 'online-gal:artemis-assets',
    configureServer: serve,
    configurePreviewServer: serve,
  };
}

/** Astro integration: production copy only, from the same snapshot the plugin serves. */
export function artemisAssetsIntegration(state) {
  return {
    name: 'online-gal:artemis-assets',
    hooks: {
      'astro:build:done'({ dir, logger }) {
        if (!state.available) {
          logger.warn(`Artemis withdrawn: ${state.reason}`);
          return;
        }
        const copied = copyArtemisAssets(state, fileURLToPath(dir));
        logger.info(`Artemis compat: ${copied.length} files emitted under ${MOUNT}`);
      },
    },
  };
}
