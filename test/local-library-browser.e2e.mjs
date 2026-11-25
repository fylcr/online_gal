import assert from 'node:assert/strict';
import { mkdtemp, mkdir, readFile, rm, writeFile } from 'node:fs/promises';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { pathToFileURL } from 'node:url';

const origin = process.env.ONLINE_GAL_LIBRARY_URL ?? 'http://127.0.0.1:4322';
const { PLAYWRIGHT_MODULE, CHROMIUM_EXECUTABLE } = process.env;
assert(PLAYWRIGHT_MODULE, 'Set PLAYWRIGHT_MODULE to the Playwright index.mjs path');
const { chromium } = await import(pathToFileURL(PLAYWRIGHT_MODULE).href);
const browser = await chromium.launch({ executablePath: CHROMIUM_EXECUTABLE });
const fixturesDir = await mkdtemp(join(tmpdir(), 'online-gal-library-'));
const games = ['Directory Alpha', 'Directory Beta'].map((name) => ({
  name,
  directory: join(fixturesDir, name),
  files: {
    'sub/config.txt': `nested: ${name}`,
    'config.txt': `root: ${name}`,
    'index.html': `<!doctype html><html><body><h1>Playing ${name}</h1></body></html>`,
  },
}));

async function loadLibrary(page) {
  await page.goto(origin);
  // Astro's dev toolbar otherwise covers the mobile dock on this narrow viewport.
  await page.addStyleTag({ content: 'astro-dev-toolbar { display: none !important }' });
  await page.locator('section[aria-label="本地游戏"]').waitFor();
}

async function openPicker(page, keyboard = false) {
  const add = page.getByRole('button', { name: '添加游戏', exact: true });
  if (keyboard) {
    await add.focus();
    await page.keyboard.press('Enter');
  } else {
    await add.click();
  }
  const dialog = page.getByRole('dialog', { name: '添加游戏' });
  await dialog.waitFor();
  return dialog.getByRole('button', { name: '本地文件夹' });
}

async function gameId(page, game) {
  const href = await page.getByRole('link', { name: `开始 ${game.name}` }).getAttribute('href');
  assert(href.startsWith('/play?id='), href);
  return new URL(href, origin).searchParams.get('id');
}

async function play(page, game, id) {
  await page.getByRole('link', { name: `开始 ${game.name}` }).click();
  await page.waitForURL((url) => url.pathname === '/play' && url.searchParams.get('id') === id);
  await playing(page, game);
}

async function playing(page, game) {
  await page.frameLocator('iframe[title="game"]').getByRole('heading', { name: `Playing ${game.name}` }).waitFor();
  assert.equal(await page.locator('canvas').isVisible(), true);
}

async function saveAndRead(page, id, overlayText) {
  return page.evaluate(async ({ id, overlayText }) => {
    // Use the same composition root as the UI. Run through astro dev so this module URL exists.
    const { getPlatform } = await import('/src/lib/platform.ts');
    const { library, saves } = getPlatform();
    const record = await library.get(id);
    const decode = (bytes) => new TextDecoder().decode(bytes);
    let paths;
    if (overlayText !== undefined) {
      const source = await library.openSource(record);
      paths = {
        root: decode(await source.readFile('config.txt')),
        nested: decode(await source.readFile('sub/config.txt')),
      };
      await source.writeGameFile('config.txt', new TextEncoder().encode(overlayText));
      paths.overlaid = decode(await source.readFile('config.txt'));
      await saves.put({ gameId: id, createdAt: 1, slot: 1, caption: `slot ${id}`, state: {}, playTimeMs: 0 });
    }
    const store = saves.fileStore(id);
    const overlay = {};
    for (const path of await store.list()) overlay[path] = decode(await store.read(path));
    return { record: record?.id, slot: (await saves.get(id, 1))?.caption, overlay, paths };
  }, { id, overlayText });
}

async function verifySourceFiles() {
  for (const { directory, files } of games) {
    for (const [path, content] of Object.entries(files)) {
      assert.deepEqual(await readFile(join(directory, path)), Buffer.from(content));
    }
  }
}

try {
  for (const { directory, files } of games) {
    await mkdir(join(directory, 'sub'), { recursive: true });
    for (const [path, content] of Object.entries(files)) await writeFile(join(directory, path), content);
  }
  console.log(`Chromium ${browser.version()}`);
  const [alpha, beta] = games;
  const context = await browser.newContext({ viewport: { width: 320, height: 640 }, hasTouch: true });
  // Exercise real structured-cloned handle persistence with browser OPFS handles.
  // Picker selection and permission transitions are controlled test inputs.
  await context.addInitScript(() => {
    const value = (key, fallback) => sessionStorage.getItem(`test:${key}`) ?? fallback;
    const chooser = async ({ mode }) => {
      if (mode !== 'read' || !navigator.userActivation.isActive) throw new Error('expected user gesture and read-only picker');
      const name = value('folder', 'Directory Alpha');
      switch (value('picker', 'select')) {
        case 'cancel': throw new DOMException('canceled', 'AbortError');
        case 'fail': throw new DOMException('could not open', 'NotReadableError');
        default: {
          const handle = await (await navigator.storage.getDirectory()).getDirectoryHandle(name);
          sessionStorage.setItem('test:permission', 'granted');
          return handle;
        }
      }
    };
    Object.defineProperty(window, 'showDirectoryPicker', {
      configurable: true,
      get: () => value('picker', 'select') === 'fallback' ? undefined : chooser,
    });
    Object.defineProperties(FileSystemHandle.prototype, {
      queryPermission: { configurable: true, value: async () => value('permission', 'granted') },
      requestPermission: {
        configurable: true,
        value: async ({ mode }) => {
          if (mode !== 'read' || !navigator.userActivation.isActive) throw new Error('expected user gesture and read-only permission');
          if (value('decision', 'denied') === 'denied') return 'denied';
          sessionStorage.setItem('test:permission', 'granted');
          return 'granted';
        },
      },
    });
  });
  const page = await context.newPage();
  page.setDefaultTimeout(10_000);
  const set = (key, value) => page.evaluate(([k, v]) => sessionStorage.setItem(`test:${k}`, v), [key, value]);
  await loadLibrary(page);
  await page.getByRole('heading', { name: '尚无游戏' }).waitFor();
  assert.equal(await page.evaluate(() => crossOriginIsolated), true);
  const add = page.getByRole('button', { name: '添加游戏', exact: true });
  const bounds = await add.boundingBox();
  const dock = await page.getByRole('navigation', { name: '主导航' }).boundingBox();
  assert(bounds && dock && bounds.x > 200 && bounds.x + bounds.width <= 320 && bounds.y + bounds.height < dock.y);
  assert.equal(bounds.width, bounds.height);
  await openPicker(page, true);
  await page.keyboard.press('Escape');
  await page.getByRole('link', { name: '设置', exact: true }).click();
  await page.waitForURL(/\/settings\/?$/);
  await add.waitFor({ state: 'detached' });
  await page.getByRole('link', { name: '游戏库', exact: true }).click();
  await add.waitFor();
  await page.getByRole('heading', { name: '尚无游戏' }).waitFor();

  await page.evaluate(async ({ name, files }) => {
    const root = await navigator.storage.getDirectory();
    const dir = await root.getDirectoryHandle(name, { create: true });
    for (const [path, content] of Object.entries(files)) {
      const [leaf, nested] = path.split('/').reverse();
      const parent = nested ? await dir.getDirectoryHandle(nested, { create: true }) : dir;
      const handle = await parent.getFileHandle(leaf, { create: true });
      const stream = await handle.createWritable();
      await stream.write(content);
      await stream.close();
    }
    // Re-select a distinct handle to the same game (e.g. after moving the folder).
    const moved = await root.getDirectoryHandle('Moved Alpha', { create: true });
    for (const [path, content] of Object.entries(files)) {
      const [leaf, nested] = path.split('/').reverse();
      const parent = nested ? await moved.getDirectoryHandle(nested, { create: true }) : moved;
      const stream = await (await parent.getFileHandle(leaf, { create: true })).createWritable();
      await stream.write(content);
      await stream.close();
    }
  }, alpha);
  await set('picker', 'cancel');
  await (await openPicker(page)).tap();
  await page.getByRole('button', { name: '本地文件夹' }).waitFor();
  await set('picker', 'fail');
  await page.getByRole('button', { name: '本地文件夹' }).tap();
  await page.getByRole('alert').filter({ hasText: '导入失败' }).waitFor();
  await page.getByRole('button', { name: '关闭', exact: true }).click();
  await page.getByRole('heading', { name: '尚无游戏' }).waitFor();
  await set('picker', 'select');
  await (await openPicker(page)).tap();
  await page.getByRole('status').filter({ hasText: /已添加.*识别为/ }).waitFor();
  const alphaId = await gameId(page, alpha);
  const alphaSaves = await saveAndRead(page, alphaId, 'Alpha save file');
  assert.deepEqual(alphaSaves.paths, { root: alpha.files['config.txt'], nested: alpha.files['sub/config.txt'], overlaid: 'Alpha save file' });
  assert.deepEqual(alphaSaves.overlay, { 'userfs/config.txt': 'Alpha save file' });
  await play(page, alpha, alphaId);
  await page.reload();
  await playing(page, alpha);

  await set('permission', 'prompt');
  await page.reload();
  await page.getByRole('alert').filter({ hasText: '请重新授权' }).waitFor();
  await page.getByRole('button', { name: '重新授权' }).click();
  await page.getByRole('alert').filter({ hasText: '未获得读取权限' }).waitFor();
  await set('decision', 'granted');
  await page.getByRole('button', { name: '重新授权' }).click();
  await playing(page, alpha);

  await set('permission', 'denied');
  await page.reload();
  const reselect = page.getByRole('button', { name: '重选文件夹' });
  for (const outcome of ['cancel', 'fail']) {
    await set('picker', outcome);
    await reselect.click();
    if (outcome === 'fail') await page.getByRole('alert').filter({ hasText: '恢复目录失败' }).waitFor();
    assert.deepEqual(await saveAndRead(page, alphaId), { ...alphaSaves, paths: undefined });
    assert.equal(new URL(page.url()).searchParams.get('id'), alphaId);
  }
  await set('picker', 'select');
  await set('folder', 'Moved Alpha');
  await set('permission', 'prompt');
  await reselect.click();
  await playing(page, alpha);
  assert.deepEqual(await saveAndRead(page, alphaId), { ...alphaSaves, paths: undefined });
  assert.equal(new URL(page.url()).searchParams.get('id'), alphaId);
  await page.getByRole('link', { name: '返回游戏库' }).click();
  assert.equal(await gameId(page, alpha), alphaId);
  console.log('4.5 and 4.7–4.8: empty/existing library, Swup, keyboard/touch, retained handle, reauthorization and same-ID reselect passed');

  await set('picker', 'fallback');
  const folder = await openPicker(page);
  const [chooser] = await Promise.all([page.waitForEvent('filechooser'), folder.tap()]);
  await chooser.setFiles(beta.directory);
  await page.getByRole('status').filter({ hasText: /已添加.*识别为/ }).waitFor();
  const betaId = await gameId(page, beta);
  assert.notEqual(betaId, alphaId);
  const betaSaves = await saveAndRead(page, betaId, 'Beta save file');
  assert.deepEqual(betaSaves.paths, { root: beta.files['config.txt'], nested: beta.files['sub/config.txt'], overlaid: 'Beta save file' });
  assert.deepEqual(betaSaves.overlay, { 'userfs/config.txt': 'Beta save file' });
  await page.reload();
  assert.equal(await gameId(page, beta), betaId);
  await play(page, beta, betaId);
  await page.getByRole('link', { name: '返回游戏库' }).click();
  console.log('4.8: physical directory input, exact nested paths, reopen after reload and overlay write passed');

  const card = page.getByRole('link', { name: `开始 ${alpha.name}` });
  const remove = page.locator('[data-card-context-action]');
  await card.click({ button: 'right' });
  await remove.waitFor();
  await page.keyboard.press('Escape');
  await remove.waitFor({ state: 'hidden' });
  await card.click({ button: 'right' });
  await remove.waitFor();
  await page.getByRole('heading', { name: '游戏库', exact: true }).click();
  await remove.waitFor({ state: 'hidden' });
  await card.click({ button: 'right' });
  await remove.click();
  const confirmation = page.getByRole('dialog', { name: '从游戏库删除' });
  await confirmation.getByText(/Directory Alpha.*本站保存的该游戏存档会一并删除/).waitFor();
  await confirmation.getByRole('button', { name: '取消' }).click();
  assert.deepEqual(await saveAndRead(page, alphaId), { ...alphaSaves, paths: undefined });
  assert.deepEqual(await saveAndRead(page, betaId), { ...betaSaves, paths: undefined });
  await page.getByRole('button', { name: `游戏详情 ${alpha.name}` }).tap();
  await page.getByRole('dialog', { name: alpha.name }).getByRole('button', { name: '从游戏库删除' }).click();
  await confirmation.getByText(/Directory Alpha.*本站保存的该游戏存档会一并删除/).waitFor();
  await confirmation.getByRole('button', { name: '取消' }).click();
  await card.click({ button: 'right' });
  await remove.click();
  await confirmation.getByRole('button', { name: '确认删除' }).click();
  await card.waitFor({ state: 'detached' });
  await page.reload();
  assert.equal(await gameId(page, beta), betaId);
  assert.equal(await card.count(), 0);
  assert.deepEqual(await saveAndRead(page, alphaId), { record: undefined, slot: undefined, overlay: {}, paths: undefined });
  assert.deepEqual(await saveAndRead(page, betaId), { ...betaSaves, paths: undefined });
  assert.deepEqual(await page.evaluate(async (paths) => {
    const root = await navigator.storage.getDirectory();
    const values = [];
    for (const folder of ['Directory Alpha', 'Moved Alpha']) {
      const dir = await root.getDirectoryHandle(folder);
      for (const path of paths) {
        const [leaf, nested] = path.split('/').reverse();
        const parent = nested ? await dir.getDirectoryHandle(nested) : dir;
        values.push(await (await (await parent.getFileHandle(leaf)).getFile()).text());
      }
    }
    return values;
  }, Object.keys(alpha.files)), [...Object.values(alpha.files), ...Object.values(alpha.files)]);
  await verifySourceFiles();
  console.log('4.9: dismiss, cancel, shared confirmation, delete, reload, save/overlay isolation and untouched source bytes passed');
  await context.close();
} finally {
  await browser.close();
  await rm(fixturesDir, { recursive: true, force: true });
}
