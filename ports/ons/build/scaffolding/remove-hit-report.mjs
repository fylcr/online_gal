/**
 * Removes the hit report from `ons_host_push_mouse`, restoring the last working input path.
 *
 * The build that included it produces a crash loop: the worker logs
 * `audio callback failed: RuntimeError: memory access out of bounds` endlessly and the
 * engine never reaches its banner. The only change on that path was this report, which
 * calls `ons_host_pointer_sweep` from inside a *mouse* handler — and `mouseOverCheck`
 * walks the button list, and that list is being modified while the audio thread runs.
 *
 * The report was a good idea in the right place (the input path is the only code that runs
 * while the game is live) but the wrong function to call there. It is removed so the engine
 * works again; the diagnostic stays available through the worker-request path, which does
 * not run during input handling.
 */
import { readFileSync, writeFileSync } from 'node:fs';

const path = 'ports/ons/build/ons_host_shim.cpp';
const lines = readFileSync(path, 'utf8').split('\n');

const start = lines.findIndex((line) => line.includes('Report the hit test for this exact press'));
if (start === -1) {
  console.log('hit report not found — already removed');
  process.exit(0);
}
// The block runs to the closing brace of the `if (type == ...)` that opens after the comment.
let end = -1;
for (let index = start; index < lines.length; index += 1) {
  if (lines[index].trim() === '}' && lines[index - 1].includes('emscripten_run_script(script);')) {
    end = index;
    break;
  }
}
if (end === -1) {
  console.error('could not find the end of the hit-report block');
  process.exit(1);
}

const updated = [...lines.slice(0, start - 1), ...lines.slice(end + 1)];
writeFileSync(path, updated.join('\n'), 'utf8');
console.log(`removed the hit report (lines ${start}..${end + 1})`);
