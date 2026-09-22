/**
 * Logs the coordinates the engine's own mouse handler receives.
 *
 * Every value on the host side now looks right — canvas pixels arrive unconverted
 * (`451,521`), the engine reacts (`load023` loads), and the motion-before-press ordering
 * matches AetherKiri. What is still unverified is whether the injected motion event reaches
 * `ONScripter::mouseMoveEvent` at all, which is the question this answers:
 *
 * > if (++ons_move_calls % 8 == 0) report event->x, event->y, and whether WAIT_BUTTON_MODE
 * > is set.
 *
 * `mouseMoveEvent` copies `event->x/y` into `current_button_state`, and the patched
 * hit test reads them from there — so if this logs plausible coordinates, the pipeline up to
 * the hit test is sound and the fault is in `mouseOverCheck`'s comparison. If it never logs,
 * the motion event is being dropped before the handler.
 *
 * Guarded by `#if defined(WEB)` and throttled, so it neither affects a native build nor
 * floods the log.
 */
import { readFileSync, writeFileSync } from 'node:fs';

const path = 'ports/ons/build/run-web-build-shimmed.sh';
let text = readFileSync(path, 'utf8');

if (text.includes('ons_move_calls')) {
  console.log('mouse-move diagnostic already present');
  process.exit(0);
}

const anchor = "print('## appended the pointer probe to ONScripter.cpp')";
if (!text.includes(anchor)) {
  console.error('insertion anchor not found');
  process.exit(1);
}

const patch = `
# --- log what the engine's mouse handler receives --------------------------
#
# The last unverified link: whether the injected motion event reaches
# \`ONScripter::mouseMoveEvent\`, which is what copies \`event->x/y\` into
# \`current_button_state\` — the value the patched hit test reads.
evt = pathlib.Path('src/onsyuri/ONScripter_event.cpp')
evt_src = evt.read_text(encoding='utf-8', errors='surrogateescape')
evt_anchor = """    current_button_state.x = event->x;
    current_button_state.y = event->y;"""
evt_patch = """#if defined(WEB)
    // Throttled, and reports the mode as well: \`mouseOverCheck\` is only reached when
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
`;

// The tracked-file restore list has to include this file too, or a killed run leaves it
// patched and the next run patches the patch.
text = text.replace(
  'TRACKED_SOURCES="CMakeLists.txt src/onsyuri/ONScripter.cpp src/onsyuri/ONScripter.h"',
  'TRACKED_SOURCES="CMakeLists.txt src/onsyuri/ONScripter.cpp src/onsyuri/ONScripter.h src/onsyuri/ONScripter_event.cpp"',
);
text = text.replace(
  'git checkout -- CMakeLists.txt src/onsyuri/ONScripter.cpp src/onsyuri/ONScripter.h 2>/dev/null || true',
  'git checkout -- CMakeLists.txt src/onsyuri/ONScripter.cpp src/onsyuri/ONScripter.h src/onsyuri/ONScripter_event.cpp 2>/dev/null || true',
);

writeFileSync(path, text.replace(anchor, patch + anchor), 'utf8');
console.log('motion-receive diagnostic added to the build overlay');
