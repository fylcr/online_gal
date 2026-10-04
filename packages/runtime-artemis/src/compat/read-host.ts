/**
 * Page half of the blocking range transport.
 * The worker publishes a sequence, posts `{type:'read'}` and blocks in Atomics.wait; this
 * thread only calls `source.readRange`, then copies into the shared window and wakes it.
 */
import type { GameSource } from '@online-gal/engine-api';
import {
  CONTROL,
  READ_FATAL,
  READ_MISSING,
  type CompatFileDescriptor,
  type CompatSharedBlock,
  storeOffset,
} from './protocol.js';

export interface CompatReadRequest {
  readonly fileId: number;
  readonly offset: number;
  readonly length: number;
  readonly sequence: number;
  /**
   * The epoch the worker stamped before posting. Optional because a hand-written caller may omit it;
   * `answer` then reads the current word, which is the only epoch it could mean.
   */
  readonly epoch?: number;
}

const INT32_MAX = 0x7fffffff;
const U32_MAX = 0xffffffff;
const PLAIN_ROOT = '/game';

export class CompatReadHost {
  private readonly source: GameSource;
  private readonly block: CompatSharedBlock;
  private readonly descriptors = new Map<number, CompatFileDescriptor>();
  private closed = false;
  /** Sequence currently being answered, so a second read cannot publish over it. */
  private serving: number | null = null;
  /** Why the last fatal status happened. The caller reports it; the window only carries a word. */
  lastReason: string | null = null;

  constructor(source: GameSource, block: CompatSharedBlock, descriptors: readonly CompatFileDescriptor[]) {
    this.source = source;
    this.block = block;
    for (const descriptor of descriptors) {
      const problem = descriptorError(descriptor, this.descriptors);
      // A bad table fails at construction, where the caller still knows which descriptor it built.
      if (problem) throw new Error(problem);
      this.descriptors.set(descriptor.id, descriptor);
    }
  }

  /** Answers one worker read. Returns the byte count published, READ_MISSING or READ_FATAL. */
  async answer(request: CompatReadRequest): Promise<number> {
    const sequence = request.sequence;
    const rawEpoch = request.epoch;
    // Raw signed int32, before `| 0`. `1.5` and `2^31` are not the word the worker published.
    if (!signed32(sequence)) {
      this.lastReason = 'malformed read sequence';
      return READ_FATAL;
    }
    if (rawEpoch !== undefined && !signed32(rawEpoch)) {
      this.lastReason = 'malformed read epoch';
      return READ_FATAL;
    }
    const epoch = Atomics.load(this.block.control, CONTROL.EPOCH) | 0;
    const posted = Atomics.load(this.block.control, CONTROL.SEQUENCE) | 0;
    // Stale before `serving` is touched: a late request must not clear the read in flight or write COUNT.
    if (posted !== sequence || (rawEpoch !== undefined && rawEpoch !== epoch)) {
      this.lastReason = posted !== sequence
        ? `stale read sequence ${sequence}, the worker is waiting on ${posted}`
        : `stale read epoch ${String(rawEpoch)}, the live one is ${epoch}`;
      return READ_FATAL;
    }
    if (this.serving !== null) {
      this.lastReason = 'read already in flight';
      return READ_FATAL;
    }
    if (this.closed) return this.reject(sequence, epoch, READ_FATAL, 'host closed');
    const capacity = this.block.payload.length;
    const fileId = request.fileId;
    const offset = request.offset;
    const length = request.length;
    // Every field is the number that arrived. `1.5` and `2^32 + 1` are not lengths.
    if (!exactInt(fileId, INT32_MAX) || fileId < 1) return this.reject(sequence, epoch, READ_FATAL, 'malformed file id');
    if (!exactInt(length, U32_MAX) || length < 1) return this.reject(sequence, epoch, READ_FATAL, 'malformed read length');
    if (!Number.isSafeInteger(offset) || offset < 0 || offset > U32_MAX) {
      return this.reject(sequence, epoch, READ_FATAL, 'malformed read offset');
    }
    const descriptor = this.descriptors.get(fileId);
    if (!descriptor) return this.reject(sequence, epoch, READ_MISSING, 'unknown file id');
    if (length > capacity) {
      return this.reject(sequence, epoch, READ_FATAL, `read of ${length} B does not fit the ${capacity}-byte window`);
    }
    // `offset + length <= 2^32` by subtraction. JS addition does not wrap; the file may exceed 4 GiB.
    if (length > U32_MAX - offset + 1) {
      return this.reject(sequence, epoch, READ_FATAL, 'read leaves the 32-bit address space');
    }
    if (length > descriptor.size - offset) {
      return this.reject(sequence, epoch, READ_FATAL, `read past end of ${descriptor.virtualPath}`);
    }
    if (typeof this.source.readRange !== 'function') {
      return this.reject(sequence, epoch, READ_FATAL, 'source cannot read ranges');
    }
    this.serving = sequence;
    let bytes: Uint8Array;
    try {
      bytes = await this.source.readRange(descriptor.path, offset, length);
    } catch (error) {
      return this.reject(sequence, epoch, READ_FATAL, `source error: ${text(error)}`);
    }
    // The worker may have timed out and bumped the epoch, or moved on. Never write over a newer read.
    // Checked before the type gate as well, so a late answer of any shape stays a late answer.
    const postedNow = Atomics.load(this.block.control, CONTROL.SEQUENCE) | 0;
    const epochNow = Atomics.load(this.block.control, CONTROL.EPOCH) | 0;
    this.serving = null;
    if (this.closed || epochNow !== epoch || postedNow !== sequence) {
      this.lastReason = this.closed ? 'host closed' : 'read superseded before publish';
      return READ_FATAL;
    }
    // A source that answers with something other than bytes is refused, never repaired: a fabricated
    // empty buffer would read as a successful short read and hand the engine zeroes it never stored.
    if (!(bytes instanceof Uint8Array)) {
      return this.reject(sequence, epoch, READ_FATAL, 'source returned a non-Uint8Array');
    }
    if (bytes.byteLength < length) {
      return this.reject(sequence, epoch, READ_FATAL, `short read ${bytes.byteLength}/${length}`);
    }
    this.block.payload.set(bytes.subarray(0, length), 0);
    return this.release(sequence, length);
  }

  /** Drops a late completion. A read already in flight is published as a failure. */
  close(): void {
    if (this.closed) return;
    this.closed = true;
    Atomics.add(this.block.control, CONTROL.EPOCH, 1);
    const sequence = this.serving;
    this.serving = null;
    if (sequence !== null) this.reject(sequence, Atomics.load(this.block.control, CONTROL.EPOCH) | 0, READ_FATAL, 'host closed');
  }

  private reject(sequence: number, epoch: number, status: number, reason: string): number {
    this.serving = null;
    const posted = Atomics.load(this.block.control, CONTROL.SEQUENCE) | 0;
    const epochWord = Atomics.load(this.block.control, CONTROL.EPOCH) | 0;
    if (posted !== sequence || epochWord !== epoch) return status;
    this.lastReason = reason;
    return this.release(sequence, status);
  }

  private release(sequence: number, count: number): number {
    Atomics.store(this.block.control, CONTROL.COUNT, count | 0);
    Atomics.store(this.block.control, CONTROL.SEQUENCE, (sequence + 1) | 0);
    Atomics.notify(this.block.control, CONTROL.SEQUENCE);
    return count;
  }
}

function text(error: unknown): string {
  return error instanceof Error ? error.message : String(error);
}

/** An integer exactly, bounded by `max`. No coercion: `1.5`, `NaN` and `2^32 + 1` are all refused. */
function exactInt(value: unknown, max: number): value is number {
  return typeof value === 'number' && Number.isSafeInteger(value) && value >= 0 && value <= max;
}

/** A signed int32 exactly. `-2147483648` is a wrapped sequence; `1.5` and `2^31` are not. */
function signed32(value: unknown): value is number {
  return typeof value === 'number' && Number.isInteger(value) && value >= -0x80000000 && value <= 0x7fffffff;
}

/**
 * Why a descriptor cannot serve a read, or `null` when it can.
 *
 * The id is what the engine's own `fileIdFor` hands back and what a reply is matched against, so a
 * duplicate id would let the `Map` keep whichever came last and quietly answer for the other file —
 * the pack the engine asked for would be read from a different path. A path that is absolute, or a
 * virtual path outside the plain `/game` root or a `..` segment, is a read of something the pack
 * chain never declared. `size` is the range ceiling and must be a real byte count.
 */
function descriptorError(
  descriptor: CompatFileDescriptor,
  seen: ReadonlyMap<number, CompatFileDescriptor>,
): string | null {
  if (!exactInt(descriptor.id, INT32_MAX) || descriptor.id < 1) return `descriptor id ${String(descriptor.id)} is not positive`;
  if (seen.has(descriptor.id)) return `descriptor id ${descriptor.id} is declared twice`;
  // `path` is handed straight to `source.readRange`, so it is strictly relative: an absolute path, a
  // drive letter, a backslash or a `..` segment is a read of something the pack chain never declared.
  const problem = plainPath('path', descriptor.path);
  if (problem) return `descriptor ${descriptor.id} ${problem}`;
  // `virtualPath` is rooted at the one directory the transport declares, so it is checked as a
  // rooted path rather than as a relative one.
  const rooted = plainPath('virtualPath', descriptor.virtualPath, PLAIN_ROOT);
  if (rooted) return `descriptor ${descriptor.id} ${rooted}`;
  for (const prior of seen.values()) {
    if (prior.virtualPath === descriptor.virtualPath) return `descriptor virtual path ${descriptor.virtualPath} is declared twice`;
  }
  if (!Number.isSafeInteger(descriptor.size) || descriptor.size < 0) {
    return `descriptor ${descriptor.id} size ${String(descriptor.size)} is not a byte count`;
  }
  return null;
}

/**
 * Why `value` is not a plain path below `root`, or `null` when it is.
 *
 * Segments are what matter, not the string: `a/../b` and `a//b` are not names, and a NUL truncates
 * whatever the far side does with it. `root` is the prefix a rooted path must stay under — empty for
 * a relative path, which is therefore not allowed to start with a separator at all.
 */
function plainPath(name: string, value: unknown, root = ''): string | null {
  if (typeof value !== 'string' || value === '') return `has an empty ${name}`;
  if (value.includes('\0')) return `${name} contains a NUL`;
  if (value.includes('\\')) return `${name} ${value} contains a backslash`;
  if (/^[A-Za-z]:/.test(value)) return `${name} ${value} carries a drive letter`;
  if (root === '') {
    if (value.startsWith('/')) return `${name} ${value} is absolute`;
  } else {
    if (value !== root && !value.startsWith(`${root}/`)) return `${name} ${value} is outside ${root}`;
  }
  // Below the root the path is relative, so the single separator that introduced the first segment is
  // not itself a segment: `/game/root.pfs` slices to `/root.pfs`, whose only real segment is
  // `root.pfs`. Exactly one separator is stripped — a doubled one is an empty segment and is refused.
  // The root on its own has no relative part at all, which is the directory itself.
  const below = root === '' ? value : value.slice(root.length);
  if (below === '' && root !== '') return null;
  const relative = below.startsWith('/') ? below.slice(1) : below;
  for (const segment of relative.split('/')) {
    if (segment === '' || segment === '.' || segment === '..') return `${name} ${value} is not a plain path`;
  }
  return null;
}

export { storeOffset };