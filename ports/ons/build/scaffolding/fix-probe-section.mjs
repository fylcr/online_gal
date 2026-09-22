/**
 * Replaces the probe section of the shimmed build driver with a version that compiles.
 *
 * Three earlier attempts failed on member access: `current_button_state`,
 * `current_over_button`, `screen_width`, `screen_height`, `mouseOverCheck` and
 * `root_button_link` are all private on `ONScripter`, and `ButtonState` is a nested
 * private type. A free function cannot reach any of them, so the sweep has to be a
 * **member**, and the host has to read the result through a fixed address because the
 * build does not export `_malloc` or `HEAP32`.
 */
import { readFileSync, writeFileSync } from 'node:fs';

const path = 'ports/ons/build/run-web-build-shimmed.sh';
const lines = readFileSync(path, 'utf8').split('\n');

// Locate the probe block: from the `probe = '''` assignment to the closing `PY`.
const start = lines.findIndex((line) => line.startsWith("probe = '''"));
if (start === -1) throw new Error('probe block not found');
let end = -1;
for (let index = start; index < lines.length; index += 1) {
  if (lines[index].trim() === 'PY') {
    end = index;
    break;
  }
}
if (end === -1) throw new Error('end of probe block not found');
console.log(`replacing lines ${start + 1}..${end + 1}`);

const replacement = `probe = '''

// --- embedded-host verification hook (added by the web build overlay) ---------
#if defined(WEB)
int ONScripter::embed_host_probe[ONScripter::EMBED_HOST_PROBE_MAX];

// Reports the pointer state the hit test uses, and maps where the buttons actually are.
//
// This is a **member** for one reason: \\\`current_button_state\\\`, \\\`current_over_button\\\`,
// \\\`screen_width\\\`, \\\`screen_height\\\`, \\\`mouseOverCheck\\\` and \\\`root_button_link\\\` are all
// private, and \\\`ButtonState\\\` is a nested private type — so even naming it from outside
// fails. A free function reaches none of them, which is why two earlier attempts did not
// compile. A member can read every one.
//
// The grid sweep is what makes the question answerable. The only observable available
// from the host was "did the engine fetch new assets", and the first click on a title
// screen loads a whole scene anyway, so that signal cannot tell a correct hit from an
// incidental advance. \\\`mouseOverCheck\\\` is the engine's own hit test.
void ONScripter::embed_host_pointer_probe(int cols, int rows) {
    embed_host_probe[0] = current_button_state.x;
    embed_host_probe[1] = current_button_state.y;
    embed_host_probe[2] = current_button_state.button;
    embed_host_probe[3] = current_button_state.down_flag ? 1 : 0;
    embed_host_probe[4] = screen_width;
    embed_host_probe[5] = screen_height;

    int written = 6;
    if (cols > 0 && rows > 0) {
        // Screen space, which is what \\\`mouseOverCheck\\\` compares against — the space
        // \\\`refreshMouseOverButton\\\` reaches after multiplying by \\\`screen_scale_ratio\\\`.
        for (int row = 0; row < rows && written < EMBED_HOST_PROBE_MAX; row++) {
            for (int col = 0; col < cols && written < EMBED_HOST_PROBE_MAX; col++) {
                const int x = (screen_width * (2 * col + 1)) / (2 * cols);
                const int y = (screen_height * (2 * row + 1)) / (2 * rows);
                mouseOverCheck(x, y);
                embed_host_probe[written++] = current_over_button;
            }
        }
    }
    embed_host_probe_count = written;
}

// Hands the host a fixed address it can read with \\\`Module.getValue\\\`. A pointer parameter
// was not an option: \\\`_malloc\\\` and \\\`HEAP32\\\` are not in the build's
// \\\`EXPORTED_RUNTIME_METHODS\\\`, so the caller cannot prepare a buffer.
extern "C" int *ons_host_pointer_buffer() { return ONScripter::embed_host_probe; }

// Runs the sweep and returns how many ints are valid.
extern "C" int ons_host_pointer_sweep(int cols, int rows) {
    if (!embed_host_instance) return 0;
    embed_host_instance->embed_host_pointer_probe(cols, rows);
    return ONScripter::embed_host_probe_count;
}
#endif
'''
with ons.open('a', encoding='utf-8', errors='surrogateescape') as handle:
    handle.write(probe)
print('## appended the pointer probe to ONScripter.cpp')

# --- declare the probe on the class ----------------------------------------
#
# It is a member, so the class needs declarations plus the shared buffer. WEB-guarded, so
# a native build of the same tree is untouched.
hdr = pathlib.Path('src/onsyuri/ONScripter.h')
header = hdr.read_text(encoding='utf-8', errors='surrogateescape')
hdr_anchor = '    Uint32 getTextureFormat() { return texture_format; };\\n'
hdr_accessor = hdr_anchor + '''#if defined(WEB)
public:
    // --- embedded-host verification probe (web build overlay) ---
    // Defined at the end of ONScripter.cpp. Fixed-size because the host reads the buffer
    // by address rather than allocating.
    enum { EMBED_HOST_PROBE_MAX = 6 + 16 * 16 };
    void embed_host_pointer_probe(int cols, int rows);
    static int embed_host_probe[];
    static int embed_host_probe_count;
#endif
'''
if hdr_anchor not in header:
    sys.exit('ONScripter.h accessor anchor not found; upstream changed')
hdr.write_text(header.replace(hdr_anchor, hdr_accessor, 1), encoding='utf-8', errors='surrogateescape')
print('## added the WEB pointer probe to ONScripter.h')
PY`;

const updated = [...lines.slice(0, start), ...replacement.split('\n'), ...lines.slice(end + 1)];
writeFileSync(path, updated.join('\n'), 'utf8');
console.log('rewritten; new length', updated.length, 'lines (was', lines.length + ')');
