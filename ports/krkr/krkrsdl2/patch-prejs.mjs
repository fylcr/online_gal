/**
 * Links `krkr-range.js` into the module with `--js-library`.
 *
 * The ranged-read bridge is an ordinary `extern "C"` function on the C++ side, and the JavaScript
 * that implements it has to be included before the module runs. `--js-library` is the supported way to
 * do that, and it keeps the script in a real `.js` file rather than inside a C string — which is the
 * whole point, since embedding it with `EM_JS` meant a stray apostrophe in a comment terminated a
 * string literal and produced a syntax error that pointed at the preprocessor.
 *
 * Appended to `LINK_FLAGS` for Emscripten targets only. The file must exist before the link, which
 * is why `patch-ranged-storage.mjs` writes it and the build driver runs that patcher before CMake
 * configures.
 */
import { existsSync, readFileSync } from 'node:fs';
import { requireRef } from '../../lib/ref.mjs';

const repo = requireRef('krkrsdl2', 'KRKRSDL2_DIR');
const path = `${repo}/CMakeLists.txt`;
const preJs = `${repo}/krkr-range.js`;

if (!existsSync(preJs)) {
  console.error(`missing ${preJs} — run patch-ranged-storage.mjs first`);
  process.exit(1);
}

let text = readFileSync(path, 'utf8');

if (text.includes('krkr-range.js')) {
  console.log('pre-js already linked');
  process.exit(0);
}

/*
  Anchored on a single line and located by index, not by matching a block.

  The first attempt at this anchor spanned three lines including an indented flag and failed on
  indentation, which is a tab in some places. The anchor line `if(${CMAKE_SYSTEM_NAME} STREQUAL
  "Linux")` also appears later in macOS-specific handling, where inserting would be wrong, so the
  first occurrence is used explicitly.
*/
const anchor = 'if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")';
const at = text.indexOf(anchor);
if (at === -1) {
  console.error('anchor not found in CMakeLists.txt');
  process.exit(1);
}

const patch = `# --- port fix (applied by ports/krkr/krkrsdl2) ---
#
# The ranged-read bridge. C++ declares it extern "C"; this file implements it. Linked with --js-library
# so the JavaScript stays in a .js file instead of being embedded in a C string literal.
if(\${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
  set_property(TARGET \${KRKRSDL2_NAME} APPEND_STRING PROPERTY LINK_FLAGS
    " --js-library \${CMAKE_CURRENT_SOURCE_DIR}/krkr-range.js")
endif()
# --- end port fix ---
${anchor}`;

text = text.slice(0, at) + patch + text.slice(at + anchor.length);
const { writeFileSync } = await import('node:fs');
writeFileSync(path, text, 'utf8');
console.log('linked krkr-range.js with --js-library');
