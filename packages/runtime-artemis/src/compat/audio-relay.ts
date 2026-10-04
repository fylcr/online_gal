/** Main-thread per-voice WebAudio queue. Native PcmStream stays the loop; this only schedules blocks. */
type Push = { handle: number; rate: number; samples: Int16Array; gain1000: number; pan1000: number };
type Ctx = AudioContext;
const AHEAD = 0.2;
/** Interleaved Int16 stereo: 4096 frames == 8192 samples. Credit is frames. */
const CAP_FRAMES = 4096, CAP_SAMPLES = CAP_FRAMES * 2;

export class AudioRelay {
  onConsumed: ((handle: number, cumulativeFrames: number) => void) | null = null;
  private factory: () => Ctx;
  private own: boolean;
  private ctx: Ctx | null = null;
  private closed = false;
  private epoch = 0;
  private voices = new Map<number, Voice>();
  private op: Promise<void> = Promise.resolve();
  private paused = false;
  /** Cumulative ACKed frames per handle; it survives a voice rebuild and resets only on clear/dispose. */
  private acked = new Map<number, number>();
  private peak = 0;
  private blocks = 0;
  /** Set when resume() rejected. The context may still say running; the next resume must be asked. */
  private resumeFailed = false;

  constructor(opts: { contextFactory?: () => Ctx; context?: Ctx; onConsumed?: (h: number, f: number) => void } = {}) {
    if (opts.context) { this.ctx = opts.context; this.own = false; this.factory = () => opts.context!; }
    else { this.own = true; this.factory = opts.contextFactory ?? (() => new AudioContext()); }
    this.onConsumed = opts.onConsumed ?? null;
  }

  private ensure(): Ctx {
    if (this.closed) throw new Error('disposed');
    let fresh = false;
    if (!this.ctx) { this.ctx = this.factory(); fresh = true; }
    if (typeof this.ctx.createStereoPanner !== 'function') { this.closed = true; throw new Error('StereoPannerNode unavailable'); }
    if (fresh) this.ctx.onstatechange = () => this.endedFrames();
    return this.ctx;
  }

  push(p: Push): void {
    const { handle, rate, samples, gain1000, pan1000 } = p;
    if (!Number.isSafeInteger(handle) || handle <= 0) throw new Error('bad handle');
    if (!Number.isSafeInteger(rate) || rate <= 0 || rate > 384000) throw new Error('bad rate');
    if (!(samples instanceof Int16Array) || samples.length === 0 || samples.length > CAP_SAMPLES || (samples.length & 1)) throw new Error('bad samples');
    if (!Number.isSafeInteger(gain1000) || gain1000 < 0 || gain1000 > 1000) throw new Error('bad gain');
    if (!Number.isSafeInteger(pan1000) || pan1000 < -1000 || pan1000 > 1000) throw new Error('bad pan');
    const ctx = this.ensure();
    const v = this.voice(handle, rate, ctx);
    if (v.rate !== rate) throw new Error('rate changed');
    const frames = samples.length >> 1;
    if ((v.queuedFrames() + frames) / rate > AHEAD + 1e-9) throw new Error('ahead > 200ms');
    const buf = ctx.createBuffer(2, frames, rate);
    const L = buf.getChannelData(0), R = buf.getChannelData(1);
    let peak = 0;
    for (let i = 0; i < frames; i++) {
      const l = samples[i * 2]!, r = samples[i * 2 + 1]!;
      L[i] = l / 32768; R[i] = r / 32768;
      const a = Math.max(Math.abs(l), Math.abs(r)) / 32768;
      if (a > peak) peak = a;
    }
    const src = ctx.createBufferSource();
    const gain = ctx.createGain(), pan = ctx.createStereoPanner();
    gain.gain.value = gain1000 / 1000; pan.pan.value = pan1000 / 1000;
    src.buffer = buf; src.connect(gain); gain.connect(pan); pan.connect(ctx.destination);
    const when = Math.max(ctx.currentTime, v.next);
    const block: Block = { src, gain, pan, frames, epoch: this.epoch, acked: false };
    v.q.push(block); v.next = when + frames / rate;
    src.onended = () => this.ended(v, block);
    src.start(when);
    if (peak > this.peak) this.peak = peak;
    this.blocks++;
  }

  /** Frames still allowed: 200ms minus queued-not-yet-played, never more than 4096 frames. */
  credit(handle: number, rate: number): number {
    if (!Number.isSafeInteger(handle) || handle <= 0) throw new Error('bad handle');
    if (!Number.isSafeInteger(rate) || rate <= 0) throw new Error('bad rate');
    const v = this.voices.get(handle);
    const ahead = v && v.rate === rate ? v.queuedFrames() : 0;
    return Math.min(CAP_FRAMES, Math.max(0, Math.floor(rate * AHEAD) - ahead));
  }

  syncHandles(current: number[]): void {
    const keep = new Set(current);
    for (const [h, v] of [...this.voices]) if (!keep.has(h)) this.drop(v);
  }

  // Rejections stay with the caller; the chain itself only advances.
  private queue(fn: () => Promise<unknown> | void): Promise<void> {
    const run = this.op.then(fn);
    this.op = run.then(() => undefined, () => undefined);
    return run.then(() => undefined);
  }

  /** Latches the intent even with no context yet, and marks it as this op's intent. */
  pause(): Promise<void> {
    if (this.closed) throw new Error('disposed');
    this.paused = true;
    return this.queue(() => this.act(true));
  }

  /** Unlocks within the gesture turn: builds the context if needed, resumes, and never latches it. */
  unlock(): Promise<void> {
    if (this.closed) throw new Error('disposed');
    const ctx = this.ensure(); // synchronous, inside the gesture; a failed ensure must not be swallowed
    const run = this.queue(() => (this.paused ? undefined : this.resumeCtx(ctx))); // a gesture never cancels an explicit pause
    if (this.closed) throw new Error('disposed');
    return run;
  }

  resume(): Promise<void> {
    this.paused = false;
    return this.queue(() => this.act(false));
  }

  /** Runs the last-called intent against the context: a stale queued op never re-latches it. */
  private act(paused: boolean): Promise<void> | void {
    if (this.paused !== paused) return; // superseded before it ran: it neither suspends nor resumes
    const ctx = this.ctx;
    if (!ctx) return;
    return paused
      ? ctx.suspend().then(() => undefined, err => { this.paused = false; throw err; })
      : this.resumeCtx(ctx);
  }

  private resumeCtx(ctx: Ctx): Promise<void> | void {
    if (ctx.state === 'running' && !this.resumeFailed) { this.endedFrames(); return; }
    return ctx.resume().then(
      () => { this.resumeFailed = false; this.endedFrames(); },
      (err: unknown) => { this.resumeFailed = true; throw err; },
    );
  }

  /** Cancel every voice without ACK. Future restore may call this. */
  clear(): void { this.epoch++; this.acked.clear(); for (const v of [...this.voices.values()]) this.drop(v); }

  dispose(): Promise<void> {
    if (this.closed) return Promise.resolve();
    this.epoch++;
    this.closed = true;
    this.acked.clear();
    for (const v of [...this.voices.values()]) { this.stop(v); this.voices.delete(v.handle); }
    const ctx = this.ctx;
    const own = this.own; this.own = false; // repeated dispose can never close a second time
    if (!own || !ctx) return Promise.resolve();
    return ctx.close();
  }

  stats(): { peak: number; queuedFrames: number; contextState: string; blocks: number } {
    let queuedFrames = 0;
    for (const v of this.voices.values()) queuedFrames += v.queuedFrames();
    return { peak: this.peak, queuedFrames, contextState: this.ctx?.state ?? 'none', blocks: this.blocks };
  }

  private voice(handle: number, rate: number, ctx: Ctx): Voice {
    let v = this.voices.get(handle);
    if (!v) { v = new Voice(handle, rate); this.voices.set(handle, v); }
    return v;
  }

  private ended(v: Voice, block: Block): void {
    if (block.epoch !== this.epoch || this.voices.get(v.handle) !== v || block.acked) return;
    // Marked once, here: a natural end under suspension is remembered and flushed later, never re-awaited.
    block.acked = true;
    this.detach(block);
    this.endedFrames();
  }

  /** ACK the contiguous played prefix once the context is running. Safe to call repeatedly. */
  private endedFrames(): void {
    if (this.closed || !this.ctx || this.ctx.state !== 'running') return;
    for (const v of [...this.voices.values()]) {
      let n = 0;
      while (v.q[0]?.acked) n += v.q.shift()!.frames;
      if (!n) continue;
      const total = (this.acked.get(v.handle) ?? 0) + n;
      this.acked.set(v.handle, total);
      if (this.onConsumed) this.onConsumed(v.handle, total);
    }
  }

  private detach(b: Block): void {
    try { b.src.disconnect(); b.gain.disconnect(); b.pan.disconnect(); } catch { /* already detached */ }
  }

  /** Detach the whole chain and drop the callback so no late onended can ACK. */
  private stop(v: Voice): void {
    for (const b of v.q) {
      b.epoch = -1; b.acked = false; b.src.onended = null;
      try { b.src.stop(); } catch { /* natural end already */ }
      this.detach(b);
    }
    v.q.length = 0;
  }
  private drop(v: Voice): void { this.stop(v); this.voices.delete(v.handle); }
}
class Voice {
  q: Block[] = []; next = 0;
  constructor(readonly handle: number, readonly rate: number) {}
  queuedFrames(): number { let n = 0; for (const b of this.q) n += b.frames; return n; }
}
type Block = { src: AudioBufferSourceNode; gain: GainNode; pan: StereoPannerNode; frames: number; epoch: number; acked: boolean };
