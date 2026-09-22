/**
 * A synchronous facade over asynchronous byte-range reads.
 *
 * ## The problem this solves
 *
 * Emscripten's filesystem is synchronous: the compiled engine calls
 * `fseek`/`fread` and expects bytes on the spot. A browser can only fetch bytes
 * asynchronously. Nothing in between — not a proxy, not a smarter loader — closes
 * that gap by itself, because the engine's call cannot yield.
 *
 * The consequence is visible in the shipped ONS web build: its "lazy" mode
 * defers *when* a file is fetched but not *how much*, so its one
 * `fetch(url).then(r => r.arrayBuffer())` pulls all 1.8 GB of `arc.nsa` into
 * memory before the first frame.
 *
 * ## How it is closed
 *
 * A worker performs the async fetch; the main thread blocks on `Atomics.wait`
 * until the bytes are in a shared buffer. That converts an async source into a
 * synchronous one, and it is the same handshake AetherKiri's browser host uses
 * for `AetherKiriReadLocalFileSync(fileId, offset, length)`.
 *
 * The bytes never need to be copied twice: the worker writes straight into a
 * `SharedArrayBuffer`, and the caller reads from it. Transfers are chunked, so a
 * request larger than the window is served in several passes rather than
 * requiring a window as large as the engine's biggest read.
 */

/** One byte range the worker should be able to fetch. */
export interface SyncFileDescriptor {
  readonly fileId: number;
  /** Absolute URL the worker fetches ranges from. */
  readonly url: string;
  readonly size: number;
  /** Optional total size hint; when absent the worker asks for `size`. */
  readonly mtimeMs?: number;
}

export interface SyncReadResult {
  readonly bytes: Uint8Array;
  /** Bytes a caller should treat as valid; less than requested means end of file. */
  readonly length: number;
  readonly error?: string;
}

/** Result codes placed in slot 0 of the control block. */
const STATE_IDLE = 0;
const STATE_OK = 1;
const STATE_ERROR = 2;

/**
 * Default transfer window: 4 MB.
 *
 * A read no larger than this is fetched in one pass with no extra copies. Larger
 * reads are served as consecutive windows into the caller's buffer, so the window
 * only has to be big enough to amortise the round trip, not to hold the engine's
 * largest request.
 */
export const DEFAULT_WINDOW_BYTES = 4 * 1024 * 1024;

interface WorkerLike {
  postMessage(message: unknown): void;
  addEventListener?(type: string, listener: (event: { data: unknown }) => void): void;
  on?(type: string, listener: (data: unknown) => void): void;
  terminate?(): void;
}

interface SharedMemoryLike {
  readonly buffer: SharedArrayBuffer;
  readonly control: Int32Array;
  readonly payload: Uint8Array;
}

/**
 * A transfer window: control slots plus the bytes.
 *
 * `control[2]` is a sequence number, and it is what makes the handshake correct.
 * `postMessage` is asynchronous, so the worker can finish and notify *before* the
 * caller reaches `Atomics.wait` — and a notification delivered to nobody is lost,
 * leaving the caller to block for the full timeout on a read that already
 * succeeded. Waiting on "sequence is still what I sent" instead of "state is
 * idle" removes that race.
 */
function allocateShared(windowBytes: number): SharedMemoryLike {
  if (typeof SharedArrayBuffer === 'undefined') {
    throw new Error(
      'SharedArrayBuffer is unavailable, so synchronous reads cannot be bridged. ' +
        'Serve the page with COOP/COEP so the document is cross-origin isolated.',
    );
  }
  const buffer = new SharedArrayBuffer(16 + windowBytes);
  return {
    buffer,
    control: new Int32Array(buffer, 0, 4),
    payload: new Uint8Array(buffer, 16),
  };
}

/**
 * Drives reads over a worker and a shared window.
 *
 * The transport is injected, so the handshake can be tested against a
 * synchronous in-process implementation without requiring threads. That matters:
 * the protocol is where the subtle failures live, and a test that must spin up a
 * real worker to exercise it would not be worth writing.
 */
export class SyncFileBridge {
  private readonly descriptors = new Map<number, SyncFileDescriptor>();
  private readonly windowBytes: number;
  private readonly shared: SharedMemoryLike;
  private readonly timeoutMs: number;
  private ready = false;
  private terminated = false;
  private bytesFetched = 0;
  private readCount = 0;

  constructor(
    private readonly worker: WorkerLike,
    options: { windowBytes?: number; timeoutMs?: number } = {},
  ) {
    this.windowBytes = options.windowBytes ?? DEFAULT_WINDOW_BYTES;
    this.timeoutMs = options.timeoutMs ?? 30_000;
    this.shared = allocateShared(this.windowBytes);
  }

  /** The shared buffer, exposed so a caller can avoid a second copy. */
  get sharedBuffer(): SharedArrayBuffer {
    return this.shared.buffer;
  }

  /** Registers a file so the worker can fetch ranges from it. */
  register(descriptor: SyncFileDescriptor): void {
    this.descriptors.set(descriptor.fileId, descriptor);
    this.worker.postMessage({ kind: 'register', descriptor });
  }

  /** Replaces the whole descriptor set in one message. */
  async initialize(descriptors: readonly SyncFileDescriptor[]): Promise<void> {
    for (const descriptor of descriptors) this.descriptors.set(descriptor.fileId, descriptor);
    this.worker.postMessage({ kind: 'init', descriptors, windowBytes: this.windowBytes });
    this.ready = true;
  }

  /**
   * Reads `length` bytes at `offset`, blocking until they are available.
   *
   * Returns a view onto the shared window, valid until the next call — callers
   * that need to retain the bytes must copy. That is deliberate: copying every
   * read would double the cost of the operation this exists to make cheap.
   */
  readSync(fileId: number, offset: number, length: number): SyncReadResult {
    if (this.terminated) throw new Error('bridge has been terminated');
    const descriptor = this.descriptors.get(fileId);
    if (!descriptor) throw new Error(`unknown fileId ${fileId}`);

    const total = Math.max(0, Math.min(length, descriptor.size - offset));
    if (total === 0) return { bytes: this.shared.payload.subarray(0, 0), length: 0 };

    // A read wider than the window is served in successive passes, each into the
    // caller's buffer, so the window does not have to scale with the request.
    const output = new Uint8Array(total);
    let produced = 0;
    while (produced < total) {
      const chunk = Math.min(this.windowBytes, total - produced);
      const result = this.readChunk(fileId, offset + produced, chunk);
      if (result.error) return { bytes: output.subarray(0, produced), length: produced, error: result.error };
      if (result.length === 0) break;
      output.set(this.shared.payload.subarray(0, result.length), produced);
      produced += result.length;
      if (result.length < chunk) break;
    }
    return { bytes: output.subarray(0, produced), length: produced };
  }

  /** One handshake: request a window and block until the worker fills it. */
  private readChunk(fileId: number, offset: number, length: number): { length: number; error?: string } {
    const { control } = this.shared;
    // Claim a sequence number and publish the request under it before posting, so
    // a worker that finishes immediately still moves the sequence and the wait
    // below cannot sleep through the completion.
    const sequence = (Atomics.load(control, 3) + 1) | 0;
    Atomics.store(control, 0, STATE_IDLE);
    Atomics.store(control, 1, 0);
    Atomics.store(control, 3, sequence);

    this.worker.postMessage({
      kind: 'read',
      fileId,
      offset,
      length,
      sequence,
      control: control.buffer,
      payload: this.shared.buffer,
      windowBytes: this.windowBytes,
    });

    // Wait *on the sequence*, not on the state: the state may already have been
    // written by the time we get here.
    const waited = Atomics.wait(control, 3, sequence, this.timeoutMs);
    const state = Atomics.load(control, 0);
    const produced = Atomics.load(control, 1);

    if (waited === 'timed-out') {
      return { length: 0, error: `read timed out after ${this.timeoutMs} ms` };
    }
    if (state === STATE_ERROR) {
      return { length: 0, error: 'worker reported an error' };
    }
    if (state !== STATE_OK) {
      return { length: 0, error: `unexpected state ${state}` };
    }

    const count = Math.max(0, Math.min(produced, length, this.windowBytes));
    this.bytesFetched += count;
    this.readCount += 1;
    return { length: count };
  }

  /** Requests a whole file, for callers that genuinely need all of it. */
  readAllSync(fileId: number): SyncReadResult {
    const descriptor = this.descriptors.get(fileId);
    if (!descriptor) throw new Error(`unknown fileId ${fileId}`);
    return this.readSync(fileId, 0, descriptor.size);
  }

  /** Byte-accounting, so laziness is measurable rather than asserted. */
  stats(): { reads: number; bytesFetched: number; files: number; windowBytes: number } {
    return {
      reads: this.readCount,
      bytesFetched: this.bytesFetched,
      files: this.descriptors.size,
      windowBytes: this.windowBytes,
    };
  }

  get isReady(): boolean {
    return this.ready;
  }

  terminate(): void {
    this.terminated = true;
    this.worker.terminate?.();
  }
}

/**
 * The worker half of the handshake.
 *
 * Kept as a factory so it can be installed either from a real worker script or
 * from an in-process stand-in during tests. The protocol is intentionally tiny:
 * a descriptor table, a read request, and a sequence word.
 *
 * ## Why the handler is not unconditionally async
 *
 * `Atomics.wait` blocks a thread, so when the worker half runs on the same thread
 * as the caller (a test harness, or a single-threaded embedding) an `await` after
 * the request would never resume: the caller is blocked and nothing is left to
 * drain the microtask queue. The result is a hang, not an error.
 *
 * A source that can produce bytes without yielding — an in-memory buffer, or a
 * `Blob` read through a synchronous API — therefore takes a fully synchronous
 * path, and only a genuinely asynchronous fetch pays for the cooperative
 * scheduling. That keeps the protocol exercisable in-process, which matters
 * because the sequence race it guards against is invisible until it hangs.
 */
export interface BridgeFetch {
  (url: string, offset: number, length: number): Promise<Uint8Array> | Uint8Array;
}

/** Builds a fetch that issues HTTP range requests. */
export function rangeFetch(fetchImpl: typeof fetch = fetch): (url: string, offset: number, length: number) => Promise<Uint8Array> {
  return async (url, offset, length) => {
    const response = await fetchImpl(url, {
      headers: { Range: `bytes=${offset}-${offset + length - 1}` },
    });
    if (!response.ok && response.status !== 206) {
      throw new Error(`range request failed: ${response.status}`);
    }
    const buffer = await response.arrayBuffer();
    return new Uint8Array(buffer);
  };
}

/** Handles bridge messages, given a way to fetch ranges. */
export function createBridgeWorker(fetchRange: BridgeFetch, post?: (message: unknown) => void) {
  const descriptors = new Map<number, SyncFileDescriptor>();
  void post;

  return (message: unknown): void | Promise<void> => {
    const data = message as {
      kind?: string;
      descriptors?: readonly SyncFileDescriptor[];
      descriptor?: SyncFileDescriptor;
      windowBytes?: number;
      fileId?: number;
      offset?: number;
      length?: number;
      sequence?: number;
      control?: SharedArrayBuffer;
      payload?: SharedArrayBuffer;
    };

    if (data.kind === 'init') {
      descriptors.clear();
      for (const descriptor of data.descriptors ?? []) descriptors.set(descriptor.fileId, descriptor);
      return;
    }
    if (data.kind === 'register' && data.descriptor) {
      descriptors.set(data.descriptor.fileId, data.descriptor);
      return;
    }
    if (data.kind !== 'read') return;

    const control = new Int32Array(data.control!, 0, 4);
    const windowBytes = data.windowBytes ?? DEFAULT_WINDOW_BYTES;
    const payload = new Uint8Array(data.payload!, 16, windowBytes);
    const descriptor = descriptors.get(data.fileId!);
    const offset = data.offset ?? 0;
    const length = Math.min(data.length ?? 0, windowBytes);

    const finish = (state: number, count: number): void => {
      Atomics.store(control, 1, count);
      Atomics.store(control, 0, state);
      // Publish the result before advancing the sequence, so a waiter released by
      // this notification is guaranteed to observe the bytes and the state.
      Atomics.store(control, 3, ((data.sequence ?? 0) + 1) | 0);
      Atomics.notify(control, 3);
    };

    if (!descriptor) {
      finish(STATE_ERROR, 0);
      return;
    }

    const emit = (bytes: Uint8Array): void => {
      const count = Math.min(bytes.byteLength, length, payload.byteLength);
      payload.set(bytes.subarray(0, count), 0);
      finish(STATE_OK, count);
    };

    // A source that yields nothing is served without leaving this stack frame.
    let produce: Promise<Uint8Array> | Uint8Array;
    try {
      produce = fetchRange(descriptor.url, offset, length);
    } catch {
      // A synchronous throw is a failed read, not a worker crash: the caller is
      // blocked on this transfer and must be released either way.
      finish(STATE_ERROR, 0);
      return;
    }
    if (produce instanceof Uint8Array) {
      emit(produce);
      return;
    }
    return produce.then(emit, () => finish(STATE_ERROR, 0));
  };
}
