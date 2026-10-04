/**
 * The app-side Artemis compat asset seam (`apps/app/scripts/artemis-assets.mjs`).
 *
 * Run: `node --test test/artemis-compat-app-assets.test.mjs`
 *
 * The fixtures are **synthetic bytes**: `artemis-compat.wasm` here is not a WebAssembly module,
 * because this file exercises the gate, the HTTP surface and the copy — not the engine. The manifest
 * is computed from those same synthetic bytes, so "verified" here means exactly what it means in
 * production (the five hashed files match the recorded sha256) and nothing more. A real build is
 * the CI artifact's acceptance; a real game is a separate oracle.
 *
 * No server is started and no network is touched: the middleware is called directly with a request
 * and a `PassThrough` standing in for the response.
 */
import assert from 'node:assert/strict';
import test from 'node:test';
import { createHash } from 'node:crypto';
import { mkdirSync, mkdtempSync, readFileSync, readdirSync, realpathSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join } from 'node:path';
import { pathToFileURL } from 'node:url';
import { PassThrough } from 'node:stream';

import {
  MOUNT,
  PUBLISH,
  SERVE,
  artemisAssetMiddleware,
  artemisAssetPlugin,
  artemisAssetsIntegration,
  copyArtemisAssets,
  inspectArtemisAssets,
} from '../apps/app/scripts/artemis-assets.mjs';

const HASHED = ['artemis-compat.js', 'artemis-compat.wasm', 'artemis-compat-source.tar.gz', 'LICENSE', 'THIRD_PARTY_NOTICES.md'];
const MANIFEST = 'artemis-compat-source.tar.gz.sha256';
const BYTES = {
  'artemis-compat.js': Buffer.from('// synthetic glue, not the real Emscripten output\n'),
  'artemis-compat.wasm': Buffer.from([0x00, 0x61, 0x73, 0x6d, 1, 0, 0, 0]),
  'artemis-compat-source.tar.gz': Buffer.from([0x1f, 0x8b, 8, 0, 0, 0, 0, 0]),
  LICENSE: Buffer.from('GNU GENERAL PUBLIC LICENSE version 3\n'),
  'THIRD_PARTY_NOTICES.md': Buffer.from('# Third-party notices\n'),
};
const RPC = Buffer.from('/* synthetic RPC worker source */\n');

const temp = mkdtempSync(join(tmpdir(), 'artemis-app-assets-'));
process.on('exit', () => {
  if (realpathSync(dirname(temp)) === realpathSync(tmpdir()) && basename(temp).startsWith('artemis-app-assets-')) {
    rmSync(temp, { recursive: true, force: true });
  }
});

/** A candidate build directory: the five hashed files, the worker source, and a real manifest. */
function fixture(name, { manifest, tamper } = {}) {
  const dir = join(temp, name);
  mkdirSync(dir, { recursive: true });
  const rpc = join(dir, 'rpc-worker.js');
  writeFileSync(rpc, RPC);
  for (const [file, bytes] of Object.entries(BYTES)) {
    writeFileSync(join(dir, file), bytes);
  }
  const lines = manifest ?? HASHED.map((file) => `${sha(file, dir)}  ${file}`);
  writeFileSync(join(dir, MANIFEST), `${lines.join('\n')}\n`);
  if (tamper) writeFileSync(join(dir, 'artemis-compat.wasm'), Buffer.concat([BYTES['artemis-compat.wasm'], Buffer.from('x')]));
  return { dir, rpc };
}

function sha(file, dir) {
  return createHash('sha256').update(readFileSync(join(dir, file))).digest('hex');
}

/** GNU two-space, or the binary-mode ` *` sha256sum writes on Windows. */
function manifestLine(file, dir, star) {
  return `${sha(file, dir)} ${star ? '*' : ' '}${file}`;
}

/** Calls the middleware once and returns what a client would have seen. */
async function get(middleware, url, method = 'GET') {
  const response = new PassThrough();
  response.statusCode = 200;
  response.headers = {};
  response.headersSent = false;
  response.setHeader = (name, value) => { response.headers[name.toLowerCase()] = value; };
  response.destroy = () => { response.destroyed = true; response.end(); };
  let passed = false;
  middleware({ url, method }, response, () => { passed = true; });
  const chunks = [];
  if (!passed) {
    for await (const chunk of response) chunks.push(chunk);
  }
  return { status: response.statusCode, type: response.headers['content-type'], headers: response.headers, body: Buffer.concat(chunks), passed };
}

// ---------------------------------------------------------------------------
// the presence gate: presence is not enough, the manifest has to describe *these* bytes
// ---------------------------------------------------------------------------

test('a complete, manifest-verified candidate publishes the engine; every other shape withdraws it', () => {
  const complete = inspectArtemisAssets(fixture('complete'));
  assert.equal(complete.available, true, complete.reason);

  const absent = inspectArtemisAssets({ dir: join(temp, 'nope'), rpc: join(temp, 'complete', 'rpc-worker.js') });
  assert.equal(absent.available, false);
  assert.match(absent.reason, /no candidate build/);

  // Tampered after the manifest was written: present, complete, and still not publishable.
  const tampered = inspectArtemisAssets(fixture('tampered', { tamper: true }));
  assert.equal(tampered.available, false, 'a manifest that does not describe the bytes withdraws the engine');
  assert.match(tampered.reason, /artemis-compat\.wasm does not match/);

  // A manifest that quietly stops covering a file is refused too — a shorter list is not a pass.
  const short = inspectArtemisAssets(fixture('short', { manifest: HASHED.slice(1).map((file) => `0${'0'.repeat(63)}  ${file}`) }));
  assert.equal(short.available, false);
  assert.match(short.reason, /does not cover artemis-compat\.js/);

  // And the RPC worker is part of the input: without it there is nothing to talk to.
  const noRpc = inspectArtemisAssets({ dir: fixture('complete').dir, rpc: join(temp, 'complete', 'absent-worker.js') });
  assert.equal(noRpc.available, false);
  assert.match(noRpc.reason, /RPC worker source is missing/);

  // A directory that is not there withdraws. This is a fixture, not this checkout's dist/.
  const missing = inspectArtemisAssets({ dir: join(temp, 'missing-candidate'), rpc: join(temp, 'complete', 'rpc-worker.js') });
  assert.equal(missing.available, false);
  assert.match(missing.reason, /no candidate build/);
});

// ---------------------------------------------------------------------------
// the HTTP surface: three distinct URLs, correct MIME, nothing else served
// ---------------------------------------------------------------------------

test('the three engine URLs answer with their own bytes and MIME, and nothing else is served', async () => {
  const state = inspectArtemisAssets(fixture('served'));
  const middleware = artemisAssetMiddleware(state);

  const expected = [
    ['compat-worker.js', RPC, 'text/javascript; charset=utf-8'],
    ['artemis-compat.js', BYTES['artemis-compat.js'], 'text/javascript; charset=utf-8'],
    ['artemis-compat.wasm', BYTES['artemis-compat.wasm'], 'application/wasm'],
  ];
  const urls = expected.map(([name]) => `${MOUNT}${name}`);
  assert.equal(new Set(urls).size, 3, 'three distinct URLs');

  for (const [name, bytes, type] of expected) {
    const answer = await get(middleware, `${MOUNT}${name}`);
    assert.equal(answer.status, 200, name);
    assert.equal(answer.type, type, name);
    assert.equal(answer.headers['cross-origin-opener-policy'], 'same-origin', name);
    assert.equal(answer.headers['cross-origin-embedder-policy'], 'credentialless', name);
    assert.equal(answer.headers['x-content-type-options'], 'nosniff', name);
    assert.deepEqual(answer.body, bytes, name);
    // A query string is not part of the name, and HEAD must not carry a body.
    const head = await get(middleware, `${MOUNT}${name}?v=2`, 'HEAD');
    assert.equal(head.status, 200);
    assert.equal(head.type, type);
    assert.equal(head.body.byteLength, 0);
  }

  for (const name of ['LICENSE', 'THIRD_PARTY_NOTICES.md', 'artemis-compat-source.tar.gz', 'artemis-compat-source.tar.gz.sha256']) {
    const offer = await get(middleware, `${MOUNT}${name}`);
    assert.equal(offer.status, 200, name);
    assert.equal(offer.headers['cross-origin-opener-policy'], 'same-origin', name);
    assert.equal(offer.headers['cross-origin-embedder-policy'], 'credentialless', name);
    assert.equal(offer.headers['x-content-type-options'], 'nosniff', name);
  }
  for (const [url, status] of [
    [`${MOUNT}../rpc-worker.js`, 403],
    [`${MOUNT}%2e%2e%2frpc-worker.js`, 403],
    [`${MOUNT}%2`, 400],
    [`${MOUNT}sub/dir/artemis-compat.wasm`, 404],
    [`${MOUNT}missing.wasm`, 404],
  ]) {
    assert.equal((await get(middleware, url)).status, status, url);
  }
  assert.equal((await get(middleware, '/krkr2-web/index.html')).passed, true, 'other mounts fall through');
});

test('a withdrawn engine serves nothing, and the copy emits exactly the allowlist', () => {
  const withdrawn = artemisAssetMiddleware(inspectArtemisAssets({ dir: join(temp, 'nope'), rpc: join(temp, 'nope', 'w.js') }));
  return get(withdrawn, `${MOUNT}artemis-compat.wasm`).then((answer) => {
    assert.equal(answer.status, 404);
    assert.match(answer.body.toString(), /withdrawn/);
  }).then(() => {
    const state = inspectArtemisAssets(fixture('copied'));
    const outDir = join(temp, 'out');
    const copied = copyArtemisAssets(state, outDir);
    assert.deepEqual(copied, PUBLISH);
    const target = join(outDir, 'engines', 'artemis-compat');
    assert.deepEqual(readdirSync(target).sort(), [...PUBLISH].sort(), 'no extra file, none missing');
    assert.deepEqual(readFileSync(join(target, 'compat-worker.js')), RPC, 'the RPC worker comes from the package source');
    assert.deepEqual(readFileSync(join(target, 'LICENSE')), BYTES.LICENSE);
    assert.deepEqual(readFileSync(join(target, 'artemis-compat-source.tar.gz')), BYTES['artemis-compat-source.tar.gz']);
    assert.equal(readFileSync(join(target, MANIFEST), 'utf8'), readFileSync(join(state.dir, MANIFEST), 'utf8'), 'the manifest ships beside the bytes it describes');
    for (const name of SERVE) {
      assert.equal(readFileSync(join(target, name)).byteLength > 0, true, `${name} is non-empty`);
    }
    assert.throws(() => copyArtemisAssets({ available: false, reason: 'withdrawn' }, join(temp, 'refused')), /withdrawn/);
    writeFileSync(join(state.dir, 'artemis-compat.wasm'), Buffer.concat([BYTES['artemis-compat.wasm'], Buffer.from('y')]));
    assert.throws(() => copyArtemisAssets(state, join(temp, 'stale')), /not the inspected sha256|does not match/);
  });
});

/** Rewrite a fixture's manifest after its bytes exist. `star` is sha256sum's binary mode. */
function rewriteManifest(name, star, mutate) {
  const built = fixture(name);
  const lines = HASHED.map((file) => manifestLine(file, built.dir, star));
  writeFileSync(join(built.dir, MANIFEST), `${(mutate ? mutate(lines, built.dir) : lines).join('\n')}\n`);
  return built;
}

test('the manifest is exactly the five safe names, in either sha256sum spelling', () => {
  assert.equal(inspectArtemisAssets(rewriteManifest('star', true)).available, true);
  assert.equal(inspectArtemisAssets(rewriteManifest('spaced', false)).available, true);
  assert.match(inspectArtemisAssets(rewriteManifest('doubled', false, (lines, dir) => [...lines, manifestLine(HASHED[0], dir, false)])).reason, /lists artemis-compat\.js twice/);
  assert.match(inspectArtemisAssets(rewriteManifest('extra', false, (lines) => [...lines, `${'a'.repeat(64)}  notes.txt`])).reason, /notes\.txt/);
  assert.match(inspectArtemisAssets(rewriteManifest('path', false, (lines) => lines.map((line, i) => (i === 0 ? `${'b'.repeat(64)}  sub/${HASHED[0]}` : line)))).reason, /not a sha256 of a safe filename/);
  assert.equal(inspectArtemisAssets(rewriteManifest('upper', false, (lines) => lines.map((line) => line.replace(/^[0-9a-f]/, (c) => c.toUpperCase())))).available, true);
});

test('a file that cannot be read withdraws the gate, and a vanished file is a 503', async () => {
  const gone = fixture('gone');
  const unread = inspectArtemisAssets({ ...gone, dir: join(gone.dir, MANIFEST) });
  assert.equal(unread.available, false);
  assert.match(unread.reason, /unreadable/);

  const state = inspectArtemisAssets(fixture('stream'));
  const vanished = { ...state, rpc: join(temp, 'stream', 'not-there.js') };
  const missing = await get(artemisAssetMiddleware(vanished), `${MOUNT}compat-worker.js`);
  assert.equal(missing.status, 503);
  assert.match(missing.body.toString(), /unavailable/);

  const post = await get(artemisAssetMiddleware(state), `${MOUNT}artemis-compat.wasm`, 'POST');
  assert.equal(post.status, 405);
});

test('the offer files carry the MIME a client needs, and the gzip archive is not re-labelled', async () => {
  const middleware = artemisAssetMiddleware(inspectArtemisAssets(fixture('mimes')));

  // The source archive is *already* gzip. It must be described as such by Content-Type, and the
  // response must not add a transfer Content-Encoding: a client that stores what it downloaded
  // has to get the bytes the manifest hashed, not a decoded tar.
  const gz = await get(middleware, `${MOUNT}artemis-compat-source.tar.gz`);
  assert.equal(gz.status, 200);
  assert.equal(gz.type, 'application/gzip');
  assert.equal(gz.headers['content-encoding'], undefined, 'no transfer gzip on an already-gzip file');
  assert.deepEqual(gz.body, BYTES['artemis-compat-source.tar.gz'], 'the bytes on the wire are the file');
  assert.equal(gz.body.subarray(0, 2).toString('hex'), '1f8b');

  // Extensionless and unknown-extension offers still get a usable, non-empty type.
  for (const name of ['LICENSE', 'artemis-compat-source.tar.gz.sha256']) {
    const offer = await get(middleware, `${MOUNT}${name}`);
    assert.equal(offer.status, 200, name);
    assert.match(String(offer.type), /^text\/plain/, name);
  }
  assert.equal((await get(middleware, `${MOUNT}THIRD_PARTY_NOTICES.md`)).type, 'text/plain; charset=utf-8');
});

test('every served and offered file carries the isolation and no-sniff header contract', async () => {
  const middleware = artemisAssetMiddleware(inspectArtemisAssets(fixture('headers')));
  for (const name of PUBLISH) {
    const answer = await get(middleware, `${MOUNT}${name}`);
    assert.equal(answer.status, 200, name);
    assert.equal(answer.headers['cross-origin-opener-policy'], 'same-origin', name);
    assert.equal(answer.headers['cross-origin-embedder-policy'], 'credentialless', name);
    assert.equal(answer.headers['x-content-type-options'], 'nosniff', name);
    assert.notEqual(answer.type, undefined, `${name} has a Content-Type`);
    assert.notEqual(String(answer.type).trim(), '', `${name} MIME is non-empty`);
  }

  // A query string selects the same file; HEAD reports the same metadata with no body.
  for (const name of SERVE) {
    const head = await get(middleware, `${MOUNT}${name}?v=2`, 'HEAD');
    assert.equal(head.status, 200, name);
    assert.equal(head.body.byteLength, 0, name);
    assert.equal(head.headers['x-content-type-options'], 'nosniff', name);
    assert.equal(head.headers['cross-origin-opener-policy'], 'same-origin', name);
  }
});

test('malformed and escaping paths are refused with a client error, never a server error', async () => {
  const middleware = artemisAssetMiddleware(inspectArtemisAssets(fixture('refusals')));
  for (const [url, status] of [
    [`${MOUNT}%2`, 400],
    [`${MOUNT}%zz`, 400],
    [`${MOUNT}../rpc-worker.js`, 403],
    [`${MOUNT}%2e%2e%2frpc-worker.js`, 403],
    [`${MOUNT}deep/../../rpc-worker.js`, 403],
    [`${MOUNT}sub/dir/artemis-compat.wasm`, 404],
    [`${MOUNT}missing.wasm`, 404],
    [`${MOUNT}`, 404],
  ]) {
    const answer = await get(middleware, url);
    assert.equal(answer.status, status, url);
    assert.notEqual(answer.status, 500, `${url} must not be a 500`);
  }
});

test('a foreign mount and an unknown path are never answered by this middleware', async () => {
  const middleware = artemisAssetMiddleware(inspectArtemisAssets(fixture('catalog')));

  // The middleware owns exactly one mount. The /engines/ catalog page and every other route must
  // fall through untouched, so the app's own routing (not this seam) decides what they are.
  for (const url of ['/', '/engines/', '/engines/artemis-compat', '/krkr2-web/index.html', '/play', '/settings']) {
    const answer = await get(middleware, url);
    assert.equal(answer.passed, true, `${url} falls through`);
    assert.equal(answer.body.byteLength, 0, `${url} writes nothing`);
  }

  // A sibling engine directory is a different mount and must not be served from this one.
  const sibling = await get(middleware, '/engines/other-engine/artemis-compat.wasm');
  assert.equal(sibling.passed, true, 'a sibling engine mount is not this middleware');

  // And a withdrawn snapshot answers on its own mount only.
  const withdrawn = artemisAssetMiddleware({ available: false, dir: join(temp, 'nope'), reason: 'withdrawn' });
  assert.equal((await get(withdrawn, `${MOUNT}artemis-compat.wasm`)).status, 404);
  assert.equal((await get(withdrawn, '/engines/')).passed, true);
});

test('the config puts the same snapshot on a Vite plugin and an Astro build hook', async () => {
  const state = inspectArtemisAssets(fixture('wired'));
  const plugin = artemisAssetPlugin(state);
  const integration = artemisAssetsIntegration(state);
  assert.equal(typeof plugin.configureServer, 'function');
  assert.equal(typeof plugin.configurePreviewServer, 'function');
  assert.equal(plugin.hooks, undefined, 'configureServer is a Vite hook, not an Astro one');
  assert.equal(typeof integration.hooks['astro:build:done'], 'function');
  assert.equal(integration.configureServer, undefined);

  const used = [];
  const server = { middlewares: { use(fn) { used.push(fn); } } };
  plugin.configureServer(server);
  plugin.configurePreviewServer(server);
  assert.equal(used.length, 2);
  assert.equal(used[0], used[1], 'dev and preview mount the same middleware');
  const served = await get(used[0], `${MOUNT}artemis-compat.wasm`);
  assert.equal(served.status, 200);
  assert.deepEqual(served.body, BYTES['artemis-compat.wasm']);

  const logs = [];
  const out = join(temp, 'hook-out');
  integration.hooks['astro:build:done']({
    dir: pathToFileURL(out),
    logger: { warn(m) { logs.push(['warn', m]); }, info(m) { logs.push(['info', m]); } },
  });
  assert.match(logs[0][1], /7 files emitted/);
  assert.deepEqual(readdirSync(join(out, 'engines', 'artemis-compat')).sort(), [...PUBLISH].sort());

  const quiet = [];
  artemisAssetsIntegration({ available: false, reason: 'fixture withdrawn' }).hooks['astro:build:done']({
    dir: pathToFileURL(join(temp, 'hook-withdrawn')),
    logger: { warn(m) { quiet.push(m); }, info() {} },
  });
  assert.match(quiet[0], /fixture withdrawn/);

  const astro = await import('../apps/app/astro.config.mjs');
  const live = astro.default;
  const liveIntegration = live.integrations.find((item) => item?.name === 'online-gal:artemis-assets');
  const livePlugin = live.vite.plugins.find((item) => item?.name === 'online-gal:artemis-assets');
  assert.equal(typeof liveIntegration.hooks['astro:build:done'], 'function');
  assert.equal(liveIntegration.configureServer, undefined);
  assert.equal(typeof livePlugin.configureServer, 'function');
  assert.equal(typeof livePlugin.configurePreviewServer, 'function');
  assert.equal(livePlugin.hooks, undefined);
  assert.match(String(live.vite.define.__ARTEMIS_ASSETS_AVAILABLE__), /^(true|false)$/);

  // `astro preview` builds Vite with `configFile: false`, so the `vite.plugins` middleware above
  // does NOT run there. `server.headers` is forwarded to Vite's preview headers and is the only
  // lever preview honours, so the no-sniff contract has to be reachable from that map or the
  // engine files are served snifffable in preview.
  assert.equal(live.server.headers['X-Content-Type-Options'], 'nosniff');
  assert.equal(live.server.headers['Cross-Origin-Opener-Policy'], 'same-origin');
  assert.equal(live.server.headers['Cross-Origin-Embedder-Policy'], 'credentialless');
});