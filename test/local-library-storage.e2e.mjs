import assert from 'node:assert/strict';
import { createServer } from 'node:http';
import { createRequire } from 'node:module';
import { mkdtemp, mkdir, rm, writeFile } from 'node:fs/promises';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

// Run `npm run build:packages` first. Use an external tool directory with Playwright and esbuild.
const root = fileURLToPath(new URL('../', import.meta.url));
const require = createRequire(resolve(process.env.LIBRARY_BROWSER_TOOLS ?? root, 'package.json'));
const { chromium } = require('playwright');
const { build } = require('esbuild');
const { outputFiles: [bundle] } = await build({
  stdin: {
    contents: `
      export { DirectoryPermissionError, Platform } from './packages/core/dist/index.js';
      export { FileSystemDirectorySource, IndexedDbStore } from './packages/vfs/dist/index.js';
    `,
    resolveDir: root,
  },
  bundle: true,
  format: 'esm',
  write: false,
});
const server = createServer((request, response) => {
  if (request.url === '/library.js') {
    response.setHeader('Content-Type', 'text/javascript');
    response.end(bundle.contents);
  } else {
    response.setHeader('Content-Type', 'text/html; charset=utf-8');
    response.end('<input type="file" webkitdirectory><button type="button">Restore access</button>');
  }
});
const files = {
  '00.txt': '*define\ngame\n*start\nend\n',
  'config.txt': 'original root bytes',
  'sub/config.txt': 'different nested bytes',
};
const temp = await mkdtemp(join(tmpdir(), 'online-gal-library-'));
let browser;
try {
  const inputFolder = join(temp, 'picker-fixture');
  await mkdir(join(inputFolder, 'sub'), { recursive: true });
  await Promise.all(Object.entries(files).map(([path, data]) => writeFile(join(inputFolder, path), data)));
  await new Promise((ready) => server.listen(0, '127.0.0.1', ready));
  browser = await chromium.launch(process.env.CHROMIUM_PATH ? { executablePath: process.env.CHROMIUM_PATH } : {});
  const page = await browser.newPage();
  await page.goto(`http://127.0.0.1:${server.address().port}`);
  const initial = await page.evaluate(async (files) => {
    const { FileSystemDirectorySource, Platform } = await import('/library.js');
    const platform = new Platform({ dbName: 'directory-regression' });
    const disk = await navigator.storage.getDirectory();
    const encode = (text) => new TextEncoder().encode(text);
    const names = ['first-game', 'second-game', 'moved-game'];
    const handles = [];
    for (const name of names) {
      const root = await disk.getDirectoryHandle(name, { create: true });
      const sub = await root.getDirectoryHandle('sub', { create: true });
      for (const [path, text] of Object.entries(files)) {
        const dir = path.includes('/') ? sub : root;
        const file = await dir.getFileHandle(path.split('/').at(-1), { create: true });
        const writer = await file.createWritable();
        await writer.write(name === 'moved-game' && path === 'sub/config.txt' ? 'moved folder bytes' : text);
        await writer.close();
      }
      handles.push(root);
    }
    const trace = [];
    const getFile = FileSystemFileHandle.prototype.getFile;
    FileSystemFileHandle.prototype.getFile = function () {
      trace.push(['getFile', this.name]);
      return getFile.call(this);
    };
    const slice = Blob.prototype.slice;
    Blob.prototype.slice = function (start, end, type) {
      if (this instanceof File && this.name === 'config.txt') trace.push(['slice', start, end]);
      return slice.call(this, start, end, type);
    };
    const first = (await platform.importDirectory(handles[0])).record;
    const second = (await platform.importDirectory(handles[1])).record;
    const opened = await platform.library.openSource(first);
    const beforeRange = trace.filter((item) => item[1] === 'config.txt');
    const range = new TextDecoder().decode(await opened.readRange('sub/config.txt', 3, 7));
    const afterRange = trace.filter((item) => item[1] === 'config.txt' || item[0] === 'slice');
    const size = await opened.size('sub/config.txt');
    const raw = new FileSystemDirectorySource(first.name, handles[0]);
    for (const record of [first, second]) {
      const source = await platform.library.openSource(record);
      await source.writeGameFile('config.txt', encode(`overlay ${record.name}`));
      await source.writeGameFile('save/chapter.dat', encode(`chapter ${record.name}`));
      await platform.saves.put({
        gameId: record.id,
        slot: 1,
        createdAt: Date.now(),
        caption: record.name,
        state: { checkpoint: record.name },
        playTimeMs: 0,
      });
    }
    return {
      ids: [first.id, second.id],
      detected: [first.detection.availability, second.detection.availability],
      beforeRange,
      afterRange,
      size,
      range,
      sourceIsReadOnly: !raw.capabilities.writable,
      index: await raw.list(),
      nested: await raw.list('sub'),
    };
  }, files);
  assert.deepEqual(initial.detected, ['runnable', 'runnable']);
  assert.deepEqual(initial.beforeRange, []);
  assert.deepEqual(initial.afterRange, [['getFile', 'config.txt'], ['slice', 3, 10]]);
  assert.equal(initial.size, files['sub/config.txt'].length);
  assert.equal(initial.range, files['sub/config.txt'].slice(3, 10));
  assert.equal(initial.sourceIsReadOnly, true);
  assert.deepEqual(initial.index, ['00.txt', 'config.txt', 'sub/']);
  assert.deepEqual(initial.nested, ['config.txt']);

  await page.reload();
  const [firstId, secondId] = initial.ids;
  const denied = await page.evaluate(async (id) => {
    const { DirectoryPermissionError, Platform } = await import('/library.js');
    const platform = new Platform({ dbName: 'directory-regression' });
    const record = await platform.library.get(id);
    const reopened = await platform.library.openSource(record);
    const before = new TextDecoder().decode(await reopened.readFile('config.txt'));
    let grant = 'prompt';
    const requests = [];
    // Control only the operating-system permission result on the saved browser handle.
    Object.defineProperties(record.source.handle, {
      queryPermission: { value: async () => grant },
      requestPermission: { value: async ({ mode }) => {
        requests.push({ mode, userActivation: navigator.userActivation.isActive });
        return grant = 'granted';
      } },
    });
    let code;
    try {
      await platform.library.openSource(record);
    } catch (error) {
      if (!(error instanceof DirectoryPermissionError)) throw error;
      code = error.code;
    }
    document.querySelector('button').onclick = () => {
      globalThis.restoredAccess = platform.requestDirectoryPermission(record).then(async (ok) => {
        const opened = await platform.library.openSource(record);
        return { ok, requests, overlay: new TextDecoder().decode(await opened.readFile('config.txt')) };
      });
    };
    return { before, code, requestsBeforeClick: requests.length, sameRecord: record.id === id };
  }, firstId);
  assert.deepEqual(denied, {
    before: 'overlay first-game', code: 'directory-permission-required', requestsBeforeClick: 0, sameRecord: true,
  });
  await page.getByRole('button', { name: 'Restore access' }).click();
  assert.deepEqual(await page.evaluate(() => globalThis.restoredAccess), {
    ok: true, requests: [{ mode: 'read', userActivation: true }], overlay: 'overlay first-game',
  });

  const reselect = await page.evaluate(async (id) => {
    const { Platform } = await import('/library.js');
    const platform = new Platform({ dbName: 'directory-regression' });
    const disk = await navigator.storage.getDirectory();
    const original = await disk.getDirectoryHandle('first-game');
    const stale = await platform.library.get(id);
    Object.defineProperty(stale.source.handle, 'entries', {
      value: () => { throw new DOMException('folder moved', 'NotFoundError'); },
    });
    let unavailable;
    try { await platform.library.openSource(stale); } catch (error) { unavailable = error.code; }
    const moved = await disk.getDirectoryHandle('moved-game');
    Object.defineProperty(moved, 'queryPermission', { configurable: true, value: async () => 'denied' });
    let denied;
    try { await platform.reselectDirectory(id, moved); } catch (error) { denied = error.code; }
    const afterDenied = await platform.library.get(id);
    const unchanged = await afterDenied.source.handle.isSameEntry(original);
    delete moved.queryPermission;
    const updated = await platform.reselectDirectory(id, moved);
    const reopened = await platform.library.openSource(updated);
    const decode = (bytes) => new TextDecoder().decode(bytes);
    return {
      unavailable,
      denied,
      unchanged,
      id: updated.id,
      overlay: decode(await reopened.readFile('config.txt')),
      movedBytes: decode(await reopened.readFile('sub/config.txt')),
      slot: (await platform.saves.get(id, 1))?.state,
    };
  }, firstId);
  assert.deepEqual(reselect, {
    unavailable: 'directory-permission-required', denied: 'directory-permission-required',
    unchanged: true, id: firstId,
    overlay: 'overlay first-game', movedBytes: 'moved folder bytes', slot: { checkpoint: 'first-game' },
  });

  await page.locator('input').setInputFiles(inputFolder);
  const inputId = await page.evaluate(async () => {
    const { Platform } = await import('/library.js');
    const platform = new Platform({ dbName: 'directory-regression' });
    const list = [...document.querySelector('input').files];
    // Force the nested config to arrive first. An endsWith('config.txt') lookup would take it.
    list.sort((a, b) => b.webkitRelativePath.length - a.webkitRelativePath.length);
    const { record } = await platform.importFileList(list);
    const importedAgain = await platform.importFileList(list);
    if (!importedAgain.replaced || importedAgain.record.id !== record.id) throw new Error('input re-import changed ID');
    return record.id;
  });
  await page.reload();
  const input = await page.evaluate(async (id) => {
    const { Platform } = await import('/library.js');
    const platform = new Platform({ dbName: 'directory-regression' });
    const record = await platform.library.get(id);
    const source = await platform.library.openSource(record);
    const decode = (bytes) => new TextDecoder().decode(bytes);
    return [record.source.kind, decode(await source.readFile('config.txt')), decode(await source.readFile('sub/config.txt'))];
  }, inputId);
  assert.deepEqual(input, ['files', files['config.txt'], files['sub/config.txt']]);

  await page.evaluate(async ([id, filesId]) => {
    const { Platform } = await import('/library.js');
    const platform = new Platform({ dbName: 'directory-regression' });
    await platform.library.remove(id);
    await platform.library.remove(filesId);
  }, [firstId, inputId]);
  await page.reload();
  const afterRemove = await page.evaluate(async ({ firstId, secondId, inputId, paths }) => {
    const { IndexedDbStore, Platform } = await import('/library.js');
    const platform = new Platform({ dbName: 'directory-regression' });
    const second = await platform.library.get(secondId);
    const secondSource = await platform.library.openSource(second);
    const disk = await navigator.storage.getDirectory();
    const originals = {};
    for (const name of ['first-game', 'second-game', 'moved-game']) {
      const root = await disk.getDirectoryHandle(name);
      originals[name] = {};
      for (const path of paths) {
        const dir = path.includes('/') ? await root.getDirectoryHandle('sub') : root;
        const handle = await dir.getFileHandle(path.split('/').at(-1));
        originals[name][path] = await (await handle.getFile()).text();
      }
    }
    return {
      ids: (await platform.library.list()).map(({ id }) => id),
      deletedSlots: await platform.saves.list(firstId),
      deletedOverlay: await platform.saves.fileStore(firstId).list(),
      deletedImports: await new IndexedDbStore('files', 'directory-regression', 'fileIndex').list(inputId),
      keptSlots: (await platform.saves.list(secondId)).map(({ state }) => state),
      keptOverlay: await platform.saves.fileStore(secondId).list(),
      keptBytes: new TextDecoder().decode(await secondSource.readFile('config.txt')),
      originals,
    };
  }, { firstId, secondId, inputId, paths: Object.keys(files) });
  assert.deepEqual(afterRemove, {
    ids: [secondId], deletedSlots: [], deletedOverlay: [], deletedImports: [],
    keptSlots: [{ checkpoint: 'second-game' }],
    keptOverlay: ['userfs/config.txt', 'userfs/save/chapter.dat'], keptBytes: 'overlay second-game',
    originals: {
      'first-game': files,
      'second-game': files,
      'moved-game': { ...files, 'sub/config.txt': 'moved folder bytes' },
    },
  });
  console.log(`Chromium ${browser.version()} passed directory restore, input fallback and per-game removal`);
} finally {
  await browser?.close();
  await new Promise((done) => server.close(done));
  await rm(temp, { recursive: true, force: true });
}
