import type { ProbeEvidence, RuntimeAvailability, RuntimeId } from '@online-gal/engine-api';

/** How a game's files are obtained. */
export type SourceKind = 'directory' | 'files' | 'local';

/**
 * Pointer to where a game's content lives.
 *
 * Only `idb` sources need no permission to reopen. `fs` sources store the
 * `FileSystemDirectoryHandle` the user granted; the browser may require
 * re-authorisation on a later visit, which the launcher surfaces explicitly
 * rather than silently failing.
 */
export interface GameSourceDescriptor {
  readonly kind: SourceKind;
  /** `FileSystemDirectoryHandle` for `directory` sources. */
  readonly handle?: FileSystemDirectoryHandle;
  /** Number of files, for `files` sources. */
  readonly fileCount?: number;
  /** Developer shortcut: a same-origin path served by the dev server. */
  readonly localPath?: string;
}

/** Detection outcome recorded on the library entry. */
export interface DetectionRecord {
  readonly runtimeId: RuntimeId | null;
  readonly runtimeName: string | null;
  readonly confidence: 'high' | 'medium' | 'low' | 'none';
  readonly score: number;
  readonly availability: RuntimeAvailability;
  readonly evidence: readonly ProbeEvidence[];
  /** Alternative engines that also matched, to offer as an override. */
  readonly alternatives: readonly { runtimeId: RuntimeId; displayName: string; score: number }[];
  readonly detectedAt: number;
}

/** Per-game settings overrides. */
export interface GameConfig {
  /** Force a runtime instead of the detected one. */
  readonly forcedRuntimeId?: RuntimeId;
  readonly surface?: { width: number; height: number };
  /** XP3 decryption key as a decimal string, when the game needs one. */
  readonly xp3Key?: string;
  /** Entry document for embedded web runtimes. */
  readonly entry?: string;
  /** Free-form runtime options surfaced in the settings drawer. */
  readonly options?: Record<string, unknown>;
}

export interface GameRecord {
  readonly id: string;
  readonly name: string;
  /** Title reported by the game itself, when it differs from the folder name. */
  readonly title?: string;
  readonly source: GameSourceDescriptor;
  readonly detection: DetectionRecord;
  readonly config: GameConfig;
  /** Cover art as a data URL. */
  readonly cover?: string;
  readonly addedAt: number;
  readonly lastPlayedAt: number | null;
  /** Accumulated play time in milliseconds. */
  readonly playTimeMs: number;
  readonly favorite: boolean;
  readonly tags: readonly string[];
}

/** A save slot stored alongside the library. */
export interface SaveSlotRecord {
  readonly gameId: string;
  readonly slot: number;
  readonly createdAt: number;
  readonly caption: string;
  /** Runtime-specific serialised state. */
  readonly state: unknown;
  /** Small PNG data URL captured at save time. */
  readonly thumbnail?: string;
  /** Total play time when the slot was written. */
  readonly playTimeMs: number;
}

/** Global application settings. */
export interface AppSettings {
  readonly textSpeed: number;
  readonly volumes: { readonly bgm: number; readonly se: number; readonly voice: number };
  readonly mute: boolean;
  readonly showDiagnostics: boolean;
  /** Default surface for runtimes that accept a resize. */
  readonly surface: { readonly width: number; readonly height: number };
  readonly theme: 'dark' | 'light';
}

export const DEFAULT_SETTINGS: AppSettings = {
  textSpeed: 45,
  volumes: { bgm: 0.8, se: 0.9, voice: 1 },
  mute: false,
  showDiagnostics: false,
  surface: { width: 1280, height: 720 },
  theme: 'dark',
};

/** Creates a stable id without depending on `crypto.randomUUID` support. */
export function createId(prefix = 'g'): string {
  if (typeof crypto !== 'undefined' && 'randomUUID' in crypto) {
    return `${prefix}_${crypto.randomUUID()}`;
  }
  return `${prefix}_${Date.now().toString(36)}_${Math.random().toString(36).slice(2, 10)}`;
}

/** Human-readable play time. */
export function formatPlayTime(milliseconds: number): string {
  if (!Number.isFinite(milliseconds) || milliseconds <= 0) return '—';
  const totalMinutes = Math.floor(milliseconds / 60000);
  if (totalMinutes < 60) return `${totalMinutes} 分钟`;
  const hours = Math.floor(totalMinutes / 60);
  const minutes = totalMinutes % 60;
  return `${hours} 小时 ${minutes} 分`;
}

export function relativeTime(timestamp: number | null): string {
  if (!timestamp) return '从未运行';
  const delta = Date.now() - timestamp;
  if (delta < 60_000) return '刚刚';
  if (delta < 3_600_000) return `${Math.floor(delta / 60_000)} 分钟前`;
  if (delta < 86_400_000) return `${Math.floor(delta / 3_600_000)} 小时前`;
  if (delta < 2_592_000_000) return `${Math.floor(delta / 86_400_000)} 天前`;
  return new Date(timestamp).toLocaleDateString();
}
