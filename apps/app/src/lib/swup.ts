/**
 * Page transitions between the shell pages (`/`, `/settings`).
 *
 * - Only `#swup` and the header's `#page-title` are replaced; the rest of the
 *   header, the dock and the footer stay mounted.
 * - `/play` is never a swup visit: it runs an engine with its own lifetime
 *   and needs a clean document, so entering or leaving it is a full load.
 * - Head plugin merges each page's Astro-injected `<style>`/`<link>` tags.
 * - Scripts plugin re-runs body scripts so Astro islands in the new content
 *   hydrate. Astro's island bootstrap is idempotent; module scripts with the
 *   same `src` are not re-executed by the browser.
 * - Svelte islands that leave the DOM are unmounted by Astro's `astro-island`
 *   disconnect handling, so components must release resources in their
 *   teardown (`$effect` cleanup / `onDestroy`).
 */
import Swup from 'swup';
import SwupA11yPlugin from '@swup/a11y-plugin';
import SwupHeadPlugin from '@swup/head-plugin';
import SwupScriptsPlugin from '@swup/scripts-plugin';

declare global {
  interface Window {
    __swup?: Swup;
  }
}

const PLAYER_PATH = /^\/play\/?$/;

function normalize(path: string): string {
  return path.replace(/\/+$/, '') || '/';
}

/**
 * Marks nav links (`a[data-nav]`, outside `#swup`) for `pathname` and moves
 * the dock highlight (`[data-dock]`, `--dock-i`) to the matching item.
 */
/** `/` matches only itself; any other nav target also covers its sub-pages (`/settings/audio`). */
function matchesNav(link: HTMLAnchorElement, here: string): boolean {
  const target = normalize(new URL(link.href).pathname);
  return target === '/' ? here === '/' : here === target || here.startsWith(`${target}/`);
}

function markCurrentNav(pathname: string = location.pathname): void {
  const here = normalize(pathname);
  for (const dock of document.querySelectorAll<HTMLElement>('[data-dock]')) {
    const links = [...dock.querySelectorAll<HTMLAnchorElement>('a[data-nav]')];
    const index = links.findIndex((link) => matchesNav(link, here));
    // Keep the last position when idle, so the highlight fades out in place.
    if (index >= 0) dock.style.setProperty('--dock-i', String(index));
    dock.toggleAttribute('data-idle', index < 0);
  }
  for (const link of document.querySelectorAll<HTMLAnchorElement>('a[data-nav]')) {
    if (matchesNav(link, here)) {
      link.setAttribute('aria-current', 'page');
    } else {
      link.removeAttribute('aria-current');
    }
  }
}

/**
 * Header heading of the page at `pathname`, taken from the matching dock link's label
 * (the heading is the dock section name: 游戏库 / 设置). `null` when no dock item matches.
 */
function headingFor(pathname: string): string | null {
  const here = normalize(pathname);
  const link = [...document.querySelectorAll<HTMLAnchorElement>('[data-dock] a[data-nav]')].find((a) =>
    matchesNav(a, here),
  );
  return link?.getAttribute('aria-label') ?? null;
}

/**
 * Skip the header heading's fade when the visit keeps the same heading (e.g. `/settings` →
 * `/settings/audio`). The flag sits on `<html>` so it survives `#page-title` being replaced;
 * `global.css` turns the fade off under it.
 */
function holdUnchangedHeading(pathname: string): void {
  const current = document.getElementById('page-title')?.textContent?.trim() ?? '';
  const next = headingFor(pathname);
  document.documentElement.toggleAttribute('data-static-title', next !== null && next === current);
}

export function startSwup(): void {
  // The player page has no `#swup`; a re-run of this script must not create a second instance.
  if (window.__swup || !document.getElementById('swup')) return;

  const swup = new Swup({
    // The header heading and the breadcrumbs sit outside `#swup` and are swapped alongside it.
    containers: ['#swup', '#page-title', '#breadcrumbs'],
    // Only the content fade gates the visit. swup's default `[class*="transition-"]` also matched
    // Tailwind's `transition-colors` / `transition-[translate,opacity]` on the dock, so each phase
    // waited out the dock's 300ms timeout on top of the 180ms fade.
    animationSelector: '.transition-fade',
    ignoreVisit: (url) => PLAYER_PATH.test(new URL(url, location.href).pathname),
    plugins: [
      new SwupA11yPlugin(),
      new SwupHeadPlugin({ awaitAssets: true }),
      new SwupScriptsPlugin({ head: false, body: true }),
    ],
  });

  swup.hooks.on('visit:start', (visit) => {
    const pathname = new URL(visit.to.url, location.href).pathname;
    holdUnchangedHeading(pathname);
    // Move the highlight as soon as the visit starts, so it slides while the content fades.
    markCurrentNav(pathname);
  });
  swup.hooks.on('visit:end', () => document.documentElement.removeAttribute('data-static-title'));
  // Re-sync after the swap (covers redirects and aborted visits).
  swup.hooks.on('page:view', () => markCurrentNav());
  window.__swup = swup;
}
