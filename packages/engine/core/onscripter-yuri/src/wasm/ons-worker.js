/**
 * ONS engine host: runs OnscripterYuri inside a worker, reading game data in
 * byte ranges.
 *
 * ## Why the engine lives in a worker
 *
 * Emscripten's filesystem is synchronous; a browser can only fetch
 * asynchronously. The bridge between them blocks a thread on `Atomics.wait`,
 * which browsers forbid on the main thread — it exists to block, and blocking the
 * thread that runs the event loop is what it must not do. In a worker it is legal.
 * The page therefore keeps only the DOM, and the engine renders into an
 * `OffscreenCanvas`.
 *
 * ## Where laziness is injected
 *
 * This build already has a per-file hook, visible in its embedded JS library:
 *
 * ```js
 * 764270: () => g_onsyuri_module && g_onsyuri_index && fetch_file && g_onsyuri_filemap ...
 * 764462: (path) => { var key = g_onsyuri_module.UTF8ToString(path);
 *                     if (!g_onsyuri_filemap[key]) return 0;
 *                     if (g_onsyuri_filemap[key].loaded) return 2;
 *                     try { fetch_file(g_onsyuri_module.FS, key, g_onsyuri_filemap); return 1 } }
 * ```
 *
 * `fetch_file` is supplied by the host, so the decision of *how much* to fetch is
 * ours. The shipped page implements it as `fetch(url).then(r => r.arrayBuffer())`,
 * which is why a 1.8 GB archive arrives as a single allocation. Here it is a
 * blocking ranged request, so a file is fetched a window at a time and the
 * archive never has to exist whole.
 */
'use strict';

/**
 * Posts a message, optionally transferring buffers.
 *
 * Audio arrives as a pair of `Float32Array`s per callback — around 50 per second —
 * so they are transferred rather than copied: a transfer moves ownership and costs
 * nothing, while structured-cloning several hundred kilobytes a second would be
 * work the main thread does not need.
 */
const post = (message, transfer) =>
  transfer && transfer.length ? self.postMessage(message, transfer) : self.postMessage(message);

/**
 * Audio relay with flow control.
 *
 * SDL asks for a large block per callback (4096 frames in this build), and the
 * callback is driven here by a timer rather than by an audio clock — because in a
 * worker there is no audio clock to drive it. A 20 ms timer therefore produces
 * 4096 × 50 = 200 kHz worth of samples, roughly four times real time, and the page
 * would have to discard three quarters of everything it receives.
 *
 * Instead the engine's output is accumulated and released in exact real-time
 * blocks: a 20 ms timer emits 20 ms of audio. Production then matches playback, the
 * queue holds steady instead of being trimmed, and audio stays in step with the
 * picture. The backlog gives the relay something to draw on when a timer tick is
 * late, which is what prevents underruns.
 */
const audioBacklog = { left: [], right: [], frames: 0 };
let audioFramesSent = 0;

function relayAudio(left, right) {
  const frames = left.length;
  audioBacklog.left.push(left);
  audioBacklog.right.push(right);
  audioBacklog.frames += frames;

  // Release whole playback blocks. `release` is the pending amount, so this drains
  // as fast as it fills and the relay's own queue absorbs the difference.
  while (audioBacklog.frames >= audioPlaybackBlock) {
    const blockLeft = new Float32Array(audioPlaybackBlock);
    const blockRight = new Float32Array(audioPlaybackBlock);
    let filled = 0;

    while (filled < audioPlaybackBlock) {
      const head = audioBacklog.left[0];
      const take = Math.min(head.length - audioBacklog.leftOffset, audioPlaybackBlock - filled);
      blockLeft.set(head.subarray(audioBacklog.leftOffset, audioBacklog.leftOffset + take), filled);
      blockRight.set(
        audioBacklog.right[0].subarray(audioBacklog.leftOffset, audioBacklog.leftOffset + take),
        filled,
      );
      filled += take;
      audioBacklog.leftOffset += take;
      if (audioBacklog.leftOffset >= head.length) {
        audioBacklog.left.shift();
        audioBacklog.right.shift();
        audioBacklog.leftOffset = 0;
      }
    }

    audioBacklog.frames -= audioPlaybackBlock;
    audioFramesSent += audioPlaybackBlock;
    post(
      { kind: 'audio', sampleRate: 48000, frames: audioPlaybackBlock, left: blockLeft, right: blockRight },
      [blockLeft.buffer, blockRight.buffer],
    );
  }
}

/** Frames per released block: 20 ms at 48 kHz, matching the timer below. */
const audioPlaybackBlock = 960;
audioBacklog.leftOffset = 0;

/**
 * Reports the resolution the engine actually renders at.
 *
 * The engine prints it during start-up, and that banner is the authoritative
 * source:
 *
 * ```text
 * Display: 793 x 446 (32 bpp)
 * ```
 *
 * Reading it is necessary because SDL does **not** resize `Module.canvas` in this
 * configuration. Measured directly: the transferred `OffscreenCanvas` stays at the
 * HTML default of 300x150 for the whole session while the engine renders 793x446.
 * The page therefore has no way to learn the aspect ratio from the canvas, and
 * sizing against 300x150 is what made clicks collapse toward the origin.
 *
 * Both sources are tried — the banner, and the canvas in case a future build does
 * resize it — so whichever reports first wins and a change from either is passed on.
 */
function watchCanvasResolution(canvas, stdout) {
  let reported = '';

  const announce = (width, height, source) => {
    if (!width || !height) return;
    const size = `${width}x${height}`;
    if (size === reported) return;
    reported = size;

    // Deliberately **not** resizing the canvas here.
    //
    // An earlier version set it to this resolution, which looked right and was
    // wrong: the engine had already created its surface — measured at 1189x669,
    // taken from the `innerWidth`/`innerHeight` values this worker stubs — and
    // SDL scales that surface to whatever the canvas is. Forcing the canvas to the
    // smaller 793x446 banner size therefore made the engine scale its own output
    // *down* into it, cropping the picture.
    //
    // The engine's surface and the canvas have to agree, and the engine controls
    // that. Display scaling is the page's job, and it does it from this report.
    post({ kind: 'resolution', width, height, source });
  };

  // The banner is matched loosely: the engine's own log line is the only place the
  // render resolution appears, and its exact wording varies between builds.
  if (stdout) stdout.onDisplaySize = (width, height) => announce(width, height, 'banner');

  // The canvas is *not* sized here. The engine owns it — see `announce` for why
  // forcing a size cropped the picture.
  let attempts = 0;
  const timer = setInterval(() => {
    attempts += 1;
    // Only report a canvas size the engine actually chose, and treat the HTML
    // default as "not sized yet".
    if (canvas && !(canvas.width === 300 && canvas.height === 150)) {
      announce(canvas.width, canvas.height, 'canvas');
    }
    if (attempts > 300) clearInterval(timer);
  }, 100);
}

const counters = { ranges: 0, bytes: 0, files: 0 };

const stdout = {
  onDisplaySize: null,
};

/** Address of the engine's pointer-probe buffer; stable for the process lifetime. */
let pointerBuffer = 0;

/**
 * Receives the engine's hit-test report for each press.
 *
 * The shim calls this from inside its input path through `emscripten_run_script`, which
 * runs here in the worker. The report is forwarded as an ordinary log line, so it can be
 * read from the DOM — and, importantly, without asking the page anything: the engine
 * saturates the thread, so `evaluate_script` and the console API both time out.
 *
 * This is the measurement that was missing. `hits: 0` could not previously be told apart
 * from "the game had no buttons on screen", and neither could a press that resolved to a
 * button from one that did not.
 */
self.__onsHit = (info) => {
  const type = info.type === 1025 ? 'mousedown' : info.type === 1024 ? 'mousemove' : `type${info.type}`;
  log(
    `## worker: HIT-REPORT ${type} sent=(${info.x},${info.y}) engineScreen=${info.screenW}x${info.screenH} ` +
      `hits=${info.hits} firstHit=${info.firstHit}`,
  );
};

const log = (...parts) => {
  const text = parts
    .map((part) => (typeof part === 'string' ? part : JSON.stringify(part)))
    .join(' ');
  // The engine's own banner is the only place its render resolution appears, so it
  // is parsed here rather than guessed at elsewhere.
  const display = /Display:\s*(\d+)\s*x\s*(\d+)/i.exec(text);
  if (display && typeof stdout.onDisplaySize === 'function') {
    stdout.onDisplaySize(Number(display[1]), Number(display[2]));
  }
  post({ kind: 'log', text });
};

/* ------------------------------------------------------------------ */
/* Descriptor table                                                    */
/* ------------------------------------------------------------------ */

/** Every file the engine may read, keyed by its lowercased path. */
const files = new Map();

/**
 * Reads a byte range synchronously.
 *
 * A blocking `XMLHttpRequest` with a `Range` header is the mechanism Emscripten's
 * own lazy-file plugin uses. It is legal in a worker — the UI never freezes,
 * because this thread is not the one running the event loop — and it fetches
 * exactly the requested window, which is the whole point.
 */
function rangeRequest(url, offset, length) {
  const xhr = new XMLHttpRequest();
  xhr.open('GET', url, false);
  xhr.responseType = 'arraybuffer';
  xhr.setRequestHeader('Range', `bytes=${offset}-${offset + length - 1}`);
  xhr.send(null);
  if (xhr.status !== 206 && xhr.status !== 200) {
    throw new Error(`range request failed: HTTP ${xhr.status} for ${url}`);
  }
  counters.ranges += 1;
  const bytes = new Uint8Array(xhr.response);
  counters.bytes += bytes.byteLength;
  return bytes;
}

/* ------------------------------------------------------------------ */
/* Browser globals the engine's glue expects                           */
/* ------------------------------------------------------------------ */

/**
 * Provides the handful of `window` members Emscripten's glue reads unguarded.
 *
 * Emscripten's browser support assumes a document. `_emscripten_get_screen_size`
 * dereferences `screen.width` without a `typeof` check, and several other helpers
 * read `innerWidth`, `devicePixelRatio` or `location` the same way — so in a
 * worker they throw rather than degrade. Nothing here is used for rendering; the
 * values only have to be present and plausible, and the real display size is
 * passed in from the page, which is the only place that can know it.
 */
function installWorkerGlobals(display) {
  const notes = [];
  const stub = (name, value) => {
    // `in self` is not enough: Chrome's workers already expose a real (mostly
    // empty) `document`, so a property-define would be skipped and the engine
    // would keep hitting the unusable original. Force the value instead.
    try {
      Object.defineProperty(self, name, { value, writable: true, configurable: true });
      notes.push(`${name}: forced`);
    } catch (error) {
      notes.push(`${name}: FAILED ${String(error)}`);
    }
  };

  /** Applies a stub only when the global is missing; used for optional members. */
  const stubIfAbsent = (name, value) => {
    if (name in self) {
      notes.push(`${name}: already present (${typeof self[name]})`);
      return;
    }
    stub(name, value);
  };

  stub('screen', {
    width: display.width,
    height: display.height,
    availWidth: display.width,
    availHeight: display.height,
    colorDepth: 24,
    pixelDepth: 24,
    orientation: { type: 'landscape-primary', angle: 0 },
  });
  stub('innerWidth', display.width);
  stub('innerHeight', display.height);
  stub('outerWidth', display.width);
  stub('outerHeight', display.height);
  stub('devicePixelRatio', display.devicePixelRatio || 1);
  stub('scrollX', 0);
  stub('scrollY', 0);
  stub('pageXOffset', 0);
  stub('pageYOffset', 0);
  stub('location', { href: self.location.href, protocol: 'http:', host: self.location.host });

  /**
   * A minimal `document`, because SDL's cursor and touch setup reaches for one.
   *
   * `SDL_SetCursor` in this build does `document.createElement('canvas').style.cursor`
   * on the way to setting the cursor on the target element, so with no `document`
   * at all it throws `Cannot set properties of undefined` and aborts start-up.
   * The element returned here is inert but structurally adequate: it needs a
   * `style` object and the event-listener methods, and nothing is ever drawn
   * through it — rendering goes to the transferred `OffscreenCanvas`.
   */
  const inertElement = (tag) => {
    const element = {
      tagName: String(tag).toUpperCase(),
      style: {},
      children: [],
      parentNode: null,
      width: 0,
      height: 0,
      setAttribute() {},
      getAttribute() {
        return null;
      },
      removeAttribute() {},
      appendChild(child) {
        this.children.push(child);
        child.parentNode = this;
        return child;
      },
      removeChild(child) {
        this.children = this.children.filter((entry) => entry !== child);
        return child;
      },
      addEventListener() {},
      removeEventListener() {},
      dispatchEvent() {
        return true;
      },
      getBoundingClientRect() {
        return { left: 0, top: 0, right: display.width, bottom: display.height, width: display.width, height: display.height, x: 0, y: 0 };
      },
    };
    return element;
  };

  const body = inertElement('body');
  const documentElement = inertElement('html');

  /**
   * The canvas element the engine looks up by id.
   *
   * The engine's own glue does `document.getElementById("canvas")` during
   * start-up to attach event handlers and to set a mouse cursor, so returning
   * `null` — which is what a stub with no document content would do — makes it
   * throw `Cannot set properties of undefined (setting 'cursor')`. Rendering still
   * goes to the transferred `OffscreenCanvas`; this element only has to satisfy
   * the lookups and hold a `style` object.
   *
   * It is a real `EventTarget` so that Emscripten's `addEventListener` calls
   * actually register: a no-op `addEventListener` would leave the engine with no
   * input handlers at all, and the failure would be silent — the game would render
   * and simply not respond.
   */
  const canvasElement = Object.assign(new EventTarget(), inertElement('canvas'), {
    id: 'canvas',
    width: display.width,
    height: display.height,
    style: {
      cursor: 'auto',
      display: 'block',
      width: `${display.width}px`,
      height: `${display.height}px`,
    },
  });
  canvasElement.focus = () => {};
  canvasElement.getContext = () => null;

  const byId = new Map([
    ['canvas', canvasElement],
    ['onsyuri', canvasElement],
  ]);

  /**
   * A document that can carry listeners.
   *
   * Emscripten's event registration resolves a target through
   * `specialHTMLTargets`, whose first entry is the document, and then calls
   * `addEventListener` on it. A plain object with a no-op `addEventListener`
   * therefore drops every handler, so this is an `EventTarget` as well.
   */
  const documentStub = Object.assign(new EventTarget(), {
    body,
    documentElement,
    createElement: (tag) => inertElement(tag),
    createElementNS: (_ns, tag) => inertElement(tag),
    createTextNode: (text) => ({ textContent: String(text) }),
    getElementById: (id) => byId.get(String(id)) ?? null,
    querySelector: (selector) => (String(selector).includes('canvas') ? canvasElement : null),
    querySelectorAll: () => [],
    getElementsByTagName: (tag) => (String(tag).toLowerCase() === 'canvas' ? [canvasElement] : []),
    fullscreenEnabled: false,
    webkitFullscreenEnabled: false,
    fullscreenElement: null,
    exitFullscreen() {},
    pointerLockElement: null,
  });
  /**
   * Counts listener registrations, to answer whether SDL wired up any input.
   *
   * Emscripten resolves an event target through
   * `specialHTMLTargets = [0, globalThis.document ?? 0, globalThis.window ?? 0]`
   * and then calls `addEventListener` on it. If it captured the wrong object —
   * which is easy, because the array is built when the glue loads — the
   * registration is skipped silently and events are delivered to nobody: the
   * game renders and does not respond. Counting the registrations distinguishes
   * "SDL never registered" from "the events do not carry what SDL reads".
   */
  const listenerCounts = { document: 0, canvas: 0, window: 0, self: 0 };
  const countListeners = (target, label) => {
    const original = target.addEventListener.bind(target);
    target.addEventListener = (type, listener, options) => {
      listenerCounts[label] = (listenerCounts[label] || 0) + 1;
      self.__onsLastListener = { label, type };
      return original(type, listener, options);
    };
  };
  countListeners(documentStub, 'document');
  countListeners(canvasElement, 'canvas');
  // In a worker `window` is `self`, and `specialHTMLTargets[2]` is
  // `globalThis.window ?? 0` — so this is a distinct possible landing site, and
  // the only one not yet ruled out.
  countListeners(self, 'self');
  self.__onsListenerCounts = listenerCounts;

  /**
   * Records any `on*` input property the glue assigns.
   *
   * Emscripten has two registration paths: `addEventListener`, and assigning an
   * `on<event>` property on the target (the older `getEventHandler`/
   * `setEventHandler` route). When the first shows zero calls, the second is what
   * is being used — and the handlers then live as properties, so an event must be
   * passed to that function rather than dispatched as a DOM event.
   */
  self.__onsHandlers = {};
  const captureHandler = (target, label, name) => {
    let stored = target[name];
    Object.defineProperty(target, name, {
      configurable: true,
      get() {
        return stored;
      },
      set(value) {
        self.__onsHandlers[`${label}.${name}`] = typeof value;
        stored = value;
      },
    });
  };
  for (const name of [
    'onkeydown', 'onkeyup', 'onkeypress',
    'onmousedown', 'onmouseup', 'onmousemove', 'onwheel', 'onclick',
    'ontouchstart', 'ontouchend', 'ontouchmove',
  ]) {
    captureHandler(documentStub, 'document', name);
    captureHandler(canvasElement, 'canvas', name);
  }

  stub('document', documentStub);
  // Kept so the input bridge can dispatch into the handlers Emscripten registers.
  self.__onsInputTargets = { document: documentStub, canvas: canvasElement };

  // Fullscreen requests have no meaning without a real document; the engine only
  // calls these on an explicit user action.
  stub('getComputedStyle', () => ({ getPropertyValue: () => '' }));

  /**
   * An `AudioContext`, because SDL's audio init treats its absence as fatal.
   *
   * The glue does literally this:
   *
   * ```js
   * if (!SDL2.audioContext) {
   *   if (typeof AudioContext !== "undefined") SDL2.audioContext = new AudioContext
   *   else if (typeof webkitAudioContext !== "undefined") ...
   * }
   * return SDL2.audioContext === undefined ? -1 : 0
   * ```
   *
   * `SDL_Init` therefore fails with "No audio context available" in a worker,
   * and this engine treats that as fatal — it aborts before it reads the archives
   * at all, which is why the ranged `fetch_file` never even got called for them.
   *
   * A worker genuinely has no Web Audio, so this is a silent context: the graph is
   * accepted and the processor callback fires, but nothing is routed anywhere. That
   * turns a fatal failure into silent audio, which is the right trade for getting
   * the engine to run; real output needs the PCM relayed to an `AudioWorklet` on
   * the main thread, where `AudioContext` does exist.
   */
  if (typeof self.AudioContext === 'undefined') {
    const node = () => ({
      connect() {
        return this;
      },
      disconnect() {},
      start() {},
      stop() {},
      gain: { value: 1 },
      onaudioprocess: null,
      buffer: null,
    });

    /**
     * Audio: a real mixer in the worker, replayed on the main thread.
     *
     * A worker has no `AudioContext`, and SDL's audio init treats its absence as
     * fatal, so this stub exists first to let the engine start. But the engine does
     * not merely need the object to exist — it *writes its mixed PCM into the
     * callback's output buffer*. The previous version handed out a new
     * `Float32Array` on every `getChannelData()` call, so every sample was written
     * into an array that was immediately discarded: the context was not silent
     * because nothing fed it, it was silent because the samples went nowhere.
     *
     * The buffers are therefore allocated once per processor and reused, and
     * whatever the engine writes is copied out after each callback and posted to the
     * main thread, which owns the real `AudioContext`. That is the only way to get
     * sound out of a worker — the audio must cross the thread boundary as data.
     */
    class SilentAudioContext {
      constructor() {
        this.sampleRate = 48000;
        this.currentTime = 0;
        this.state = 'running';
        this.destination = node();
        this.listener = {};
        this._processors = [];
        // Counters so the relay is observable: silence and a broken relay look
        // identical from the page otherwise.
        this._callbacks = 0;
        this._framesSent = 0;
        this._peak = 0;

        this._timer = setInterval(() => {
          this.currentTime += 0.02;
          for (const processor of this._processors) {
            const handler = processor.onaudioprocess;
            if (!handler) continue;
            const frames = processor._frames || 1024;
            // Stable, reused channel buffers — the engine fills these in place.
            const channels = processor._channels;
            channels[0].fill(0);
            channels[1].fill(0);
            try {
              handler({
                outputBuffer: {
                  numberOfChannels: 2,
                  length: frames,
                  sampleRate: this.sampleRate,
                  getChannelData: (index) => channels[index] ?? channels[0],
                },
                inputBuffer: {
                  numberOfChannels: 2,
                  length: frames,
                  sampleRate: this.sampleRate,
                  getChannelData: () => new Float32Array(frames),
                },
                playbackTime: this.currentTime,
              });
            } catch (error) {
              post({ kind: 'warn', text: `audio callback failed: ${String(error)}` });
            }

            this._callbacks += 1;
            // Measure before deciding to send: a completely silent buffer is the
            // normal state between sounds, and posting it every 20 ms would flood
            // the channel for nothing.
            let peak = 0;
            for (let index = 0; index < frames; index += 16) {
              const sample = channels[0][index];
              const magnitude = sample < 0 ? -sample : sample;
              if (magnitude > peak) peak = magnitude;
            }
            if (peak > this._peak) this._peak = peak;

            if (this._callbacks % 500 === 0) {
              log(
                `## worker: audio ${this._callbacks} callbacks, ${audioFramesSent} frames relayed, ` +
                  `peak ${this._peak.toFixed(4)}`,
              );
            }

            // The engine's mixed output is relayed in real-time blocks by
            // `relayAudio`, not posted here: this callback produces roughly four
            // times real time, and posting it directly would flood the page.
            relayAudio(channels[0], channels[1]);
          }
        }, 20);
        notes.push('AudioContext: PCM relay stub installed');
      }

      createScriptProcessor(frames) {
        const processor = node();
        processor._frames = frames || 1024;
        // Allocated once and reused for the lifetime of the node.
        processor._channels = [new Float32Array(processor._frames), new Float32Array(processor._frames)];
        this._processors.push(processor);
        return processor;
      }

      createGain() {
        return node();
      }

      createBufferSource() {
        return node();
      }

      createBuffer(channels, length, rate) {
        const data = Array.from({ length: channels }, () => new Float32Array(length));
        return {
          numberOfChannels: channels,
          length,
          sampleRate: rate,
          duration: length / rate,
          getChannelData: (index) => data[index],
        };
      }

      decodeAudioData(_buffer, onSuccess) {
        // Without real decoding there is nothing to hand back; failing is honest.
        const error = new Error('decodeAudioData is unavailable in a worker');
        if (typeof onSuccess === 'function') {
          setTimeout(() => onSuccess(null), 0);
          return;
        }
        return Promise.reject(error);
      }

      resume() {
        this.state = 'running';
        return Promise.resolve();
      }

      suspend() {
        this.state = 'suspended';
        return Promise.resolve();
      }

      close() {
        clearInterval(this._timer);
        this.state = 'closed';
        return Promise.resolve();
      }
    }

    stub('AudioContext', SilentAudioContext);
    stub('webkitAudioContext', SilentAudioContext);
  } else {
    notes.push('AudioContext: already present');
  }

  /**
   * Host functions and globals the engine's glue reads by bare name.
   *
   * The shipped page defines all of these, and the glue calls them as identifiers
   * — so `ReferenceError: scale_full is not defined` is what a worker gets without
   * them. They are the page half of the engine: window scaling, video playback,
   * save flushing, and a set of menu toggles that are pure DOM.
   *
   * With no document, most of these are no-ops that keep the engine running. The
   * ones that carry real behaviour forward their intent to the page rather than
   * pretending to implement it here.
   */
  const hostGlobals = {
    // Screen/window state the engine writes and reads.
    g_screen_width: display.width,
    g_screen_height: display.height,
    g_fullstrech: false,
    g_skip: false,
    g_lastpressdown: undefined,
    g_onsyuri_index: null,
    g_onsyuri_module: null,
    g_onsyuri_filemap: {},

    /**
     * Sizes the canvas to the engine's render resolution.
     *
     * The engine calls this from its own initialisation:
     *
     * ```cpp
     * // ONScripter.cpp:295
     * let canvas = document.getElementById("canvas");
     * if(scale_full && canvas) scale_full(canvas, self.g_screen_width/self.g_screen_height);
     * ```
     *
     * Stubbing it as a no-op is what left the game rendering into the canvas's HTML
     * default of 300x150 while the engine believed it was drawing 793x446 — the
     * source of the wrong scaling. The element's CSS size is deliberately not set
     * here: the page owns presentation and fits the canvas to the viewport itself,
     * while this only has to give the drawing the right number of pixels.
     *
     * The ratio argument is not used. It describes what the page should display,
     * and the page computes that from the engine's reported resolution — applying it
     * here as well would scale the image twice.
     */
    scale_full(docobj, ratio) {
      void ratio;
      const target = docobj || self.__onsCanvas || moduleOptions.canvas;
      if (!target) return;
      self.g_screen_width = self.g_screen_width || 0;
      self.g_screen_height = self.g_screen_height || 0;
      post({
        kind: 'log',
        text:
          `## worker: scale_full called (canvas currently ${target.width}x${target.height}, ` +
          `screen ${self.g_screen_width}x${self.g_screen_height})`,
      });
      // Record the object the engine itself resolved, so the correct one is sized
      // even if it is not the one transferred to this worker.
      self.__onsCanvas = target;
    },
    toggle_stretchfull() {
      self.g_fullstrech = !self.g_fullstrech;
    },
    toggle_screenfull() {
      post({ kind: 'ui', action: 'toggleScreenfull' });
    },
    toggle_onsmenu() {
      post({ kind: 'ui', action: 'toggleMenu' });
    },
    toggle_skip() {
      self.g_skip = !self.g_skip;
    },
    toggle_webinfo() {},
    toggle_webmenu() {
      post({ kind: 'ui', action: 'toggleWebMenu' });
    },
    click_export() {},
    click_import() {},
    videoEnded() {},
    /**
     * Video playback needs a DOM `<video>` element, so it is reported to the page
     * instead of faked. Returning without playing is honest; the alternative is a
     * silent no-op that looks like a bug later.
     */
    playVideo(path, clickFlag, loopFlag) {
      post({ kind: 'ui', action: 'playVideo', path, clickFlag, loopFlag });
    },
    wait_video: false,
    /**
     * Persists the save directory. MEMFS keeps saves for the session only, so
     * there is nothing to flush; the name must exist because the engine calls it.
     */
    flush_save() {},
    delay: (ms) => new Promise((resolve) => setTimeout(resolve, ms)),
  };

  for (const [name, value] of Object.entries(hostGlobals)) {
    if (name in self && self[name] !== undefined && self[name] !== null) continue;
    try {
      Object.defineProperty(self, name, { value, writable: true, configurable: true });
    } catch (error) {
      notes.push(`${name}: FAILED ${String(error)}`);
    }
  }

  // Report the outcome, so a failing stub is visible as itself rather than as a
  // confusing error from deep inside the engine.
  post({ kind: 'globals', notes });

  /**
   * Diagnostic trap, enabled with `?trace`.
   *
   * `Cannot set properties of undefined (setting 'cursor')` names the property
   * but not the object, and over minified glue the stack is uninformative. Giving
   * the global scope a setter for `cursor` makes the assignment land here
   * instead, so the stack is captured at the point of failure and identifies the
   * caller.
   */
  if (display.trace) {
    let depth = 0;
    Object.defineProperty(self, 'cursor', {
      configurable: true,
      get() {
        return undefined;
      },
      set(value) {
        if (depth > 0) return;
        depth += 1;
        post({ kind: 'trap', text: `cursor assigned ${String(value)}\n${new Error('trap').stack}` });
      },
    });
  }
}

/**
 * Adds the `style` object Emscripten's glue expects, without wrapping the canvas.
 *
 * Two glue functions set `Module["canvas"].style["cursor"]`:
 *
 * ```js
 * 773728: ($0) => { if (Module["canvas"]) { Module["canvas"].style["cursor"] = UTF8ToString($0) } }
 * 773811: () => { if (Module["canvas"]) { Module["canvas"].style["cursor"] = "none" } }
 * ```
 *
 * An `OffscreenCanvas` has no `style`, so those throw. The first attempt at this
 * returned a `Proxy` that supplied one — which fixed the cursor and introduced a
 * worse bug: SDL sizes `Module.canvas` during initialisation, the writes landed on
 * the proxy, the real `OffscreenCanvas` kept the HTML default of 300x150, and
 * every mouse coordinate was then scaled against that. `width` and `height` are
 * read-only on a native `OffscreenCanvas`, but arbitrary *new* properties are not,
 * so `style` is attached to the real object and the canvas is handed over
 * unwrapped. Anything the engine gets to see must be the genuine object.
 */
function withStyleProperty(canvas) {
  if (!canvas || canvas.style) return canvas;
  try {
    Object.defineProperty(canvas, 'style', {
      value: { cursor: 'auto' },
      writable: true,
      configurable: true,
    });
  } catch (error) {
    post({ kind: 'warn', text: `could not add .style to the canvas: ${String(error)}` });
  }
  return canvas;
}

/* ------------------------------------------------------------------ */
/* Boot                                                                */
/* ------------------------------------------------------------------ */

self.addEventListener('message', (event) => {
  const data = event.data || {};
  if (data.kind === 'init') void boot(data);
  if (data.kind === 'probe') probeFileSystem();
  if (data.kind === 'readMouseState') {
    const module = self.g_onsyuri_module;
    if (module && typeof module._ons_host_mouse_state === 'function') {
      const packed = module._ons_host_mouse_state();
      post({ kind: 'mouseState', packed, x: packed & 0xffff, y: (packed >> 16) & 0xffff });
    } else {
      post({ kind: 'mouseState', packed: null, unavailable: true });
    }
  }
  if (data.kind === 'pointerProbe') {
    // Runs the probe on request as well, for when the host wants a fresh reading after
    // interacting with the game. The startup probe is the reliable one — see the timer in
    // `boot` — because this thread is usually busy inside the engine's loop.
    runPointerProbe(data.cols ?? 8, data.rows ?? 6);
  }
});

/**
 * Reads the engine's own hit test and posts the result.
 *
 * Gives the host the ground truth that the asset-traffic proxy could not: where the
 * engine thinks the pointer is (`current_button_state`, which the patched
 * `refreshMouseOverButton` feeds to `mouseOverCheck`) and which button it resolves at
 * each point of a grid.
 *
 * The buffer is read through `Module.getValue` because `_malloc`/`HEAP32` are not in this
 * build's `EXPORTED_RUNTIME_METHODS`, so the host cannot pass one in. Its address is
 * stable, so it is fetched once.
 */
function runPointerProbe(cols, rows) {
  // Logged on entry so "the probe was never called" is distinguishable from "it ran and
  // produced nothing" — the two need different fixes.
  log(`## worker: pointer probe invoked (${cols}x${rows})`);
  const module = self.g_onsyuri_module;
  const sweep = module && module._ons_host_pointer_sweep;
  if (typeof sweep !== 'function') {
    post({
      kind: 'pointerProbe',
      unavailable: true,
      hasModule: !!module,
      sweepType: typeof sweep,
      moduleKeys: module ? Object.keys(module).filter((key) => key.startsWith('_ons')).slice(0, 8) : [],
    });
    return;
  }
  // The result arrives as the function's **return value**, not from memory.
  //
  // Reading a buffer is not possible in this build: `getValue` belongs to
  // `EXPORTED_RUNTIME_METHODS`, whose flag has the same replace-not-merge hazard as the
  // function export list (adding it produced no change), and the `HEAP*` views are not
  // attached to `Module` either. An integer return crosses the boundary with no export
  // list involved, which is why the engine packs the summary instead of writing it.
  const packed = sweep(cols, rows);

  const button = (packed & 0xffff) - 1;
  const down = (packed >> 16) & 1;
  const hits = (packed >> 17) & 0x3f;
  const mask = (packed >>> 23) & 0x1ff;

  post({
    kind: 'pointerProbe',
    packed,
    cols,
    rows,
    pointer: { button, down },
    hits,
    cells: cols * rows,
    // Which grid cells resolved to a button, as a row-major bitmap for the first nine.
    maskBits: Array.from({ length: Math.min(9, cols * rows) }, (_, i) => (mask >> i) & 1),
  });
}

/**
 * Reports which lazy-file mechanisms the build actually provides.
 *
 * `FS.createLazyFile` is the natural fit: it creates a real file node whose reads
 * are satisfied by synchronous ranged requests, so the engine can seek and read
 * normally and only the bytes it touches cross the wire. It depends on the
 * lazy-file plugin, which is compiled in only when the build passed
 * `--use-preload-plugins`, so its availability has to be checked rather than
 * assumed. `FS.createPreloadedFile` is the fallback, and it materialises the whole
 * file — which is exactly the behaviour this work exists to remove.
 */
function probeFileSystem() {
  const FS = self.__onsFs;
  if (!FS) {
    post({ kind: 'probeResult', text: 'FS not captured yet' });
    return;
  }
  const has = (name) => typeof FS[name] === 'function';
  post({
    kind: 'probeResult',
    text:
      `createLazyFile=${has('createLazyFile')} ` +
      `createPreloadedFile=${has('createPreloadedFile')} ` +
      `createDataFile=${has('createDataFile')} ` +
      `registerDevice=${has('registerDevice')} ` +
      `analyzePath=${has('analyzePath')} ` +
      `open=${has('open')} write=${has('write')} mount=${has('mount')}`,
  });
}

async function boot(options) {
  installWorkerGlobals(
    Object.assign(
      { width: 1280, height: 720, devicePixelRatio: 1, trace: false },
      options.display || {},
    ),
  );

  let index;
  try {
    const response = await fetch(options.indexUrl);
    index = await response.json();
  } catch (error) {
    post({ kind: 'error', text: `could not load index: ${String(error)}` });
    return;
  }

  for (const entry of index.files) {
    files.set(entry.path.replace(/\\/g, '/').toLowerCase(), {
      path: entry.path.replace(/\\/g, '/'),
      url: new URL(entry.url, self.location.href).href,
      size: entry.size,
    });
  }

  const gamedir = index.gamedir;
  const savedir = index.savedir;
  log(`## worker: ${files.size} files registered`);
  log(
    `## worker: sizes ` +
      [...files.values()].map((file) => `${file.path}=${file.size}`).join(' '),
  );

  importScripts(options.wasmJsUrl);
  if (typeof onsyuri !== 'function') {
    post({ kind: 'error', text: 'onsyuri module did not load' });
    return;
  }

  const moduleOptions = {
    canvas: withStyleProperty(options.canvas),
    print: (text) => log(String(text)),
    printErr: (text) => post({ kind: 'stderr', text: String(text) }),
    locateFile: (path) => new URL(path, options.engineBase).href,
    onAbort: (reason) => post({ kind: 'error', text: `engine abort: ${String(reason)}` }),
  };

  moduleOptions.preRun = [() => prepareFileSystem(moduleOptions, index, gamedir, savedir)];

  // Publish the globals the engine's embedded library reads. They are referenced
  // as bare identifiers, so they have to exist on the global scope, not merely be
  // reachable — and `fetch_file` must be synchronous, because the engine calls it
  // from inside a file-open.
  self.g_onsyuri_module = moduleOptions;
  self.g_onsyuri_index = index;
  self.g_onsyuri_filemap = {};
  self.fetch_file = fetchFile;

  try {
    await onsyuri(moduleOptions);
  } catch (error) {
    post({
      kind: 'error',
      text: `module failed: ${String(error && error.stack ? error.stack : error)}`,
    });
    return;
  }

  post({ kind: 'ready' });
  log('## worker: module initialised');

  // Report whether this build carries the injection shim. Without this line a
  // missing shim is invisible: input simply does nothing, and there is no error to
  // notice because the DOM path fails silently by construction.
  const hasShimFn = typeof moduleOptions._ons_host_shim_present === 'function';
  const shimOn = hasShimFn ? moduleOptions._ons_host_shim_present() : 0;
  log(
    `## worker: injection shim ${shimOn ? 'ACTIVE' : 'ABSENT'} ` +
      `(fn=${hasShimFn}, present=${shimOn}, keys=${
        ['_ons_host_push_key', '_ons_host_push_mouse', '_ons_host_wait_event']
          .map((name) => `${name}:${typeof moduleOptions[name]}`)
          .join(' ')
      })`,
  );

  inspectEventRegistry();

  const args = ['--root', gamedir, '--font', `${gamedir}/default.ttf`, '--save-dir', savedir];
  log(`## worker: args ${JSON.stringify(args)}`);
  watchCanvasResolution(options.canvas, stdout);
  log(`## worker: listener registrations ${JSON.stringify(self.__onsListenerCounts)}`);
  log(`## worker: last listener ${JSON.stringify(self.__onsLastListener ?? null)}`);
  log(`## worker: on* handlers assigned ${JSON.stringify(self.__onsHandlers)}`);

  // The engine skips its own `main` (built with `-sINVOKE_RUN=0`), so the host
  // runs it. Keep the loop alive after it returns, as the shipped page does.
  // Probe repeatedly *while the game runs*, not after it has exited.
  //
  // The first working reading came back with `hits: 0/48` and `button = -1` — no button
  // anywhere in the grid and no interaction ever recorded — which is what a reading taken
  // after `callMain` returns looks like: the buttons and the pointer state exist only while
  // the script is live.
  //
  // `callMain` occupies this thread, but the shim's `ons_host_wait_event` yields through
  // `emscripten_sleep(0)` whenever nothing is queued, so a timer registered *before* it
  // runs during those yields. A repeating timer therefore samples the running game, and the
  // host keeps the last reading that actually found something.
  const probeTimer = setInterval(() => {
    try {
      runPointerProbe(12, 8);
    } catch (error) {
      post({ kind: 'warn', text: `pointer probe failed: ${String(error)}` });
      clearInterval(probeTimer);
    }
  }, 2000);

  try {
    moduleOptions.callMain(args);
  } catch (error) {
    post({ kind: 'error', text: `callMain failed: ${String(error)}` });
    return;
  } finally {
    clearInterval(probeTimer);
  }

  post({ kind: 'exited', rangesServed: counters.ranges, bytesServed: counters.bytes });
  // Flush per-archive lazy statistics, so a savings claim is attributable to a
  // specific file rather than only to the aggregate.
  for (const report of self.__lazyReports || []) report();
}

/**
 * Files above this size are registered as Emscripten lazy files instead of being
 * materialised through `fetch_file`.
 *
 * The two archives are 1.8 GB and 1.2 GB, and the engine reads them in full —
 * measuring 2.9 GB of transfer with every byte written into the heap. A lazy file
 * keeps the bytes on the server: the node reports the right size, and each read
 * becomes a ranged request that fills the requested window and nothing else.
 *
 * The scripts and the font stay on the eager path. They are small, and they are
 * read start-to-finish during start-up, so windows would add requests without
 * saving anything.
 */
const LAZY_FILE_THRESHOLD = 16 * 1024 * 1024;

/**
 * Replaces a lazy node's read path with one that sends a `Range` header.
 *
 * `FS.createLazyFile` gets the *timing* right — the file is not fetched until it
 * is opened — but not the *amount*: its loader issues a plain `XMLHttpRequest`
 * with no `Range` header, so the server answers `200` with the entire body and the
 * plugin writes all of it into the heap. Its own error text names the design:
 *
 * > Lazy loading should have been performed (contents set) in createLazyFile, but
 * > it was not. Lazy loading only works in web workers.
 *
 * For a 1.8 GB archive that means the whole archive arrives and is copied, which
 * is why the transfer report showed 1.87 GB for a file of 1.866 GB.
 *
 * Emscripten dispatches file operations through `stream_ops`, so installing a
 * `read` that fetches only the requested window is enough: the engine keeps
 * seeking and reading normally, and each read costs exactly the bytes it asked
 * for. `llseek` must be provided too, because the engine seeks before reading and
 * would otherwise fall back to a stock implementation whose position tracking does
 * not match this node.
 */
function installRangedRead(FS, node, path, url, declaredSize) {
  const size = declaredSize > 0 ? declaredSize : FS.stat(path).size;
  let served = 0;
  let requests = 0;

  node.stream_ops = {
    llseek(stream, offset, whence) {
      let position;
      if (whence === 0) position = offset;
      else if (whence === 1) position = (stream.position ?? 0) + offset;
      else if (whence === 2) position = size + offset;
      else return -28; // EINVAL
      stream.position = position;
      return position;
    },

    read(stream, buffer, offset, length, position) {
      const at = position === undefined || position === null ? (stream.position ?? 0) : position;
      const want = Math.min(length, Math.max(0, size - at));
      if (want <= 0) return 0;
      let bytes;
      try {
        bytes = rangeRequest(url, at, want);
      } catch (error) {
        post({ kind: 'warn', text: `lazy read failed at ${at}: ${String(error)}` });
        return -5; // EIO
      }
      const count = Math.min(bytes.byteLength, want);
      buffer.set(bytes.subarray(0, count), offset);
      if (position === undefined || position === null) stream.position = (stream.position ?? 0) + count;
      served += count;
      requests += 1;
      return count;
    },

    write: () => -5, // read-only
    close: () => 0,
    // The plugin installs its own `mmap`; removing it forces the engine through
    // `read`, which is the path that ranges.
    mmap: undefined,
  };

  // Report on first read, so the effect is visible per archive rather than only in
  // the aggregate transfer report.
  const report = () => {
    log(`## worker: lazy ${path} served ${(served / 1024 / 1024).toFixed(2)} MB in ${requests} range reads`);
  };
  self.__lazyReports = self.__lazyReports || [];
  self.__lazyReports.push(report);
}

/**
 * Reports what Emscripten's event registry actually contains.
 *
 * `addEventListener` wrappers on the stub document and canvas were never called,
 * which means the engine registered somewhere else. Emscripten keeps its own
 * table (`JSEvents.eventHandlers`) and resolves targets through
 * `specialHTMLTargets`, so reading that table identifies the real target instead
 * of guessing at it — and `specialHTMLTargets` matters especially because it is
 * built once, when the glue loads, from `globalThis.document ?? 0`.
 */
function inspectEventRegistry() {
  try {
    if (typeof JSEvents === 'undefined') {
      post({ kind: 'warn', text: 'JSEvents is not a global; cannot inspect' });
      return;
    }
    const handlers = (JSEvents.eventHandlers || []).map((entry) => ({
      event: entry.eventTypeString,
      target: (() => {
        const target = entry.target;
        if (target === self) return '#self';
        if (target === self.__onsInputTargets?.document) return '#documentStub';
        if (target === self.__onsInputTargets?.canvas) return '#canvasStub';
        if (target === self.document) return '#globalDocument';
        return target === undefined ? 'undefined' : String(target && target.constructor && target.constructor.name);
      })(),
      hasTarget: !!entry.target,
    }));
    post({
      kind: 'registry',
      text:
        `handlers=${handlers.length} ` +
        JSON.stringify(handlers.slice(0, 12)) +
        ` | canvasStubIsModuleCanvas=${self.__onsInputTargets?.canvas === self.g_onsyuri_module?.canvas}` +
        ` | documentIsGlobal=${self.document === self.__onsInputTargets?.document}`,
    });
  } catch (error) {
    post({ kind: 'warn', text: `registry inspection failed: ${String(error)}` });
  }
}

/**
 * Creates the game and save directories.
 *
 * Small files are declared in `g_onsyuri_filemap`, which the engine's own hook
 * consults when it opens a path — that is where {@link fetchFile} materialises
 * them. Large files instead get a lazy node from Emscripten, so the engine reads
 * them without the host ever holding them.
 */
function prepareFileSystem(moduleOptions, index, gamedir, savedir) {
  const FS = moduleOptions.FS;
  if (!FS) {
    post({ kind: 'error', text: 'FS unavailable; the build needs -sFORCE_FILESYSTEM' });
    return;
  }
  // Kept for the probe, which the page can request at any time.
  self.__onsFs = FS;

  const mkdirp = (path) => {
    let current = '';
    for (const part of path.split('/').filter(Boolean)) {
      current += `/${part}`;
      try {
        FS.mkdir(current);
      } catch {
        // already present
      }
    }
  };

  mkdirp(gamedir);
  mkdirp(savedir);

  // Saves go to MEMFS: persistent storage would need IDBFS, and a save directory
  // is small and not what this harness is about.
  try {
    FS.mount(moduleOptions.MEMFS, {}, savedir);
  } catch {
    // already mounted
  }

  const lazyAvailable = typeof FS.createLazyFile === 'function';
  let declared = 0;
  let lazy = 0;

  for (const record of files.values()) {
    const full = `${gamedir}/${record.path}`.replace(/\\/g, '/');
    const slash = full.lastIndexOf('/');
    mkdirp(full.slice(0, slash));
    const key = full.toLowerCase();
    if (self.g_onsyuri_filemap[key]) continue;

    if (lazyAvailable && record.size > LAZY_FILE_THRESHOLD) {
      // A lazy node: the engine sees a file of the declared size, and reads are
      // served as ranges — but only after {@link installRangedRead} replaces the
      // plugin's own read path.
      try {
        const node = FS.createLazyFile(
          full.slice(0, slash),
          full.slice(slash + 1),
          record.url,
          true,
          false,
        );
        installRangedRead(FS, node, full, record.url, record.size);
        lazy += 1;
        continue;
      } catch (error) {
        post({ kind: 'warn', text: `createLazyFile failed for ${record.path}: ${String(error)}` });
      }
    }

    self.g_onsyuri_filemap[key] = { url: record.url, loaded: false };
    declared += 1;
  }

  log(
    `## worker: ${declared} eager files, ${lazy} lazy files (threshold ` +
      `${(LAZY_FILE_THRESHOLD / 1024 / 1024).toFixed(0)} MB) under ${gamedir}`,
  );
}

/**
 * Materialises one file into the Emscripten filesystem.
 *
 * Called by the engine's own library when it opens a path it has a `filemap`
 * entry for. Must be synchronous, and must leave the whole file present, because
 * the engine may then seek anywhere within it — so the range requests happen here,
 * in windows, rather than in one allocation.
 */
function fetchFile(FS, path, filemap) {
  const key = String(path).toLowerCase();
  const node = filemap[key];
  if (!node) return;

  const record = files.get(key.replace(/^.*?\/onscript\//, ''));
  const url = node.url;
  const gamedir = self.g_onsyuri_index ? self.g_onsyuri_index.gamedir : '';

  // Write in windows so peak allocation is one window, not the file.
  const WINDOW = 8 * 1024 * 1024;
  let offset = 0;
  let stream = null;
  let total = 0;

  // The size is known from the index, so the loop bound does not depend on
  // discovering the end.
  const declared = record ? record.size : -1;
  if (declared < 0) {
    // Unknown size: fall back to one request and take what comes back.
    const bytes = rangeRequest(url, 0, Number.MAX_SAFE_INTEGER);
    total = bytes.byteLength;
    stream = FS.open(path, 'w+');
    FS.write(stream, bytes, 0, total, 0);
    FS.close(stream);
    node.loaded = true;
    counters.files += 1;
    return;
  }

  stream = FS.open(path, 'w+');
  while (offset < declared) {
    const length = Math.min(WINDOW, declared - offset);
    const bytes = rangeRequest(url, offset, length);
    if (bytes.byteLength === 0) break;
    FS.write(stream, bytes, 0, bytes.byteLength, offset);
    offset += bytes.byteLength;
    total += bytes.byteLength;
    if (bytes.byteLength < length) break;
  }
  FS.close(stream);
  node.loaded = true;
  counters.files += 1;
  log(`## worker: fetched ${path} (${(total / 1024 / 1024).toFixed(2)} MB in ranges)`);
}

/* ------------------------------------------------------------------ */
/* Input                                                               */
/* ------------------------------------------------------------------ */

/**
 * Delivers a forwarded input event into the engine.
 *
 * The engine does not read input itself — SDL registers DOM listeners through
 * Emscripten's event helpers, which are attached to `specialHTMLTargets[0]` (the
 * document) and to `Module.canvas`. In a worker there is no browser to fire
 * those, so the host's forwarded event is turned back into a real `Event` and
 * dispatched at both targets. SDL's registered listeners then run exactly as they
 * would on a page, which is why no engine-side glue is needed.
 *
 * The synthetic event needs the fields SDL reads — coordinates, button state,
 * modifier keys, `timeStamp` — and must be cancelable, because SDL checks
 * `defaultPrevented` to decide whether the page consumed the input.
 */
/**
 * Input via the host shim, which is the path that actually works.
 *
 * The engine's own input arrives through SDL, and SDL's web backend registers its
 * listeners with Emscripten's DOM helpers — which, in a worker, find no target.
 * Measured directly: zero registrations on all four possible targets and on the
 * `on<event>` property path, so DOM dispatch reaches nobody no matter how the
 * events are built.
 *
 * This build instead replaces the engine's `SDL_WaitEvent` with a host function and
 * exports `ons_host_push_*`, so the host hands events straight to the engine. No
 * DOM, no listeners — which is why it works where dispatch could not.
 *
 * SDL event type values are used numerically rather than imported: they are part of
 * SDL's stable ABI (1025/1026/1024 for mouse down/up/motion, 768/769 for key
 * down/up) and the host has no SDL headers.
 */
const SHIM_EVENTS = {
  SDL_KEYDOWN: 768,
  SDL_KEYUP: 769,
  SDL_MOUSEMOTION: 1024,
  SDL_MOUSEBUTTONDOWN: 1025,
  SDL_MOUSEBUTTONUP: 1026,
};

/** SDL button indices: 1 = left, 2 = middle, 3 = right. */
function sdlButton(domButton) {
  if (domButton === 0) return 1;
  if (domButton === 1) return 2;
  if (domButton === 2) return 3;
  return 1;
}

/**
 * Best-effort SDL keycode for a DOM key.
 *
 * SDL keycodes are `SDLK_*` values; for the printable ASCII range SDL uses the
 * character's own code, which covers the keys a visual novel reads (Enter, Space,
 * Escape, arrows, and letters). Anything unmapped becomes 0, which SDL treats as
 * "unknown" rather than as a wrong key — preferable to inventing a value.
 */
function sdlKeycode(key, code) {
  const NAMED = {
    Enter: 13,
    Escape: 27,
    ' ': 32,
    Backspace: 8,
    Tab: 9,
    ArrowRight: 1073741903,
    ArrowLeft: 1073741904,
    ArrowDown: 1073741905,
    ArrowUp: 1073741906,
    Control: 1073742048,
    Shift: 1073742049,
    F1: 1073741882,
    F2: 1073741883,
    F3: 1073741884,
    F4: 1073741885,
    F5: 1073741886,
    F9: 1073741890,
    F10: 1073741891,
    F11: 1073741892,
  };
  if (key && key.length === 1) return key.charCodeAt(0);
  if (NAMED[key] !== undefined) return NAMED[key];
  if (code && /^Key([A-Z])$/.test(code)) return code.charCodeAt(3) + 32;
  if (code && /^Digit(\d)$/.test(code)) return code.charCodeAt(5);
  return 0;
}

/** Returns true when the build carries the injection shim. */
function shimAvailable() {
  const module = self.g_onsyuri_module;
  return !!(module && typeof module._ons_host_shim_present === 'function' && module._ons_host_shim_present());
}

/**
 * The screen size the engine's script thinks in, from its own banner.
 *
 * The engine hit-tests in this space, not in canvas pixels — see `screenScale`.
 */
let engineScreen = { width: 793, height: 446 };

/**
 * Scale from canvas device space to the engine's screen space.
 *
 * The engine's own arithmetic states the relationship:
 *
 * ```cpp
 * mx = mx * screen_scale_ratio1;   // screen_width / screen_device_width
 * mouseOverCheck( mx, my );
 * ```
 *
 * `screen_device_*` is the surface the canvas shows (1189x669 here) and `screen_*` is what
 * the script thinks in (`Display: 793 x 446`). So the hit test compares against **screen**
 * coordinates while the host forwards canvas pixels, and without converting, the click
 * lands elsewhere — measured as `hits: 1/96` with the single hit never where the click was,
 * which is what a constant factor error looks like. Here the ratio is 1189/793 = 1.5.
 */
function screenScale() {
  const module = self.g_onsyuri_module;
  const deviceWidth = module && module.canvas && module.canvas.width ? module.canvas.width : 0;
  if (deviceWidth > 0 && engineScreen.width > 0) return deviceWidth / engineScreen.width;
  return 1.5;
}

/**
 * Delivers one host event through the shim.
 *
 * Canvas-pixel coordinates arrive already scaled by `host.mjs`, which is what the
 * engine works in.
 */
function pushShimInput(data) {
  const module = self.g_onsyuri_module;
  if (!module) return false;

  switch (data.type) {
    case 'keydown':
    case 'keyup': {
      const pressed = data.type === 'keydown' ? 1 : 0;
      const keycode = sdlKeycode(data.key, data.code);
      module._ons_host_push_key(pressed, 0, keycode, 0);
      return true;
    }
    case 'mousedown':
    case 'mouseup':
    case 'mousemove': {
      const type =
        data.type === 'mousedown'
          ? SHIM_EVENTS.SDL_MOUSEBUTTONDOWN
          : data.type === 'mouseup'
            ? SHIM_EVENTS.SDL_MOUSEBUTTONUP
            : SHIM_EVENTS.SDL_MOUSEMOTION;
      // Convert canvas pixels into the engine's **screen** space.
      //
      // The `WEB` build is special, and reading the engine's source settles it:
      // `runEventLoop` skips the coordinate conversion that other platforms perform —
      //
      // ```cpp
      // case SDL_MOUSEMOTION:
      // #if !defined(WEB)
      //     event.button.x = (event.button.x - render_view_rect.x) * screen_scale_ratio1;
      // #endif
      // ```
      //
      // — and `mousePressEvent` then copies the coordinates through unscaled:
      //
      // ```cpp
      // current_button_state.x = event->x;
      // current_button_state.y = event->y;
      // ```
      //
      // while `mouseOverCheck` compares against screen space (793x446). Since SDL on other
      // platforms supplies window/device pixels and the engine converts them down, a `WEB`
      // build expects the *host* to have done that: screen coordinates, i.e. canvas pixels
      // divided by `screen_scale_ratio` (1189/793 = 1.5).
      //
      // Forwarding canvas pixels unconverted is what left every click 1.5x too far right
      // and too low — the position offset that was still visible.
      const toScreen = screenScale();
      const sx = Math.round((data.x ?? 0) / toScreen);
      const sy = Math.round((data.y ?? 0) / toScreen);
      module._ons_host_push_mouse(type, sx, sy, sdlButton(data.button));
      return true;
    }
    default:
      // Wheel and touch have no shim constructor yet; they fall through to the DOM
      // path, which is inert but harmless.
      return false;
  }
}

function dispatchInput(data) {
  // The shim is tried first, and for input that only ever reached nobody before it
  // is the whole point of building from source.
  if (shimAvailable() && pushShimInput(data)) {
    counters.shimInputs = (counters.shimInputs || 0) + 1;
    // Log the first few with their coordinates: the engine's hit test reads
    // `SDL_GetMouseState`, which a pushed event does not update, so whether input
    // lands correctly is decided by the coordinate space these arrive in.
    if (counters.shimInputs <= 6) {
      const where = data.x !== undefined ? ` at (${Math.round(data.x)},${Math.round(data.y)})` : '';
      log(`## worker: input ${data.type}${where} injected via shim (${counters.shimInputs} total)`);
    } else if (counters.shimInputs % 100 === 0) {
      log(`## worker: input ${data.type} injected via shim (${counters.shimInputs} total)`);
    }
    return;
  }

  const targets = self.__onsInputTargets;
  if (!targets) return;

  const common = {
    bubbles: true,
    cancelable: true,
    composed: true,
    // SDL measures click duration against this, so a long-press right-click menu
    // depends on it being plausible.
    timeStamp: typeof performance !== 'undefined' ? performance.now() : Date.now(),
  };

  /**
   * Builds the event from a plain `Event` plus own properties.
   *
   * `KeyboardEvent`, `MouseEvent` and `WheelEvent` constructors do not exist in a
   * worker — they belong to the window environment — so they cannot be
   * instantiated here. SDL and Emscripten only read the event's fields, so an
   * `Event` carrying the same properties behaves identically for their purposes,
   * and `type`/`cancelable` still come from the real constructor.
   */
  const makeEvent = (type, fields) => {
    const event = new Event(type, common);
    for (const [key, value] of Object.entries(fields)) {
      Object.defineProperty(event, key, { value, writable: false, configurable: true });
    }
    // SDL checks both of these to map keys to scancodes.
    if (fields.keyCode !== undefined && fields.which === undefined) {
      Object.defineProperty(event, 'which', { value: fields.keyCode, configurable: true });
    }
    return event;
  };

  let event;
  switch (data.type) {
    case 'keydown':
    case 'keyup':
      event = makeEvent(data.type, {
        key: data.key ?? '',
        code: data.code ?? '',
        keyCode: data.keyCode ?? 0,
        which: data.keyCode ?? 0,
        shiftKey: !!data.shiftKey,
        ctrlKey: !!data.ctrlKey,
        altKey: !!data.altKey,
        metaKey: !!data.metaKey,
        repeat: false,
      });
      break;
    case 'mousedown':
    case 'mouseup':
    case 'mousemove':
      event = makeEvent(data.type, {
        clientX: data.x ?? 0,
        clientY: data.y ?? 0,
        screenX: data.x ?? 0,
        screenY: data.y ?? 0,
        pageX: data.x ?? 0,
        pageY: data.y ?? 0,
        offsetX: data.x ?? 0,
        offsetY: data.y ?? 0,
        button: data.button ?? 0,
        buttons: data.type === 'mouseup' ? 0 : 1,
        detail: 1,
      });
      break;
    case 'wheel':
      event = makeEvent('wheel', {
        clientX: data.x ?? 0,
        clientY: data.y ?? 0,
        deltaX: 0,
        deltaY: data.deltaY ?? 0,
        deltaZ: 0,
        deltaMode: 0,
      });
      break;
    case 'touchstart':
    case 'touchend':
    case 'touchmove': {
      // `Touch` and `TouchEvent` are window constructors too, so the touch lists
      // are attached as plain arrays of structurally compatible objects.
      const points = (data.touches || []).map((point, index) => ({
        identifier: index,
        clientX: point.x,
        clientY: point.y,
        screenX: point.x,
        screenY: point.y,
        pageX: point.x,
        pageY: point.y,
        target: targets.canvas,
      }));
      event = makeEvent(data.type, {
        touches: points,
        changedTouches: points,
        targetTouches: points,
      });
      break;
    }
    default:
      return;
  }

  // Both targets are tried: Emscripten attaches to the document, while SDL's own
  // canvas handlers attach to the canvas, and which one is used varies by event.
  let delivered = 0;
  for (const target of [targets.document, targets.canvas]) {
    try {
      target.dispatchEvent(event);
      delivered += 1;
    } catch (error) {
      post({ kind: 'warn', text: `input dispatch failed for ${data.type}: ${String(error)}` });
    }
  }
  counters.inputEvents = (counters.inputEvents || 0) + 1;
  if (counters.inputEvents <= 3 || counters.inputEvents % 50 === 0) {
    log(`## worker: input ${data.type} delivered to ${delivered} target(s)`);
  }
}

self.addEventListener('message', (event) => {
  const data = event.data || {};
  if (data.kind !== 'input') return;
  dispatchInput(data);
});
