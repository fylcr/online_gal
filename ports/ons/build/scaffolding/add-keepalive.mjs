/**
 * Adds `EMSCRIPTEN_KEEPALIVE` to the two probe entry points in the build driver.
 *
 * They were listed in `EXPORTED_FUNCTIONS` and still absent from the output, because
 * nothing in the engine references them and the linker discards unreferenced code. The
 * keepalive attribute is what stops that — the same reason the other shim entry points
 * carry it.
 */
import { readFileSync, writeFileSync } from 'node:fs';

const path = 'ports/ons/build/run-web-build-shimmed.sh';
let text = readFileSync(path, 'utf8');

const edits = [
  [
    'extern "C" int *ons_host_pointer_buffer()',
    'EMSCRIPTEN_KEEPALIVE extern "C" int *ons_host_pointer_buffer()',
  ],
  [
    'extern "C" int ons_host_pointer_sweep(int cols, int rows)',
    'EMSCRIPTEN_KEEPALIVE extern "C" int ons_host_pointer_sweep(int cols, int rows)',
  ],
];

for (const [from, to] of edits) {
  if (!text.includes(from)) {
    console.log(`not found (already patched?): ${from}`);
    continue;
  }
  // Only the definition, not the declaration in the shim header (which is a separate file).
  text = text.replace(from, to);
  console.log(`patched: ${to}`);
}

writeFileSync(path, text, 'utf8');
console.log('done');
