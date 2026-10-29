/**
 * ONScripter save-file conventions.
 *
 * Ported from AetherKiri's `bridge/onscripter_runtime/src/onscripter_save_storage.cpp`.
 *
 * NScripter does not use numbered save slots in the abstract: it writes real
 * files into a `savedata` directory, and a handful of them carry whole-game
 * state rather than a single slot. Knowing exactly which filenames count as
 * saves is what makes it possible to migrate, back up or clear a game's
 * progress without guessing.
 *
 * The native implementation additionally prefers `savedata` *beside the game*
 * when the media is writable. A browser import is read-only by design — the
 * user's folder is never modified — so here the writable overlay always plays
 * the role of the fallback directory, which is the same effective outcome for
 * any read-only source.
 */

/** Files that hold game state rather than a numbered slot. */
const FIXED_SAVE_FILES: readonly string[] = [
  // Global flags and per-playthrough variables.
  'envdata',
  'gloval.sav',
  // Read-text ("kidoku") log, under its two historical spellings.
  'kidoku.dat',
  'NScrllog.dat',
  'NScrflog.dat',
  // The engine's redirected standard streams, kept so logs travel with saves.
  'stdout.txt',
  'stderr.txt',
];

const SAVE_PREFIX = 'save';
const SAVE_SUFFIX = '.dat';

/**
 * True when a filename is an ONScripter save.
 *
 * Matches the fixed-state files plus `save<digits>.dat`. The digit check is
 * what separates a real slot (`save01.dat`) from an unrelated asset that merely
 * happens to start with `save` (`savepoint.png`).
 */
export function isOnsSaveFileName(path: string): boolean {
  const name = path.slice(path.lastIndexOf('/') + 1);
  if (FIXED_SAVE_FILES.includes(name)) return true;

  if (
    name.length <= SAVE_PREFIX.length + SAVE_SUFFIX.length ||
    !name.startsWith(SAVE_PREFIX) ||
    !name.endsWith(SAVE_SUFFIX)
  ) {
    return false;
  }
  const digits = name.slice(SAVE_PREFIX.length, name.length - SAVE_SUFFIX.length);
  if (digits.length === 0) return false;
  for (const char of digits) {
    if (char < '0' || char > '9') return false;
  }
  return true;
}

/** Directory ONScripter reads and writes its saves from. */
export const ONS_SAVE_DIRECTORY = 'savedata';

/** The save path for a numbered slot, e.g. slot 3 → `savedata/save03.dat`. */
export function onsSavePath(slot: number): string {
  const padded = String(Math.max(0, Math.trunc(slot))).padStart(2, '0');
  return `${ONS_SAVE_DIRECTORY}/save${padded}.dat`;
}

/**
 * True when a path is a save file in one of the two places ONScripter uses.
 *
 * A save is either directly in the game root or directly inside `savedata`;
 * anything deeper is a packaged asset that merely has a save-like name. The
 * constraint matters during migration, where an unconstrained match would treat
 * a file just written into `savedata/` as a fresh source and copy it forever.
 */
export function isOnsSavePath(path: string): boolean {
  const normalised = path.replace(/^\.?\//, '');
  if (!normalised) return false;

  const slash = normalised.lastIndexOf('/');
  if (slash === -1) {
    // Directly in the game root.
    return isOnsSaveFileName(normalised);
  }

  const directory = normalised.slice(0, slash);
  const name = normalised.slice(slash + 1);
  // Directly inside `savedata`, and nowhere deeper: a save is never nested, so
  // `savedata/old/save01.dat` is a packaged asset that only looks like one.
  return directory === ONS_SAVE_DIRECTORY && isOnsSaveFileName(name);
}

/** Every path under `savedata` and the root that counts as save state. */
export function findOnsSavePaths(paths: readonly string[]): readonly string[] {
  return paths.filter((path) => isOnsSavePath(path));
}

export interface SaveMigrationResult {
  readonly copied: number;
  /** Files skipped because the destination already held them. */
  readonly skipped: number;
  readonly warnings: readonly string[];
}

/** Minimal store surface migration needs, matching `WritableStore`. */
export interface MigratableStore {
  read(path: string): Promise<Uint8Array | undefined>;
  write(path: string, data: Uint8Array): Promise<void>;
  list(prefix?: string): Promise<readonly string[]>;
}

/**
 * Copies save files from one prefix to another without overwriting.
 *
 * Mirrors AetherKiri's `MigrateSaveFiles`: a newer save already sitting at the
 * destination always wins, because an upgrade must never roll a player's
 * progress backwards. Failures are collected rather than thrown so a partial
 * migration still leaves the game playable.
 */
export async function migrateOnsSaves(
  store: MigratableStore,
  fromPrefix: string,
  toPrefix: string,
): Promise<SaveMigrationResult> {
  const warnings: string[] = [];
  let copied = 0;
  let skipped = 0;

  const normalise = (prefix: string): string =>
    prefix.endsWith('/') ? prefix.slice(0, -1) : prefix;
  const source = normalise(fromPrefix);
  const destination = normalise(toPrefix);

  if (source === destination) {
    return { copied: 0, skipped: 0, warnings };
  }

  let paths: readonly string[];
  try {
    // An empty source means the store's root, so the prefix must be empty
    // rather than "/" — a leading slash matches nothing.
    paths = await store.list(source ? `${source}/` : '');
  } catch (error) {
    return {
      copied: 0,
      skipped: 0,
      warnings: [`cannot inspect old saves in ${source}: ${String(error)}`],
    };
  }

  for (const path of paths) {
    const relative = source ? path.slice(source.length + 1) : path;
    if (!relative) continue;
    // Only root-level saves migrate. Without this, a file written into the
    // destination would be picked up as a source on the same pass.
    if (relative.includes('/')) continue;
    if (!isOnsSaveFileName(relative)) continue;
    if (destination && path.startsWith(`${destination}/`)) continue;

    const target = destination ? `${destination}/${relative}` : relative;
    try {
      const existing = await store.read(target);
      if (existing) {
        skipped += 1;
        continue;
      }
      const data = await store.read(path);
      if (!data) continue;
      await store.write(target, data);
      copied += 1;
    } catch (error) {
      warnings.push(`cannot migrate ${relative}: ${String(error)}`);
    }
  }

  return { copied, skipped, warnings };
}
