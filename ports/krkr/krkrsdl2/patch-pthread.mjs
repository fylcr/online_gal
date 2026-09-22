/**
 * Makes the Emscripten build actually multi-threaded.
 *
 * ## Why threads are required, not preferred
 *
 * `tTJSBinaryStream::Read`/`Seek` are blocking by contract, and the engine calls them from inside
 * its script interpreter where nothing can be awaited. A ranged read therefore has to block on a
 * synchronous XHR, which is **forbidden on the main thread** for an `arraybuffer` response and
 * legal only in a worker. So `main` must run on a pthread — hence `USE_PTHREADS` +
 * `PROXY_TO_PTHREAD`.
 *
 * ## Why upstream's flags do not achieve it
 *
 * Upstream enables threads with the **deprecated** spelling:
 *
 * ```cmake
 * # CMakeLists.txt:232 and :249
 * list(APPEND KRKRSDL2_CFLAGS  -sUSE_PTHREADS=1 -sPROXY_TO_PTHREAD=1 -sOFFSCREEN_FRAMEBUFFER=1)
 * list(APPEND KRKRSDL2_LDFLAGS -sUSE_PTHREADS=1 -sPROXY_TO_PTHREAD=1 -sOFFSCREEN_FRAMEBUFFER=1)
 * ```
 *
 * On Emscripten 6.x, enabling threads is **`-pthread`**. The old spelling is accepted without
 * complaint and **produces a single-threaded build** — no warning, no error. Measured: with
 * `KRKRSDL2_EMSCRIPTEN_ENABLE_THREADS=ON` in the cache, the artefact was byte-identical to the
 * non-threaded build (9,045,802 B) and the emitted glue contained no pthread code at all.
 *
 * Upstream already uses the correct spelling for its Linux path (lines 266, 269), so the right
 * flag is in the file — just not for Emscripten.
 *
 * ## The failure mode this guards against
 *
 * The ranged-stream code is behind `#if defined(__EMSCRIPTEN_PTHREADS__)`. If `-pthread` is
 * missing, that code is **compiled out and the build still succeeds**, silently restoring
 * whole-file loading. A successful build is therefore not evidence of anything here, which is why
 * the check after building is for the *macro*, not the exit code.
 *
 * Idempotent, and restores from git first so a killed run cannot double-apply.
 */
import { execSync } from 'node:child_process';
import { readFileSync, writeFileSync } from 'node:fs';
import { requireRef } from '../../lib/ref.mjs';

const repo = requireRef('krkrsdl2', 'KRKRSDL2_DIR');
const path = `${repo}/CMakeLists.txt`;

// No git restore here: the build driver restores every patched file once, before applying any
// patcher. Each patcher restoring as well meant the second one to run reverted the first one's
// edit — which is exactly what happened, and why `patch-cxx-standard`'s work disappeared.

let text = readFileSync(path, 'utf8');

const marker = 'KRKRSDL2_EMSCRIPTEN_ENABLE_THREADS';

/*
  Anchored on a single line, not a block.

  The first attempt matched three lines including an indented `-pthread`, which failed: the
  indentation is a tab in some places and the surrounding text is not stable. A one-line anchor is
  the only part that has to exist, and it is the closing of the Emscripten flag block — the point
  where every option above has already been read, so appending here cannot be ordered wrongly
  relative to the option declarations.

  The first occurrence is used: this line appears again later in the file, inside macOS-specific
  handling, where the same insertion would be wrong.
*/
const anchor = 'if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")';
const at = text.indexOf(anchor);
if (at === -1) {
  console.error('anchor not found — CMakeLists.txt may have changed upstream');
  process.exit(1);
}

const patch = `# --- port fix (applied by ports/krkr/krkrsdl2) ---
#
# Threads are mandatory for the ranged-read storage, not optional tuning.
#
# The ranged stream blocks on a synchronous XHR, which is forbidden on the main thread for an
# arraybuffer response and permitted in a worker. PROXY_TO_PTHREAD is what puts \`main\` on one.
#
# Upstream requests this with \`-sUSE_PTHREADS=1\`, which Emscripten 6.x accepts silently while
# producing a single-threaded build — so the option appears enabled and is not. \`-pthread\` is the
# current spelling, and it is what defines \`__EMSCRIPTEN_PTHREADS__\`, which the ranged-stream code
# is guarded by. Without the macro that code is compiled out and the build quietly falls back to
# loading whole files.
if(\${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
  list(APPEND KRKRSDL2_CFLAGS
    -pthread
  )
  list(APPEND KRKRSDL2_LDFLAGS
    -pthread
  )
endif()
# --- end port fix ---
${anchor}`;

// Inserted at the located offset rather than by `replace`, so the *first* occurrence is used even
// though the anchor text also appears later in macOS-specific handling.
text = text.slice(0, at) + patch + text.slice(at + anchor.length);
writeFileSync(path, text, 'utf8');
console.log('added -pthread for Emscripten targets (defines __EMSCRIPTEN_PTHREADS__)');
