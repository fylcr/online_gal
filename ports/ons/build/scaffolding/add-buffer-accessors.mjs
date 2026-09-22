/**
 * Adds the buffer accessors the in-engine hit report needs, and exports them.
 *
 * Written as a file rather than `node -e`: the patcher contains escaped quotes, and passing
 * that through PowerShell has now mangled it three times. A file has no such layer.
 */
import { readFileSync, writeFileSync } from 'node:fs';

const path = 'ports/ons/build/run-web-build-shimmed.sh';
let text = readFileSync(path, 'utf8');

const anchor = 'EMSCRIPTEN_KEEPALIVE extern "C" int *ons_host_pointer_buffer()';
if (!text.includes(anchor)) {
  console.error('buffer accessor anchor not found');
  process.exit(1);
}

if (!text.includes('ons_host_pointer_count')) {
  const added = [
    '// Buffer length and address, for the in-engine hit report that runs inside the shim.',
    '// The report cannot read memory from JavaScript — `getValue` is not exported and the',
    '// HEAP views are not on `Module` — so it reads them here instead.',
    'EMSCRIPTEN_KEEPALIVE extern "C" int ons_host_pointer_count(void) {',
    '    return ONScripter::embed_host_probe_count;',
    '}',
    'EMSCRIPTEN_KEEPALIVE extern "C" const int *ons_host_pointer_values(void) {',
    '    return ONScripter::embed_host_probe;',
    '}',
    '',
    anchor,
  ].join('\n');
  text = text.replace(anchor, added);
  console.log('accessors added');
} else {
  console.log('accessors already present');
}

// Export the two new symbols. The function-export list is the mechanism verified to work;
// the runtime-methods list is not reachable from here.
const exportNeedle = '\\"_ons_host_pointer_buffer\\"]"';
const exportReplacement =
  '\\"_ons_host_pointer_buffer\\",\\"_ons_host_pointer_count\\",\\"_ons_host_pointer_values\\"]"';
if (text.includes(exportNeedle)) {
  text = text.replace(exportNeedle, exportReplacement);
  console.log('export list extended');
} else if (text.includes('_ons_host_pointer_count')) {
  console.log('export list already extended');
} else {
  console.log('WARNING: export list anchor not found');
}

writeFileSync(path, text, 'utf8');
