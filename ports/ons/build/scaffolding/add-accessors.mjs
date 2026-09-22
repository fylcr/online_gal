/**
 * Adds the two buffer accessors the engine-loop probe path needs.
 *
 * The loop path (in the shim) posts the same `values` array as the worker-request path, so
 * it needs the buffer and its length. Both are added next to the existing
 * `ons_host_pointer_buffer`, under the same `#if defined(WEB)` guard.
 */
import { readFileSync, writeFileSync } from 'node:fs';

const path = 'ports/ons/build/run-web-build-shimmed.sh';
let text = readFileSync(path, 'utf8');

if (text.includes('ons_host_pointer_count')) {
  console.log('accessors already present');
  process.exit(0);
}

const anchor = '// Returns the address of the probe buffer';
if (!text.includes(anchor)) {
  console.error('anchor not found');
  process.exit(1);
}

const added = [
  '// Length of the probe buffer, and the buffer itself, so the engine-loop path can report',
  '// the same `values` array the worker-request path posts. Two paths with two formats and',
  '// one decoder is what produced out-of-range readings earlier.',
  'EMSCRIPTEN_KEEPALIVE extern "C" int ons_host_pointer_count(void) {',
  '    return ONScripter::embed_host_probe_count;',
  '}',
  'EMSCRIPTEN_KEEPALIVE extern "C" const int *ons_host_pointer_values(void) {',
  '    return ONScripter::embed_host_probe;',
  '}',
  '',
  anchor,
].join('\n');

writeFileSync(path, text.replace(anchor, added), 'utf8');
console.log('accessors added');
