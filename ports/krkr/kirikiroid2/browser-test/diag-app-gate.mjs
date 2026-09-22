// One-off diagnostic: ask the *app's own* platform instance to detect two sources, so
// the two possible reasons the wasm provider declines can be told apart:
//   - a mountable source (data.xp3) still landing on `krkr2` means the deployment gate
//     is shut in the app's module instance (nothing published the page URL);
//   - landing on `krkr2-wasm` means the gate is open and the problem is the mount
//     derivation for the imported source.
import { launchEdge, sleep } from './cdp.mjs';
import { repoRootPosix as repoRootPosixUrl } from '../../lib/ref.mjs';

const BASE = process.env.KIRI_APP_BASE ?? 'http://127.0.0.1:4321';

const EXPRESSION = `(async () => {
  const platform = (await import('/src/lib/platform.ts')).getPlatform();
  const ea = await import('${repoRootPosixUrl}/packages/engine-api/src/index.ts');
  const vm = await import('${repoRootPosixUrl}/packages/runtime-kag/src/wasm-config.ts');
  const withXp3 = new ea.MemoryGameSource('probe-with-xp3');
  withXp3.addFile('data.xp3', 'XP3 fake payload');
  const loose = new ea.MemoryGameSource('probe-loose');
  loose.addFile('first.ks', '*start');
  const inXp3 = await platform.library.detect(withXp3);
  const looseResult = await platform.library.detect(loose);

  // What the *imported* record's source actually shows, and what the engine's mount
  // derivation makes of it.
  const records = await platform.library.list();
  const record = records[0] ?? null;
  let imported = null;
  if (record) {
    const source = await platform.library.openSource(record);
    imported = {
      name: record.name,
      runtimeId: record.detection.runtimeId,
      root: await source.list(''),
      hasArchive: await source.exists('game.zip'),
      mount: await vm.findKiriMount(source),
      pageUrl: vm.kirikiriWasmPageUrl(),
    };
  }

  return {
    xp3Source: { runtimeId: inXp3.runtimeId, score: inXp3.score },
    looseSource: { runtimeId: looseResult.runtimeId, score: looseResult.score },
    providers: platform.registry.providers
      ? platform.registry.providers.map((p) => p.runtimeId)
      : 'registry exposes no provider list',
    imported,
  };
})()`;

const browser = await launchEdge({ port: 9506, enable: ['Page', 'Runtime', 'Log'] });
try {
  await browser.cdp.send('Page.navigate', { url: `${BASE}/` });
  await sleep(8000);
  console.log(JSON.stringify(await browser.cdp.evaluate(EXPRESSION), null, 2));
} catch (error) {
  console.error('diagnostic failed:', String(error).slice(0, 400));
} finally {
  await browser.stop();
}
