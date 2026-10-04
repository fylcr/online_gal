/** Wire constants shared by the page and the classic worker. The worker cannot import this. */

export const COMPAT_PROTOCOL_VERSION = 1;
export const COMPAT_WINDOW_BYTES = 4 * 1024 * 1024;
export const COMPAT_CONTROL_BYTES = 24;

/**
 * Shared words. The worker publishes SEQUENCE, then waits for it to change.
 * COUNT is the synchronous result: byte count, READ_MISSING or READ_FATAL.
 * OFFSET is two unsigned halves, so an offset past 2 GiB never wraps negative.
 */
export const CONTROL = {
  SEQUENCE: 0,
  COUNT: 1,
  FILE: 2,
  OFFSET_LO: 3,
  OFFSET_HI: 4,
  EPOCH: 5,
} as const;

/** No such descriptor / patch path. The engine treats this as an expected absence, not a fault. */
export const READ_MISSING = -1;
/** Short read, source error or out-of-range request. Terminal: the engine must not continue. */
export const READ_FATAL = -2;

/** Production stall budget for one blocking read. Tests may shorten it per init; production may not. */
export const READ_TIMEOUT_MS = 30_000;

/** Native open arguments. Both are `const char *`, so they are marshalled, never passed as raw JS strings. */
export const COMPAT_OPEN_PACK = '/game/root.pfs';
export const COMPAT_OPEN_MODE = 'windows';

/**
 * Where the engine writes its save slots.
 *
 * `EngineContext::Open` sets `save_dir_ = fs::path(pack).parent_path()`, and the pack this transport
 * declares is always `/game/root.pfs` — so the slot lands in `/game`. The engine itself never creates
 * that directory (`ReadSaveFile`/`WriteSaveFile` are plain `ifstream`/`open`), which is why the
 * worker makes it before `artc_web_open`.
 */
export const COMPAT_SAVE_DIR = '/game';
/** The slot name handed to `artc_web_save`. A fixed name, not a user path: the host keys the slot. */
export const COMPAT_SAVE_SLOT = 'platform-slot.dat';

/**
 * Checkpoint payload bounds, matching `save_storage.cpp`'s own `max_bytes = 64 MiB`.
 *
 * The native writer refuses a file past that size, and the reader refuses one on the way back in.
 * The page enforces the same three numbers **before** it mutates anything, so a hostile or corrupt
 * payload is refused by us rather than written to the store and discovered later by a broken restore.
 * The count and sum are the set-level bounds a per-file limit cannot express.
 */
export const COMPAT_SAVE_MAX_FILE_BYTES = 64 * 1024 * 1024;
export const COMPAT_SAVE_MAX_FILES = 4096;
export const COMPAT_SAVE_MAX_TOTAL_BYTES = 256 * 1024 * 1024;

/** The `kind` every compat checkpoint carries, so a reader can tell the shape apart from a foreign slot. */
export const COMPAT_SAVE_KIND = 'artemis-compat-checkpoint';

/**
 * The one slot the transport reads back on the way out, in the engine's own checkpoint format.
 *
 * A checkpoint is only a checkpoint because **this** file is in it. The engine writes it in
 * `EncodeVariableBank(..., checkpoint=true)`: `ARCV`, version 1, the bank, then a CRC32 over everything
 * before it. A payload without that file restores nothing — `artc_web_load` would have to invent the
 * slot — so it is refused instead of persisted as a slot that cannot be loaded.
 */
export const COMPAT_SLOT_HEADER_BYTES = 16;
/** `save_storage.cpp`'s own ceiling on the bank, applied before a single byte is allocated for it. */
export const COMPAT_SLOT_MAX_BANK_BYTES = 64 * 1024 * 1024;

/** Optional CABI. Missing names are not implemented; never invent a success. */
export const OPTIONAL_CABI = [
  '_artc_web_pause',
  '_artc_web_dispose',
  '_artc_web_save',
  '_artc_web_load',
  '_artc_web_now_ms',
] as const;

export interface CompatFileDescriptor {
  readonly id: number;
  /** Source path passed to `GameSource.readRange`, e.g. `root.pfs`. */
  readonly path: string;
  /** Engine virtual path, e.g. `/game/root.pfs`. */
  readonly virtualPath: string;
  /** Total byte size. Every request is range-checked against it. */
  readonly size: number;
}

export interface CompatSharedBlock {
  readonly buffer: SharedArrayBuffer;
  readonly control: Int32Array;
  readonly payload: Uint8Array;
}

export function allocateCompatBlock(windowBytes: number = COMPAT_WINDOW_BYTES): CompatSharedBlock {
  const buffer = new SharedArrayBuffer(COMPAT_CONTROL_BYTES + windowBytes);
  return {
    buffer,
    control: new Int32Array(buffer, 0, COMPAT_CONTROL_BYTES / 4),
    payload: new Uint8Array(buffer, COMPAT_CONTROL_BYTES),
  };
}

/** Unsigned 32-bit load. A signed view of a byte past 2 GiB is negative; this is not. */
export function u32(view: Int32Array, index: number): number {
  return Atomics.load(view, index) >>> 0;
}

export function loadOffset(view: Int32Array): number {
  return u32(view, CONTROL.OFFSET_HI) * 0x100000000 + u32(view, CONTROL.OFFSET_LO);
}

export function storeOffset(view: Int32Array, offset: number): void {
  Atomics.store(view, CONTROL.OFFSET_LO, offset >>> 0);
  Atomics.store(view, CONTROL.OFFSET_HI, Math.floor(offset / 0x100000000) >>> 0);
}