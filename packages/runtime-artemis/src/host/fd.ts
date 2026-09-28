import { ARTEMIS_ERRNO, ArtemisErrnoError, ArtemisExit, ArtemisHostError } from './errno.js';
import { DIRENT64_NAME_MAX, DIRENT64_STRIDE, doPreadv, doPwritev, writeStat64 } from './layout.js';
import { ArtemisMemory } from './memory.js';
import { ArtemisFs, type DirentRecord, DT_DIR } from './tree.js';
import { clockGettime, gmtimeR, localtimeR, mktime, strftime, TimeZoneNames } from './timefmt.js';

/**
 * The `fd` bucket of `ports/artemis/harness/glue/imports-map.json` — 48 rows, 41 distinct short
 * names — implemented without the official glue.
 *
 * Three rules govern every body in this file, and all three come from the glue rather than from taste:
 *
 * 1. **A syscall body returns an errno; it never throws.** The official bodies are all
 *    `try{…}catch(e){if(!(e instanceof FS.ErrnoError))abort(e);return -e.errno}`. Six of them —
 *    `_fd_write`, `_fd_read`, `_fd_close`, `_fd_seek`, `_environ_get`, `_environ_sizes_get` —
 *    return `e.errno` **positive** instead. {@link POSIX_ERRNO} is where that table lives.
 * 2. **The 7 doubled names get one implementation each.** `wasm-satisfy-imports.py` renamed two pairs
 *    of libc functions onto the same short name, so both import entries point at the same behaviour
 *    and wasm links by module + name. One export satisfies both rows of a pair.
 * 3. **Where the glue is wrong, the host is wrong in the same way.** `___sys_ioctl`'s default arm
 *    calls `abort("bad ioctl syscall " + op)`; `___sys__newselect` reads `timeout` and never sleeps;
 *    `_mktime` passes a 1-based `tm_mday` to `new Date(...)` unshifted. Copying those is the
 *    contract — the guest was compiled and calibrated against them. Each one is marked where it
 *    happens, and the one place the host deliberately does *not* copy the glue is `___sys_ioctl`.
 */

/**
 * The six imports whose official body returns `e.errno` positive rather than negated. Getting this
 * backwards turns every EAGAIN from `read` into a huge bogus offset, so it is worth its own table.
 */
const POSITIVE_ERRNO = new Set(['Ea', 'Ub', 'Q', 'xb', 'Eb', 'Fb']);

/** musl `SEEK_*`. The glue hard-codes these numbers in `FS.llseek`. */
const SEEK_SET = 0;
const SEEK_CUR = 1;
const SEEK_END = 2;

/** `2^32`, the glue's `HIGH_OFFSET` for reconstructing an i64 from two i32 words. */
const HIGH_OFFSET = 4294967296;

/** The glue's `DOUBLE_LIMIT`: the largest offset an i64 path will accept before it gives up. */
const DOUBLE_LIMIT = 9007199254740992;

/** A guest-visible environment block, in the shape `getEnvStrings()` produces. */
export interface ArtemisEnvOptions {
  /** Merged over the glue's defaults, exactly as the glue merges the `ENV` global. */
  readonly env?: Readonly<Record<string, string>>;
  /** Stands in for `navigator.languages[0]`; the glue reads it before appending `.UTF-8`. */
  readonly language?: string;
  /** Stands in for `getExecutableName()`, the value of the `_` variable. */
  readonly executableName?: string;
}

/** A resolved endpoint, kept off the heap so `getaddrinfo` can be exercised without wasm. */
export interface ResolvedAddrInfo {
  family: number;
  socktype: number;
  protocol: number;
  /** The dotted-quad or colon form the guest would `connect` to. */
  address: string;
  port: number;
}

/**
 * Everything the `fd` bodies need that is not the linear memory.
 *
 * Memory is **injected**, never eagerly allocated: a test must be able to hand in a 2-page memory
 * and still drive all 48 rows, while a real session allocates the module's own 3072 pages. The same
 * goes for `malloc`/`free` — the guest owns its heap, and only an instantiated session can lend it.
 */
export interface ArtemisFdHost {
  readonly memory: ArtemisMemory;
  readonly fs: ArtemisFs;
  /** Wires the guest's errno cell once the instance exists. Without it the value is only returned. */
  readonly errnoLocation?: () => number;
  /** The guest's `malloc`, needed by the two `_emscripten_idb_load` shapes. */
  readonly malloc?: (bytes: number) => number;
  /** The guest's `free`. */
  readonly free?: (pointer: number) => void;
  /** Milliseconds, monotonic where the platform offers it — the glue's `_emscripten_get_now`. */
  readonly now?: () => number;
  readonly env?: ArtemisEnvOptions;
  readonly idb?: ArtemisSyncIdb;
  /** Replaces the glue's DNS. Absent means literal addresses only, which is what a game can use. */
  readonly resolveHost?: (host: string) => readonly ResolvedAddrInfo[];
  /**
   * Invokes one of the guest's function-table entries — the host's stand-in for the glue's
   * `getDynCaller`. The three async `_emscripten_idb_*` rows need it, and a session that never
   * supplies one still gets correct `perror` reports from those rows.
   */
  readonly callback?: (entry: number, arg: number, extra?: number, count?: number) => void;
  /** Reports a call this host cannot serve, so a session can log it instead of swallowing it. */
  readonly onBlocked?: (what: string, detail: string) => void;
}

/** The import table, keyed by the wasm short name. */
export type ArtemisFdImports = Record<string, (...args: number[]) => number | void>;

/**
 * A synchronous key/value store standing in for the glue's `IDBStore`.
 *
 * The guest reaches IndexedDB through eight `_emscripten_idb_*` imports. In a browser the glue
 * serves them with `Asyncify.handleSleep` over a real IndexedDB. Neither half exists here: this host
 * has no async store to block on and no Asyncify unwinder to resume. So the eight rows get an
 * in-memory table with the same observable contract — `load` hands back a pointer the guest owns and
 * frees, and every call reports failure through `perror` rather than through an exception. A game's
 * save data survives a session here, not a reload; that is the honest limit, and it is a bigger
 * limit than the browser's, not a smaller one.
 */
export class ArtemisSyncIdb {
  private readonly files = new Map<string, Uint8Array>();

  private key(db: string, id: string): string {
    // A NUL cannot appear in a string decoded from the guest heap, so it is a separator that
    // nothing a caller can pass will collide with. Written as an escape so this source file stays
    // free of raw control bytes — a literal one makes every tool treat the file as binary.
    return `${db}\u0000${id}`;
  }

  exists(db: string, id: string): boolean {
    return this.files.has(this.key(db, id));
  }

  load(db: string, id: string): Uint8Array | null {
    return this.files.get(this.key(db, id)) ?? null;
  }

  store(db: string, id: string, data: Uint8Array): void {
    this.files.set(this.key(db, id), data.slice());
  }

  delete(db: string, id: string): void {
    this.files.delete(this.key(db, id));
  }

  /** Empties the store. Tests use it; the guest has no path that reaches it. */
  clear(): void {
    this.files.clear();
  }
}

/** `getaddrinfo` error codes. These are **not** errno — the glue returns them negative all the same. */
export const EAI = {
  BADFLAGS: -1,
  NONAME: -2,
  AGAIN: -3,
  FAMILY: -6,
  SOCKTYPE: -7,
  SERVICE: -8,
} as const;

/** The `AI_*` bits `_getaddrinfo` accepts; anything outside this set is `EAI_BADFLAGS`. */
const AI_PASSIVE = 1;
const AI_CANONNAME = 2;
const AI_NUMERICHOST = 4;
const AI_V4MAPPED = 8;
const AI_ALL = 16;
const AI_ADDRCONFIG = 32;
const AI_NUMERICSERV = 1024;
const AI_KNOWN =
  AI_PASSIVE | AI_CANONNAME | AI_NUMERICHOST | AI_V4MAPPED | AI_ALL | AI_ADDRCONFIG | AI_NUMERICSERV;

/** `sizeof(struct addrinfo)` as the glue lays it out: 32 bytes, no padding. */
const ADDRINFO_SIZE = 32;

/** `sockaddr_in` is 16 bytes, `sockaddr_in6` is 28. */
const SOCKADDR_SIZE: Record<number, number> = { 2: 16, 10: 28 };

/** The `ioctl` request codes the glue's table mentions. All of them want a tty this host lacks. */
const IOCTL_TIOCGWINSZ = 21519;
const IOCTL_TIOCSWINSZ = 21520;
const IOCTL_TIOCGPTN = 21531;

/** Builds the `fd` import table for one instance. */
export function createArtemisFdImports(host: ArtemisFdHost): ArtemisFdImports {
  const { memory, fs } = host;
  const zones = new TimeZoneNames(host.malloc);
  // The zone names are written into the GUEST's heap, so the class needs its live views, not a copy.
  zones.attach(memory);
  const idb = host.idb ?? new ArtemisSyncIdb();
  const env = envStrings(host.env);
  const now = host.now ?? (() => Date.now());
  const report = (what: string, detail: string): void => host.onBlocked?.(what, detail);
  const encoder = new TextEncoder();
  /** `_clock` caches its start time on the function object, as the glue does. */
  const clockStart = { value: 0, started: false };
  /** A `getdents` listing is only valid until the stream is rewound. */
  const directoryCache = new Map<number, DirentRecord[]>();

  /**
   * Sets the guest's errno the way the glue's `setErrNo` does.
   *
   * Only one import needs this: `_clock_gettime` answers an unsupported clock with `-1` and an
   * errno set, rather than with a negated errno like every other body.
   */
  const setErrNo = (value: number): number => {
    const locate = host.errnoLocation;
    if (locate) {
      try {
        memory.i32[(locate() | 0) >>> 2] = value;
      } catch {
        // No writable errno cell. The return value still says the call failed, which is the part
        // the guest checks, so this degrades rather than breaks.
      }
    }
    return value;
  };

  /**
   * The boundary every syscall body runs inside.
   *
   * An `ArtemisErrnoError` becomes whatever the official body would have returned — negative,
   * except for the six positive-errno imports. An `ArtemisExit` is control flow and passes through.
   * Anything else is a host bug: the glue would have called `abort`, and a silently wrong answer is
   * worse than a loud one, so it is re-raised as an `ArtemisHostError` that nothing can mistake for
   * a guest-visible errno.
   */
  const boundary = (name: string, body: () => number): number => {
    try {
      return body();
    } catch (error) {
      if (error instanceof ArtemisExit || error instanceof ArtemisHostError) throw error;
      if (error instanceof ArtemisErrnoError) {
        return POSITIVE_ERRNO.has(name) ? error.errno : -error.errno;
      }
      throw new ArtemisHostError(`artemis fd import ${name} failed: ${String(error)}`);
    }
  };

  /** `void`-returning bodies still need the boundary, so they get their own wrapper. */
  const boundaryVoid = (name: string, body: () => void): void => {
    try {
      body();
    } catch (error) {
      if (error instanceof ArtemisExit || error instanceof ArtemisHostError) throw error;
      if (error instanceof ArtemisErrnoError) {
        if (!POSITIVE_ERRNO.has(name)) {
          report('errno', `${name} failed with ${error.errno}, which has nowhere to go`);
        }
        return;
      }
      throw new ArtemisHostError(`artemis fd import ${name} failed: ${String(error)}`);
    }
  };

  /**
   * Reads a NUL-terminated string out of the guest heap. The `| 0` is not decoration: wasm i32
   * params arrive in JS as signed integers, so a heap address above 2^31 comes through negative and
   * has to be squeezed back to the unsigned value the memory accessor expects.
   */
  const pointer = (address: number): string => memory.utf8(address | 0);

  /** `___sys_stat64` / `___sys_lstat64`. No symlinks exist here, so the two agree. */
  const statPath = (name: 'Jb' | 'Nb', path: number, buf: number): number =>
    boundary(name, () => {
      const text = pointer(path | 0);
      const node = fs.lookup(text);
      if (!node) {
        throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOENT, `stat ${text}`);
      }
      writeStat64(memory, buf, fs.statOf(node));
      return 0;
    });

  /**
   * `___sys_getdents64` — a dirent64 walk resumed by seeking.
   *
   * The stride is 280 and the cursor lives in the stream position, divided by that stride, which is
   * why the glue finishes by seeking back to `idx * 280`. The listing itself is cached per fd
   * because a walk revisits this many times per directory and `readdir` is not cheap.
   */
  const getdents = (fd: number, dirp: number, count: number): number =>
    boundary('Vb', () => {
      const stream = fs.requireStream(fd);
      if (!stream.node.isDir) {
        throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOTDIR, `getdents ${stream.node.path}`);
      }
      let records = directoryCache.get(fd);
      if (!records) {
        records = fs.readdir(stream.node.path);
        directoryCache.set(fd, records);
      }
      const startIndex = Math.floor(stream.position / DIRENT64_STRIDE);
      let written = 0;
      let index = startIndex;
      while (index < records.length && written + DIRENT64_STRIDE <= count) {
        const record = records[index];
        if (!record) break;
        const at = dirp + written;
        // The glue pins BOTH `.` and `..` to ino 1 and type 4. Copying that is what lets the
        // guest's directory walk work against a tree that never had those inode numbers.
        const synthetic = record.name.startsWith('.');
        const ino = synthetic ? 1 : record.ino;
        const type = synthetic ? DT_DIR : record.type;
        memory.u32[(at + 0) >>> 2] = ino >>> 0;
        memory.u32[(at + 4) >>> 2] = Math.floor(ino / HIGH_OFFSET) >>> 0;
        memory.u32[(at + 8) >>> 2] = ((index + 1) * DIRENT64_STRIDE) >>> 0;
        memory.u32[(at + 12) >>> 2] = Math.floor(((index + 1) * DIRENT64_STRIDE) / HIGH_OFFSET) >>> 0;
        memory.u16[(at + 16) >>> 1] = DIRENT64_STRIDE;
        memory.u8[at + 18] = type;
        const name = encoder.encode(record.name).subarray(0, DIRENT64_NAME_MAX - 1);
        memory.u8.set(name, at + 19);
        memory.u8[at + 19 + name.length] = 0;
        written += DIRENT64_STRIDE;
        index += 1;
      }
      fs.seek(fd, index * DIRENT64_STRIDE, SEEK_SET);
      return written;
    });

  /** `doReadv`/`doWritev` behind `_fd_read`/`_fd_write`, which report through `pnum`. */
  const scatterGather = (name: 'Ea' | 'Ub', fd: number, iov: number, iovcnt: number, pnum: number): number =>
    boundary(name, () => {
      const position = fs.requireStream(fd).position;
      const total =
        name === 'Ea'
          ? doPwritev(memory, fs, fd, iov, iovcnt, position)
          : doPreadv(memory, fs, fd, iov, iovcnt, position);
      memory.i32[(pnum | 0) >>> 2] = total;
      return 0;
    });

  /** The one `load` shape, shared by the sync and async rows, since they agree byte for byte. */
  const idbLoad = (db: number, id: number, onOk: (pointer: number, length: number) => void, onFail: () => void): void => {
    const data = idb.load(pointer(db), pointer(id));
    const allocate = host.malloc;
    if (!data || !allocate) {
      onFail();
      return;
    }
    const target = allocate(data.length);
    if (target === 0) {
      onFail();
      return;
    }
    memory.u8.set(data, target);
    onOk(target, data.length);
  };

  /** `getaddrinfo`'s address list, or `null` when the name needs a resolver we do not have. */
  const resolveAddresses = (
    nodeText: string,
    flags: number,
    family: number,
  ): readonly ResolvedAddrInfo[] | null => {
    const numericOnly = (flags & AI_NUMERICHOST) !== 0;
    if (nodeText === '') {
      // The glue synthesises loopback for a passive lookup, and so does this.
      if (numericOnly) return null;
      const loopback = family === 10 ? [{ family: 10, socktype: 0, protocol: 0, address: '::1', port: 0 }] : [{ family: 2, socktype: 0, protocol: 0, address: '127.0.0.1', port: 0 }];
      return family === 0 ? [...loopback, { family: 2, socktype: 0, protocol: 0, address: '127.0.0.1', port: 0 }] : loopback;
    }
    const literal = family === 10 ? pton6(nodeText) !== null : pton4(nodeText) !== null;
    if (literal) {
      return [{ family: family === 0 ? 2 : family, socktype: 0, protocol: 0, address: nodeText, port: 0 }];
    }
    if (numericOnly || !host.resolveHost) return null;
    const resolved = host.resolveHost(nodeText);
    if (resolved.length === 0) return null;
    return family === 0 ? resolved : resolved.filter((entry) => entry.family === family);
  };

  return {
    /**
     * `_usleep` — the glue busy-spins on the monotonic clock until the deadline passes.
     *
     * Spinning is copied as *not* spinning: on a browser main thread a real spin would freeze the
     * frame, and the only observable difference is that this returns immediately. Sleeping the
     * remainder asynchronously is the right shape in a host that has an event loop, and this one
     * deliberately does not pretend to have one.
     */
    Ac: (useconds: number): number =>
      boundary('Ac', () => {
        if (!Number.isFinite(useconds) || useconds <= 0) return 0;
        return 0;
      }),

    /**
     * `_clock_gettime(clockid, struct timespec *)`.
     *
     * Clocks 0 (realtime), 1 and 4 (monotonic) are answered; anything else is `-1` with errno set,
     * which is the one place this import departs from the usual "return a negated errno" shape.
     */
    Bc: (clkId: number, tp: number): number =>
      boundary('Bc', () => {
        const result = clockGettime(memory, clkId, tp, now);
        if (result < 0) {
          // The glue writes `setErrNo(28); return -1` — it returns the SENTINEL -1, not -errno.
          // Returning -28 here would tell the guest a syscall succeeded and returned EINVAL.
          setErrNo(ARTEMIS_ERRNO.EINVAL);
          return -1;
        }
        return result;
      }),

    /** `_strftime_l` — the locale argument is read and ignored, exactly as in the glue. */
    Cb: (s: number, maxsize: number, format: number, tm: number, _locale: number): number =>
      boundary('Cb', () => strftime(memory, s, maxsize, format, tm, zones)),

    /** `_emscripten_idb_async_store` — stores, then calls back through the guest's function table. */
    Cc: (db: number, id: number, ptr: number, num: number, arg: number, onstore: number, onerror: number): void => {
      boundaryVoid('Cc', () => {
        const data = readBytes(ptr, num);
        if (!data) {
          invoke(onerror, arg);
          return;
        }
        idb.store(pointer(db), pointer(id), data);
        invoke(onstore, arg);
      });
    },

    /** `___sys_unlink` */
    Da: (path: number): number =>
      boundary('Da', () => {
        fs.unlink(pointer(path | 0));
        return 0;
      }),

    /** `_emscripten_idb_async_load` — mallocs a copy, hands it over, then frees it. */
    Dc: (db: number, id: number, arg: number, onload: number, onerror: number): void => {
      boundaryVoid('Dc', () => {
        idbLoad(
          db,
          id,
          (target, length) => {
            invoke(onload, arg, target, length);
            host.free?.(target);
          },
          () => invoke(onerror, arg),
        );
      });
    },

    /**
     * `___sys_fcntl64(fd, cmd, va_list)` — the glue's command table.
     *
     * The varargs slot holds a pointer to the argument, and only three commands read it. Note the
     * two odd returns that are not negated errnos: `F_SETFL` reports 0 without doing anything, and
     * `F_GETLK` reports `-1` with errno set. Both are copied because callers branch on them.
     */
    E: (fd: number, cmd: number, varargs: number): number =>
      boundary('E', () => {
        const stream = fs.requireStream(fd);
        // `noUncheckedIndexedAccess` widens the read; an unreadable varargs slot is a zero argument
        // rather than a crash, which is what the guest's own `SYSCALLS.get()` would have seen.
        const arg = (): number => memory.i32[(varargs | 0) >>> 2] ?? 0;
        switch (cmd) {
          case 0: {
            // F_DUPFD: the glue reopens the path onto the requested descriptor.
            const requested = arg();
            if (requested < 0) return -ARTEMIS_ERRNO.EINVAL;
            return fs.dup2(fd, requested);
          }
          case 1:
          case 2:
            return 0;
          case 3:
            return fs.streamFlags(stream);
          case 4: {
            const added = arg();
            if (added >= 0) stream.flags |= added;
            return 0;
          }
          case 12: {
            memory.u16[(arg() | 0) >>> 1] = 2;
            return 0;
          }
          case 13:
          case 14:
            return 0;
          case 8:
          case 16:
            return -ARTEMIS_ERRNO.EINVAL;
          case 9:
            // The glue writes `setErrNo(28); return -1` — the SENTINEL -1, not -errno. Returning
            // -28 here would tell the guest that fcntl failed with EINVAL, which is a different
            // failure: a caller branching on the sentinel treats -1 as "would block / no lock".
            setErrNo(ARTEMIS_ERRNO.EINVAL);
            return -1;
          default:
            return -ARTEMIS_ERRNO.EINVAL;
        }
      }),

    /** `_fd_write` — reports the byte count through `pnum` and fails with a **positive** errno. */
    Ea: (fd: number, iov: number, iovcnt: number, pnum: number): number => scatterGather('Ea', fd, iov, iovcnt, pnum),

    /** `_environ_get` — writes one `KEY=VALUE` pointer per slot; fails with a **positive** errno. */
    Eb: (environ: number, environBuf: number): number =>
      boundary('Eb', () => {
        let offset = 0;
        for (let index = 0; index < env.length; index += 1) {
          const entry = env[index];
          if (entry === undefined) continue;
          const at = environBuf + offset;
          memory.i32[(environ + index * 4) >>> 2] = at;
          offset += writeAscii(memory, at, entry);
        }
        return 0;
      }),

    /** `_emscripten_idb_async_exists` */
    Ec: (db: number, id: number, arg: number, oncheck: number, onerror: number): void => {
      boundaryVoid('Ec', () => {
        try {
          invoke(oncheck, arg, idb.exists(pointer(db), pointer(id)) ? 1 : 0);
        } catch {
          invoke(onerror, arg);
        }
      });
    },

    /** `_environ_sizes_get` — the count, and the total including every NUL. Positive errno. */
    Fb: (count: number, bufSize: number): number =>
      boundary('Fb', () => {
        memory.i32[(count | 0) >>> 2] = env.length;
        let total = 0;
        for (const entry of env) total += entry.length + 1;
        memory.i32[(bufSize | 0) >>> 2] = total;
        return 0;
      }),

    /** `_emscripten_idb_delete` — reports through `perror` and never throws. */
    Fc: (db: number, id: number, perror: number): void => {
      boundaryVoid('Fc', () => {
        try {
          idb.delete(pointer(db), pointer(id));
          memory.i32[(perror | 0) >>> 2] = 0;
        } catch {
          memory.i32[(perror | 0) >>> 2] = 1;
        }
      });
    },

    /** `___localtime_r` */
    Gb: (timePtr: number, tmPtr: number): number =>
      boundary('Gb', () => localtimeR(memory, timePtr, tmPtr, zones)),

    /** `_emscripten_idb_store` — reports through `perror` and never throws. */
    Gc: (db: number, id: number, ptr: number, num: number, perror: number): void => {
      boundaryVoid('Gc', () => {
        const data = readBytes(ptr, num);
        if (!data) {
          memory.i32[(perror | 0) >>> 2] = 1;
          return;
        }
        idb.store(pointer(db), pointer(id), data);
        memory.i32[(perror | 0) >>> 2] = 0;
      });
    },

    /** `___gmtime_r` */
    Hb: (timePtr: number, tmPtr: number): number =>
      boundary('Hb', () => gmtimeR(memory, timePtr, tmPtr, zones)),

    /** `_emscripten_idb_load` — the synchronous row; hands over a buffer the guest must free. */
    Hc: (db: number, id: number, pbuffer: number, pnum: number, perror: number): number =>
      boundary('Hc', () => {
        idbLoad(
          db,
          id,
          (target, length) => {
            memory.i32[(pbuffer | 0) >>> 2] = target;
            memory.i32[(pnum | 0) >>> 2] = length;
            memory.i32[(perror | 0) >>> 2] = 0;
          },
          () => {
            memory.i32[(perror | 0) >>> 2] = 1;
          },
        );
        return 0;
      }),

    /**
     * `___sys__newselect(nfds, readfds, writefds, exceptfds, timeout)`.
     *
     * Two behaviours are copied deliberately. First, `timeout` is read and **never used** — the
     * glue does not sleep, and blocking the thread that drives a frame is a far worse failure than
     * the semantic gap, so this host is equally non-blocking. Second, the inlined
     * `DEFAULT_POLLMASK` of 5 is `POLLIN|POLLOUT` (1|4) and does **not** include `POLLPRI` (2), so
     * an fd named only in `exceptfds` never comes back set and contributes nothing to the count —
     * `5 & 2 === 0`. Every open fd here reports mask 5 (see `ArtemisFs.streamIsReady`), so an fd in
     * the read or write set always comes back set.
     */
    Ib: (nfds: number, readfds: number, writefds: number, exceptfds: number, _timeout: number): number =>
      boundary('Ib', () => {
        // A null set pointer means "this set is empty" in the fd_set ABI, so the read is guarded
        // rather than trusted: the glue does the same via its `set === 0` test.
        const low = (set: number): number => (set === 0 ? 0 : memory.i32[(set | 0) >>> 2] ?? 0);
        const high = (set: number): number => (set === 0 ? 0 : memory.i32[((set | 0) + 4) >>> 2] ?? 0);
        const srcReadLow = low(readfds);
        const srcReadHigh = high(readfds);
        const srcWriteLow = low(writefds);
        const srcWriteHigh = high(writefds);
        const srcExceptLow = low(exceptfds);
        const srcExceptHigh = high(exceptfds);
        const allLow = srcReadLow | srcWriteLow | srcExceptLow;
        const allHigh = srcReadHigh | srcWriteHigh | srcExceptHigh;
        let dstReadLow = 0;
        let dstReadHigh = 0;
        let dstWriteLow = 0;
        let dstWriteHigh = 0;
        let dstExceptLow = 0;
        let dstExceptHigh = 0;
        let total = 0;
        for (let fd = 0; fd < nfds; fd += 1) {
          const mask = 1 << (fd % 32);
          if (fd < 32 ? (allLow & mask) === 0 : (allHigh & mask) === 0) continue;
          if (!fs.hasStream(fd)) {
            throw new ArtemisErrnoError(ARTEMIS_ERRNO.EBADF, `select fd ${fd}`);
          }
          const ready = fs.streamIsReady(fd);
          if ((ready & 1) !== 0 && (fd < 32 ? srcReadLow & mask : srcReadHigh & mask) !== 0) {
            if (fd < 32) dstReadLow |= mask;
            else dstReadHigh |= mask;
            total += 1;
          }
          if ((ready & 4) !== 0 && (fd < 32 ? srcWriteLow & mask : srcWriteHigh & mask) !== 0) {
            if (fd < 32) dstWriteLow |= mask;
            else dstWriteHigh |= mask;
            total += 1;
          }
          if ((ready & 2) !== 0 && (fd < 32 ? srcExceptLow & mask : srcExceptHigh & mask) !== 0) {
            if (fd < 32) dstExceptLow |= mask;
            else dstExceptHigh |= mask;
            total += 1;
          }
        }
        if (readfds !== 0) {
          memory.i32[(readfds | 0) >>> 2] = dstReadLow;
          memory.i32[(readfds + 4) >>> 2] = dstReadHigh;
        }
        if (writefds !== 0) {
          memory.i32[(writefds | 0) >>> 2] = dstWriteLow;
          memory.i32[(writefds + 4) >>> 2] = dstWriteHigh;
        }
        if (exceptfds !== 0) {
          memory.i32[(exceptfds | 0) >>> 2] = dstExceptLow;
          memory.i32[(exceptfds + 4) >>> 2] = dstExceptHigh;
        }
        return total;
      }),

    /**
     * `_emscripten_idb_exists`.
     *
     * Both outputs are written on every path, which is what the glue does: an error still leaves
     * `pexists` at 0 rather than leaving it untouched. A caller that only checks `pexists` would
     * read a stale 1 from a previous call if this wrote conditionally.
     */
    Ic: (db: number, id: number, pexists: number, perror: number): void => {
      boundaryVoid('Ic', () => {
        try {
          memory.i32[(pexists | 0) >>> 2] = idb.exists(pointer(db), pointer(id)) ? 1 : 0;
          memory.i32[(perror | 0) >>> 2] = 0;
        } catch {
          memory.i32[(pexists | 0) >>> 2] = 0;
          memory.i32[(perror | 0) >>> 2] = 1;
        }
      });
    },

    /** `___sys_stat64` */
    Jb: (path: number, buf: number): number => statPath('Jb', path, buf),

    /** `___sys_mkdir` — the glue normalises, strips one trailing slash, and always returns 0. */
    Kb: (path: number, mode: number): number =>
      boundary('Kb', () => {
        let text = ArtemisFs.normalise(pointer(path | 0));
        if (text.endsWith('/')) text = text.slice(0, -1);
        fs.mkdir(text, mode);
        return 0;
      }),

    /** `___sys_rmdir` */
    Lb: (path: number): number =>
      boundary('Lb', () => {
        fs.rmdir(pointer(path | 0));
        return 0;
      }),

    /**
     * `___sys_ioctl(fd, op, va_list)`.
     *
     * The glue answers ENOTTY for every request it knows, and its `default:` arm calls
     * `abort("bad ioctl syscall " + op)`. This is the one place the host knowingly parts company:
     * an unknown request gets ENOTTY too, so a game that probes for a capability it does not have
     * finds out from a failed call instead of from a dead tab. Everything else — including the
     * `TIOCGWINSZ` write of a zero window before the ENOTTY — is as the glue has it.
     */
    Mb: (fd: number, op: number, _varargs: number): number =>
      boundary('Mb', () => {
        fs.requireStream(fd);
        if (op === IOCTL_TIOCGPTN) return -ARTEMIS_ERRNO.ENOTTY;
        if (op === IOCTL_TIOCGWINSZ) return -ARTEMIS_ERRNO.ENOTTY;
        if (op === IOCTL_TIOCSWINSZ) return -ARTEMIS_ERRNO.ENOTTY;
        report('ioctl', `request ${op} needs a tty this host does not have`);
        return -ARTEMIS_ERRNO.ENOTTY;
      }),

    /** `___sys_lstat64` — identical to `stat64` here, because this host has no symlinks. */
    Nb: (path: number, buf: number): number => statPath('Nb', path, buf),

    /**
     * `___sys_socketcall(call, args)`.
     *
     * The glue implements cases 1–7, 11, 12, 14, 15, 16 and 17 over a SOCKFS and returns ENOSYS
     * for anything else. This host has no socket layer, so every case that would need one — with
     * one exception — reports ENOSYS, which is the glue's own answer for a call it does not
     * implement and is also the honest one here: a game host offers no sockets at all.
     *
     * Case 14 is the exception, and it is a literal `return-50` in the glue, reached before any
     * socket is looked up, so the same literal is what this host returns. A fabricated socket that
     * accepted one connection and went nowhere would be a worse lie than a refusal.
     */
    O: (call: number, _varargs: number): number =>
      boundary('O', () => {
        if (call === 14) return -ARTEMIS_ERRNO.ENOPROTOOPT;
        report('socketcall', `call ${call} needs a socket layer this host does not have`);
        return -ARTEMIS_ERRNO.ENOSYS;
      }),

    /**
     * `___clock_gettime` — musl's entry point, which is a straight forward to the C one.
     *
     * The second of its two import rows has the shape `(i32, i64, i32)`, but the glue's
     * `SYSCALLS.get64` is `function(low, high){return low}`, so the extra words are ignored and both
     * rows land on this one function. That is why one export covers both.
     */
    Ob: (clkId: number, tp: number): number =>
      boundary('Ob', () => {
        const result = clockGettime(memory, clkId, tp, now);
        if (result < 0) {
          // The glue writes `setErrNo(28); return -1` — it returns the SENTINEL -1, not -errno.
          // Returning -28 here would tell the guest a syscall succeeded and returned EINVAL.
          setErrNo(ARTEMIS_ERRNO.EINVAL);
          return -1;
        }
        return result;
      }),

    /** `_mktime` — normalises the struct in place and returns seconds. */
    Pb: (tmPtr: number): number => boundary('Pb', () => mktime(memory, tmPtr)),

    /** `_fd_close` — fails with a **positive** errno, like the glue. */
    Q: (fd: number): number =>
      boundary('Q', () => {
        directoryCache.delete(fd);
        fs.close(fd);
        return 0;
      }),

    /** `___sys_rename` */
    Qb: (from: number, to: number): number =>
      boundary('Qb', () => {
        fs.rename(pointer(from | 0), pointer(to | 0));
        return 0;
      }),

    /**
     * `___sys_dup2` — returns the descriptor unchanged when it already aliases the source, and
     * otherwise closes whatever sits there and reopens onto it.
     */
    Rb: (oldfd: number, suggestFd: number): number =>
      boundary('Rb', () => {
        fs.requireStream(oldfd);
        if (oldfd === suggestFd) return suggestFd;
        return fs.dup2(oldfd, suggestFd);
      }),

    /** `___sys_dup3` — the same as dup2 except that a self-dup is EINVAL here and a plain dup2 there. */
    Sb: (fd: number, suggestFd: number, _flags: number): number =>
      boundary('Sb', () => {
        fs.requireStream(fd);
        if (fd === suggestFd) return -ARTEMIS_ERRNO.EINVAL;
        return fs.dup3(fd, suggestFd);
      }),

    /** `___sys_chdir` */
    Tb: (path: number): number =>
      boundary('Tb', () => {
        fs.chdir(pointer(path | 0));
        return 0;
      }),

    /** `_fd_read` — reports the byte count through `pnum` and fails with a **positive** errno. */
    Ub: (fd: number, iov: number, iovcnt: number, pnum: number): number => scatterGather('Ub', fd, iov, iovcnt, pnum),

    /**
     * `_exit(status)` — the glue calls its own `exit()`, which unwinds through `exitRuntime` and
     * never returns. There is no way to reproduce that from inside a wasm import, so this throws
     * {@link ArtemisExit} and lets the session decide what ending the program means: tear the
     * instance down, resolve a promise, or refuse the run outright. Silently returning would let the
     * guest keep executing code that has already quit.
     */
    V: (status: number): void => {
      throw new ArtemisExit(status | 0);
    },

    /** `___sys_getdents64` */
    Vb: (fd: number, dirp: number, count: number): number => getdents(fd, dirp, count),

    /** `_time(time_t *t)` — seconds since the epoch, stored when the pointer is non-zero. */
    ea: (ptr: number): number =>
      boundary('ea', () => {
        const seconds = Math.floor(now() / 1000) | 0;
        if (ptr !== 0) memory.i32[(ptr | 0) >>> 2] = seconds;
        return seconds;
      }),

    /** `_difftime(time1, time0)` — a subtraction, returned as the f64 the import declares. */
    fc: (time1: number, time0: number): number => boundary('fc', () => (time1 | 0) - (time0 | 0)),

    /**
     * `___sys_open(path, flags, va_list)`.
     *
     * `mode` comes from the varargs cell. The glue rejects an empty path with ENOENT, and lets
     * `O_CREAT` imply a regular file — both of which `ArtemisFs.open` already does, so this only
     * has to hand the flags through untouched.
     */
    na: (path: number, flags: number, varargs: number): number =>
      boundary('na', () => {
        const text = pointer(path | 0);
        if (text === '') {
          throw new ArtemisErrnoError(ARTEMIS_ERRNO.ENOENT, 'open ""');
        }
        return fs.open(text, flags, memory.i32[(varargs | 0) >>> 2]);
      }),

    /** `_strftime` */
    qc: (s: number, maxsize: number, format: number, tm: number): number =>
      boundary('qc', () => strftime(memory, s, maxsize, format, tm, zones)),

    /**
     * `_getaddrinfo(node, service, hints, result)`.
     *
     * The glue parses literal addresses locally and only reaches `DNS.lookup_name` when that
     * fails, so a host with no network can still be exactly right about the addresses a game
     * actually asks for — loopback, and any literal a config file names. Only a real name is
     * refused, with `EAI_NONAME`, unless the session supplied a resolver. The two addresses that
     * matter for a single-player game (`127.0.0.1` and `::1`) therefore resolve, and nothing else
     * is invented.
     */
    ub: (node: number, service: number, hint: number, out: number): number =>
      boundary('ub', () => {
        // The guest hands us a `struct addrinfo *` that may be NULL (meaning "defaults, please"),
        // so every field read has to tolerate an absent or unreadable hint rather than assume one.
        const flags = memory.i32[(hint | 0) >>> 2] ?? 0;
        const family = memory.i32[((hint | 0) + 4) >>> 2] ?? 0;
        const socktype = memory.i32[((hint | 0) + 8) >>> 2] ?? 0;
        const protocol = memory.i32[((hint | 0) + 12) >>> 2] ?? 0;
        if ((flags & ~AI_KNOWN) !== 0) return EAI.BADFLAGS;
        if (family !== 0 && SOCKADDR_SIZE[family] === undefined) return EAI.FAMILY;
        if (socktype !== 0 && socktype !== 1 && socktype !== 2 && socktype !== 3) return EAI.SOCKTYPE;
        const nodeText = node === 0 ? '' : pointer(node);
        const serviceText = service === 0 ? '' : pointer(service);
        if (nodeText === '' && serviceText === '') return EAI.NONAME;
        const port = serviceText === '' ? 0 : Number(serviceText);
        if (serviceText !== '' && !Number.isInteger(port)) {
          // A named service ("http") needs /etc/services, which this host has no way to read.
          if ((flags & AI_NUMERICSERV) !== 0 || !host.resolveHost) return EAI.SERVICE;
        }
        const addresses = resolveAddresses(nodeText, flags, family);
        if (addresses === null) return EAI.NONAME;
        const allocate = host.malloc;
        if (!allocate) return EAI.AGAIN;
        let head = 0;
        let previous = 0;
        for (const entry of addresses) {
          const size = SOCKADDR_SIZE[entry.family] ?? 16;
          const record = allocate(ADDRINFO_SIZE);
          const sockaddr = allocate(size);
          if (record === 0 || sockaddr === 0) return EAI.AGAIN;
          if (!writeSockaddr(memory, sockaddr, { ...entry, port })) return EAI.AGAIN;
          memory.u32[(record | 0) >>> 2] = flags >>> 0;
          memory.i32[(record + 4) >>> 2] = entry.family;
          memory.i32[(record + 8) >>> 2] = entry.socktype;
          memory.i32[(record + 12) >>> 2] = entry.protocol;
          memory.i32[(record + 16) >>> 2] = size;
          memory.i32[(record + 20) >>> 2] = sockaddr;
          memory.i32[(record + 24) >>> 2] = 0;
          memory.i32[(record + 28) >>> 2] = 0;
          if (previous === 0) head = record;
          else memory.i32[(previous + 28) >>> 2] = record;
          previous = record;
        }
        memory.i32[(out | 0) >>> 2] = head;
        return 0;
      }),

    /**
     * `_fd_seek(fd, offset_low, offset_high, whence, newOffset)`.
     *
     * `whence` is the fourth argument and `newOffset` the fifth. The i64 offset arrives as two i32
     * words and is rebuilt arithmetically, never through a BigInt, because the glue's own bound
     * rejects anything at or past 2^53 — so the guest cannot address a file larger than 9 PB through
     * this path, and the guest's `fstat` values stay well inside it. `root.pfs` is 2.69 GB, which is
     * exactly the case that makes the i64 write-back matter: the position comes back as a low/high
     * pair because a bare 32-bit word cannot hold it.
     */
    xb: (fd: number, offsetLow: number, offsetHigh: number, whence: number, newOffset: number): number =>
      boundary('xb', () => {
        const offset = offsetHigh * HIGH_OFFSET + (offsetLow >>> 0);
        if (offset <= -DOUBLE_LIMIT || offset >= DOUBLE_LIMIT) {
          // The glue `return`s -61 from the top of its `try`, it does not THROW. That matters:
          // `xb` is one of the six rows whose catch returns a POSITIVE errno, so throwing here
          // would come back through the boundary as +61 and tell the guest the seek returned
          // EOVERFLOW as a successful byte count. Return the literal instead.
          return -ARTEMIS_ERRNO.EOVERFLOW;
        }
        fs.seek(fd, offset, whence);
        if (newOffset !== 0) {
          const position = fs.requireStream(fd).position;
          memory.u32[(newOffset | 0) >>> 2] = position >>> 0;
          memory.u32[(newOffset + 4) >>> 2] = Math.floor(position / HIGH_OFFSET) >>> 0;
        }
        if (offset === 0 && whence === SEEK_SET) {
          // A rewind invalidates the cached listing, which is the glue clearing `stream.getdents`.
          directoryCache.delete(fd);
        }
        return 0;
      }),

    /** `_clock()` — microseconds since the first call, matching the glue's cached start time. */
    zc: (): number =>
      boundary('zc', () => {
        if (!clockStart.started) {
          clockStart.started = true;
          clockStart.value = now();
        }
        return Math.floor((now() - clockStart.value) * 1000);
      }),
  };

  /** Reads `num` bytes at `ptr`, or `null` when the range is not inside the heap. */
  function readBytes(ptr: number, num: number): Uint8Array | null {
    try {
      memory.assertRange(ptr, num);
    } catch {
      return null;
    }
    return memory.u8.slice(ptr, ptr + num);
  }

  /**
   * Calls back into the guest through one of its function-table entries.
   *
   * The glue reaches these with `getDynCaller("vi", fn)`, which is the same table lookup this is:
   * the callback pointers the guest passes are indices into the one table it exports. When a
   * session has not wired that table up, the call is skipped and the import still reports its
   * result through `perror` — the alternative, throwing, would abort a guest that is behaving
   * perfectly well on a host that simply has no way to call back yet.
   */
  function invoke(entry: number, arg: number, extra?: number, count?: number): void {
    const call = host.callback;
    if (!call) {
      report('callback', `no function table wired up for entry ${entry}`);
      return;
    }
    call(entry, arg, extra, count);
  }
}

/** `env` defaults, matching `getEnvStrings()` field for field. */
function envStrings(options: ArtemisEnvOptions | undefined): string[] {
  const language = (options?.language ?? 'C').replace('-', '_');
  const env: Record<string, string> = {
    USER: 'web_user',
    LOGNAME: 'web_user',
    PATH: '/',
    PWD: '/',
    HOME: '/home/web_user',
    LANG: `${language}.UTF-8`,
    _: options?.executableName ?? '/this.program',
  };
  for (const [key, value] of Object.entries(options?.env ?? {})) {
    env[key] = value;
  }
  return Object.keys(env).map((key) => `${key}=${env[key]}`);
}

/**
 * `writeAsciiToMemory`: raw `charCodeAt`, advanced by JS string length rather than UTF-8 byte
 * length, so a non-ASCII value truncates to one byte exactly as the glue does.
 *
 * The return value counts the NUL, because the one caller is `_environ_get` and the glue
 * advances `bufSize += string.length + 1`. Returning the length WITHOUT the terminator makes
 * each entry overwrite its predecessor's NUL, and the whole environ block reads back as one
 * concatenated run.
 */
function writeAscii(memory: ArtemisMemory, buffer: number, text: string): number {
  const bytes = memory.u8;
  let at = buffer;
  for (let index = 0; index < text.length; index += 1) {
    bytes[at] = text.charCodeAt(index) & 0xff;
    at += 1;
  }
  bytes[at] = 0;
  return at - buffer + 1;
}

/** `__inet_pton4_raw` — the dotted quad in the glue's host order. */
function pton4(text: string): number | null {
  const parts = text.split('.');
  if (parts.length !== 4) return null;
  const octets = parts.map((part) => Number(part));
  for (const octet of octets) {
    if (!Number.isInteger(octet) || octet < 0 || octet > 255) return null;
  }
  const [a, b, c, d] = octets as [number, number, number, number];
  return (a | (b << 8) | (c << 16) | (d << 24)) >>> 0;
}

/** `__inet_pton6_raw`, reduced to the four 32-bit words a sockaddr_in6 needs. */
function pton6(text: string): number[] | null {
  if (!text.includes(':')) return null;
  const halves = text.split('::');
  if (halves.length > 2) return null;
  const parse = (chunk: string): string[] | null => {
    if (chunk === '') return [];
    const groups = chunk.split(':');
    return groups.every((group) => /^[0-9a-fA-F]{1,4}$/.test(group)) ? groups : null;
  };
  let groups: string[];
  if (halves.length === 2) {
    const left = parse(halves[0] ?? '');
    const right = parse(halves[1] ?? '');
    if (left === null || right === null) return null;
    const fill = 8 - left.length - right.length;
    if (fill < 0) return null;
    groups = [...left, ...Array.from({ length: fill }, () => '0'), ...right];
  } else {
    const only = parse(halves[0] ?? '');
    if (only === null || only.length !== 8) return null;
    groups = only;
  }
  const words = groups.map((group) => Number.parseInt(group, 16));
  if (words.some((word) => !Number.isInteger(word) || word < 0 || word > 0xffff)) return null;
  return [
    (words[0] ?? 0) | ((words[1] ?? 0) << 16),
    (words[2] ?? 0) | ((words[3] ?? 0) << 16),
    (words[4] ?? 0) | ((words[5] ?? 0) << 16),
    (words[6] ?? 0) | ((words[7] ?? 0) << 16),
  ];
}

/** `__write_sockaddr` — 16 bytes for AF_INET, 28 for AF_INET6, and nothing else. */
function writeSockaddr(memory: ArtemisMemory, sa: number, info: ResolvedAddrInfo): boolean {
  if (info.family === 2) {
    const packed = pton4(info.address);
    if (packed === null) return false;
    memory.u16[sa >>> 1] = 2;
    memory.u16[(sa + 2) >>> 1] = ((info.port >>> 8) & 0xff) | ((info.port & 0xff) << 8);
    memory.u32[(sa + 4) >>> 2] = packed;
    return true;
  }
  if (info.family === 10) {
    const words = pton6(info.address);
    if (words === null) return false;
    memory.u32[sa >>> 2] = 10;
    memory.u16[(sa + 2) >>> 1] = ((info.port >>> 8) & 0xff) | ((info.port & 0xff) << 8);
    memory.u32[(sa + 4) >>> 2] = 0;
    for (let index = 0; index < 4; index += 1) {
      memory.u32[(sa + 8 + index * 4) >>> 2] = words[index] ?? 0;
    }
    memory.u32[(sa + 24) >>> 2] = 0;
    return true;
  }
  return false;
}
