import { createHash } from 'node:crypto';
import { gunzipSync, gzipSync } from 'node:zlib';
import { mkdir, readFile, writeFile } from 'node:fs/promises';
import { dirname, join, resolve } from 'node:path';
import { pathToFileURL } from 'node:url';

import { BUNDLE_FILE, MANIFEST_FILE, PACKAGE_FILE, PACKAGE_LIMITS, parseEnvelope, parsePackage, safeRelativePath } from '../dist/index.js';
import { fileListHash, packDirectory } from './pack-plugin.mjs';

export const ARTIFACT_FORMAT = 1;
export const DEFAULT_LAZY = ['engines/**', '_astro/cf/**', '**/*.map'];
export const MANAGER_ROOT_RESERVED = new Set([
  'registry', '__kernel', '__plugins', '__ca', '__cf', '__webdav', 'sw.js', 'sw.js.map',
  '_headers', '_routes.json', '_redirects', '_worker.js', '.well-known',
  PACKAGE_FILE, MANIFEST_FILE, BUNDLE_FILE,
]);
const ARCHIVE_LIMIT = PACKAGE_LIMITS.totalBytes + 16 * 1024 * 1024;
const textEncoder = new TextEncoder();
const sha256 = (bytes) => createHash('sha256').update(bytes).digest('hex');
const asBuffer = (bytes) => Buffer.isBuffer(bytes) ? bytes : Buffer.from(bytes);
const fail = (message) => { throw new Error(`manager artifact is invalid: ${message}`); };

function safePath(path) {
  if (safeRelativePath(path) === null || path.split('/').some((part) => /[. ]$/.test(part) || /^(?:con|prn|aux|nul|com[1-9]|lpt[1-9])(?:\.|$)/i.test(part))) {
    fail(`unsafe archive path ${JSON.stringify(path)}`);
  }
  return path;
}
function octal(value, length) {
  const encoded = value.toString(8).padStart(length - 1, '0');
  if (encoded.length > length - 1) fail(`archive field is too large: ${value}`);
  return `${encoded}\0`;
}
function putString(target, offset, length, value) {
  const bytes = textEncoder.encode(value);
  if (bytes.length > length) fail(`archive field is too long: ${value}`);
  target.set(bytes, offset);
}
function tarHeader(path, size) {
  const header = Buffer.alloc(512);
  const slash = textEncoder.encode(path).length > 100 ? path.lastIndexOf('/') : -1;
  const name = slash > 0 ? path.slice(slash + 1) : path;
  const prefix = slash > 0 ? path.slice(0, slash) : '';
  if (textEncoder.encode(name).length > 100 || textEncoder.encode(prefix).length > 155) fail(`archive path exceeds ustar limits: ${path}`);
  putString(header, 0, 100, name);
  putString(header, 100, 8, octal(0o644, 8));
  putString(header, 124, 12, octal(size, 12));
  header.fill(0x20, 148, 156);
  header[156] = 0x30;
  putString(header, 257, 6, 'ustar\0');
  putString(header, 263, 2, '00');
  putString(header, 345, 155, prefix);
  const checksum = header.reduce((sum, byte) => sum + byte, 0);
  putString(header, 148, 8, `${checksum.toString(8).padStart(6, '0')}\0 `);
  return header;
}
export function createArchive(entries) {
  const chunks = [];
  for (const [path, raw] of [...entries].sort(([a], [b]) => a < b ? -1 : a > b ? 1 : 0)) {
    safePath(path);
    const bytes = asBuffer(raw);
    chunks.push(tarHeader(path, bytes.length), bytes);
    const padding = (512 - (bytes.length % 512)) % 512;
    if (padding) chunks.push(Buffer.alloc(padding));
  }
  chunks.push(Buffer.alloc(1024));
  return gzipSync(Buffer.concat(chunks), { level: 9, mtime: 0 });
}
function field(bytes, offset, length) {
  return Buffer.from(bytes.subarray(offset, offset + length)).toString('utf8').split('\0', 1)[0];
}
function octalField(bytes, offset, length) {
  const value = field(bytes, offset, length).trim();
  if (!/^[0-7]*$/.test(value)) fail(`invalid octal field ${JSON.stringify(value)}`);
  return value ? Number.parseInt(value, 8) : 0;
}
export function readArchive(input) {
  let bytes;
  try { bytes = gunzipSync(asBuffer(input), { maxOutputLength: ARCHIVE_LIMIT }); } catch { fail('archive is not valid gzip or exceeds the size limit'); }
  const entries = new Map();
  for (let offset = 0; offset + 512 <= bytes.length;) {
    const header = bytes.subarray(offset, offset + 512);
    if (header.every((byte) => byte === 0)) {
      if (bytes.length - offset < 1024 || bytes.subarray(offset).some((byte) => byte !== 0)) fail('invalid archive terminator');
      if (!entries.size) fail('archive is empty');
      return entries;
    }
    const checksum = header.reduce((sum, byte, index) => sum + (index >= 148 && index < 156 ? 32 : byte), 0);
    if (field(header, 257, 6) !== 'ustar' || octalField(header, 148, 8) !== checksum) fail('invalid ustar header');
    if (entries.size >= PACKAGE_LIMITS.files + 2) fail('archive contains too many entries');
    const name = field(header, 0, 100);
    const prefix = field(header, 345, 155);
    const path = safePath(prefix ? `${prefix}/${name}` : name);
    if (header[156] !== 0x30) fail(`archive entry ${path} is not a regular file`);
    const size = octalField(header, 124, 12);
    const start = offset + 512;
    const end = start + size;
    if (end > bytes.length) fail(`archive entry ${path} is truncated`);
    if (entries.has(path)) fail(`archive entry ${path} is duplicated`);
    entries.set(path, bytes.subarray(start, end));
    offset = start + Math.ceil(size / 512) * 512;
  }
  fail('archive has no complete terminator');
}
function parseArtifact(input) {
  const entries = readArchive(input);
  const envelopeBytes = entries.get(PACKAGE_FILE);
  const manifestBytes = entries.get(MANIFEST_FILE);
  if (!envelopeBytes || !manifestBytes) fail(`archive must contain ${PACKAGE_FILE} and ${MANIFEST_FILE}`);
  let envelope;
  let pkg;
  try {
    envelope = parseEnvelope(JSON.parse(envelopeBytes.toString('utf8')));
    pkg = parsePackage(JSON.parse(manifestBytes.toString('utf8')));
  } catch (error) { fail(error instanceof Error ? error.message : String(error)); }
  if (envelope.manifest !== MANIFEST_FILE || envelope.bundle !== undefined) fail('release archive must point at mf-manifest.json and contain no signing bundle');
  if (pkg.manifest.id !== 'online-gal.manager' || pkg.manifest.role !== 'manager' || pkg.manifest.provenance !== 'official') fail('package is not the official online-gal.manager');
  const expected = new Set([PACKAGE_FILE, MANIFEST_FILE]);
  const files = new Set();
  const directories = new Set();
  for (const file of pkg.files) {
    const parts = file.path.toLowerCase().split('/');
    if (MANAGER_ROOT_RESERVED.has(parts[0])) fail(`manager file uses a reserved site path: ${file.path}`);
    const key = parts.join('/');
    if (files.has(key) || directories.has(key)) fail(`archive path conflicts with another path: ${file.path}`);
    for (let i = 1; i < parts.length; i++) {
      const prefix = parts.slice(0, i).join('/');
      if (files.has(prefix)) fail(`archive path conflicts with a file: ${file.path}`);
      directories.add(prefix);
    }
    files.add(key);
    expected.add(file.path);
    const bytes = entries.get(file.path);
    if (!bytes) fail(`manifest file is missing from archive: ${file.path}`);
    if (bytes.length !== file.size || sha256(bytes) !== file.sha256) fail(`manifest file does not match archive: ${file.path}`);
  }
  for (const path of entries.keys()) if (!expected.has(path)) fail(`archive contains an unlisted file: ${path}`);
  if (!pkg.files.some((file) => file.path === 'index.html' && file.size > 0 && !file.lazy)) fail('package has no eager index.html entry');
  return { entries, envelope, manifestBytes, package: pkg, archiveSha256: sha256(asBuffer(input)), manifestSha256: sha256(manifestBytes) };
}
export const inspectManagerArtifact = parseArtifact;
async function engineInputs(repoRoot) {
  const result = [];
  for (const path of ['pnpm-lock.yaml', 'ports/lib/engine-artifacts.json', 'packages/cf-browser/upstream.json']) {
    result.push({ path, sha256: sha256(await readFile(join(repoRoot, path))) });
  }
  return result;
}
export async function buildManagerArtifact({ appDist, manifestPath, packageJsonPath, out, sourceSha, releaseTag, repoRoot }) {
  if (!/^[0-9a-f]{40,64}$/.test(sourceSha)) throw new Error('--source-sha must be a full hexadecimal commit SHA');
  if (typeof releaseTag !== 'string' || !releaseTag || /[\r\n]/.test(releaseTag)) throw new Error('--release-tag is required');
  const base = JSON.parse(await readFile(manifestPath, 'utf8'));
  const version = JSON.parse(await readFile(packageJsonPath, 'utf8')).version;
  const probe = await packDirectory({ dir: appDist, manifest: { ...base, version }, lazy: DEFAULT_LAZY });
  const manifest = { ...base, version: `${version}+${fileListHash(probe.files).slice(0, 12)}` };
  const packed = await packDirectory({ dir: appDist, manifest, lazy: DEFAULT_LAZY });
  const entries = new Map([[PACKAGE_FILE, packed.envelope], [MANIFEST_FILE, packed.bytes]]);
  for (const file of packed.files) entries.set(file.path, await readFile(file.abs));
  const archive = createArchive(entries);
  const inspected = parseArtifact(archive);
  const asset = `online-gal.manager-${manifest.version.replace('+', '_')}.tar.gz`;
  const record = {
    format: ARTIFACT_FORMAT,
    id: manifest.id,
    version: manifest.version,
    releaseTag,
    asset,
    sourceSha,
    archiveSha256: inspected.archiveSha256,
    manifestSha256: inspected.manifestSha256,
    fileCount: packed.files.length,
    lazyFiles: packed.files.filter((file) => file.lazy).map((file) => file.path),
    engineFiles: packed.files.filter((file) => file.path.startsWith('engines/')).map(({ path, sha256 }) => ({ path, sha256 })),
    inputs: await engineInputs(repoRoot),
  };
  await mkdir(out, { recursive: true });
  await writeFile(join(out, asset), archive);
  await writeFile(join(out, `${asset}.json`), `${JSON.stringify(record, null, 2)}\n`);
  return { ...record, archive, inspected };
}
export async function fetchManagerArtifact({ archivePath, archiveSha256, lockPath, token, fetcher = globalThis.fetch }) {
  if (archivePath !== undefined) {
    if (!/^[a-f0-9]{64}$/.test(archiveSha256 ?? '')) throw new Error('local manager archive requires an explicit SHA-256');
    const archive = await readFile(resolve(archivePath));
    if (sha256(archive) !== archiveSha256) throw new Error('local manager archive does not match its locked SHA-256');
    return archive;
  }

  const lock = JSON.parse(await readFile(lockPath, 'utf8'));
  if (!lock || lock.format !== ARTIFACT_FORMAT || !/^[\w.-]+\/[\w.-]+$/.test(lock.repository ?? '') ||
      !/^manager-v[\w.-]+$/.test(lock.releaseTag ?? '') || !/^online-gal\.manager-[\w.-]+\.tar\.gz$/.test(lock.asset ?? '') ||
      !/^[a-f0-9]{64}$/.test(lock.archiveSha256 ?? '') || !/^[a-f0-9]{40}$/.test(lock.sourceSha ?? '')) {
    throw new Error(`manager artifact lock is invalid: ${lockPath}`);
  }
  const api = `https://api.github.com/repos/${lock.repository}`;
  const headers = {
    accept: 'application/vnd.github+json',
    'x-github-api-version': '2022-11-28',
    ...(token ? { authorization: `Bearer ${token}` } : {}),
  };
  try {
    const request = (url, options) => fetcher(url, { signal: AbortSignal.timeout(120_000), ...options });
    const releaseResponse = await request(`${api}/releases/tags/${encodeURIComponent(lock.releaseTag)}`, { headers, redirect: 'error' });
    if (!releaseResponse.ok) throw new Error(`release ${lock.releaseTag} returned HTTP ${releaseResponse.status}`);
    const release = await releaseResponse.json();
    const asset = release.assets?.find((candidate) => candidate?.name === lock.asset);
    if (release.draft || release.tag_name !== lock.releaseTag || !Number.isSafeInteger(asset?.id) || asset.id <= 0) {
      throw new Error(`published release ${lock.releaseTag} has no ${lock.asset} asset`);
    }
    const download = await request(`${api}/releases/assets/${asset.id}`, {
      headers: { ...headers, accept: 'application/octet-stream' }, redirect: 'manual',
    });
    let response = download;
    if ([301, 302, 303, 307, 308].includes(download.status)) {
      const location = new URL(download.headers.get('location'));
      if (location.protocol !== 'https:' || location.username || location.password || location.port ||
          !['release-assets.githubusercontent.com', 'objects.githubusercontent.com'].includes(location.hostname)) {
        throw new Error('GitHub asset redirect is outside the allowed download hosts');
      }
      response = await request(location, { redirect: 'error' });
    }
    if (!response.ok) throw new Error(`asset ${lock.asset} returned HTTP ${response.status}`);
    const bytes = asBuffer(await response.arrayBuffer());
    if (sha256(bytes) !== lock.archiveSha256) throw new Error(`asset ${lock.asset} does not match its locked SHA-256`);
    return bytes;
  } catch (error) {
    // Fetch exceptions can include signed download URLs in their causes.
    const message = error instanceof TypeError || error instanceof SyntaxError || error instanceof DOMException
      ? 'invalid response, timeout, or network error' : error.message;
    throw new Error(`manager artifact download failed: ${message}`);
  }
}


function parseArgs(argv) {
  const args = {};
  for (let i = 0; i < argv.length; i++) {
    const arg = argv[i];
    if (!arg.startsWith('--')) throw new Error(`unexpected argument ${arg}`);
    args[arg.slice(2)] = argv[++i];
  }
  return args;
}

async function main(argv) {
  const args = parseArgs(argv);
  if (args.verify !== undefined) {
    const item = parseArtifact(await readFile(resolve(args.verify)));
    console.log(JSON.stringify({ archiveSha256: item.archiveSha256, manifestSha256: item.manifestSha256, id: item.package.manifest.id, version: item.package.manifest.version, files: item.package.files.length }));
    return;
  }
  for (const key of ['app-dist', 'manifest', 'package-json', 'out', 'source-sha', 'release-tag']) {
    if (args[key] === undefined) throw new Error(`missing --${key}`);
  }
  const result = await buildManagerArtifact({
    appDist: resolve(args['app-dist']),
    manifestPath: resolve(args.manifest),
    packageJsonPath: resolve(args['package-json']),
    out: resolve(args.out),
    sourceSha: args['source-sha'],
    releaseTag: args['release-tag'],
    repoRoot: resolve(args['repo-root'] ?? process.cwd()),
  });
  console.log(JSON.stringify({
    asset: result.asset,
    releaseTag: result.releaseTag,
    sourceSha: result.sourceSha,
    archiveSha256: result.archiveSha256,
    manifestSha256: result.manifestSha256,
    version: result.version,
    fileCount: result.fileCount,
  }));
}

if (import.meta.url === pathToFileURL(process.argv[1] ?? '').href) {
  main(process.argv.slice(2)).catch((error) => {
    console.error(error instanceof Error ? error.message : error);
    process.exit(1);
  });
}
