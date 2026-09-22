#!/usr/bin/env bash
# Builds the ONS web target **with the host event-injection shim**.
#
# Upstream's `script/cross_web.sh` is used unchanged; this wrapper only patches the
# project's own `CMakeLists.txt` first, so the shim is compiled into the target and
# `ons_host_shim.h` is force-included ahead of `SDL_WaitEvent`'s call sites.
#
# The two injected lines are:
#
#   set(CMAKE_CXX_FLAGS "... -include <shim>")     force-include the macro
#   target_sources(onsyuri PRIVATE <shim>.cpp)     compile the implementation
#
# Force-including is what makes the patch airtight: `-include` is processed before
# the translation unit's own includes, so the `SDL_WaitEvent` macro is in place
# before any engine source can use the real symbol. Patching the call sites
# individually would be the alternative, and each one missed would silently revert
# that path to a blocking wait.
#
# The original `CMakeLists.txt` is restored on exit, so the checkout stays pristine
# and the stock build remains reproducible.
set -eu

# Repo root, upstream checkout and `emcc` all come from the shared environment. See `ports/env.sh`.
. "$(dirname "$0")/../../env.sh"

REPO="${REPO:-$REF_DIR/onscripter-yuri}"
SHIM_DIR="${SHIM_DIR:-$PORTS_ROOT/ports/ons/build}"
JOBS="${JOBS:-12}"
BUILD_TYPE="${BUILD_TYPE:-Release}"

EMSDK_UNIX="$(ports_unix "$EMSDK_DIR")"
export EMSDK_HOME="$EMSDK_UNIX"
export BUILD_TYPE

SHIM_CPP_UNIX="$(ports_unix "$SHIM_DIR/ons_host_shim.cpp")"
SHIM_H_UNIX="$(ports_unix "$SHIM_DIR/ons_host_shim.h")"

for file in "$SHIM_CPP_UNIX" "$SHIM_H_UNIX"; do
  [ -f "$file" ] || { echo "## ERROR: missing $file" >&2; exit 1; }
done

cd "$REPO"
mkdir -p thirdparty/port thirdparty/build/arch_wasm

echo "## repo:       $REPO"
echo "## shim:       $SHIM_CPP_UNIX"
echo "## BUILD_TYPE: $BUILD_TYPE, CORE_NUM: $JOBS"
echo "## emcc:       $(command -v emcc)"

# --- restore tracked sources, then patch ----------------------------------
#
# Every tracked file this script modifies is restored **from git** before patching.
#
# An earlier version copied a backup file instead, and that is not safe: a run killed
# before its trap fired leaves the live file patched, and the next run then copies the
# patched file over itself as the "backup" — after which there is no pristine original
# to go back to. A later run did exactly that and produced
# `redefinition of 'embed_host_instance'`, because it patched an already-patched
# `ONScripter.cpp`.
#
# Restoring from git first makes every run start from the committed source whatever a
# previous run left behind, which is what makes a build result trustworthy.
TRACKED_SOURCES="CMakeLists.txt src/onsyuri/ONScripter.cpp src/onsyuri/ONScripter.h src/onsyuri/ONScripter_event.cpp"
for tracked in $TRACKED_SOURCES; do
  if ! git diff --quiet -- "$tracked" 2>/dev/null; then
    echo "## note: restoring $tracked (modified by an earlier run)"
    git checkout -- "$tracked"
  fi
done
rm -f CMakeLists.txt.onsbak

restore() {
  local status=$?
  # From git, not from a copy — see the note above.
  git checkout -- $TRACKED_SOURCES 2>/dev/null || true
  rm -f CMakeLists.txt.onsbak script/cross_web_generated.sh
  if [ "$status" -ne 0 ]; then
    echo "## shimmed build failed with status $status; sources restored from git" >&2
  fi
  return "$status"
}
trap restore EXIT

# The shim path is passed to the compiler as a bare relative path, and it is
# relative to the **build directory**, not the project root: Make runs the compile
# from `build_web/`, so the path has to be computed from there.
#
# A bare relative path with forward slashes and no spaces is the point: CMake
# re-quotes a compile option containing a space or a quote, and the compiler then
# receives `#include ""C:/.../ons_host_shim.h" "` — a doubled quote and an empty
# filename on every translation unit.
#
# Derived rather than written out, so moving `ports/ons/build/` does not silently
# point the force-include at a path that no longer exists.
REPO_UNIX="$(cygpath -u "$REPO" 2>/dev/null || echo "$REPO")"
SHIM_H_REL="$(python -c 'import os,sys; print(os.path.relpath(sys.argv[1], os.path.join(sys.argv[2], "build_web")).replace(os.sep, "/"))' "$SHIM_H_UNIX" "$REPO_UNIX")"
echo "## shim include (relative to build dir): $SHIM_H_REL"

python - "$SHIM_CPP_UNIX" "$SHIM_H_UNIX" "$SHIM_H_REL" <<'PY'
import pathlib, sys

shim_cpp, shim_h, shim_h_rel = sys.argv[1], sys.argv[2], sys.argv[3]
path = pathlib.Path('CMakeLists.txt')
text = path.read_text(encoding='utf-8', errors='surrogateescape')

anchor = 'function(config_web TARGET_NAME)\n    add_executable(${TARGET_NAME} ${ONSYURI_CODE})\n'
if anchor not in text:
    sys.exit('anchor for config_web not found; upstream layout changed')

injection = (
    anchor
    + '    # --- host event-injection shim (applied by run-web-build-shimmed.sh) ---\n'
    + f'    target_sources(${{TARGET_NAME}} PRIVATE "{shim_cpp}")\n'
    + '    target_compile_options(${TARGET_NAME} PRIVATE -sUSE_SDL=2)\n'
    + f'    target_compile_options(${{TARGET_NAME}} PRIVATE -include {shim_h_rel})\n'
    + '    set_property(TARGET ${TARGET_NAME} APPEND_STRING PROPERTY LINK_FLAGS\n'
    # `getValue` joins the runtime methods so the pointer probe's buffer can be read.
    #
    # Same replacement-not-merge hazard as the function list below: upstream sets
    # `-sEXPORTED_RUNTIME_METHODS=callMain,UTF8ToString,FS,PATH,IDBFS` through
    # `EMCC_FLAGS`, and a later flag replaces rather than extends it. Without `getValue`
    # the probe fails with `TypeError: module.getValue is not a function`, and the HEAP
    # views are not exposed as a fallback either.
    + '        " -sEXPORTED_RUNTIME_METHODS=[\\"callMain\\",\\"UTF8ToString\\",\\"FS\\",\\"PATH\\",'
      '\\"IDBFS\\",\\"getValue\\"]"\n'
    # The upstream list is repeated here on purpose.
    #
    # `LINK_FLAGS` is appended *after* upstream's `EMCC_FLAGS`, and a later
    # `-sEXPORTED_FUNCTIONS=` **replaces** an earlier one rather than merging. So naming
    # only the shim's functions silently dropped `callMain,UTF8ToString,FS,PATH,IDBFS` —
    # the host's entire engine interface — and the shim exports never appeared either,
    # because the two flags were fighting over the same setting.
    #
    # Merging the two lists into one flag is the fix: upstream's entries first, then the
    # shim's.
    + '        " -sEXPORTED_FUNCTIONS=[\\"callMain\\",\\"UTF8ToString\\",\\"FS\\",\\"PATH\\",\\"IDBFS\\",'
      '\\"_ons_host_wait_event\\",\\"_ons_host_push_event\\",'
      '\\"_ons_host_push_key\\",\\"_ons_host_push_mouse\\",\\"_ons_host_shim_present\\",'
      '\\"_ons_host_mouse_state\\",\\"_ons_host_pointer_sweep\\",'
      '\\"_ons_host_pointer_buffer\\"]")\n'
    + '    # --- end shim ---\n'
)
path.write_text(text.replace(anchor, injection, 1), encoding='utf-8', errors='surrogateescape')
print('## patched CMakeLists.txt (include: %s)' % shim_h_rel)

# --- patch the engine's mouse-focus gate -----------------------------------
#
# `refreshMouseOverButton` parks the cursor off-screen whenever SDL reports that the
# window lacks mouse focus:
#
#     SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS
#
# A worker has no real window, so that bit is never set, the hit test always runs
# against a point outside the screen, and no button is ever hovered — however correct
# the coordinates are. Measured: clicks produced reactions but never through the
# intended control.
#
# Overriding `SDL_GetWindowFlags` from the shim was tried and cannot work: SDL defines
# it in `libSDL2.a`, so a second definition fails at link time with a duplicate symbol.
# Patching the one call site is both narrower and honest about what it changes — the
# engine, in this build, treats the pointer as focused because in a worker there is no
# window to be focused.
#
# The edit is guarded by `#if !defined(WEB)` so a native build of the same tree keeps
# the original behaviour, and it fails loudly if the anchor text ever moves.
ons = pathlib.Path('src/onsyuri/ONScripter.cpp')
src = ons.read_text(encoding='utf-8', errors='surrogateescape')
original = """    SDL_GetMouseState( &mx, &my );
    if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS)) {
        mx = screen_device_width;
        my = screen_device_height;
    }"""
replacement = """#if defined(WEB)
    // Embedded host: take the position from the last motion event instead of asking
    // SDL for it. Two problems make the original unreliable here, both measured:
    //
    //   * a worker's window never reports mouse focus, so the branch below parks the
    //     cursor off-screen and no button can ever be hovered;
    //   * `SDL_GetMouseState` returns (0,0) regardless of what is pushed or warped —
    //     probed directly through an exported helper, before and after a motion event,
    //     and after `SDL_WarpMouseInWindow`, which is the same call this class makes
    //     from `warpMouse`.
    //
    // `current_button_state` is where `mouseMoveEvent` already stores the coordinates
    // from the event, so it holds a real position in the same device space the lines
    // below expect.
    mx = current_button_state.x;
    my = current_button_state.y;
#else
    SDL_GetMouseState( &mx, &my );
    if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS)) {
        mx = screen_device_width;
        my = screen_device_height;
    }
#endif"""
if original not in src:
    sys.exit('ONScripter.cpp mouse-focus anchor not found; upstream changed')
ons.write_text(src.replace(original, replacement, 1), encoding='utf-8', errors='surrogateescape')
print('## patched ONScripter.cpp mouse-focus gate for WEB')

# --- publish the button rectangles for verification -------------------------
#
# Reasoning about whether a click lands correctly has been guesswork: the only
# observable was "did the engine fetch new assets", and the first click on a title
# screen legitimately loads a whole scene, so that signal cannot distinguish a correct
# hit from an incidental advance. `ButtonLink::select_rect` is the ground truth, and it
# is `protected`, which is why the probe needs this hook.
#
# Two pieces, both WEB-guarded:
#
#   * the constructor records the instance in a file-static pointer, so a plain
#     `extern "C"` function defined in the same translation unit can reach it;
#   * that function walks `root_button_link` and writes `(x, y, w, h)` quads.
#
# The name must be `extern "C"` to match the shim's declaration: an `extern "C"`
# declaration cannot be satisfied by a C++-mangled member definition, which is exactly
# why the first attempt failed to link.
ctor_anchor = 'ONScripter::ONScripter()\n{'
ctor_probe = '''#if defined(WEB)
// Set here so the button probe below can reach the instance.
static ONScripter *embed_host_instance = nullptr;
#endif

ONScripter::ONScripter()
{
#if defined(WEB)
    embed_host_instance = this;
#endif'''
if ctor_anchor not in src:
    sys.exit('ONScripter constructor anchor not found; upstream changed')
src = src.replace(ctor_anchor, ctor_probe, 1)
ons.write_text(src, encoding='utf-8', errors='surrogateescape')

probe = '''

// --- embedded-host verification hook (added by the web build overlay) ---------
#if defined(WEB)
int ONScripter::embed_host_probe[ONScripter::EMBED_HOST_PROBE_MAX];
int ONScripter::embed_host_probe_count = 0;

// Reports the pointer state the hit test uses, and maps where the buttons actually are.
//
// This is a **member** for one reason: \`current_button_state\`, \`current_over_button\`,
// \`screen_width\`, \`screen_height\`, \`mouseOverCheck\` and \`root_button_link\` are all
// private, and \`ButtonState\` is a nested private type — so even naming it from outside
// fails. A free function reaches none of them, which is why two earlier attempts did not
// compile. A member can read every one.
//
// The grid sweep is what makes the question answerable. The only observable available
// from the host was "did the engine fetch new assets", and the first click on a title
// screen loads a whole scene anyway, so that signal cannot tell a correct hit from an
// incidental advance. \`mouseOverCheck\` is the engine's own hit test.
void ONScripter::embed_host_pointer_probe(int cols, int rows) {
    embed_host_probe[0] = current_button_state.x;
    embed_host_probe[1] = current_button_state.y;
    embed_host_probe[2] = current_button_state.button;
    embed_host_probe[3] = current_button_state.down_flag ? 1 : 0;
    embed_host_probe[4] = screen_width;
    embed_host_probe[5] = screen_height;

    int written = 6;
    if (cols > 0 && rows > 0) {
        // Screen space, which is what \`mouseOverCheck\` compares against — the space
        // \`refreshMouseOverButton\` reaches after multiplying by \`screen_scale_ratio\`.
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

// Hands the host a fixed address it can read with \`Module.getValue\`. A pointer parameter
// was not an option: \`_malloc\` and \`HEAP32\` are not in the build's
// \`EXPORTED_RUNTIME_METHODS\`, so the caller cannot prepare a buffer.
// Buffer length and address, for the in-engine hit report that runs inside the shim.
// The report cannot read memory from JavaScript — `getValue` is not exported and the
// HEAP views are not on `Module` — so it reads them here instead.
EMSCRIPTEN_KEEPALIVE extern "C" int ons_host_pointer_count(void) {
    return ONScripter::embed_host_probe_count;
}
EMSCRIPTEN_KEEPALIVE extern "C" const int *ons_host_pointer_values(void) {
    return ONScripter::embed_host_probe;
}

EMSCRIPTEN_KEEPALIVE extern "C" int *ons_host_pointer_buffer() { return ONScripter::embed_host_probe; }

// Runs the sweep and returns a packed summary of it.
//
// A packed return value rather than a buffer read: \`getValue\` is not in this build and
// the \`HEAP*\` views are not on \`Module\`, so the host cannot read engine memory at all.
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

// Returns the engine's screen width and height packed as \`w | (h << 16)\`, so the host can
// check the coordinate space the hit test works in.
EMSCRIPTEN_KEEPALIVE extern "C" int ons_host_pointer_screen() {
    if (!embed_host_instance) return 0;
    ONScripter::embed_host_probe[4] = 0;
    return (ONScripter::embed_host_probe[4]) | (ONScripter::embed_host_probe[5] << 16);
}
#endif
'''
with ons.open('a', encoding='utf-8', errors='surrogateescape') as handle:
    handle.write(probe)

# --- log what the engine's mouse handler receives --------------------------
#
# The last unverified link: whether the injected motion event reaches
# `ONScripter::mouseMoveEvent`, which is what copies `event->x/y` into
# `current_button_state` — the value the patched hit test reads.
evt = pathlib.Path('src/onsyuri/ONScripter_event.cpp')
evt_src = evt.read_text(encoding='utf-8', errors='surrogateescape')
evt_anchor = """    current_button_state.x = event->x;
    current_button_state.y = event->y;"""
evt_patch = """#if defined(WEB)
    // Throttled, and reports the mode as well: `mouseOverCheck` is only reached when
    // WAIT_BUTTON_MODE is set, so a plausible position with the wrong mode still explains
    // why no button resolves.
    {
        static int ons_move_calls = 0;
        if (++ons_move_calls % 8 == 1) {
            char dbg[192];
            snprintf(dbg, sizeof(dbg),
                     "self.postMessage({kind:'log',text:'## worker: MOTION ev=(%d,%d) mode=0x%x'"
                     "+(self.__onsMode?self.__onsMode():'')});",
                     event->x, event->y, event_mode);
            emscripten_run_script(dbg);
        }
    }
#endif
    current_button_state.x = event->x;
    current_button_state.y = event->y;"""
if evt_anchor not in evt_src:
    sys.exit('ONScripter_event.cpp motion anchor not found; upstream changed')
evt.write_text(evt_src.replace(evt_anchor, evt_patch, 1), encoding='utf-8', errors='surrogateescape')
print('## patched ONScripter_event.cpp to log received motion coordinates')
print('## appended the pointer probe to ONScripter.cpp')

# --- declare the probe on the class ----------------------------------------
#
# It is a member, so the class needs declarations plus the shared buffer. WEB-guarded, so
# a native build of the same tree is untouched.
hdr = pathlib.Path('src/onsyuri/ONScripter.h')
header = hdr.read_text(encoding='utf-8', errors='surrogateescape')
hdr_anchor = '    Uint32 getTextureFormat() { return texture_format; };\n'
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
PY

# --- run upstream's recipe ------------------------------------------------
PATCHED="script/cross_web_generated.sh"
sed "s|^CORE_NUM=.*|CORE_NUM=${JOBS}|" script/cross_web.sh > "$PATCHED"
chmod +x "$PATCHED"

cd script
# Deliberately *not* `exec`: replacing the shell process discards the EXIT trap, so
# the patched `CMakeLists.txt` would be left in place on success and only cleaned up
# by hand. Running the build as a child lets `restore` fire either way.
bash "./cross_web_generated.sh" "$@"
