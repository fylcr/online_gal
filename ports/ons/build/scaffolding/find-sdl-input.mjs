/**
 * Finds how this build's SDL delivers input, so a worker can join that path.
 *
 * Wrapping `addEventListener` on the stub document and canvas caught nothing, and
 * the `on<event>` property path is empty too, so the engine registers somewhere
 * else. Emscripten's event helpers live inside the module closure, not on the
 * worker global, so they cannot be inspected from outside.
 *
 * This searches the glue for the callbacks the engine itself must have installed,
 * and for the target identifiers they are installed with. The target identifier is
 * the key: `findEventTarget` resolves `0`/`1`/`2` through `specialHTMLTargets` and
 * anything else through `document.querySelector`, so a numeric target means the
 * array decides where the handler lands — and that array was built from
 * `globalThis.document` when the glue loaded.
 */
import { readFileSync } from 'node:fs';

const glue = readFileSync('.ref/onsyuri-web/onsyuri_web/onsyuri.js', 'utf8');

/** Prints every distinct call to an `_emscripten_set_*` input registration. */
console.log('=== input registrations the glue defines ===');
const registrations = new Set();
for (const match of glue.matchAll(/_(emscripten_set_(?:key|mouse|touch|wheel|focus|blur|pointer)[a-z_]*)_callback_on_thread\s*=/g)) {
  registrations.add(match[1]);
}
for (const name of [...registrations].sort()) console.log('  ' + name);

console.log('\n=== how a target is resolved ===');
const findIndex = glue.indexOf('var findEventTarget=');
console.log('  ' + glue.slice(findIndex, findIndex + 260));

console.log('\n=== is there an SDL-specific event path? ===');
for (const name of [
  'SDL2',
  'SDL_GetMouseState',
  '_SDL_SendMouseMotion',
  'EmscriptenMouseEvent',
  'EmscriptenKeyboardEvent',
  'registerMouseEventCallback',
]) {
  const count = glue.split(name).length - 1;
  console.log(`  ${String(count).padStart(4)}  ${name}`);
}

console.log('\n=== the ids Emscripten accepts for special targets ===');
const idIndex = glue.indexOf('specialHTMLTargets=[');
console.log('  ' + glue.slice(idIndex, idIndex + 120));

console.log('\n=== does the engine use SDL_PollEvent through a callback? ===');
for (const name of ['pollEvent', 'SDL_WaitEvent', 'videoPoll', 'PollEvent']) {
  const count = glue.split(name).length - 1;
  console.log(`  ${String(count).padStart(4)}  ${name}`);
}
