import type { ProbeContext, ProbeResult } from '@online-gal/engine-api';
import { parseGameexe, type GameexeDocument } from './gameexe.js';

/**
 * Markers that identify a SiglusEngine game.
 *
 * Ported from AetherKiri's `bridge/siglus_runtime/src/siglus_runtime_provider.cpp`,
 * which probes a game root for these names and scores a match as 90. The two
 * families carry different weight: `Scene.pck` is unique to Siglus, while
 * `Gameexe.ini`/`Gameexe.dat` is the engine's configuration table and is the
 * only signal a stripped build leaves behind.
 */
export const SIGLUS_MARKERS = {
  configuration: ['Gameexe.ini', 'gameexe.ini', 'Gameexe.dat', 'gameexe.dat'],
  scenes: ['Scene.pck', 'scene.pck'],
  /** Alternate location some builds use for the packed tables. */
  dataDirectories: ['Data', 'data'],
} as const;

/** Score AetherKiri assigns to a Siglus directory match. */
export const SIGLUS_PROBE_SCORE = 90;

export interface SiglusInspection {
  /** Path the configuration table was found at, when one was found. */
  readonly configPath: string | null;
  /** Directory the configuration table lives in (`''` for the root). */
  readonly configDirectory: string | null;
  /** True when only the encrypted `Gameexe.dat` form is present. */
  readonly encryptedConfiguration: boolean;
  /** True when a packed scene table exists. */
  readonly hasScenePackage: boolean;
  /** Path the scene table was found at, when one was found. */
  readonly scenePath: string | null;
  /** Parsed configuration, when a plain-text table was readable. */
  readonly gameexe: GameexeDocument | null;
  /** Screen size the game declares, in game-native pixels. */
  readonly screenSize: { width: number; height: number } | null;
  readonly title: string | null;
  /** Directories that were searched, for diagnostics. */
  readonly searched: readonly string[];
}

/** True when any of `names` exists directly inside `directory`. */
async function findFirst(
  game: ProbeContext,
  names: readonly string[],
  directory: string,
): Promise<string | null> {
  for (const name of names) {
    const path = directory ? `${directory}/${name}` : name;
    if (await game.has(path)) return path;
  }
  return null;
}

/**
 * Inspects a game directory for Siglus markers.
 *
 * Separate from `probe` so the launcher can show what was found — including a
 * readable configuration table — without committing to a match.
 */
export async function inspectSiglus(game: ProbeContext): Promise<SiglusInspection> {
  // The marker set is the same in either location, so the root is tried first
  // and `Data/` only when the root has nothing.
  const directories = ['', ...SIGLUS_MARKERS.dataDirectories];
  const searched: string[] = [];

  let configPath: string | null = null;
  let configDirectory: string | null = null;
  let gameexe: GameexeDocument | null = null;

  for (const directory of directories) {
    searched.push(directory || '(root)');
    const found = await findFirst(game, SIGLUS_MARKERS.configuration, directory);
    if (!found) continue;
    configPath = found;
    configDirectory = directory;
    if (!found.toLowerCase().endsWith('.dat')) {
      const text = await game.readText(found);
      if (text !== undefined) gameexe = parseGameexe(text);
    }
    break;
  }

  let scenePath: string | null = null;
  for (const directory of directories) {
    const found = await findFirst(game, SIGLUS_MARKERS.scenes, directory);
    if (!found) continue;
    scenePath = found;
    if (!searched.includes(directory || '(root)')) searched.push(directory || '(root)');
    break;
  }

  return {
    configPath,
    // `Gameexe.dat` is the encrypted form of the same table; there is no public
    // decoder for it, so its screen size stays unknown rather than guessed.
    encryptedConfiguration:
      configPath !== null && configPath.toLowerCase().endsWith('.dat'),
    hasScenePackage: scenePath !== null,
    scenePath,
    configDirectory,
    gameexe,
    screenSize: gameexe?.screenSize ?? null,
    title: gameexe?.title ?? null,
    searched,
  };
}

/**
 * Scores a directory as a SiglusEngine title.
 *
 * `Scene.pck` reaches AetherKiri's score of 90 on its own. The configuration
 * table alone scores lower: it is a weaker signal that a few unrelated tools
 * also emit, and scoring it at 90 would let such a directory outrank a real
 * KiriKiri or ONScripter game.
 */
export async function probeSiglus(game: ProbeContext): Promise<{
  score: number;
  evidence: { path: string; detail: string }[];
  inspection: SiglusInspection;
}> {
  const inspection = await inspectSiglus(game);
  const evidence: { path: string; detail: string }[] = [];
  let score = 0;

  if (inspection.hasScenePackage) {
    evidence.push({
      path: inspection.configPath?.startsWith('Data') ? 'Data/Scene.pck' : 'Scene.pck',
      detail: 'packed Siglus scene table (unique to SiglusEngine)',
    });
    score += SIGLUS_PROBE_SCORE;
  }

  if (inspection.configPath) {
    evidence.push({
      path: inspection.configPath,
      detail: inspection.encryptedConfiguration
        ? 'encrypted Siglus configuration table'
        : 'Siglus configuration table',
    });
    score += inspection.hasScenePackage ? 40 : 55;
  }

  return { score, evidence, inspection };
}

/**
 * Builds the `ProbeResult` for the registry.
 *
 * `confidence` is deliberately capped below `high` when only the configuration
 * table was found, because that case cannot distinguish a Siglus game from a
 * directory that happens to carry the same file name.
 */
export async function siglusProbeResult(
  game: ProbeContext,
): Promise<ProbeResult | null> {
  const { score, evidence, inspection } = await probeSiglus(game);
  if (score <= 0) return null;

  const confidence =
    inspection.hasScenePackage && inspection.configPath
      ? 'high'
      : inspection.hasScenePackage
        ? 'high'
        : 'medium';

  return {
    runtimeId: 'siglus',
    displayName: 'SiglusEngine',
    score,
    confidence,
    ...(inspection.configPath ? { startup: inspection.configPath } : {}),
    evidence: evidence.map((entry) => ({
      path: entry.path,
      detail: entry.detail,
      weight: inspection.hasScenePackage ? SIGLUS_PROBE_SCORE : 55,
    })),
  };
}

export type { GameexeDocument };
export { parseGameexe };
