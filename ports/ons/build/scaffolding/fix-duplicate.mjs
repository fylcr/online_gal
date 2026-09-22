/**
 * Removes the duplicated body left by two overlapping edits to `ons_host_push_mouse`.
 *
 * The signature was removed by one script and re-added by another before the first body was
 * gone, so the file now has the signature attached to one body followed by an orphaned
 * second body — hence `unknown type name 'event'`.
 *
 * The orphan is the second copy: it begins at the first line containing
 * `if (type == SDL_MOUSEMOTION || type == SDL_MOUSEBUTTONDOWN` that is preceded by comment
 * lines about `SDL_WarpMouseInWindow` and *not* preceded by a function signature.
 */
import { readFileSync, writeFileSync } from 'node:fs';

const path = 'ports/ons/build/ons_host_shim.cpp';
const lines = readFileSync(path, 'utf8').split('\n');

// Find every occurrence of the warp guard.
const guards = [];
lines.forEach((line, index) => {
  if (line.includes('if (type == SDL_MOUSEMOTION || type == SDL_MOUSEBUTTONDOWN')) guards.push(index);
});
console.log(`found ${guards.length} copy/copies of the warp guard at lines ${guards.map((i) => i + 1).join(', ')}`);
if (guards.length < 2) {
  console.log('no duplication to remove');
  process.exit(0);
}

// Keep the first copy. Remove the second: from the start of its preceding comment block to
// the end of its closing brace.
const second = guards[1];
let commentStart = second;
while (commentStart > 0 && lines[commentStart - 1].trim().startsWith('//')) commentStart -= 1;
let end = second;
while (end < lines.length && lines[end].trim() !== '}') end += 1;
// The function's final `}` is the next one at column 0 after that.
while (end < lines.length && lines[end] !== '}') end += 1;

const removed = end - commentStart + 1;
const updated = [...lines.slice(0, commentStart), ...lines.slice(end + 1)];
writeFileSync(path, updated.join('\n'), 'utf8');
console.log(`removed the duplicate (lines ${commentStart + 1}..${end + 1}, ${removed} lines)`);
