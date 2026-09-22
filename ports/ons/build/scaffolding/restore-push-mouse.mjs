/**
 * Restores `ons_host_push_mouse` after the previous script deleted the whole function.
 *
 * The removal step searched for the end of the report block and matched a brace one level
 * too high, taking the function with it. Re-adding it verbatim, without the hit report that
 * caused the crash loop.
 */
import { readFileSync, writeFileSync } from 'node:fs';

const path = 'ports/ons/build/ons_host_shim.cpp';
let text = readFileSync(path, 'utf8');

if (text.includes('extern "C" int ons_host_push_mouse')) {
  console.log('push_mouse already present');
  process.exit(0);
}

// Insert before the shim's `present` marker, which follows the input entry points.
const anchor = 'extern "C" int ons_host_shim_present(void) { return 1; }';
if (!text.includes(anchor)) {
  console.error('anchor not found');
  process.exit(1);
}

const fn = `extern "C" int ons_host_push_mouse(int type, int x, int y, int button) {
    // The position must be set through SDL, not by pushing a motion event.
    //
    // The engine hit-tests buttons through \`SDL_GetMouseState\`, not through the event it is
    // handling:
    //
    // \`\`\`cpp
    // void ONScripter::refreshMouseOverButton() {
    //     SDL_GetMouseState( &mx, &my );
    //     mx = mx * screen_scale_ratio1;
    //     my = my * screen_scale_ratio2;
    //     mouseOverCheck( mx, my );
    // }
    // \`\`\`
    //
    // Measured: with only pushed events that call returns \`(0,0)\` — before and after a
    // motion event — because \`SDL_PushEvent\` queues an event without updating SDL's internal
    // mouse state. So every click hit-tested against the origin. \`SDL_WarpMouseInWindow\` is
    // the function that does update it, and it is what the engine's own \`warpMouse\` uses.
    if (type == SDL_MOUSEMOTION || type == SDL_MOUSEBUTTONDOWN || type == SDL_MOUSEBUTTONUP) {
        SDL_WarpMouseInWindow(nullptr, x, y);
    }

    SDL_Event event;
    SDL_zero(event);
    event.type = static_cast<Uint32>(type);
    if (type == SDL_MOUSEMOTION) {
        event.motion.type = static_cast<Uint32>(type);
        event.motion.x = x;
        event.motion.y = y;
    } else {
        event.button.type = static_cast<Uint32>(type);
        event.button.state = (type == SDL_MOUSEBUTTONDOWN) ? SDL_PRESSED : SDL_RELEASED;
        event.button.button = static_cast<Uint8>(button);
        event.button.clicks = 1;
        event.button.x = x;
        event.button.y = y;
    }
    return ons_host_push_event(&event);
}

${anchor}`;

writeFileSync(path, text.replace(anchor, fn), 'utf8');
console.log('ons_host_push_mouse restored');
