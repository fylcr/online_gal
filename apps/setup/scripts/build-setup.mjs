import { spawnSync } from 'node:child_process';
import { cp, writeFile } from 'node:fs/promises';
import { join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { build } from 'esbuild';

const workspace = fileURLToPath(new URL('../', import.meta.url));
const headers = `/*
  Cache-Control: no-store
  X-Content-Type-Options: nosniff
  Referrer-Policy: no-referrer
  X-Frame-Options: DENY
  Content-Security-Policy: frame-ancestors 'none'
`;
const cleanupWorker = `self.addEventListener('install', (event) => event.waitUntil(self.skipWaiting()));
self.addEventListener('activate', (event) => event.waitUntil((async () => {
  await Promise.all((await caches.keys()).map((key) => caches.delete(key)));
  await self.registration.unregister();
  const windows = await self.clients.matchAll({ type: 'window', includeUncontrolled: true });
  await Promise.allSettled(windows.map((client) => client.navigate(client.url)));
})()));
`;

export function setupConfig(env) {
  let rebuild = { kind: 'manual' };
  if (env.CF_PAGES === '1') rebuild = { kind: 'pages-git' };
  else if (env.GITHUB_ACTIONS === 'true') {
    if (!/^[\w.-]+\/[\w.-]+$/.test(env.GITHUB_REPOSITORY ?? '') || !/^[1-9]\d*$/.test(env.GITHUB_RUN_ID ?? '')) {
      throw new Error('GitHub Actions 缺少有效的仓库或运行 ID。');
    }
    rebuild = { kind: 'github-rerun', repo: env.GITHUB_REPOSITORY, runId: env.GITHUB_RUN_ID };
  }
  return { siteHost: env.SITE_HOST || 'onlinegal.cc.cd', rebuild, builtAt: new Date().toISOString() };
}

export async function buildSetup(out) {
  const child = spawnSync(process.execPath, [join(workspace, 'node_modules/astro/bin/astro.mjs'), 'build'], {
    cwd: workspace,
    stdio: 'inherit',
    env: { ...process.env, PUBLIC_ONLINE_GAL_SETUP: JSON.stringify(setupConfig(process.env)) },
  });
  if (child.status !== 0) throw new Error('初始化页面构建失败。');
  await cp(join(workspace, 'dist'), out, { recursive: true });
  await build({
    entryPoints: [join(workspace, 'server/worker.ts')],
    outfile: join(out, '_worker.js'),
    bundle: true,
    format: 'esm',
    platform: 'browser',
    target: 'es2022',
  });
  await Promise.all([
    writeFile(join(out, 'sw.js'), cleanupWorker),
    writeFile(join(out, '_routes.json'), JSON.stringify({ version: 1, include: ['/__cf/*', '/__setup/*'], exclude: [] })),
    writeFile(join(out, '_headers'), headers),
  ]);
}
