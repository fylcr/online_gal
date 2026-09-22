/**
 * Replaces the shim's loop-probe block with a minimal compiling form, by line range.
 *
 * String anchors are fragile here because the block contains escaped quotes and backticks
 * that are easy to mangle when writing the patcher (which has now happened twice). Locating
 * the block by its own markers and replacing the span avoids that class of mistake.
 */
import { readFileSync, writeFileSync } from 'node:fs';

const path = 'ports/ons/build/ons_host_shim.cpp';
const lines = readFileSync(path, 'utf8').split('\n');

const start = lines.findIndex((line) => line.includes('% ONS_PROBE_EVERY == 0'));
if (start === -1) {
  console.error('loop-probe marker not found');
  process.exit(1);
}
// The block ends at the closing brace of the `if`, which is the first line that is
// exactly `    }` after the marker.
let end = -1;
for (let index = start; index < lines.length; index += 1) {
  if (lines[index].trim() === '}') {
    end = index;
    break;
  }
}
if (end === -1) {
  console.error('end of loop-probe block not found');
  process.exit(1);
}

const replacement = [
  '        // Confirms from the log that the probe fires **during** the game rather than',
  '        // after it exits. The readings come from the worker-request path, which posts',
  '        // the `values` array the host decodes; this path deliberately does not post a',
  '        // second, differently-shaped message, because two formats with one decoder is',
  '        // what produced out-of-range numbers earlier.',
  '        ons_host_pointer_sweep(12, 8);',
  "        emscripten_run_script(\"self.postMessage({kind:'probeTick',fromLoop:true});\");",
];

const updated = [...lines.slice(0, start), ...replacement, ...lines.slice(end + 1)];
writeFileSync(path, updated.join('\n'), 'utf8');
console.log(`replaced lines ${start + 1}..${end + 1} with the minimal form`);
