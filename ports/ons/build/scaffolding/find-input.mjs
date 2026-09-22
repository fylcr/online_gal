/**
 * Locate how input reaches the ONS engine in this build.
 *
 * The page's own handlers (`window.onkeydown` and friends) only drive the shell
 * UI — fullscreen, menus — so the game input path must be elsewhere. Finding it
 * decides how a worker can deliver events, because in a worker there is no DOM
 * for SDL's registration helpers to attach to.
 */
import { readFileSync } from 'node:fs';

const glue = readFileSync('.ref/onsyuri-web/onsyuri_web/onsyuri.js', 'utf8');

console.log('=== SDL / Emscripten input entry points ===');
for (const name of [
  'SDL_PushEvent',
  'SDL_SendMouse',
  'SDL_SendKeyboard',
  'SDL_SendTouch',
  'JSEvents.registerOrRemoveHandler',
  'registerKeyEventCallback',
  'registerMouseEventCallback',
  'registerTouchEventCallback',
  'specialHTMLTargets',
]) {
  console.log(`  ${glue.includes(name) ? 'present' : 'absent '}  ${name}`);
}

console.log('\n=== handler property names the glue assigns ===');
const names = new Set();
for (const match of glue.matchAll(/["']?(on(?:key|mouse|touch|pointer|wheel|click)[A-Za-z]*)["']?\s*[:=]/gi)) {
  names.add(match[1]);
}
console.log('  ' + ([...names].join(', ') || '(none)'));

console.log('\n=== contexts mentioning mousedown ===');
let index = glue.indexOf('mousedown');
let count = 0;
while (index !== -1 && count < 5) {
  console.log('  ...' + glue.slice(Math.max(0, index - 160), index + 160).replace(/\n/g, ' ') + '...');
  console.log();
  index = glue.indexOf('mousedown', index + 1);
  count += 1;
}

console.log('=== how the engine finds its event target ===');
for (const name of ['Module["canvas"]', 'specialHTMLTargets', 'document.getElementById']) {
  console.log(`  ${glue.split(name).length - 1}  occurrences of ${name}`);
}
