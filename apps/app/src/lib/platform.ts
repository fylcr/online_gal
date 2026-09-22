/**
 * The single client-side composition root.
 *
 * Everything that touches IndexedDB, the runtime registry or a running session
 * goes through here, so Svelte components stay presentation-only. Engine
 * discovery is entirely browser-side, which is why the platform is built once
 * per page load rather than per component.
 */
import { Platform, checkBrowserSupport, type BrowserSupport } from '@online-gal/core';
import { DEFAULT_SETTINGS } from '@online-gal/core';
import type { AppSettings, GameRecord, SaveSlotRecord } from '@online-gal/core';
import { configureKirikiriWasm } from '@online-gal/runtime-kag';

export type { AppSettings, GameRecord, SaveSlotRecord, BrowserSupport };

/**
 * Where this deployment serves the KiriKiri WebAssembly build.
 *
 * `astro.config.mjs` mounts the reference build's `out/web/release/` at this path in
 * development and preview, with the same COOP/COEP headers the app itself uses, so the
 * framed page is cross-origin isolated and same-origin with the seam that feeds it bytes.
 *
 * The engine is only *published* in development: a production `astro build` emits
 * `public/` alone and does not carry those artifacts yet, and publishing a URL that is
 * not really served would make the provider claim KiriKiri games it cannot start —
 * turning games the interpreter can run into failures. Until the artifacts are packaged
 * into the build, production keeps the engine withdrawn and the interpreter handles them.
 */
const KIRIKIRI_PAGE_URL = '/krkr2-web/index.html';

if (import.meta.env.DEV) {
  configureKirikiriWasm({ pageUrl: KIRIKIRI_PAGE_URL });
}

let platform: Platform | null = null;

/** The shared platform instance, created on first use. */
export function getPlatform(): Platform {
  platform ??= new Platform();
  return platform;
}

/** Feature-detects the APIs the runtimes depend on. */
export function browserSupport(): BrowserSupport {
  return checkBrowserSupport();
}

export const defaultSettings = DEFAULT_SETTINGS;

/** Formats milliseconds as a compact play-time string. */
export function formatPlayTime(milliseconds: number): string {
  if (!Number.isFinite(milliseconds) || milliseconds <= 0) return '—';
  const totalMinutes = Math.floor(milliseconds / 60_000);
  if (totalMinutes < 60) return `${totalMinutes} 分钟`;
  const hours = Math.floor(totalMinutes / 60);
  return `${hours} 小时 ${totalMinutes % 60} 分`;
}

/** Formats a save timestamp. */
export function formatDate(timestamp: number): string {
  return new Date(timestamp).toLocaleString('zh-CN', {
    month: '2-digit',
    day: '2-digit',
    hour: '2-digit',
    minute: '2-digit',
  });
}

export function relativeTime(timestamp: number | null): string {
  if (!timestamp) return '从未运行';
  const delta = Date.now() - timestamp;
  if (delta < 60_000) return '刚刚';
  if (delta < 3_600_000) return `${Math.floor(delta / 60_000)} 分钟前`;
  if (delta < 86_400_000) return `${Math.floor(delta / 3_600_000)} 小时前`;
  if (delta < 2_592_000_000) return `${Math.floor(delta / 86_400_000)} 天前`;
  return new Date(timestamp).toLocaleDateString('zh-CN');
}
