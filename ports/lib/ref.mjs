/**
 * Locates an upstream reference checkout from a file inside `ports/`.
 *
 * The build recipes under `ports/` drive upstream checkouts that are **not** part of this
 * repository — they are gitignored and the user fetches them (see the root `.gitignore`). The
 * originals hardcoded `D:/online_gal/.ref/<name>`, which made them unrunnable anywhere else.
 *
 * Resolution is: the named environment variable first (so a machine with the checkouts somewhere
 * unusual can override), then `<repo-root>/.ref/<name>`. The repo root is found by walking up
 * from the calling module, so this works for any depth under `ports/`.
 */
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { existsSync } from 'node:fs';

/** Absolute path of the repository root, derived from this module's own location. */
export const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..');

/** {@link repoRoot} with forward slashes, for URLs and for Vite's `/@fs/` prefix. */
export const repoRootPosix = repoRoot.replace(/\\/g, '/');

/** {@link repoRoot} as a `file://` URL, for `roots` handshakes and other URL-shaped consumers. */
export const repoRootUrl = new URL(`file:///${repoRootPosix}/`).href.replace(/\/$/, '');

/**
 * Absolute path of an upstream checkout under `.ref/`.
 *
 * @param {string} name          directory name under `.ref/`, e.g. `'krkrsdl2'`
 * @param {string} [envVar]      environment variable that overrides the location
 * @returns {string} absolute path, whether or not it exists
 */
export function refPath(name, envVar) {
  const override = envVar ? process.env[envVar] : undefined;
  if (override) return resolve(override);
  return join(repoRoot, '.ref', name);
}

/**
 * Same as {@link refPath}, but fails loudly when the checkout is missing.
 *
 * A patcher that silently no-ops because its target directory does not exist is worse than one
 * that stops: the build then proceeds against unpatched sources and fails somewhere unrelated.
 *
 * @param {string} name
 * @param {string} [envVar]
 * @returns {string}
 */
export function requireRef(name, envVar) {
  const path = refPath(name, envVar);
  if (!existsSync(path)) {
    throw new Error(
      `upstream checkout not found at ${path}\n` +
        `fetch it into .ref/${name} (see the root .gitignore for the URL), ` +
        `or point ${envVar ?? 'the matching environment variable'} at it`,
    );
  }
  return path;
}
