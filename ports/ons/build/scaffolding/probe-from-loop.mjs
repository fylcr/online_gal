/**
 * Runs the pointer probe from inside the engine's event loop.
 *
 * Every scheduling attempt from the worker has failed, and the reason is now clear:
 * `callMain` fully occupies the thread. `emscripten_sleep(0)` in the shim yields the
 * engine, but not to JavaScript timers — a repeating `setInterval` registered before
 * `callMain` fired zero times (`probeInvocations: 0`).
 *
 * `ons_host_wait_event` is the one place that runs *inside* the engine's loop while the
 * game is live, so the probe belongs there. Every N calls it sweeps the engine's own hit
 * test and reports through `emscripten_run_script`, which executes in the page context —
 * no return value to ferry back, no memory to read, no export list involved.
 */
import { readFileSync, writeFileSync } from 'node:fs';

const path = 'ports/ons/build/ons_host_shim.cpp';
let text = readFileSync(path, 'utf8');

const from = `extern "C" int ons_host_wait_event(SDL_Event *event) {
    if (!event) return 0;`;

const to = `// Probe the engine's hit test from inside its own loop.
//
// Nothing scheduled from the worker can do this: \`callMain\` occupies the thread, and
// \`emscripten_sleep\` yields the engine rather than the JavaScript event loop, so a
// \`setInterval\` registered before it fires zero times. This function is called by the
// engine's loop while the game is live, which is the only point where the buttons and the
// pointer state exist.
//
// The result is reported with \`emscripten_run_script\`, which runs in the page context.
// That avoids every blocked route at once: no return value to ferry, no engine memory to
// read (\`getValue\` is not exported and the HEAP views are not on \`Module\`), and no export
// list involved.
int ons_host_probe_summary(void);
extern "C" int ons_host_pointer_sweep(int cols, int rows);

static int ons_wait_calls = 0;

/// Calls per probe. Frequent enough to catch a live frame, rare enough not to flood.
static const int ONS_PROBE_EVERY = 240;

extern "C" int ons_host_wait_event(SDL_Event *event) {
    if (!event) return 0;

    if (++ons_wait_calls % ONS_PROBE_EVERY == 0) {
        const unsigned packed = (unsigned)ons_host_pointer_sweep(12, 8);
        char script[160];
        snprintf(script, sizeof(script),
                 "self.postMessage({kind:'pointerProbe',packed:%u,fromLoop:true});",
                 packed);
        emscripten_run_script(script);
    }`;

if (!text.includes(from)) {
  console.error('anchor not found in ons_host_shim.cpp');
  process.exit(1);
}
let updated = text.replace(from, to);

// `snprintf` needs the C header; the shim already pulls in SDL and emscripten.
if (!updated.includes('#include <cstdio>')) {
  updated = updated.replace('#include <emscripten.h>', '#include <emscripten.h>\n#include <cstdio>');
}

writeFileSync(path, updated, 'utf8');
console.log('shim now probes from inside the engine loop');
