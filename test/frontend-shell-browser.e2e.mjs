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

// The empty notice is centred on the browser viewport, not on its parent.
async function assertEmptyCentered(target) {
  const box = await target.getByRole('heading', { name: '暂无游戏' }).locator('xpath=..').boundingBox();
  const { width, height } = target.viewportSize();
  assert(box);
  assert(Math.abs(box.x + box.width / 2 - width / 2) <= 1, `horizontal centre ${box.x + box.width / 2} vs ${width / 2}`);
  assert(Math.abs(box.y + box.height / 2 - height / 2) <= 1, `vertical centre ${box.y + box.height / 2} vs ${height / 2}`);
}

// Search is a placeholder daisyUI dialog opened by the
// navbar buttons. Their content is redesigned later; here we only check that
// each opens and closes.
async function openDialog(name) {
  const trigger = page.getByRole('button', { name });
  await trigger.focus();
  await page.keyboard.press('Enter');
  const dialog = page.getByRole('dialog', { name });
  await dialog.waitFor();
  return { trigger, dialog };
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

  await page.goto(origin);
  await hideDevToolbar();
  await page.locator('section[aria-label="本地游戏"]').waitFor();
  // Empty library: centered, frameless notice with no button.
  await page.getByRole('heading', { name: '暂无游戏' }).waitFor();
  await assertEmptyCentered(page);
  assert.equal(await page.getByRole('button', { name: '导入游戏' }).count(), 0);
  await assertShell();

  // The library page itself carries no buttons beyond the navbar and the dock.
  const pageButtons = await page.evaluate(() => {
    const outside = (element) =>
      !element.closest('[data-site-header]') && !element.closest('[data-dock]') && !element.closest('dialog');
    return [...document.querySelectorAll('button, .btn, [role="button"]')]
      .filter(outside)
      .filter((element) => element.getClientRects().length > 0)
      .map((element) => element.getAttribute('aria-label') || element.textContent.trim());
  });
  assert.deepEqual(pageButtons, []);

  await seedGames();
  await hideDevToolbar();
  // No inline search field and no floating add button.
  assert.equal(await page.locator('#game-search').count(), 0);
  assert.equal(await page.getByRole('button', { name: '添加游戏', exact: true }).count(), 0);

  const search = await openDialog('搜索游戏');
  assert.match(await search.dialog.textContent(), /搜索弹窗待重新设计/);
  await page.keyboard.press('Escape');
  await search.dialog.waitFor({ state: 'hidden' });

  // The hamburger is a daisyUI dropdown with a placeholder panel, not a dialog.
  assert.equal(await page.getByRole('dialog', { name: '打开导航菜单' }).count(), 0);
  const dropdown = page.locator('[data-site-menu]');
  await page.getByRole('button', { name: '打开导航菜单' }).focus();
  await dropdown.waitFor();
  assert.match(await dropdown.textContent(), /菜单待重新设计/);
  const toggleBox = await page.getByRole('button', { name: '打开导航菜单' }).boundingBox();
  const dropdownBox = await dropdown.boundingBox();
  assert(toggleBox && dropdownBox && dropdownBox.y >= toggleBox.y + toggleBox.height);
  await page.locator('#swup').click({ position: { x: 8, y: 8 } });
  await dropdown.waitFor({ state: 'hidden' });
  await page.getByRole('button', { name: '打开导航菜单' }).click();
  await dropdown.waitFor();
  await page.getByRole('link', { name: '设置', exact: true }).first().click();
  await page.waitForURL(/\/settings\/?$/);
  await dropdown.waitFor({ state: 'hidden' });
  await page.getByRole('link', { name: '游戏库', exact: true }).first().click();
  await page.waitForURL((url) => url.pathname === '/');

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

  const mobile = await browser.newPage({ viewport: { width: 320, height: 640 }, hasTouch: true });
  await mobile.goto(origin);
  await mobile.addStyleTag({ content: 'astro-dev-toolbar { display: none !important }' });
  await mobile.evaluate(() => window.scrollTo(0, 0));
  const cdp = await mobile.context().newCDPSession(mobile);
  await cdp.send('Emulation.setSafeAreaInsetsOverride', { insets: { top: 12, right: 12, bottom: 20, left: 8 } });
  const menuToggle = mobile.getByRole('button', { name: '打开导航菜单' });
  const menuToggleBounds = await menuToggle.boundingBox();
  const searchBounds = await mobile.getByRole('button', { name: '搜索游戏' }).boundingBox();
  const dockBounds = await mobile.getByRole('navigation', { name: '主导航' }).boundingBox();
  const footerBoundsMobile = await mobile.locator('[data-site-footer]').boundingBox();
  assert(menuToggleBounds && searchBounds && dockBounds && footerBoundsMobile);
  assert(searchBounds.x + searchBounds.width <= menuToggleBounds.x);
  assert(searchBounds.y + searchBounds.height <= footerBoundsMobile.y);
  assert(footerBoundsMobile.y >= dockBounds.y + dockBounds.height);
  assert.equal(await mobile.locator('[data-site-header]').evaluate((element) => getComputedStyle(element).position), 'fixed');
  await mobile.getByRole('button', { name: '打开导航菜单' }).tap();
  const mobileMenu = mobile.locator('[data-site-menu]');
  await mobileMenu.waitFor();
  const mobileMenuBox = await mobileMenu.boundingBox();
  assert(mobileMenuBox && mobileMenuBox.x >= 0 && mobileMenuBox.x + mobileMenuBox.width <= 320);
  await mobile.locator('#swup').tap({ position: { x: 4, y: 8 } });
  await mobileMenu.waitFor({ state: 'hidden' });
  await assertEmptyCentered(mobile);

  await mobile.goto(`${origin}/play`);
  assert.equal(await mobile.locator('[data-site-header]').count(), 0);
  assert.equal(await mobile.locator('[data-site-footer]').count(), 0);
  console.log('shell: fixed header/footer, frameless empty state, no page buttons, placeholder search dialog, placeholder menu dropdown, viewport-centred empty state, safe-area portrait layout, repeated Swup visits, bare player passed');
} finally {
  await context.close();
  await browser.close();
}
