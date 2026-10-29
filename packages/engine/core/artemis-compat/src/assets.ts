/**
 * The asyncify asset seam: turning a game's `.pfs` chain into the loose-file view the
 * official Artemis WebAssembly build fetches over HTTP.
 *
 * ## Why this exists
 *
 * `pack_file.txt` caps one `.pfs` at 2 GB, so a real title ships as `root.pfs` (2.69 GB)
 * plus continuations such as `root.pfs.000` (3.36 GB) — several GB in total — while the
 * pack the engine loads synchronously at boot is a `pf6` holding `system.ini` and
 * nothing else (`official-web-deploy.py`). Everything after that is a **loose file**,
 * fetched through the seam the SDK readme describes:
 *
 * - `[WASM] SAVEPATH` — the name the file cache is filed under;
 * - `[WASM] ASYNCIFY_URL` — the directory prefix prepended to every requested path;
 * - `[WASM] ASYNCIFY_HASH_LIST_URL` — an optional hash list that costs a full pass over
 *   the deployed tree, because it hashes every payload.
 *
 * This module produces the first two, the patched configuration, the tiny bootstrap pack,
 * and an in-process resolver that answers exactly the paths the engine would have fetched
 * from `ASYNCIFY_URL`.
 *
 * ## What it must not do
 *
 * A real deployment is **29,224 files and ~13 GB**. Three consequences, each a measured
 * constraint rather than a style preference:
 *
 * - **Never read an archive whole.** `readPfsTableFrom` touches `11 + index_size` bytes —
 *   1.13 MB, 0.042% of a 2.69 GB pack — where `readFile` on that pack does not even fit
 *   Node's `readFileSync` (`ERR_FS_FILE_TOO_LARGE`, > 2 GiB).
 * - **Never enumerate the loose tree.** The 29,224 names are the *extracted* tree; listing
 *   it is the thing this seam replaces. Only the game root is listed, and only for the
 *   pack chain.
 * - **Never preload payloads.** `resolve` reads one entry on demand; nothing else is
 *   fetched until it is asked for.
 *
 * ## Indexing, layering and identity
 *
 * Archives are indexed **by table only** (header plus index, never payloads) in a
 * deliberate order:
 *
 * 1. `root.pfs` — indexed first, so a later archive of the same name wins;
 * 2. its numbered continuations `root.pfs.000`, `root.pfs.001`, … in numeric order — a
 *   continuation is *not* a patch, and `extract-pfs.mjs` is explicit that a real game's
 *   images live in the continuation while the base holds scripts, sounds and fonts;
 * 3. every other `*.pfs` / `*.pfs.NNN` in the root listing, in lexicographic order of
 *   their lowercased names, so two runs over the same directory agree.
 *
 * Its `pf8c` header carries the **complete name table** (every entry, its 64-bit length
 * and its CRC32) with absolute payload offsets, so one 32-byte header read answers "does
 * this archive have the path?" — which is what makes 3.4 GB of images lookable-up without
 * reading an index or a payload. The payload themselves are addressed by `[offset, length]`
 * and read by range from `root.pfs`, the chain would be walked, and the path's payload was
 * the one thing worth counting.
 *
 * Two archives can hold the same absolute offset — the game's own packs are split, and a
 * `pf8c` written beside them restarts its offsets — so an archive claims a range only when
 * its own name matches the payload's own name, and only when that name is not a `.pfs`.
 * The check is a path compare, not a byte read: the worst it costs is one `exists` probe.
 *
 * Entries are keyed case-insensitively for lookup but remembered with their own spelling,
 * so a resolved path round-trips the archive's name. Inside one archive the **first** record
 * for a path wins (matching `findPfsEntry`); across archives the **last archive indexed**
 * wins (matching `extract-pfs.mjs`'s "later entries overwrite earlier ones"). Because that is
 * the opposite of `findPfsEntry`'s across-archive rule, the order above is what encodes
 * "later patches override": `root.pfs` first, patches last.
 *
 * ## The tiny bootstrap pack
 *
 * `rootPfs` is the `pf6` the synchronous bootstrap loads: a UTF-8-named `system.ini` and a
 * mandatory **directory trailer** that `pfs.ts` deliberately stops short of — see
 * {@link writeTinyRootPfs}.
 *
 * ## Request validation
 *
 * `resolve` is the only place a caller-supplied string becomes a fetch, so it is the only
 * place that has to be strict: absolute paths, `..`, NUL, `%`-escapes that are not escapes,
 * a non-GET method, and anything that would leave the configured origin are rejected before
 * a byte is read. `new URL` does the scheme/host work rather than a browser global, so this
 * module loads in Node as well as in a page.
 *
 * ## Encoding
 *
 * Only a **UTF-8** `system.ini` — with or without a BOM — is patched. The patch re-encodes with
 * `TextEncoder`, which emits UTF-8 and nothing else, so a Shift_JIS configuration used to come
 * back as different bytes and every non-ASCII path in it broke. It is refused instead, by name,
 * rather than silently rewritten; the newline convention and the BOM are preserved for the files
 * that are accepted, and `CHARSET` is never touched either way.
 */

import type { GameSource } from '@online-gal/engine-api';
import { sha1 } from './sha1.js';
import {
  findPfsEntry,
  readPfsEntryFrom,
  readPfsTableFrom,
  type PfsArchive,
  type PfsByteSource,
} from './pfs.js';

/** Options for {@link createArtemisAssets}. */
export interface ArtemisAssetsOptions {
  /**
   * Loose-file directory the engine is told to read, **with a trailing slash**.
   *
   * Written verbatim as `[WASM] SAVEPATH`, apart from the ASCII normalisation described on
   * {@link deriveSavePath}. The SDK readme's requirement — unique per game *per origin* —
   * holds naturally when this is a path like `assets/<title>/`: the logical directory name
   * is what the cache keys on, so a `SAVEPATH` of `x` and one of `y/x` do not share a slot.
   */
  readonly savePath?: string;
  /**
   * `[WASM] ASYNCIFY_URL`, default `'artemis-assets/'`.
   *
   * Absolute when it carries a scheme, otherwise a **virtual prefix** — see
   * {@link resolveAsyncifyUrl}. The engine appends the game path to it directly, so the
   * trailing slash is mandatory and is added when missing.
   */
  readonly baseUrl?: string;
  /** Byte budget for resolved payloads. Default 32 MiB. `0` disables caching. */
  readonly cacheBytes?: number;
}

/** The virtual prefix `[WASM] ASYNCIFY_URL` defaults to. */
export const DEFAULT_ASYNCIFY_URL = 'artemis-assets/';

/** Default payload cache budget: 32 MiB. */
export const DEFAULT_ARTEMIS_ASSET_CACHE_BYTES = 32 * 1024 * 1024;

/** Config key the engine case-folds its mount directory out of. */
const LOGICAL_MOUNT_KEY = 'SAVEPATH';

/**
 * `[WASM]` keys this module owns, written with the deployer's own spelling.
 *
 * The names are the ones `patch_system_ini` writes and the ones the asyncify string table
 * carries, so a patched file compares line-for-line with a deployed one.
 */
const MANAGED_KEYS = ['SAVEPATH', 'ASYNCIFY_URL', 'ASYNCIFY_HASH_LIST_URL'] as const;

/**
 * Managed keys matched **case-insensitively**.
 *
 * All three, because the engine case-folds the keys it reads: a `savepath` or `asyncify_url`
 * left in another case is still a live pointer, so matching strictly would leave the patched
 * file carrying two mount directories — the stale one the engine may read instead of the
 * written one — and a differently-cased hash list URL nobody regenerated. Native builds ship
 * exactly this spelling mix, so this is the normal case rather than a hostile one.
 *
 * The *written* keys still use {@link MANAGED_KEYS}' own spelling, so the patched file lines up
 * with the deployer's. Case-folding is for removal only.
 */
const CASE_FOLDED_MANAGED_KEYS: readonly string[] = MANAGED_KEYS;

/** Marker that starts the section the engine reads. */
const SECTION = '[WASM]';

/** The native file section, where a build without `[WASM] SAVEPATH` keeps its own. */
const FILE_SECTION = '[FILE]';

/** Seed used when the original configuration has no `SAVEPATH` to derive from. */
const EMPTY_SAVEPATH_SEED = 'artemis';

/** Raised when neither a loose `system.ini` nor an indexed archive carries one. */
export class MissingSystemIniError extends Error {
  override readonly name = 'MissingSystemIniError';
}

/**
 * What was decided while patching `system.ini`.
 *
 * `savePath` and `asyncifyUrl` are the values actually written (derived ones included), and
 * `logicalMount` is the directory name the engine will file the file cache under.
 */
export interface ArtemisConfigPatch {
  readonly savePath: string;
  readonly asyncifyUrl: string;
  readonly logicalMount: string;
  /** True when the source `system.ini` carried a UTF-8 BOM. */
  readonly bom: boolean;
  /** Patched text, in the source's own encoding, newline convention preserved. */
  readonly text: string;
  /** {@link text} as bytes, in the encoding it was read in. */
  readonly bytes: Uint8Array;
}

/** The contract the browser asyncify session consumes. */
export interface ArtemisAssets {
  /** Patched configuration, byte-exact as fed to {@link patchSystemIni}. */
  readonly config: Uint8Array;
  /** `pf6` package holding `system.ini` and nothing else, trailer included. */
  readonly rootPfs: Uint8Array;
  /** `[WASM] SAVEPATH` the engine will be told; a persistence/diagnostics key. */
  readonly savePath: string;
  /** Resolves one engine-requested path, or null when the game does not have it. */
  resolve(url: string, method?: string, body?: Uint8Array): Promise<Uint8Array | null>;
  /** Drops the cache and refuses further reads. Idempotent. */
  dispose(): void;
}

/** How a `system.ini` byte string was decoded. */
type IniEncoding = 'utf-8' | 'utf-8-bom';

/** An archive that has been looked up in, never opened. */
interface IndexedArchive {
  readonly name: string;
  readonly source: PfsByteSource;
  readonly archive: PfsArchive;
}

/** A `(size, readRange)` view over one game file. */
type ByteSource = PfsByteSource;

/**
 * Bounds that keep a malformed archive from being mistaken for an absent one.
 *
 * `readPfsTableFrom` covers the header and the index under `MAX_TABLE_BYTES`; the payload
 * entry it reads afterwards is bounded here too, because an entry that declares a length it
 * does not have is a misread rather than a big file.
 */
export const ARTEMIS_INDEX_LIMITS = {
  /** `pfs.ts`'s own index ceiling, restated so callers can assert against it. */
  tableBytes: 2048 * 1024,
  /** Largest `system.ini` claimed from an archive. */
  iniBytes: 1024 * 1024,
} as const;

/**
 * Reads a `system.ini` byte string, refusing any encoding this module cannot write back.
 *
 * Only UTF-8 — with or without a BOM — is accepted, and a refusal is the point rather than a
 * limitation. `TextEncoder` emits UTF-8 and nothing else, so a `system.ini` that arrived as
 * Shift_JIS used to be decoded from Shift_JIS and re-encoded as UTF-8: the script paths and
 * Japanese section names in it came back as different bytes, the engine converted against the
 * wrong table, and every non-ASCII resource 404'd with `共通-01が見つかりませんでした`. A
 * configuration that cannot be round-tripped is therefore rejected with a message that names the
 * encoding, so the caller can convert it deliberately instead of receiving silent corruption.
 *
 * `CHARSET` is deliberately *not* rewritten: it says what the engine converts script paths
 * *into*, and it is not a statement about this file's own encoding.
 */
function decodeIni(data: Uint8Array, where: string): { text: string; encoding: IniEncoding } {
  const bom = data.byteLength >= 3 && data[0] === 0xef && data[1] === 0xbb && data[2] === 0xbf;
  const body = bom ? data.subarray(3) : data;
  try {
    return {
      text: new TextDecoder('utf-8', { fatal: true }).decode(body),
      encoding: bom ? 'utf-8-bom' : 'utf-8',
    };
  } catch {
    // Deliberately not a fallback: see the doc comment above. The message names the file, says
    // what was found, and says what to do, because the caller's only correct move is to convert
    // the file rather than to have this module guess.
  }
  throw new Error(
    `${where}: not valid UTF-8${bom ? ' after its byte order mark' : ''}; refusing to rewrite it in a ` +
      'different encoding, because TextEncoder only emits UTF-8 and re-encoding a Shift_JIS ' +
      'configuration would silently corrupt every non-ASCII path in it. Convert the file to ' +
      'UTF-8 first, or leave CHARSET alone and patch it with a tool that preserves its encoding.',
  );
}

/** Encodes back in the encoding recorded on the way in: UTF-8, BOM only when it was there. */
function encodeIni(text: string, encoding: IniEncoding): Uint8Array {
  const body = new TextEncoder().encode(text);
  if (encoding !== 'utf-8-bom') return body;
  const out = new Uint8Array(body.byteLength + 3);
  out.set([0xef, 0xbb, 0xbf]);
  out.set(body, 3);
  return out;
}

/**
 * Key of a config line, folded to upper case.
 *
 * Folding is not cosmetic: the engine's own INI reader matches keys case-insensitively, so a
 * native `savepath` and a managed `SAVEPATH` are one key to it. Matching exactly here would let
 * the patched file carry both and leave the stale value live.
 */
function lineKey(line: string): string {
  const equals = line.indexOf('=');
  if (equals < 0) return '';
  return line.slice(0, equals).replace(/[\r\n]+$/, '').trim().toUpperCase();
}

/**
 * Finds the section a heading opens, or null when the line is not a heading.
 *
 * Returned upper-cased, because the engine matches section names case-insensitively and a
 * native build spells it `[file]` as readily as `[FILE]`.
 */
function sectionOf(line: string): string | null {
  const trimmed = line.replace(/^\uFEFF/, '').trim();
  return isSectionHeading(line) ? trimmed.toUpperCase() : null;
}

/** True when the line opens an INI section. */
function isSectionHeading(line: string): boolean {
  return line.replace(/^\uFEFF/, '').trimStart().startsWith('[');
}

/**
 * True when the line at `index` sits inside a `[WASM]` body rather than another section's.
 *
 * A heading belongs to the section it opens, and a body extends to the next heading — so a line
 * is in `[WASM]` when the nearest heading at or above it is one of `headings`. That is what lets
 * the `[FILE]` block, which also carries a `SAVEPATH`, go untouched.
 */
function insideWasm(lines: readonly string[], index: number, headings: readonly number[]): boolean {
  let nearest = -1;
  for (const heading of headings) if (heading <= index) nearest = heading;
  if (nearest < 0) return false;
  // Any heading between the nearest `[WASM]` and this line closes that block and opens another,
  // so the line belongs to whatever section that heading named.
  for (let at = nearest + 1; at <= index; at += 1) {
    if (sectionOf(lines[at] ?? '') !== null) return false;
  }
  return true;
}

/**
 * Reads `SAVEPATH` out of the original configuration.
 *
 * `[WASM] SAVEPATH` is preferred, because that is the key the seam itself writes. A build that
 * has never been web-deployed carries no `[WASM]` block at all and keeps its mount directory
 * under `[FILE]` — the native key — and reading only `[WASM]` made every such title derive from
 * an empty string and collide on one cache slot. So `[FILE]` is the fallback, and the two are
 * never mixed: they are different keys and the first one seen wins.
 *
 * Case-folded on both the key and the section name, matching the engine's own reader.
 */
function readOriginalSavePath(text: string, newline: string): string {
  const lines = text.split(newline);
  const wanted = [SECTION, FILE_SECTION];
  for (const heading of wanted) {
    let inside = false;
    for (const line of lines) {
      const section = sectionOf(line);
      if (section !== null) {
        inside = section === heading;
        continue;
      }
      if (!inside) continue;
      if (lineKey(line) !== LOGICAL_MOUNT_KEY) continue;
      const value = line.slice(line.indexOf('=') + 1).trim();
      if (value !== '') return value;
    }
  }
  return '';
}

/** 40 lowercase hex characters, matching what the engine's cache key accepts. */
function shortHash(input: string): string {
  return [...sha1(new TextEncoder().encode(input))]
    .map((byte) => byte.toString(16).padStart(2, '0'))
    .join('');
}

/**
 * Derives a cache key that is **stable ASCII**.
 *
 * The official deployment's `detect_savepath` is the reference: a Japanese `SAVEPATH`
 * (`まどそふと\ハミダシクリエイティブ`) leaves nothing once the non-lowercase-ASCII characters
 * are stripped, and collapsing every such title onto one key is precisely the collision the
 * uniqueness rule exists to prevent.
 *
 * - A **non-empty** `originalSavePath` keeps the deployer's behaviour exactly: the ASCII remnant
 *   when there is one, a hash of that value otherwise. Provided identity is honoured as given.
 * - An **empty** one means no identity was found — no `[WASM]` or `[FILE]` `SAVEPATH` anywhere —
 *   and hashing a constant seed would hand every such title the same cache slot, which is the
 *   collision this function exists to prevent. The seed is therefore built from `identity`: the
 *   full configuration text when the caller has it, so two different titles differ, plus the
 *   namespace and (only when it pins one) the origin.
 *
 * Read from the *original* configuration, not from the caller's remount prefix, so the key
 * survives a deployment being mounted elsewhere.
 */
export function deriveSavePath(
  originalSavePath: string,
  namespace: string,
  baseUrl: string,
  identity = '',
): string {
  if (originalSavePath === '') {
    // No identity in the file itself, so the caller's `identity` stands in for it. It is a
    // parameter rather than a constant so that two titles with no `SAVEPATH` — the case this
    // exists for — do not share one slot, and it falls back to the seed alone when a caller
    // has nothing to offer.
    const scope = identity === EMPTY_SAVEPATH_SEED ? EMPTY_SAVEPATH_SEED : seed(identity, EMPTY_SAVEPATH_SEED);
    return `artemis_${shortHash(seed(scope, namespace, scoped(baseUrl))).slice(0, 10)}`;
  }
  // The deployer strips everything outside `[a-z0-9_]` from the **whole** value, not from its
  // last segment: `My Game\Title` is one key, and splitting on the separator first would throw
  // away `My Game` and let two titles sharing a leaf name collide.
  const cleaned = originalSavePath.toLowerCase().replace(/[^a-z0-9_]/g, '').slice(0, 32);
  if (cleaned) return cleaned;
  return `artemis_${shortHash(seed(originalSavePath, namespace, scoped(baseUrl))).slice(0, 10)}`;
}

/** The origin, when the base URL pins one. A relative prefix cannot pin an origin. */
function scoped(baseUrl: string): string {
  return /^[a-z][a-z0-9+.-]*:/i.test(baseUrl) ? baseUrl : '';
}

/**
 * Builds the collision seed.
 *
 * Empty components are **dropped, not joined as empty fields**: the deployer's hash is taken
 * over the original value alone, and a separator that survives an absent namespace would give
 * a title deployed at the site root a different key from the same title deployed under a
 * prefix — the key would then depend on where it was mounted, which is exactly what the
 * "stable across remounts" rule forbids. A component's *presence* is what disambiguates.
 */
function seed(...parts: string[]): string {
  const present = parts.filter((part) => part !== '');
  const [first = '', ...rest] = present;
  return rest.reduce((acc, part) => `${acc}\u0000${part}`, first);
}

/**
 * Normalises `ASYNCIFY_URL`.
 *
 * Absolute when it carries a scheme, and then whatever origin is written is the origin the
 * engine fetches from — the deployer does the same with its own
 * `http://127.0.0.1:<port>/game/`. Anything else is a **virtual prefix**: the engine builds
 * `<baseUrl><game path>`, so `artemis-assets/` describes requests like
 * `artemis-assets/pc/ja/artemis.png` for a host that answers them from the packs. That is a
 * deliberate, self-describing namespace rather than a guess at an origin.
 *
 * A relative value keeps its own shape — `game` stays `game/` even at the site root —
 * because resolving it against a page URL cannot be done here, and inventing an absolute
 * origin would silently point the engine at the wrong host.
 */
function resolveAsyncifyUrl(baseUrl: string | undefined): string {
  const raw = (baseUrl ?? DEFAULT_ASYNCIFY_URL).trim();
  if (raw === '') return DEFAULT_ASYNCIFY_URL;
  if (raw.startsWith('//')) {
    throw new Error(
      `baseUrl ${raw} is protocol-relative: ASYNCIFY_URL must be absolute with a scheme, or a ` +
        'relative virtual prefix with no leading slashes',
    );
  }
  // Only a scheme **with** an authority is absolute. Requiring `://` keeps a bare `c:` from
  // being handed to `new URL`, which would call it a one-letter scheme rather than the drive
  // spelling it is, and the capture keeps the scheme group non-optional for the type checker.
  const scheme = /^([a-z][a-z0-9+.-]*):\/\//i.exec(raw);
  if (!scheme) return raw.endsWith('/') ? raw : `${raw}/`;
  // `new URL('https://cdn.example/game')` would keep the prefix's last segment as a file
  // name and lose it to the first appended game path, so a trailing slash is forced on
  // before the value is canonicalized.
  const wanted = new URL(raw.endsWith('/') ? raw : `${raw}/`);
  if (wanted.username !== '' || wanted.password !== '') {
    throw new Error(`baseUrl ${raw} carries credentials; ASYNCIFY_URL must not`);
  }
  if (wanted.search !== '' || wanted.hash !== '') {
    throw new Error(`baseUrl ${raw} carries a query or fragment; ASYNCIFY_URL is a directory`);
  }
  return wanted.href;
}

/**
 * Patches `system.ini`, in memory, exactly as `patch_system_ini` patches a file on disk.
 *
 * Properties kept from the Python original, each one a bug first:
 *
 * - **Managed keys are dropped unless this call sets them.** Skipping only the keys in
 *   `extra` leaves a stale `ASYNCIFY_HASH_LIST_URL` behind, pointing the engine at a hash
 *   list nobody regenerated. Case does not save such a line: the engine's reader folds keys,
 *   so a `savepath` or `asyncify_url` in another case is the same key and is removed too.
 * - **Exactly one `[WASM]` block survives.** A native `system.ini` can spell the section more
 *   than once — a publisher-written block near the top and a deployer-written one further down
 *   — and the engine reads *a* block, not the one this module edited. Every `[WASM]` body is
 *   therefore collected, in file order, into the first: the managed keys are dropped from all
 *   of them, the unmanaged lines keep their own text and their relative order, and the later
 *   blocks collapse to nothing. Collecting before emitting is what keeps a key that only the
 *   second block carried — `BOOT`, typically — under `[WASM]`, where the engine reads it,
 *   instead of leaving it under whatever section happened to stand between the two blocks.
 * - **Nothing else moves.** Unknown sections, unknown keys, comments, blank lines and the
 *   file's own newline convention survive; only managed lines change, and only in `[WASM]`
 *   itself.
 * - **`CHARSET` is never touched.** It is unmanaged, and rewriting it would change which
 *   encoding the engine converts script paths *into* — the failure recorded in `write_pf8`
 *   as `共通-01が見つかりませんでした` with only ASCII files still loading.
 * - **`ASYNCIFY_HASH_LIST_URL` is never generated.** Producing one means hashing every
 *   deployed payload; the key is removed and left to whoever has the bytes.
 *
 * `namespace` is the directory the engine is told to read, used only for
 * {@link deriveSavePath}'s collision domain.
 */
export function patchSystemIni(
  ini: Uint8Array | string,
  options: {
    savePath?: string;
    baseUrl?: string;
    namespace?: string | null;
    where?: string;
  } = {},
): ArtemisConfigPatch {
  const where = options.where ?? 'system.ini';
  const decoded =
    typeof ini === 'string' ? { text: ini, encoding: 'utf-8' as IniEncoding } : decodeIni(ini, where);
  const text = decoded.text;
  const newline = text.includes('\r\n') ? '\r\n' : '\n';
  const asyncifyUrl = resolveAsyncifyUrl(options.baseUrl);

  const explicit = options.savePath !== undefined;
  const namespace = options.namespace !== undefined ? options.namespace : explicit ? null : '';
  const original = readOriginalSavePath(text, newline);
  const savePath =
    options.savePath ?? deriveSavePath(original, namespace ?? '', asyncifyUrl, text);

  const managed = new Set<string>(CASE_FOLDED_MANAGED_KEYS);
  const lines = text.split(newline);
  const headings: number[] = [];
  for (let index = 0; index < lines.length; index += 1) {
    if (sectionOf(lines[index] ?? '') === SECTION) headings.push(index);
  }
  const written = [`${MANAGED_KEYS[0]} = ${savePath}`, `${MANAGED_KEYS[1]} = ${asyncifyUrl}`];

  if (headings.length === 0) {
    // No `[WASM]` block. The deployer refuses outright; a library has to stay usable on a
    // plain tree, so the section is appended after any trailing blanks — which is what keeps
    // a blank line separating two sections.
    let insert = lines.length;
    while (insert > 0 && (lines[insert - 1] ?? '').trim() === '') insert -= 1;
    lines.splice(insert, 0, ...(insert === 0 ? [] : ['']), SECTION, ...written);
  } else {
    // Two passes over the original lines, rebuilding the file.
    //
    // Pass one collects the body of *every* `[WASM]` block, in file order, with the managed
    // keys dropped and everything else kept verbatim. Pass two emits the surviving file: the
    // first `[WASM]` heading, the written keys, then that collected body, while every line that
    // is not part of a `[WASM]` block is copied at its own position and every later `[WASM]`
    // heading disappears.
    //
    // Collecting first is what keeps a second block's keys where the engine looks for them. A
    // single positional pass appended the tail of the file to the first block's body, so a
    // `[WASM]`-only key that sat after an intervening `[FILE]` — `BOOT`, typically — came out
    // under `[FILE]`, where the engine never reads it, and the title failed to boot.
    const at = headings[0]!;
    const body: string[] = [];
    for (const heading of headings) {
      const block: string[] = [];
      let index = heading + 1;
      for (; index < lines.length; index += 1) {
        const line = lines[index] ?? '';
        if (sectionOf(line) !== null) break; // the block ends at the next heading
        if (managed.has(lineKey(line))) continue;
        // A blank that only separated a dropped managed key from its neighbour is gone; a run
        // of blanks collapses to the one that separated the block's own content.
        if (line === '' && (block[block.length - 1] ?? '').trim() === '') continue;
        block.push(line);
      }
      // The blank that closed a block separated it from the heading behind it, and the tail
      // below re-emits exactly one there. A block that runs to the end of the file owns its
      // trailing blanks instead: they are the file's own final newline.
      if (index < lines.length) {
        while (block.length > 0 && (block[block.length - 1] ?? '').trim() === '') block.pop();
      }
      // Leading blanks go with the heading, which now carries the written keys in their place.
      while (block.length > 1 && (block[0] ?? '').trim() === '') block.shift();
      for (const line of block) {
        if (line === '' && (body[body.length - 1] ?? '').trim() === '') continue;
        body.push(line);
      }
    }

    const rebuilt: string[] = [];
    for (let index = 0; index < at; index += 1) rebuilt.push(lines[index] ?? '');
    rebuilt.push(SECTION, ...written);
    for (const line of body) rebuilt.push(line);
    let separator = false;
    for (let index = at + 1; index < lines.length; index += 1) {
      const line = lines[index] ?? '';
      // Every later `[WASM]` heading and body collapses: the body was folded into the first
      // block above, so only lines belonging to other sections are emitted here.
      if (sectionOf(line) === SECTION || insideWasm(lines, index, headings)) continue;
      if (line === '') {
        // A blank that only separated a collapsed block from what follows leaves with the
        // block; exactly one is put back, and only where the file already had one.
        let next = index + 1;
        while (next < lines.length && (lines[next] ?? '').trim() === '') next += 1;
        if (next < lines.length && sectionOf(lines[next] ?? '') === SECTION) {
          separator = true;
          continue;
        }
      }
      if (separator) {
        // The blank is re-emitted here, unless this line is itself the blank that carried it.
        if (line !== '' && (rebuilt[rebuilt.length - 1] ?? '').trim() !== '') rebuilt.push('');
        separator = false;
      }
      rebuilt.push(line);
    }
    lines.length = 0;
    lines.push(...rebuilt);
  }

  const patched = lines.join(newline);
  return {
    savePath,
    asyncifyUrl,
    logicalMount: savePath.toUpperCase(),
    bom: decoded.encoding === 'utf-8-bom',
    text: patched,
    bytes: encodeIni(patched, decoded.encoding),
  };
}

/** Reads a 32-bit little-endian field. */
function readU32(view: DataView, offset: number): number {
  return view.getUint32(offset, true);
}

/** Writes a 32-bit little-endian field. */
function putU32(out: number[], value: number): void {
  out.push(value & 0xff, (value >>> 8) & 0xff, (value >>> 16) & 0xff, (value >>> 24) & 0xff);
}

/**
 * Builds the `pf6` pack the synchronous bootstrap loads.
 *
 * The web build's pack contains `system.ini` and nothing else, because the readme is
 * explicit that everything else comes from the server. `pfs.ts` is the right reader for a
 * game's pack and the wrong writer for this one, because it stops where the official packer
 * does not:
 *
 * ```text
 * 0   "pf6"          3 bytes
 * 3   index_size     u32 LE, byte count of the index blob including its count
 * 7   index          index_size bytes
 *                    --- the blob's end, where both reference readers stop ---
 *     count + 1      u32 LE                trailer
 *     count x 8      u32 name_end, u32 0   trailer, one per record
 *     0, 0           u32, u32              trailer terminator
 *     cursor         u32 LE                trailer, index offset past the last record
 * ```
 *
 * `name_end` is the index offset just **past that record's own name**, not past the record;
 * the SDK packer's output is byte-identical to this formula. Without the trailer the
 * official engine aborts while opening, with
 * `Assertion failed: mlf_ >= minimum_max_load_factor, at: .../unordered/detail/implementation.hpp,3027,min_buckets_for_size`
 * followed by `babel.cpp:1513,translate` — which is why the trailer is mandatory here even
 * though both readers ignore it.
 *
 * Names are stored with `\` separators and sorted with a separator comparing below every
 * other byte, matching the packer, because the engine's lookup depends on that order.
 * Offsets are absolute, so the index is built once with placeholder offsets to learn its own
 * length, then once more with the real ones. The version digit is `6`: unkeyed, which is
 * what the SDK's own packer emits.
 */
function writeTinyRootPfs(files: ReadonlyArray<{ name: string; data: Uint8Array }>): Uint8Array {
  const encoded = files
    .map((file) => ({ raw: new TextEncoder().encode(file.name.replace(/\//g, '\\')), data: file.data }))
    .sort((a, b) => comparePackerOrder(a.raw, b.raw));

  const buildIndex = (offsets: readonly number[]): Uint8Array => {
    const out: number[] = [];
    putU32(out, encoded.length);
    const nameEnds: number[] = [];
    let cursor = 4;
    for (let index = 0; index < encoded.length; index += 1) {
      const raw = encoded[index]!.raw;
      putU32(out, raw.byteLength);
      for (const byte of raw) out.push(byte);
      cursor += 4 + raw.byteLength;
      nameEnds.push(cursor);
      putU32(out, 0); // the field both reference readers ignore
      putU32(out, offsets[index] ?? 0);
      putU32(out, encoded[index]!.data.byteLength);
      cursor += 12;
    }
    putU32(out, encoded.length + 1);
    for (const nameEnd of nameEnds) {
      putU32(out, nameEnd);
      putU32(out, 0);
    }
    putU32(out, 0);
    putU32(out, 0);
    putU32(out, cursor);
    return new Uint8Array(out);
  };

  // Two passes: the offsets are absolute, so the index has to be sized before any is known.
  const sized = buildIndex(encoded.map(() => 0));
  let cursor = 7 + sized.byteLength;
  const offsets = encoded.map((file) => {
    const at = cursor;
    cursor += file.data.byteLength;
    return at;
  });
  const index = buildIndex(offsets);

  const payloadBytes = encoded.reduce((sum, file) => sum + file.data.byteLength, 0);
  const out = new Uint8Array(7 + index.byteLength + payloadBytes);
  out[0] = 0x70;
  out[1] = 0x66;
  out[2] = 0x36; // "6": unkeyed, so payloads are stored as they are
  new DataView(out.buffer).setUint32(3, index.byteLength, true);
  out.set(index, 7);
  let at = 7 + index.byteLength;
  for (const file of encoded) {
    out.set(file.data, at);
    at += file.data.byteLength;
  }
  return out;
}

/** The packer's name order: a `\` or `/` compares below every other byte. */
function comparePackerOrder(a: Uint8Array, b: Uint8Array): number {
  const length = Math.min(a.byteLength, b.byteLength);
  for (let index = 0; index < length; index += 1) {
    const left = a[index] === 0x5c || a[index] === 0x2f ? 1 : a[index]!;
    const right = b[index] === 0x5c || b[index] === 0x2f ? 1 : b[index]!;
    if (left !== right) return left - right;
  }
  return a.byteLength - b.byteLength;
}

/** `root.pfs.000` continuation numbering. */
const CONTINUATION = /^root\.pfs\.(\d{1,9})$/i;

/** Any package, patches included. */
const PACKAGE = /\.pfs(\.\d{1,9})?$/i;

/**
 * Sort key: `root.pfs` first, its continuations next (numerically, so `.009` precedes
 * `.010`), then every other package by lowercased name.
 */
function compareArchives(a: string, b: string): number {
  const rank = (name: string): [number, bigint, string] => {
    const lower = name.toLowerCase();
    if (lower === 'root.pfs') return [0, 0n, lower];
    const continuation = CONTINUATION.exec(lower);
    if (continuation) return [1, BigInt(continuation[1]!), lower];
    return [2, 0n, lower];
  };
  const left = rank(a);
  const right = rank(b);
  if (left[0] !== right[0]) return left[0] - right[0];
  if (left[1] !== right[1]) return left[1] < right[1] ? -1 : 1;
  return left[2] < right[2] ? -1 : left[2] > right[2] ? 1 : 0;
}

/** `(size, readRange)` over one game file. */
function openGameFile(source: GameSource, path: string): ByteSource {
  // An archive is always read by range: `readPfsTableFrom` touches `11 + index_size` bytes of a
  // multi-GB pack. The whole-file fallback this used to carry read the *entire* archive — 3 GiB
  // in one `readFile` — which is the one thing this module documents it must never do. A source
  // that cannot serve a range cannot index an archive at all, so it is refused here, before a
  // byte is requested, rather than handed an unbounded read to perform.
  if (!source.capabilities.randomAccess || !source.readRange) {
    throw new Error(
      `cannot index ${path} in ${source.name}: reading a package needs ` +
        'capabilities.randomAccess and a readRange implementation, because an archive is read ' +
        'by offset and length and must never be read whole',
    );
  }
  return {
    size: () => source.size(path),
    readRange: (offset, length) => source.readRange!(path, offset, length),
  };
}

/** Decodes an entry name, tolerating the Shift_JIS names older archives carry. */
function decodeNameBytes(bytes: Uint8Array): string | null {
  for (const byte of bytes) {
    if (byte === 0) return null;
    if (byte < 0x09 || (byte > 0x0d && byte < 0x20)) return null;
  }
  try {
    return new TextDecoder('utf-8', { fatal: true }).decode(bytes);
  } catch {
    try {
      return new TextDecoder('shift_jis', { fatal: true }).decode(bytes);
    } catch {
      return null;
    }
  }
}

/** Indexes one archive by table only: the header, the index, and nothing else. */
async function indexArchive(source: GameSource, name: string): Promise<IndexedArchive | null> {
  const byteSource = openGameFile(source, name);
  const archive = await readPfsTableFrom(byteSource);
  if (!archive) return null;
  if (archive.tableLength > ARTEMIS_INDEX_LIMITS.tableBytes) return null;
  return { name, source: byteSource, archive };
}

/**
 * Reads one payload of a `system.ini` out of an archive, by range.
 *
 * The entry's declared length is checked against the archive before the read, so a record
 * that claims a length it cannot have is rejected here rather than fetched as garbage.
 */
async function readIndexed(
  source: GameSource,
  archive: IndexedArchive,
  path: string,
): Promise<Uint8Array | null> {
  const entry = findPfsEntry(archive.archive, path);
  if (!entry) return null;
  if (entry.length > ARTEMIS_INDEX_LIMITS.iniBytes) {
    throw new Error(
      `${archive.name}: ${entry.path} claims ${entry.length} bytes, above the ` +
        `${ARTEMIS_INDEX_LIMITS.iniBytes}-byte configuration limit`,
    );
  }
  return readPfsEntryFrom(archive.source, archive.archive, entry);
}

/**
 * Percent-decodes a path, and rejects malformed escapes rather than ignoring them.
 *
 * The escapes are UTF-8 octets — `%E5%A4%A9` is 天 — so the three ingredients are decoded
 * separately and only the escaped runs are passed through the decoder: text that arrived
 * literally is already a string, and re-encoding it as bytes would reject perfectly good
 * non-ASCII names like `天梨.png`.
 */
function decodeEscapes(raw: string, baseUrl: string): string {
  const clipped = raw.startsWith(baseUrl) ? raw.slice(baseUrl.length) : raw;
  // A NUL is refused whether it arrived literally or as `%00`: cutting a C string at it is the
  // engine's habit, not this module's, so the escape must not become a way around the check.
  if (clipped.includes('\u0000') || clipped.includes('%00')) {
    throw new Error(`${baseUrl}: ${clipped} contains a NUL byte`);
  }
  if (!clipped.includes('%')) return clipped;
  const out: string[] = [];
  let run: number[] = [];
  // A failed decode is a rejection rather than a lossy replacement, and the escapes are UTF-8
  // octets — `%E5%A4%A9` is 天 — so only the escaped runs reach the decoder.
  const flush = (): void => {
    if (run.length === 0) return;
    try {
      out.push(new TextDecoder('utf-8', { fatal: true }).decode(new Uint8Array(run)));
    } catch {
      throw new Error(`${baseUrl}: ${clipped} is not valid UTF-8 after percent-decoding`);
    }
    run = [];
  };
  for (let index = 0; index < clipped.length; index += 1) {
    const char = clipped[index]!;
    if (char !== '%') {
      flush();
      out.push(char);
      continue;
    }
    const hex = clipped.slice(index + 1, index + 3);
    if (!/^[0-9a-fA-F]{2}$/.test(hex)) {
      throw new Error(`${baseUrl}: malformed percent-escape at byte ${index} of ${clipped}`);
    }
    run.push(Number.parseInt(hex, 16));
    index += 2;
  }
  flush();
  return out.join('');
}

/**
 * Turns one request into a game path, or throws.
 *
 * Everything here is a rejection rather than a sanitisation. A path that escapes the asset
 * tree is not silently clipped to one that does not: the caller asked for something the
 * assets cannot answer, and the answer is "no" rather than some other file's bytes. That is
 * also why the raw text is judged *before* any URL normalization: `../secret.txt` must be
 * refused as an escape rather than quietly folded back inside the root by a dot-segment
 * resolver, and `/etc/passwd` must be refused as absolute rather than become a root-relative
 * request the root would have answered.
 *
 * A request is one of two shapes: the configured prefix followed by a game path, or a bare
 * game path. The prefix is never required, because callers hand over both.
 */
function parseRequest(url: string, method: string, baseUrl: string): string {
  const verb = (method ?? 'GET').toUpperCase();
  if (verb !== 'GET' && verb !== 'HEAD') {
    throw new Error(`resolve: ${method} is not a supported method; the asset seam only reads`);
  }
  const text = url ?? '';

  // The query and the fragment are not part of a path. They are cut here so that the checks
  // below read one kind of text, and so that an absolute spelling is tested on its own path.
  const cut = text.search(/[?#]/);
  const raw = cut === -1 ? text : text.slice(0, cut);

  // A drive is refused by its spelling before anything else looks at the text: the check runs
  // on the raw and on the decoded spelling, because `C%3A/x` is the same drive as `C:/x`. A
  // protocol-relative spelling is refused by name rather than as `//evil.example/...`, which
  // would read like an ordinary absolute path: it carries an origin, and no origin was consulted
  // to produce it, so there is nothing to compare it against.
  if (/^[a-zA-Z]:/.test(raw) || /^[a-zA-Z]:/.test(decodeEscapes(raw, baseUrl))) {
    throw new Error(`resolve: ${raw} names a drive, not a game path`);
  }
  if (raw.startsWith('//')) {
    throw new Error(`resolve: ${url} is protocol-relative and carries no origin to check`);
  }

  // The prefix is settled first, so the hostile spellings are judged on what is left after it:
  // `artemis-assets/../secret.png` is an escape, and `artemis-assets/nul\u0000.png` still carries
  // its NUL. Two spellings of one request are both recognized — `artemis-assets/x` as it arrives
  // from the engine, and `/artemis-assets/x` as a browser sends it. An encoded leading slash is
  // *not* one of them: `%2Fetc/passwd` names a path that begins with a slash and is refused below.
  let rest = raw;
  if (!isAbsolute(raw)) {
    const head = raw.startsWith('/') ? `/${baseUrl}` : baseUrl;
    if (raw.startsWith(head)) rest = raw.slice(head.length);
  } else {
    // A cross-origin request is refused here even though the caller chose the origin: this seam
    // answers from a game directory, and fetching elsewhere is not an answer it has.
    if (!isAbsolute(baseUrl)) {
      throw new Error(
        `resolve: ${url} is absolute but ASYNCIFY_URL ${baseUrl} is a relative prefix, so the ` +
          'request cannot be attributed to this asset root',
      );
    }
    const configured = new URL(baseUrl);
    const wanted = new URL(raw);
    // Credentials are refused rather than ignored: the origin check below passes anyway, so a
    // request carrying them would otherwise be answered with the asset root's bytes. The path to
    // compare is the one spelled in the raw text, reached by skipping its `//authority` —
    // `configured.pathname` is a path, so `https://host/game/x` would read as starting with `h`
    // and never match — and the raw spelling is used rather than `wanted.pathname`, because the
    // URL constructor has already folded any `../` away and an escape has to be refused rather
    // than become the path it folded down to. The prefix is a directory, so `/game` and `/game/`
    // are one request and a sibling such as `/other/...` is outside the root rather than in it.
    if (wanted.username !== '' || wanted.password !== '') {
      throw new Error(`resolve: ${url} carries credentials; the asset root is read anonymously`);
    }
    if (wanted.origin !== configured.origin) {
      throw new Error(`resolve: ${wanted.origin} is not the configured origin ${configured.origin}`);
    }
    const pathAt = raw.indexOf('/', raw.indexOf('//') + 2);
    const rawPath = pathAt === -1 ? '/' : raw.slice(pathAt);
    const prefix = configured.pathname.endsWith('/') ? configured.pathname : `${configured.pathname}/`;
    if (!rawPath.startsWith(prefix)) {
      throw new Error(
        `resolve: ${raw} is not under the configured directory ${prefix} on ${configured.origin}`,
      );
    }
    rest = rawPath.slice(prefix.length);
  }

  const path = decodeEscapes(rest, baseUrl).replace(/\\/g, '/');
  if (path.includes('\u0000')) throw new Error('resolve: path contains a NUL byte');
  if (path.startsWith('/')) {
    throw new Error(`resolve: absolute path ${path} is outside the asset root`);
  }
  const parts = path.split('/').filter((part) => part !== '' && part !== '.');
  if (parts.length === 0) throw new Error(`resolve: empty path`);
  // `..` is judged after the separators are unified, so `..\secret.png` is the escape its `/`
  // spelling is, and after the escapes are decoded, so `%2E%2E` is too.
  if (parts.some((part) => part === '..')) {
    throw new Error(`resolve: ${path} escapes the asset root`);
  }
  return parts.join('/');
}

/** A string carrying a scheme is absolute; the asset root is only ever named by a relative one. */
function isAbsolute(text: string): boolean {
  return /^[a-z][a-z0-9+.-]*:/i.test(text);
}

export { LOGICAL_MOUNT_KEY, MANAGED_KEYS, SECTION };

/**
 * Builds the asset seam for one game directory.
 *
 * Reads a loose `system.ini` when there is one — one probe, one file, nothing else — and
 * otherwise one root listing and, per archive, a header plus an index plus the single
 * `system.ini` payload. It never reads an archive whole, never enumerates the loose tree,
 * and never touches a payload it was not asked for.
 */
export async function createArtemisAssets(
  source: GameSource,
  options: ArtemisAssetsOptions = {},
): Promise<ArtemisAssets> {
  const baseUrl = resolveAsyncifyUrl(options.baseUrl);
  const cacheLimit =
    options.cacheBytes === undefined
      ? DEFAULT_ARTEMIS_ASSET_CACHE_BYTES
      : Math.max(0, Math.floor(options.cacheBytes));

  // One root listing, shared by the configuration search and the lazy index. The loose tree
  // is 29,224 files in a real deployment and is never walked; `list('')` is one level.
  let listing: Promise<readonly string[]> | null = null;
  const rootEntries = () => (listing ??= source.list(''));

  let discovered: IndexedArchive[] | null = null;
  let discovery: Promise<IndexedArchive[]> | null = null;
  const archives = async (): Promise<IndexedArchive[]> => {
    if (discovered) return discovered;
    // The listing can change under a long-running session — that is the point of the lazy
    // build — so a failed discovery is retried rather than cached as an empty one.
    discovery ??= (async () => {
      const names = (await rootEntries())
        .filter((entry) => !entry.endsWith('/') && PACKAGE.test(entry))
        .sort(compareArchives);
      const found: IndexedArchive[] = [];
      for (const name of names) {
        const archive = await indexArchive(source, name);
        if (archive) found.push(archive);
      }
      return found;
    })();
    try {
      discovered = await discovery;
      return discovered;
    } catch (error) {
      discovery = null;
      throw error;
    }
  };

  // A loose `system.ini` is authoritative, and costs one probe and (when present) one read.
  let iniBytes: Uint8Array | null = null;
  if (await source.exists('system.ini')) iniBytes = await source.readFile('system.ini');
  if (!iniBytes) {
    const found = await archives();
    // Later patches override, so the search runs from the last-indexed archive backwards;
    // `root.pfs` is first-indexed, which is what makes it the base.
    for (let index = found.length - 1; index >= 0 && !iniBytes; index -= 1) {
      const archive = found[index]!;
      if (!findPfsEntry(archive.archive, 'system.ini')) continue;
      iniBytes = await readIndexed(source, archive, 'system.ini');
    }
    if (!iniBytes) {
      // A typed error, because "there is no configuration anywhere" is a fact about the game
      // rather than a failure of this module, and a caller should not have to read a message
      // to tell those apart. It — and every I/O error the probes above raise — rejects
      // construction: manufacturing an empty `system.ini` here would write a patched
      // configuration that no game ever shipped, and would hide the backend failure behind a
      // seam that looks healthy until the engine asks for a file.
      throw new MissingSystemIniError(
        `no system.ini in ${source.name}: not loose at the root, and none of the ` +
          `${found.length} readable package${found.length === 1 ? '' : 's'} carries one`,
      );
    }
  }

  // `where` names the game rather than the bare filename, because "not valid UTF-8" is only
  // actionable when the message says which tree the file came out of.
  const patched = patchSystemIni(iniBytes, {
    savePath: options.savePath,
    baseUrl,
    where: `system.ini in ${source.name}`,
  });

  // --- payload cache ---
  const cache = new Map<string, Uint8Array>();
  let cachedBytes = 0;
  let disposed = false;

  const evictOldest = (): boolean => {
    const oldest = cache.keys().next();
    if (oldest.done) return false;
    const held = cache.get(oldest.value)!;
    cachedBytes -= held.byteLength;
    cache.delete(oldest.value);
    return true;
  };

  const remember = (id: string, bytes: Uint8Array): void => {
    if (disposed || cacheLimit <= 0 || bytes.byteLength > cacheLimit) return;
    while (cachedBytes + bytes.byteLength > cacheLimit && evictOldest()) {
      // keep evicting until it fits
    }
    cache.set(id, bytes);
    cachedBytes += bytes.byteLength;
  };

  const inflight = new Map<string, Promise<Uint8Array | null>>();

  const readOnce = async (id: string, path: string): Promise<Uint8Array | null> => {
    const existing = inflight.get(id);
    if (existing) return existing;
    const pending = (async () => {
      // A loose file wins, and is answered by one probe plus one read — never by the archives
      // and never by a listing, so a request the loose tree can answer costs no discovery.
      if (await source.exists(path)) {
        const bytes = await source.readFile(path);
        remember(id, bytes);
        return bytes.slice();
      }
      const found = await archives();
      for (let index = found.length - 1; index >= 0; index -= 1) {
        const archive = found[index]!;
        const entry = findPfsEntry(archive.archive, path);
        if (!entry) continue;
        const bytes = await readPfsEntryFrom(archive.source, archive.archive, entry);
        if (!bytes) return null;
        remember(id, bytes);
        // A clone, so a caller that mutates what it received cannot corrupt the cache.
        return bytes.slice();
      }
      return null;
    })();
    inflight.set(id, pending);
    try {
      return await pending;
    } finally {
      inflight.delete(id);
    }
  };

  const resolve = async (url: string, method = 'GET', _body?: Uint8Array): Promise<Uint8Array | null> => {
    if (disposed) throw new Error('artemis assets were disposed');
    const path = parseRequest(url, method, baseUrl);
    const id = path.toLowerCase();

    const held = cache.get(id);
    if (held) {
      // Re-inserted so the LRU sees this as the most recently used entry.
      cache.delete(id);
      cache.set(id, held);
      return held.slice();
    }

    const bytes = await readOnce(id, path);
    // Rechecked after the await: a disposal during a pending read must not hand back bytes the
    // caller was told it would stop being served.
    if (disposed) throw new Error('artemis assets were disposed');
    return bytes === null ? null : bytes.slice();
  };

  return {
    config: patched.bytes,
    rootPfs: writeTinyRootPfs([{ name: 'system.ini', data: patched.bytes }]),
    savePath: patched.savePath,
    resolve,
    dispose(): void {
      disposed = true;
      cache.clear();
      cachedBytes = 0;
      inflight.clear();
    },
  };
}
