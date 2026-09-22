/**
 * Plays the engine's audio on the main thread.
 *
 * ## Why the audio crosses a thread boundary
 *
 * The engine runs in a worker, and a worker has no `AudioContext` — it is a window
 * API. So the worker mixes the PCM (SDL's callback writes into buffers the worker
 * owns) and posts each block here, where the real context lives. This is the only
 * arrangement that produces sound: the alternative is a silent context, which is
 * what the harness had before.
 *
 * ## Why a queue rather than immediate playback
 *
 * Audio arrives in blocks of about 20 ms from a `setInterval` in the worker, while
 * the audio clock here is independent and does not jitter in step with it. Playing
 * each block the moment it arrives would stutter, so blocks are queued and a
 * worklet drains them, inserting silence when the queue runs dry. A queue a little
 * behind real time is what makes playback continuous.
 */

/** How far behind real time playback aims to stay, in seconds. */
const TARGET_LATENCY = 0.25;
/** Beyond this the queue is trimmed, so a slow patch cannot build a delay. */
const MAX_LATENCY = 1.0;

/**
 * Plays from a ring buffer, on the audio thread.
 *
 * An `AudioWorklet` is used rather than the deprecated `ScriptProcessorNode`
 * because this is new code and the worklet runs on its own thread — the main thread
 * stays free to handle input and the engine's own work.
 */
const WORKLET_SOURCE = `
class PcmQueueProcessor extends AudioWorkletProcessor {
  constructor() {
    super();
    // A ring buffer, generous enough for the target latency at 48 kHz stereo.
    this.capacity = 1 << 18;
    this.left = new Float32Array(this.capacity);
    this.right = new Float32Array(this.capacity);
    this.read = 0;
    this.write = 0;
    this.available = 0;
    this.underruns = 0;
    this.port.onmessage = (event) => {
      const data = event.data;
      if (data.kind === 'clear') {
        this.read = this.write = this.available = 0;
        return;
      }
      if (data.kind !== 'pcm') return;
      const count = data.left.length;
      for (let index = 0; index < count; index += 1) {
        this.left[this.write] = data.left[index];
        this.right[this.write] = data.right[index];
        this.write = (this.write + 1) % this.capacity;
      }
      this.available = Math.min(this.available + count, this.capacity);
    };
  }

  process(_inputs, outputs) {
    const output = outputs[0];
    if (!output || output.length === 0) return true;
    const left = output[0];
    const right = output.length > 1 ? output[1] : null;
    const frames = left.length;

    if (this.available < frames) {
      // Not enough buffered: emit silence and let the queue refill. Reporting the
      // count makes starvation visible instead of sounding like a broken relay.
      this.underruns += 1;
      if (this.underruns % 50 === 1) {
        this.port.postMessage({ kind: 'underrun', available: this.available, underruns: this.underruns });
      }
      for (let index = 0; index < frames; index += 1) {
        left[index] = 0;
        if (right) right[index] = 0;
      }
      return true;
    }

    for (let index = 0; index < frames; index += 1) {
      left[index] = this.left[this.read];
      if (right) right[index] = this.right[this.read];
      this.read = (this.read + 1) % this.capacity;
    }
    this.available -= frames;
    return true;
  }
}
registerProcessor('pcm-queue', PcmQueueProcessor);
`;

export class AudioRelay {
  constructor(options = {}) {
    this.sampleRate = options.sampleRate || 48000;
    this.context = null;
    this.node = null;
    this.queued = 0;
    this.blocks = 0;
    this.started = false;
    this.peak = 0;
    this.onLog = options.onLog || (() => {});
    this.onError = options.onError || (() => {});
    this._pending = [];
  }

  /**
   * Brings the context up. Must be called from a user gesture.
   *
   * Browsers refuse to start an `AudioContext` otherwise, and a context created
   * before the gesture starts suspended with no way to tell from the code — so the
   * page calls this from the first click.
   */
  async start() {
    if (this.started) {
      if (this.context && this.context.state === 'suspended') await this.context.resume();
      return true;
    }
    try {
      const AudioContextClass = window.AudioContext || window.webkitAudioContext;
      if (!AudioContextClass) {
        this.onError('no AudioContext available in this browser');
        return false;
      }
      this.context = new AudioContextClass({ sampleRate: this.sampleRate });
      const moduleUrl = URL.createObjectURL(new Blob([WORKLET_SOURCE], { type: 'text/javascript' }));
      try {
        await this.context.audioWorklet.addModule(moduleUrl);
      } finally {
        URL.revokeObjectURL(moduleUrl);
      }
      this.node = new AudioWorkletNode(this.context, 'pcm-queue', {
        numberOfInputs: 0,
        numberOfOutputs: 1,
        outputChannelCount: [2],
      });
      this.node.port.onmessage = (event) => {
        if (event.data && event.data.kind === 'underrun') {
          this.onLog(`audio: underrun (buffered ${event.data.available}, total ${event.data.underruns})`);
        }
      };
      this.node.connect(this.context.destination);
      await this.context.resume();
      this.started = true;
      this.onLog(`audio: context running at ${this.context.sampleRate} Hz`);

      // Anything that arrived before the gesture still deserves to be heard.
      for (const block of this._pending) this.push(block.left, block.right);
      this._pending = [];
      return true;
    } catch (error) {
      this.onError(`audio start failed: ${String(error)}`);
      return false;
    }
  }

  /** Queues one block of stereo PCM. */
  push(left, right) {
    if (!left || left.length === 0) return;
    if (!this.node) {
      // Before the first gesture there is nowhere to play; keep a little so the
      // very first sound is not lost, but do not grow without bound.
      if (this._pending.length < 64) this._pending.push({ left, right });
      return;
    }
    const length = left.length;
    let peak = 0;
    for (let index = 0; index < length; index += 32) {
      const magnitude = left[index] < 0 ? -left[index] : left[index];
      if (magnitude > peak) peak = magnitude;
    }
    if (peak > this.peak) this.peak = peak;

    this.node.port.postMessage({ kind: 'pcm', left, right }, [left.buffer, right.buffer]);
    this.blocks += 1;
    this.queued += length / this.sampleRate;

    // Stay near the target. The producer runs on a timer while playback follows the
    // audio clock, so their rates never match exactly and the queue drifts; dropping
    // the oldest audio costs a click but keeps the delay bounded, whereas letting it
    // grow would desynchronise sound from the picture by seconds. The trim is only
    // reached when the producer is running ahead, so the count doubles as a
    // flow-control readout.
    if (this.queued > MAX_LATENCY) {
      this.node.port.postMessage({ kind: 'clear' });
      this.queued = 0;
      this.trims = (this.trims || 0) + 1;
      // Reported once per second at most, not per block, so the log stays readable.
      if (this.trims === 1 || this.trims % 50 === 0) {
        this.onLog(`audio: queue trimmed ${this.trims} time(s) — producer is ahead of playback`);
      }
    }
  }

  /** Drops queued audio, for when the engine jumps (a seek, or a new track). */
  clear() {
    if (this.node) this.node.port.postMessage({ kind: 'clear' });
    this._pending = [];
    this.queued = 0;
  }

  stats() {
    return {
      started: this.started,
      state: this.context ? this.context.state : 'none',
      blocks: this.blocks,
      queuedSeconds: Number(this.queued.toFixed(3)),
      peak: Number(this.peak.toFixed(4)),
      targetLatency: TARGET_LATENCY,
    };
  }
}
