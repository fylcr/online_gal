/**
 * Shows how the shimmed build exposes its host-injection functions.
 *
 * The wasm export table is minified, so the readable names only exist in the JS
 * glue. This matters for the worker: it calls `module._ons_host_push_key(...)`, and
 * Emscripten only attaches an exported function to the `Module` object if the name
 * was listed in `EXPORTED_FUNCTIONS` — otherwise it exists solely as an internal
 * binding reached through `wasmExports`.
 */
import { readFileSync } from 'node:fs';

const glue = readFileSync('packages/engine/core/onscripter-yuri/assets/onsyuri.js', 'utf8');

const names = [
  '_ons_host_wait_event',
  '_ons_host_push_event',
  '_ons_host_push_key',
  '_ons_host_push_mouse',
  '_ons_host_shim_present',
];

console.log('=== how each name appears in the glue ===\n');
for (const name of names) {
  const patterns = {
    'as a string literal': `"${name}"`,
    'Module["name"]': `Module["${name}"]`,
    'Module.name=': `Module.${name}`,
    'name=assignment': `${name}=`,
  };
  const found = Object.entries(patterns)
    .filter(([, pattern]) => glue.includes(pattern))
    .map(([label]) => label);
  console.log(`  ${name}`);
  console.log(`    ${found.length ? found.join(', ') : '(no recognisable form)'}`);
  const at = glue.indexOf(name);
  if (at >= 0) {
    const context = glue.slice(Math.max(0, at - 90), at + 90).replace(/\s+/g, ' ');
    console.log(`    context: ...${context}...`);
  }
  console.log();
}

console.log('=== EXPORTED_FUNCTIONS marker present? ===');
console.log('  ' + (glue.includes('EXPORTED_FUNCTIONS') ? 'yes' : 'no — the list is consumed at link time, which is expected'));
