/**
 * Unifies the probe's two delivery paths onto one format.
 *
 * The engine-loop path (via `emscripten_run_script`) posted `{packed}`, while the
 * worker-request path posted `{values: [...]}`. The host decoded only `values`, so the loop
 * path's readings were read as an array and produced out-of-range numbers
 * (`button: 65531`) — the same engine state that had previously decoded correctly as
 * `hits: 1`. Two paths, two formats, one decoder: the bug was in the reporting, not the
 * engine, and it cost a round of reasoning on garbage.
 *
 * The loop path now emits `values` too, so both are decodable identically.
 */
import { readFileSync, writeFileSync } from 'node:fs';

const path = 'ports/ons/build/ons_host_shim.cpp';
let text = readFileSync(path, 'utf8');

const from = `    if (++ons_wait_calls % ONS_PROBE_EVERY == 0) {
        const unsigned packed = (unsigned)ons_host_pointer_sweep(12, 8);
        char script[160];
        snprintf(script, sizeof(script),
                 "self.postMessage({kind:'pointerProbe',packed:%u,fromLoop:true});",
                 packed);
        emscripten_run_script(script);
    }`;

const to = `    if (++ons_wait_calls % ONS_PROBE_EVERY == 0) {
        // Same shape as the worker-request path: a flat int array. Two paths with two
        // formats and one decoder is what produced out-of-range readings earlier, so both
        // now emit \`values\`.
        ons_host_pointer_sweep(12, 8);
        const int n = ons_host_pointer_count();
        const int *buf = ons_host_pointer_values();

        char script[1024];
        int written = snprintf(script, sizeof(script),
                               "self.postMessage({kind:'pointerProbe',count:%d,cols:12,rows:8,"
                               "values:[",
                               n);
        for (int i = 0; i < n && written > 0 && written < (int)sizeof(script) - 16; i++) {
            written += snprintf(script + written, sizeof(script) - written,
                                (i == 0 ? "%d" : ",%d"), buf[i]);
        }
        snprintf(script + (written > 0 ? written : 0),
                 written > 0 ? sizeof(script) - written : sizeof(script),
                 "],fromLoop:true});");
        emscripten_run_script(script);
    }`;

if (!text.includes(from)) {
  console.error('anchor not found in ons_host_shim.cpp');
  process.exit(1);
}
let updated = text.replace(from, to);

// Declare the two accessors the loop path now needs. They live in ONScripter.cpp, added by
// the same build overlay that defines the probe.
const decl = 'extern "C" int ons_host_pointer_sweep(int cols, int rows);';
const newDecls =
  decl +
  '\nextern "C" int ons_host_pointer_count(void);\nextern "C" const int *ons_host_pointer_values(void);';
if (updated.includes(decl) && !updated.includes('ons_host_pointer_count')) {
  updated = updated.replace(decl, newDecls);
}

writeFileSync(path, updated, 'utf8');
console.log('loop path now posts the same `values` array as the request path');
