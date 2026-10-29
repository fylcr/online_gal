import { ArtemisErrnoError, ARTEMIS_ERRNO } from './errno.js';

/**
 * The wasm module's linear memory, which the **host** supplies.
 *
 * The bridge is built with `-sIMPORTED_MEMORY`: `bridge.wasm` declares `a.a` as a *memory import*,
 * not a memory it defines. Instantiation therefore fails outright unless the host hands it one. The
 * declared limits are min 3072 pages, max 65536 — 192 MiB initial, 4 GiB ceiling.
 *
 * This class is deliberately not a `new WebAssembly.Memory({ initial: 3072, maximum: 65536 })` in
 * its constructor. Node tolerates the eager allocation (201,326,592 B, rss ~57 MB because pages are
 * demand-zeroed), but a host that cannot be handed a small memory cannot be unit-tested against a
 * handful of kilobytes of synthetic filesystem, and the real browser host will want the pages sized
 * per game rather than per module. So the memory is a parameter, and every typed view is derived
 * from it here so that `grow` cannot leave a stale detached view behind.
 */

/** The subset of `WebAssembly.Memory` this host needs. Lets tests pass a small memory. */
export interface MemoryLike {
  readonly buffer: ArrayBuffer;
  grow(delta: number): number;
}

export interface ArtemisMemoryOptions {
  /** The wasm memory to wrap. Required — see the note above on why. */
  memory: MemoryLike;
  /** Prefix for the derived view accessors, e.g. `HEAP8`. Purely cosmetic. */
  label?: string;
}

/**
 * Typed views over the wasm heap, re-derived whenever the underlying buffer is replaced.
 *
 * `Memory.prototype.grow` **detaches** the old `ArrayBuffer` and installs a new one. Every view
 * taken before the grow becomes a zero-length view over a detached buffer: reads return `0` or
 * `undefined` instead of throwing, which turns a memory-growth bug into silent data corruption
 * rather than a crash. That is the single nastiest failure mode in this file's neighbourhood, so
 * the views live in one place and `refresh()` is called automatically after every grow.
 */
export class ArtemisMemory {
  readonly memory: MemoryLike;
  readonly label: string;

  /** Byte view. `HEAP8` in the official glue; the unit every pointer is expressed in. */
  u8!: Uint8Array;
  /** Signed byte view, for wasm `i32.load8_s` results. */
  i8!: Int8Array;
  /** 16-bit views. */
  u16!: Uint16Array;
  i16!: Int16Array;
  /** 32-bit views — `HEAP32` in the glue, and the view most pointer arithmetic needs. */
  u32!: Uint32Array;
  i32!: Int32Array;
  /** Float views, used by the timekeeping imports. */
  f32!: Float32Array;
  f64!: Float64Array;
  /** Byte view, for `ArrayBuffer` copies. */
  data!: DataView;

  constructor(options: ArtemisMemoryOptions) {
    this.memory = options.memory;
    this.label = options.label ?? 'HEAP';
    this.refresh();
  }

  /** Size of the heap in bytes. */
  get byteLength(): number {
    return this.memory.buffer.byteLength;
  }

  /** Heap size in 64 KiB wasm pages. */
  get pages(): number {
    return this.memory.buffer.byteLength / 65536;
  }

  /**
   * Re-derive every view from the current buffer. Call after anything that can replace the buffer.
   */
  refresh(): void {
    const buffer = this.memory.buffer;
    this.u8 = new Uint8Array(buffer);
    this.i8 = new Int8Array(buffer);
    this.u16 = new Uint16Array(buffer);
    this.i16 = new Int16Array(buffer);
    this.u32 = new Uint32Array(buffer);
    this.i32 = new Int32Array(buffer);
    this.f32 = new Float32Array(buffer);
    this.f64 = new Float64Array(buffer);
    this.data = new DataView(buffer);
  }

  /**
   * Grow the heap, re-deriving the views. Returns the previous page count, as `grow` does.
   *
   * A wasm `memory.grow` that fails does not throw: it returns `-1`. This throws instead, because a
   * caller that ignored the result would go on to write past the end of a heap it believes it grew,
   * and a host that cannot allocate should not be allowed to fail quietly.
   */
  grow(delta: number): number {
    const previous = this.memory.grow(delta);
    if (previous < 0) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOMEM, `memory.grow(${delta}) refused`);
    }
    this.refresh();
    return previous;
  }

  /**
   * Bounds-check a `[ptr, ptr+len)` range against the current heap.
   *
   * wasm itself traps on an out-of-bounds access, but only once the guest has already committed to
   * the call. Checking first turns a guest bug into a clean errno at the syscall boundary, which is
   * where the guest can still recover.
   */
  assertRange(ptr: number, len: number): void {
    if (!Number.isInteger(ptr) || ptr < 0) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EFAULT, `pointer ${ptr}`);
    }
    if (len < 0 || ptr + len > this.byteLength) {
      throw new ArtemisErrnoError(
        ARTEMIS_ERRNO.EFAULT,
        `range ${ptr}+${len} exceeds heap of ${this.byteLength}`,
      );
    }
  }

  /** Read a NUL-terminated UTF-8 string at `ptr`, as `UTF8ToString` does in the glue. */
  utf8(ptr: number): string {
    if (ptr < 0 || ptr >= this.byteLength) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EFAULT, `string pointer ${ptr}`);
    }
    const bytes = this.u8.subarray(ptr);
    const end = bytes.indexOf(0);
    const slice = end < 0 ? bytes : bytes.subarray(0, end);
    return new TextDecoder('utf-8').decode(slice);
  }

  /**
   * Write `text` as UTF-8 plus a NUL at `ptr`, bounded by `maxBytes`. Returns the byte count
   * written **excluding** the NUL, or `-1` if the string does not fit.
   */
  writeUtf8(ptr: number, text: string, maxBytes: number): number {
    const encoded = new TextEncoder().encode(text);
    if (encoded.length + 1 > maxBytes) {
      return -1;
    }
    this.assertRange(ptr, encoded.length + 1);
    this.u8.set(encoded, ptr);
    this.u8[ptr + encoded.length] = 0;
    return encoded.length;
  }
}

/**
 * Create the memory the bridge actually asks for: 3072 pages minimum, 65536 pages maximum.
 *
 * 3072 pages is what the module's own import declaration demands, so anything less fails
 * instantiation. The upper bound is the module's declared maximum and cannot be exceeded by `grow` —
 * the engine cannot be given more than it was compiled to address.
 */
export function createArtemisMemory(initialPages = 3072, maximumPages = 65536): ArtemisMemory {
  return new ArtemisMemory({
    memory: new WebAssembly.Memory({ initial: initialPages, maximum: maximumPages }),
    label: 'HEAP',
  });
}
