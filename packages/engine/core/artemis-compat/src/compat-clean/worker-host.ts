import type { GameSource } from '@online-gal/engine-api';
import { AudioRelay } from '../compat/audio-relay.js';
import { CompatReadHost } from '../compat/read-host.js';
import { allocateCompatBlock, type CompatFileDescriptor, COMPAT_SAVE_MAX_FILE_BYTES, COMPAT_SAVE_MAX_FILES, COMPAT_SAVE_MAX_TOTAL_BYTES, COMPAT_SAVE_SLOT } from '../compat/protocol.js';

export interface CleanEngineUrls {
  readonly workerUrl: string;
  readonly engineJsUrl: string;
  readonly wasmUrl: string;
  /** Independent classic helper, or omitted when D concatenates worker-audio.js before the owner. */
  readonly audioHelperUrl?: string;
}
export interface CleanFile { readonly path: string; readonly data: Uint8Array }
export const CLEAN_STORE_PREFIX = 'artemis-compat/';

type Reply = Record<string, any>;
interface Pending { type: string; resolve: (reply: Reply) => void; reject: (error: Error) => void; timer: ReturnType<typeof setTimeout> }

/** One worker, one request identity space. Frames never accumulate demand or self-repeat on ACK. */
export class CleanWorkerHost {
  private worker: Worker | null = null;
  private reader: CompatReadHost | null = null;
  private epoch = 1;
  private nextId = 1;
  private pending = new Map<number, Pending>();
  private frameFlight: Promise<Reply> | null = null;
  private queue: Promise<void> = Promise.resolve();
  private depth = 0;
  private closed = false;
  private paused = false;
  private audioEpoch = 1;
  private audioFlight = false;
  private stats: Reply = {};
  private readonly speaker = new AudioRelay({ onConsumed: (handle, frames) => {
    if (!this.closed) this.post('audio-ack', { handle, frames, audioEpoch: this.audioEpoch });
  } });
  private released: Promise<void> = Promise.resolve();

  constructor(private readonly source: GameSource, private readonly urls: CleanEngineUrls,
    private readonly onFatal: (reason: string) => void, private readonly onExit: () => void,
    private readonly onLog: (level: 'info' | 'error', line: string) => void,
    private readonly muted: () => boolean) {}

  get busy(): boolean { return this.depth !== 0; }
  get alive(): boolean { return !this.closed; }
  describe(): Reply { return { ...this.stats, busy: this.busy, closed: this.closed, audio: this.speaker.stats() }; }
  unlockAudio(): Promise<void> { return this.speaker.unlock(); }

  async load(canvas: OffscreenCanvas, descriptors: readonly CompatFileDescriptor[],
    surface: { width: number; height: number }, saved: readonly CleanFile[], loose: AsyncIterable<CleanFile>): Promise<void> {
    if (this.worker || this.closed) throw new Error('clean worker cannot reopen');
    const block = allocateCompatBlock();
    this.reader = new CompatReadHost(this.source, block, descriptors);
    try {
      this.worker = new Worker(this.urls.workerUrl, { type: 'classic', name: 'artemis-clean' });
      this.worker.onmessage = (event: MessageEvent<Reply>) => this.receive(event.data);
      this.worker.onerror = (event) => this.fail(event.message || 'clean worker error');
      this.worker.onmessageerror = () => this.fail('clean worker message decode failed');
      await this.rpc('init', { ...this.urls, canvas, descriptors, buffer: block.buffer, surface }, [canvas]);
      // Sequential source reads/writes: no temporary directory-sized array. MEMFS itself is cumulative.
      for await (const file of loose) {
        if (this.closed) throw new Error('load disposed during loose preparation');
        plainPath(file.path);
        await this.rpc('loose', { file });
      }
      await this.rpc('open', { files: validateFiles(saved, false) });
      const first = await this.frame();
      if (!first || first.started !== 1 || first.running !== true) throw new Error('clean first frame did not boot');
    } catch (error) {
      this.dispose();
      throw error;
    }
  }

  /** Called by the host's existing single rAF tick. Busy ticks are dropped, not mailboxed. */
  frame(): Promise<Reply> | null {
    if (this.closed || this.paused || this.busy || this.frameFlight) return null;
    const run = this.rpc('frame', {}).then((reply) => {
      this.stats = reply;
      if (reply.running === false) { this.dispose(); this.onExit(); }
      else if (!this.closed) void this.pullAudio(reply.voices ?? []);
      return reply;
    });
    this.frameFlight = run;
    void run.finally(() => { if (this.frameFlight === run) this.frameFlight = null; }).catch(() => undefined);
    return run;
  }

  operation<T>(work: () => Promise<T>): Promise<T> {
    this.depth++;
    const run = this.queue.then(async () => {
      await this.frameFlight;
      if (this.closed) throw new Error('clean worker disposed');
      return work();
    }).finally(() => { this.depth--; });
    this.queue = run.then(() => undefined, () => undefined);
    return run;
  }

  setPaused(paused: boolean): Promise<void> {
    this.paused = paused;
    return this.operation(async () => {
      await this.rpc('pause', { paused });
      await (paused ? this.speaker.pause() : this.speaker.resume());
    });
  }
  input(input: Reply): void { if (!this.closed && !this.paused && !this.busy) this.post('input', { input }); }
  resize(width: number, height: number, backingWidth: number, backingHeight: number): void {
    if (!this.closed) this.post('resize', { width, height, backingWidth, backingHeight });
  }
  async checkpoint(): Promise<readonly CleanFile[]> {
    const reply = await this.rpc('save', {});
    return validateFiles(reply.files, true);
  }
  async restore(files: readonly CleanFile[]): Promise<void> {
    this.audioEpoch++;
    this.speaker.clear();
    await this.rpc('load', { files: validateFiles(files, true), audioEpoch: this.audioEpoch });
  }

  private async pullAudio(voices: Reply[]): Promise<void> {
    if (this.audioFlight || this.closed || this.paused || this.busy) return;
    const epoch = this.audioEpoch;
    this.audioFlight = true;
    try {
      this.speaker.syncHandles(voices.map((v) => v.handle));
      // Do not decode before a real user gesture has opened the speaker.
      if (this.speaker.stats().contextState !== 'running') return;
      const requests = voices.map((v) => ({ handle: v.handle, frames: this.speaker.credit(v.handle, v.rate) }));
      const reply = await this.rpc('audio-pull', { requests, audioEpoch: epoch });
      if (this.closed || epoch !== this.audioEpoch) return;
      for (const item of reply.blocks) this.speaker.push({ ...item, gain1000: this.muted() ? 0 : item.gain1000 });
    } catch (error) { if (!this.closed) this.fail(String(error)); }
    finally { this.audioFlight = false; }
  }

  private receive(reply: Reply): void {
    if (this.closed || !reply || reply.epoch !== this.epoch) return;
    if (reply.type === 'read') {
      void this.reader?.answer({ fileId: reply.fileId, offset: reply.offset, length: reply.length, sequence: reply.sequence, epoch: reply.readEpoch }).then((count) => {
        if (count === -2 && !this.closed) this.fail(this.reader?.lastReason ?? 'PF8 range failed');
      }).catch((error) => this.fail(String(error)));
      return;
    }
    if (reply.type === 'log') { try { this.onLog(reply.level === 'error' ? 'error' : 'info', String(reply.line)); } catch { /* diagnostic only */ } return; }
    if (reply.type === 'fatal') { this.fail(String(reply.reason)); return; }
    const pending = this.pending.get(reply.id);
    if (!pending) return;
    this.pending.delete(reply.id); clearTimeout(pending.timer);
    if (reply.type === 'error') pending.reject(new Error(String(reply.reason)));
    else if (reply.type !== pending.type + '-ok') { pending.reject(new Error('mismatched clean reply')); this.fail('mismatched clean reply'); }
    else pending.resolve(reply);
  }
  private post(type: string, data: Reply): void {
    try { this.worker?.postMessage({ ...data, type, epoch: this.epoch }); }
    catch (error) { this.fail(String(error)); }
  }
  private rpc(type: string, data: Reply, transfer: Transferable[] = []): Promise<Reply> {
    if (this.closed || !this.worker) return Promise.reject(new Error('clean worker absent'));
    const id = this.nextId++;
    return new Promise((resolve, reject) => {
      const timer = setTimeout(() => this.fail(`clean ${type} reply timeout`), 30_000);
      this.pending.set(id, { type, resolve, reject, timer });
      try { this.worker!.postMessage({ ...data, type, id, epoch: this.epoch }, transfer); }
      catch (error) { this.fail(String(error)); }
    });
  }
  private fail(reason: string): void {
    if (this.closed) return;
    this.dispose(); try { this.onFatal(reason); } catch { /* terminal notification */ }
  }
  dispose(): void {
    if (this.closed) return;
    this.closed = true; this.epoch++; this.audioEpoch++;
    this.reader?.close(); this.reader = null;
    for (const pending of this.pending.values()) { clearTimeout(pending.timer); pending.reject(new Error('clean worker disposed')); }
    this.pending.clear();
    const worker = this.worker; this.worker = null;
    const nativeReleased = worker ? new Promise<void>((resolve) => {
      let ended = false;
      const finish = () => {
        if (ended) return;
        ended = true; clearTimeout(timer);
        worker.onmessage = null; worker.onerror = null; worker.onmessageerror = null;
        try { worker.terminate(); } finally { resolve(); }
      };
      const timer = setTimeout(finish, 1000);
      worker.onmessage = (event) => { if (event.data?.type === 'disposed') finish(); };
      worker.onerror = finish; worker.onmessageerror = finish;
      try { worker.postMessage({ type: 'dispose', epoch: this.epoch - 1 }); } catch { finish(); }
    }) : Promise.resolve();
    this.released = Promise.all([nativeReleased, this.speaker.dispose().catch((error) => {
      try { this.onLog('error', `audio dispose: ${String(error)}`); } catch { /* diagnostic only */ }
    })]).then(() => undefined);
  }
  whenDisposed(): Promise<void> { return this.released; }
}

export function plainPath(path: string): void {
  if (typeof path !== 'string' || !path || /[\\\0:]/.test(path) || path.split('/').some((s) => !s || s === '.' || s === '..')) throw new Error(`unsafe relative path: ${String(path)}`);
}
export function validateFiles(files: readonly CleanFile[], requireSlot: boolean): readonly CleanFile[] {
  if (!Array.isArray(files) || files.length > COMPAT_SAVE_MAX_FILES) throw new Error('invalid save file count');
  const seen = new Set<string>(); let total = 0; let slot = false;
  for (const file of files) {
    plainPath(file.path);
    if (!(file.data instanceof Uint8Array) || file.data.byteLength > COMPAT_SAVE_MAX_FILE_BYTES ||
        (typeof SharedArrayBuffer === 'function' && file.data.buffer instanceof SharedArrayBuffer)) throw new Error('invalid save bytes');
    const key = file.path.toLowerCase();
    if (seen.has(key) || /\.pfs(\.\d+)?$/i.test(key)) throw new Error('duplicate or pack save path');
    seen.add(key); total += file.data.byteLength;
    if (file.path === COMPAT_SAVE_SLOT) { slot = true; if (requireSlot) validateSlot(file.data); }
  }
  if (total > COMPAT_SAVE_MAX_TOTAL_BYTES || (requireSlot && !slot)) throw new Error('save bounds or slot missing');
  return files;
}
/** Clean upstream ARCV only: no old patched scene-trailer decoder. Native validates the bank/CRC. */
function validateSlot(data: Uint8Array): void {
  if (data.length < 16 || String.fromCharCode(...data.subarray(0, 4)) !== 'ARCV') throw new Error('not an ARCV v1 slot');
  const view = new DataView(data.buffer, data.byteOffset, data.byteLength);
  if (view.getUint32(4, true) !== 1 || view.getUint32(8, true) > 65536) throw new Error('invalid ARCV version/count');
  let position = 12;
  const keys = new Set<string>();
  for (let i = 0; i < view.getUint32(8, true); i++) {
    for (let field = 0; field < 2; field++) {
      if (data.length - position < 4) throw new Error('truncated ARCV length');
      const size = view.getUint32(position, true); position += 4;
      if (size > (field ? 16777216 : 1048576) || size > data.length - position) throw new Error('invalid ARCV span');
      if (!field) {
        let key = ''; for (let j = 0; j < size; j++) key += String.fromCharCode(data[position + j]!);
        if (keys.has(key)) throw new Error('duplicate ARCV key'); keys.add(key);
      }
      position += size;
    }
  }
  if (data.length - position !== 4) throw new Error('unexpected ARCV trailer');
  let crc = 0xffffffff;
  for (let i = 0; i < position; i++) { crc ^= data[i]!; for (let b = 0; b < 8; b++) crc = (crc >>> 1) ^ (0xedb88320 & -(crc & 1)); }
  if (view.getUint32(position, true) !== ((crc ^ 0xffffffff) >>> 0)) throw new Error('ARCV CRC mismatch');
}
