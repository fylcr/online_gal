/**
 * Finds the event entry point the prebuilt ONS build calls into.
 *
 * AetherKiri does not rely on DOM event registration for input. Its build-time
 * shim replaces the engine's own event wait with a host-supplied one:
 *
 * ```c
 * extern "C" int aetherkiri_onscripter_wait_event(SDL_Event *event);
 * extern "C" int aetherkiri_onscripter_poll_host_key_release(SDL_Event *event);
 * ```
 *
 * and `PrepareOnscripterYuriSources.cmake` rewrites `ONScripter.cpp` and the event
 * and sound sources to call those instead of `SDL_WaitEvent`/`SDL_PollEvent`.
 *
 * A prebuilt binary cannot be patched that way, so the question is what it calls
 * instead. SDL's web backend registers DOM listeners through Emscripten, and the
 * engine reaches SDL through the wasm imports — so if the import table exposes the
 * event wait, that is the function to intercept from the worker, which would give
 * input without needing any listener registration at all.
 */
import { readFileSync } from 'node:fs';

const wasm = readFileSync('.ref/onsyuri-web/onsyuri_web/onsyuri.wasm');
const text = wasm.toString('latin1');

console.log('=== event-related symbols in the wasm ===');
const names = new Set();
for (const match of text.matchAll(/[A-Za-z_][\w]{3,40}/g)) {
  const name = match[0];
  if (/^(SDL_)?(Wait|Poll|Peep|Push|Add|Has)Event/.test(name) || /EventWatch|SDL_WaitEvent|SDL_PollEvent/.test(name)) {
    names.add(name);
  }
}
for (const name of [...names].sort()) console.log('  ' + name);

console.log('\n=== named imports the wasm expects ===');
// Emscripten import fields are plain strings in the import section; scanning for
// the conventional prefixes finds them without a full section walk.
const importish = new Set();
for (const match of text.matchAll(/([A-Za-z_][\w]{4,40})/g)) {
  const name = match[0];
  if (/^(emscripten_|_emscripten_|SDL_|dynCall_)/.test(name)) importish.add(name);
}
const interesting = [...importish].filter((name) => /event|Event|mouse|Mouse|key|Key|touch|Touch/.test(name));
console.log(`  ${interesting.length} event-ish names, sample:`);
for (const name of interesting.slice(0, 40)) console.log('    ' + name);

console.log('\n=== does the glue define the SDL event wait? ===');
const glue = readFileSync('.ref/onsyuri-web/onsyuri_web/onsyuri.js', 'utf8');
for (const name of ['SDL_WaitEvent', 'SDL_PollEvent', 'SDL_PeepEvents', 'waitEvent', 'pollEvent']) {
  console.log(`  ${glue.includes(name) ? 'present' : 'absent '}  ${name}`);
}
