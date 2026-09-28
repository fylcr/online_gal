/**
 * Assembling the Artemis bridge's import table.
 *
 * The bridge is a wasm module with 208 imports, all under the module name `a` (207 functions plus
 * one memory). They fall into seven buckets; this file wires up the `fd` bucket and leaves the other
 * 159 rows to their own modules. The authoritative classification is
 * `ports/artemis/harness/glue/imports-map.json` — 207 rows, generated and committed, not editable
 * here.
 *
 * The two facts that shape everything below:
 *
 * - **Memory is injected, never allocated here.** A host that eagerly built a 3072-page memory
 *   would make a unit test pay 201 MB, and would make the guest's own `memory.grow` policy
 *   invisible. The caller owns the instance and passes it in.
 * - **A wasm i32 argument arrives in JavaScript as a signed 32-bit number.** A guest pointer above
 *   2^31 comes through negative. Every index derived from an argument is squeezed with `| 0` at
 *   the point of use, which is why the bodies read the way they do.
 */

import { createArtemisFdImports, type ArtemisFdHost } from './fd.js';
import { ArtemisMemory, createArtemisMemory, type MemoryLike } from './memory.js';
import { MemoryByteSource, type SyncByteSource } from './fs.js';
import { ArtemisFs } from './tree.js';
import { gameSourceByteSource } from '@online-gal/vfs/archives';
import type { GameSource } from '@online-gal/engine-api';

/** A host function in the bridge's import table. `void` rows return nothing. */
export type ImportFn = (...args: number[]) => number | void;

/**
 * The import table, keyed by the short name the wasm module asks for.
 *
 * Keys are the *short* names (`Ea`, `na`, …), not the official ones. wasm links a function import by
 * the pair (module name, field name), and the bridge declares all 208 under the module `a` with
 * these minified short names. The official name — `_fd_write`, `___sys_open` — is documentation, and
 * lives in `imports-map.json`.
 */
export type ArtemisImportTable = Record<string, ImportFn>;

/** The module name every one of the bridge's imports is declared under. */
export const ARTEMIS_IMPORT_MODULE = 'a';

/**
 * How a filesystem tree is handed to the host.
 *
 * `entries` is a flat list rather than a nested object because that is the shape a PFS table
 * already has: one record per path, parents created on demand. `writable` marks a subtree as
 * mutable — see {@link ArtemisMount}.
 */
export interface ArtemisMount {
  /** Absolute path the tree is mounted at, e.g. `/game`. */
  readonly mountPoint: string;
  /** Flat `[path, contents]` pairs. Paths are absolute and may reference absent parents. */
  readonly entries: Iterable<MountEntry>;
  /** Subtrees that may be written. Everything else is a read-only layer. */
  readonly writable?: readonly string[];
}

/** Re-exported so a caller can build entries without importing two modules. */
export type MountEntry = readonly [string, Uint8Array | string | SyncByteSource];

/**
 * A mount entry may hand over a `SyncByteSource` instead of bytes. This is the only way to mount
 * something larger than memory: `root.pfs` is 2.69 GB, so copying it into a `Uint8Array` is not an
 * option, and a session that did so would be paying for the whole archive to read one table.
 */
export function isByteSource(value: Uint8Array | string | SyncByteSource): value is SyncByteSource {
  return typeof value === 'object' && value !== null && !(value instanceof Uint8Array);
}

/** What a session must supply that the host cannot invent. */
export interface ArtemisHostSetup {
  /** The guest's memory. Required: the host never allocates one on the guest's behalf. */
  readonly memory: ArtemisMemory;
  /** The tree the guest sees. Built by the caller so a test can pass an empty one. */
  readonly fs: ArtemisFs;
  /** A pointer to the guest's `int errno`, for the one import that sets it. */
  readonly errnoLocation?: () => number;
  /** The guest's `malloc`, needed only by `_getaddrinfo` to build result records. */
  readonly malloc?: (bytes: number) => number;
  /** Overrides `Date.now`, so a test can make the clock deterministic. */
  readonly now?: () => number;
  /** Merged over the default environment block, exactly as the glue merges its `ENV` global. */
  readonly env?: Record<string, string>;
  /** Called when the guest asks for something this host refuses, before it is answered with a code. */
  readonly onBlocked?: (what: string, detail: string) => void;
}

/**
 * The fd bucket: 48 rows in `imports-map.json`, 41 distinct short names.
 *
 * The 7 names that appear twice (`Ea Ub Q Ob Jb V xb`) are linked by name, not by row, so one
 * implementation satisfies both entries. Their two type indices differ, which is why the
 * classification table lists them twice — the difference is in how the *caller* marshals the
 * arguments, not in what the body does.
 */
export function createArtemisFdTable(setup: ArtemisHostSetup): ArtemisImportTable {
  const host: ArtemisFdHost = {
    memory: setup.memory,
    fs: setup.fs,
    errnoLocation: setup.errnoLocation,
    malloc: setup.malloc,
    now: setup.now,
    env: setup.env,
    onBlocked: setup.onBlocked,
  };
  return createArtemisFdImports(host) as ArtemisImportTable;
}

/**
 * A memory sized the way the bridge declares it.
 *
 * The bridge asks for `initial: 3072` and `maximum: 65536` pages — 192 MiB up front, 4 GiB of
 * ceiling. Node allocates it lazily (the pages are only backed once touched), so this is affordable
 * in a test, but a test that does not need the real geometry should pass its own memory instead.
 */
export function createBridgeMemory(): ArtemisMemory {
  return createArtemisMemory(3072, 65536);
}

/**
 * Mounts a tree built from flat entries.
 *
 * Thin wrapper over the two calls every caller would otherwise make in sequence, kept here so the
 * reconciliation test and a real session build their trees the same way. `fs.mount` takes
 * `{ path, data }` records with a `SyncByteSource`, so a string is encoded and a byte array wrapped —
 * a caller should not have to know that.
 */
export function mountArtemisTree(
  fs: ArtemisFs,
  mountPoint: string,
  entries: Iterable<MountEntry>,
  writable: readonly string[] = [],
): void {
  const records: { path: string; data: SyncByteSource }[] = [];
  const encoder = new TextEncoder();
  for (const [path, contents] of entries) {
    records.push({
      path,
      data: typeof contents === 'string'
        ? new MemoryByteSource(encoder.encode(contents))
        : isByteSource(contents) ? contents : new MemoryByteSource(contents),
    });
  }
  fs.mount(mountPoint, records);
  for (const path of writable) {
    fs.makeWritable(path);
  }
}

/**
 * The full import table, for the rows that are not fd.
 *
 * The other 159 rows belong to buckets this package does not implement yet — `gl` (99 OpenGL and
 * OpenAL calls), `eh` (17 exception-handling imports), `async` (30 Asyncify re-entry trampolines),
 * `runtime` (8), `asyncify` (4), `todo` (1, `_system`). Each missing row is reported by name rather
 * than stubbed: a stub that answers 0 would let the guest run and draw nothing, which is the
 * failure mode this package exists to avoid.
 */
export interface ImportCoverage {
  /** Short names the table provides. */
  readonly provided: readonly string[];
  /** Short names the bridge still needs, with the bucket each belongs to. */
  readonly missing: readonly { name: string; bucket: string; official: string }[];
}

/** The bridge's 41 fd short names, in the order `imports-map.json` first lists them. */
export const ARTEMIS_FD_IMPORTS: readonly string[] = [
  'Ac', 'Bc', 'Cb', 'Cc', 'Da', 'Dc', 'E', 'Ea', 'Eb', 'Ec', 'Fb', 'Fc', 'Gb', 'Gc', 'Hb', 'Hc',
  'Ib', 'Ic', 'Jb', 'Kb', 'Lb', 'Mb', 'Nb', 'O', 'Ob', 'Pb', 'Q', 'Qb', 'Rb', 'Sb', 'Tb', 'Ub',
  'V', 'Vb', 'ea', 'fc', 'na', 'qc', 'ub', 'xb', 'zc',
];

/** The official name behind each fd short name, for a report a human can read. */
export const ARTEMIS_FD_OFFICIAL: Readonly<Record<string, string>> = {
  Ac: '_usleep', Bc: '_clock_gettime', Cb: '_strftime_l', Cc: '_emscripten_idb_async_store',
  Da: '___sys_unlink', Dc: '_emscripten_idb_async_load', E: '___sys_fcntl64', Ea: '_fd_write',
  Eb: '_environ_get', Ec: '_emscripten_idb_async_exists', Fb: '_environ_sizes_get',
  Fc: '_emscripten_idb_delete', Gb: '___localtime_r', Gc: '_emscripten_idb_store',
  Hb: '___gmtime_r', Hc: '_emscripten_idb_load', Ib: '___sys__newselect',
  Ic: '_emscripten_idb_exists', Jb: '___sys_stat64', Kb: '___sys_mkdir', Lb: '___sys_rmdir',
  Mb: '___sys_ioctl', Nb: '___sys_lstat64', O: '___sys_socketcall', Ob: '___clock_gettime',
  Pb: '_mktime', Q: '_fd_close', Qb: '___sys_rename', Rb: '___sys_dup2', Sb: '___sys_dup3',
  Tb: '___sys_chdir', Ub: '_fd_read', V: '_exit', Vb: '___sys_getdents64', ea: '_time',
  fc: '_difftime', na: '___sys_open', qc: '_strftime', ub: '_getaddrinfo', xb: '_fd_seek',
  zc: '_clock',
};

/**
 * Reads one archive of a real game into mountable entries.
 *
 * The shape a PFS table already has — path, offset, length — is exactly what a lazy mount needs, so
 * nothing is unpacked ahead of time. `gameSourceByteSource` gives a range reader, which is what makes
 * a 2.69 GB `root.pfs` addressable at all: reading it whole would exceed Node's 2 GB buffer ceiling.
 *
 * `limit` exists because a test should mount a handful of files, not 21,587. Entries are returned in
 * catalogue order, which for `root.pfs` is not sorted — the first few are whatever the archive lists
 * first, so a caller that needs a specific file should look it up with `findPfsEntry` instead of
 * assuming position.
 */
export async function entriesFromPfs(
  game: GameSource,
  archive: string,
  limit?: number,
): Promise<MountEntry[]> {
  const source = gameSourceByteSource(game, archive);
  const { readPfsTableFrom, readPfsEntryFrom } = await import('../pfs.js');
  const header = await readPfsTableFrom(source);
  if (!header) return [];
  const entries: MountEntry[] = [];
  for (const entry of header.entries) {
    if (limit !== undefined && entries.length >= limit) break;
    const bytes = await readPfsEntryFrom(source, header, entry);
    if (!bytes) continue;
    const name = entry.path.startsWith('/') ? entry.path : `/${entry.path}`;
    entries.push([`${archive}${name}`, bytes]);
  }
  return entries;
}

/** Re-exported for callers that only need the memory interface. */
export type { MemoryLike };
