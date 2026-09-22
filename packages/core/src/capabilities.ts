import type { RuntimeId } from '@online-gal/engine-api';
import type { GameRecord } from './types.js';

/** Feature detection for the browser APIs the runtimes depend on. */
export interface BrowserSupport {
  readonly indexedDb: boolean;
  readonly fileSystemAccess: boolean;
  readonly createImageBitmap: boolean;
  readonly decompressionStream: boolean;
  readonly audioContext: boolean;
  readonly offscreenCanvas: boolean;
  readonly webAssembly: boolean;
}

export function checkBrowserSupport(): BrowserSupport {
  const scope = globalThis as unknown as Record<string, unknown>;
  return {
    indexedDb: typeof indexedDB !== 'undefined',
    fileSystemAccess: typeof scope.showDirectoryPicker === 'function',
    createImageBitmap: typeof scope.createImageBitmap === 'function',
    decompressionStream: typeof scope.DecompressionStream === 'function',
    audioContext:
      typeof scope.AudioContext === 'function' || typeof scope.webkitAudioContext === 'function',
    offscreenCanvas: typeof scope.OffscreenCanvas === 'function',
    webAssembly: typeof scope.WebAssembly === 'object',
  };
}

/** Environment that cannot run the platform at all. */
export function isSupported(): boolean {
  const support = checkBrowserSupport();
  return support.indexedDb && support.createImageBitmap;
}

/**
 * Explains why a runtime cannot start in this browser.
 *
 * Returning a reason rather than a bare boolean lets the UI tell the user what
 * is missing instead of offering a play button that fails silently.
 */
export function unsupportedReason(
  runtimeId: RuntimeId,
  support: BrowserSupport = checkBrowserSupport(),
): string | null {
  if (!support.indexedDb) return '此浏览器不支持 IndexedDB，无法保存游戏库与存档。';
  switch (runtimeId) {
    case 'krkr2':
      if (!support.decompressionStream) {
        return 'KiriKiri 需要 DecompressionStream 来解包 XP3 资源，请升级浏览器。';
      }
      if (!support.createImageBitmap) return '缺少 createImageBitmap，无法解码游戏图像。';
      return null;
    case 'onscripter':
      if (!support.createImageBitmap) return '缺少 createImageBitmap，无法解码游戏图像。';
      return null;
    default:
      return null;
  }
}

/** Default engine display names, so the UI can label a record without a probe. */
export const RUNTIME_LABELS: Readonly<Record<string, string>> = {
  krkr2: 'KiriKiri / KAG3',
  onscripter: 'ONScripter',
  tyrano: 'TyranoBuilder',
  web: 'HTML5 / Web',
  siglus: 'SiglusEngine',
  artemis: 'Artemis',
  renpy: "Ren'Py",
  // The WebAssembly engine builds are their own runtime ids — a different execution
  // strategy for the same games — so each needs a label. Without one the library and
  // the play header fall back to printing the raw id (`krkr2-wasm`), which says
  // nothing to a reader.
  'krkr2-wasm': 'KiriKiri / KAG3 (WebAssembly)',
  'onscripter-wasm': 'ONScripter (WebAssembly)',
};

export function runtimeLabel(runtimeId: string | null): string {
  if (!runtimeId) return '未识别';
  return RUNTIME_LABELS[runtimeId] ?? runtimeId;
}

/** Whether a recorded game can actually be started on this platform. */
export function isPlayable(record: GameRecord): boolean {
  return record.detection.runtimeId !== null && record.detection.availability !== 'unavailable';
}
