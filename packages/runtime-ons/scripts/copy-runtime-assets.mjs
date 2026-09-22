/**
 * Copies the ported harness's JavaScript into `dist`, alongside the compiled TypeScript.
 *
 * `tsc` only emits compiled output, so the worker, the host and the audio relay — which are
 * plain `.mjs` on purpose, since the worker is loaded by URL and neither benefits from being
 * compiled — would otherwise be missing from `dist`, and every import of the package outside
 * the dev resolver would fail with `ERR_MODULE_NOT_FOUND`. `dist/wasm/host.mjs` in
 * particular is imported by the compiled `wasm-runtime.js`.
 *
 * The engine binaries under `assets/` are also copied, because the runtime resolves them
 * relative to the module at run time and a published package would not otherwise carry them.
 *
 * Copying rather than compiling keeps the harness byte-identical to the version that was
 * verified working in the browser harness under `ports/ons/harness/`, which is the point:
 * those files are the engine host, not something to rewrite.
 *
 * Invoked from the root `build:packages` script and not only from this package's own
 * `build`, because `tsc -b` at the root writes every package's `dist/` through project
 * references and would otherwise never run this: a fresh clone could compile but not
 * build the app, failing on `Could not resolve "./wasm/host.mjs"`.
 */
import { copyFileSync, mkdirSync, readdirSync, existsSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const here = dirname(fileURLToPath(import.meta.url));
const packageRoot = join(here, '..');

/** Recursively copies one directory into another, creating destinations as needed. */
function copyTree(from, to) {
  if (!existsSync(from)) return 0;
  mkdirSync(to, { recursive: true });
  let copied = 0;
  for (const entry of readdirSync(from, { withFileTypes: true })) {
    const source = join(from, entry.name);
    const target = join(to, entry.name);
    if (entry.isDirectory()) {
      copied += copyTree(source, target);
    } else {
      copyFileSync(source, target);
      copied += 1;
    }
  }
  return copied;
}

const wasmSources = copyTree(join(packageRoot, 'src', 'wasm'), join(packageRoot, 'dist', 'wasm'));
const assets = copyTree(join(packageRoot, 'assets'), join(packageRoot, 'dist', 'assets'));
console.log(`copied ${wasmSources} harness file(s) and ${assets} engine asset(s) into dist/`);
