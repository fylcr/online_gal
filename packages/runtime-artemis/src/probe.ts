import type { ProbeContext, ProbeResult } from '@online-gal/engine-api';
// The adapter lives under the `archives` subpath, which is where VFS's byte-source helpers are
// exported; the package root deliberately exposes only the source/bridge surface.
import { gameSourceByteSource } from '@online-gal/vfs/archives';
import {
  findPfsEntry,
  hasPfsEntryEndingWith,
  hasPfsMagic,
  readPfsEntryData,
  readPfsEntryFrom,
  readPfsTable,
  readPfsTableFrom,
  type PfsArchive,
  type PfsByteSource,
} from './pfs.js';

/**
 * ArtemisEngine markers.
 *
 * Derived from the fingerprint table documented in Tyranor-Next's
 * `docs/Artemis自动引擎识别优化计划方案.md` and
 * `app/.../ArtemisEngineFingerprintDetector.kt` — file names and layout in the
 * game directory, treated as facts. No code is copied from that project, whose
 * licence (GPL-2.0-only) is incompatible with this repository's GPL-3.0.
 *
 * The distinction that matters for probing is **loose versus packaged**:
 *
 * - A loose game keeps its configuration and scripts as ordinary files at the
 *   root (`system.ini`, `boot.ini`, `system/first.iet`) alongside `.pfs`
 *   packages.
 * - A packaged game ships only `root.pfs`; the configuration is inside it.
 *
 * Numbered patches (`root.pfs.000`, `any.pfs.001`) are applied on top of the
 * base package, so their presence is itself evidence of an Artemis title.
 */
export const ARTEMIS_MARKERS = {
  /** Configuration and boot tables, present only in loose distributions. */
  configuration: ['system.ini', 'boot.ini'],
  /** The base package. */
  rootPackages: ['root.pfs'],
  /** Any package, including the numbered patch variants. */
  packageSuffix: '.pfs',
  /** The scene script entry point, inside the package or loose. */
  entryScript: 'system/first.iet',
  scriptSuffix: '.iet',
  luaSuffix: '.lua',
} as const;

/** `root.pfs.000` and `any.pfs.001` patch numbering, as documented. */
const PATCH_PATTERN = /\.pfs\.\d{3}$/i;

/** True when a file name honours the Artemis patch naming convention. */
export function isPfsPatchName(name: string): boolean {
  return PATCH_PATTERN.test(name);
}

export interface ArtemisInspection {
  /** True when a `.pfs` container exists at the root. */
  readonly hasRootPfs: boolean;
  /** True when at least one `.pfs` container exists, patches included. */
  readonly hasAnyPfs: boolean;
  /** True when a numbered patch container exists. */
  readonly hasPatchPfs: boolean;
  /** Package files found, in name order. */
  readonly packages: readonly string[];
  /** True when `system.ini` exists as a loose file. */
  readonly hasLooseSystemIni: boolean;
  /** True when the entry script exists as a loose file. */
  readonly hasLooseEntryScript: boolean;
  /** Contents of `system.ini`, from a loose file or read out of a package. */
  readonly systemIni: string | null;
  /** Where `system.ini` came from, for diagnostics. */
  readonly systemIniSource: string | null;
  /** Entry paths found inside the base package, when it could be read. */
  readonly packageEntries: readonly string[];
  /** True when the package table could not be read (encrypted or corrupt). */
  readonly packageUnreadable: boolean;
}

/** Reads a whole file, returning null instead of throwing. */
async function tryRead(
  game: ProbeContext,
  path: string,
): Promise<Uint8Array | null> {
  try {
    if (!(await game.has(path))) return null;
    const size = await game.source.size(path);
    // Packages are read into memory to walk their table, so an implausibly
    // large one is skipped rather than fetched.
    if (size > 512 * 1024 * 1024) return null;
    return await game.source.readFile(path);
  } catch {
    return null;
  }
}

/**
 * A byte source over one package, for reading it **by range**.
 *
 * This is what lets a multi-gigabyte package be inspected at all. Reading a PFS table needs the header
 * and the index — `11 + index_size` bytes, about 1.13 MB for a 2.69 GB `root.pfs`, or 0.042% — so the
 * whole-file path and its 512 MB ceiling are unnecessary whenever the source supports random access.
 *
 * The ceiling is still applied to sources that *cannot* seek, because `gameSourceByteSource` then
 * falls back to reading the file whole, and that fallback is exactly what the ceiling guards.
 */
async function pfsSource(
  game: ProbeContext,
  path: string,
): Promise<PfsByteSource | null> {
  try {
    if (!(await game.has(path))) return null;
    if (!game.source.capabilities.randomAccess) {
      const size = await game.source.size(path);
      if (size > 512 * 1024 * 1024) return null;
    }
    return gameSourceByteSource(game.source, path);
  } catch {
    return null;
  }
}

/**
 * Inspects a directory for Artemis markers.
 *
 * Separate from `probe` so the launcher can display what was found — including a
 * `system.ini` recovered from inside `root.pfs` — without committing to a match.
 */
export async function inspectArtemis(game: ProbeContext): Promise<ArtemisInspection> {
  const rootEntries = await game.list('');

  const packages: string[] = [];
  for (const entry of rootEntries) {
    if (entry.endsWith('/')) continue;
    const lower = entry.toLowerCase();
    if (!lower.endsWith(ARTEMIS_MARKERS.packageSuffix) && !PATCH_PATTERN.test(lower)) {
      continue;
    }
    packages.push(entry);
  }
  packages.sort();

  const hasRootPfs = packages.some((name) => name.toLowerCase() === 'root.pfs');
  const hasPatchPfs = packages.some((name) => isPfsPatchName(name));

  let hasLooseSystemIni = false;
  for (const name of ARTEMIS_MARKERS.configuration) {
    if (await game.has(name)) {
      hasLooseSystemIni = true;
      break;
    }
  }

  const hasLooseEntryScript =
    (await game.has(ARTEMIS_MARKERS.entryScript)) ||
    (await game.has('system/first.IET'));

  let systemIni: string | null = null;
  let systemIniSource: string | null = null;
  let packageEntries: readonly string[] = [];
  let packageUnreadable = false;

  // A loose `system.ini` is authoritative and needs no package read.
  for (const name of ARTEMIS_MARKERS.configuration) {
    if (!(await game.has(name))) continue;
    const text = await game.readText(name);
    if (text !== undefined) {
      systemIni = text;
      systemIniSource = name;
    }
    break;
  }

  if (!systemIni) {
    // Otherwise the configuration lives inside the base package. Only the base
    // is read: patches overlay it, and the base carries the boot tables.
    const base = packages.find((name) => name.toLowerCase() === 'root.pfs');
    if (base) {
      // Read by range first. A packaged title's `root.pfs` is routinely larger than the whole-file
      // ceiling — the sample here is 2.69 GB — and skipping it meant the probe reported a packaged
      // game as having no readable configuration, which reads like a corrupt package rather than an
      // oversized one.
      const source = await pfsSource(game, base);
      if (source) {
        const archive = await readPfsTableFrom(source);
        if (!archive) {
          packageUnreadable = true;
        } else {
          packageEntries = archive.entries.map((entry) => entry.path);
          systemIni = await extractSystemIniFrom(source, archive);
          if (systemIni) systemIniSource = `${base}!system.ini`;
        }
      } else {
        // No byte source: fall back to the whole-file path for small packages.
        const bytes = await tryRead(game, base);
        if (bytes) {
          if (!hasPfsMagic(bytes)) {
            packageUnreadable = true;
          } else {
            const archive = readPfsTable(bytes);
            if (!archive) {
              packageUnreadable = true;
            } else {
              packageEntries = archive.entries.map((entry) => entry.path);
              systemIni = extractSystemIni(bytes, archive);
              if (systemIni) systemIniSource = `${base}!system.ini`;
            }
          }
        }
      }
    }
  }

  return {
    hasRootPfs,
    hasAnyPfs: packages.length > 0,
    hasPatchPfs,
    packages,
    hasLooseSystemIni,
    hasLooseEntryScript,
    systemIni,
    systemIniSource,
    packageEntries,
    packageUnreadable,
  };
}

/** Pulls `system.ini` out of a readable package. */
/** Decodes `system.ini` bytes. Artemis builds ship Shift_JIS or UTF-8 depending on the target. */
function decodeSystemIni(data: Uint8Array): string | null {
  try {
    return new TextDecoder('utf-8', { fatal: true }).decode(data);
  } catch {
    try {
      return new TextDecoder('shift_jis').decode(data);
    } catch {
      return null;
    }
  }
}

function extractSystemIni(bytes: Uint8Array, archive: PfsArchive): string | null {
  const entry = findPfsEntry(archive, 'system.ini');
  if (!entry) return null;
  const data = readPfsEntryData(bytes, archive, entry);
  if (!data) return null;
  return decodeSystemIni(data);
}

/**
 * The ranged counterpart of {@link extractSystemIni}: fetches only the `system.ini` entry.
 *
 * For the 2.69 GB sample this reads a few kilobytes out of the package rather than pulling it in,
 * which is the difference between a probe that can report the title's configuration and one that
 * cannot open the file at all.
 */
async function extractSystemIniFrom(
  source: PfsByteSource,
  archive: PfsArchive,
): Promise<string | null> {
  const entry = findPfsEntry(archive, 'system.ini');
  if (!entry) return null;
  const data = await readPfsEntryFrom(source, archive, entry);
  if (!data) return null;
  return decodeSystemIni(data);
}

/**
 * Scores a directory as an ArtemisEngine title.
 *
 * The weights encode how much each signal actually proves:
 *
 * - `root.pfs` is the base package; on its own it is a strong Artemis signal.
 * - A loose `system.ini` plus a `.pfs` package is stronger still, because the
 *   loose configuration confirms the layout rather than just the container.
 * - A numbered patch alone is weak: the naming convention could belong to
 *   another engine that also uses `.pfs`.
 * - A readable package that contains `system/first.iet` is definitive.
 */
export async function probeArtemis(game: ProbeContext): Promise<{
  score: number;
  evidence: { path: string; detail: string }[];
  inspection: ArtemisInspection;
}> {
  const inspection = await inspectArtemis(game);
  const evidence: { path: string; detail: string }[] = [];
  let score = 0;

  if (inspection.hasRootPfs) {
    evidence.push({ path: 'root.pfs', detail: 'Artemis base package' });
    score += 70;
  }

  if (inspection.hasLooseSystemIni) {
    evidence.push({
      path: 'system.ini',
      detail: 'Artemis configuration table (loose)',
    });
    score += 25;
  }

  if (inspection.hasLooseEntryScript) {
    evidence.push({
      path: ARTEMIS_MARKERS.entryScript,
      detail: 'Artemis scene entry script (loose)',
    });
    score += 60;
  }

  if (inspection.packageEntries.length > 0) {
    const hasEntry = inspection.packageEntries.some((path) =>
      path.toLowerCase().endsWith(ARTEMIS_MARKERS.entryScript),
    );
    if (hasEntry) {
      evidence.push({
        path: 'root.pfs!system/first.iet',
        detail: 'scene entry script inside the base package',
      });
      score += 60;
    }
    if (inspection.systemIni) {
      evidence.push({
        path: inspection.systemIniSource ?? 'root.pfs!system.ini',
        detail: 'Artemis configuration table inside the base package',
      });
      score += 30;
    }
  }

  if (inspection.hasPatchPfs) {
    evidence.push({
      path: inspection.packages.find((name) => isPfsPatchName(name)) ?? '*.pfs.NNN',
      detail: 'numbered Artemis patch package',
    });
    score += 20;
  }

  // A patch with no base package is much weaker evidence than a real install.
  if (score === 0 && inspection.hasAnyPfs) {
    evidence.push({
      path: inspection.packages[0] ?? '*.pfs',
      detail: 'PFS container (Artemis-compatible)',
    });
    score += 25;
  }

  return { score, evidence, inspection };
}

/**
 * Builds the `ProbeResult` for the registry.
 *
 * Confidence stays below `high` unless the layout was confirmed either by a
 * loose entry script or by reading the entry script's path out of the package —
 * a bare container name is not enough to claim the engine.
 */
export async function artemisProbeResult(
  game: ProbeContext,
): Promise<ProbeResult | null> {
  const { score, evidence, inspection } = await probeArtemis(game);
  if (score <= 0) return null;

  const confirmed =
    inspection.hasLooseEntryScript ||
    inspection.packageEntries.some((path) =>
      path.toLowerCase().endsWith(ARTEMIS_MARKERS.entryScript),
    );

  const confidence = confirmed ? 'high' : score >= 70 ? 'medium' : 'low';
  const startup = confirmed
    ? ARTEMIS_MARKERS.entryScript
    : inspection.hasLooseSystemIni
      ? 'system.ini'
      : undefined;

  return {
    runtimeId: 'artemis',
    displayName: 'ArtemisEngine',
    score,
    confidence,
    ...(startup ? { startup } : {}),
    evidence: evidence.map((entry) => ({ ...entry, weight: score })),
  };
}

export { readPfsTable, findPfsEntry, hasPfsEntryEndingWith };
export type { PfsArchive };
