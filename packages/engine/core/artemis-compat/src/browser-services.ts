/**
 * Browser service layer for the Artemis port: the pieces of the shipped HTML shell that the
 * WebAssembly module reaches for through `EM_ASM` and `FS.syncfs`, rebuilt as an explicit seam.
 *
 * The original build glued these into `Module` as closures over page globals — a fullscreen video
 * element, a `syncing` counter, `IndexedDBFS`, and `window.open`. None of that survives a port that
 * has no `eval`, no inline `<script>` and no `IDBFS`, so each one is implemented here against the
 * host's own interfaces: the session core's filesystem and memory, the host's `WritableStore`, and
 * the canvas the runtime already owns.
 *
 * Two rules shape the whole file:
 *
 * - **A synchronous ABI cannot await.** `_emscripten_asm_const_int` demands a number *now*, so every
 *   code below either performs its effect synchronously or starts asynchronous work and reports
 *   honestly that it is still running. Nothing ever returns 0 to mean "pretend it worked".
 * - **Only `assets.savePath` is ours to persist.** The game tree is mounted read-only and must never
 *   be enumerated into a snapshot; the save root is the single directory this layer writes, reads
 *   and deletes inside.
 */

import type { RuntimeHost } from '@online-gal/engine-api';

import { ArtemisHostError, ARTEMIS_ERRNO } from './host/errno.js';
import { ArtemisSyncIdb } from './host/fd.js';
import type { FsNode } from './host/fs.js';
import type { ArtemisFs } from './host/tree.js';
import type { ArtemisMemory } from './host/memory.js';
import type { ArtemisAsmConstRunner } from './host/runtime.js';
import type { ArtemisSessionCore } from './session-core.js';

/** The `EM_ASM` codes the shipped `artemis.js` table defines. Anything else is a porting bug. */
export const ARTEMIS_BROWSER_ASM_CODES = {
  videoReset: 710692,
  videoCopyReady: 710801,
  videoEnded: 710864,
  videoWidth: 710922,
  videoHeight: 710967,
  videoOpen: 711012,
  videoPause: 712047,
  videoUpload: 712081,
  alert: 712896,
  confirm: 712921,
  mountLoad: 716400,
  syncState: 716732,
  syncSave: 716748,
  openUrl: 717384,
} as const;

/** Version tag written into the persisted document; a reader that sees another one must refuse it. */
export const ARTEMIS_BROWSER_SNAPSHOT_VERSION = 1;

/** One saved file, addressed by a path relative to the save root. The array is cloned on read. */
export type ArtemisBrowserSnapshotFile = readonly [relativePath: string, bytes: Uint8Array];

/** One saved IndexedDB record. `id` is the guest's own record id within `db`. */
export type ArtemisBrowserSnapshotIdb = readonly [db: string, id: string, bytes: Uint8Array];

export interface ArtemisBrowserSnapshot {
  readonly version: number;
  readonly files: readonly ArtemisBrowserSnapshotFile[];
  readonly idb: readonly ArtemisBrowserSnapshotIdb[];
}

/** The result of the last failure this layer saw, so a caller can report it instead of a silent 0. */
export interface ArtemisBrowserReceipt {
  readonly ok: boolean;
  readonly syncing: boolean;
  readonly lastError?: string;
}

export interface ArtemisBrowserServices {
  /** The IndexedDB stand-in the guest's `_emscripten_idb_*` calls reach; a tracked subclass. */
  readonly idb: ArtemisSyncIdb;
  /** The `EM_ASM` entry point handed to the session core as `runAsmConst`. */
  readonly runAsmConst: ArtemisAsmConstRunner;
  /** Binds a core and hydrates persisted state **before** the guest boots. Memoized. */
  attach(core: ArtemisSessionCore): Promise<void>;
  /** Writes the current save root and idb to the host store. Resolves when the write lands. */
  flush(): Promise<void>;
  /** Clones the save root and idb. Never enumerates the read-only game tree. */
  snapshot(): Promise<ArtemisBrowserSnapshot>;
  /** Replaces the save root and idb from a snapshot. Validates the whole document first. */
  restore(snapshot: ArtemisBrowserSnapshot): Promise<void>;
  /** Suspends video I/O and starts refusing persistence without tearing anything down. */
  pause(): void;
  /** Re-arms persistence. Does not re-hydrate: the state is already in memory. */
  resume(): Promise<void>;
  /** Final teardown: stops sync, kills video I/O, awaits the in-flight store write. Not the core. */
  dispose(): Promise<void>;
}

/**
 * `ArtemisSyncIdb` with a mirror of every mutation.
 *
 * The base class is the authority — the guest's `_emscripten_idb_*` calls go through it — but its
 * map is private, and a snapshot needs to walk it. Rather than reflect into private state, this
 * subclass keeps its own copy of each record and delegates to `super` for the real behaviour, so
 * the mirror is always written by the same call that writes the store. Reads still come from
 * `super`: hydration uses the same public API the guest does, so there is exactly one code path.
 */
export class ArtemisBrowserIdb extends ArtemisSyncIdb {
  /** Mirrored records, keyed exactly as the base class keys its own map. */
  private readonly mirror = new Map<string, Uint8Array>();

  private static mirrorKey(db: string, id: string): string {
    return `${db}\u0000${id}`;
  }

  override store(db: string, id: string, data: Uint8Array): void {
    super.store(db, id, data);
    // A fresh slice, not a view: the caller's buffer is guest-visible and will be rewritten.
    this.mirror.set(ArtemisBrowserIdb.mirrorKey(db, id), data.slice());
  }

  override delete(db: string, id: string): void {
    super.delete(db, id);
    this.mirror.delete(ArtemisBrowserIdb.mirrorKey(db, id));
  }

  override clear(): void {
    super.clear();
    this.mirror.clear();
  }

  /** Snapshot of every mirrored record, already cloned. Internal to this module. */
  mirrorEntries(): ArtemisBrowserSnapshotIdb[] {
    const entries: ArtemisBrowserSnapshotIdb[] = [];
    for (const [key, bytes] of this.mirror) {
      const split = key.indexOf('\u0000');
      // The key is built by this class, so a missing separator is impossible; the guard keeps the
      // destructuring honest under `noUncheckedIndexedAccess` rather than asserting it away.
      if (split < 0) continue;
      entries.push([key.slice(0, split), key.slice(split + 1), bytes.slice()]);
    }
    return entries;
  }

  /** Replaces the mirror from a snapshot. Internal to this module; hydration calls it. */
  mirrorReplace(entries: readonly ArtemisBrowserSnapshotIdb[]): void {
    this.clear();
    for (const [db, id, bytes] of entries) this.store(db, id, bytes);
  }
}

/** Directory entries are `['/', name]` pairs in `idb`; the record `id` is the second half. */
function idbRecordFromKey(key: string): { db: string; id: string } | null {
  const split = key.indexOf('\u0000');
  if (split < 0) return null;
  return { db: key.slice(0, split), id: key.slice(split + 1) };
}

/** `fs` argument used by `readdir`-shaped helpers; kept out of the hot path but exported for tests. */
export const ARTEMIS_BROWSER_IDB_KEY_SEPARATOR = '\u0000';

/** Guard against writing a path that is not strictly inside the save root. */
export function assertArtemisSaveRelative(relativePath: string): string {
  if (relativePath.length === 0) throw new ArtemisHostError('artemis save path is empty');
  if (relativePath.includes('\u0000')) {
    throw new ArtemisHostError('artemis save path contains a NUL');
  }
  const normalised = relativePath.replace(/\\/g, '/');
  if (normalised.startsWith('/')) {
    throw new ArtemisHostError(`artemis save path ${relativePath} is absolute`);
  }
  if (/^[A-Za-z]:/.test(normalised)) {
    throw new ArtemisHostError(`artemis save path ${relativePath} names a drive`);
  }
  for (const segment of normalised.split('/')) {
    if (segment === '..') {
      throw new ArtemisHostError(`artemis save path ${relativePath} escapes the save root`);
    }
    if (segment === '') throw new ArtemisHostError(`artemis save path ${relativePath} is not normalised`);
  }
  return normalised;
}

/**
 * Canonicalises the mount point the guest hands to `716400`.
 *
 * `assets.savePath` is engine data, and engines write both `/save/game/` and `gameone/`: two
 * spellings of one directory. Everything downstream keys off the canonical form, so the trailing
 * slash is dropped and a relative save path becomes an absolute one at the VFS root — `/gameone`.
 * That keeps the store namespace (`savePath`, verbatim) and the filesystem root (`saveMountPath`)
 * separable: the first is a name a player's save already lives under, the second is a location.
 *
 * The path is checked rather than trusted, because this is where a guest-supplied string becomes a
 * place we persist from: no NUL, no `..`, no `.`, no empty interior segment, no drive letter, no
 * URL. Empty and root are refused — neither names a save to hydrate. A foreign mount is refused
 * rather than silently adopted.
 */
export function assertArtemisMountPoint(mountPoint: string): string {
  if (mountPoint.length === 0) throw new ArtemisHostError('artemis mount point is empty');
  if (mountPoint.includes('\u0000')) {
    throw new ArtemisHostError('artemis mount point contains a NUL');
  }
  const normalised = mountPoint.replace(/\\/g, '/');
  if (/^[A-Za-z]:/.test(normalised) || /^[a-z][a-z0-9+.-]*:\/\//i.test(normalised)) {
    throw new ArtemisHostError(`artemis mount point ${mountPoint} names a drive or a URL`);
  }
  const absolute = normalised.startsWith('/');
  // A trailing slash is a spelling, not a segment: `gameone/` and `/save/game/` are directories whose
  // last segment is `gameone` and `game`. Only the *trailing* run is stripped; an interior `//` — a
  // real empty segment — is not, and is refused below.
  const body = normalised.replace(/\/+$/, '');
  if (body === '') {
    throw new ArtemisHostError(`artemis mount point ${mountPoint} names no directory`);
  }
  const segments = body.split('/');
  if (absolute) segments.shift();
  for (const segment of segments) {
    if (segment === '' || segment === '.' || segment === '..') {
      throw new ArtemisHostError(`artemis mount point ${mountPoint} is not normalised`);
    }
  }
  return `/${segments.join('/')}`;
}

/** Flattens any thrown value into a one-line report; a diagnostic must never itself throw. */
function describeError(error: unknown): string {
  if (error instanceof Error) return error.message;
  return String(error);
}

function isPersistenceKeyOf(savePath: string): string {
  return `artemis/${encodeURIComponent(savePath)}/state-v1.json`;
}

/** `Uint8Array` ↔ base64, without depending on a DOM `btoa` that may not exist in every host. */
const BASE64_ALPHABET = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';

const BASE64_LOOKUP: Int16Array = (() => {
  const table = new Int16Array(128).fill(-1);
  for (let index = 0; index < BASE64_ALPHABET.length; index += 1) {
    table[BASE64_ALPHABET.charCodeAt(index)] = index;
  }
  return table;
})();

function toBase64(bytes: Uint8Array): string {
  let out = '';
  let index = 0;
  for (; index + 2 < bytes.length; index += 3) {
    const word = ((bytes[index] as number) << 16) | ((bytes[index + 1] as number) << 8) | (bytes[index + 2] as number);
    out += BASE64_ALPHABET[(word >> 18) & 63];
    out += BASE64_ALPHABET[(word >> 12) & 63];
    out += BASE64_ALPHABET[(word >> 6) & 63];
    out += BASE64_ALPHABET[word & 63];
  }
  const rest = bytes.length - index;
  if (rest === 1) {
    const word = (bytes[index] as number) << 16;
    out += BASE64_ALPHABET[(word >> 18) & 63];
    out += BASE64_ALPHABET[(word >> 12) & 63];
    out += '==';
  } else if (rest === 2) {
    const word = ((bytes[index] as number) << 16) | ((bytes[index + 1] as number) << 8);
    out += BASE64_ALPHABET[(word >> 18) & 63];
    out += BASE64_ALPHABET[(word >> 12) & 63];
    out += BASE64_ALPHABET[(word >> 6) & 63];
    out += '=';
  }
  return out;
}

function fromBase64(text: string): Uint8Array {
  if (text.length % 4 !== 0) throw new ArtemisHostError('artemis snapshot holds malformed base64');
  let padding = 0;
  if (text.endsWith('==')) padding = 2;
  else if (text.endsWith('=')) padding = 1;
  const bytes = new Uint8Array((text.length / 4) * 3 - padding);
  let out = 0;
  for (let index = 0; index < text.length; index += 4) {
    let word = 0;
    for (let offset = 0; offset < 4; offset += 1) {
      const code = text.charCodeAt(index + offset);
      const value = code === 61 ? 0 : code < 128 ? (BASE64_LOOKUP[code] as number) : -1;
      if (value < 0) throw new ArtemisHostError('artemis snapshot holds malformed base64');
      word = (word << 6) | value;
    }
    if (out < bytes.length) bytes[out++] = (word >> 16) & 0xff;
    if (out < bytes.length) bytes[out++] = (word >> 8) & 0xff;
    if (out < bytes.length) bytes[out++] = word & 0xff;
  }
  return bytes;
}

/** Wire shape of one persisted file record; kept flat so the JSON stays readable and diffable. */
interface PersistedFile {
  readonly path: string;
  readonly data: string;
}

interface PersistedIdb {
  readonly db: string;
  readonly id: string;
  readonly data: string;
}

interface PersistedState {
  readonly version: number;
  readonly files: readonly PersistedFile[];
  readonly idb: readonly PersistedIdb[];
}

/**
 * Parses a persisted document, rejecting rather than repairing.
 *
 * Every field is checked for shape *and* for meaning: a path that would escape the save root is as
 * malformed as a missing one, and base64 that does not decode is not a file. A store that has been
 * truncated, edited, or written by an incompatible version must surface as an error the caller can
 * show — silently emptying the save would look to the player like their progress vanished.
 */
export function parseArtemisBrowserSnapshot(json: string): ArtemisBrowserSnapshot {
  let raw: unknown;
  try {
    raw = JSON.parse(json);
  } catch (error) {
    throw new ArtemisHostError(`artemis save state is not valid JSON: ${describeError(error)}`);
  }
  if (typeof raw !== 'object' || raw === null) {
    throw new ArtemisHostError('artemis save state is not an object');
  }
  const record = raw as Partial<PersistedState>;
  if (record.version !== ARTEMIS_BROWSER_SNAPSHOT_VERSION) {
    throw new ArtemisHostError(
      `artemis save state version ${String(record.version)} is not version ${ARTEMIS_BROWSER_SNAPSHOT_VERSION}`,
    );
  }
  if (!Array.isArray(record.files) || !Array.isArray(record.idb)) {
    throw new ArtemisHostError('artemis save state is missing its files or idb array');
  }
  const files: ArtemisBrowserSnapshotFile[] = [];
  for (const entry of record.files as unknown[]) {
    if (typeof entry !== 'object' || entry === null) throw new ArtemisHostError('artemis save entry is not an object');
    const file = entry as Partial<PersistedFile>;
    if (typeof file.path !== 'string' || typeof file.data !== 'string') {
      throw new ArtemisHostError('artemis save entry is missing a string path or data');
    }
    files.push([assertArtemisSaveRelative(file.path), fromBase64(file.data)]);
  }
  const idb: ArtemisBrowserSnapshotIdb[] = [];
  for (const entry of record.idb as unknown[]) {
    if (typeof entry !== 'object' || entry === null) throw new ArtemisHostError('artemis idb entry is not an object');
    const item = entry as Partial<PersistedIdb>;
    if (typeof item.db !== 'string' || typeof item.id !== 'string' || typeof item.data !== 'string') {
      throw new ArtemisHostError('artemis idb entry is missing a string db, id or data');
    }
    idb.push([item.db, item.id, fromBase64(item.data)]);
  }
  return { version: ARTEMIS_BROWSER_SNAPSHOT_VERSION, files, idb };
}

/** Serialises a snapshot. Bytes are base64 so the document is valid JSON in any encoding. */
export function serialiseArtemisBrowserSnapshot(snapshot: ArtemisBrowserSnapshot): string {
  const payload: PersistedState = {
    version: ARTEMIS_BROWSER_SNAPSHOT_VERSION,
    files: snapshot.files.map(([path, bytes]) => ({ path, data: toBase64(bytes) })),
    idb: snapshot.idb.map(([db, id, bytes]) => ({ db, id, data: toBase64(bytes) })),
  };
  return JSON.stringify(payload);
}

/** One file in the save root, plus the directories that led to it. */
interface CollectedFile {
  readonly relativePath: string;
  readonly bytes: Uint8Array;
}

/**
 * Walks the save root and collects regular files, cloning their bytes as it goes.
 *
 * Deliberately recursive rather than `fs.readdir`-driven: the guest mutates this tree between
 * frames, and the only stable handle on a node is the node itself, so the walk follows the `NUL`
 * of `children` maps it already holds instead of re-resolving paths that may have moved.
 */
function collectSaveRoot(root: FsNode): CollectedFile[] {
  const files: CollectedFile[] = [];
  const walk = (node: FsNode, prefix: string): void => {
    if (node.isFile) {
      const size = node.size;
      files.push({ relativePath: prefix, bytes: node.data ? node.data.readRange(0, size).slice() : new Uint8Array(0) });
      return;
    }
    if (!node.isDir) return;
    for (const [name, child] of node.children) {
      walk(child, prefix.length === 0 ? name : `${prefix}/${name}`);
    }
  };
  walk(root, '');
  return files;
}

/** Recursively removes the *contents* of a directory without touching the directory itself. */
function clearDirectory(fs: ArtemisFs, node: FsNode): void {
  for (const [name, child] of [...node.children]) {
    const path = `${node.path === '/' ? '' : node.path}/${name}`;
    if (child.isDir) {
      clearDirectory(fs, child);
      fs.rmdir(path);
    } else {
      fs.unlink(path);
    }
  }
}

/**
 * Builds the browser service layer.
 *
 * `canvas` is the same element the runtime renders into; `712081` uploads a video frame to its
 * WebGL context, so the services must not open a second context of their own.
 */
export function createArtemisBrowserServices(
  host: RuntimeHost,
  canvas: HTMLCanvasElement,
): ArtemisBrowserServices {
  const idb = new ArtemisBrowserIdb();
  if (!host) throw new ArtemisHostError('artemis browser services need a runtime host');
  if (!canvas) throw new ArtemisHostError('artemis browser services need the runtime canvas');

  // The document and view are read once from the canvas this runtime owns. Every DOM seam below —
  // video creation, dialogs, new windows — uses this view, never a global one: a port that reached
  // for `globalThis` would drive a different window than the one the runtime renders into.
  const document = canvas.ownerDocument;
  const view = document.defaultView;

  /** A dialog is only honest if this view can actually show it; there is no silent no-op. */
  function requireDialog(name: 'alert' | 'confirm'): (message?: string) => unknown {
    const method = view?.[name];
    if (typeof method !== 'function') {
      throw new ArtemisHostError(`artemis EM_ASM ${name} needs a view that can show a ${name}`);
    }
    return method as (message?: string) => unknown;
  }

  /** Set by `attach`; every code that needs the core refuses until then. */
  let core: ArtemisSessionCore | null = null;
  /** The save root path, captured at attach **verbatim**. Also the persistence key's namespace. */
  let savePath = '';
  /** `/save/game/` and `gameone/` are one directory; this is that one filesystem root. */
  let saveMountPath = '';
  /** `saveMountPath` is only a location until it is hydrated through `716400`. */
  let mountAttached = false;
  /** Resolved from `core.fs` at attach; the node all saves are confined to. */
  let saveRoot: FsNode | null = null;
  /** Memoized attach, so a second call is a no-op rather than a second hydration. */
  let attaching: Promise<void> | null = null;
  /** Outstanding hydration + flush count, reported verbatim by `716732`. */
  let syncing = 0;
  /** Set once `dispose` runs: every queued operation checks it and refuses. */
  let disposed = false;
  /** Deferred until `resume`; distinct from `disposed` because the core stays alive. */
  let paused = false;
  let lastError: string | undefined;
  /** The tail of the persistence chain, so `flush` can await an in-flight write. */
  let pendingWrite: Promise<void> = Promise.resolve();

  // ---------------------------------------------------------------------------------------------
  // Fullscreen video
  // ---------------------------------------------------------------------------------------------
  /** The detached `<video>` the port owns. Never appended to the document. */
  let video: HTMLVideoElement | null = null;
  /** Flags mirrored from the shipped globals, kept as one record so they reset together. */
  const videoFlags = { playing: false, timeupdate: false, ended: false };
  /** Bumped on every `711012`; a completion from an older generation must not revive state. */
  let videoGeneration = 0;
  /** The object URL we created, revoked exactly once — never a URL we did not mint. */
  let videoObjectUrl: string | null = null;
  let videoHandlers: { playing: EventListener; timeupdate: EventListener; ended: EventListener } | null = null;
  /** `true` while the current video's frame loop is allowed to upload. */
  let videoIoEnabled = true;

  /** Records the last failure and returns a receipt, so no caller has to invent a success. */
  function receipt(): ArtemisBrowserReceipt {
    return { ok: lastError === undefined, syncing: syncing > 0, lastError };
  }

  /** Safe diagnostic: never throws, never promotes a failure into a crash of the ASM path. */
  function report(message: string, error?: unknown): void {
    if (error !== undefined) lastError = `${message}: ${describeError(error)}`;
    else lastError = message;
    try {
      host.log('error', `artemis browser: ${lastError}`);
    } catch {
      // A host whose logger throws must not take the guest down with it.
    }
  }

  function note(message: string): void {
    try {
      host.log('debug', `artemis browser: ${message}`);
    } catch {
      // Diagnostics are best-effort by definition.
    }
  }

  /** Releases the object URL we created. A URL we did not mint is never revoked. */
  function revokeOwnedUrl(): void {
    if (videoObjectUrl === null) return;
    const owned = videoObjectUrl;
    videoObjectUrl = null;
    try {
      URL.revokeObjectURL(owned);
    } catch (error) {
      report('revoking the video object URL failed', error);
    }
  }

  /** Detaches the three listeners this layer added; the shipped shell simply dropped the element. */
  function removeVideoHandlers(): void {
    if (video === null || videoHandlers === null) {
      videoHandlers = null;
      return;
    }
    const element = video;
    const handlers = videoHandlers;
    videoHandlers = null;
    element.removeEventListener('playing', handlers.playing, true);
    element.removeEventListener('timeupdate', handlers.timeupdate, true);
    element.removeEventListener('ended', handlers.ended, true);
  }

  /** Suspends playback but **keeps** the element, its object URL, its listeners and its position. */
  function suspendPlayback(): void {
    const element = video;
    if (element === null) return;
    try {
      element.pause();
    } catch (error) {
      report('pausing the fullscreen video failed', error);
    }
  }

  /** Returns the shared video element to its reset state and stops whatever it was playing. */
  function teardownVideo(): void {
    videoGeneration += 1;
    removeVideoHandlers();
    const element = video;
    video = null;
    videoFlags.playing = false;
    videoFlags.timeupdate = false;
    videoFlags.ended = false;
    if (element !== null) {
      try {
        element.pause();
        // Detach the source so the decoder is released even if the element is retained by a caller.
        element.removeAttribute('src');
        element.load();
      } catch (error) {
        report('stopping the fullscreen video failed', error);
      }
    }
    revokeOwnedUrl();
  }

  /** `716400` — the guest is telling us the save mount is live, which is our hydration trigger. */
  function handleMount(mountPoint: string): number {
    // Canonicalised first, so the comparison below is between two spellings of the same thing.
    const mount = assertArtemisMountPoint(mountPoint);
    if (core === null) throw new ArtemisHostError('artemis mount requested before the core was attached');
    if (saveMountPath !== '' && mount !== saveMountPath) {
      // A mount of some *other* directory would hydrate a second tree from the same document and
      // then persist that tree back under it. Refused, never adopted.
      throw new ArtemisHostError(
        `artemis mount ${mount} is outside the attached save root ${saveMountPath}`,
      );
    }
    if (disposed || paused) {
      note(`mount ${mount} noted while ${disposed ? 'disposed' : 'paused'}; not loading`);
      return 0;
    }
    // An already-hydrated core is idempotent here: the counter must not be bumped for a second read
    // that will not happen, or the guest would spin on `716732` forever.
    if (attachedHydrated || mountAttached) {
      note(`mount ${mount} already hydrated; nothing to load`);
      return 0;
    }
    // Attach hydrated the canonical root. A guest that asks for it again gets no second read; a
    // guest that asks for the root before attach did is hydrated here, once.
    if (saveRoot !== null) {
      mountAttached = true;
      return 0;
    }
    mountAttached = true;
    if (hydrating === null) {
      hydrating = queuePersistedLoad(mount);
    }
    return 0;
  }

  /** True once the attach-time hydration finished; guards the idempotent `716400` path. */
  let attachedHydrated = false;
  /** The in-flight hydration started by `716400`, memoized for the same reason. */
  let hydrating: Promise<void> | null = null;

  // ---------------------------------------------------------------------------------------------
  // FS <-> snapshot
  // ---------------------------------------------------------------------------------------------

  /** Snapshot of the save root only. The game tree is read-only and is never walked. */
  function snapshotSaveRoot(): ArtemisBrowserSnapshotFile[] {
    if (saveRoot === null) return [];
    return collectSaveRoot(saveRoot).map(
      (file): ArtemisBrowserSnapshotFile => [file.relativePath, file.bytes],
    );
  }

  /**
   * Replaces the save root's contents from a snapshot, inside the caller's `enqueue`.
   *
   * Only nodes under the live save root are touched. Old files are removed with `fs.unlink` and
   * directories with `fs.rmdir`, because both are the real VFS entry points that also clear the
   * write scope; writing a file with `seedFile` marks it writable, and the save root itself is
   * re-promoted afterwards so the guest's own writes keep landing.
   */
  function applySnapshotToFileSystem(fs: ArtemisFs, rootPath: string, files: readonly ArtemisBrowserSnapshotFile[]): void {
    const root = fs.mkdirp(rootPath);
    fs.makeWritable(rootPath);
    // Everything currently under the save root is stale; the snapshot is the whole truth.
    clearDirectory(fs, root);
    for (const [relativePath, bytes] of files) {
      const relative = assertArtemisSaveRelative(relativePath);
      const absolute = `${rootPath}/${relative}`;
      const segments = relative.split('/');
      const parent = segments.slice(0, -1).join('/');
      if (parent.length > 0) {
        fs.mkdirp(`${rootPath}/${parent}`);
        fs.makeWritable(`${rootPath}/${parent}`);
      }
      const existing = fs.lookup(absolute);
      if (existing !== null) {
        // A snapshot may legitimately name a directory where a file once was.
        if (existing.isDir) clearDirectory(fs, existing);
        fs.unlink(absolute);
      }
      const node = fs.createFile(absolute, bytes.slice());
      fs.makeWritable(absolute);
      node.touch();
    }
    fs.makeWritable(rootPath);
  }

  /** Reads the persisted document. An absent document is an empty store, by contract. */
  async function readPersisted(): Promise<ArtemisBrowserSnapshot> {
    const key = isPersistenceKeyOf(savePath);
    const raw = await host.store.read(key);
    if (raw === undefined) return { version: ARTEMIS_BROWSER_SNAPSHOT_VERSION, files: [], idb: [] };
    // A store that cannot be read is an I/O failure, not an empty save; it propagates.
    return parseArtemisBrowserSnapshot(new TextDecoder('utf-8', { fatal: true }).decode(raw));
  }

  /**
   * The hydration a `716400` mount point asks for, wrapped so the guest's `716732` sees it.
   *
   * The mount point is not necessarily `assets.savePath`; it is whatever directory the guest wants
   * hydrated. State nonetheless lives under `assets.savePath`'s key, so a mount of a *different*
   * directory restores from the same document — the alternative is a per-mount copy of the save.
   */
  function queuePersistedLoad(mountPoint: string): Promise<void> {
    if (core === null) return Promise.resolve();
    const bound = core;
    syncing += 1;
    return bound
      .enqueue(async () => {
        if (disposed) throw new ArtemisHostError('artemis browser services are disposed');
        const state = await readPersisted();
        applySnapshotToFileSystem(bound.fs, mountPoint, state.files);
        idb.mirrorReplace(state.idb);
        // The guest may write inside the mounted directory as soon as this returns.
        saveRoot = bound.fs.mkdirp(mountPoint);
        bound.fs.makeWritable(mountPoint);
        note(`hydrated ${state.files.length} file(s) and ${state.idb.length} idb record(s) into ${mountPoint}`);
      })
      .catch((error: unknown) => {
        report(`hydrating ${mountPoint} failed`, error);
        throw error;
      })
      .finally(() => {
        syncing = Math.max(0, syncing - 1);
        hydrating = null;
      });
  }

  /**
   * Reads the whole state into clones. Synchronous and side-effect free, so it can be taken **inside**
   * the guest's queue unit and handed to the store afterwards.
   */
  function capturePersisted(): { key: string; snapshot: ArtemisBrowserSnapshot; bytes: Uint8Array } {
    if (core === null) throw new ArtemisHostError('artemis browser services have no core to persist');
    if (savePath === '') throw new ArtemisHostError('artemis browser services have no save path');
    const snapshot: ArtemisBrowserSnapshot = {
      version: ARTEMIS_BROWSER_SNAPSHOT_VERSION,
      files: snapshotSaveRoot(),
      idb: idb.mirrorEntries(),
    };
    const json = serialiseArtemisBrowserSnapshot(snapshot);
    return { key: isPersistenceKeyOf(savePath), snapshot, bytes: new TextEncoder().encode(json) };
  }

  /**
   * Writes an already captured state. The snapshot is immutable by the time it gets here, so the live
   * tree may change underneath it without tearing the pending document — that is the whole reason
   * capture and write are separate.
   */
  async function writePersisted(captured: {
    key: string;
    snapshot: ArtemisBrowserSnapshot;
    bytes: Uint8Array;
  }): Promise<void> {
    // The single-document write IS the atomicity: the store replaces the document or it fails.
    await host.store.write(captured.key, captured.bytes);
    note(`persisted ${captured.snapshot.files.length} file(s) and ${captured.snapshot.idb.length} idb record(s)`);
  }

  /**
   * Captures the state now (the caller is synchronous, so this is the tree as the guest sees it) and
   * chains the resulting write onto the persistence tail. No `core.enqueue` anywhere in here: the
   * ASM path already runs inside a queue unit, and re-entering that queue from within one deadlocks.
   */
  function queuePersist(): Promise<void> {
    const captured = capturePersisted();
    const next = pendingWrite.then(
      () => writePersisted(captured),
      () => writePersisted(captured),
    );
    // Keep the chain alive on failure while still surfacing it to this caller.
    pendingWrite = next.then(
      () => undefined,
      () => undefined,
    );
    return next;
  }

  // ---------------------------------------------------------------------------------------------
  // ASM codes
  // ---------------------------------------------------------------------------------------------

  /** Resolves a guest pointer to text through the core's memory, refusing when unattached. */
  function guestString(pointer: number): string {
    if (core === null) throw new ArtemisHostError('artemis guest string requested before the core was attached');
    const memory: ArtemisMemory = core.memory;
    // `memory.grow` detaches the old buffer, so the views must be re-derived before every read — a
    // stale `u8` is a zero-length view over a detached buffer, and the guest string decodes to ''.
    memory.refresh();
    return memory.utf8((pointer | 0) >>> 0);
  }

  /** The canvas's WebGL context, checked rather than assumed. */
  function requireWebgl(): WebGLRenderingContext {
    const context = canvas.getContext('webgl');
    if (context === null) {
      // The shipped shell would have thrown a TypeError here; the honest failure is an error, and
      // returning 0 would tell the guest its frame uploaded when nothing did.
      throw new ArtemisHostError('artemis EM_ASM 712081 needs a WebGL context the canvas does not have');
    }
    return context as WebGLRenderingContext;
  }

  /** `712081` — uploads the current video frame to the canvas texture. */
  function uploadVideoFrame(): number {
    if (video === null) throw new ArtemisHostError('artemis EM_ASM 712081 has no fullscreen video to upload');
    if (!videoIoEnabled) throw new ArtemisHostError('artemis EM_ASM 712081 is disabled while paused or disposed');
    const gl = requireWebgl();
    gl.texImage2D(
      gl.TEXTURE_2D,
      0,
      gl.RGBA,
      gl.RGBA,
      gl.UNSIGNED_BYTE,
      video as unknown as TexImageSource,
    );
    return 0;
  }

  /** `717384` — opens a URL, but only one this layer is willing to hand to the browser. */
  function openUrl(url: string): number {
    const trimmed = url.trim();
    // `javascript:` and `data:` are script execution by another name; the port refuses both.
    const scheme = /^([a-z][a-z0-9+.-]*):/i.exec(trimmed);
    const isHttp = scheme !== null && /^https?$/i.test(scheme[1] ?? '');
    const opened = view?.open;
    if (typeof opened !== 'function') {
      throw new ArtemisHostError('artemis EM_ASM 717384 needs a view that can open a window');
    }
    // Anything without a scheme is resolved against the page, then judged — never before: a
    // `javascript:` hidden inside a relative form must be caught after resolution, not trusted
    // because the raw text looked relative.
    const target = isHttp ? trimmed : new URL(trimmed, document.baseURI).href;
    const resolvedScheme = /^([a-z][a-z0-9+.-]*):/i.exec(target)?.[1] ?? '';
    if (!/^https?$/i.test(resolvedScheme)) {
      throw new ArtemisHostError(`artemis EM_ASM 717384 refused to open ${target.slice(0, 64)}`);
    }
    const handle = opened.call(view, target, '_blank', 'noopener,noreferrer') as { opener?: unknown } | null;
    if (handle === null || handle === undefined) {
      // A blocked popup is reported, not swallowed: silently returning 0 would look like success.
      report(`the browser blocked opening ${target}`);
      return 0;
    }
    try {
      // `noopener` is a window feature the browser may ignore on a returned handle.
      handle.opener = null;
    } catch {
      // Cross-origin handles refuse this assignment; the feature string is the real guarantee.
    }
    return 0;
  }

  /** `711012` — creates the fullscreen video and starts it from a guest-supplied path. */
  function startVideo(pathPointer: number): number {
    if (disposed || paused) {
      throw new ArtemisHostError('artemis EM_ASM 711012 arrived while the services are not running');
    }
    if (core === null) throw new ArtemisHostError('artemis EM_ASM 711012 arrived before the core was attached');
    const path = guestString(pathPointer);
    const bound = core;
    videoGeneration += 1;
    const generation = videoGeneration;
    teardownVideo();
    // `teardownVideo` bumped the generation; re-stamp so the handlers below belong to this start.
    videoGeneration = generation;

    const element = document.createElement('video');
    element.autoplay = true;
    element.setAttribute('playsinline', '');
    videoFlags.playing = false;
    videoFlags.timeupdate = false;
    videoFlags.ended = false;

    const handlers = {
      playing: () => {
        if (generation !== videoGeneration || video !== element) return;
        videoFlags.playing = true;
        // The shipped shell only reports a frame ready once both events have been seen.
        if (videoFlags.timeupdate) videoFlags.playing = true;
      },
      timeupdate: () => {
        if (generation !== videoGeneration || video !== element) return;
        videoFlags.timeupdate = true;
      },
      ended: () => {
        if (generation !== videoGeneration || video !== element) return;
        videoFlags.ended = true;
      },
    };
    element.addEventListener('playing', handlers.playing, true);
    element.addEventListener('timeupdate', handlers.timeupdate, true);
    element.addEventListener('ended', handlers.ended, true);
    videoHandlers = handlers as unknown as typeof videoHandlers;
    video = element;

    void (async () => {
      const bytes = await bound.assets.resolve(path, 'GET');
      if (generation !== videoGeneration || disposed) return;
      if (bytes === null) {
        report(`the asset seam has no video at ${path}`);
        return;
      }
      let url: string;
      try {
        url = URL.createObjectURL(new Blob([bytes as unknown as BlobPart]));
      } catch (error) {
        report('creating the video object URL failed', error);
        return;
      }
      if (generation !== videoGeneration || disposed) {
        URL.revokeObjectURL(url);
        return;
      }
      revokeOwnedUrl();
      videoObjectUrl = url;
      element.src = url;
      try {
        await element.play();
      } catch (error) {
        // Autoplay refusal is not fatal: the play promise rejects on most browsers before a
        // gesture. It is reported so a caller can retry from a user gesture rather than hang.
        report('the fullscreen video could not start; a user gesture may be required', error);
      }
    })().catch((error: unknown) => {
      // Attached in the same turn the promise is created, so a rejection is owned immediately —
      // including the one `report` itself raises when the host's logger throws. Without the `.catch`
      // attached here, `await flush()` hands that rejection to whatever the next macrotask is.
      report(`loading the fullscreen video from ${path} failed`, error);
    });

    return 0;
  }

  // ---------------------------------------------------------------------------------------------
  // The ASM runner
  // ---------------------------------------------------------------------------------------------

  const runAsmConst: ArtemisAsmConstRunner = (call) => {
    const { code, args } = call;
    switch (code) {
      case ARTEMIS_BROWSER_ASM_CODES.videoReset: {
        teardownVideo();
        return 0;
      }
      case ARTEMIS_BROWSER_ASM_CODES.videoCopyReady:
        return videoFlags.playing && videoFlags.timeupdate ? 1 : 0;
      case ARTEMIS_BROWSER_ASM_CODES.videoEnded:
        return videoFlags.ended ? 1 : 0;
      case ARTEMIS_BROWSER_ASM_CODES.videoWidth:
        return video?.videoWidth ?? 0;
      case ARTEMIS_BROWSER_ASM_CODES.videoHeight:
        return video?.videoHeight ?? 0;
      case ARTEMIS_BROWSER_ASM_CODES.videoOpen:
        return startVideo(args[0] ?? 0);
      case ARTEMIS_BROWSER_ASM_CODES.videoPause:
        if (video === null) throw new ArtemisHostError('artemis EM_ASM 712047 has no fullscreen video to pause');
        video.pause();
        return 0;
      case ARTEMIS_BROWSER_ASM_CODES.videoUpload:
        return uploadVideoFrame();
      case ARTEMIS_BROWSER_ASM_CODES.alert: {
        const alert = requireDialog('alert');
        alert.call(view, guestString(args[0] ?? 0));
        return 0;
      }
      case ARTEMIS_BROWSER_ASM_CODES.confirm: {
        const confirm = requireDialog('confirm');
        return confirm.call(view, guestString(args[0] ?? 0)) === true ? 1 : 0;
      }
      case ARTEMIS_BROWSER_ASM_CODES.mountLoad:
        return handleMount(guestString(args[0] ?? 0));
      case ARTEMIS_BROWSER_ASM_CODES.syncState:
        return syncing;
      case ARTEMIS_BROWSER_ASM_CODES.syncSave: {
        if (disposed || paused || core === null) {
          note(`save requested while ${disposed ? 'disposed' : paused ? 'paused' : 'unattached'}; not queueing`);
          return 0;
        }
        syncing += 1;
        // The snapshot is taken inside a queue unit — the guest has just written the save it wants
        // persisted, and only the queue can say when that write has entered. The task body is
        // synchronous on purpose: `queuePersist` captures and chains, it never re-enters the queue.
        core
          .enqueue(() => queuePersist())
          .catch((error: unknown) => report('saving the browser state failed', error))
          .finally(() => {
            syncing = Math.max(0, syncing - 1);
          });
        return 0;
      }
      case ARTEMIS_BROWSER_ASM_CODES.openUrl:
        return openUrl(guestString(args[0] ?? 0));
      default:
        // The shipped glue would have thrown `ASM_CONSTS[code].apply` on an unknown entry; a 0 here
        // would be a fabricated success for a code this port never implemented.
        throw new ArtemisHostError(`artemis EM_ASM code ${code} is not implemented by the browser services`);
    }
  };

  // ---------------------------------------------------------------------------------------------
  // Lifecycle
  // ---------------------------------------------------------------------------------------------

  // Not `async`: an `async` function boxes its result in a fresh promise on every call, so a second
  // `attach` would hand back a different promise than the first and the memoization would be a lie.
  // Refusals after the first call are returned as rejected promises for the same reason.
  function attach(bound: ArtemisSessionCore): Promise<void> {
    if (attaching !== null) return attaching;
    if (disposed) return Promise.reject(new ArtemisHostError('artemis browser services are disposed'));
    attaching = (async () => {
      core = bound;
      savePath = bound.assets.savePath;
      saveMountPath = assertArtemisMountPoint(savePath);
      // `mountArtemisTree` places the save root inside the session core's own initialised tree;
      // hydration happens BEFORE boot so the guest's first `open` already sees its saved data.
      try {
        saveRoot = bound.fs.mkdirp(saveMountPath);
      } catch (error) {
        // A mounted, read-only save path is a configuration error the caller must see.
        if (error instanceof Error && 'errno' in error && (error as { errno: number }).errno === ARTEMIS_ERRNO.EROFS) {
          throw new ArtemisHostError(`the save path ${saveMountPath} is mounted read-only`);
        }
        throw error;
      }
      bound.fs.makeWritable(saveMountPath);
      syncing += 1;
      try {
        await bound.enqueue(async () => {
          if (disposed) throw new ArtemisHostError('artemis browser services were disposed during attach');
          const state = await readPersisted();
          applySnapshotToFileSystem(bound.fs, saveMountPath, state.files);
          idb.mirrorReplace(state.idb);
          note(`attached with ${state.files.length} file(s) and ${state.idb.length} idb record(s)`);
        });
      } catch (error) {
        // A malformed or unreadable store must not be swallowed: booting on top of it would let the
        // guest overwrite a save the player can still be told about.
        report('loading the saved browser state failed', error);
        throw error;
      } finally {
        syncing = Math.max(0, syncing - 1);
      }
      saveRoot = bound.fs.lookup(saveMountPath) ?? saveRoot;
      attachedHydrated = true;
    })();
    return attaching;
  }

  return {
    idb,
    runAsmConst,
    attach,
    async flush(): Promise<void> {
      if (disposed) throw new ArtemisHostError('artemis browser services are disposed');
      // Await the tail first: a write queued by `716748` must be durable before this one starts.
      await pendingWrite;
      if (core === null || savePath === '') throw new ArtemisHostError('nothing has been attached to flush');
      syncing += 1;
      try {
        // Captured inside the queue unit, but never queued *inside* `core.enqueue`: this runs after
        // the guest's own unit has entered, and `queuePersist` only chains the store write.
        await core.enqueue(() => queuePersist());
        await pendingWrite;
      } finally {
        syncing = Math.max(0, syncing - 1);
      }
    },
    async snapshot(): Promise<ArtemisBrowserSnapshot> {
      if (core === null) throw new ArtemisHostError('nothing has been attached to snapshot');
      // Clone on the guest's queue too: the tree is live and a dump taken mid-write would be torn.
      return core.enqueue(() => ({
        version: ARTEMIS_BROWSER_SNAPSHOT_VERSION,
        files: snapshotSaveRoot(),
        idb: idb.mirrorEntries(),
      }));
    },
    async restore(next: ArtemisBrowserSnapshot): Promise<void> {
      if (disposed) throw new ArtemisHostError('artemis browser services are disposed');
      if (core === null || savePath === '') throw new ArtemisHostError('nothing has been attached to restore into');
      if (next.version !== ARTEMIS_BROWSER_SNAPSHOT_VERSION) {
        throw new ArtemisHostError(`artemis snapshot version ${String(next.version)} is not supported`);
      }
      // Validate every path and clone every buffer before anything is applied, so a bad snapshot
      // cannot leave the save root half-replaced.
      const files = next.files.map(([path, bytes]): ArtemisBrowserSnapshotFile => {
        return [assertArtemisSaveRelative(path), bytes.slice()];
      });
      const records = next.idb.map(([db, id, bytes]): ArtemisBrowserSnapshotIdb => [db, id, bytes.slice()]);
      const bound = core;
      const root = saveMountPath;
      await bound.enqueue(() => {
        if (disposed) throw new ArtemisHostError('artemis browser services were disposed during restore');
        applySnapshotToFileSystem(bound.fs, root, files);
        idb.mirrorReplace(records);
      });
    },
    pause(): void {
      paused = true;
      videoIoEnabled = false;
      // Playback stops, but the loaded element is kept: its object URL, listeners and position are
      // what `resume` needs to put the same frame back on screen.
      suspendPlayback();
    },
    async resume(): Promise<void> {
      if (disposed) throw new ArtemisHostError('artemis browser services are disposed');
      paused = false;
      videoIoEnabled = true;
      // Only the in-flight store write is awaited; the state is already hydrated in memory, so a
      // second read would be a second source of truth for the same save.
      await pendingWrite;
      const element = video;
      if (element !== null && videoFlags.playing) {
        try {
          // The generation is deliberately preserved across pause/resume; only a reset, a dispose or
          // a new `711012` invalidates it. The rejection is caught so a refused autoplay is reported
          // rather than becoming an unhandled rejection from a lifecycle call.
          await element.play();
        } catch (error) {
          report('the fullscreen video could not resume; a user gesture may be required', error);
        }
      }
    },
    async dispose(): Promise<void> {
      if (disposed) return;
      disposed = true;
      paused = true;
      videoIoEnabled = false;
      // Stop video first: a frame loop must not be uploading while the tree is being torn down.
      teardownVideo();
      hydrating = null;
      // Let any store write already started finish; the core is not ours to dispose.
      try {
        await pendingWrite;
      } catch (error) {
        report('an in-flight save failed during dispose', error);
      }
      note('browser services disposed');
    },
  };
}

/** Re-exported so a caller can report a failure without importing the errno module directly. */
export { ArtemisHostError as ArtemisBrowserHostError };
