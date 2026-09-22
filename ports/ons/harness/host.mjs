/**
 * Main-thread half of the ONS worker host.
 *
 * Responsibilities, and deliberately nothing else:
 *
 *  - hand the canvas to the worker as an `OffscreenCanvas`, because a transferred
 *    canvas can only be controlled from one thread and the engine now owns it;
 *  - forward input, since the engine can no longer read the DOM;
 *  - play the engine's audio, which arrives as PCM because a worker has no
 *    `AudioContext`;
 *  - relay logs.
 *
 * The engine itself lives in the worker because byte-granular loading needs
 * `Atomics.wait`, which is illegal on the main thread.
 */

import { AudioRelay } from './audio-relay.mjs';

/** Input events worth forwarding, with the fields the engine's bridge reads. */
const KEY_EVENTS = ['keydown', 'keyup'];
const POINTER_EVENTS = ['mousedown', 'mouseup', 'mousemove', 'wheel'];
const TOUCH_EVENTS = ['touchstart', 'touchend', 'touchmove'];

export function startOnsHost(options) {
  const {
    canvas,
    workerUrl = './ons-worker.js',
    indexUrl = './onsyuri_index.json',
    wasmJsUrl = './onsyuri.js',
    engineBase = './',
    onLog = () => {},
    onError = () => {},
    onWarn = () => {},
    onReady = () => {},
    onExited = () => {},
  } = options;

  const worker = new Worker(workerUrl);

  /**
   * The engine's own resolution, once it reports one.
   *
   * Everything that has to agree with the game — canvas letterboxing and the
   * mouse mapping — depends on this, and it cannot be derived on this side because
   * the canvas control belongs to the worker. Until it arrives, clicks are not
   * forwarded: sending coordinates scaled against a placeholder maps them into the
   * wrong box, and a click that lands in the wrong place is worse than one that
   * does not land at all.
   */
  let engineSize = null;
  /** Guards the one-time resize listener registration. */
  let resizeBound = false;
  /** Pending `readMouseState()` calls, resolved when the worker answers. */
  const mouseStateWaiters = [];
  /** Pending `probePointer()` calls, resolved when the worker answers. */
  const pointerWaiters = [];
  /** The most recent probe result, from the startup probe or a request. */
  let lastPointerProbe = null;
  /** Count of messages received per kind, for diagnosing a silent path. */
  const seenKinds = {};

  /**
   * Where the game is actually drawn inside the canvas element.
   *
   * With `object-fit: contain` the canvas element fills its box while the drawing
   * is centred inside it with letterboxing, so the element's rect is *not* the
   * drawing area. Mapping a click requires the drawn sub-rectangle, which is
   * computed here from the engine's aspect ratio against the element's box.
   */
  /**
   * The rectangle the drawing actually occupies.
   *
   * With `object-fit: fill` this is the whole element — there is no letterboxing to
   * subtract, and the game is stretched to cover the viewport, which is what was asked
   * for. The function is kept rather than inlined because the mapping reads better with a
   * named rectangle, and because `viewport()` reports it for verification.
   *
   * Earlier versions used `contain`, where the element's rect was *not* the drawing area
   * and the sub-rectangle had to be computed; that is where several mapping bugs came from.
   */
  const drawnRect = () => {
    const rect = canvas.getBoundingClientRect();
    if (!engineSize || !rect.width || !rect.height) return null;

    if (scaleMode === 'contain') {
      // Aspect-preserving: `object-fit: contain` fits the drawing inside the element, so the
      // element's rect is *not* the drawing area and the bars have to be excluded. This is
      // the calculation the fill mode was able to drop.
      const scale = Math.min(rect.width / engineSize.width, rect.height / engineSize.height);
      const width = engineSize.width * scale;
      const height = engineSize.height * scale;
      return {
        left: rect.left + (rect.width - width) / 2,
        top: rect.top + (rect.height - height) / 2,
        width,
        height,
      };
    }

    // Fill: the drawing covers the whole element, so there is nothing to exclude.
    return {
      left: rect.left,
      top: rect.top,
      width: rect.width,
      height: rect.height,
    };
  };

  /**
   * How the canvas is scaled into the viewport.
   *
   * `fill` stretches to cover the window, which is what was asked for and what the canvas
   * size makes free of distortion in practice (the element and the game share a 1.778
   * aspect ratio). `contain` preserves the aspect ratio and letterboxes instead, for whenever
   * that is preferred — the game's own resolution may change with a `--window` argument, at
   * which point the two would no longer coincide.
   *
   * Only the CSS `object-fit` changes. Because `drawnRect` computes the drawing area for the
   * contain case, the coordinate mapping stays correct either way.
   */
  let scaleMode = 'fill';

  /** Applies `scaleMode`, and reports the change so it is visible while playing. */
  const applyScaleMode = (mode) => {
    scaleMode = mode === 'contain' ? 'contain' : 'fill';
    canvas.style.objectFit = scaleMode;
    onLog(
      `## worker: scale mode = ${scaleMode}` +
        (scaleMode === 'fill'
          ? ' (stretched to fill the viewport)'
          : ' (aspect ratio preserved, letterboxed)'),
    );
    return scaleMode;
  };

  /**
   * Records the engine's resolution.
   *
   * The canvas is pre-sized to the game's own resolution (793x446) in the markup, so the
   * engine's `alignAspectRatio` has nothing to letterbox, and the page stretches it to fill
   * the viewport with `object-fit: fill`. There is therefore nothing to size here — only to
   * remember, for the coordinate mapping and for `viewport()`.
   */
  const applyResolution = (width, height) => {
    engineSize = { width, height };
    onLog(`engine resolution: ${width}x${height} (canvas stretched to fill the viewport)`);
  };

  /**
   * The engine's audio, played here because a worker has no `AudioContext`.
   *
   * Browsers also refuse to start one without a user gesture, so the relay stays
   * idle until the first interaction and buffers a little until then — a game's
   * opening music often begins before the player clicks.
   */
  const audio = new AudioRelay({
    onLog: (text) => onLog(text),
    onError: (text) => onError(text),
  });
  const resumeAudio = () => {
    void audio.start().then((ok) => {
      if (ok) onLog('audio: started (user gesture received)');
    });
  };
  window.addEventListener('pointerdown', resumeAudio, { once: false });
  window.addEventListener('keydown', resumeAudio, { once: false });

  /**
   * `F2` toggles between stretching and preserving the aspect ratio.
   *
   * A key the engine does not use, and the event is not forwarded either way — the game
   * keeps every key it cares about. Reading a game's script to find a free key would be
   * fragile; F2 is outside what ONScripter binds by default, and the alternative of adding
   * a button would mean editing the page's markup for a diagnostic option.
   */
  window.addEventListener('keydown', (event) => {
    if (event.key === 'F2') {
      event.preventDefault();
      applyScaleMode(scaleMode === 'fill' ? 'contain' : 'fill');
    }
  });

  // The canvas must be transferred, not shared: once transferred the main thread
  // can no longer draw to it, which is why nothing else here touches it.
  let offscreen = null;
  if (typeof canvas.transferControlToOffscreen === 'function') {
    offscreen = canvas.transferControlToOffscreen();
  } else {
    onError('transferControlToOffscreen is unavailable, so the engine cannot render');
  }

  worker.addEventListener('message', (event) => {
    const data = event.data || {};
    // Record every kind received. Without this, "the probe did not report" is
    // indistinguishable from "the probe was never called" — the two need different
    // fixes, and guessing between them is what wasted earlier rounds.
    seenKinds[data.kind] = (seenKinds[data.kind] || 0) + 1;
    switch (data.kind) {
      case 'log':
        onLog(data.text);
        break;
      case 'stderr':
        onLog(`[stderr] ${data.text}`);
        break;
      case 'warn':
        onWarn(data.text);
        break;
      case 'error':
        onError(data.text);
        break;
      case 'index':
        onLog(`index: ${data.fileCount} files, gamedir=${data.gamedir}`);
        break;
      case 'globals':
        onLog(`globals: ${(data.notes || []).join(' | ')}`);
        break;
      case 'trap':
        onError(`TRAP ${data.text}`);
        break;
      case 'probeResult':
        onLog(`probe: ${data.text}`);
        break;
      case 'registry':
        onLog(`registry: ${data.text}`);
        break;
      case 'audio':
        // Transferred buffers: the worker no longer owns them, so nothing is
        // copied here beyond what the relay hands to the audio thread.
        audio.push(data.left, data.right);
        break;
      case 'resolution':
        applyResolution(data.width, data.height);
        break;
      case 'mouseState': {
        // Resolves the pending readMouseState() call.
        const pending = mouseStateWaiters.shift();
        if (pending) pending(data);
        break;
      }
      case 'pointerProbe': {
        // Cached as well as resolved: the startup probe is the reliable reading, so the
        // host keeps the latest result for inspection at any later point.
        lastPointerProbe = data;
        const pending = pointerWaiters.shift();
        if (pending) pending(data);
        break;
      }
      case 'ready':
        onReady();
        break;
      case 'exited':
        onExited({ rangesServed: data.rangesServed, bytesServed: data.bytesServed });
        break;
      default:
        break;
    }
  });

  worker.addEventListener('error', (event) => {
    onError(`worker error: ${event.message} @${event.filename}:${event.lineno}`);
  });

  worker.postMessage(
    {
      kind: 'init',
      indexUrl: new URL(indexUrl, location.href).href,
      wasmJsUrl: new URL(wasmJsUrl, location.href).href,
      engineBase: new URL(engineBase, location.href).href,
      canvas: offscreen,
      // A worker has no window, so the display size has to come from here — it is
      // the only place that can know it. Emscripten's glue reads it unguarded.
      display: {
        width: window.screen ? window.screen.width : window.innerWidth,
        height: window.screen ? window.screen.height : window.innerHeight,
        devicePixelRatio: window.devicePixelRatio || 1,
        // `?trace` turns on the diagnostic cursor trap in the worker.
        trace: new URLSearchParams(location.search).has('trace'),
      },
    },
    offscreen ? [offscreen] : [],
  );

  /**
   * Forwards an event to the worker as a plain object.
   *
   * Events cannot be structured-cloned, and most of their useful state lives
   * behind getters, so the fields are copied explicitly rather than passing the
   * event through.
   *
   * Coordinates are converted through {@link drawnRect} — the letterboxed drawing
   * area, not the element — and scaled by the engine's resolution. Using the
   * element's own rect against the canvas's `width`/`height` attributes is what
   * made clicks land in the corner: the attributes stay at the 300x150 default
   * while the element is over a thousand pixels wide, so the scale factor came out
   * near 0.29 and every click collapsed into a small box at the origin.
   */
  const forward = (type, event) => {
    const payload = { kind: 'input', type };
    if (typeof event.clientX === 'number') {
      const drawn = drawnRect();
      if (drawn) {
        payload.x = (event.clientX - drawn.left) * (engineSize.width / drawn.width);
        payload.y = (event.clientY - drawn.top) * (engineSize.height / drawn.height);
        // Clicks outside the drawing area (in the letterbox bars) are clamped into
        // range rather than dropped, so a click near an edge still registers.
        payload.x = Math.max(0, Math.min(engineSize.width - 1, payload.x));
        payload.y = Math.max(0, Math.min(engineSize.height - 1, payload.y));
      } else if (type !== 'mousemove') {
        // No resolution yet: forwarding would map the click into the wrong box.
        return;
      }
    }
    if (typeof event.button === 'number') payload.button = event.button;
    if (typeof event.deltaY === 'number') payload.deltaY = event.deltaY;
    if (typeof event.key === 'string') {
      payload.key = event.key;
      payload.code = event.code;
      payload.keyCode = event.keyCode;
      payload.shiftKey = event.shiftKey;
      payload.ctrlKey = event.ctrlKey;
      payload.altKey = event.altKey;
      payload.metaKey = event.metaKey;
    }
    if (event.touches) {
      const drawn = drawnRect();
      payload.touches = Array.from(event.touches, (touch) => ({
        x: drawn && engineSize ? (touch.clientX - drawn.left) * (engineSize.width / drawn.width) : 0,
        y: drawn && engineSize ? (touch.clientY - drawn.top) * (engineSize.height / drawn.height) : 0,
      }));
    }
    worker.postMessage(payload);
  };

  for (const type of KEY_EVENTS) {
    window.addEventListener(type, (event) => forward(type, event));
  }
  for (const type of POINTER_EVENTS) {
    canvas.addEventListener(type, (event) => forward(type, event));
  }
  for (const type of TOUCH_EVENTS) {
    canvas.addEventListener(type, (event) => forward(type, event), { passive: true });
  }
  canvas.addEventListener('contextmenu', (event) => event.preventDefault());

  return {
    worker,
    audio,
    post: (message) => worker.postMessage(message),
    /**
     * Delivers an already-normalised input event to the engine.
     *
     * The DOM listeners call this after converting a browser event; a host that drives the
     * runtime programmatically — the `RuntimeInstance` adapter in `@online-gal/runtime-ons` —
     * calls it directly with coordinates already in engine space.
     *
     * `wasm-runtime.ts` calls this for every pointer and key event, and `host.d.mts` declares it,
     * so leaving it out is not a harmless omission: the runtime's `sendInput` throws a
     * `TypeError` on the first click instead of driving the engine. The worker side has always
     * understood `{kind:'input'}`, so this is the only missing half.
     */
    sendInput: (type, payload = {}) => worker.postMessage({ kind: 'input', type, ...payload }),
    /** Asks the worker which lazy-file mechanisms the build provides. */
    probe: () => worker.postMessage({ kind: 'probe' }),
    /** Audio state, for a verification harness. */
    audioStats: () => audio.stats(),
    /** The engine's resolution and the letterboxed area, for verification. */
    viewport: () => ({ engine: engineSize, drawn: drawnRect() }),
    /** Current scaling mode, and a way to set it from the console harness. */
    scaleMode: () => scaleMode,
    setScaleMode: (mode) => applyScaleMode(mode),
    /**
     * Reads `SDL_GetMouseState` from the engine.
     *
     * Answers a question that cannot be settled by observation: the engine hit-tests
     * buttons through that call rather than the event it is handling, so whether a
     * pushed motion event updates SDL's internal position decides whether clicks can
     * land at all.
     */
    readMouseState: () =>
      new Promise((resolve) => {
        mouseStateWaiters.push(resolve);
        worker.postMessage({ kind: 'readMouseState' });
        setTimeout(() => resolve(null), 2000);
      }),
    /**
     * Asks the engine where its buttons are, using the engine's own hit test.
     *
     * Returns the pointer state followed by a `cols x rows` grid of resolved button
     * numbers (`-1` = none). This is the ground truth that the asset-traffic proxy could
     * not provide.
     */
    probePointer: (cols = 8, rows = 6) =>
      new Promise((resolve) => {
        pointerWaiters.push(resolve);
        worker.postMessage({ kind: 'pointerProbe', cols, rows });
        setTimeout(() => resolve(lastPointerProbe), 3000);
      }),
    /** The latest probe result without waiting, for a quick read. */
    lastProbe: () => lastPointerProbe,
    /** Message kinds received so far, and the raw last probe message. */
    diagnostics: () => ({ kinds: { ...seenKinds }, lastProbe: lastPointerProbe }),
    terminate: () => worker.terminate(),
  };
}
