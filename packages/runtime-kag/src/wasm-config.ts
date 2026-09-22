/**
 * Where the platform serves the built KiriKiri page, and which file in a game's
 * source the engine should mount.
 *
 * The WebAssembly engine is not self-contained the way the interpreted runtimes
 * are: it needs a *served* build (`index.html`, `index.js`, `index.wasm`,
 * `vlfs.js`, `assets.zip`) that the host publishes at a URL, and that page reads
 * the game through the host byte-source seam. Two consequences follow, and both
 * are why this module exists rather than a constant:
 *
 *  - **The engine only participates when a build is actually published.** A
 *    deployment that serves no build must not claim KiriKiri games: if it did,
 *    `probe` would win the dispatch and `load()` would fail on a missing
 *    `pageUrl`, turning games that the interpreter can run into failures. So
 *    {@link configureKirikiriWasm} is the switch, and the provider declines while
 *    no page URL is set.
 *  - **The build mounts *files*, not directories.** The framed shell takes one
 *    archive: `?xp3=` mounts an XP3 as `/data.xp3` (the normal KiriKiri shipping
 *    form), `?game=` mounts a ZIP whose entries become the game's files.
 *    {@link findKiriMount} therefore only offers a mount that the shell can
 *    actually serve, and declines otherwise — a directory of loose files stays
 *    with the interpreter instead of failing in the engine.
 */
import type { GameSource, ProbeContext } from '@online-gal/engine-api';

/**
 * The URL of the built page, or null while no build is published.
 *
 * Module state on purpose: a provider's `probe` receives only the game, never the
 * deployment's configuration, so the deployment fact has to live somewhere the
 * provider can read. The app sets it once at startup.
 */
let configuredPageUrl: string | null = null;

/** The archive inside a game's source that the shell should mount. */
export interface KiriMount {
  /** Path of the archive within the game source, e.g. `data.xp3`. */
  readonly key: string;
  /** Which shell parameter carries it: `xp3` for one archive, `game` for a ZIP. */
  readonly param: 'xp3' | 'game';
}

/** Anything that can list a directory — satisfied by both `GameSource` and `ProbeContext`. */
export interface MountSource {
  list(dir?: string): Promise<readonly string[]>;
}

/**
 * Publishes (or withdraws) the served build.
 *
 * Called by the host once, before any game is launched. An empty or whitespace
 * `pageUrl` withdraws the engine, which is what a deployment without the build
 * wants and what tests use to isolate themselves.
 */
export function configureKirikiriWasm(options: { pageUrl?: string | null }): void {
  const next = (options.pageUrl ?? '').trim();
  configuredPageUrl = next === '' ? null : next;
}

/** The published build URL, or null when the engine is unavailable here. */
export function kirikiriWasmPageUrl(): string | null {
  return configuredPageUrl;
}

/**
 * Forgets the published build.
 *
 * A test seam, and the only reason it is exported: module state would otherwise
 * leak between test files that run in one process.
 */
export function resetKirikiriWasmConfiguration(): void {
  configuredPageUrl = null;
}

/**
 * Picks the archive the framed shell should mount, or null when it cannot mount one.
 *
 * The rules follow how KiriKiri games actually ship:
 *
 *  - **An XP3 wins, and `data.xp3` wins among XP3s.** That is the shipping form,
 *    and `?xp3=` mounts it lazily — the engine reads only the parts it needs.
 *  - **Otherwise a single ZIP is the zipped-directory form** (`?game=`), which the
 *    shell expands entry by entry. Loose files beside it are expected and ignored:
 *    when a source came from a ZIP, the platform exposes the archive *and* its
 *    expanded contents, so requiring the archive to be the only entry would reject
 *    the very case this rule is for.
 *  - **Anything else is declined**, so a directory of loose files keeps going to
 *    the TypeScript interpreter rather than failing inside the engine.
 *
 * Known limit: a ZIP that is a *patch* rather than the game (with the game itself
 * only loose) is mounted as the game and will not start. Telling the two apart needs
 * the archive's entry list, which the probe context cannot read; a source like that
 * should be imported as a directory instead. Synthesising a ZIP from loose files
 * would remove the rule — and the limit — altogether.
 */
export async function findKiriMount(source: MountSource): Promise<KiriMount | null> {
  let root: readonly string[];
  try {
    root = await source.list('');
  } catch {
    return null;
  }
  const files = root.filter((name) => name && !name.endsWith('/'));
  const lower = (name: string): string => name.toLowerCase();

  const xp3 = files.filter((name) => lower(name).endsWith('.xp3'));
  if (xp3.length > 0) {
    const preferred = xp3.find((name) => lower(name) === 'data.xp3') ?? (xp3.length === 1 ? xp3[0]! : null);
    if (preferred) return { key: preferred, param: 'xp3' };
    return null;
  }

  const zip = files.filter((name) => lower(name).endsWith('.zip'));
  if (zip.length === 1) {
    // Not "the only *file*", but "the only *archive*": a source that came from a ZIP
    // shows the archive beside its own expanded contents (the platform mounts every
    // archive as a tree as well), so a lone-file test would reject exactly the case
    // this rule exists for. With no XP3 present, one ZIP is the game.
    return { key: zip[0]!, param: 'game' };
  }
  return null;
}

/** Re-exported so callers can type a `GameSource` without importing engine-api themselves. */
export type { GameSource, ProbeContext };
