import assert from 'node:assert/strict';
import { createRequire } from 'node:module';
import { resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const origin = new URL(process.env.ONLINE_GAL_SHELL_URL ?? 'http://127.0.0.1:4358').origin;
const root = fileURLToPath(new URL('../', import.meta.url));
const require = createRequire(resolve(process.env.LIBRARY_BROWSER_TOOLS ?? root, 'package.json'));
const { chromium } = require('playwright');
const browser = await chromium.launch(process.env.CHROMIUM_PATH ? { executablePath: process.env.CHROMIUM_PATH } : {});
const context = await browser.newContext({ viewport: { width: 1280, height: 800 } });
const page = await context.newPage();
page.setDefaultTimeout(10_000);
const crossOriginRequests = [];
page.on('request', (request) => {
  if (!request.url().startsWith(origin)) crossOriginRequests.push(request.url());
});

async function hideDevToolbar() {
  await page.addStyleTag({ content: 'astro-dev-toolbar { display: none !important }' });
}

async function assertShell() {
  const geometry = await page.evaluate(() => {
    const rect = (selector) => {
      const { x, y, width, height } = document.querySelector(selector).getBoundingClientRect();
      return { x, y, width, height };
    };
    return {
      headerPosition: getComputedStyle(document.querySelector('[data-site-header]')).position,
      footerPosition: getComputedStyle(document.querySelector('[data-site-footer]')).position,
      header: rect('[data-site-header]'),
      footer: rect('[data-site-footer]'),
    };
  });
  assert.equal(geometry.headerPosition, 'fixed');
  assert.equal(geometry.footerPosition, 'fixed');
  assert.equal(geometry.header.y, 0);
  assert.equal(geometry.footer.y + geometry.footer.height, page.viewportSize().height);
}

async function seedGames() {
  await page.evaluate(async () => {
    const { getPlatform } = await import('/src/lib/platform.ts');
    const platform = getPlatform();
    for (const [folder, title, tag] of [
      ['Alpha Sakura', 'Evening Blossom', 'visual novel'],
      ['Beta Moon', 'Moonlit Story', 'mystery'],
    ]) {
      const file = new File(['*define\ngame\n*start\nend\n'], '00.txt');
      Object.defineProperty(file, 'webkitRelativePath', { value: `${folder}/00.txt` });
      const { record } = await platform.importFileList([file]);
      await platform.library.patch(record.id, { title, tags: [tag] });
    }
  });
  await page.reload();
  await page.getByRole('link', { name: '开始 Evening Blossom' }).waitFor();
}

async function openMenuWithKeyboard() {
  const toggle = page.getByRole('button', { name: '打开导航菜单' });
  await toggle.focus();
  await page.keyboard.press('Enter');
  const menu = page.getByRole('navigation', { name: '导航菜单' });
  await menu.waitFor();
  assert.equal(await menu.evaluate((element) => element.matches(':popover-open')), true);
  return { toggle, menu };
}

async function assertDelayedSearchFocus() {
  const delayed = await browser.newPage();
  const { promise, resolve: allowHydration } = Promise.withResolvers();
  try {
    await delayed.route('**/src/components/library/GameLibrary.svelte*', async (route) => {
      await promise;
      await route.continue();
    });
    await delayed.goto(`${origin}/settings/`);
    await delayed.waitForFunction(() => Boolean(window.__swup));
    await delayed.evaluate(() => {
      window.__swup.hooks.once('visit:end', () => { window.shellTestVisitEnded = true; });
    });
    await delayed.getByRole('button', { name: '搜索游戏' }).click();
    await delayed.waitForFunction(() => window.shellTestVisitEnded && document.querySelector('astro-island[ssr] #game-search'));
    allowHydration();
    await delayed.waitForFunction(() => document.activeElement === document.getElementById('game-search'));
  } finally {
    allowHydration();
    await delayed.close();
  }
}

try {
  console.log(`Chromium ${browser.version()}`);
  const staticContext = await browser.newContext({ javaScriptEnabled: false, viewport: { width: 1280, height: 800 } });
  const staticPage = await staticContext.newPage();
  await staticPage.goto(origin, { waitUntil: 'domcontentloaded' });
  await staticPage.locator('[data-site-header]').waitFor();
  await staticPage.getByRole('heading', { name: '游戏库', exact: true }).waitFor();
  await staticPage.locator('[data-site-footer]').waitFor();
  await staticContext.close();
  await assertDelayedSearchFocus();
  await page.goto(origin);
  await hideDevToolbar();
  await page.locator('section[aria-label="本地游戏"]').waitFor();
  await page.getByRole('heading', { name: '尚无游戏' }).waitFor();
  await assertShell();

  await seedGames();
  await hideDevToolbar();
  const searchButton = page.getByRole('button', { name: '搜索游戏' });
  await searchButton.click();
  await page.waitForFunction(() => document.activeElement === document.querySelector('#game-search'));
  await page.getByRole('heading', { name: '游戏库', exact: true }).click();
  const settings = page.getByRole('link', { name: '设置', exact: true });
  await settings.first().click();
  await page.waitForURL(/\/settings\/?$/);
  await searchButton.focus();
  await page.keyboard.press('Enter');
  await page.waitForURL((url) => url.pathname === '/');
  const search = page.getByRole('searchbox', { name: '搜索游戏' });
  await search.waitFor();
  await page.waitForFunction(() => document.activeElement === document.querySelector('#game-search'));
  await assertShell();

  await search.fill('blossom');
  await page.getByRole('link', { name: '开始 Evening Blossom' }).waitFor();
  assert.equal(await page.getByRole('link', { name: '开始 Moonlit Story' }).count(), 0);
  await search.fill('mystery');
  await page.getByRole('link', { name: '开始 Moonlit Story' }).waitFor();
  assert.equal(await page.getByRole('link', { name: '开始 Evening Blossom' }).count(), 0);
  await search.fill('not a local game');
  await page.getByRole('status').filter({ hasText: '搜索无结果' }).waitFor();
  assert.equal(new URL(page.url()).search, '');
  await page.getByRole('button', { name: '清空' }).click();
  await page.getByRole('link', { name: '开始 Evening Blossom' }).waitFor();
  await page.getByRole('link', { name: '开始 Moonlit Story' }).waitFor();
  assert.deepEqual(crossOriginRequests, []);

  const { toggle, menu } = await openMenuWithKeyboard();
  assert.equal(await menu.getByRole('link', { name: '游戏库', exact: true }).getAttribute('aria-current'), 'page');
  await page.keyboard.press('Escape');
  await menu.waitFor({ state: 'hidden' });
  assert.equal(await toggle.evaluate((element) => document.activeElement === element), true);

  await searchButton.click();
  await openMenuWithKeyboard();
  await page.keyboard.press('Tab');
  assert.equal(await menu.evaluate((element) => element.contains(document.activeElement)), true);
  await page.keyboard.press('Escape');
  await menu.waitFor({ state: 'hidden' });
  await toggle.click();
  await menu.waitFor();
  await page.locator('#swup').click({ position: { x: 8, y: 8 } });
  await menu.waitFor({ state: 'hidden' });

  for (let index = 0; index < 3; index += 1) {
    await page.getByRole('link', { name: '设置', exact: true }).first().click();
    await page.waitForURL(/\/settings\/?$/);
    await page.getByRole('link', { name: '游戏库', exact: true }).first().click();
    await page.waitForURL((url) => url.pathname === '/');
    await assertShell();
  }

  await page.goto(`${origin}/settings/audio`);
  await hideDevToolbar();
  await page.locator('#swup').evaluate((main) => {
    const spacer = document.createElement('div');
    spacer.style.height = '140vh';
    const tail = document.createElement('button');
    tail.id = 'shell-scroll-tail';
    tail.textContent = 'tail';
    main.append(spacer, tail);
  });
  await page.evaluate(() => window.scrollTo(0, document.body.scrollHeight));
  await assertShell();
  const tail = page.locator('#shell-scroll-tail');
  await tail.focus();
  const focusBounds = await tail.boundingBox();
  const footerBounds = await page.locator('[data-site-footer]').boundingBox();
  assert(focusBounds && footerBounds && focusBounds.y + focusBounds.height <= footerBounds.y);
  const { menu: settingsMenu } = await openMenuWithKeyboard();
  assert.equal(await settingsMenu.getByRole('link', { name: '设置', exact: true }).getAttribute('aria-current'), 'page');
  await settingsMenu.getByRole('link', { name: '游戏库', exact: true }).click();
  await page.waitForURL((url) => url.pathname === '/');
  await searchButton.click();
  await page.waitForURL((url) => url.pathname === '/');
  await page.waitForFunction(() => document.activeElement === document.querySelector('#game-search'));

  const mobile = await browser.newPage({ viewport: { width: 320, height: 640 }, hasTouch: true });
  await mobile.goto(origin);
  await mobile.addStyleTag({ content: 'astro-dev-toolbar { display: none !important }' });
  await mobile.evaluate(() => window.scrollTo(0, 0));
  const cdp = await mobile.context().newCDPSession(mobile);
  await cdp.send('Emulation.setSafeAreaInsetsOverride', { insets: { top: 12, right: 12, bottom: 20, left: 8 } });
  const add = mobile.getByRole('button', { name: '添加游戏', exact: true });
  const initial = await add.boundingBox();
  await cdp.send('Emulation.setSafeAreaInsetsOverride', { insets: { top: 12, right: 20, bottom: 32, left: 8 } });
  const safe = await add.boundingBox();
  const menuToggle = mobile.getByRole('button', { name: '打开导航菜单' });
  const menuToggleBounds = await menuToggle.boundingBox();
  const searchBounds = await mobile.getByRole('button', { name: '搜索游戏' }).boundingBox();
  const dockBounds = await mobile.getByRole('navigation', { name: '主导航' }).boundingBox();
  const footerBoundsMobile = await mobile.locator('[data-site-footer]').boundingBox();
  assert(initial && safe && menuToggleBounds && searchBounds && dockBounds && footerBoundsMobile);
  assert.equal(initial.x - safe.x, 8);
  assert.equal(initial.y - safe.y, 12);
  assert.equal(await add.evaluate((element) => getComputedStyle(element).position), 'fixed');
  assert(searchBounds.x + searchBounds.width <= menuToggleBounds.x);
  assert(searchBounds.y + searchBounds.height <= footerBoundsMobile.y);
  assert(footerBoundsMobile.y >= dockBounds.y + dockBounds.height);
  assert(safe.y + safe.height <= dockBounds.y);
  assert.equal(await mobile.locator('[data-site-header]').evaluate((element) => getComputedStyle(element).position), 'fixed');
  await mobile.getByRole('button', { name: '打开导航菜单' }).tap();
  const mobileMenu = mobile.getByRole('navigation', { name: '导航菜单' });
  await mobileMenu.waitFor();
  const menuBounds = await mobileMenu.boundingBox();
  assert(menuBounds && menuBounds.x + menuBounds.width <= 300 && menuBounds.y >= 60);
  await mobile.locator('#swup').tap({ position: { x: 4, y: 8 } });
  await mobileMenu.waitFor({ state: 'hidden' });

  await mobile.goto(`${origin}/play`);
  assert.equal(await mobile.locator('[data-site-header]').count(), 0);
  assert.equal(await mobile.locator('[data-site-footer]').count(), 0);
  console.log('3.4-3.7: fixed shell, scroll focus, safe-area portrait layout, keyboard/touch menu, repeated Swup visits, local search and bare player passed');
} finally {
  await context.close();
  await browser.close();
}
