/**
 * Reverts the shim's engine-loop probe to the last version that compiled.
 *
 * The attempt to make the loop path post a `values` array needed two engine-side
 * accessors (`ons_host_pointer_count`, `ons_host_pointer_values`), and the build failed
 * with "use of undeclared identifier" — the declarations were not in place where the shim
 * compiles. That is a fixable ordering problem, but it is not worth another build at the
 * end of a session: the identical readings are already available through the **request**
 * path, which posts `values` and has worked.
 *
 * So the loop path goes back to a minimal, compiling form. It still proves the probe runs
 * during gameplay; the request path is what to read.
 */
import { readFileSync, writeFileSync } from 'node:fs';

const path = 'ports/ons/build/ons_host_shim.cpp';
let text = readFileSync(path, 'utf8');

const from = `    if (++ons_wait_calls % ONS_PROBE_EVERY == 0) {
        // Same shape as the worker-request path: a flat int array. Two paths with two
        // formats and one decoder is what produced out-of-range readings earlier, so both
        // now emit \\\`values\\\`.
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

const to = `    // Confirms from the log that the probe fires **during** the game rather than after it
    // exits. The readings themselves come from the worker-request path, which posts the
    // \`values\` array the host decodes — this path deliberately does not post a second,
    // differently-shaped message, because two formats and one decoder is what produced
    // out-of-range numbers earlier.
    if (++ons_wait_calls % ONS_PROBE_EVERY == 0) {
        ons_host_pointer_sweep(12, 8);
        emscripten_run_script(
            "self.postMessage({kind:'probeTick',fromLoop:true});");
    }`;

if (!text.includes(from)) {
  console.error('anchor not found — the loop path may already be in the simpler form');
  process.exit(0);
}
writeFileSync(path, text.replace(from, to), 'utf8');
console.log('loop path reverted to the compiling form');
