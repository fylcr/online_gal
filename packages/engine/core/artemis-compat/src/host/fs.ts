import { ARTEMIS_ERRNO, ArtemisErrnoError, ArtemisHostError } from './errno.js';

/**
 * A synchronous, random-access byte source.
 *
 * This interface is the seam between "the guest asked for a file" and "@online-gal/vfs is async".
 * Every `GameSource` method returns a Promise, but `_fd_read` must return before the wasm call
 * returns — the engine's C code is compiled against a synchronous POSIX `read`. Something has to
 * break that symmetry, and this type is where the choice is made explicit rather than smeared
 * through the syscall layer.
 *
 * Two honest ways to satisfy it, both used:
 *
 * - **Pre-resolve into memory.** `readAllIntoMemory` walks a `GameSource` with the vfs API and
 *   materialises the bytes. Correct for small files (the package's `system.ini` is 3,618 B, its
 *   smallest entry is 59 B) and for tests.
 * - **Positional host reads.** A synchronous read at a given offset, with no whole-file read
 *   anywhere. Required for the real package: `root.pfs` is 2,690,454,559 B, well past the 2 GiB
 *   limit at which a single Node read fails outright. A positional read of a 64 KiB window never
 *   touches the limit. The adapter lives outside this package (the host declares no node types);
 *   see the note in `README.md`.
 */

/** Synchronous random access to one file's bytes. */
export interface SyncByteSource {
  /** Total length in bytes. */
  size(): number;
  /** Read `length` bytes starting at `offset`. Must not read past the end; a short read is a defect. */
  readRange(offset: number, length: number): Uint8Array;
  /**
   * Write `data` at `offset`, extending the file if needed. Returns the byte count transferred.
   *
   * Optional rather than required, because a read-only mount genuinely cannot support it and
   * saying so in the type is better than discovering it at the first `write(2)`. A source that
   * implements this is treated as writable by `ArtemisFs`; one that does not is EROFS.
   */
  write?(offset: number, data: Uint8Array): number;
  /** Truncate to `size` bytes. Optional for the same reason as `write`. */
  truncate?(size: number): void;
}

export type NodeKind = 'file' | 'dir' | 'link' | 'chrdev';

/** File type bits, as musl's `struct stat` and the glue's `FS.isDir`/`isFile` agree on. */
export const S_IFMT = 0o170000;
export const S_IFCHR = 0o020000;
export const S_IFDIR = 0o040000;
export const S_IFREG = 0o100000;
export const S_IFLNK = 0o120000;

/** `open(2)` flags, musl numbering — the values the compiled `fcntl`/`open` code compares against. */
export const ARTEMIS_O = {
  WRONLY: 0o1,
  RDWR: 0o2,
  CREAT: 0o100,
  EXCL: 0o200,
  NOCTTY: 0o400,
  TRUNC: 0o1000,
  APPEND: 0o2000,
  NONBLOCK: 0o4000,
  DSYNC: 0o10000,
  DIRECTORY: 0o200000,
  NOFOLLOW: 0o2000000,
  CLOEXEC: 0o2000000,
} as const;

let nextIno = 1;

/**
 * One entry in the host's mirror of the package filesystem.
 *
 * Read-only entries are *views*: `data` is whatever the adapter hands over, and nothing is copied.
 * A writable entry is a `MutableByteSource`, which stays a view of the source until its first
 * write — so a guest that writes to a real game file cannot corrupt the bytes the adapter serves
 * to anyone else.
 */
export class FsNode {
  readonly ino: number = nextIno++;
  name: string;
  path: string;
  parent: FsNode | null = null;
  /** Child map, present only for directories. Keyed by name, insertion-ordered. */
  children = new Map<string, FsNode>();
  /** Bytes behind a file node. `null` for directories and character devices. */
  data: SyncByteSource | null = null;
  mode = S_IFREG | 0o644;
  atime = 0;
  mtime = 0;
  ctime = 0;
  rdev = 0;

  constructor(name: string, path: string, kind: NodeKind) {
    this.name = name;
    this.path = path;
    if (kind === 'dir') {
      this.mode = S_IFDIR | 0o755;
      this.data = null;
    } else if (kind === 'chrdev') {
      this.mode = S_IFCHR | 0o666;
      this.data = new ChrDevice();
      this.rdev = 0;
    }
  }

  get isDir(): boolean {
    return (this.mode & S_IFMT) === S_IFDIR;
  }
  get isFile(): boolean {
    return (this.mode & S_IFMT) === S_IFREG;
  }
  get isLink(): boolean {
    return (this.mode & S_IFMT) === S_IFLNK;
  }
  get isChrdev(): boolean {
    return (this.mode & S_IFMT) === S_IFCHR;
  }
  get size(): number {
    return this.data ? this.data.size() : 0;
  }

  touch(): void {
    this.mtime = nowSeconds();
    this.ctime = this.mtime;
  }

  /** Remove from the parent. Caller must have already unlinked the children. */
  detach(): void {
    this.parent?.children.delete(this.name);
    this.parent = null;
  }

  /** Attach under `parent`, recomputing `path` from the new location. */
  reparentTo(parent: FsNode): void {
    this.parent = parent;
    this.path = parent.path ? `${parent.path}/${this.name}` : this.name;
  }

  /** Rename in place, keeping the parent link and the descendants' paths consistent. */
  renameSelf(name: string, parent: FsNode): void {
    this.name = name;
    this.reparentTo(parent);
  }
}

/**
 * A character device. The game probes the terminal through one of these; every operation on it
 * succeeds or reports ENOTTY exactly as a real tty would, but nothing is buffered.
 */
export class ChrDevice implements SyncByteSource {
  size(): number {
    return 0;
  }
  readRange(_offset: number, _length: number): Uint8Array {
    return new Uint8Array(0);
  }
  write(offset: number, data: Uint8Array): number {
    // A tty that is not a terminal in canonical mode discards input. Discarding is the honest
    // behaviour: reporting a short write would invite a retry loop the guest can never satisfy.
    void offset;
    void data;
    return data.length;
  }
  truncate(): void {
    /* a character device has no length to truncate */
  }
}

/**
 * Copy-on-write wrapper: reads fall through to `base` until the first write, then the whole file
 * is materialised into an editable copy.
 *
 * Full materialisation rather than a sparse overlay is a deliberate choice. A sparse overlay is
 * cleverer on paper, but it needs a boundary marker between "before the first write" and "after"
 * in every read path, and every one of those paths is a chance to hand the guest a short read. The
 * files a guest actually writes are save data and generated config — kilobytes. The 2.6 GB
 * `root.pfs` is exposed as many small entries and is never opened for writing; if it were, the copy
 * would be the thing that fails, loudly, rather than a silently wrong read.
 */
export class MutableByteSource implements SyncByteSource {
  private copy: Uint8Array | null = null;

  constructor(private readonly base: SyncByteSource) {}

  size(): number {
    return this.copy ? this.copy.length : this.base.size();
  }

  readRange(offset: number, length: number): Uint8Array {
    const total = this.size();
    const start = Math.max(0, Math.min(offset, total));
    const end = Math.max(start, Math.min(offset + length, total));
    const out = new Uint8Array(end - start);
    if (this.copy) {
      out.set(this.copy.subarray(start, end));
    } else if (out.length > 0) {
      const got = this.base.readRange(start, out.length);
      out.set(got.subarray(0, out.length));
    }
    return out;
  }

  /** Adopt the base bytes into an editable copy. Idempotent. */
  private materialise(): Uint8Array {
    if (!this.copy) {
      this.copy = this.base.readRange(0, this.base.size());
    }
    return this.copy;
  }

  write(offset: number, data: Uint8Array): number {
    if (offset > this.size()) {
      // POSIX leaves the gap zero-filled, but a game writing past EOF means it miscalculated;
      // a sparse hole here would look like corruption of the source file.
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EINVAL, `write at ${offset} past end of file`);
    }
    const bytes = this.materialise();
    const end = offset + data.length;
    if (bytes.length < end) {
      const grown = new Uint8Array(end);
      grown.set(bytes, 0);
      this.copy = grown;
    }
    this.copy!.set(data, offset);
    return data.length;
  }

  truncate(size: number): void {
    const bytes = this.materialise();
    if (size === bytes.length) {
      return;
    }
    const next = new Uint8Array(size);
    next.set(bytes.subarray(0, Math.min(size, bytes.length)));
    this.copy = next;
  }

  /** True once a write or truncate has made this file independent of its source. */
  get detached(): boolean {
    return this.copy !== null;
  }
}

/** An empty, growable in-memory file. */
export class MemoryByteSource implements SyncByteSource {
  private bytes: Uint8Array;

  constructor(initial?: Uint8Array) {
    this.bytes = initial ? new Uint8Array(initial) : new Uint8Array(0);
  }

  size(): number {
    return this.bytes.length;
  }

  readRange(offset: number, length: number): Uint8Array {
    const start = Math.max(0, Math.min(offset, this.bytes.length));
    const end = Math.max(start, Math.min(offset + length, this.bytes.length));
    return this.bytes.slice(start, end);
  }

  write(offset: number, data: Uint8Array): number {
    const end = offset + data.length;
    if (this.bytes.length < end) {
      const grown = new Uint8Array(end);
      grown.set(this.bytes, 0);
      this.bytes = grown;
    }
    this.bytes.set(data, offset);
    return data.length;
  }

  truncate(size: number): void {
    if (size === this.bytes.length) {
      return;
    }
    const next = new Uint8Array(size);
    next.set(this.bytes.subarray(0, Math.min(size, this.bytes.length)));
    this.bytes = next;
  }
}

/** Seconds since the epoch, as `time(2)` reports them. */
export function nowSeconds(): number {
  return Math.floor(Date.now() / 1000);
}

/** An in-memory `SyncByteSource`. Small enough to be obvious, used by tests and the IDB store. */
export function memoryByteSource(bytes?: Uint8Array): SyncByteSource {
  return new MemoryByteSource(bytes);
}

/**
 * Mount a tree of read-only entries as a directory.
 *
 * Takes the paths the adapter resolved and builds the intermediate directories, so a package that
 * lists `system/adv/adv.lua` does not need the adapter to have enumerated `system` first. The
 * `.`/`..` names the glue's `FS.readdir` includes are **not** nodes — they are synthesised in
 * `readdir`, because a name that can be deleted is a name that breaks path resolution.
 */
export function mountTree(
  root: FsNode,
  entries: Iterable<{ path: string; data: SyncByteSource; mode?: number }>,
): number {
  let count = 0;
  for (const entry of entries) {
    const segments = entry.path.split('/').filter(Boolean);
    if (segments.length === 0) {
      continue;
    }
    let dir = root;
    for (let i = 0; i < segments.length - 1; i += 1) {
      const segment = segments[i]!;
      let next = dir.children.get(segment);
      if (!next) {
        next = new FsNode(segment, dir === root ? segment : `${dir.path}/${segment}`, 'dir');
        next.parent = dir;
        dir.children.set(segment, next);
      }
      if (!next.isDir) {
        // A file already occupies this prefix. The package is malformed; refuse rather than
        // silently replacing a real file with a directory.
        throw new ArtemisHostError(`cannot mount ${entry.path}: ${dir.path} is not a directory`);
      }
      dir = next;
    }
    const leaf = segments[segments.length - 1]!;
    if (dir.children.has(leaf)) {
      continue;
    }
    const node = new FsNode(leaf, dir === root ? leaf : `${dir.path}/${leaf}`, 'file');
    node.data = entry.data;
    if (entry.mode !== undefined) {
      node.mode = entry.mode;
    }
    node.parent = dir;
    dir.children.set(leaf, node);
    count += 1;
  }
  return count;
}
