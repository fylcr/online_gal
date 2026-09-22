/**
 * Moves the pointer probe from "after the engine exits" to "while the game runs".
 *
 * The first working reading returned `hits: 0/48` and `button = -1`, which means the state
 * had never been touched — consistent with `callMain` having already returned by the time
 * the probe ran. The buttons and the pointer state exist only while the script is live, so
 * a reading taken after the loop has exited cannot answer the question.
 *
 * `callMain` occupies the thread, but the shim's `ons_host_wait_event` yields through
 * `emscripten_sleep(0)` when nothing is queued. A timer registered before `callMain` runs
 * during those yields, so the probe is scheduled as a repeating one rather than a one-shot
 * after it returns — that way readings are taken while the script is executing, and the
 * host keeps the last non-empty one.
 */
import { readFileSync, writeFileSync } from 'node:fs';

const path = 'ports/ons/harness/ons-worker.js';
let text = readFileSync(path, 'utf8');

const from = `  // Probe the engine's own hit test.
  //
  // Placement matters and was learned the hard way: putting it on a timer does not work,
  // because \`callMain\` occupies the thread and a timer registered before it never fires.
  // Placing it *before* \`callMain\` finds no buttons yet. It goes here, immediately after
  // \`callMain\` returns — the first point where this thread is certainly free and the
  // script has already run. See the message handler below for repeat requests.
  try {
    runPointerProbe(8, 6);
  } catch (error) {
    post({ kind: 'warn', text: \`pointer probe failed: \${String(error)}\` });
  }

  try {
    moduleOptions.callMain(args);
  } catch (error) {
    post({ kind: 'error', text: \`callMain failed: \${String(error)}\` });
    return;
  }

  try {
    runPointerProbe(8, 6);
  } catch (error) {
    post({ kind: 'warn', text: \`pointer probe after callMain failed: \${String(error)}\` });
  }
`;

const to = `  // Probe repeatedly *while the game runs*, not after it has exited.
  //
  // The first working reading came back with \`hits: 0/48\` and \`button = -1\` — no button
  // anywhere in the grid and no interaction ever recorded — which is what a reading taken
  // after \`callMain\` returns looks like: the buttons and the pointer state exist only while
  // the script is live.
  //
  // \`callMain\` occupies this thread, but the shim's \`ons_host_wait_event\` yields through
  // \`emscripten_sleep(0)\` whenever nothing is queued, so a timer registered *before* it
  // runs during those yields. A repeating timer therefore samples the running game, and the
  // host keeps the last reading that actually found something.
  const probeTimer = setInterval(() => {
    try {
      runPointerProbe(12, 8);
    } catch (error) {
      post({ kind: 'warn', text: \`pointer probe failed: \${String(error)}\` });
      clearInterval(probeTimer);
    }
  }, 2000);

  try {
    moduleOptions.callMain(args);
  } catch (error) {
    post({ kind: 'error', text: \`callMain failed: \${String(error)}\` });
    return;
  } finally {
    clearInterval(probeTimer);
  }
`;

if (!text.includes(from)) {
  console.error('anchor not found; the probe scheduling may already be changed');
  process.exit(1);
}
writeFileSync(path, text.replace(from, to), 'utf8');
console.log('probe rescheduled to run while the game is running');
