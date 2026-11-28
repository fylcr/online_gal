/**
 * Page transitions between the shell pages (`/`, `/settings`).
 *
 * - The content, page title and breadcrumbs are replaced; the rest of the
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
import { matchesSection } from './section-nav';

declare global {
  interface Window {
    __swup?: Swup;
  }
}

const PLAYER_PATH = /^\/play\/?$/;
const LIBRARY_PATH = '/';

/**
 * Marks nav links (`a[data-nav]`, outside `#swup`) for `pathname` and moves
 * the dock highlight (`[data-dock]`, `--dock-i`) to the matching item.
 */
function matchesNav(link: HTMLAnchorElement, pathname: string): boolean {
  return matchesSection(new URL(link.href).pathname, pathname);
}

function markCurrentNav(pathname: string = location.pathname): void {
  for (const dock of document.querySelectorAll<HTMLElement>('[data-dock]')) {
    const links = [...dock.querySelectorAll<HTMLAnchorElement>('a[data-nav]')];
    const index = links.findIndex((link) => matchesNav(link, pathname));
    // Keep the last position when idle, so the highlight fades out in place.
    if (index >= 0) dock.style.setProperty('--dock-i', String(index));
    dock.toggleAttribute('data-idle', index < 0);
  }
  for (const link of document.querySelectorAll<HTMLAnchorElement>('a[data-nav]')) {
    if (matchesNav(link, pathname)) {
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
  const link = [...document.querySelectorAll<HTMLAnchorElement>('[data-dock] a[data-nav]')].find((a) =>
    matchesNav(a, pathname),
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

function focusLibrarySearch(signal: AbortSignal): void {
  const input = document.querySelector<HTMLInputElement>('#game-search');
  if (!input) return;
  const focus = () => {
    if (!input.isConnected) return;
    input.scrollIntoView({ block: 'nearest' });
    input.focus({ preventScroll: true });
  };
  // The server-rendered field exists before Svelte can handle input.
  const island = input.closest('astro-island[ssr]');
  if (island) island.addEventListener('astro:hydrate', focus, { once: true, signal });
  else focus();
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

  const menu = document.getElementById('site-menu');
  const closeMenu = () => menu?.hidePopover?.();
  let searchFocus = new AbortController();
  const focusSearch = () => {
    searchFocus.abort();
    searchFocus = new AbortController();
    focusLibrarySearch(searchFocus.signal);
  };

  document.querySelector('[data-search-game]')?.addEventListener('click', () => {
    if (matchesSection(LIBRARY_PATH, location.pathname) && document.getElementById('game-search')) {
      focusSearch();
    } else {
      swup.navigate(LIBRARY_PATH, { meta: { focusLibrarySearch: true } });
    }
  });
  menu?.addEventListener('click', (event) => {
    if (event.target instanceof Element && event.target.closest('a')) closeMenu();
  });

  swup.hooks.on('visit:start', (visit) => {
    closeMenu();
    searchFocus.abort();
    if (visit.meta.focusLibrarySearch === true) visit.a11y.focus = false;
    const pathname = new URL(visit.to.url, location.href).pathname;
    holdUnchangedHeading(pathname);
    // Move the highlight as soon as the visit starts, so it slides while the content fades.
    markCurrentNav(pathname);
  });
  swup.hooks.on('visit:end', (visit) => {
    document.documentElement.removeAttribute('data-static-title');
    if (visit.meta.focusLibrarySearch === true) focusSearch();
  });
  // Re-sync after the swap (covers redirects and aborted visits).
  swup.hooks.on('page:view', () => markCurrentNav());
  window.__swup = swup;
}
