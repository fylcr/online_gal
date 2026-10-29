import type { ArtemisMemory } from './memory.js';
import type { ArtemisFs, DirentRecord, StatFields } from './tree.js';
import { ARTEMIS_ERRNO, ArtemisErrnoError } from './errno.js';
import { MutableByteSource } from './fs.js';

/**
 * The byte layouts the guest reads straight out of the heap.
 *
 * These are not negotiable and not self-describing: the compiled C code has a `struct stat` and a
 * `struct linux_dirent64` with fixed offsets, and a host that writes the fields in a tidier order
 * produces a guest that reads a plausible-looking number that is actually `nlink`. Every offset
 * below was read out of the official glue's own `SYSCALLS.doStat` and `___sys_getdents64`, which
 * is the only authority on what the compiled code expects.
 */

/** `sizeof(struct stat64)` as musl lays it out for a 32-bit target. */
export const STAT64_SIZE = 96;

/** Byte offsets within the guest's `struct stat64`. */
export const STAT64_OFFSET = {
  dev: 0,
  ino: 8,
  mode: 12,
  nlink: 16,
  uid: 20,
  gid: 24,
  rdev: 28,
  size: 40,
  blksize: 48,
  blocks: 52,
  atime: 56,
  mtime: 64,
  ctime: 72,
} as const;

/**
 * `sizeof(struct linux_dirent64)` — 19 bytes of header plus a NUL-padded name.
 *
 * The value 280 is not a struct size; it is the *stride* the official `___sys_getdents64` uses,
 * which is the name field padded out to 256 bytes. Getting this wrong makes `readdir` appear to
 * work and then return garbage on the second call, because the resume index is
 * `llseek / 280`.
 */
export const DIRENT64_STRIDE = 280;
export const DIRENT64_NAME_OFFSET = 19;
export const DIRENT64_NAME_MAX = 256;

/** `offsetof(struct stat64, st_atime)` etc. expressed as {i32 low, i32 high} pairs. */
const TIMESPEC = { sec: 0, nsec: 4 } as const;

/**
 * Write a `struct stat64` at `buf`.
 *
 * The high halves of every 64-bit field are written as zero, and the times are seconds with a zero
 * nanosecond field — the glue does the same (`HEAP32[tp>>>2]=now/1e3|0`,
 * `HEAP32[tp+4>>>2]=now%1e3*1e6|0` is the *clock* convention; `stat` itself divides by 1e3 and
 * zeroes the upper word). A 32-bit guest that only ever reads the low word cannot tell the
 * difference, and writing a plausible nanosecond count would make `st_mtim` differ from `st_mtime`
 * in a way no musl consumer expects.
 */
export function writeStat64(memory: ArtemisMemory, buf: number, stat: StatFields): void {
  memory.assertRange(buf, STAT64_SIZE);
  const { u32, f64 } = memory;
  const base = buf >>> 0;
  u32[(base + STAT64_OFFSET.dev) >>> 2] = stat.dev >>> 0;
  u32[(base + 4) >>> 2] = 0; // pad
  // st_ino and st_dev/st_size are 64-bit; musl's 32-bit stat64 exposes them as lo/hi pairs.
  u32[(base + STAT64_OFFSET.ino) >>> 2] = stat.ino >>> 0;
  u32[(base + (STAT64_OFFSET.ino + 4)) >>> 2] = 0;
  u32[(base + STAT64_OFFSET.mode) >>> 2] = stat.mode >>> 0;
  u32[(base + STAT64_OFFSET.nlink) >>> 2] = stat.nlink >>> 0;
  u32[(base + STAT64_OFFSET.uid) >>> 2] = stat.uid >>> 0;
  u32[(base + STAT64_OFFSET.gid) >>> 2] = stat.gid >>> 0;
  u32[(base + STAT64_OFFSET.rdev) >>> 2] = stat.rdev >>> 0;
  u32[(base + 32) >>> 2] = 0; // pad
  // st_size as an i64 lo/hi pair rather than an f64 write: the same 8 bytes, but a double cannot
  // represent every 64-bit integer and root.pfs is 2,690,454,559 bytes.
  u32[(base + STAT64_OFFSET.size) >>> 2] = stat.size >>> 0;
  u32[(base + (STAT64_OFFSET.size + 4)) >>> 2] = Math.floor(stat.size / 4294967296) >>> 0;
  u32[(base + STAT64_OFFSET.blksize) >>> 2] = stat.blksize >>> 0;
  u32[(base + STAT64_OFFSET.blocks) >>> 2] = stat.blocks >>> 0;
  writeTime64(memory, base + STAT64_OFFSET.atime, stat.atime);
  writeTime64(memory, base + STAT64_OFFSET.mtime, stat.mtime);
  writeTime64(memory, base + STAT64_OFFSET.ctime, stat.ctime);
  // musl keeps a second ino at the end for its own bookkeeping; the glue writes it too.
  u32[(base + 80) >>> 2] = stat.ino >>> 0;
  u32[(base + 84) >>> 2] = 0;
  void f64;
}

function writeTime64(memory: ArtemisMemory, at: number, seconds: number): void {
  memory.u32[(at + TIMESPEC.sec) >>> 2] = seconds >>> 0;
  memory.u32[(at + TIMESPEC.nsec) >>> 2] = 0;
}

/**
 * Write one `linux_dirent64` record at `dirp + pos`. Returns the number of bytes consumed.
 *
 * `d_off` is a real cursor, not an index into the record array: the official body stores
 * `(idx + 1) * 280` so that a caller which seeks forward can resume. The `name` is NUL-terminated
 * and padded to the full 256-byte field, so the stride stays constant whatever the name's length.
 */
export function writeDirent64(
  memory: ArtemisMemory,
  dirp: number,
  pos: number,
  record: DirentRecord,
  nextOffset: number,
): number {
  if (pos + DIRENT64_STRIDE > memory.byteLength) {
    throw new ArtemisErrnoError(ARTEMIS_ERRNO.EFAULT, `dirent at ${dirp}+${pos}`);
  }
  const at = (dirp >>> 0) + pos;
  memory.u32[at >>> 2] = record.ino >>> 0; // d_ino lo
  memory.u32[(at + 4) >>> 2] = 0; // d_ino hi
  memory.u32[(at + 8) >>> 2] = nextOffset >>> 0; // d_off lo
  memory.u32[(at + 12) >>> 2] = 0; // d_off hi
  memory.u16[(at + 16) >>> 1] = DIRENT64_STRIDE;
  memory.u8[at + 18] = record.type;
  const encoded = new TextEncoder().encode(record.name);
  const limit = Math.min(encoded.length, DIRENT64_NAME_MAX - 1);
  memory.u8.set(encoded.subarray(0, limit), at + DIRENT64_NAME_OFFSET);
  memory.u8[at + DIRENT64_NAME_OFFSET + limit] = 0;
  return DIRENT64_STRIDE;
}

/**
 * `readv(2)` / `writev(2)`: scatter one read or write across an iovec array, **advancing the
 * stream position**.
 *
 * The iovec is `{ base: i32, len: i32 }` pairs, 8 bytes each, so element `i` lives at `iov + i*8`.
 * The glue's `doReadv` stops at the first short read and returns the total; `doWritev` does not,
 * because a short write on a regular file is not a reason to abandon the rest of the vector. That
 * asymmetry is reproduced here deliberately — a host that "helpfully" breaks out of the write
 * loop would drop data on a large save.
 *
 * ## Why this advances, and why that is not a detail
 *
 * Emscripten's `FS.read` is `var seeking = typeof position !== "undefined"; if (!seeking)
 * { position = stream.position } … if (!seeking) { stream.position += bytesRead }`, and the glue
 * calls `SYSCALLS.doReadv(stream, iov, iovcnt)` with **no** fourth argument — so `seeking` is
 * `false` and every byte read moves the position. `_fd_read` is the plain advancing readv, not
 * `preadv`: the four-argument `(fd, iov, iovcnt, pnum)` row has no offset parameter to pass on.
 *
 * Implementing it as `preadv` instead is silent and fatal. The Artemis engine opens `/root.pfs`
 * and reads the container **without ever seeking** — 1,026 bytes, then the rest of the index —
 * relying on the second read continuing where the first stopped. A non-advancing host serves the
 * same region twice, the record-table parser walks misaligned bytes, and the engine dies in
 * boost's `unordered_map` with `Assertion failed: mlf_ >= minimum_max_load_factor`, thousands of
 * frames deep in a library that never saw a bad byte from us. Measured on the real 2.69 GB
 * `root.pfs`: the trace shows `[0, 1026)` followed by `[0, 1185779)` with no `lseek` in between,
 * and the index is exactly 1,185,775 bytes starting at offset 7.
 *
 * Returns the total bytes transferred, or throws the errno the kernel would.
 */
export function doReadv(
  memory: ArtemisMemory,
  fs: ArtemisFs,
  fd: number,
  iov: number,
  iovcnt: number,
): number {
  const stream = fs.requireStream(fd);
  const position = stream.position;
  let total = 0;
  for (let i = 0; i < iovcnt; i += 1) {
    const entry = (iov >>> 0) + i * 8;
    memory.assertRange(entry, 8);
    const base = memory.u32[entry >>> 2] ?? 0;
    const len = memory.u32[(entry + 4) >>> 2] ?? 0;
    if (len === 0) {
      continue;
    }
    memory.assertRange(base, len);
    const view = memory.u8.subarray(base, base + len);
    const got = fs.read(fd, view, position + total, len);
    total += got;
    if (got < len) {
      break;
    }
  }
  stream.position = position + total;
  return total;
}

/** `writev(2)`. Same as {@link doReadv} but without the short-read break. */
export function doWritev(
  memory: ArtemisMemory,
  fs: ArtemisFs,
  fd: number,
  iov: number,
  iovcnt: number,
): number {
  const stream = fs.requireStream(fd);
  const position = stream.position;
  let total = 0;
  for (let i = 0; i < iovcnt; i += 1) {
    const entry = (iov >>> 0) + i * 8;
    memory.assertRange(entry, 8);
    const base = memory.u32[entry >>> 2] ?? 0;
    const len = memory.u32[(entry + 4) >>> 2] ?? 0;
    if (len === 0) {
      continue;
    }
    memory.assertRange(base, len);
    // A copy, not a subarray: `fs.write` appends through a MutableByteSource, and a grow inside
    // that write would otherwise detach the very view we are reading from.
    const view = new Uint8Array(memory.u8.subarray(base, base + len));
    total += fs.write(fd, view, position + total);
  }
  stream.position = position + total;
  return total;
}

/**
 * Flatten a directory into the byte stream `getdents64` produces, honouring `count`.
 *
 * `startIndex` is the resume position the caller derived from its own seek. Returns the bytes
 * written plus the index of the next record, so the caller can store the cursor without
 * re-deriving it. A short final chunk is normal: the kernel reports however many whole records
 * fitted, and the caller loops until it gets 0.
 */
export function packDirents(
  memory: ArtemisMemory,
  records: DirentRecord[],
  dirp: number,
  count: number,
  startIndex: number,
): { written: number; nextIndex: number } {
  let pos = 0;
  let index = startIndex;
  const limit = Math.max(0, count);
  while (index < records.length && pos + DIRENT64_STRIDE <= limit) {
    const record = records[index]!;
    pos += writeDirent64(memory, dirp, pos, record, (index + 1) * DIRENT64_STRIDE);
    index += 1;
  }
  return { written: pos, nextIndex: index };
}

/** `fsync(2)`. This host writes through to the source, so there is nothing to flush. */
export function fsync(_fs: ArtemisFs, _fd: number): void {
  /* writes are already visible to every reader */
}

/** True when the node's bytes may be written. Re-exported so `fd.ts` needs one import. */
export function isMutable(node: { data: unknown } | null): boolean {
  return node !== null && node.data instanceof MutableByteSource;
}
