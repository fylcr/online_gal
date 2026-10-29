/**
 * Copies the ported harness's JavaScript into `dist`, alongside the compiled TypeScript.
 *
 * `tsc` only emits compiled output, so the worker, the host and the audio relay — which are
 * plain `.mjs` on purpose, since the worker is loaded by URL and neither benefits from being
 * compiled — would otherwise be missing from `dist`, and every import of the package outside
 * the dev resolver would fail with `ERR_MODULE_NOT_FOUND`. `dist/wasm/host.mjs` in
 * particular is imported by the compiled `wasm-runtime.js`.
 *
 * The engine binaries are deliberately *not* copied. They live once, in `assets/` at the package
 * root, which is where the compiled runtime resolves them from (`new URL('../assets/', ...)` in
 * `dist/wasm-runtime.js`). An earlier version of this script also copied them to `dist/assets/`;
 * nothing in the package or the app ever read that copy, and because it was taken from the
 * *checkout* while `assets/` is taken from the engine build, the two copies disagreed on line
 * endings -- `.gitattributes` normalises the repository to LF while the ONScripter build emits a
 * CRLF html -- so a tarball built on Windows differed in bytes from the one CI built. One copy,
 * from the build, is what makes the packaged tarball a function of its inputs.
 *
 * Copying rather than compiling keeps the harness byte-identical to the version that was
 * verified working in the browser harness under `ports/ons/harness/`, which is the point:
 * those files are the engine host, not something to rewrite.
 *
 * Invoked from the root `build:packages` script and not only from this package's own
 * `build`, because `tsc -b` at the root writes every package's `dist/` through project
 * references and would otherwise never run this: a fresh clone could compile but not
 * build the app, failing on `Could not resolve "./wasm/host.mjs"`.
 *
 * It also asserts one property of the compiled output, which is the only committed place that can:
 * that the default engine base the runtime computes still resolves to this package's own `assets/`.
 * See the comment next to that check.
 */
import { copyFileSync, existsSync, mkdirSync, readdirSync, readFileSync, rmSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

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

// `tsc` does not clean `dist/`, and the older version of this script did not either, so a
// checkout that ran it once would keep a stale `dist/assets/` for ever and ship it.
rmSync(join(packageRoot, 'dist', 'assets'), { recursive: true, force: true });

console.log(`copied ${wasmSources} harness file(s) into dist/`);

// The compiled runtime finds its engine files through a URL built from `import.meta.url`, and a
// depth mistake there is invisible until the first frame of the first game: it 404s `onsyuri.js`
// and reads as a broken engine rather than as a wrong path. `dist/wasm-runtime.js` is one level
// below the package root, which is why the literal is `../assets/`. The check lives here rather
// than only in a test because CI runs this script, so a regression fails the build.
const runtimeJs = join(packageRoot, 'dist', 'wasm-runtime.js');
const engineBase = /new URL\('([^']+)',\s*import\.meta\.url\)/.exec(readFileSync(runtimeJs, 'utf8'));
if (!engineBase) {
  console.error(`copy-runtime-assets: no new URL(<literal>, import.meta.url) in ${runtimeJs}`);
  process.exit(1);
}
const engineAssets = fileURLToPath(new URL(engineBase[1], pathToFileURL(runtimeJs)));
if (!existsSync(join(engineAssets, 'onsyuri.js'))) {
  console.error(`copy-runtime-assets: the default engine base resolves to ${engineAssets}, which holds no onsyuri.js`);
  process.exit(1);
}
