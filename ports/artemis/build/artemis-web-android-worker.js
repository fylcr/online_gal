/* Drives the engine's own Android host, compiled to wasm.
   The engine does its own threading and rendering; this worker only starts it, pumps the proxy queue,
   and feeds input in. */
if (self.name && self.name.indexOf('em-pthread') === 0) {
  importScripts('artemis-web-android.js');
  // Probe: the wake-up for a synchronous proxy is raised from *inside* the pthread (`ELSE` branch of
  // `__emscripten_notify_mailbox_postmessage`, which requires ENVIRONMENT_IS_PTHREAD) or, if that flag
  // is somehow false, it takes the main-thread branch and looks up `PThread.pthreads[...]` instead. The
  // two produce different messages on this side, so the flag is worth printing once.
  console.log('[artc-pt] name=' + self.name + ' isPthread=' + ENVIRONMENT_IS_PTHREAD +
    ' waitingAsync=' + (typeof wasmMemory !== 'undefined' ? 'vm-ok' : 'no-vm'));
} else {
// This worker is Emscripten's **main runtime thread**: it is a worker (so `ENVIRONMENT_IS_WORKER`), and
// its name is not `em-pthread-*`. The engine runs on pthreads it spawns itself.
//
// A pthread reaches this thread through a *synchronous* proxy (`__proxy: 'sync'`, which is how every
// EGL entry point and — with OFFSCREEN_FRAMEBUFFER — every `gl*` call is compiled): it enqueues the call
// on the system proxying queue and blocks until this thread runs it.
//
// `_emscripten_main_thread_process_queued_calls()` is NOT the right pump. It goes through
// `emscripten_proxy_execute_queue` (`library_pthread.c:94`), which runs the queued tasks directly and so
// never runs `receive_notification` — the only code that resets the queue's `notification` state to
// `NOTIFICATION_NONE` (`proxying.c:178`, `em_task_queue.c:209`). Left `PENDING` by a direct drain,
// `em_task_queue_send` then skips `emscripten_thread_mailbox_send` forever, so no further mailbox wake-up
// is ever delivered and the caller's latency becomes whatever the host's timer is.
//
// `__emscripten_check_mailbox` runs the mailbox task instead: it drains the queue *and* keeps the SDK's
// wake-up handshake alive. The wake-up reaches this worker as `{targetThread, cmd:4}` — the glue installs
// a `cmd:4` branch only on pthread workers, and this thread is not one — so that message is handled below
// and is the path that should be doing the work. The timer is a fallback so a lost wake-up cannot hang
// the engine thread.
let runtimeReady = false;
let lastError = '';
let pumps = 0;
let drained = 0;
let cmd4Count = 0;
let unknownMsgs = 0;
let maxGapMs = 0;
let lastCmd4At = 0;
let lastPumpAt = 0;
let maxPumpGapMs = 0;
let ptrMsgs = 0;
let keyMsgs = 0;
// The page works in CSS pixels, but the Android host does not: `ANativeWindow_getWidth/Height` is the
// physical surface size (the GL drawing buffer, i.e. backing-store pixels), and `Renderer::SetStage`
// computes the letterbox viewport from it, while `Renderer::StageX/StageY` map window coordinates back
// through that same viewport. On a real device both the surface and the MotionEvent coordinates are
// physical pixels, so the host must be told the backing size and fed backing coordinates. Booting with
// the CSS size instead (the earlier bug) made `glViewport` cover only a fraction of the drawing buffer,
// which shrank the whole scene into a corner of the canvas.
let dpr = 1;
const toBacking = function (v) { return v * dpr; };

const pump = function (why) {
  if (!runtimeReady) return;
  const m = self.Module;
  if (!m) return;
  const t0 = performance.now();
  if (lastPumpAt) maxPumpGapMs = Math.max(maxPumpGapMs, t0 - lastPumpAt);
  lastPumpAt = t0;
  pumps++;
  try {
    if (m.__emscripten_check_mailbox) m.__emscripten_check_mailbox();
    else if (m._emscripten_main_thread_process_queued_calls)
      m._emscripten_main_thread_process_queued_calls();
  } catch (e) {
    lastError = String((e && e.message) || e).slice(0, 200);
    self.postMessage({ type: 'log', text: 'pump threw (' + why + '): ' + lastError });
  }
};

// Input must not be forwarded before the runtime exists.
//
// `runtimeReady` is checked separately because the exported functions are **not absent** before
// initialization: the Emscripten glue fills every `EXPORTED_FUNCTIONS` slot with
// `makeInvalidEarlyAccess(name)`, which is a real, truthy function that raises
//   Aborted(Assertion failed: call to '_artc_shim_push_motion' via reference taken before Wasm
//   module initialization)
// when called. So `if (Module._artc_shim_push_motion)` is always true and does not guard anything —
// and because `abort()` tears the runtime down (it is what installs the "runtime is dead" state),
// a single pointer event arriving during instantiation kills the engine *before* it ever boots:
// no `packs registered`, no `artc_android_boot`, and `pump()` silently returns forever after. The
// pre-boot presses are dropped, which is correct — there is nothing yet that could receive them.
const live = function (fn) {
  return runtimeReady && typeof fn === 'function';
};

// Fallback only. If the notification handshake is alive this almost never finds work, because `cmd:4`
// drives every drain; when it fires it means a wake-up was dropped.
const FALLBACK_MS = 250;
setInterval(function () { pump('timer'); }, FALLBACK_MS);

// One summary a second rather than a line per drain: a log per drain would itself be the bottleneck.
setInterval(function () {
  self.postMessage({
    type: 'log',
    kind: 'tick',
    text: 'pumps/s ' + pumps + ' cmd4/s ' + cmd4Count + ' unknown=' + unknownMsgs +
      ' key=' + keyMsgs + ' ptr=' + ptrMsgs +
      ' maxPumpGap=' + Math.round(maxPumpGapMs) + 'ms maxCmd4Gap=' + Math.round(maxGapMs) + 'ms err=' + lastError,
    heap: self.Module && self.Module.HEAPU8 ? self.Module.HEAPU8.length : -1,
  });
  pumps = 0;
  cmd4Count = 0;
  unknownMsgs = 0;
  maxGapMs = 0;
  maxPumpGapMs = 0;
  ptrMsgs = 0;
  keyMsgs = 0;
}, 1000);

self.onmessage = function (event) {
  const data = event.data || {};
  // Mailbox wake-up from a pthread's synchronous proxy — the low-latency path. The glue's own `cmd:4`
  // branch only exists on pthread workers, so this thread has to handle it itself.
  if (data.cmd === 4) {
    const now = performance.now();
    if (lastCmd4At) maxGapMs = Math.max(maxGapMs, now - lastCmd4At);
    lastCmd4At = now;
    cmd4Count++;
    pump('cmd4');
    return;
  }
  if (data.type === 'pointer') {
    ptrMsgs++;
    if (live(self.Module && self.Module._artc_shim_push_motion))
      self.Module._artc_shim_push_motion(data.down ? 0 : 1, toBacking(data.x), toBacking(data.y));
    return;
  }
  if (data.type === 'pointermove') {
    if (live(self.Module && self.Module._artc_shim_push_motion))
      self.Module._artc_shim_push_motion(2, toBacking(data.x), toBacking(data.y));
    return;
  }
  if (data.type === 'key') {
    keyMsgs++;
    if (live(self.Module && self.Module._artc_shim_push_key))
      self.Module._artc_shim_push_key(data.key, data.down ? 0 : 1);
    return;
  }
  if (data.type === 'resize') {
    // The Android host leaves `onNativeWindowResized` null, so the engine has no resize path: the
    // viewport is fixed at boot. Keep the ratio current so later input maps correctly, and note the gap
    // rather than pretending to resize.
    if (typeof data.dpr === 'number' && data.dpr > 0) dpr = data.dpr;
    return;
  }
  if (data.type !== 'canvas') {
    // Anything else is unexpected. Reporting it by shape distinguishes "the wake-up never came" from
    // "it came in a form this thread does not recognise".
    unknownMsgs++;
    if (unknownMsgs === 1)
      self.postMessage({ type: 'log', text: 'unhandled message: ' + JSON.stringify(Object.keys(data)) });
    if (data.targetThread !== undefined) pump('targetThread');
    return;
  }
  const msgDpr = (typeof data.dpr === 'number' && data.dpr > 0) ? data.dpr : 1;
  dpr = msgDpr;
  self.__artcDpr = dpr;
  self.__artcBase = data.base || './';
  self.__artcPacks = {};
  self.document = { querySelector: function (s) { return s === '#canvas' ? data.canvas : null; } };
  if (typeof self.WebGLRenderingContext === 'undefined')
    self.WebGLRenderingContext = function WebGLRenderingContext() {};
  const say = function (t) { self.postMessage({ type: 'log', text: String(t) }); };
  self.__artcSay = say;
  self.Module = {
    canvas: data.canvas,
    print: function (t) { console.log('[artc]', t); },
    printErr: function (t) {
      console.warn('[artc]', t);
      // Surface the engine's own diagnostics in the page log too, so a measurement does not have to be
      // fished out of a 1000-line console by hand.
      const s = String(t);
      if (/SetText:|viewport:|print:|draw\[/.test(s)) say('eng|' + s.slice(0, 220));
    },
    onRuntimeInitialized: function () {
      runtimeReady = true;
      // The Android host finds its pack chain by listing the app data directory, and on web that
      // directory is a registry in the shim (see android-shim/src/shim.cpp). It must be filled before
      // `artc_android_boot`, because the engine thread starts and scans it inside that call.
      const addPack = self.Module.cwrap('artc_shim_add_pack', null, ['string']);
      const boot = self.Module.cwrap('artc_android_boot', 'number', ['number', 'number']);
      const base = self.__artcBase || './';
      fetch(base + '__packs')
        .then(function (r) { return r.json(); })
        .then(function (names) {
          for (const n of names) addPack(n);
          say('packs registered: ' + names.join(', '));
          // Backing-store pixels, matching the transferred canvas: this becomes the engine's
          // `ANativeWindow` size, and therefore its `glViewport`.
          const bw = Math.max(1, Math.round((data.width || 1280) * dpr));
          const bh = Math.max(1, Math.round((data.height || 720) * dpr));
          say('artc_android_boot(' + bw + 'x' + bh + ') -> ' + boot(bw, bh));
        })
        .catch(function (e) { say('packs/boot failed: ' + (e && e.message)); });
    },
  };
  importScripts('artemis-web-android.js');
};
}
