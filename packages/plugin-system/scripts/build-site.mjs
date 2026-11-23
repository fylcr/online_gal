#!/usr/bin/env node
/**
 * Builds the deployable site (Cloudflare Pages output) from the plugin system
 * and the official plugins:
 *
 *   dist/site/
 *     index.html                      the default manager as static pages + /sw.js registration script
 *     sw.js                           the plugin system (Service Worker kernel)
 *     _headers                        Cloudflare Pages cache rules
 *     registry/index.json             official plugin registry
 *     registry/p/<id>/<hash>/         plugin.json + mf-manifest.json + the plugin's files
 *
 * There is deliberately no top-level 404.html: Cloudflare Pages then serves
 * index.html for every unknown path, so a deep link opened before the worker
 * is installed still lands on the manager.
 *
 *   node scripts/build-site.mjs [--app-dist <dir>] [--out <dir>] [--github-raw <url>]
 *
 * `--github-raw` replaces https://raw.githubusercontent.com/ for end-to-end
 * tests against a local fake; production builds never set it.
 *
 * Needs `tsc -b packages/plugin-system` and a built manager (`apps/app/dist`);
 * the root `build:site` script runs both first.
 */
import { createHash } from 'node:crypto';
import { copyFile, mkdir, mkdtemp, readdir, readFile, rename, rm, stat, writeFile } from 'node:fs/promises';
import { dirname, join, relative, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { build } from 'esbuild';

import { BUNDLE_FILE, microCaCommitment, microCaFingerprint, parseBundle, verifyBundle } from '../dist/index.js';
import { officialPackagePath } from '../dist/release-metadata.js';
import { fileListHash, packDirectory } from './pack-plugin.mjs';
import { signPlugin } from './sign-plugin.mjs';
import { siteSigning } from './site-signing.mjs';

const here = dirname(fileURLToPath(import.meta.url));
const pkgRoot = resolve(here, '..');
const repoRoot = resolve(pkgRoot, '../..');

const DEFAULT_MANAGER = 'online-gal.manager';

/** Official plugins built from this repository: where their files are and their manifest. */
const OFFICIAL = [
  {
    dir: (args) => resolve(args['app-dist'] ?? join(repoRoot, 'apps/app/dist')),
    manifest: join(repoRoot, 'apps/app/plugin.manifest.json'),
    packageJson: join(repoRoot, 'apps/app/package.json'),
    // The manager is also served as plain static pages from the site root (see copyManagerToRoot),
    // and the worker installs after `load`, so install is off the first-paint path. The worker
    // therefore downloads the pages and assets (reloads and offline use are answered locally);
    // only the engine (wasm, glue, worker, source archive) and source maps wait for their first request.
    lazy: ['engines/**', '_astro/cf/**', '**/*.map'],
    root: true,
  },
];

function parseArgs(argv) {
  const args = {};
  for (let i = 0; i < argv.length; i++) {
    const arg = argv[i];
    if (!arg.startsWith('--')) throw new Error(`unexpected argument "${arg}"`);
    args[arg.slice(2)] = argv[++i];
  }
  return args;
}

const sha256 = (bytes) => createHash('sha256').update(bytes).digest('hex');
const readJson = async (path) => JSON.parse(await readFile(path, 'utf8'));

async function bundleKernel(version, githubRaw) {
  const result = await build({
    entryPoints: [join(pkgRoot, 'src/sw.ts')],
    bundle: true,
    write: false,
    // Classic script: module Service Workers are missing from older Firefox.
    format: 'iife',
    platform: 'browser',
    target: 'es2022',
    minify: true,
    sourcemap: false,
    outfile: 'sw.js',
    conditions: ['development'],
    define: {
      __KERNEL_VERSION__: JSON.stringify(version),
      __DEFAULT_MANAGER__: JSON.stringify(DEFAULT_MANAGER),
      __GITHUB_RAW__: JSON.stringify(githubRaw),
      // Compared instead of localhost by the site-admin check (openspec cf-account-admin design.md A6).
      __SITE_HOST__: JSON.stringify(process.env.SITE_HOST || 'onlinegal.de5.net'),
    },
    logLevel: 'warning',
  });
  const js = result.outputFiles.find((file) => file.path.endsWith('.js'));
  return { js: js.contents };
}

async function packOfficial(entry, args, out, signing) {
  const dir = entry.dir(args);
  try {
    await stat(join(dir, 'index.html'));
  } catch {
    throw new Error(`${dir} has no index.html; build the manager first (pnpm --filter @online-gal/app build)`);
  }
  const base = await readJson(entry.manifest);
  const { version } = await readJson(entry.packageJson);
  // Keep the file-list hash in the version, then hash the final package for
  // the immutable URL, including the version actually published.
  const probe = await packDirectory({ dir, manifest: { ...base, version }, lazy: entry.lazy });
  const manifest = { ...base, version: `${version}+${fileListHash(probe.files).slice(0, 12)}` };
  const { token, host, trust } = signing;
  const { bytes, envelope, digest, files } = await packDirectory({
    dir, manifest, lazy: entry.lazy, ...(token ? { bundle: BUNDLE_FILE } : {}),
  });
  const bundle = token ? await signPlugin(bytes, { token, siteHost: host }) : null;
  if (bundle) {
    // S13 also binds the build to the configured CA commitment. Manual signing
    // self-checks the proof against the CA authenticated by the issuing endpoint.
    const salt = trust?.salt ?? 'build';
    const hash = trust?.hash ?? await microCaCommitment(await microCaFingerprint(bundle.ca), 'cf-pages', salt);
    await verifyBundle(parseBundle(bundle), bytes, { salt, hashes: [hash] }, manifest.id);
  }
  const pending = await mkdtemp(join(out, 'registry', '.pack-'));
  let path;
  try {
    for (const file of files) {
      const dest = join(pending, file.path);
      await mkdir(dirname(dest), { recursive: true });
      await copyFile(file.abs, dest);
    }
    // Format 2: registry digest is the SHA-256 of mf-manifest.json alone.
    await writeFile(join(pending, 'mf-manifest.json'), bytes);
    await writeFile(join(pending, 'plugin.json'), envelope);
    const bundleBytes = bundle ? Buffer.from(`${JSON.stringify(bundle, null, 1)}\n`) : undefined;
    if (bundleBytes) await writeFile(join(pending, BUNDLE_FILE), bundleBytes);
    path = await officialPackagePath(manifest.id, envelope, bytes, bundleBytes);
    const target = join(out, 'registry', path);
    await mkdir(dirname(target), { recursive: true });
    await rename(pending, target);
  } finally {
    await rm(pending, { recursive: true, force: true });
  }
  const total = files.reduce((sum, file) => sum + file.size, 0);
  const lazyFiles = files.filter((file) => file.lazy);
  const eager = total - lazyFiles.reduce((sum, file) => sum + file.size, 0);
  console.log(
    `  ${manifest.id} ${manifest.version}: ${files.length} files, ${(total / 1048576).toFixed(1)} MiB (${(eager / 1048576).toFixed(1)} MiB at install, ${lazyFiles.length} lazy)`,
  );
  return [
    manifest.id,
    {
      version: manifest.version,
      digest,
      path,
      name: manifest.name,
      ...(manifest.description === undefined ? {} : { description: manifest.description }),
      ...(manifest.role === undefined ? {} : { role: manifest.role }),
    },
  ];
}

/**
 * Registers the plugin system from a plain static page, after `load`, so installing the worker
 * never competes with the first paint. Once the worker controls the site, the same pages are
 * answered from its cache. A reload is only needed when the worker is active but did not control
 * this load (a hard reload, Shift+F5), so a manager the user switched to is shown instead.
 */
const REGISTER_SCRIPT = `<script>(() => {
  if (window.__onlineGalPlugins || !('serviceWorker' in navigator)) return;
  window.__onlineGalPlugins = true;
  const reloadOnce = () => {
    try {
      const last = Number(sessionStorage.getItem('online-gal.reload') || 0);
      if (Date.now() - last < 10000) return;
      sessionStorage.setItem('online-gal.reload', String(Date.now()));
      location.reload();
    } catch {}
  };
  const start = () => navigator.serviceWorker
    .register('/sw.js', { scope: '/', updateViaCache: 'none' })
    .then((registration) => {
      if (!navigator.serviceWorker.controller && registration.active && !registration.installing && !registration.waiting) reloadOnce();
    })
    .catch((error) => console.warn('[plugin-system] could not register', error));
  if (document.readyState === 'complete') start();
  else addEventListener('load', start, { once: true });
})();</script>`;

/** Names the site root keeps for itself; the manager build must not use them. */
const RESERVED = new Set(['registry', '__ca', '__cf', 'sw.js', 'sw.js.map', '_headers', '_routes.json']);

/** Copies the default manager to the site root as plain static pages that register the worker. */
async function copyManagerToRoot(dir, out) {
  const clash = (await readdir(dir)).filter((name) => RESERVED.has(name));
  if (clash.length > 0) throw new Error(`the manager build uses names the site root keeps: ${clash.join(', ')}`);
  let pages = 0;
  for (const entry of await readdir(dir, { recursive: true, withFileTypes: true })) {
    if (!entry.isFile()) continue;
    const src = join(entry.parentPath ?? entry.path, entry.name);
    const dest = join(out, relative(dir, src));
    await mkdir(dirname(dest), { recursive: true });
    if (!entry.name.endsWith('.html')) {
      await copyFile(src, dest);
      continue;
    }
    const html = await readFile(src, 'utf8');
    const at = html.lastIndexOf('</body>');
    await writeFile(dest, at === -1 ? html + REGISTER_SCRIPT : html.slice(0, at) + REGISTER_SCRIPT + html.slice(at));
    pages++;
  }
  console.log(`  site root: the manager as static pages (${pages} html pages register /sw.js)`);
}


// Cloudflare joins values when several rules match one path, so the patterns
// here never set the same header twice. `/*` carries the isolation headers the
// kernel adds to everything it serves, so the first visit, before the worker
// exists, behaves the same.
const HEADERS = `/*
  Cross-Origin-Opener-Policy: same-origin
  Cross-Origin-Embedder-Policy: credentialless
  X-Content-Type-Options: nosniff
/sw.js
  Cache-Control: no-cache
/registry/index.json
  Cache-Control: no-cache
/registry/p/*
  Cache-Control: public, max-age=31536000, immutable
  Access-Control-Allow-Origin: *
`;

async function main() {
  const args = parseArgs(process.argv.slice(2));
  const out = resolve(args.out ?? join(repoRoot, 'dist/site'));
  await rm(out, { recursive: true, force: true });
  await mkdir(join(out, 'registry'), { recursive: true });

  console.log(`building ${out}`);
  const signing = siteSigning(process.env);
  const log = {
    bootstrap: '生产引导构建，官方包未签名，启用 CA 路由',
    required: '生产签名构建，必须签名并自验所有官方包',
    preview: '预览构建，官方包未签名',
    manual: '手动签名构建，必须签名并自验所有官方包',
    unsigned: '未启用 ONLINE_GAL_CA_BUILD_TOKEN 签名，官方包未签名',
  };
  console.log(`[plugin-system] ${log[signing.mode]}`);
  const plugins = Object.fromEntries(await Promise.all(OFFICIAL.map((entry) => packOfficial(entry, args, out, signing))));
  if (plugins[DEFAULT_MANAGER] === undefined) throw new Error(`the default manager ${DEFAULT_MANAGER} was not built`);
  await writeFile(join(out, 'registry/index.json'), `${JSON.stringify({ format: 1, plugins }, null, 1)}\n`);

  // The version is stamped into the bundle, so hash a provisional build first.
  const { version } = await readJson(join(pkgRoot, 'package.json'));
  const githubRaw = args['github-raw'] ?? 'https://raw.githubusercontent.com/';
  if (!/^https?:\/\/[^/]+\/(?:.*\/)?$/.test(githubRaw)) throw new Error('--github-raw must be an http(s) URL ending in /');
  const provisional = await bundleKernel(version, githubRaw);
  const kernelVersion = `${version}+${sha256(provisional.js).slice(0, 12)}`;
  const kernel = await bundleKernel(kernelVersion, githubRaw);
  await writeFile(join(out, 'sw.js'), kernel.js);
  console.log(`  kernel ${kernelVersion}: sw.js ${(kernel.js.byteLength / 1024).toFixed(1)} KiB`);

  const root = OFFICIAL.find((entry) => entry.root);
  if (root === undefined || (await readJson(root.manifest)).id !== DEFAULT_MANAGER) throw new Error(`the site root must serve ${DEFAULT_MANAGER}`);
  await copyManagerToRoot(root.dir(args), out);
  await writeFile(join(out, '_headers'), HEADERS);
  // Send only the registered Function prefixes to Pages Functions, not static assets.
  await writeFile(join(out, '_routes.json'), `${JSON.stringify({ version: 1, include: ['/__ca/*', '/__cf/*'], exclude: [] }, null, 1)}\n`);
}

main().catch((error) => {
  console.error(error instanceof Error ? error.message : error);
  process.exit(1);
});
