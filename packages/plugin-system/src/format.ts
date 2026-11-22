import { assertApiCompatible, parseManifest, type PluginManifest } from '@online-gal/plugin-host';

import { KernelError } from './errors.js';

/**
 * The on-the-wire shape of a plugin: a `plugin.json` next to the plugin's
 * files, wherever it is published (the official registry on Cloudflare Pages,
 * or a tag of a GitHub repository).
 *
 * ```json
 * { "format": 1,
 *   "manifest": { "id": "online-gal.manager", "version": "0.1.0+3fa2c1d0", ... },
 *   "files": [ { "path": "index.html", "size": 1234, "sha256": "<64 hex>" } ] }
 * ```
 *
 * Every file is listed with its size and hash. The kernel refuses anything it
 * cannot verify, so a half-uploaded release or a tampered mirror never gets
 * served as if it were the plugin.
 *
 * Format 2 (preferred) splits this into a slim `plugin.json` that only points
 * at `mf-manifest.json` (and optionally `plugin.bundle.json`), so the pointer
 * and the content can change independently. Format 1 (inline, GitHub only)
 * keeps everything in `plugin.json` with no attestation.
 */
export const PACKAGE_FORMAT = 1;
export const PACKAGE_FILE = 'plugin.json';
export const ENVELOPE_FORMAT = 2;
export const MANIFEST_FILE = 'mf-manifest.json';
export const BUNDLE_FILE = 'plugin.bundle.json';

export const PACKAGE_LIMITS = Object.freeze({
  files: 5000,
  fileBytes: 256 * 1024 * 1024,
  totalBytes: 1024 * 1024 * 1024,
  pathLength: 512,
});

export interface PackageFile {
  /** Relative path, `/` separated, e.g. `_astro/client.1a2b.js`. */
  readonly path: string;
  readonly size: number;
  /** Lower-case hex SHA-256 of the file bytes. */
  readonly sha256: string;
  /**
   * Not downloaded at install time: the kernel fetches (and verifies) the file
   * the first time something asks for it. Absent means eager. Old kernels ignore
   * the flag and download everything, so the format version stays 1.
   */
  readonly lazy?: boolean;
}

/**
 * The content manifest body: the manifest plus the verified file list. In
 * format 2 this is `mf-manifest.json`; in format 1 it is `plugin.json` itself.
 */
export interface PluginPackage {
  readonly format: typeof PACKAGE_FORMAT;
  readonly manifest: PluginManifest;
  readonly files: readonly PackageFile[];
}

const SHA256 = /^[0-9a-f]{64}$/;
/** Characters that change URL meaning or are unsafe in a cache key. */
const BAD_CHAR = /[\u0000-\u001f\u007f\\?#%:*"<>|]/;

function bad(message: string): KernelError {
  return new KernelError('package-invalid', message);
}

function isRecord(value: unknown): value is Record<string, unknown> {
  return typeof value === 'object' && value !== null && !Array.isArray(value);
}

/** Returns the path unchanged when it is a safe relative file path, otherwise `null`. */
export function safeRelativePath(path: string): string | null {
  if (path.length === 0 || path.length > PACKAGE_LIMITS.pathLength) return null;
  if (BAD_CHAR.test(path)) return null;
  for (const segment of path.split('/')) {
    if (segment === '' || segment === '.' || segment === '..') return null;
  }
  return path;
}

/** Validates an untrusted content-manifest body (`mf-manifest.json` in format 2, `plugin.json` in format 1). */
export function parsePackage(input: unknown): PluginPackage {
  if (!isRecord(input)) throw bad(`${PACKAGE_FILE} must be an object`);
  const unknown = Object.keys(input).filter((key) => !['format', 'manifest', 'files'].includes(key));
  if (unknown.length > 0) throw bad(`${PACKAGE_FILE} has unknown field(s): ${unknown.join(', ')}`);
  if (input.format !== PACKAGE_FORMAT) {
    throw bad(`${PACKAGE_FILE} format ${String(input.format)} is not supported (expected ${PACKAGE_FORMAT})`);
  }

  const manifest = parseManifest(input.manifest);
  assertApiCompatible(manifest);

  if (!Array.isArray(input.files) || input.files.length === 0) {
    throw bad(`${PACKAGE_FILE} must list at least one file`);
  }
  if (input.files.length > PACKAGE_LIMITS.files) {
    throw new KernelError('too-large', `plugin lists ${input.files.length} files, the limit is ${PACKAGE_LIMITS.files}`, manifest.id);
  }

  const seen = new Set<string>();
  let total = 0;
  const files: PackageFile[] = [];
  for (const entry of input.files) {
    if (!isRecord(entry)) throw bad('every file entry must be an object');
    const { path, size, sha256, lazy } = entry;
    if (typeof path !== 'string' || safeRelativePath(path) === null) {
      throw bad(`file path "${String(path)}" is not a safe relative path`);
    }
    if (seen.has(path)) throw bad(`file "${path}" is listed twice`);
    seen.add(path);
    if (typeof size !== 'number' || !Number.isSafeInteger(size) || size < 0) {
      throw bad(`file "${path}" has an invalid size`);
    }
    if (size > PACKAGE_LIMITS.fileBytes) {
      throw new KernelError('too-large', `file "${path}" is ${size} bytes, the limit is ${PACKAGE_LIMITS.fileBytes}`, manifest.id);
    }
    if (typeof sha256 !== 'string' || !SHA256.test(sha256)) {
      throw bad(`file "${path}" has an invalid sha256`);
    }
    if (lazy !== undefined && typeof lazy !== 'boolean') {
      throw bad(`file "${path}" has an invalid lazy flag`);
    }
    total += size;
    files.push({ path, size, sha256, ...(lazy === true ? { lazy: true } : {}) });
  }
  if (files.every((file) => file.lazy === true)) {
    throw bad(`${PACKAGE_FILE} must list at least one file that is not lazy`);
  }
  if (total > PACKAGE_LIMITS.totalBytes) {
    throw new KernelError('too-large', `plugin is ${total} bytes, the limit is ${PACKAGE_LIMITS.totalBytes}`, manifest.id);
  }

  if (manifest.role === 'manager') {
    // These URLs are answered by the kernel or the server before the manager
    // is consulted; a manager shipping them would be shadowed silently.
    const shadowed = files.find(({ path }) =>
      path === 'sw.js' || /^(?:__kernel|__plugins|__ca|__cf|registry)\//.test(path),
    );
    if (shadowed !== undefined) {
      throw bad(`manager file "${shadowed.path}" collides with a URL the kernel reserves`);
    }
  }

  return Object.freeze({ format: PACKAGE_FORMAT, manifest, files: Object.freeze(files) });
}

// ---------------------------------------------------------------- format 2 envelope

/**
 * The slim `plugin.json` (format 2): only pointers, no content.
 *
 * ```json
 * { "format": 2, "manifest": "mf-manifest.json", "bundle": "plugin.bundle.json" }
 * ```
 *
 * `manifest` and `bundle` are relative paths in the same directory or a
 * subdirectory. Changing plugin.json (the pointers) does not change
 * mf-manifest.json, and vice versa: the two are independent. The attestation
 * bundle signs the mf-manifest digest, so tampering with plugin.json only
 * breaks the pointer, not the proof.
 */
export interface PluginEnvelope {
  readonly format: typeof ENVELOPE_FORMAT;
  /** Path to the content manifest (mf-manifest.json), same-dir or subdir. */
  readonly manifest: string;
  /** Path to the attestation bundle (plugin.bundle.json), if present. */
  readonly bundle?: string;
}

const ENVELOPE_KEYS = new Set(['format', 'manifest', 'bundle']);

/** Validates an untrusted `plugin.json` (format 2) value. */
export function parseEnvelope(input: unknown): PluginEnvelope {
  if (!isRecord(input)) throw bad(`${PACKAGE_FILE} must be an object`);
  const unknown = Object.keys(input).filter((key) => !ENVELOPE_KEYS.has(key));
  if (unknown.length > 0) throw bad(`${PACKAGE_FILE} has unknown field(s): ${unknown.join(', ')}`);
  if (input.format !== ENVELOPE_FORMAT) {
    throw bad(`${PACKAGE_FILE} format ${String(input.format)} is not supported (expected ${ENVELOPE_FORMAT})`);
  }
  const manifest = input.manifest;
  if (typeof manifest !== 'string' || safeRelativePath(manifest) === null) {
    throw bad(`${PACKAGE_FILE} field "manifest" must be a safe relative path`);
  }
  const bundle = input.bundle;
  if (bundle !== undefined && (typeof bundle !== 'string' || safeRelativePath(bundle) === null)) {
    throw bad(`${PACKAGE_FILE} field "bundle" must be a safe relative path`);
  }
  return Object.freeze({ format: ENVELOPE_FORMAT, manifest, ...(bundle === undefined ? {} : { bundle }) });
}

/** Result of parsing the top-level `plugin.json`: a format 2 envelope or a format 1 inline package. */
export type ParsedPluginJson =
  | { readonly kind: 'envelope'; readonly envelope: PluginEnvelope }
  | { readonly kind: 'inline'; readonly pkg: PluginPackage };

/**
 * Parses the top-level `plugin.json` and detects whether it is a format 2
 * envelope (pointing at mf-manifest.json) or a format 1 inline package
 * (containing the manifest and files directly, GitHub only).
 */
export function parsePluginJson(input: unknown): ParsedPluginJson {
  if (!isRecord(input)) throw bad(`${PACKAGE_FILE} must be an object`);
  if (input.format === ENVELOPE_FORMAT) {
    return { kind: 'envelope', envelope: parseEnvelope(input) };
  }
  if (input.format === PACKAGE_FORMAT) {
    return { kind: 'inline', pkg: parsePackage(input) };
  }
  throw bad(`${PACKAGE_FILE} format ${String(input.format)} is not supported (expected ${ENVELOPE_FORMAT} or ${PACKAGE_FORMAT})`);
}

/** Encodes a validated relative path for use inside a URL. */
export function encodePath(path: string): string {
  return path.split('/').map(encodeURIComponent).join('/');
}

const CONTENT_TYPES: Readonly<Record<string, string>> = {
  html: 'text/html; charset=utf-8',
  htm: 'text/html; charset=utf-8',
  js: 'text/javascript; charset=utf-8',
  mjs: 'text/javascript; charset=utf-8',
  css: 'text/css; charset=utf-8',
  json: 'application/json',
  map: 'application/json',
  webmanifest: 'application/manifest+json',
  wasm: 'application/wasm',
  svg: 'image/svg+xml',
  png: 'image/png',
  jpg: 'image/jpeg',
  jpeg: 'image/jpeg',
  webp: 'image/webp',
  avif: 'image/avif',
  gif: 'image/gif',
  ico: 'image/x-icon',
  woff2: 'font/woff2',
  woff: 'font/woff',
  ttf: 'font/ttf',
  otf: 'font/otf',
  txt: 'text/plain; charset=utf-8',
  md: 'text/plain; charset=utf-8',
  sha256: 'text/plain; charset=utf-8',
  xml: 'application/xml',
  mp3: 'audio/mpeg',
  ogg: 'audio/ogg',
  opus: 'audio/ogg',
  wav: 'audio/wav',
  m4a: 'audio/mp4',
  mp4: 'video/mp4',
  webm: 'video/webm',
  gz: 'application/gzip',
};

/** Content type by extension; unknown extensions are served as opaque bytes. */
export function contentTypeOf(path: string): string {
  const name = path.slice(path.lastIndexOf('/') + 1);
  const dot = name.lastIndexOf('.');
  if (dot <= 0) return 'application/octet-stream';
  return CONTENT_TYPES[name.slice(dot + 1).toLowerCase()] ?? 'application/octet-stream';
}
