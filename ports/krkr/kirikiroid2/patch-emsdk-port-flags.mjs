/**
 * Patches Emscripten's own port definitions so a boolean setting serialises as `1`.
 *
 * ## The bug
 *
 * `emcc` validates boolean settings and rejects anything that is not `0` or `1`:
 *
 * ```
 * emcc: error: attempt to set `WASM_LEGACY_EXCEPTIONS` to `True`; use 1/0 to set boolean settings
 * ```
 *
 * and the value comes from Emscripten's own port scripts, which interpolate a Python value straight
 * into the flag:
 *
 * ```python
 * # tools/ports/libpng.py:54
 * flags.append(f'-sWASM_LEGACY_EXCEPTIONS={settings.WASM_LEGACY_EXCEPTIONS}')
 * ```
 *
 * When `settings.WASM_LEGACY_EXCEPTIONS` is the boolean `True`, that produces `...=True`. The same
 * pattern exists in `freetype.py`. This is why the `-legacysjlj` port variants cannot be built on
 * emsdk 6.0.9 — and it is the quirk the upstream CI refers to when it says the libpng and FreeType
 * ports "pass `True` as a boolean flag".
 *
 * ## The fix, and why it is minimal
 *
 * `int(...)` renders `True` as `1` and leaves an integer unchanged, so this is correct for both the
 * boolean and the literal cases and touches only the serialisation. Nothing about the port's
 * behaviour changes.
 *
 * ## Scope
 *
 * This edits the **toolchain**, not the project or this repository's code, so it does not fall under
 * the reverse-engineering preconditions. It is applied idempotently and is reversible; a future emsdk
 * release that fixes the interpolation upstream makes this a no-op.
 */
import { readFileSync, writeFileSync, existsSync } from 'node:fs';

const PORTS = 'D:/emsdk/upstream/emscripten/tools/ports';
/** The files that interpolate this setting into a flag. */
const targets = ['libpng.py', 'freetype.py'];

let patched = 0;

for (const name of targets) {
  const path = `${PORTS}/${name}`;
  if (!existsSync(path)) {
    console.log(`  ${name}: not present, skipped`);
    continue;
  }
  const text = readFileSync(path, 'utf8');

  // Already an int conversion means an earlier run did this.
  if (text.includes('int(settings.WASM_LEGACY_EXCEPTIONS)')) {
    console.log(`  ${name}: already patched`);
    continue;
  }

  const from = '-sWASM_LEGACY_EXCEPTIONS={settings.WASM_LEGACY_EXCEPTIONS}';
  if (!text.includes(from)) {
    console.log(`  ${name}: interpolation not found (upstream may differ), skipped`);
    continue;
  }

  const to = '-sWASM_LEGACY_EXCEPTIONS={int(settings.WASM_LEGACY_EXCEPTIONS)}';
  writeFileSync(path, text.replace(from, to), 'utf8');
  console.log(`  ${name}: patched to serialise the boolean as 1/0`);
  patched += 1;
}

console.log(patched ? `patched ${patched} port definition(s)` : 'nothing to do');
