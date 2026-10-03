import { ArtemisHostError } from './errno.js';
import type { ArtemisMemory } from './memory.js';

/**
 * The 19 `_al*` / `_alc*` rows of the `gl` bucket, implemented against the page's WebAudio.
 *
 * `gl.ts` has carried these nineteen rows since it landed, as *documented refusals*: nineteen short
 * names answering OpenAL's own "no device", "no context" values. That was the honest answer for a
 * bucket with no audio, and it stays the right answer for a session that supplies none. This module
 * is the other half — a session that wants the game's music and effects says so by handing
 * {@link createArtemisAudioHost} the same memory the bridge was instantiated with, and gl.ts stops
 * being asked.
 *
 * **This is a reimplementation, not a port.** The reference is the official glue's own `AL` object
 * (`ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.js`, `var AL={` at byte 133***);
 * nothing here evaluates, embeds or imports that file. The guest-visible contract — ids, error
 * codes, enum values, the buffer/source state machine — is copied from it because the guest was
 * compiled against it; the mechanism is not. Two deliberate departures, both named in the file:
 *
 *  - **Exactly one WebAudio `AudioContext`, created at `_alcCreateContext` and not before.** The
 *    official glue builds a per-context gain node graph and sweeps sources from a 25 ms
 *    `setInterval`; this host has one context, one `_alSourcePlay` → `onended` pump, and no timer.
 *  - **An explicit free list instead of a no-reuse id counter.** The glue's `AL.newId()` pops
 *    deleted ids and reuses them, so a device/source/buffer id can name a *different* object after
 *    a delete. Ids here are monotonic within a family and never reused.
 *
 * **What this cannot honestly claim.** OpenAL is larger than these nineteen rows: listener
 * property, doppler, capture, distance-model and `_alBufferf`/`_alBuffer3f` calls are imports this
 * build does not have, so a guest can never reach them and neither can this file. Within the
 * nineteen, `AL_GAIN` is one gain node applied *after* the source's own gain rather than folded
 * into it, so a node inspector reads two gains where OpenAL has one; and `MIN_GAIN`/`MAX_GAIN` are
 * range-checked and stored but not applied to a linear ramp. `DOUBLE` buffers (float32) are accepted
 * because the guest's build may call them, but they are converted through the same `Float32Array`
 * path as `SHORT`, which is exactly right for a float input and lossy for nothing.
 */

/* -------------------------------------------------------------------------- */
/* The ABI                                                                     */
/* -------------------------------------------------------------------------- */

/**
 * The import table, keyed by the wasm short name.
 *
 * Named separately from `ArtemisGlImports` on purpose: this table is *dropped in where gl.ts's
 * version of these nineteen names was*, and the only thing that makes it safe to do so is that the
 * two types are structurally identical. Spelling that out here is what lets a caller mix them
 * without a cast.
 */
export type ArtemisAudioImports = Record<string, (...args: number[]) => number | void>;

/** What a session supplies. `memory` and `malloc` are the two it cannot do without. */
export interface ArtemisAudioHostOptions {
  /** The linear memory the bridge was instantiated with. Every pointer argument indexes it. */
  readonly memory: ArtemisMemory;
  /**
   * The guest's allocator, for the strings `_alGetString` hands back as pointers.
   *
   * Required, not optional, for the same reason the event blocks in `gl.ts` require one: a pointer
   * the guest did not allocate is a pointer nothing frees. A host without `malloc` cannot serve
   * `_alGetString` at all, and the honest answer there is the refusal in `gl.ts`, not a fabricated
   * address.
   */
  readonly malloc: (bytes: number) => number;
  /**
   * Builds the `AudioContext`. Optional so a host can supply its own (an `OfflineAudioContext` for
   * a test, a pre-created context for a page that made one at first user gesture).
   *
   * Absent, the host falls back to `globalThis.AudioContext ?? globalThis.webkitAudioContext`, and
   * if neither exists `_alcOpenDevice` reports **0 devices** and `_alcCreateContext` **0 contexts**
   * with a diagnostic naming the reason. A silent fake device is the one answer this file refuses:
   * the engine would then play music into nothing and never know it had no audio.
   */
  readonly contextFactory?: () => AudioContext;
  /** Reports a call this host declined or could not serve. A throwing sink is swallowed. */
  readonly onBlocked?: (what: string, detail: string) => void;
}

/** The lifecycle a session drives. Deliberately three methods and a disposable flag, no state. */
export interface ArtemisAudioHost {
  /** The nineteen rows, ready to spread over gl.ts's table. */
  readonly imports: ArtemisAudioImports;
  /** Suspend the context. A no-op before `_alcCreateContext`, and never sets OpenAL's pause state. */
  pause(): Promise<void>;
  /** Resume the context. Never creates one — there is nothing to resume before a context exists. */
  resume(): Promise<void>;
  /** Idempotent teardown: stop every node, close the context once, drop every id. */
  dispose(): Promise<void>;
}

/* -------------------------------------------------------------------------- */
/* Frozen constants — OpenAL's own values, from the glue's AL object            */
/* -------------------------------------------------------------------------- */

/** The five error codes the glue can latch. This host latching anything else would be a defect. */
const AL_NO_ERROR = 0;
const AL_INVALID_NAME = 40961;
const AL_INVALID_ENUM = 40962;
const AL_INVALID_VALUE = 40963;
const AL_INVALID_OPERATION = 40964;
const AL_OUT_OF_MEMORY = 40965;

/** `ALC_FREQUENCY`. `4103` is also `AL_LOOPING`; OpenAL reuses the number across the two APIs. */
const ALC_FREQUENCY = 4103;

/** The seven source states, plus the three types `AL_SOURCE_TYPE` can report. */
const AL_INITIAL = 4113;
const AL_PLAYING = 4114;
const AL_PAUSED = 4115;
const AL_STOPPED = 4116;
const AL_STATIC = 4136;
const AL_STREAMING = 4137;
const AL_UNDETERMINED = 4144;

/** Source parameters. The list is short because it is the list the guest's build can actually send. */
const AL_SOURCE_RELATIVE = 514;
const AL_PITCH = 4099;
const AL_POSITION = 4100;
const AL_DIRECTION = 4101;
const AL_VELOCITY = 4102;
const AL_LOOPING = 4103;
const AL_BUFFER = 4105;
const AL_GAIN = 4106;
const AL_MIN_GAIN = 4109;
const AL_MAX_GAIN = 4110;
const AL_REFERENCE_DISTANCE = 4128;
const AL_ROLLOFF_FACTOR = 4129;
const AL_MAX_DISTANCE = 4131;
const AL_SEC_OFFSET = 4132;
const AL_SAMPLE_OFFSET = 4133;
const AL_BYTE_OFFSET = 4134;
const AL_SOURCE_STATE = 4112;
const AL_BUFFERS_QUEUED = 4117;
const AL_BUFFERS_PROCESSED = 4118;
const AL_SOURCE_TYPE = 4135;

/**
 * The `_alSourcei` parameters, exactly as the glue's switch lists them.
 *
 * A parameter *outside* this set is `AL_INVALID_ENUM`; a parameter inside it that this host does not
 * honour is reported and accepted. Keeping the set complete is what makes the first answer correct:
 * a guest sending `AL_DIRECTION` must not be told its enum is wrong just because this host only
 * stores the value.
 */
const SOURCE_I_PARAMS: ReadonlySet<number> = new Set([
  AL_SOURCE_RELATIVE, 4097, 4098, AL_PITCH, AL_POSITION, AL_DIRECTION, AL_VELOCITY, AL_LOOPING,
  AL_BUFFER, AL_GAIN, AL_MIN_GAIN, AL_MAX_GAIN, AL_REFERENCE_DISTANCE, AL_ROLLOFF_FACTOR,
  AL_MAX_DISTANCE, AL_SEC_OFFSET, AL_SAMPLE_OFFSET, AL_BYTE_OFFSET, 4628, 8201, 8202, 53248,
]);

/** `_alSourcef`'s own list, from the glue's switch: the same numerals, plus 4130 and 8203. */
const SOURCE_F_PARAMS: ReadonlySet<number> = new Set([
  4097, 4098, AL_PITCH, AL_GAIN, AL_MIN_GAIN, AL_MAX_GAIN, AL_REFERENCE_DISTANCE,
  AL_ROLLOFF_FACTOR, 4130, AL_MAX_DISTANCE, AL_SEC_OFFSET, AL_SAMPLE_OFFSET, AL_BYTE_OFFSET, 8203,
]);

/** `_alSource3f` takes three floats and only three parameters, all of them position-shaped. */
const SOURCE_3F_PARAMS: ReadonlySet<number> = new Set([AL_POSITION, AL_DIRECTION, AL_VELOCITY]);

/** The six `_alGetString` enums, and the four capability strings. */
const AL_VENDOR = 45057;
const AL_VERSION = 45058;
const AL_RENDERER = 45059;
const AL_EXTENSIONS = 45060;

/**
 * The extension list, verbatim from the glue.
 *
 * Claiming an extension is claiming the guest may call the rows behind it, and the guest can only
 * call rows this build imports. These five are the glue's, so they are the five the guest was
 * compiled against; nothing is added.
 */
const AL_EXTENSIONS_LIST: readonly string[] = [
  'AL_EXT_float32', 'AL_SOFT_loop_points', 'AL_SOFT_source_length',
  'AL_EXT_source_distance_model', 'AL_SOFT_source_spatialize',
];

/** `ALC_FREQUENCY` is the only attribute honoured; these are the ones acknowledged and ignored. */
const ALC_ACKNOWLEDGED_ATTRS: ReadonlySet<number> = new Set([
  4112, // ALC_MAJOR_VERSION
  4113, // ALC_MINOR_VERSION
  6546, // ALC_HRTF_SOFT
  6550, // ALC_HRTF_REQUIRED_SOFT
]);

/** The interleaved PCM formats: `bytesPerSample * channels` decides the frame stride. */
const FORMAT_LAYOUT: Readonly<Record<number, readonly [number, number]>> = {
  4352: [1, 1], // AL_FORMAT_MONO8
  4353: [2, 1], // AL_FORMAT_MONO16
  4354: [1, 2], // AL_FORMAT_STEREO8
  4355: [2, 2], // AL_FORMAT_STEREO16
  65552: [4, 1], // AL_FORMAT_MONO_FLOAT32
  65553: [4, 2], // AL_FORMAT_STEREO_FLOAT32
};

/** The device name `_alcOpenDevice` accepts when it is asked for one by name, from the glue. */
const DEVICE_NAME = 'Emscripten OpenAL';

/** WebAudio's own cap per `AudioBuffer` channel; OpenAL has no equivalent and would not care. */
const MAX_BUFFER_FRAMES = 1 << 24;

/* -------------------------------------------------------------------------- */
/* State                                                                       */
/* -------------------------------------------------------------------------- */

interface AudioDevice {
  readonly name: string;
}

interface AudioBufferRecord {
  readonly id: number;
  readonly format: number;
  readonly frequency: number;
  readonly channels: number;
  readonly bytesPerSample: number;
  /** Frames per channel — what `AL_SAMPLE_OFFSET` counts and `createBuffer` was asked for. */
  readonly frames: number;
  readonly data: Float32Array[];
  /** Queued on a source: `_alBufferData` and `_alDeleteBuffers` must both refuse. */
  refCount: number;
}

interface AudioSourceRecord {
  readonly gain: GainNode;
  panner: PannerNode | null;
  state: number;
  type: number;
  bufQueue: AudioBufferRecord[];
  bufsProcessed: number;
  /** Frames already consumed. `AL_SAMPLE_OFFSET` reads it; a pause freezes it. */
  bufOffset: number;
  looping: boolean;
  pitch: number;
  gainValue: number;
  minGain: number;
  maxGain: number;
  relative: boolean;
  refDistance: number;
  rolloffFactor: number;
  maxDistance: number;
  position: [number, number, number];
  direction: [number, number, number];
  velocity: [number, number, number];
  /** Bumped by anything that invalidates a scheduled `onended`. The only defence against it. */
  generation: number;
  /** `bufOffset` captured at `_alSourcePause`, so `_alSourcePlay` resumes rather than restarts. */
  pausedOffset: number | null;
  /** Nodes handed to WebAudio and not yet ended. Stopped and disconnected on pause, stop and dispose. */
  readonly live: Set<AudioBufferSourceNode>;
  /** The node currently producing sound, when one is scheduled. `_alSourceStop` stops it. */
  audioNode: AudioBufferSourceNode | null;
}

interface AudioContextRecord {
  readonly device: number;
  readonly sampleRate: number;
  readonly context: AudioContext;
  /** The context-level gain every source connects to, so dispose has one node to detach. */
  readonly master: GainNode;
  readonly sources: Map<number, AudioSourceRecord>;
}

/* -------------------------------------------------------------------------- */
/* The host                                                                    */
/* -------------------------------------------------------------------------- */

/**
 * Build the nineteen-row import table and the lifecycle that drives it.
 *
 * Everything the caller must not lose is one object: the device map, the context map, the buffers,
 * the error latch. There is no module-level state, so two hosts in one page are two independent
 * OpenAL implementations — which is what a test that runs two games in two tabs' worth of harness
 * would need, and which a shared `currentCtx` global would make impossible.
 */
export function createArtemisAudioHost(options: ArtemisAudioHostOptions): ArtemisAudioHost {
  const { memory, malloc } = options;

  /** A diagnostic sink that throws must not become the failure it was describing. */
  const report = (what: string, detail: string): void => {
    try {
      options.onBlocked?.(what, detail);
    } catch {
      /* see above */
    }
  };

  /**
   * The error latch, with OpenAL's setter semantics rather than a plain field.
   *
   * `AL_INVALID_OPERATION` is what "no current context" means, and it has to stay readable until
   * the guest calls `_alGetError`. The one shot that clears it is an explicit `0`: the glue's own
   * `set err(val) { if (this._err === 0 || val === 0) this._err = val }` refuses to overwrite a
   * pending error with a second one, and this host is bit-compatible with that so a guest that
   * checks after two bad calls sees the first.
   */
  let latched = AL_NO_ERROR;
  const setError = (code: number): void => {
    if (latched === AL_NO_ERROR || code === AL_NO_ERROR) latched = code;
  };
  const takeError = (): number => {
    if (current === null) return AL_INVALID_OPERATION;
    const code = latched || (perContextError ?? AL_NO_ERROR);
    latched = AL_NO_ERROR;
    perContextError = AL_NO_ERROR;
    return code;
  };
  /**
   * The same latch, per context.
   *
   * The glue keeps one `_err` cell *per context*, so an error raised before `_alcMakeContextCurrent`
   * and read after it is still visible. Two cells here — one for the current context, one for the
   * window in which there is none — reproduce that without a third state.
   */
  let perContextError = AL_NO_ERROR;

  let devices = new Map<number, AudioDevice>();
  let contexts = new Map<number, AudioContextRecord>();
  const buffers = new Map<number, AudioBufferRecord>();
  const stringCache = new Map<number, number>();
  let current: AudioContextRecord | null = null;
  let nextId = 1;
  let disposed = false;
  /** Flipped once `_alcOpenDevice` has tried, so a missing WebAudio is reported once, not per call. */
  let backendProbed = false;
  let backend: (() => AudioContext) | null = null;

  /**
   * `_alcCreateContext` is the only place a context is ever built.
   *
   * `contextFactory` wins over the globals because a caller that supplied one has already decided;
   * the globals are the browser default and `webkitAudioContext` is the Safari spelling that a
   * page with a strict `AudioContext` might still need.
   */
  const resolveBackend = (): (() => AudioContext) | null => {
    if (backendProbed) return backend;
    backendProbed = true;
    if (options.contextFactory) {
      backend = options.contextFactory;
      return backend;
    }
    const scope = globalThis as { AudioContext?: unknown; webkitAudioContext?: unknown };
    const ctor = (scope.AudioContext ?? scope.webkitAudioContext) as
      | (new () => AudioContext)
      | undefined;
    if (typeof ctor === 'function') {
      backend = () => new ctor();
      return backend;
    }
    backend = null;
    report('audio', 'no WebAudio: this host has no AudioContext and webkitAudioContext, so it opens 0 devices');
    return backend;
  };

  /**
   * Reads `count` i32 ids out of the guest heap.
   *
   * The whole array is validated **before** the caller mutates anything, because every consumer of
   * this list is a "do it to all of them" call: a half-applied `_alDeleteSources` is a worse outcome
   * than a refused one, and the glue's own two-pass loops say the same thing in a different shape.
   */
  const readIds = (pIDs: number, count: number): number[] => {
    assertRange(pIDs, count * 4);
    if (count === 0) return [];
    const ids: number[] = [];
    for (let i = 0; i < count; i += 1) ids.push(memory.u32[(pIDs >>> 2) + i] ?? 0);
    return ids;
  };

  /**
   * Bounds-check a `[ptr, ptr+len)` range against the *current* heap.
   *
   * `memory.assertRange` throws an `ArtemisErrnoError`, which is a syscall-shaped failure; an
   * out-of-range pointer here is a guest bug that must become `AL_INVALID_VALUE`. The check is re-run
   * on every call rather than cached because any import can grow the heap, and a view held across a
   * grow reads a detached buffer — so the refresh and the check are one act, and every reader below
   * is reading the heap this call just looked at.
   *
   * `len` is checked as well as `ptr`: a non-finite or negative length makes `ptr + len` a NaN or a
   * backwards range, and `NaN > byteLength` is false, which would let a malformed length through and
   * leave the typed-view reads to silently answer `undefined`/`0`.
   */
  const assertRange = (ptr: number, len: number): void => {
    memory.refresh();
    if (len < 0 || !Number.isInteger(len)) {
      throw new ArtemisHostError(`audio range length ${len} is not a whole count of bytes`);
    }
    const full = ptr >>> 0;
    if (!Number.isInteger(ptr) || ptr !== full || full + len > memory.byteLength) {
      throw new ArtemisHostError(`audio pointer ${ptr}+${len} outside heap of ${memory.byteLength}`);
    }
  };

  /** A pointer that must be 4-byte aligned before it is read or written as i32. */
  const assertAligned = (ptr: number): void => {
    memory.refresh();
    const full = ptr >>> 0;
    if (!Number.isInteger(ptr) || ptr !== full || full % 4 !== 0) {
      throw new ArtemisHostError(`audio pointer ${ptr} is not an i32-aligned address`);
    }
  };

  /**
   * The current-context guard.
   *
   * Every `_al*` row in the glue starts with `if(!AL.currentCtx){return}` — the call is a silent
   * no-op and `_alGetError` reports `AL_INVALID_OPERATION` afterwards. That is reproduced exactly,
   * including for the rows that return a value: `_alGetString` returns `0` and `_alGetSourcei`
   * writes nothing.
   */
  const requireContext = (): AudioContextRecord | null => {
    if (current === null) {
      setError(AL_INVALID_OPERATION);
      return null;
    }
    return current;
  };

  /**
   * The source every mutating `_al*` row starts with, or `null` after latching `AL_INVALID_NAME`.
   *
   * `_alGetSourcei` reads `_alGetError` after this on purpose: an unknown source and an unknown
   * parameter must both latch, and the glue's order — name first, then enum — is the one a guest
   * that only reads one code will see.
   */
  const requireSource = (ctx: AudioContextRecord, sourceId: number): AudioSourceRecord | null => {
    const src = ctx.sources.get(sourceId);
    if (src === undefined) {
      setError(AL_INVALID_NAME);
      return null;
    }
    return src;
  };

  /**
   * The `value` half of `_alSourcef`'s validators, from the glue's `setSourceParam`.
   *
   * Returns `null` when the value is acceptable, and the error to latch when it is not — so a caller
   * reads one line instead of threading a boolean through every case. The ranges are the glue's:
   * `PITCH > 0`, `GAIN >= 0`, `MIN_GAIN` in `[0, min(maxGain, 1)]`, `MAX_GAIN` in `[max(0, minGain), 1]`,
   * and the three distance parameters `>= 0`.
   */
  const validateFloat = (param: number, value: number, src: AudioSourceRecord): number | null => {
    if (!Number.isFinite(value)) return AL_INVALID_VALUE;
    switch (param) {
      case AL_PITCH:
        return value > 0 ? null : AL_INVALID_VALUE;
      case AL_GAIN:
        return value >= 0 ? null : AL_INVALID_VALUE;
      case AL_MIN_GAIN:
        return value >= 0 && value <= Math.min(src.maxGain, 1) ? null : AL_INVALID_VALUE;
      case AL_MAX_GAIN:
        return value >= Math.max(0, src.minGain) && value <= 1 ? null : AL_INVALID_VALUE;
      case AL_REFERENCE_DISTANCE:
      case AL_ROLLOFF_FACTOR:
      case AL_MAX_DISTANCE:
        return value >= 0 ? null : AL_INVALID_VALUE;
      case 4097: // AL_CONE_INNER_ANGLE
      case 4098: // AL_CONE_OUTER_ANGLE
      case 4130: // AL_CONE_OUTER_GAIN
        return null;
      default:
        return AL_INVALID_ENUM;
    }
  };

  /** Attach or detach the panner. Position is the only thing this host spatializes on. */
  const syncSpace = (ctx: AudioContextRecord, src: AudioSourceRecord): void => {
    const wanted = src.relative === true && src.panner === null;
    if (wanted) {
      const panner = ctx.context.createPanner();
      panner.panningModel = 'equalpower';
      panner.refDistance = src.refDistance;
      panner.maxDistance = src.maxDistance;
      panner.rolloffFactor = src.rolloffFactor;
      panner.positionX.value = src.position[0];
      panner.positionY.value = src.position[1];
      panner.positionZ.value = src.position[2];
      src.gain.disconnect();
      src.gain.connect(panner);
      panner.connect(ctx.master);
      src.panner = panner;
      return;
    }
    if (!wanted && src.panner !== null) {
      src.panner.disconnect();
      src.gain.disconnect();
      src.gain.connect(ctx.master);
      src.panner = null;
      return;
    }
    const panner = src.panner;
    if (panner === null) return;
    panner.positionX.value = src.position[0];
    panner.positionY.value = src.position[1];
    panner.positionZ.value = src.position[2];
  };

  /**
   * Build one `AudioBufferSourceNode` for one buffer, connect it, and start it.
   *
   * Returns the node so the caller can keep it in the source's live set; the `onended` handler is
   * **not** installed here, because the only place that knows whether a node is still the current
   * one is the pump, which holds the generation counter.
   */
  const startNode = (
    ctx: AudioContextRecord,
    src: AudioSourceRecord,
    buf: AudioBufferRecord,
    when: number,
    offsetFrames: number,
  ): AudioBufferSourceNode => {
    const node = ctx.context.createBufferSource();
    node.buffer = ctx.context.createBuffer(
      buf.channels,
      buf.frames,
      buf.frequency,
    );
    for (let c = 0; c < buf.channels; c += 1) node.buffer.getChannelData(c).set(buf.data[c]!);
    node.loop = src.looping;
    node.playbackRate.value = src.pitch;
    node.connect(src.gain);
    src.live.add(node);
    // Three facts nothing on the node itself records, and the offset arithmetic needs all of them:
    // the frame of the queue this node was built from, the frame *within its buffer* it began at (a
    // resume into the middle), and the clock reading it began at.
    node.__artemisBuf = buf.id;
    node.__artemisStart = ctx.context.currentTime;
    src.audioNode = node;
    node.start(when, offsetFrames / buf.frequency);
    return node;
  };

  /**
   * The pump: queue the next unprocessed buffer, one node at a time.
   *
   * The official glue runs `scheduleContextAudio` from a 25 ms interval and schedules a whole
   * `QUEUE_LOOKAHEAD` (100 ms) of nodes ahead. This host does the same job event-driven: each node's
   * `onended` starts its successor, so a playing source needs no timer and a stopped one has nothing
   * scheduled. `seq` is the whole safety story — a node whose `seq` is stale is a node some pause,
   * stop or dispose has already invalidated, and it must not mark a buffer processed.
   */
  const pump = (ctx: AudioContextRecord, src: AudioSourceRecord, seq: number): void => {
    if (disposed || src.generation !== seq) return;
    if (src.state !== AL_PLAYING) return;
    if (src.bufsProcessed >= src.bufQueue.length) {
      if (src.looping && src.bufQueue.length > 0) {
        src.bufsProcessed = 0;
        src.bufOffset = 0;
      } else if (src.type === AL_STREAMING) {
        // A drained queue is not the end of a streaming source: the guest is about to queue more.
        // Processed stays where it is so `_alSourceUnqueueBuffers` can hand the played buffers back.
      } else {
        setSourceState(ctx, src, AL_STOPPED);
        return;
      }
    }
    const buf = src.bufQueue[src.bufsProcessed];
    if (buf === undefined) {
      setSourceState(ctx, src, AL_STOPPED);
      return;
    }
    const node = startNode(ctx, src, buf, ctx.context.currentTime, src.bufOffset);
    const markProcessed = (): void => {
      // First, before anything this handler might do: a node whose generation has moved on belongs
      // to a pause, stop or dispose that has already happened. It must not disconnect the node that
      // succeeded it, must not count as processed, and must not schedule.
      if (disposed || src.generation !== seq) return;
      node.onended = null;
      src.live.delete(node);
      if (src.audioNode === node) src.audioNode = null;
      src.bufsProcessed += 1;
      src.bufOffset = 0;
      pump(ctx, src, seq);
    };
    node.onended = markProcessed;
  };

  /**
   * `AL.setSourceState`, faithfully.
   *
   * `PLAYING` from `PLAYING` or `STOPPED` rewinds to the top — that is OpenAL's "restart a stopped
   * source", and it is why `_alSourcePlay` after `_alSourceStop` plays the queue from the beginning
   * rather than from wherever it had got to. `PAUSED` and `STOPPED` both silence every live node
   * first, so nothing keeps playing under a state the guest believes has changed.
   */
  const setSourceState = (ctx: AudioContextRecord, src: AudioSourceRecord, state: number): void => {
    if (state === AL_PLAYING) {
      if (src.state === AL_PLAYING || src.state === AL_STOPPED) {
        // A genuine restart rewinds. A resume out of `AL_PAUSED` does not: it keeps the offset the
        // pause froze and replays that buffer from `bufOffset` in, which is what makes a second
        // `_alSourcePlay` on a paused source continue rather than replay.
        src.bufsProcessed = 0;
        src.bufOffset = 0;
      }
      const resumeAt = src.state === AL_PAUSED ? src.pausedOffset ?? src.bufOffset : src.bufOffset;
      stopNodes(src);
      src.generation += 1;
      src.state = AL_PLAYING;
      src.pausedOffset = null;
      // A resume re-enters the queue at its head, not at `bufsProcessed`: a streaming guest may have
      // unqueued the played prefix while the source was paused, and every later index — this one and
      // `currentOffset`'s — is measured against the queue as it stands now.
      src.bufsProcessed = 0;
      src.bufOffset = resumeAt;
      const from = src.bufQueue[0];
      if (from !== undefined) {
        const node = startNode(ctx, src, from, ctx.context.currentTime, resumeAt);
        const seq = src.generation;
        node.onended = (): void => {
          if (disposed || src.generation !== seq) return;
          node.onended = null;
          src.live.delete(node);
          if (src.audioNode === node) src.audioNode = null;
          src.bufsProcessed += 1;
          src.bufOffset = 0;
          pump(ctx, src, seq);
        };
        return;
      }
      return;
    }
    if (state === AL_PAUSED) {
      if (src.state === AL_PLAYING) {
        // Measured first, then frozen: the node still exists and its stamp still says when it began.
        capturePlayedFrames(src);
        // The offset is recorded on the source as well as left in `bufOffset`; `stopNodes` below
        // clears `audioNode`, so a resume that read the clock again would read nothing.
        src.pausedOffset = src.bufOffset;
        stopNodes(src);
        src.generation += 1;
        src.state = AL_PAUSED;
      }
      return;
    }
    if (state === AL_STOPPED) {
      if (src.state !== AL_INITIAL) {
        src.state = AL_STOPPED;
      }
      // OpenAL promises `AL_BUFFERS_PROCESSED == AL_BUFFERS_QUEUED` after a stop, because a stopped
      // source has no unplayed audio. The buffers stay *queued* — a streaming guest unqueues them —
      // they are simply all marked played.
      src.bufsProcessed = src.bufQueue.length;
      src.bufOffset = 0;
      src.pausedOffset = null;
      stopNodes(src);
      src.generation += 1;
      return;
    }
    // AL_INITIAL is the fourth arm the glue has, and the only one that does not touch the queue
    // length: it rewinds to frame zero and re-silences.
    if (state === AL_INITIAL && src.state !== AL_INITIAL) {
      src.state = AL_INITIAL;
      src.bufsProcessed = 0;
      src.bufOffset = 0;
      src.pausedOffset = null;
      stopNodes(src);
      src.generation += 1;
    }
  };

  /**
   * Advance `bufOffset`/`bufsProcessed` to match what WebAudio actually played.
   *
   * Called before a pause so the resume point is real rather than the frame the last buffer started
   * at. `context.currentTime` is the clock both sides read, so the arithmetic is exactly the glue's:
   * the buffer started at `startedAt` and has been playing since.
   */
  const capturePlayedFrames = (src: AudioSourceRecord): void => {
    const node = src.audioNode;
    if (node === null || node.buffer === null || node.buffer === undefined) return;
    const started = node.__artemisStart;
    if (started === undefined) return;
    // Seconds of wall clock, not whole seconds: the glue's own arithmetic is `(now - started) * rate`,
    // and flooring the seconds first would round every pause down to the buffer's start.
    const seconds = Math.max(0, node.context.currentTime - started);
    const frames = Math.min(node.buffer.length, Math.floor(seconds * node.buffer.sampleRate * src.pitch));
    // The node began `src.bufOffset` frames into its buffer, so what has been played is that much
    // plus everything the clock has counted since.
    src.bufOffset = Math.min(node.buffer.length, src.bufOffset + frames);
  };

  /** Stop and detach every node still live on a source, and forget them. */
  function stopNodes(src: AudioSourceRecord): void {
    for (const node of src.live) {
      node.onended = null;
      try {
        node.stop();
      } catch {
        // A node that already ended throws on `stop()`; that is the outcome this wanted anyway.
      }
      try {
        node.disconnect();
      } catch {
        /* already detached */
      }
    }
    src.live.clear();
    src.audioNode = null;
  }

  /**
   * Validate and adopt a buffer id for `_alSourcei(AL_BUFFER)`.
   *
   * `_alSourcei`'s guard is the state one: a source that is playing or paused refuses the change
   * outright, which is what `AL_INVALID_OPERATION` means here.
   */
  const attachBuffer = (src: AudioSourceRecord, value: number): void => {
    if (src.state === AL_PLAYING || src.state === AL_PAUSED) {
      setError(AL_INVALID_OPERATION);
      return;
    }
    if (value === 0) {
      for (const buf of src.bufQueue) buf.refCount -= 1;
      src.bufQueue = [];
      src.bufsProcessed = 0;
      src.bufOffset = 0;
      src.type = AL_UNDETERMINED;
      return;
    }
    const buf = buffers.get(value);
    if (buf === undefined) {
      setError(AL_INVALID_VALUE);
      return;
    }
    for (const old of src.bufQueue) old.refCount -= 1;
    buf.refCount += 1;
    src.bufQueue = [buf];
    src.bufsProcessed = 0;
    src.bufOffset = 0;
    src.type = AL_STATIC;
  };

  /**
   * Resolve a seek parameter to a frame offset, or latch and return `null`.
   *
   * `AL_SEC_OFFSET` is seconds, `AL_SAMPLE_OFFSET` is frames, `AL_BYTE_OFFSET` is bytes; the glue
   * converts the latter two using the *first* buffer's rate and frame size, and refuses anything
   * past the end of the queue. The comparison is against the queue's total duration, which is what
   * makes a seek to 0 on an empty source legal and a seek to 1 second on nothing not.
   */
  const seekFrames = (param: number, value: number, src: AudioSourceRecord): number | null => {
    let frames = value;
    const head = src.bufQueue[0];
    if (param === AL_SAMPLE_OFFSET) {
      if (head === undefined) frames = value;
    } else if (param === AL_BYTE_OFFSET) {
      const stride = head === undefined ? 0 : head.frequency * head.bytesPerSample * head.channels;
      frames = stride > 0 ? value / stride : value;
    }
    let total = 0;
    for (const buf of src.bufQueue) total += buf.frames;
    if (!Number.isFinite(frames) || frames < 0 || (total > 0 && frames > total)) {
      setError(AL_INVALID_VALUE);
      return null;
    }
    return frames;
  };

  /**
   * `AL.sourceSeek`: rewind to the frame, then restart if the source was playing.
   *
   * The rewind walks the queue to the buffer that contains `offset`, which is what makes a seek into
   * the second buffer of a streaming queue land in the second buffer rather than in the first.
   */
  const seek = (ctx: AudioContextRecord, src: AudioSourceRecord, frames: number): void => {
    const playing = src.state === AL_PLAYING;
    if (playing) setSourceState(ctx, src, AL_INITIAL);
    let remaining = frames;
    let index = 0;
    while (index < src.bufQueue.length && remaining >= (src.bufQueue[index]?.frames ?? 0)) {
      remaining -= src.bufQueue[index]?.frames ?? 0;
      index += 1;
    }
    src.bufsProcessed = Math.min(index, src.bufQueue.length);
    src.bufOffset = remaining;
    if (playing) setSourceState(ctx, src, AL_PLAYING);
  };

  /* ---------------------------------------------------------------------- */
  /* The nineteen rows                                                       */
  /* ---------------------------------------------------------------------- */

  /**
   * `_alcOpenDevice(name)` — an id, or `0` for "no device".
   *
   * `0` is OpenAL's own failure value here, so it is the honest answer for a host with no WebAudio
   * rather than a fabricated device the guest would then build a context on.
   */
  const alcOpenDevice = (pDeviceName: number): number => {
    if (disposed) return 0;
    if (pDeviceName !== 0) {
      let name: string;
      try {
        // The name is read out of a heap the caller may have grown since the last call, so the
        // refresh has to happen before the string walk — `utf8` dereferences the very view a stale
        // heap would have detached.
        memory.refresh();
        name = memory.utf8(pDeviceName);
      } catch {
        name = '';
      }
      if (name !== DEVICE_NAME) return 0;
    }
    if (resolveBackend() === null) return 0;
    const id = nextId;
    nextId += 1;
    devices.set(id, { name: DEVICE_NAME });
    return id;
  };

  /**
   * `_alcCreateContext(device, attrs)` — the one place an `AudioContext` is constructed.
   *
   * The attribute list is read as `[name, value]` pairs terminated by a `0`, and the reader is
   * bounded: a list with no terminator stops at the end of the heap and is refused, because the
   * official reader would otherwise walk off the buffer. `ALC_FREQUENCY` sets the sample rate; the
   * four other attributes the glue recognises are accepted and ignored; anything else is
   * `AL_INVALID_OPERATION` and no context.
   */
  const alcCreateContext = (deviceId: number, pAttrList: number): number => {
    if (disposed) return 0;
    if (!devices.has(deviceId)) {
      setError(AL_INVALID_NAME);
      return 0;
    }
    const build = resolveBackend();
    if (build === null) {
      report('audio', `alcCreateContext: device ${deviceId} has no WebAudio backend to build a context from`);
      return 0;
    }
    let sampleRate: number | null = null;
    if (pAttrList !== 0) {
      assertAligned(pAttrList);
      let cursor = pAttrList;
      for (;;) {
        assertRange(cursor, 4);
        const attr = memory.u32[cursor >>> 2] ?? 0;
        if (attr === 0) break;
        assertRange(cursor, 8);
        const value = memory.u32[(cursor >>> 2) + 1] ?? 0;
        if (attr === ALC_FREQUENCY) {
          sampleRate = value;
        } else if (!ALC_ACKNOWLEDGED_ATTRS.has(attr)) {
          setError(AL_INVALID_OPERATION);
          report('audio', `alcCreateContext: unsupported attribute ${attr}`);
          return 0;
        }
        cursor += 8;
      }
    }
    if (sampleRate !== null && !(sampleRate > 0)) {
      setError(AL_INVALID_VALUE);
      report('audio', `alcCreateContext: refused sample rate ${sampleRate}`);
      return 0;
    }
    let context: AudioContext;
    try {
      context = build();
    } catch {
      report('audio', 'alcCreateContext: the AudioContext constructor threw, so there is no context');
      return 0;
    }
    const master = context.createGain();
    master.connect(context.destination);
    const id = nextId;
    nextId += 1;
    contexts.set(id, {
      device: deviceId,
      sampleRate: sampleRate ?? context.sampleRate,
      context,
      master,
      sources: new Map(),
    });
    /**
     * The autoplay policy, handled without ever returning a promise to the guest.
     *
     * A context built before the page's first user gesture starts `suspended`, and the promise from
     * `resume()` rejects on a page that never gets the gesture. Neither can be allowed to reach the
     * guest — it has no way to await either — so the promise is consumed here and the outcome is a
     * diagnostic. {@link ArtemisAudioHost.resume} is the explicit path a session drives.
     */
    if (context.state === 'suspended') {
      const attempt = context.resume();
      void Promise.resolve(attempt).then(
        () => report('audio', 'the audio context started suspended and resumed on creation'),
        (reason: unknown) =>
          report('audio', `the audio context is suspended and resume() failed: ${String(reason)}`),
      );
    }
    return id;
  };

  /**
   * `_alcMakeContextCurrent(ctx)` — official `1`/`0`, where `0` clears the current context.
   *
   * A nonzero id that is not a context this host minted is *not* an error in the glue, and it is not
   * one here: the id is stored as-is and the next call through it fails with `AL_INVALID_OPERATION`.
   * Returning `1` for a context that does not exist is the glue's own answer, and a guest that
   * trusts it will hear nothing — which is what an unknown context means.
   */
  const alcMakeContextCurrent = (contextId: number): number => {
    if (disposed) return 0;
    if (contextId === 0) {
      current = null;
      return 0;
    }
    perContextError = latched || perContextError;
    latched = AL_NO_ERROR;
    current = contexts.get(contextId) ?? null;
    if (current === null) {
      report('audio', `alcMakeContextCurrent: no context ${contextId}; every later call sees no current context`);
    }
    return 1;
  };

  /** `_alGetString(enum)`, cached per enum exactly as the glue caches it. */
  const alGetString = (param: number): number => {
    if (current === null) return 0;
    const cached = stringCache.get(param);
    if (cached !== undefined) return cached;
    const text = stringFor(param);
    if (text === null) {
      setError(AL_INVALID_ENUM);
      return 0;
    }
    const bytes = new TextEncoder().encode(text);
    const ptr = malloc(bytes.length + 1);
    if (!Number.isInteger(ptr) || ptr <= 0) {
      setError(AL_OUT_OF_MEMORY);
      return 0;
    }
    assertRange(ptr, bytes.length + 1);
    memory.u8.set(bytes, ptr);
    memory.u8[ptr + bytes.length] = 0;
    stringCache.set(param, ptr);
    return ptr;
  };

  const stringFor = (param: number): string | null => {
    switch (param) {
      case AL_NO_ERROR:
        return 'No Error';
      case AL_INVALID_NAME:
        return 'Invalid Name';
      case AL_INVALID_ENUM:
        return 'Invalid Enum';
      case AL_INVALID_VALUE:
        return 'Invalid Value';
      case AL_INVALID_OPERATION:
        return 'Invalid Operation';
      case AL_OUT_OF_MEMORY:
        return 'Out of Memory';
      case AL_VENDOR:
        return 'Emscripten';
      case AL_VERSION:
        return '1.1';
      case AL_RENDERER:
        return 'WebAudio';
      case AL_EXTENSIONS:
        return AL_EXTENSIONS_LIST.join(' ');
      default:
        return null;
    }
  };

  /** `_alGenBuffers` / `_alGenSources` — a monotonic id each, no reuse. */
  const alGenBuffers = (count: number, pIDs: number): void => {
    if (requireContext() === null) return;
    if (count <= 0) return;
    // `_alGenSources`'s sibling, and it validates the same way: a bad output array must not leave
    // half a generation of ids behind on a heap the guest cannot name them from.
    try {
      if (!Number.isInteger(count)) throw new ArtemisHostError(`id count ${count} is not an integer`);
      assertAligned(pIDs);
      assertRange(pIDs, count * 4);
    } catch {
      setError(AL_INVALID_VALUE);
      report('audio', `alGenBuffers: id array at ${pIDs} x${count} is not writable`);
      return;
    }
    for (let i = 0; i < count; i += 1) {
      const id = nextId;
      nextId += 1;
      buffers.set(id, {
        id,
        format: 0,
        frequency: 0,
        channels: 0,
        bytesPerSample: 0,
        frames: 0,
        data: [],
        refCount: 0,
      });
      memory.u32[(pIDs >>> 2) + i] = id;
    }
  };

  const alGenSources = (count: number, pIDs: number): void => {
    const ctx = requireContext();
    if (ctx === null) return;
    if (count <= 0) return;
    assertAligned(pIDs);
    assertRange(pIDs, count * 4);
    for (let i = 0; i < count; i += 1) {
      const id = nextId;
      nextId += 1;
      const gain = ctx.context.createGain();
      gain.connect(ctx.master);
      ctx.sources.set(id, {
        gain,
        panner: null,
        state: AL_INITIAL,
        type: AL_UNDETERMINED,
        bufQueue: [],
        bufsProcessed: 0,
        bufOffset: 0,
        looping: false,
        pitch: 1,
        gainValue: 1,
        minGain: 0,
        maxGain: 1,
        relative: false,
        refDistance: 1,
        rolloffFactor: 1,
        maxDistance: 1e6,
        position: [0, 0, 0],
        direction: [0, 0, 0],
        velocity: [0, 0, 0],
        generation: 0,
        pausedOffset: null,
        live: new Set(),
        audioNode: null,
      });
      memory.u32[(pIDs >>> 2) + i] = id;
    }
  };

  /**
   * `_alDeleteBuffers` — refuse while any buffer is queued.
   *
   * A queued buffer is a buffer WebAudio is reading through an `AudioBuffer` this host handed it; the
   * valid-looking alternative (delete it, let the node keep playing a copy) is how a streaming guest
   * ends up with a queue that counts buffers it can no longer name.
   */
  const alDeleteBuffers = (count: number, pIDs: number): void => {
    if (requireContext() === null) return;
    if (count <= 0) return;
    let ids: number[];
    try {
      ids = readIds(pIDs, count);
    } catch {
      setError(AL_INVALID_VALUE);
      report('audio', `alDeleteBuffers: id array at ${pIDs} x${count} is not readable`);
      return;
    }
    for (const id of ids) {
      if (id === 0) continue;
      const buf = buffers.get(id);
      if (buf === undefined) {
        setError(AL_INVALID_NAME);
        return;
      }
      if (buf.refCount > 0) {
        setError(AL_INVALID_OPERATION);
        report('audio', `alDeleteBuffers: buffer ${id} is still queued on a source`);
        return;
      }
    }
    for (const id of ids) {
      if (id === 0) continue;
      buffers.delete(id);
    }
  };

  const alDeleteSources = (count: number, pIDs: number): void => {
    const ctx = requireContext();
    if (ctx === null) return;
    if (count <= 0) return;
    let ids: number[];
    try {
      ids = readIds(pIDs, count);
    } catch {
      setError(AL_INVALID_VALUE);
      report('audio', `alDeleteSources: id array at ${pIDs} x${count} is not readable`);
      return;
    }
    for (const id of ids) {
      if (!ctx.sources.has(id)) {
        setError(AL_INVALID_NAME);
        return;
      }
    }
    for (const id of ids) {
      const src = ctx.sources.get(id);
      if (src === undefined) continue;
      src.generation += 1;
      stopNodes(src);
      for (const buf of src.bufQueue) buf.refCount -= 1;
      src.bufQueue = [];
      try {
        src.gain.disconnect();
      } catch {
        /* already detached */
      }
      ctx.sources.delete(id);
    }
  };

  /**
   * `_alBufferData` — decode PCM into per-channel `Float32Array`s.
   *
   * The copy happens **now**, out of the current heap, which is the only moment it can: the guest is
   * free to reuse `pData` the instant this returns, and WebAudio will read the samples much later.
   * That is also why the range is checked before anything is allocated — a size that does not fit the
   * heap is `AL_INVALID_VALUE`, and it must not leave a half-built `AudioBuffer` behind.
   */
  const alBufferData = (
    bufferId: number,
    format: number,
    pData: number,
    size: number,
    frequency: number,
  ): void => {
    if (requireContext() === null) return;
    const buf = buffers.get(bufferId);
    if (buf === undefined) {
      setError(AL_INVALID_NAME);
      return;
    }
    /**
     * A buffer a source is holding is not the guest's to overwrite.
     *
     * OpenAL's own answer is `AL_INVALID_OPERATION`: a queued or attached buffer's storage is
     * owned by the source until it is unqueued or detached, and replacing the samples underneath a
     * queue that still names this id would change audio the guest already submitted. The refusal
     * precedes the allocation and the replace, so a refused call leaves the buffer exactly as it
     * was — but it comes *after* the argument checks, because a malformed call is malformed whether
     * or not the buffer happens to be queued, and the guest that sent bad numbers should hear about
     * those numbers.
     */
    const replaceOwned = buf.refCount > 0;
    const layout = FORMAT_LAYOUT[format];
    if (layout === undefined) {
      setError(AL_INVALID_VALUE);
      return;
    }
    if (!(frequency > 0) || !Number.isInteger(frequency)) {
      setError(AL_INVALID_VALUE);
      return;
    }
    const [bytesPerSample, channels] = layout;
    const frameBytes = bytesPerSample * channels;
    if (!Number.isInteger(size) || size < 0 || size % frameBytes !== 0) {
      setError(AL_INVALID_VALUE);
      report('audio', `alBufferData: size ${size} is not a whole number of ${frameBytes}-byte frames`);
      return;
    }
    const frames = size / frameBytes;
    if (frames > MAX_BUFFER_FRAMES) {
      setError(AL_INVALID_VALUE);
      report('audio', `alBufferData: ${frames} frames exceeds WebAudio's ${MAX_BUFFER_FRAMES}`);
      return;
    }
    if (size > 0) {
      try {
        assertRange(pData, size);
      } catch {
        setError(AL_INVALID_VALUE);
        report('audio', `alBufferData: PCM at ${pData}+${size} is outside the heap`);
        return;
      }
    }
    if (replaceOwned) {
      setError(AL_INVALID_OPERATION);
      report('audio', `alBufferData: buffer ${bufferId} is still queued on a source`);
      return;
    }
    const data: Float32Array[] = [];
    for (let c = 0; c < channels; c += 1) data.push(new Float32Array(frames));
    if (size > 0) {
      decodeInto(data, format, pData, frames, channels);
    }
    const previous = buffers.get(bufferId);
    if (previous === undefined) return;
    buffers.set(bufferId, {
      id: bufferId,
      format,
      frequency,
      channels,
      bytesPerSample,
      frames,
      data,
      refCount: previous.refCount,
    });
  };

  /** The four conversions, with the glue's own scaling factors. */
  const decodeInto = (
    data: Float32Array[],
    format: number,
    pData: number,
    frames: number,
    channels: number,
  ): void => {
    switch (format) {
      case 4352: {
        // Mono 8-bit unsigned: (byte - 128) / 128.
        const channel = data[0]!;
        for (let i = 0; i < frames; i += 1) channel[i] = ((memory.u8[pData + i] ?? 0) - 128) / 128;
        return;
      }
      case 4354: {
        // Stereo 8-bit unsigned, interleaved: L R L R.
        const left = data[0]!;
        const right = data[1]!;
        let cursor = pData;
        for (let i = 0; i < frames; i += 1) {
          left[i] = ((memory.u8[cursor] ?? 0) - 128) / 128;
          right[i] = ((memory.u8[cursor + 1] ?? 0) - 128) / 128;
          cursor += 2;
        }
        return;
      }
      case 4353: {
        // Mono 16-bit signed: sample / 32768.
        const channel = data[0]!;
        for (let i = 0; i < frames; i += 1) {
          channel[i] = (memory.i16[(pData >>> 1) + i] ?? 0) / 32768;
        }
        return;
      }
      case 4355: {
        const left = data[0]!;
        const right = data[1]!;
        const base = pData >>> 1;
        for (let i = 0; i < frames; i += 1) {
          left[i] = (memory.i16[base + i * 2] ?? 0) / 32768;
          right[i] = (memory.i16[base + i * 2 + 1] ?? 0) / 32768;
        }
        return;
      }
      case 65552: {
        const channel = data[0]!;
        const base = pData >>> 2;
        for (let i = 0; i < frames; i += 1) channel[i] = memory.f32[base + i] ?? 0;
        return;
      }
      case 65553: {
        const left = data[0]!;
        const right = data[1]!;
        const base = pData >>> 2;
        for (let i = 0; i < frames; i += 1) {
          left[i] = memory.f32[base + i * channels] ?? 0;
          right[i] = memory.f32[base + i * channels + 1] ?? 0;
        }
        return;
      }
      default:
        return;
    }
  };

  /**
   * `_alSourceQueueBuffers` — validate the whole list, then mutate.
   *
   * The glue has a bug here worth not reproducing: it latches a format mismatch and then queues the
   * buffers anyway. This host refuses the whole call instead, which is the behaviour a guest can
   * actually rely on — a partially queued list would leave `AL_BUFFERS_QUEUED` describing audio the
   * guest believes it did not submit.
   */
  const alSourceQueueBuffers = (sourceId: number, count: number, pBufferIds: number): void => {
    const ctx = requireContext();
    if (ctx === null) return;
    const src = requireSource(ctx, sourceId);
    if (src === null) return;
    if (src.type === AL_STATIC) {
      setError(AL_INVALID_OPERATION);
      return;
    }
    if (count === 0) return;
    let ids: number[];
    try {
      ids = readIds(pBufferIds, count);
    } catch {
      setError(AL_INVALID_VALUE);
      report('audio', `alSourceQueueBuffers: id array at ${pBufferIds} x${count} is not readable`);
      return;
    }
    const template = src.bufQueue[0];
    const incoming: AudioBufferRecord[] = [];
    for (const id of ids) {
      const buf = buffers.get(id);
      if (buf === undefined) {
        setError(AL_INVALID_NAME);
        return;
      }
      if (
        template !== undefined &&
        (buf.frequency !== template.frequency ||
          buf.bytesPerSample !== template.bytesPerSample ||
          buf.channels !== template.channels)
      ) {
        setError(AL_INVALID_OPERATION);
        report('audio', `alSourceQueueBuffers: buffer ${id} does not match the queue's format`);
        return;
      }
      incoming.push(buf);
    }
    for (const buf of incoming) {
      buf.refCount += 1;
      src.bufQueue.push(buf);
    }
    src.type = AL_STREAMING;
    // A queue that grew while the source was playing must keep playing: the pump is re-entered only
    // if nothing is live, so a source mid-buffer is left alone and a drained one picks the new
    // buffer up.
    if (src.state === AL_PLAYING && src.live.size === 0) pump(ctx, src, src.generation);
  };

  /**
   * `_alSourceUnqueueBuffers` — hand back the played prefix, or refuse.
   *
   * The validation is the glue's: `count > bufsProcessed` is `AL_INVALID_VALUE`, and it is checked
   * before a single buffer leaves the queue. A looping source reports zero processed, so this is
   * also the row that makes "a looping buffer is never unqueueable" true.
   */
  const alSourceUnqueueBuffers = (sourceId: number, count: number, pBufferIds: number): void => {
    const ctx = requireContext();
    if (ctx === null) return;
    const src = requireSource(ctx, sourceId);
    if (src === null) return;
    if (!Number.isInteger(count) || count < 0) {
      setError(AL_INVALID_VALUE);
      return;
    }
    if (count > src.bufsProcessed) {
      setError(AL_INVALID_VALUE);
      return;
    }
    if (count === 0) return;
    /**
     * The output range is checked **before** a single buffer leaves the queue.
     *
     * The glue checks it inline and lets the `shift()` loop do the writing, so a bad pointer there
     * returns a queue with buffers missing and ids nowhere. Here the destination is proven writable
     * first and a refusal is a refusal, with the source's queue and counters exactly as they were.
     */
    try {
      assertAligned(pBufferIds);
      assertRange(pBufferIds, count * 4);
    } catch {
      setError(AL_INVALID_VALUE);
      report('audio', `alSourceUnqueueBuffers: id array at ${pBufferIds} x${count} is not writable`);
      return;
    }
    for (let i = 0; i < count; i += 1) {
      const buf = src.bufQueue.shift();
      if (buf === undefined) break;
      buf.refCount -= 1;
      memory.u32[(pBufferIds >>> 2) + i] = buf.id;
    }
    src.bufsProcessed -= count;
  };

  /** `_alSourcei` — the integer parameters, with the glue's own enum set. */
  const alSourcei = (sourceId: number, param: number, value: number): void => {
    const ctx = requireContext();
    if (ctx === null) return;
    const src = requireSource(ctx, sourceId);
    if (src === null) return;
    if (!SOURCE_I_PARAMS.has(param)) {
      setError(AL_INVALID_ENUM);
      return;
    }
    switch (param) {
      case AL_LOOPING:
        if (value !== 0 && value !== 1) {
          setError(AL_INVALID_VALUE);
          return;
        }
        src.looping = value === 1;
        // A loop toggled mid-buffer changes the node that is playing *and* the successor the pump
        // would build, which is why both are touched.
        for (const node of src.live) node.loop = src.looping;
        if (src.state === AL_PLAYING && src.live.size === 0) pump(ctx, src, src.generation);
        return;
      case AL_BUFFER:
        attachBuffer(src, value);
        return;
      case AL_SOURCE_RELATIVE:
        if (value !== 0 && value !== 1) {
          setError(AL_INVALID_VALUE);
          return;
        }
        src.relative = value === 1;
        syncSpace(ctx, src);
        return;
      case AL_SEC_OFFSET:
      case AL_SAMPLE_OFFSET:
      case AL_BYTE_OFFSET: {
        const frames = seekFrames(param, value, src);
        if (frames === null) return;
        seek(ctx, src, frames);
        return;
      }
      default:
        // An enum OpenAL defines and this host does not honour: reported once, accepted with the
        // value dropped. Refusing it would break a guest that sets cone angles it never reads back.
        report('audio', `alSourcei: parameter ${param} is accepted and not honoured`);
    }
  };

  /** `_alSourcef` — the float parameters, validated before anything is stored or applied. */
  const alSourcef = (sourceId: number, param: number, value: number): void => {
    const ctx = requireContext();
    if (ctx === null) return;
    const src = requireSource(ctx, sourceId);
    if (src === null) return;
    if (!SOURCE_F_PARAMS.has(param)) {
      setError(AL_INVALID_ENUM);
      return;
    }
    const invalid = validateFloat(param, value, src);
    if (invalid !== null) {
      setError(invalid);
      return;
    }
    switch (param) {
      case AL_PITCH:
        src.pitch = value;
        for (const node of src.live) node.playbackRate.value = value;
        return;
      case AL_GAIN:
        // `gain` is the source's own node and `master` is the context's; AL_GAIN writes the source's,
        // live, so a gain change during playback is audible immediately.
        src.gainValue = value;
        src.gain.gain.value = value;
        return;
      case AL_MIN_GAIN:
        src.minGain = value;
        return;
      case AL_MAX_GAIN:
        src.maxGain = value;
        return;
      case AL_REFERENCE_DISTANCE:
        src.refDistance = value;
        if (src.panner !== null) src.panner.refDistance = value;
        return;
      case AL_ROLLOFF_FACTOR:
        src.rolloffFactor = value;
        if (src.panner !== null) src.panner.rolloffFactor = value;
        return;
      case AL_MAX_DISTANCE:
        src.maxDistance = value;
        if (src.panner !== null) src.panner.maxDistance = value;
        return;
      case AL_SEC_OFFSET:
      case AL_SAMPLE_OFFSET:
      case AL_BYTE_OFFSET: {
        const frames = seekFrames(param, value, src);
        if (frames === null) return;
        seek(ctx, src, frames);
        return;
      }
      default:
        // `4097`/`4098`/`4130` live here: validated above, stored nowhere, reported.
        report('audio', `alSourcef: parameter ${param} is accepted and not honoured`);
    }
  };

  /** `_alSource3f` — position, direction, velocity. Only position reaches WebAudio today. */
  const alSource3f = (sourceId: number, param: number, x: number, y: number, z: number): void => {
    const ctx = requireContext();
    if (ctx === null) return;
    const src = requireSource(ctx, sourceId);
    if (src === null) return;
    if (!SOURCE_3F_PARAMS.has(param)) {
      setError(AL_INVALID_ENUM);
      return;
    }
    if (!Number.isFinite(x) || !Number.isFinite(y) || !Number.isFinite(z)) {
      setError(AL_INVALID_VALUE);
      return;
    }
    if (param === AL_POSITION) {
      src.position = [x, y, z];
      syncSpace(ctx, src);
      return;
    }
    if (param === AL_DIRECTION) {
      src.direction = [x, y, z];
      return;
    }
    src.velocity = [x, y, z];
  };

  /**
   * `_alGetSourcei` — one value out of the source's state.
   *
   * The write is checked *after* the parameter is resolved, and a null pointer is `AL_INVALID_VALUE`,
   * which is the glue's own ordering: the guest learns its enum was wrong even when it also passed
   * nowhere to put the answer.
   */
  const alGetSourcei = (sourceId: number, param: number, pValue: number): void => {
    const ctx = requireContext();
    if (ctx === null) return;
    const src = requireSource(ctx, sourceId);
    if (src === null) return;
    const answer = readSourcei(ctx, src, param);
    if (answer === null) return;
    if (pValue === 0) {
      setError(AL_INVALID_VALUE);
      return;
    }
    assertAligned(pValue);
    assertRange(pValue, 4);
    memory.u32[pValue >>> 2] = answer >>> 0;
  };

  const readSourcei = (
    ctx: AudioContextRecord,
    src: AudioSourceRecord,
    param: number,
  ): number | null => {
    switch (param) {
      case AL_SOURCE_RELATIVE:
        return src.relative ? 1 : 0;
      case AL_LOOPING:
        return src.looping ? 1 : 0;
      case AL_BUFFER:
        return src.type === AL_STATIC ? (src.bufQueue[0]?.id ?? 0) : 0;
      case AL_SOURCE_STATE:
        // A streaming source whose queue drained *ends*, and it ends because WebAudio said so. This
        // is the one place the guest's poll and the pump must agree, so the pump writes the state
        // and this reads it rather than recomputing an expiry.
        return src.state;
      case AL_BUFFERS_QUEUED:
        return src.bufQueue.length;
      case AL_BUFFERS_PROCESSED:
        return src.looping ? 0 : src.bufsProcessed;
      case AL_SOURCE_TYPE:
        return src.type;
      case AL_SAMPLE_OFFSET:
        return Math.floor(currentOffset(ctx, src));
      case AL_SEC_OFFSET: {
        const rate = src.bufQueue[0]?.frequency ?? ctx.sampleRate;
        return Math.floor(currentOffset(ctx, src) / rate);
      }
      case AL_BYTE_OFFSET: {
        const head = src.bufQueue[0];
        const stride = head === undefined ? 0 : head.bytesPerSample * head.channels;
        return Math.floor(currentOffset(ctx, src) * stride);
      }
      case AL_PITCH:
        return src.pitch;
      default:
        setError(AL_INVALID_ENUM);
        return null;
    }
  };

  /**
   * Frames consumed so far, derived from the clock rather than stored.
   *
   * A paused source reports the offset it was paused at; a stopped one reports zero because its
   * queue is fully processed, and a playing one is derived from the node that is running. Nothing
   * here mutates the source, so a read cannot change what a later read says.
   */
  const currentOffset = (ctx: AudioContextRecord, src: AudioSourceRecord): number => {
    void ctx;
    if (src.state === AL_PAUSED) return src.pausedOffset ?? src.bufOffset;
    if (src.state !== AL_PLAYING) return 0;
    // A playing source's offset is entirely a reading of the node that is sounding: which buffer it
    // is reading is the node's `buffer`, and how far into it the clock has got. The queue counters
    // deliberately say nothing here — `bufsProcessed` counts buffers that have *ended*.
    const node = src.audioNode;
    if (node === null || node.buffer === null || node.buffer === undefined) return 0;
    let frames = 0;
    const index = src.bufQueue.findIndex((buf) => buf.id === node.__artemisBuf);
    for (let i = 0; i < index; i += 1) frames += src.bufQueue[i]?.frames ?? 0;
    const started = node.__artemisStart;
    if (started !== undefined) {
      const seconds = Math.max(0, node.context.currentTime - started);
      const heard = Math.min(node.buffer.length, Math.floor(seconds * node.buffer.sampleRate * src.pitch));
      frames += Math.min(node.buffer.length, src.bufOffset + heard);
    }
    return frames;
  };

  /* ---------------------------------------------------------------------- */
  /* Assembly                                                                */
  /* ---------------------------------------------------------------------- */

  /**
   * The table. Nineteen keys, and nothing else — a twentieth would be a row this build does not
   * import, and `boot.ts` supplies those itself.
   */
  const imports: ArtemisAudioImports = {
    F: (param: number): number => alGetString(param),
    G: (): number => takeError(),
    Ga: (sourceId: number, param: number, value: number): void => alSourcei(sourceId, param, value),
    Ha: (bufferId: number, format: number, pData: number, size: number, frequency: number): void =>
      alBufferData(bufferId, format, pData, size, frequency),
    Ia: (sourceId: number, count: number, pIDs: number): void =>
      alSourceUnqueueBuffers(sourceId, count, pIDs),
    Ja: (count: number, pIDs: number): void => alDeleteSources(count, pIDs),
    Ka: (count: number, pIDs: number): void => alDeleteBuffers(count, pIDs),
    La: (count: number, pIDs: number): void => alGenBuffers(count, pIDs),
    Ma: (count: number, pIDs: number): void => alGenSources(count, pIDs),
    Na: (contextId: number): number => alcMakeContextCurrent(contextId),
    Oa: (deviceId: number, pAttrList: number): number => alcCreateContext(deviceId, pAttrList),
    Pa: (pDeviceName: number): number => alcOpenDevice(pDeviceName),
    R: (sourceId: number): void => {
      const ctx = requireContext();
      if (ctx === null) return;
      const src = requireSource(ctx, sourceId);
      if (src === null) return;
      setSourceState(ctx, src, AL_PLAYING);
    },
    ga: (sourceId: number): void => {
      const ctx = requireContext();
      if (ctx === null) return;
      const src = requireSource(ctx, sourceId);
      if (src === null) return;
      setSourceState(ctx, src, AL_PAUSED);
    },
    oa: (sourceId: number, param: number, pValue: number): void =>
      alGetSourcei(sourceId, param, pValue),
    vc: (sourceId: number, param: number, x: number, y: number, z: number): void =>
      alSource3f(sourceId, param, x, y, z),
    wc: (sourceId: number, param: number, value: number): void =>
      alSourcef(sourceId, param, value),
    xc: (sourceId: number): void => {
      const ctx = requireContext();
      if (ctx === null) return;
      const src = requireSource(ctx, sourceId);
      if (src === null) return;
      setSourceState(ctx, src, AL_STOPPED);
    },
    yc: (sourceId: number, count: number, pBufferIds: number): void =>
      alSourceQueueBuffers(sourceId, count, pBufferIds),
  };

  /* ---------------------------------------------------------------------- */
  /* Lifecycle                                                               */
  /* ---------------------------------------------------------------------- */

  return {
    imports,

    /**
     * Suspend the context.
     *
     * Deliberately **not** `AL_PAUSED`. A session pauses because a tab was hidden or a menu opened,
     * and the guest's own source states are its business: setting them here would make a resume
     * restart audio the guest believes is playing. `context.suspend()` stops the clock and the
     * speakers, which is the whole of what a host-level pause means.
     */
    async pause(): Promise<void> {
      const pending: Promise<void>[] = [];
      for (const record of contexts.values()) {
        if (record.context.state === 'running') pending.push(record.context.suspend());
      }
      await Promise.all(pending);
    },

    /**
     * Resume the context.
     *
     * Returns the real `resume()` promise rather than swallowing it — a caller that awaits this
     * learns whether the autoplay policy let it through. It creates nothing: a host with no context
     * has nothing to resume, and inventing one here would be the fake device this file refuses.
     */
    async resume(): Promise<void> {
      const pending: Promise<void>[] = [];
      for (const record of contexts.values()) {
        if (record.context.state === 'suspended') pending.push(record.context.resume());
      }
      await Promise.all(pending);
    },

    /**
     * Teardown, idempotent.
     *
     * Every live node is stopped and detached before the context closes, and every generation is
     * bumped first so a late `onended` — WebAudio delivers them asynchronously, and a `stop()` is not
     * guaranteed to have suppressed one that was already queued — finds a stale sequence and does
     * nothing. `close()` is called exactly once per context, guarded by the map being emptied.
     */
    async dispose(): Promise<void> {
      if (disposed) return;
      disposed = true;
      const closing: Promise<void>[] = [];
      for (const record of contexts.values()) {
        for (const src of record.sources.values()) {
          src.generation += 1;
          stopNodes(src);
          for (const buf of src.bufQueue) buf.refCount -= 1;
          src.bufQueue = [];
          try {
            src.gain.disconnect();
          } catch {
            /* already detached */
          }
        }
        record.sources.clear();
        try {
          record.master.disconnect();
        } catch {
          /* already detached */
        }
        if (record.context.state !== 'closed') closing.push(record.context.close());
      }
      contexts = new Map();
      current = null;
      devices = new Map();
      buffers.clear();
      stringCache.clear();
      await Promise.all(closing);
    },
  };
}

/* -------------------------------------------------------------------------- */
/* One extension to the DOM type, and why it is not a cast                     */
/* -------------------------------------------------------------------------- */

declare global {
  interface AudioBufferSourceNode {
    /**
     * When this host started the node, in `context.currentTime` units.
     *
     * The offset arithmetic needs it and WebAudio does not expose it: `AudioBufferSourceNode` knows
     * its own `buffer` and the context knows the clock, but nothing on the node says when it began.
     * A private field would work until two hosts share a context, so the stamp lives on the node
     * under a name no one else has a reason to use.
     */
    __artemisStart?: number;
    /**
     * The buffer id this node was built for.
     *
     * `currentOffset` needs to know *which* entry of a streaming queue is sounding, and the node's
     * `buffer` is a WebAudio object the queue records do not name. This is that link, and it is
     * preferred over `bufsProcessed` because that counter counts buffers which have **ended** —
     * a scheduled node is playing, not processed.
     */
    __artemisBuf?: number;
  }
}
