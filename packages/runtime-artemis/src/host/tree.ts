import { ARTEMIS_ERRNO, ArtemisErrnoError } from './errno.js';
import {
  ARTEMIS_O,
  ChrDevice,
  FsNode,
  MemoryByteSource,
  MutableByteSource,
  S_IFMT,
  S_IFREG,
  mountTree,
  nowSeconds,
  type SyncByteSource,
} from './fs.js';

/**
 * The host's filesystem: a directory tree of `FsNode`s plus the fd table the guest hands around.
 *
 * Built to the same contract as Emscripten's `MEMFS`, because that is what the compiled engine
 * expects underneath it. Three properties were read out of the official glue rather than assumed,
 * and each one is a place a plausible-looking host goes wrong:
 *
 * 1. **Paths are normalised before anything else happens** — `PATH.normalize` turns backslashes
 *    into slashes and folds `.` and `..`. Every entry point calls `ArtemisFs.normalise` first, so
 *    `system/../font/x.ttf` and `font/x.ttf` are the same file and no implementation thinks about
 *    it twice. The backslash case is not hypothetical here: the PFS stores its entry names with
 *    backslashes, so a caller can easily pass one through.
 * 2. **Implied directories are synthesised, not stored.** A package layer that lists only
 *    `system/adv/adv.lua` still resolves `stat("system")` as a directory, because path resolution
 *    creates intermediate components rather than demanding they were enumerated.
 * 3. **A mounted package is read-only until the host says otherwise.** MEMFS is writable
 *    everywhere, and a host that mounted the real 2.6 GB `root.pfs` as writable would let the
 *    guest corrupt the bytes every other reader of that package sees. `makeWritable` is the only
 *    door, and it promotes a file to a copy-on-write `MutableByteSource` so the adapter's bytes
 *    stay intact.
 */

export interface ResolvedPath {
  /** Final component's node, or `null` when the parent exists but the leaf does not. */
  node: FsNode | null;
  /** Parent directory of the final component. `null` for a bare `.` at the root. */
  parent: FsNode | null;
  /** The normalised path, for error messages and for naming a newly created node. */
  path: string;
}

/** `getdents64`'s `d_type` values. */
export const DT_UNKNOWN = 0;
export const DT_CHR = 2;
export const DT_DIR = 4;
export const DT_REG = 8;
export const DT_LNK = 10;

/**
 * The glue's inlined `SYSCALLS.DEFAULT_POLLMASK` — `POLLIN|POLLOUT`, the mask `select(2)` applies
 * to a stream with no `stream_ops.poll` of its own.
 */
const DEFAULT_POLLMASK = 5;

export interface DirentRecord {
  name: string;
  type: number;
  ino: number;
  /** `d_off`. The guest resumes from it, so it must be a stable cursor, not a re-derivable index. */
  off: number;
}

export interface StatFields {
  dev: number;
  ino: number;
  mode: number;
  nlink: number;
  uid: number;
  gid: number;
  rdev: number;
  size: number;
  blksize: number;
  blocks: number;
  atime: number;
  mtime: number;
  ctime: number;
}

export function dTypeOf(node: FsNode): number {
  if (node.isDir) return DT_DIR;
  if (node.isFile) return DT_REG;
  if (node.isLink) return DT_LNK;
  if (node.isChrdev) return DT_CHR;
  return DT_UNKNOWN;
}

/**
 * One open file: the node plus the per-stream position and mode.
 *
 * The position lives on the stream rather than the node because `dup2` aliases one stream across
 * several fds, and a guest reading through the pair expects a shared cursor.
 */
export class FsStream {
  position = 0;

  /**
   * The flags this stream was opened with, kept because `fcntl(F_GETFL)` has to report them and
   * `fcntl(F_SETFL)` has to be able to change the non-access bits. The access mode lives in
   * {@link write}, so this holds the rest and is combined back on read.
   */
  flags: number;

  /**
   * The two access bits exactly as the guest passed them.
   *
   * `write` cannot stand in for this: `read`/`write` only need to know whether the stream is
   * writeable, so `O_RDWR` and `O_WRONLY` both become `write === true`. But `fcntl(F_GETFL)` hands
   * the bits back verbatim, and a guest that opened `O_RDWR|O_APPEND` and reads the flags back to
   * decide how to use the fd would see `O_WRONLY` — a stream it may read but is told it may not.
   */
  readonly access: number;

  constructor(
    readonly node: FsNode,
    readonly write: boolean,
    readonly append: boolean,
    flags = 0,
  ) {
    this.flags = flags;
    this.access = flags & (ARTEMIS_O.WRONLY | ARTEMIS_O.RDWR);
  }
}

export class ArtemisFs {
  readonly root: FsNode;
  private readonly streams = new Map<number, FsStream>();
  private readonly mounts = new Map<FsNode, Set<string>>();
  /** Nodes the host has declared writable. A write to anything else fails EROFS. */
  private readonly writeScope = new Set<FsNode>();
  private cwd: FsNode;
  /** The glue's own default (`umask: 511`); musl callers normally lower it to 0o022. */
  private umask = 0o777;
  private nextFd = 3;

  constructor(root?: FsNode) {
    this.root = root ?? new FsNode('', '', 'dir');
    this.cwd = this.root;
  }

  get cwdPath(): string {
    return this.cwd === this.root ? '/' : `/${this.cwd.path}`;
  }

  /** `umask(2)`. The glue's own default is 511 (0o777); musl callers usually set 0o022. */
  setUmask(mask: number): void {
    this.umask = mask;
  }

  // ---------------------------------------------------------------- mounting

  /**
   * Mount a package layer read-only at `mountPoint`. Returns the number of leaf files mounted.
   *
   * A second call at the same point **merges** rather than replaces. Layered packages are the
   * normal case for this game — `root.pfs` plus `chs_patch_official.pfs` — and a replace
   * semantics would make the second layer silently delete the first.
   */
  /**
   * Mount a layer at `mountPoint`, creating the point itself when it is missing.
   *
   * Mounting is host-side setup, not a guest syscall: the guest never mounts anything, so it will
   * never have `mkdir`-ed `/game` first. A mount that demanded a pre-existing directory would push
   * a pointless ordering constraint onto every caller. Intermediate points are created read-write,
   * because a directory the host invents has no backing layer to protect.
   */
  mount(
    mountPoint: string,
    entries: Iterable<{ path: string; data: SyncByteSource; mode?: number }>,
  ): number {
    const target = this.mkdirp(mountPoint);
    if (!target.isDir) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOTDIR, `mount point ${mountPoint}`);
    }
    const scratch = new FsNode('', '', 'dir');
    const added = mountTree(scratch, entries);
    for (const child of scratch.children.values()) {
      this.adopt(child, target);
    }
    const seen = this.mounts.get(target) ?? new Set<string>();
    for (const path of scratch.children.keys()) {
      seen.add(path);
    }
    this.mounts.set(target, seen);
    return added;
  }

  private adopt(node: FsNode, parent: FsNode): void {
    node.reparentTo(parent);
    for (const child of node.children.values()) {
      this.adopt(child, node);
    }
    parent.children.set(node.name, node);
  }

  /**
   * Open a mounted node for writing. Files are promoted to copy-on-write, so the adapter's bytes
   * are never modified. A directory grants the whole subtree.
   */
  makeWritable(path: string): FsNode {
    const node = this.resolveExisting(path);
    this.allowRecursive(node);
    return node;
  }

  private allowRecursive(node: FsNode): void {
    this.writeScope.add(node);
    if (node.isDir) {
      for (const child of node.children.values()) {
        this.allowRecursive(child);
      }
      return;
    }
    if (node.data && !(node.data instanceof MutableByteSource)) {
      node.data = new MutableByteSource(node.data);
    }
  }

  /** True when `node` may be written through any of its fds. */
  isWritable(node: FsNode): boolean {
    return this.writeScope.has(node);
  }

  // ------------------------------------------------------------- resolution

  /**
   * Normalise a path the way the glue's `PATH.normalize` does.
   *
   * `..` at the root is absorbed rather than escaping: the guest has no parent above the mount to
   * escape into, and letting the first `..` delete a segment would let `..` from `/` reach into a
   * layer the host never mounted.
   */
  static normalise(path: string): string {
    const out: string[] = [];
    for (const segment of path.replace(/\\/g, '/').split('/')) {
      if (segment === '' || segment === '.') continue;
      if (segment === '..') {
        out.pop();
        continue;
      }
      out.push(segment);
    }
    return out.join('/');
  }

  /** The last component of a path, as a name. */
  static baseName(path: string): string {
    return ArtemisFs.normalise(path).split('/').filter(Boolean).pop() ?? '';
  }

  /**
   * Resolve a path to its final node, or `null` if the leaf is missing.
   *
   * `createParents` synthesises missing intermediate directories, which is what makes a package
   * layer of bare leaf files work without the adapter having enumerated the directories the
   * archive never stored.
   */
  private resolve(path: string, createParents = false): ResolvedPath {
    const normalised = ArtemisFs.normalise(path);
    const segments = normalised.split('/').filter(Boolean);
    if (segments.length === 0) {
      return { node: this.root, parent: null, path: normalised };
    }
    let dir = this.root;
    for (let i = 0; i < segments.length - 1; i += 1) {
      const segment = segments[i]!;
      let next = dir.children.get(segment);
      if (!next) {
        if (!createParents || !dir.isDir) {
          return { node: null, parent: null, path: normalised };
        }
        next = new FsNode(segment, dir === this.root ? segment : `${dir.path}/${segment}`, 'dir');
        next.parent = dir;
        dir.children.set(segment, next);
        this.writeScope.add(dir);
      }
      if (!next.isDir) {
        return { node: null, parent: null, path: normalised };
      }
      dir = next;
    }
    const leaf = segments[segments.length - 1]!;
    return { node: dir.children.get(leaf) ?? null, parent: dir, path: normalised };
  }

  lookup(path: string): FsNode | null {
    return this.resolve(path).node;
  }

  resolveExisting(path: string): FsNode {
    const node = this.lookup(path);
    if (!node) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOENT, path);
    }
    return node;
  }

  // ----------------------------------------------------------------- nodes

  /** `creat(2)`-shaped: create a writable file, failing with EEXIST if it is already there. */
  createFile(path: string, data?: Uint8Array): FsNode {
    const resolved = this.resolve(path, true);
    if (resolved.node) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EEXIST, path);
    }
    if (!resolved.parent) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOENT, path);
    }
    const node = new FsNode(ArtemisFs.baseName(path), resolved.path, 'file');
    node.data = new MemoryByteSource(data);
    node.mtime = nowSeconds();
    node.parent = resolved.parent;
    resolved.parent.children.set(node.name, node);
    this.writeScope.add(resolved.parent);
    this.writeScope.add(node);
    return node;
  }

  /** `mkdir(2)`. Fails EEXIST if present, ENOTDIR if the parent is not a directory. */
  mkdir(path: string, mode?: number): FsNode {
    const resolved = this.resolve(path);
    if (resolved.node) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EEXIST, path);
    }
    const parent = resolved.parent;
    if (!parent) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOENT, path);
    }
    if (!parent.isDir) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOTDIR, parent.path);
    }
    const node = new FsNode(ArtemisFs.baseName(path), resolved.path, 'dir');
    if (mode !== undefined) {
      node.mode = (node.mode & S_IFMT) | ((mode & ~this.umask) & 0o777);
    }
    node.mtime = nowSeconds();
    node.parent = parent;
    parent.children.set(node.name, node);
    this.writeScope.add(parent);
    return node;
  }

  /** `mkdir -p`. Returns the deepest directory created, or the existing one. */
  mkdirp(path: string, mode?: number): FsNode {
    const segments = ArtemisFs.normalise(path).split('/').filter(Boolean);
    let dir = this.root;
    let walked = '';
    for (const segment of segments) {
      walked = walked ? `${walked}/${segment}` : segment;
      const next = dir.children.get(segment);
      if (!next) {
        dir = this.mkdir(walked, mode);
        continue;
      }
      if (!next.isDir) {
        throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOTDIR, walked);
      }
      dir = next;
    }
    return dir;
  }

  /**
   * Register a character device — the game's tty probe target.
   *
   * A character device is always writable, and that is not a simplification. In the glue a chrdev's
   * write op is a plain function with no read-only state anywhere: `FS.open("/dev/tty", 1)` cannot
   * fail, and neither can `open("/dev/null", 1)`. This host otherwise denies a write to anything
   * outside `writeScope` with EROFS, which is how a mounted package layer stays read-only — but
   * applying that to a chrdev would make the standard streams unopenable for writing, and the guest
   * would lose stdout at `FS.init` rather than at the first `printf`. The tty is a sink, not a
   * mount, so it joins the write scope here.
   */
  addChrdev(path: string, device?: ChrDevice): FsNode {
    const resolved = this.resolve(path, true);
    if (resolved.node) {
      this.writeScope.add(resolved.node);
      return resolved.node;
    }
    if (!resolved.parent) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOENT, path);
    }
    const node = new FsNode(ArtemisFs.baseName(path), resolved.path, 'chrdev');
    if (device) {
      node.data = device;
    }
    node.parent = resolved.parent;
    resolved.parent.children.set(node.name, node);
    this.writeScope.add(node);
    return node;
  }

  // ------------------------------------------------------------------- fds

  /**
   * `open(2)`. Returns the new fd.
   *
   * Flag handling copies the glue's `FS.open`, including the parts that are easy to miss: a newly
   * created node has `S_IFREG` forced into its mode regardless of the mode the caller passed, and
   * `O_TRUNC` on a directory or device is silently accepted rather than refused.
   */
  open(path: string, flags: number, mode = 0o666): number {
    const write = (flags & 0o3) !== 0;
    const create = (flags & ARTEMIS_O.CREAT) !== 0;
    const exclusive = (flags & ARTEMIS_O.EXCL) !== 0;
    const wantDir = (flags & ARTEMIS_O.DIRECTORY) !== 0;

    let node = this.lookup(path);
    let justCreated = false;
    if (!node && create) {
      const resolved = this.resolve(path, true);
      if (resolved.node) {
        node = resolved.node;
      } else if (resolved.parent) {
        const made = new FsNode(ArtemisFs.baseName(path), resolved.path, 'file');
        made.data = new MemoryByteSource();
        made.mode = S_IFREG | ((mode & ~this.umask) & 0o777);
        made.mtime = nowSeconds();
        made.parent = resolved.parent;
        resolved.parent.children.set(made.name, made);
        this.writeScope.add(resolved.parent);
        // The parent is in scope so the creation was legal, but the WRITE check below asks about
        // `node` — the file just made. Without this, `open(path, O_CREAT|O_WRONLY)` created a file
        // and then refused it with EROFS, which no read-only mount can explain.
        this.writeScope.add(made);
        node = made;
        justCreated = true;
      }
    }
    if (!node) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOENT, path);
    }
    if (exclusive && !justCreated && (flags & ARTEMIS_O.CREAT) !== 0) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EEXIST, path);
    }
    if (wantDir && !node.isDir) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOTDIR, path);
    }
    if (node.isDir && write) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EISDIR, path);
    }
    if (write && !this.writeScope.has(node)) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EROFS, path);
    }
    if ((flags & ARTEMIS_O.TRUNC) !== 0 && !node.isDir && !node.isChrdev && !justCreated) {
      this.truncateNode(node, 0);
    }

    const fd = this.allocateFd();
    this.streams.set(fd, new FsStream(node, write, (flags & ARTEMIS_O.APPEND) !== 0, flags));
    return fd;
  }

  private allocateFd(): number {
    while (this.streams.has(this.nextFd)) {
      this.nextFd += 1;
    }
    const fd = this.nextFd;
    this.nextFd += 1;
    return fd;
  }

  requireStream(fd: number): FsStream {
    const stream = this.streams.get(fd);
    if (!stream) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EBADF, `fd ${fd}`);
    }
    return stream;
  }

  hasStream(fd: number): boolean {
    return this.streams.has(fd);
  }

  /**
   * The flags `fcntl(F_GETFL)` reports for an open stream.
   *
   * The access bits come from {@link FsStream.access} rather than being re-derived from `write`,
   * because `O_RDWR` and `O_WRONLY` are different access modes that collapse to the same boolean.
   */
  streamFlags(stream: FsStream): number {
    return stream.access | (stream.flags & ~(ARTEMIS_O.WRONLY | ARTEMIS_O.RDWR));
  }

  /**
   * The poll mask `select(2)` sees for an fd, which is `POLLIN|POLLOUT` (5) for every stream here.
   *
   * The glue inlines `SYSCALLS.DEFAULT_POLLMASK = 5` as the default and only narrows it when a stream
   * has a `stream_ops.poll` method — a TTY or a socket. This host has neither, so every open fd is
   * always ready in both directions. That is the same answer the glue gives, and it is why
   * `___sys__newselect` can never block here.
   */
  streamIsReady(fd: number): number {
    this.requireStream(fd);
    return DEFAULT_POLLMASK;
  }

  /** `read(2)`. Copies into `buffer` at `position` and advances. */
  read(fd: number, buffer: Uint8Array, position: number, length: number): number {
    const stream = this.requireStream(fd);
    if (stream.node.isDir) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EISDIR, stream.node.path);
    }
    const source = stream.node.data;
    if (!source) {
      return 0;
    }
    const bytes = source.readRange(position, length);
    const count = Math.min(bytes.length, length, buffer.length);
    if (count > 0) {
      buffer.set(bytes.subarray(0, count), 0);
    }
    return count;
  }

  /** `write(2)`. Returns the byte count transferred. */
  write(fd: number, data: Uint8Array, position: number): number {
    const stream = this.requireStream(fd);
    if (!stream.write) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EBADF, `fd ${fd} is not writable`);
    }
    if (stream.node.isDir) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EISDIR, stream.node.path);
    }
    if (!this.writeScope.has(stream.node)) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EROFS, stream.node.path);
    }
    if (!stream.node.data) {
      return 0;
    }
    // The host widened the scope but the node may still hold a read-only view. Copy-on-write from
    // here, so the adapter's bytes stay intact and the guest's first write is not a failure.
    const current = stream.node.data;
    const sink: MutableByteSource =
      current instanceof MutableByteSource ? current : new MutableByteSource(current);
    stream.node.data = sink;
    // `sink` is bound to the concrete class on purpose: `SyncByteSource.write` is optional, so
    // going through the interface type would make every call "possibly undefined".
    const target = stream.append ? sink.size() : position;
    const count = sink.write(target, data);
    stream.node.touch();
    return Math.min(count, data.length);
  }

  /** `lseek(2)`. Returns the new absolute position. */
  seek(fd: number, offset: number, whence: number): number {
    const stream = this.requireStream(fd);
    let next: number;
    switch (whence) {
      case 0:
        next = offset;
        break;
      case 1:
        next = stream.position + offset;
        break;
      case 2:
        next = stream.node.size + offset;
        break;
      default:
        throw new ArtemisErrnoError(ARTEMIS_ERRNO.EINVAL, `whence ${whence}`);
    }
    if (next < 0) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EINVAL, `seek to ${next}`);
    }
    stream.position = next;
    return next;
  }

  /** `close(2)`. EBADF if the fd was never open, as the kernel reports. */
  close(fd: number): void {
    if (!this.streams.delete(fd)) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EBADF, `close fd ${fd}`);
    }
  }

  /** `dup2(2)`. */
  dup2(source: number, target: number): number {
    const stream = this.streams.get(source);
    if (!stream) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EBADF, `dup2 from fd ${source}`);
    }
    this.streams.set(target, stream);
    return target;
  }

  /**
   * `dup3(2)`. Identical to `dup2` except that duplicating a fd onto itself is EINVAL — the single
   * behavioural difference between the two calls, and the one the guest branches on.
   */
  dup3(source: number, target: number): number {
    if (source === target) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EINVAL, `dup3 fd ${source} onto itself`);
    }
    return this.dup2(source, target);
  }

  /** `ftruncate(2)`. */
  ftruncate(fd: number, size: number): void {
    const stream = this.requireStream(fd);
    if (!stream.write) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EBADF, `fd ${fd} is not writable`);
    }
    if (stream.node.isDir) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EISDIR, stream.node.path);
    }
    this.truncateNode(stream.node, size);
  }

  private truncateNode(node: FsNode, size: number): void {
    if (!node.data) return;
    if (node.data instanceof MutableByteSource) {
      node.data.truncate(size);
    } else if (size === 0) {
      node.data = new MemoryByteSource();
    } else {
      // Truncating a read-only mount to a non-zero size is not something a host can honestly do.
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EROFS, node.path);
    }
    node.touch();
  }

  // ------------------------------------------------------------ directories

  chdir(path: string): void {
    const node = this.resolveExisting(path);
    if (!node.isDir) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOTDIR, path);
    }
    this.cwd = node;
  }

  /** `unlink(2)`. A directory is EISDIR here, matching the kernel rather than MEMFS. */
  unlink(path: string): void {
    const node = this.resolveExisting(path);
    if (node === this.root) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EBUSY, '/');
    }
    if (node.isDir) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EISDIR, path);
    }
    node.detach();
  }

  /** `rmdir(2)`. */
  rmdir(path: string): void {
    const node = this.resolveExisting(path);
    if (!node.isDir) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOTDIR, path);
    }
    if (node === this.root) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EBUSY, '/');
    }
    if (node.children.size > 0) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOTEMPTY, path);
    }
    node.detach();
  }

  /** `rename(2)`, including moving a directory's whole subtree. */
  rename(from: string, to: string): void {
    const source = this.resolveExisting(from);
    if (source === this.root) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.EBUSY, '/');
    }
    const target = this.resolve(to, true);
    const parent = target.parent;
    if (!parent) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOENT, to);
    }
    if (!parent.isDir) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOTDIR, to);
    }
    if (target.node) {
      if (target.node.isDir && !source.isDir) {
        throw new ArtemisErrnoError(ARTEMIS_ERRNO.EISDIR, to);
      }
      if (!target.node.isDir && source.isDir) {
        throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOTDIR, to);
      }
      if (target.node !== source && target.node.isDir && target.node.children.size > 0) {
        throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOTEMPTY, to);
      }
      if (target.node !== source) {
        target.node.detach();
      }
    }
    const name = ArtemisFs.baseName(to);
    if (name !== source.name) {
      source.parent?.children.delete(source.name);
      source.renameSelf(name, parent);
    } else {
      source.reparentTo(parent);
    }
    parent.children.set(name, source);
    this.rewritePaths(source);
  }

  private rewritePaths(node: FsNode): void {
    for (const child of node.children.values()) {
      child.reparentTo(node);
      this.rewritePaths(child);
    }
  }

  /**
   * `getdents64`'s view of a directory: `.`, `..`, then each child in insertion order.
   *
   * The two dot entries are synthesised, exactly as `FS.readdir` does — a stored `..` would be a
   * name a caller could `unlink`, after which the next `stat` would resolve upward into nothing.
   * The list must also stay stable for the duration of a scan: the official
   * `___sys_getdents64` derives its resume index as `Math.floor(llseek(stream, 0, 1) / 280)`, so a
   * list that changed under a resumed scan would skip or repeat entries.
   */
  readdir(path: string): DirentRecord[] {
    const node = this.resolveExisting(path);
    if (!node.isDir) {
      throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOTDIR, path);
    }
    const records: DirentRecord[] = [
      { name: '.', type: DT_DIR, ino: node.ino, off: 0 },
      { name: '..', type: DT_DIR, ino: node.parent ? node.parent.ino : node.ino, off: 0 },
    ];
    for (const child of node.children.values()) {
      records.push({ name: child.name, type: dTypeOf(child), ino: child.ino, off: records.length });
    }
    return records;
  }

  // ------------------------------------------------------------------ stat

  /** `stat`'s field set. `fd.ts` serialises this into the guest's 64-bit `struct stat`. */
  statOf(node: FsNode): StatFields {
    const size = node.size;
    return {
      dev: 0,
      ino: node.ino,
      mode: node.mode,
      nlink: 1,
      uid: 0,
      gid: 0,
      rdev: node.rdev,
      size,
      blksize: 4096,
      blocks: Math.ceil(size / 512),
      atime: node.atime,
      mtime: node.mtime,
      ctime: node.ctime,
    };
  }
}
