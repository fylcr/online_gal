/**
 * Bumps the C++ standard for **one translation unit**, not the whole build.
 *
 * Two conflicting requirements showed up in succession, and they pin the fix precisely:
 *
 * 1. At `CXX_STANDARD 14`, the build died compiling `src/plugins/kremscripten.cpp`:
 *    `<emscripten/bind.h>` uses `std::optional` and `std::conjunction`, both C++17.
 * 2. Raising the whole target to 17 then died in `external/krkrz/tjs2/tjsArray.cpp:194`,
 *    which derives from `std::binary_function` — **removed in C++17**. TJS2 is C++14 code.
 *
 * So the tree must stay at C++14 and exactly one file needs 17. `set_source_files_properties`
 * with `CXX_STANDARD` does that at file granularity, leaving every other translation unit on
 * the project's baseline — including all of TJS2, which is the larger and more sensitive part.
 *
 * `CXX_STANDARD_REQUIRED` is set for that file so the compiler errors rather than silently
 * falling back to GNU++14, which would reproduce the original failure in a less obvious form.
 *
 * Idempotent, and it restores from git first, so a killed run cannot leave the file patched
 * twice — the mistake that broke the ONS overlay earlier.
 */
import { execSync } from 'node:child_process';
import { readFileSync, writeFileSync } from 'node:fs';
import { requireRef } from '../../lib/ref.mjs';

const repo = requireRef('krkrsdl2', 'KRKRSDL2_DIR');
const path = `${repo}/CMakeLists.txt`;

// No git restore here: the build driver restores every patched file once, before applying any
// patcher. Having each patcher restore as well was a bug — the second one to run reverted the
// first one's edit, so only the last survived. Observed as a later patcher reporting
// `anchor not found` because an earlier one had wiped the text it depends on.

let text = readFileSync(path, 'utf8');

const anchor = 'set_target_properties(${KRKRSDL2COMMON_NAME} PROPERTIES CXX_STANDARD 14)';
if (!text.includes(anchor)) {
  console.error('anchor not found — CMakeLists.txt may have changed upstream');
  process.exit(1);
}

const patch = `# --- port fix (applied by ports/krkr/krkrsdl2) ---
#
# One file needs C++17 and nothing else may have it.
#
# The Emscripten-only plugin src/plugins/kremscripten.cpp includes <emscripten/bind.h>, which
# requires C++17 (std::optional, std::conjunction). Raising the whole target to 17 instead
# breaks external/krkrz/tjs2/tjsArray.cpp, which derives from std::binary_function — removed
# in C++17. TJS2 is C++14 code and must stay that way.
#
# An explicit -std= flag is used rather than the CXX_STANDARD source property: the property was
# tried and had no effect, because the target already declares CXX_STANDARD 14 and CMake
# resolves the two against each other rather than letting the file win. A compile option goes
# straight onto that translation unit's command line, so there is no resolution to lose.
#
# The precompiled header must also be skipped for this file. It is built once at the target's
# C++14, and the compiler refuses to reuse it from a C++17 translation unit:
#
#   error: C++17 was disabled in precompiled file 'cmake_pch.hxx.pch' but is currently enabled
#
# Skipping it for one file costs a little compile time and nothing else.
set_source_files_properties(src/plugins/kremscripten.cpp PROPERTIES
  SKIP_PRECOMPILE_HEADERS ON
  COMPILE_OPTIONS "-std=c++17")
# --- end port fix ---
${anchor}`;

text = text.replace(anchor, patch);
writeFileSync(path, text, 'utf8');
console.log('src/plugins/kremscripten.cpp set to C++17; everything else stays at C++14');
