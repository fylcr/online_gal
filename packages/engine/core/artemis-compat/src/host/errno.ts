/**
 * The errno numbering the Artemis bridge was compiled against.
 *
 * These are **musl's** numbers, not Linux's, and the two disagree in places that matter — musl
 * `ENOENT` is 44 where Linux says 2, `ENOTTY` is 59 where Linux says 25. The wasm module is a
 * statically linked musl build, so it compares against these values and nothing else.
 *
 * The values are not guessed. They were read out of the official glue's own `ERRNO_CODES` table
 * (`ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.js`, `var ERRNO_CODES={` at
 * byte 93557) and cross-checked against the negative literals the glue returns inline
 * (`return-28` for EINVAL, `-54` for ENOTDIR, `-59` for ENOTTY, `-61` for EOVERFLOW, `-52` for
 * ENOSYS, `-53` for ENOTCONN, `-44` for ENOENT, `-8` for EBADF). See `.test/team-artemis/`
 * `dump-errno.mjs`, which extracts the table from that one file.
 *
 * Only the numbers this host can actually produce are listed. Adding one costs nothing but must
 * come from the same source — a value invented from a Linux header would silently produce the
 * wrong failure.
 */
export const ARTEMIS_ERRNO = {
  EPERM: 63,
  ENOENT: 44,
  EINTR: 27,
  EIO: 29,
  EBADF: 8,
  EAGAIN: 6,
  ENOMEM: 48,
  EACCES: 2,
  EFAULT: 21,
  EBUSY: 10,
  EEXIST: 20,
  EXDEV: 75,
  ENODEV: 43,
  ENOTDIR: 54,
  EISDIR: 31,
  EINVAL: 28,
  EMFILE: 33,
  ENOTTY: 59,
  ENOSPC: 51,
  ESPIPE: 70,
  EROFS: 69,
  EMLINK: 34,
  EPIPE: 64,
  EDOM: 18,
  ERANGE: 68,
  ENOTEMPTY: 55,
  ENAMETOOLONG: 37,
  ELOOP: 32,
  ENOSYS: 52,
  EDQUOT: 19,
  EOVERFLOW: 61,
  EDEADLK: 16,
  ESTALE: 72,
  ECANCELED: 11,
  EPROTONOSUPPORT: 66,
  EAFNOSUPPORT: 5,
  EADDRINUSE: 3,
  EADDRNOTAVAIL: 4,
  ENETUNREACH: 40,
  ECONNREFUSED: 14,
  EISCONN: 30,
  ENOTCONN: 53,
  ETIMEDOUT: 73,
  EINPROGRESS: 26,
  EOPNOTSUPP: 138,
  EDESTADDRREQ: 17,
  EMSGSIZE: 35,
  EPROTOTYPE: 67,
  EALREADY: 7,
  /**
   * musl 50, and the value the glue's `___sys_socketcall` returns for its two cases that answer
   * with a literal regardless of any socket. It was the one code the host table was missing
   * because no implemented body produced it until the socket row was written.
   */
  ENOPROTOOPT: 50,
} as const;

export type ArtemisErrno = (typeof ARTEMIS_ERRNO)[keyof typeof ARTEMIS_ERRNO];

/**
 * A failure that carries an errno rather than a message.
 *
 * The official glue signals every syscall failure by *throwing* an `FS.ErrnoError` and catching it
 * in the import body, converting it to a return value. The throw is internal bookkeeping; nothing
 * outside a syscall body may see it. This host keeps the same discipline — see `fd.ts`, where every
 * import body is wrapped by `errnoBoundary` so a mistake becomes a return value instead of an
 * exception escaping into wasm.
 */
export class ArtemisErrnoError extends Error {
  constructor(readonly errno: number, what?: string) {
    super(what ? `${what}: errno ${errno}` : `errno ${errno}`);
    this.name = 'ArtemisErrnoError';
  }
}

/** Raised by `_exit`. Distinguishable from a crash: wasm unwound out of the engine on purpose. */
export class ArtemisExit extends Error {
  /**
   * `detail` is an optional reason. `_exit` needs none — the guest is simply done — but
   * `___assert_fail` reuses this signal, and an assertion without its message is a report that
   * cannot be acted on.
   */
  constructor(readonly status: number, detail?: string) {
    super(detail ? `artemis guest exited (${status}): ${detail}` : `artemis called _exit(${status})`);
    this.name = 'ArtemisExit';
  }
}

/**
 * Raised when the host is asked for something it deliberately does not do.
 *
 * Distinct from `ArtemisErrnoError` on purpose. An errno means "the guest asked for a file that is
 * not there" — ordinary, expected, recoverable. This means "the host has no implementation for
 * this", which is a defect to be fixed, not a condition to swallow. `fd.ts` never converts one into
 * the other: an unimplemented path must be visible in the diagnostics rather than returned as a
 * plausible-looking errno.
 */
export class ArtemisHostError extends Error {
  constructor(message: string) {
    super(message);
    this.name = 'ArtemisHostError';
  }
}
