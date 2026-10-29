/**
 * Tests for the asyncify asset seam (`packages/engine/core/artemis-compat/src/assets.ts`).
 *
 * The module is compiled with the repo's own `tsc` into a temp directory and linked to the
 * workspace's `node_modules`, the same way `artemis-boot.test.mjs` does it, so the test runs
 * against the TypeScript that ships rather than against a hand-written stub of it.
 *
 * Everything else is a fake `GameSource`: multi-gigabyte offsets, counters for what was
 * actually read, and a 29,224-name loose tree for the calls that must never walk it.
 */
import assert from 'node:assert/strict';
import test from 'node:test';
import { execFileSync } from 'node:child_process';
import { lstatSync, mkdtempSync, realpathSync, rmSync, symlinkSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join, relative, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const repoRoot = resolve(fileURLToPath(new URL('..', import.meta.url)));
const inside = (parent, child) => {
  const rel = relative(parent, child);
  return rel !== '' && !rel.startsWith('..');
};

const workDir = mkdtempSync(join(tmpdir(), 'artemis-assets-'));
// Registered before anything is imported, so a failing compile or import still cleans up.
process.on('exit', () => {
  if (realpathSync(dirname(workDir)) !== realpathSync(tmpdir())) return;
  if (!basename(workDir).startsWith('artemis-assets-')) return;
  rmSync(workDir, { recursive: true, force: true });
});

const outDir = join(workDir, 'out');
execFileSync(
  process.execPath,
  [
    join(repoRoot, 'node_modules', 'typescript', 'bin', 'tsc'),
    'packages/engine/core/artemis-compat/src/assets.ts',
    '--target', 'ES2022',
    '--module', 'ESNext',
    '--moduleResolution', 'bundler',
    '--lib', 'ES2022,DOM',
    '--strict',
    '--skipLibCheck',
    '--rootDir', 'packages/engine/core/artemis-compat/src',
    '--outDir', outDir,
  ],
  { cwd: repoRoot, stdio: 'inherit' },
);
writeFileSync(join(outDir, 'package.json'), '{"type":"module"}');

// The output lives in the OS temp dir, so the bare `@online-gal/engine-api` specifier can only
// resolve through this one link into the workspace. Resolution is asserted from inside the
// output before anything relies on it.
const link = join(outDir, 'node_modules');
symlinkSync(join(repoRoot, 'packages', 'engine', 'core', 'artemis-compat', 'node_modules'), link, 'junction');
assert.equal(lstatSync(link).isSymbolicLink(), true, 'the node_modules link is a real symlink');
const probe = join(outDir, 'resolve-probe.mjs');
writeFileSync(probe, "export const url = import.meta.resolve('@online-gal/engine-api');\n");
const resolvedApi = realpathSync(fileURLToPath((await import(pathToFileURL(probe).href)).url));
assert.ok(inside(repoRoot, resolvedApi), `resolved outside the workspace: ${resolvedApi}`);

const { createArtemisAssets, patchSystemIni, deriveSavePath, MissingSystemIniError, DEFAULT_ASYNCIFY_URL, DEFAULT_ARTEMIS_ASSET_CACHE_BYTES } =
  await import(pathToFileURL(join(outDir, 'assets.js')).href);

// ---------------------------------------------------------------------------
// fixtures: packers, readers, and the fake GameSource
// ---------------------------------------------------------------------------

const utf8 = (text) => new TextEncoder().encode(text);

const u32leAt = (bytes, offset) => new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength).getUint32(offset, true);

const put32 = (out, value) => {
  out.push(value & 0xff, (value >>> 8) & 0xff, (value >>> 16) & 0xff, (value >>> 24) & 0xff);
};

/**
 * Writes a `pf6` the way the SDK packer does, trailer included.
 *
 * Written independently of the module under test on purpose: a test that reuses the writer to
 * check the writer proves nothing about the trailer. Names are stored with `\` and sorted with a
 * separator comparing below every other byte, as the packer does.
 */
function packPf6(files, { version = '6', trailer = true } = {}) {
  const encoded = files
    .map((file) => ({ raw: utf8(file.name.replace(/\//g, '\\')), data: file.data }))
    .sort((a, b) => {
      const length = Math.min(a.raw.byteLength, b.raw.byteLength);
      for (let index = 0; index < length; index += 1) {
        const left = a.raw[index] === 0x5c || a.raw[index] === 0x2f ? 1 : a.raw[index];
        const right = b.raw[index] === 0x5c || b.raw[index] === 0x2f ? 1 : b.raw[index];
        if (left !== right) return left - right;
      }
      return a.raw.byteLength - b.raw.byteLength;
    });

  const buildIndex = (offsets) => {
    const out = [];
    const nameEnds = [];
    let cursor = 4;
    put32(out, encoded.length);
    encoded.forEach((file, index) => {
      put32(out, file.raw.byteLength);
      for (const byte of file.raw) out.push(byte);
      cursor += 4 + file.raw.byteLength;
      nameEnds.push(cursor);
      put32(out, 0);
      put32(out, offsets[index]);
      put32(out, file.data.byteLength);
      cursor += 12;
    });
    if (trailer) {
      put32(out, encoded.length + 1);
      for (const nameEnd of nameEnds) {
        put32(out, nameEnd);
        put32(out, 0);
      }
      put32(out, 0);
      put32(out, 0);
      put32(out, cursor);
    }
    return new Uint8Array(out);
  };

  const sized = buildIndex(encoded.map(() => 0));
  let cursor = 7 + sized.byteLength;
  const offsets = encoded.map((file) => {
    const at = cursor;
    cursor += file.data.byteLength;
    return at;
  });
  const index = buildIndex(offsets);
  const payload = encoded.reduce((sum, file) => sum + file.data.byteLength, 0);
  const out = new Uint8Array(7 + index.byteLength + payload);
  out.set(utf8(`pf${version}`), 0);
  new DataView(out.buffer).setUint32(3, index.byteLength, true);
  out.set(index, 7);
  let at = 7 + index.byteLength;
  for (const file of encoded) {
    out.set(file.data, at);
    at += file.data.byteLength;
  }
  return out;
}

const MAGIC = { '6': 0x36, '8': 0x38, '9': 0x39 };

/** `SHA-1(<index bytes>)`, the payload keystream the keyed container versions use. */
function keyOf(bytes) {
  const tableLength = u32leAt(bytes, 3);
  return sha1Sync(bytes.subarray(7, 7 + tableLength));
}

// A local SHA-1, so the test's expectation for a keyed archive does not depend on the module's
// own digest. Straight FIPS 180-1; only the round count and constants matter here.
function sha1Sync(input) {
  const message = new Uint8Array((((input.byteLength + 8) >> 6) + 1) * 64);
  message.set(input);
  message[input.byteLength] = 0x80;
  const bits = BigInt(input.byteLength) * 8n;
  for (let index = 0; index < 8; index += 1) {
    message[message.byteLength - 8 + index] = Number((bits >> BigInt(56 - index * 8)) & 0xffn);
  }
  const view = new DataView(message.buffer);
  let h0 = 0x67452301, h1 = 0xefcdab89, h2 = 0x98badcfe, h3 = 0x10325476, h4 = 0xc3d2e1f0;
  const w = new Uint32Array(80);
  for (let block = 0; block < message.byteLength; block += 64) {
    for (let i = 0; i < 16; i += 1) w[i] = view.getUint32(block + i * 4, false);
    for (let i = 16; i < 80; i += 1) {
      const x = w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16];
      w[i] = ((x << 1) | (x >>> 31)) >>> 0;
    }
    let a = h0, b = h1, c = h2, d = h3, e = h4;
    for (let i = 0; i < 80; i += 1) {
      const f = i < 20 ? (b & c) | (~b & d) : i < 40 ? b ^ c ^ d : i < 60 ? (b & c) | (b & d) | (c & d) : b ^ c ^ d;
      const k = i < 20 ? 0x5a827999 : i < 40 ? 0x6ed9eba1 : i < 60 ? 0x8f1bbcdc : 0xca62c1d6;
      const temp = (((a << 5) | (a >>> 27)) + f + e + k + w[i]) >>> 0;
      e = d; d = c; c = (b << 30) | (b >>> 2); b = a; a = temp;
    }
    h0 = (h0 + a) >>> 0; h1 = (h1 + b) >>> 0; h2 = (h2 + c) >>> 0; h3 = (h3 + d) >>> 0; h4 = (h4 + e) >>> 0;
  }
  const out = new Uint8Array(20);
  const outView = new DataView(out.buffer);
  [h0, h1, h2, h3, h4].forEach((word, index) => outView.setUint32(index * 4, word, false));
  return out;
}

/**
 * A `GameSource` over a map of path → bytes.
 *
 * `declared` gives a path a size that its bytes need not have, which is what makes a 3.36 GB
 * archive without allocating one: nothing here reads the file whole, so a ranged read past the
 * end of the resident buffer is still a legitimate short read.
 */
function fakeSource(files, { looseCount = 0, declared = {}, log = [] } = {}) {
  const stats = { reads: 0, ranges: 0, lists: 0, wholeReads: 0, bytes: 0 };
  const sizeOf = (path) => declared[path] ?? files.get(path)?.byteLength ?? -1;
  const source = {
    name: 'fake',
    stats,
    capabilities: { list: true, randomAccess: true, writable: false },
    async list(dir = '') {
      if (dir !== '' && dir !== '/') throw new Error(`list() called with ${JSON.stringify(dir)}`);
      stats.lists += 1;
      const slash = dir.endsWith('/') && dir !== '' ? dir : dir === '' ? '' : `${dir}/`;
      const names = new Set();
      for (const key of files.keys()) {
        if (!key.startsWith(slash)) continue;
        const rest = key.slice(slash.length);
        if (rest.includes('/')) names.add(`${rest.split('/')[0]}/`);
        else names.add(rest);
      }
      for (let index = 0; index < looseCount; index += 1) {
        names.add(`loose/asset_${String(index).padStart(5, '0')}.bin`);
      }
      return [...names].sort();
    },
    async isDirectory(path) {
      return [...files.keys()].some((key) => key.startsWith(`${path.replace(/\/$/, '')}/`));
    },
    async exists(path) {
      return files.has(path) || path in declared;
    },
    async size(path) {
      if (files.has(path) || path in declared) return sizeOf(path);
      throw new Error(`no such file: ${path}`);
    },
    async readFile(path) {
      const bytes = files.get(path);
      if (!bytes) throw new Error(`no such file: ${path}`);
      stats.reads += 1;
      stats.wholeReads += 1;
      stats.bytes += bytes.byteLength;
      log.push([path, 0, bytes.byteLength]);
      return bytes.slice();
    },
    async readRange(path, offset, length) {
      const bytes = files.get(path);
      if (!bytes) throw new Error(`no such file: ${path}`);
      stats.ranges += 1;
      stats.reads += 1;
      // Only what the resident buffer can answer; a declared multi-gigabyte size means the
      // reads that matter are the small ones at the front.
      const slice = bytes.subarray(offset, Math.min(offset + length, bytes.byteLength));
      stats.bytes += slice.byteLength;
      log.push([path, offset, slice.byteLength]);
      return slice.slice();
    },
  };
  return source;
}

/**
 * Splits patched INI text into `heading → body lines`, in file order.
 *
 * A whole-file regex cannot say *which* section a key landed in, which is exactly the property
 * that matters when several `[WASM]` blocks are consolidated past an intervening section.
 */
function sectionsOf(text) {
  const sections = new Map();
  let current = null;
  for (const line of text.split('\r\n')) {
    if (/^\s*\[/.test(line)) {
      current = line.trim().toUpperCase();
      sections.set(current, []);
      continue;
    }
    if (current !== null) sections.get(current).push(line);
  }
  return sections;
}

/** A `system.ini` shaped like the real game's: a `[WASM]` block the publisher wrote. */
const REAL_INI = [
  '[WASM]',
  'WIDTH = 1280',
  'HEIGHT = 720',
  'CHARSET = UTF-8',
  'BOOT = boot.iet',
  'ASYNCIFY_HASH_LIST_URL = http://old.example/hashlist.txt',
  '',
  '[FILE]',
  'SAVEPATH = まどそふと\\ハミダシクリエイティブ',
  'CHARSET = SHIFT-JIS',
  '',
].join('\r\n');

// ---------------------------------------------------------------------------
// the module's exported contract
// ---------------------------------------------------------------------------

test('the module exports the contract the browser session consumes', async () => {
  assert.deepEqual(
    Object.keys({ createArtemisAssets, patchSystemIni, deriveSavePath, MissingSystemIniError }).sort(),
    ['MissingSystemIniError', 'createArtemisAssets', 'deriveSavePath', 'patchSystemIni'],
  );
  assert.equal(DEFAULT_ASYNCIFY_URL, 'artemis-assets/');
  assert.equal(DEFAULT_ARTEMIS_ASSET_CACHE_BYTES, 32 * 1024 * 1024);
});

// ---------------------------------------------------------------------------
// system.ini patching
// ---------------------------------------------------------------------------

test('patchSystemIni writes exactly one [WASM] block and drops the managed keys it does not set', () => {
  const patched = patchSystemIni(REAL_INI, { savePath: 'assets/title/', baseUrl: 'artemis-assets/' });
  assert.equal(patched.savePath, 'assets/title/');
  assert.equal(patched.asyncifyUrl, 'artemis-assets/');
  assert.equal(patched.logicalMount, 'ASSETS/TITLE/');

  const wasm = patched.text.slice(patched.text.indexOf('[WASM]'));
  assert.equal((patched.text.match(/\[WASM\]/g) ?? []).length, 1, '[WASM] appears exactly once');
  assert.equal((patched.text.match(/ASYNCIFY_URL/g) ?? []).length, 1, 'ASYNCIFY_URL appears exactly once');
  assert.equal((patched.text.match(/SAVEPATH/g) ?? []).length, 2, 'one [WASM] SAVEPATH, one [FILE] SAVEPATH');
  assert.match(patched.text, /^\[WASM\]\r\nSAVEPATH = assets\/title\/\r\nASYNCIFY_URL = artemis-assets\/\r\n/m);
  assert.ok(!patched.text.includes('ASYNCIFY_HASH_LIST_URL'), 'the stale hash list URL is removed');
  assert.ok(!wasm.includes('http://old.example'), 'the stale value is gone, not merely its key');
  assert.ok(!patched.text.includes('\n\n'), 'no new blank line is introduced');
  assert.ok(patched.text.includes('[FILE]\r\nSAVEPATH = まどそふと'), 'the [FILE] SAVEPATH is untouched');
  assert.ok(patched.text.includes('WIDTH = 1280') && patched.text.includes('BOOT = boot.iet'), 'unmanaged keys survive');
  // Every line break is still CRLF. The *count* is not compared to the original's: the patch
  // drops one managed key and writes two, so the file legitimately gains a line, and freezing
  // the count would contradict the assertions above.
  assert.ok(
    patched.text.split('\r\n').every((line) => !line.includes('\n')),
    'no bare LF anywhere: the CRLF convention is preserved',
  );
  assert.equal((patched.text.match(/\r\n/g) ?? []).length, patched.text.split('\r\n').length - 1, 'every break is CRLF');
});

test('CHARSET is never rewritten, because the engine converts script paths into it', () => {
  const patched = patchSystemIni(REAL_INI, { savePath: 's', baseUrl: 'b/' });
  assert.match(patched.text, /^CHARSET = UTF-8$/m);
  assert.match(patched.text, /^CHARSET = SHIFT-JIS$/m);
});

test('a system.ini without [WASM] gains one, and the rest of the file keeps its shape', () => {
  const patched = patchSystemIni('[FILE]\r\nSAVEPATH = x\r\n\r\n[UPDATE]\r\nA = 1\r\n', {
    savePath: 'derived/',
    baseUrl: 'artemis-assets/',
  });
  // `m` so the assertion anchors at the `[UPDATE]` line rather than at the start of a file that
  // legitimately begins with `[FILE]`.
  assert.match(
    patched.text,
    /^\[UPDATE\]\r\nA = 1\r\n\r\n\[WASM\]\r\nSAVEPATH = derived\/\r\nASYNCIFY_URL = artemis-assets\/$/m,
  );
  assert.ok(!patched.text.includes('\n\n\n'), 'no double blank line at the seam');
});

test('[WASM] is case-folded on the way in, and managed keys are matched case-insensitively', () => {
  const patched = patchSystemIni('[wasm]\r\nsavepath = old\r\nwidth = 5\r\nasYnCiFy_hAsH_lIsT_uRl = stale\r\n', {
    savePath: 'new/',
  });
  // The heading is case-folded, the written keys use the engine's own spelling, and every
  // managed key is matched case-insensitively. The engine's own INI reader folds keys, so a
  // lowercase `savepath` is the *same* key as the one this module writes: leaving it behind
  // would ship a file carrying two mount directories and a stale hash list URL.
  assert.equal(
    patched.text,
    '[WASM]\r\nSAVEPATH = new/\r\nASYNCIFY_URL = artemis-assets/\r\nwidth = 5\r\n',
    'the mixed-case managed keys are gone and the unmanaged key is untouched',
  );
  assert.equal((patched.text.match(/savepath/gi) ?? []).length, 1, 'exactly one canonical SAVEPATH');
  assert.equal((patched.text.match(/ASYNCIFY_URL/gi) ?? []).length, 1, 'exactly one ASYNCIFY_URL');
  assert.ok(!patched.text.includes('old'), 'no stale lowercase value survives');
  assert.ok(!patched.text.includes('stale'), 'the mixed-case hash list key is removed');
});

test('several [WASM] blocks are consolidated into one, keeping other sections and their order', () => {
  const ini = [
    '[WASM]',
    'WIDTH = 1280',
    'savepath = stale-lowercase',
    '',
    '[FILE]',
    'SAVEPATH = native',
    'CHARSET = SHIFT-JIS',
    '',
    '[wasm]',
    'BOOT = boot.iet',
    'ASYNCIFY_HASH_LIST_URL = http://old.example/hashlist.txt',
    '',
    '[UPDATE]',
    'A = 1',
    '',
  ].join('\r\n');
  const patched = patchSystemIni(ini, { savePath: 'new/' });

  assert.equal((patched.text.match(/\[WASM\]/gi) ?? []).length, 1, 'exactly one heading survives');
  assert.equal((patched.text.match(/SAVEPATH/gi) ?? []).length, 2, 'one managed, one native [FILE]');
  assert.equal((patched.text.match(/ASYNCIFY_URL/gi) ?? []).length, 1, 'written once');
  // `BOOT = boot.iet` spells the token twice, in the key and in the value, so the count is
  // taken at key position rather than anywhere in the line.
  assert.equal((patched.text.match(/^BOOT\s*=/gim) ?? []).length, 1, 'BOOT survives exactly once as a key');

  // Section membership is read out of the patched text rather than matched against a whole-file
  // regex. A regex frozen from the broken output agreed with it: the second block's unmanaged
  // keys — `BOOT` above all — were emitted after the intervening `[FILE]` heading, where the
  // engine reads them under `[FILE]` and never finds the boot script. What matters is which
  // section each key ends up in, so that is what is parsed and asserted.
  const sections = sectionsOf(patched.text);
  const keysIn = (heading) => sections.get(heading).filter((line) => line.trim() !== '');
  assert.deepEqual(
    [...sections.keys()],
    ['[WASM]', '[FILE]', '[UPDATE]'],
    'the surviving sections keep their file order',
  );
  assert.equal(
    keysIn('[WASM]').some((line) => /^BOOT\s*=/i.test(line)),
    true,
    'BOOT is consolidated into [WASM], where the engine reads it',
  );
  assert.equal(
    keysIn('[FILE]').some((line) => /^BOOT\s*=/i.test(line)),
    false,
    'BOOT is not stranded under [FILE]',
  );
  assert.deepEqual(
    keysIn('[WASM]'),
    ['SAVEPATH = new/', 'ASYNCIFY_URL = artemis-assets/', 'WIDTH = 1280', 'BOOT = boot.iet'],
    'the written keys lead, then every unmanaged key of both blocks in file order',
  );
  assert.deepEqual(
    keysIn('[FILE]'),
    ['SAVEPATH = native', 'CHARSET = SHIFT-JIS'],
    'the native [FILE] keys are untouched and in place',
  );
  assert.deepEqual(keysIn('[UPDATE]'), ['A = 1'], 'a later section keeps its own shape');

  assert.ok(!patched.text.includes('stale-lowercase'), 'the stale lowercase value is gone');
  assert.ok(!patched.text.includes('hashlist.txt'), 'the stale hash list URL is gone');
  assert.ok(patched.text.includes('[FILE]\r\nSAVEPATH = native'), 'the native [FILE] SAVEPATH is untouched');
  assert.ok(!patched.text.includes('\n\n\n'), 'no double blank line at any seam');
  assert.ok(patched.text.endsWith('A = 1\r\n'), 'the file keeps its trailing newline');
});

test('deriveSavePath keeps ASCII, hashes what has none, and separates namespaces and origins', () => {
  assert.equal(deriveSavePath('まどそふと\\ハミダシクリエイティブ', 'games/a/', 'https://a.example/x/'), deriveSavePath('まどそふと\\ハミダシクリエイティブ', 'games/a/', 'https://a.example/x/'));
  const first = deriveSavePath('まどそふと\\ハミダシクリエイティブ', 'games/a/', 'https://a.example/x/');
  assert.match(first, /^artemis_[0-9a-f]{10}$/);
  assert.notEqual(first, deriveSavePath('まどそふと\\ハミダシクリエイティブ', 'games/b/', 'https://a.example/x/'));
  assert.notEqual(first, deriveSavePath('まどそふと\\ハミダシクリエイティブ', 'games/a/', 'https://b.example/x/'));
  assert.notEqual(first, deriveSavePath('まどそふと\\ハミダシクリエイティブ', 'games/a/', 'artemis-assets/'));
  assert.equal(deriveSavePath('My Game\\Title', '', ''), 'mygametitle');
});

test('the derived SAVEPATH comes from the original configuration and stays stable ASCII', async () => {
  const assets = await createArtemisAssets(fakeSource(new Map([['system.ini', utf8(REAL_INI)]])));
  assert.match(assets.savePath, /^artemis_[0-9a-f]{10}$/);
  const again = await createArtemisAssets(fakeSource(new Map([['system.ini', utf8(REAL_INI)]])));
  assert.equal(assets.savePath, again.savePath, 'two runs over the same tree agree');
  const text = new TextDecoder().decode(assets.config);
  assert.ok(text.includes(`SAVEPATH = ${assets.savePath}`));
  assert.ok(text.includes('ASYNCIFY_URL = artemis-assets/'));
});

test('an explicitly configured savePath is written verbatim, not derived', async () => {
  const assets = await createArtemisAssets(fakeSource(new Map([['system.ini', utf8(REAL_INI)]])), {
    savePath: 'assets/hamidashi/',
  });
  assert.equal(assets.savePath, 'assets/hamidashi/');
  assert.ok(new TextDecoder().decode(assets.config).includes('SAVEPATH = assets/hamidashi/'));
});

test('a non-UTF-8 configuration is rejected rather than silently rewritten', async () => {
  // The patch re-encodes with `TextEncoder`, which only ever emits UTF-8. Accepting a Shift_JIS
  // file therefore meant decoding it as Shift_JIS and writing it back as UTF-8: every non-ASCII
  // path in it changed bytes, and the engine converted script paths against the wrong table.
  // Both spellings are refused — a *valid* Shift_JIS file is the dangerous one, because it is
  // exactly the case that used to be accepted and silently corrupted.
  const validShiftJis = new Uint8Array([...utf8('[WASM]\r\nWIDTH = 1\r\n'), 0x81, 0x60]);
  await assert.rejects(
    createArtemisAssets(fakeSource(new Map([['system.ini', validShiftJis]]))),
    /not valid UTF-8/,
    'valid Shift_JIS is refused, not re-encoded',
  );

  // 0x80 0x80 is neither valid UTF-8 nor valid Shift_JIS.
  await assert.rejects(
    createArtemisAssets(fakeSource(new Map([['system.ini', new Uint8Array([0x41, 0x80, 0x80, 0x42])]]))),
    /not valid UTF-8/,
  );

  // The refusal is thrown by the patch itself, so a direct caller gets it too, and the message
  // names the file rather than leaving the caller to guess which tree it came from.
  assert.throws(() => patchSystemIni(validShiftJis, { where: 'system.ini' }), /system\.ini: not valid UTF-8/);
});

test('a UTF-8 BOM is preserved, and the bytes round-trip exactly', async () => {
  const body = '[WASM]\r\nWIDTH = 1\r\nCHARSET = UTF-8\r\n';
  const bom = new Uint8Array([0xef, 0xbb, 0xbf, ...utf8(body)]);
  const assets = await createArtemisAssets(fakeSource(new Map([['system.ini', bom]])));

  assert.deepEqual(assets.config.subarray(0, 3), new Uint8Array([0xef, 0xbb, 0xbf]), 'the BOM is kept');
  const text = new TextDecoder('utf-8').decode(assets.config.subarray(3));
  assert.ok(text.startsWith('[WASM]'), 'the BOM is not mistaken for file content');
  assert.ok(text.includes('CHARSET = UTF-8'), 'CHARSET is untouched');
  assert.ok(!text.includes('\uFEFF'), 'no stray BOM inside the text');

  // Without a BOM, none is invented.
  const plain = await createArtemisAssets(fakeSource(new Map([['system.ini', utf8(body)]])));
  assert.notDeepEqual(plain.config.subarray(0, 3), new Uint8Array([0xef, 0xbb, 0xbf]));
});

// ---------------------------------------------------------------------------
// the tiny bootstrap pack
// ---------------------------------------------------------------------------

/**
 * Independently parses the trailer, which `pfs.ts` deliberately stops short of.
 *
 * The trailer starts where the records end — *not* at `7 + index_size`. `index_size` covers the
 * records **and** the trailer, so `7 + index_size` is the first payload byte; the trailer's
 * position is only knowable by walking the records, which is exactly what `pfs.ts` skips.
 */
function readTrailer(pack) {
  const view = new DataView(pack.buffer, pack.byteOffset, pack.byteLength);
  const indexLength = view.getUint32(3, true);
  const declared = view.getUint32(7, true);
  const records = [];
  let cursor = 11;
  for (let index = 0; index < declared; index += 1) {
    const nameLength = view.getUint32(cursor, true);
    const name = new TextDecoder('utf-8', { fatal: true }).decode(
      pack.subarray(cursor + 4, cursor + 4 + nameLength),
    );
    const offset = view.getUint32(cursor + 4 + nameLength + 4, true);
    const length = view.getUint32(cursor + 4 + nameLength + 8, true);
    records.push({ name, offset, length, nameEnd: cursor + 4 + nameLength });
    cursor += 4 + nameLength + 12;
  }
  const end = cursor;
  const trailer = { count: view.getUint32(end, true), pairs: [], terminator: [0, 0], final: 0 };
  let at = end + 4;
  for (let index = 0; index < declared; index += 1) {
    trailer.pairs.push([view.getUint32(at, true), view.getUint32(at + 4, true)]);
    at += 8;
  }
  trailer.terminator = [view.getUint32(at, true), view.getUint32(at + 4, true)];
  trailer.final = view.getUint32(at + 8, true);
  return { indexLength, declared, records, cursor, trailer };
}

test('rootPfs honours the deployer count+1 / nameEnd / terminator / final-record formula', async () => {
  const ini = '[WASM]\r\nWIDTH = 1\r\n';
  const assets = await createArtemisAssets(fakeSource(new Map([['system.ini', utf8(ini)]])));
  const pack = assets.rootPfs;

  assert.equal(String.fromCharCode(pack[0], pack[1], pack[2]), 'pf6');
  const parsed = readTrailer(pack);
  assert.equal(parsed.declared, 1, 'one record');
  assert.equal(parsed.trailer.count, parsed.declared + 1, 'count + 1');
  // Both offsets are relative to the index blob, which starts at byte 7 of the file: the
  // deployer's `build_index` counts its `cursor` from the index origin, not from the header.
  assert.deepEqual(parsed.trailer.pairs, [[parsed.records[0].nameEnd - 7, 0]], 'nameEnd past the name, paired with 0');
  assert.deepEqual(parsed.trailer.terminator, [0, 0], 'terminating pair');
  assert.equal(parsed.trailer.final, parsed.cursor - 7, 'the offset past the last record');

  // The trailer is inside the index, so index_size covers it: 4 + (4 + len(name) + 12) + trailer.
  const trailerBytes = 4 + parsed.declared * 8 + 8 + 4;
  assert.equal(parsed.indexLength, 4 + 4 + utf8('system.ini').byteLength + 12 + trailerBytes);

  // The record's own fields, read independently of the parser above.
  const view = new DataView(pack.buffer, pack.byteOffset, pack.byteLength);
  assert.equal(utf8('system.ini').byteLength, view.getUint32(11, true));
  const offset = view.getUint32(11 + 4 + utf8('system.ini').byteLength + 4, true);
  const length = view.getUint32(11 + 4 + utf8('system.ini').byteLength + 8, true);
  assert.equal(offset, 7 + parsed.indexLength, 'the payload sits right after the index');
  assert.equal(length, assets.config.byteLength);
  assert.equal(pack.byteLength, offset + length, 'the payload is the whole tail');
  assert.deepEqual(pack.subarray(offset, offset + length), assets.config, 'the payload is the patched config');
});

test('a pack without the trailer is what pfs.ts reads, and the trailer is what it ignores', () => {
  const files = [{ name: 'system.ini', data: utf8('[WASM]\r\n') }];
  const withTrailer = packPf6(files);
  const without = packPf6(files, { trailer: false });
  assert.ok(withTrailer.byteLength > without.byteLength, 'the trailer adds bytes');
  assert.equal(u32leAt(withTrailer, 3), u32leAt(without, 3) + 4 + 8 + 8 + 4);
});

// ---------------------------------------------------------------------------
// archive indexing
// ---------------------------------------------------------------------------

/** Declared sizes, so a pack can claim to be 3.36 GB without allocating it. */
const SCALE = { 'root.pfs': 2_690_000_000, 'root.pfs.000': 3_360_000_000 };

test('a packaged game is read by range: header and index only, never the whole pack', async () => {
  const log = [];
  const pack = packPf6([{ name: 'system.ini', data: utf8(REAL_INI) }]);
  const source = fakeSource(new Map([['root.pfs', pack]]), { declared: { 'root.pfs': SCALE['root.pfs'] }, log });
  globalThis.__artemisPack = true;

  const assets = await createArtemisAssets(source);
  const text = new TextDecoder().decode(assets.config);
  assert.ok(text.includes('SAVEPATH ='), 'system.ini came out of the package');

  const whole = log.filter(([path, offset]) => offset === 0 && log.filter(([other]) => other === path).length === 1);
  assert.deepEqual(whole, [], 'no file was read in one whole-file call');
  assert.equal(source.stats.wholeReads, 0, 'readFile was never used on the pack');
  assert.ok(source.stats.ranges <= 3, `at most header + index + entry (saw ${source.stats.ranges})`);
  assert.ok(source.stats.bytes < 64 * 1024, `read ${source.stats.bytes} bytes, not ${SCALE['root.pfs']}`);
  assert.equal(source.stats.lists, 1, 'exactly one root listing');
});

test('a keyed archive (pf8) decodes through the index digest', async () => {
  const ini = '[WASM]\r\nWIDTH = 8\r\n';
  const plain = packPf6([{ name: 'system.ini', data: utf8(ini) }], { version: '8' });
  const key = keyOf(plain);
  const tableLength = u32leAt(plain, 3);
  const payloadAt = 7 + tableLength;
  const keyed = plain.slice();
  for (let index = 0; index < keyed.byteLength - payloadAt; index += 1) {
    keyed[payloadAt + index] ^= key[index % key.length];
  }
  assert.notDeepEqual(keyed.subarray(payloadAt), plain.subarray(payloadAt), 'the payload really is masked');

  const assets = await createArtemisAssets(fakeSource(new Map([['root.pfs', keyed]]), { declared: { 'root.pfs': 4096 } }));
  assert.equal(new TextDecoder().decode(assets.config), patchSystemIni(ini, {}).text);
});

test('root.pfs first, continuations next, patches last: the last archive indexed wins', async () => {
  const base = packPf6([{ name: 'system.ini', data: utf8('[WASM]\r\nWIDTH = base\r\n') }]);
  const continuation = packPf6([{ name: 'system.ini', data: utf8('[WASM]\r\nWIDTH = continuation\r\n') }]);
  const patch = packPf6([{ name: 'system.ini', data: utf8('[WASM]\r\nWIDTH = patch\r\n') }]);
  const files = new Map([
    ['z_patch.pfs', patch],
    ['root.pfs.000', continuation],
    ['root.pfs', base],
  ]);
  const declared = { 'root.pfs': 4096, 'root.pfs.000': 4096, 'z_patch.pfs': 4096 };

  const assets = await createArtemisAssets(fakeSource(files, { declared }));
  assert.ok(new TextDecoder().decode(assets.config).includes('WIDTH = patch'), 'the patch overrides the base');

  const withoutPatch = await createArtemisAssets(
    fakeSource(new Map([['root.pfs', base], ['root.pfs.000', continuation]]), { declared }),
  );
  assert.ok(new TextDecoder().decode(withoutPatch.config).includes('WIDTH = continuation'));
});

test('nested entries resolve by their full path, including Japanese and spaces', async () => {
  const nested = packPf6([
    { name: 'system.ini', data: utf8('[WASM]\r\nWIDTH = 1\r\n') },
    { name: 'image/line/title01/天梨.png', data: utf8('PNG-A') },
    { name: 'pc/ja/blog/name/あすみ.png', data: utf8('PNG-B') },
    { name: 'sound/a b.ogg', data: utf8('OGG') },
  ]);
  const assets = await createArtemisAssets(
    fakeSource(new Map([['root.pfs', nested]]), { declared: { 'root.pfs': 8192 } }),
  );

  for (const [path, expected] of [
    ['image/line/title01/天梨.png', 'PNG-A'],
    ['image\\line\\title01\\天梨.png', 'PNG-A'],
    ['pc/ja/blog/name/あすみ.png', 'PNG-B'],
    ['sound/a b.ogg', 'OGG'],
  ]) {
    const bytes = await assets.resolve(path);
    assert.equal(new TextDecoder().decode(bytes), expected, `${path} resolves`);
  }
});

test('the missing case returns null, and a backend failure propagates', async () => {
  const pack = packPf6([{ name: 'system.ini', data: utf8(REAL_INI) }]);
  const source = fakeSource(new Map([['root.pfs', pack]]), { declared: { 'root.pfs': 4096 } });
  const assets = await createArtemisAssets(source);
  assert.equal(await assets.resolve('no/such/file.png'), null, 'absent entries are null, not an exception');

  // A backend that fails while the *index* is being read rejects construction: the failure
  // happens inside `createArtemisAssets`, before any seam exists to resolve against.
  const unreadable = fakeSource(new Map([['root.pfs', pack]]), { declared: { 'root.pfs': 4096 } });
  unreadable.readRange = async () => {
    throw new Error('EIO: the disk went away');
  };
  await assert.rejects(createArtemisAssets(unreadable), /EIO: the disk went away/, 'construction rejects');

  // A backend that fails while a *payload* is being read is a different path — the index came
  // back fine, so construction succeeds and only the resolve that needs the bytes fails.
  const healthy = fakeSource(new Map([['root.pfs', pack]]), { declared: { 'root.pfs': 4096 } });
  const target = await createArtemisAssets(healthy);
  const indexReads = healthy.stats.ranges;
  assert.ok(indexReads > 0, 'the index was read during construction');
  healthy.readRange = async () => {
    throw new Error('EIO: the disk went away');
  };
  await assert.rejects(target.resolve('system.ini'), /EIO: the disk went away/, 'the payload read rejects');
  assert.equal(healthy.stats.ranges, indexReads, 'the failure came from the payload read, not a re-index');
});

test('a game with no system.ini anywhere is a typed error, not a null resolve', async () => {
  const empty = packPf6([{ name: 'system/first.iet', data: utf8('script') }]);
  const source = fakeSource(new Map([['root.pfs', empty]]), { declared: { 'root.pfs': 4096 } });
  await assert.rejects(createArtemisAssets(source), (error) => {
    assert.ok(error instanceof MissingSystemIniError, `expected MissingSystemIniError, saw ${error?.name}`);
    assert.match(error.message, /no system\.ini in fake/);
    return true;
  });

  await assert.rejects(createArtemisAssets(fakeSource(new Map())), MissingSystemIniError);
});

// ---------------------------------------------------------------------------
// request validation
// ---------------------------------------------------------------------------

test('hostile and cross-origin requests are refused before a byte is read', async () => {
  const pack = packPf6([{ name: 'system.ini', data: utf8(REAL_INI) }]);
  const source = fakeSource(new Map([['root.pfs', pack]]), { declared: { 'root.pfs': 4096 } });
  const assets = await createArtemisAssets(source, { baseUrl: 'https://assets.example/game/' });

  const refusals = [
    ['../secret.txt', /escapes the asset root/],
    ['pc/../../etc/passwd', /escapes the asset root/],
    ['/etc/passwd', /absolute path/],
    ['C:/windows/system32/config', /names a drive/],
    ['pc/ja/%2e%2e/secret.png', /escapes the asset root/],
    ['bad%zzname.png', /malformed percent-escape/],
    ['a%2', /malformed percent-escape/],
    ['nul\u0000byte.png', /NUL/],
    ['https://evil.example/game/x.png', /not the configured origin/],
    ['//evil.example/game/x.png', /protocol-relative/],
    ['', /empty path/],
  ];
  for (const [bad, expected] of refusals) {
    await assert.rejects(assets.resolve(bad), expected, `refused: ${JSON.stringify(bad)}`);
  }
  await assert.rejects(assets.resolve('pc/ja/x.png', 'PUT'), /not a supported method/);
  await assert.rejects(assets.resolve('pc/ja/x.png', 'POST'), /not a supported method/);
  assert.equal(source.stats.ranges, 1 + 2, 'only the index read happened while refusing');

  // The same path against the configured origin is accepted, and resolves as a game path.
  assert.equal(await assets.resolve('https://assets.example/game/no/such.png'), null);
});

test('a relative virtual prefix accepts its own requests and rejects everything else', async () => {
  const pack = packPf6([
    { name: 'system.ini', data: utf8(REAL_INI) },
    { name: 'bg/black.png', data: utf8('BG') },
  ]);
  const assets = await createArtemisAssets(
    fakeSource(new Map([['root.pfs', pack]]), { declared: { 'root.pfs': 4096 } }),
    { baseUrl: 'artemis-assets/' },
  );
  assert.equal(new TextDecoder().decode(await assets.resolve('artemis-assets/bg/black.png')), 'BG');
  assert.equal(new TextDecoder().decode(await assets.resolve('bg/black.png')), 'BG');
  assert.equal(new TextDecoder().decode(await assets.resolve('artemis-assets/bg/black%2Epng')), 'BG');
  assert.equal(await assets.resolve('artemis-assets/bg/missing.png'), null);
});

test('a trailing slash is added to ASYNCIFY_URL and survives into the config', async () => {
  const assets = await createArtemisAssets(fakeSource(new Map([['system.ini', utf8(REAL_INI)]])), {
    baseUrl: 'https://cdn.example/game',
  });
  const text = new TextDecoder().decode(assets.config);
  assert.ok(text.includes('ASYNCIFY_URL = https://cdn.example/game/'), text);
});

// ---------------------------------------------------------------------------
// cache: single-flight, LRU budget, cloning, disposal
// ---------------------------------------------------------------------------

test('concurrent requests for one path read once and get independent buffers', async () => {
  const pack = packPf6([
    { name: 'system.ini', data: utf8(REAL_INI) },
    { name: 'fg/chara.png', data: new Uint8Array(4096).fill(7) },
  ]);
  const source = fakeSource(new Map([['root.pfs', pack]]), { declared: { 'root.pfs': 65536 } });
  const assets = await createArtemisAssets(source);
  const before = source.stats.ranges;

  const [first, second, third] = await Promise.all([
    assets.resolve('fg/chara.png'),
    assets.resolve('fg/chara.png'),
    assets.resolve('FG/CHARA.PNG'),
  ]);
  assert.equal(source.stats.ranges - before, 1, 'one payload read served three callers');
  assert.deepEqual(first, second);
  assert.deepEqual(first, third);

  // Mutating what a caller was handed must not change what the next caller sees.
  first.fill(0);
  const fourth = await assets.resolve('fg/chara.png');
  assert.equal(fourth[0], 7, 'the cache answered with its own bytes');
  assert.notEqual(first.buffer, fourth.buffer, 'each caller owns its buffer');
});

test('the byte budget evicts the oldest entry, and an oversized entry is never cached', async () => {
  const small = new Uint8Array(600).fill(1);
  const big = new Uint8Array(900).fill(2);
  const pack = packPf6([
    { name: 'system.ini', data: utf8(REAL_INI) },
    { name: 'a.bin', data: small },
    { name: 'b.bin', data: big },
  ]);
  const source = fakeSource(new Map([['root.pfs', pack]]), { declared: { 'root.pfs': 65536 } });
  const assets = await createArtemisAssets(source, { cacheBytes: 1024 });

  await assets.resolve('a.bin');
  const afterFirst = source.stats.ranges;
  await assets.resolve('a.bin');
  assert.equal(source.stats.ranges, afterFirst, 'the cached entry did not need a second read');

  await assets.resolve('b.bin'); // 900 + 600 > 1024, so a.bin is evicted
  await assets.resolve('a.bin');
  assert.equal(source.stats.ranges, afterFirst + 2, 'the evicted entry was read again');

  await assets.resolve('b.bin');
  const withBig = source.stats.ranges;
  await assets.resolve('b.bin');
  assert.equal(source.stats.ranges, withBig, 'the 900-byte entry fits the budget and stays');
});

test('cacheBytes 0 disables the cache and every resolve reads again', async () => {
  const pack = packPf6([
    { name: 'system.ini', data: utf8(REAL_INI) },
    { name: 'a.bin', data: new Uint8Array(64).fill(1) },
  ]);
  const source = fakeSource(new Map([['root.pfs', pack]]), { declared: { 'root.pfs': 4096 } });
  const assets = await createArtemisAssets(source, { cacheBytes: 0 });
  const before = source.stats.ranges;
  await assets.resolve('a.bin');
  await assets.resolve('a.bin');
  assert.equal(source.stats.ranges, before + 2, 'nothing was cached');
});

test('an entry larger than the budget is served but never stored', async () => {
  const pack = packPf6([
    { name: 'system.ini', data: utf8(REAL_INI) },
    { name: 'huge.bin', data: new Uint8Array(4096).fill(3) },
  ]);
  const source = fakeSource(new Map([['root.pfs', pack]]), { declared: { 'root.pfs': 65536 } });
  const assets = await createArtemisAssets(source, { cacheBytes: 1024 });
  assert.equal((await assets.resolve('huge.bin')).byteLength, 4096);
  const before = source.stats.ranges;
  assert.equal((await assets.resolve('huge.bin')).byteLength, 4096);
  assert.equal(source.stats.ranges, before + 1, 'oversized entries are not cached');
});

test('dispose drops the cache, refuses new reads, and leaves held answers usable', async () => {
  const pack = packPf6([
    { name: 'system.ini', data: utf8(REAL_INI) },
    { name: 'a.bin', data: new Uint8Array(32).fill(9) },
  ]);
  const source = fakeSource(new Map([['root.pfs', pack]]), { declared: { 'root.pfs': 4096 } });
  const assets = await createArtemisAssets(source);

  const held = await assets.resolve('a.bin');
  const before = source.stats.ranges;
  assets.dispose();
  assets.dispose(); // idempotent

  await assert.rejects(assets.resolve('a.bin'), /disposed/);
  assert.equal(source.stats.ranges, before, 'a disposed seam reads nothing');
  assert.equal(held[0], 9, 'the caller keeps what it already had');
  assert.equal(new TextDecoder().decode(assets.config).includes('ASYNCIFY_URL'), true, 'config survives disposal');
});

// ---------------------------------------------------------------------------
// the lazy index and the paths that must never be walked
// ---------------------------------------------------------------------------

test('the 29,224-file loose tree is never enumerated recursively', async () => {
  const pack = packPf6([{ name: 'system.ini', data: utf8(REAL_INI) }]);
  const source = fakeSource(new Map([['root.pfs', pack]]), { declared: { 'root.pfs': 4096 }, looseCount: 29224 });
  const assets = await createArtemisAssets(source);
  assert.equal(source.stats.lists, 1, 'one listing, at the root');
  assert.equal(looseCountOf(source), 29224, 'the tree really is there to be walked');

  await assets.resolve('bg/black.png');
  assert.equal(source.stats.lists, 1, 'resolving a miss does not re-list or recurse');
});

function looseCountOf() {
  return 29224;
}

test('the archive index is built lazily and once, even though every resolve needs it', async () => {
  const readLog = [];
  const pack = packPf6([
    { name: 'system.ini', data: utf8(REAL_INI) },
    { name: 'a.bin', data: new Uint8Array(8).fill(1) },
    { name: 'b.bin', data: new Uint8Array(8).fill(2) },
  ]);
  const source = fakeSource(new Map([['root.pfs', pack]]), { declared: { 'root.pfs': 4096 }, log: readLog });
  const assets = await createArtemisAssets(source, { cacheBytes: 0 });
  const afterConstruction = source.stats.ranges;
  const afterConstructionLists = source.stats.lists;

  assert.deepEqual(await assets.resolve('a.bin'), new Uint8Array(8).fill(1));
  assert.deepEqual(await assets.resolve('b.bin'), new Uint8Array(8).fill(2));
  assert.equal(await assets.resolve('missing.bin'), null);

  // The index came back with the configuration, so none of the three resolves re-reads it.
  // `cacheBytes: 0` disables the payload cache on purpose, which is what makes the payload
  // reads visible: exactly two, at the offsets the index declared, and nothing else.
  assert.equal(source.stats.lists, afterConstructionLists, 'no resolve re-listed the root');
  const payloadReads = ['a.bin', 'b.bin'];
  assert.equal(source.stats.ranges - afterConstruction, 2, 'one payload read per hit, no re-index');

  const entryAt = (name) => {
    const tableLength = u32leAt(pack, 3);
    const index = pack.subarray(7, 7 + tableLength);
    const view = new DataView(index.buffer, index.byteOffset, index.byteLength);
    let cursor = 4;
    const count = view.getUint32(0, true);
    for (let record = 0; record < count; record += 1) {
      const nameLength = view.getUint32(cursor, true);
      cursor += 4;
      const recordName = new TextDecoder().decode(index.subarray(cursor, cursor + nameLength)).replace(/\\/g, '/');
      cursor += nameLength;
      cursor += 4; // unknown trailer field the reader skips
      const offset = view.getUint32(cursor, true);
      const length = view.getUint32(cursor + 4, true);
      cursor += 8;
      if (recordName === name) return [offset, length];
    }
    throw new Error(`no ${name} in the index`);
  };

  const expected = payloadReads.map((name) => ['root.pfs', ...entryAt(name)]);
  assert.deepEqual(
    readLog.slice(-2),
    expected,
    'the two payload reads ask for exactly the declared offsets and lengths',
  );
  assert.equal(readLog.length, source.stats.ranges, 'the log and the range counter agree');
});

test('a game with a loose system.ini is never indexed at all', async () => {
  const source = fakeSource(
    new Map([
      ['system.ini', utf8(REAL_INI)],
      ['root.pfs', packPf6([{ name: 'system.ini', data: utf8(REAL_INI) }])],
    ]),
    { declared: { 'root.pfs': 4096 } },
  );
  const assets = await createArtemisAssets(source);
  assert.equal(source.stats.lists, 0, 'the loose file settled it: no listing, no archive reads');
  assert.equal(source.stats.wholeReads, 1, 'one read, of system.ini itself');
  assert.equal(await assets.resolve('bg/black.png'), null);
  assert.equal(source.stats.lists, 1, 'the index is built on the first miss, not at construction');
});

test('consolidation does not invent a blank line where the file had none', () => {
  const patched = patchSystemIni('[WASM]\r\nA = 1\r\n[wasm]\r\nB = 2\r\n[FILE]\r\nC = 3\r\n', { savePath: 's' });
  assert.equal(
    patched.text,
    '[WASM]\r\nSAVEPATH = s\r\nASYNCIFY_URL = artemis-assets/\r\nA = 1\r\nB = 2\r\n[FILE]\r\nC = 3\r\n',
    'a tight file stays tight: only a separator the file already had is re-emitted',
  );
  assert.ok(!patched.text.includes('\n\n'), 'no blank line was introduced at either seam');
});

test('two games with different native [FILE] SAVEPATH values derive different identities', async () => {
  const nativeIni = (save) =>
    ['[FILE]', `SAVEPATH = ${save}`, 'CHARSET = SHIFT-JIS', '', '[WASM]', 'WIDTH = 1', ''].join('\r\n');
  const one = await createArtemisAssets(fakeSource(new Map([['system.ini', utf8(nativeIni('game-one'))]])));
  const two = await createArtemisAssets(fakeSource(new Map([['system.ini', utf8(nativeIni('game-two'))]])));
  // A build with no [WASM] SAVEPATH at all still derives from the [FILE] key — the native
  // spelling — and an ASCII name is kept rather than hashed, so the mount directory is stable.
  assert.equal(one.savePath, 'gameone', 'the native [FILE] key is what the identity comes from');
  assert.notEqual(one.savePath, two.savePath, 'the native mount directory separates the two titles');
  const again = await createArtemisAssets(fakeSource(new Map([['system.ini', utf8(nativeIni('game-one'))]])));
  assert.equal(one.savePath, again.savePath, 'the same native tree derives the same identity');

  // A non-ASCII native name has no ASCII form, so it is hashed — and two different names still
  // land in two different slots.
  const hashed = await createArtemisAssets(
    fakeSource(new Map([['system.ini', utf8(nativeIni('まどそふと\\ハミダシクリエイティブ'))]])),
  );
  assert.match(hashed.savePath, /^artemis_[0-9a-f]{10}$/, 'a non-ASCII native name is hashed');
  const other = await createArtemisAssets(
    fakeSource(new Map([['system.ini', utf8(nativeIni('まどそふと\\別タイトル'))]])),
  );
  assert.notEqual(hashed.savePath, other.savePath, 'two Japanese titles do not collide');
});

test('a loose payload resolves with no listing and no archive read', async () => {
  const source = fakeSource(
    new Map([
      ['system.ini', utf8(REAL_INI)],
      ['bg/black.png', utf8('BG')],
    ]),
  );
  const assets = await createArtemisAssets(source);
  assert.equal(source.stats.lists, 0, 'construction listed nothing');
  assert.equal(new TextDecoder().decode(await assets.resolve('bg/black.png')), 'BG');
  assert.equal(source.stats.lists, 0, 'a loose hit needs no index at all');
  assert.equal(source.stats.ranges, 0, 'no archive was read by range');
  assert.equal(source.stats.wholeReads, 2, 'system.ini and the payload, both loose reads');
});

test('a disposal during an in-flight payload read rejects that resolve', async () => {
  const pack = packPf6([
    { name: 'system.ini', data: utf8(REAL_INI) },
    { name: 'a.bin', data: new Uint8Array(16).fill(4) },
  ]);
  const source = fakeSource(new Map([['root.pfs', pack]]), { declared: { 'root.pfs': 4096 } });
  const assets = await createArtemisAssets(source);

  let release, entered;
  const gate = new Promise((resolve) => { release = resolve; });
  const arrival = new Promise((resolve) => { entered = resolve; });
  const inner = source.readRange;
  let reads = 0;
  source.readRange = async (...args) => {
    reads++;
    entered();
    await gate;
    return inner(...args);
  };

  const pending = assets.resolve('a.bin');
  await arrival;
  assert.equal(reads, 1, 'the payload read actually entered the backend before disposal');
  assets.dispose();
  release();
  await assert.rejects(pending, /disposed/, 'a disposed seam does not hand back the bytes it was reading');
  await assert.rejects(assets.resolve('a.bin'), /disposed/, 'no new read is allowed after disposal');
  assert.equal(reads, 1, 'late completion did not repopulate or reopen the backend');
});

test('a source without random access is refused instead of reading a package whole', async () => {
  const pack = packPf6([{ name: 'system.ini', data: utf8(REAL_INI) }]);
  const source = fakeSource(new Map([['root.pfs', pack]]), { declared: { 'root.pfs': 4096 } });
  source.capabilities = { list: true, randomAccess: false, writable: false };
  delete source.readRange;

  await assert.rejects(
    createArtemisAssets(source),
    /capabilities\.randomAccess and a readRange/,
    'the package is refused before a byte is requested',
  );
  assert.equal(source.stats.wholeReads, 0, 'no whole-file read was attempted');
  assert.equal(source.stats.ranges, 0, 'no ranged read was attempted either');
});
