/**
 * Reports the engine's hit test at the exact moment a press is handled.
 *
 * `evaluate_script` and every other main-thread request now times out: the engine's loop
 * saturates the thread and only yields to itself through `emscripten_sleep`, so the page
 * cannot answer a query. That rules out inspecting the hit test from outside — and it is
 * why real MCP clicks cannot help either, since the input path is the same thread.
 *
 * The one place that runs while input is being processed is the shim's
 * `ons_host_push_mouse`, called from the worker the instant the page forwards a click. So
 * the hit test is queried there, and the verdict is put in the log with
 * `emscripten_run_script`, which executes in the page context without needing a reply.
 *
 * This is the measurement that has been missing: whether a click arriving with the
 * *converted* coordinates resolves to a button. Without it, "hits: 0" cannot be told apart
 * from "the game had no buttons on screen".
 */
import { readFileSync, writeFileSync } from 'node:fs';

const path = 'ports/ons/build/ons_host_shim.cpp';
let text = readFileSync(path, 'utf8');

const anchor = 'extern "C" int ons_host_push_mouse(int type, int x, int y, int button) {';
if (!text.includes(anchor)) {
  console.error('push_mouse anchor not found');
  process.exit(1);
}

const body = `${anchor}
    // Report the hit test for this exact press, from inside the input path.
    //
    // Nothing outside the engine can measure this: the loop saturates the main thread, so
    // \`evaluate_script\` times out, and a real MCP click travels the same blocked thread.
    // This function is invoked the moment the page forwards a click, so querying the hit
    // test here is the only way to see whether the click resolves to a button.
    if (type == SDL_MOUSEBUTTONDOWN || type == SDL_MOUSEMOTION) {
        ons_host_pointer_sweep(12, 8);

        const int *buf = ons_host_pointer_values();
        int hits = 0;
        int firstHit = -1;
        for (int i = 6; i < ons_host_pointer_count(); i++) {
            if (buf[i] >= 0) {
                if (firstHit < 0) firstHit = i;
                hits++;
            }
        }

        char script[320];
        snprintf(script, sizeof(script),
                 "self.__onsHit&&self.__onsHit({type:%d,x:%d,y:%d,button:%d,screenW:%d,screenH:%d,"
                 "hits:%d,firstHit:%d});",
                 type, x, y, button, buf[4], buf[5], hits, firstHit);
        emscripten_run_script(script);
    }
`;

text = text.replace(anchor, body);

// The buffer accessors the report needs. Declared here because this translation unit is
// where they are used, and defined by the build overlay in ONScripter.cpp.
const declAnchor = 'extern "C" int ons_host_pointer_sweep(int cols, int rows);';
const decls =
  declAnchor +
  '\nextern "C" int ons_host_pointer_count(void);\nextern "C" const int *ons_host_pointer_values(void);';
if (text.includes(declAnchor) && !text.includes('ons_host_pointer_count')) {
  text = text.replace(declAnchor, decls);
}

writeFileSync(path, text, 'utf8');
console.log('shim now reports the hit test at each press');
