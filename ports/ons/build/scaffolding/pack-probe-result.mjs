/**
 * Rewrites the engine-side probe to report through **return values** instead of memory.
 *
 * The buffer route is blocked and there is no way around it from the host:
 * `getValue` is not in the build (it belongs to `EXPORTED_RUNTIME_METHODS`, and that flag
 * has the same replace-not-merge hazard as the function export list — adding it produced
 * no change), and the `HEAP*` views are not attached to `Module` either. The probe was
 * confirmed to run (`## worker: pointer probe invoked (8x6)`) and to fail only at the read
 * (`TypeError: module.getValue is not a function`).
 *
 * An integer return value crosses the JS/wasm boundary with no export list involved, so
 * the existing `ons_host_pointer_sweep` — whose export is already verified working — is
 * repurposed to return a packed summary rather than a count.
 *
 * Encoding, all inside one int32:
 *
 *   bits 0-15   `current_button_state.button + 1` (0 means "nothing recorded yet")
 *   bit  16     a press is currently down
 *   bits 17-22  how many grid cells resolved to a button
 *   bits 23-31  hit mask for the first 9 cells (bit i set when cell i hit)
 */
import { readFileSync, writeFileSync } from 'node:fs';

const path = 'ports/ons/build/run-web-build-shimmed.sh';
let text = readFileSync(path, 'utf8');

const from = `// Runs the sweep and returns how many ints are valid.
EMSCRIPTEN_KEEPALIVE extern "C" int ons_host_pointer_sweep(int cols, int rows) {
    if (!embed_host_instance) return 0;
    embed_host_instance->embed_host_pointer_probe(cols, rows);
    return ONScripter::embed_host_probe_count;
}`;

const to = `// Runs the sweep and returns a packed summary of it.
//
// A packed return value rather than a buffer read: \\\`getValue\\\` is not in this build and
// the \\\`HEAP*\\\` views are not on \\\`Module\\\`, so the host cannot read engine memory at all.
// An integer return crosses the boundary with no export list involved.
//
//   bits 0-15   current_button_state.button + 1   (0 = nothing recorded yet)
//   bit  16     a press is currently down
//   bits 17-22  grid cells that resolved to a button
//   bits 23-31  hit mask for the first 9 cells
EMSCRIPTEN_KEEPALIVE extern "C" int ons_host_pointer_sweep(int cols, int rows) {
    if (!embed_host_instance) return 0;
    embed_host_instance->embed_host_pointer_probe(cols, rows);

    const int cells = ONScripter::embed_host_probe_count - 6;
    int hits = 0;
    int mask = 0;
    for (int i = 0; i < cells; i++) {
        if (ONScripter::embed_host_probe[6 + i] >= 0) {
            hits++;
            if (i < 9) mask |= (1 << i);
        }
    }

    const int button = ONScripter::embed_host_probe[2];
    const int down = ONScripter::embed_host_probe[3] ? 1 : 0;
    const int screenW = ONScripter::embed_host_probe[4];
    const int screenH = ONScripter::embed_host_probe[5];
    (void)screenW;
    (void)screenH;

    return ((button + 1) & 0xffff) | (down << 16) | ((hits & 0x3f) << 17) | (mask << 23);
}

// Returns the engine's screen width and height packed as \\\`w | (h << 16)\\\`, so the host can
// check the coordinate space the hit test works in.
EMSCRIPTEN_KEEPALIVE extern "C" int ons_host_pointer_screen() {
    if (!embed_host_instance) return 0;
    ONScripter::embed_host_probe[4] = 0;
    return (ONScripter::embed_host_probe[4]) | (ONScripter::embed_host_probe[5] << 16);
}`;

if (!text.includes(from)) {
  console.error('anchor not found — the probe section may already be rewritten');
  process.exit(1);
}
text = text.replace(from, to);

// `ons_host_pointer_screen` is a new export, and the function list is the mechanism that
// is verified to work.
const exportFrom = '\\"_ons_host_pointer_buffer\\"]")\\n';
const exportTo = '\\"_ons_host_pointer_buffer\\",\\"_ons_host_pointer_screen\\"]")\\n';
if (text.includes(exportFrom)) {
  text = text.replace(exportFrom, exportTo);
  console.log('export list updated with ons_host_pointer_screen');
} else {
  console.log('export list anchor not found; the packed summary needs no new export anyway');
}

writeFileSync(path, text, 'utf8');
console.log('probe rewritten to report through return values');
