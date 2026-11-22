/**
 * Builds the browser cf (openspec/changes/cf-account-admin design.md A5; tasks 2.4).
 *
 *   node packages/cf-browser/scripts/build.mjs
 *
 * Bundles the pinned upstream `src/commands/_generated/**` and `src/api/**` unchanged, with the
 * `#lib/*` modules that need Node replaced by `src/lib/*`. The build fails when the bundle graph
 * reaches a Node builtin or a package outside the allowlist (yargs and the packages it brings),
 * which is what keeps the output browser-only. Upstream's own `lazyCommand` already splits every
 * top-level command group behind `import()`, so `splitting` turns each group into its own chunk.
 */
import { copyFile, mkdir, rm, writeFile } from 'node:fs/promises';
import { existsSync, readFileSync, realpathSync } from 'node:fs';
import { builtinModules } from 'node:module';
import { dirname, isAbsolute, join, relative, resolve, sep } from 'node:path';
import { fileURLToPath } from 'node:url';

import * as esbuild from 'esbuild';

import { assertPinned, checkout } from './fetch-upstream.mjs';
import { writeIdentity } from './identity.mjs';

const pkgRoot = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const repoRoot = resolve(pkgRoot, '../..');
const pin = JSON.parse(readFileSync(join(pkgRoot, 'upstream.json'), 'utf8'));
const OURS = join(pkgRoot, 'src');
const OUT = join(pkgRoot, 'dist');

/** yargs and the packages its browser build pulls in; nothing else may be bundled. */
const ALLOWED_PACKAGES = new Set([
  'yargs',
  'cliui',
  'yargs-parser',
  'string-width',
  'strip-ansi',
  'wrap-ansi',
  'ansi-regex',
  'ansi-styles',
  'get-east-asian-width',
  'emoji-regex',
]);
const BUILTINS = new Set(builtinModules);
/**
 * Upstream modules reused as-is although they import Node: each uses only readFileSync (for
 * `@file` values), path.resolve and isDeepStrictEqual, which src/node-standins.ts provides.
 * Any other module importing node:* still fails the gate below.
 */
const NODE_STANDIN_IMPORTERS = new Set(['lib/body-parser.ts', 'lib/input-validation.ts', 'lib/schema-flags.ts']);
const NODE_STANDIN_MODULES = new Set(['node:fs', 'node:path', 'node:util']);

const NOTICE = `/*! cf - The Cloudflare CLI (https://github.com/cloudflare/cf @ ${pin.commit.slice(0, 12)})
 * Copyright (c) Cloudflare, Inc. Licensed under MIT OR Apache-2.0; see LICENSE-MIT.txt and LICENSE-APACHE.txt.
 * Browser build for online-gal (GPL-3.0-or-later). */`;

const packageName = (spec) => (spec.startsWith('@') ? spec.split('/').slice(0, 2).join('/') : spec.split('/')[0]);
const show = (path) => relative(repoRoot, path).split(sep).join('/');

/** A `.js` specifier as the `.ts` file next to it, or a directory's index. */
function tsFile(base) {
  const stem = base.replace(/\.js$/, '');
  for (const candidate of [`${stem}.ts`, join(stem, 'index.ts')]) if (existsSync(candidate)) return candidate;
  return null;
}

export function resolver(UP) {
  const yargsDir = realpathSync(join(pkgRoot, 'node_modules/yargs'));
  const upstreamLib = join(UP, 'lib');
  const shimmed = new Set();
  const upstreamLibUsed = new Set();

  /** Our replacement for an upstream `lib/<name>`, if there is one. */
  const shimFor = (name) => tsFile(join(OURS, 'lib', name));

  return {
    shimmed,
    upstreamLibUsed,
    plugin: {
      name: 'cf-browser',
      setup(build) {
        build.onResolve({ filter: /^#lib\// }, (args) => {
          const name = args.path.slice('#lib/'.length);
          const ours = shimFor(name);
          if (ours) {
            shimmed.add(name.replace(/\.js$/, ''));
            return { path: ours };
          }
          const theirs = tsFile(join(upstreamLib, name));
          if (!theirs) return { errors: [{ text: `no upstream module for ${args.path}` }] };
          upstreamLibUsed.add(name.replace(/\.js$/, ''));
          return { path: theirs };
        });
        build.onResolve({ filter: /^#api$/ }, () => ({ path: join(UP, 'api/index.ts') }));
        build.onResolve({ filter: /^#generated$/ }, () => ({ path: join(UP, 'commands/_generated/index.ts') }));
        build.onResolve({ filter: /^#operations\// }, (args) => {
          const file = tsFile(join(UP, 'commands/_generated/_operations', args.path.slice('#operations/'.length)));
          return file ? { path: file } : { errors: [{ text: `no upstream operation for ${args.path}` }] };
        });
        // Hand-written commands need the file system, child processes or workerd (design.md
        // Non-Goals); each one mounted inside a generated group becomes a stub that says so.
        build.onResolve({ filter: /^#commands\// }, (args) => ({ path: args.path, namespace: 'unavailable' }));
        build.onLoad({ filter: /.*/, namespace: 'unavailable' }, (args) => {
          const parts = args.path.slice('#commands/'.length).replace(/\/index\.js$/, '').split('/');
          return {
            contents: `import { unavailableCommand } from ${JSON.stringify(join(OURS, 'unavailable.ts'))};
export default unavailableCommand(${JSON.stringify(parts.at(-1))}, ${JSON.stringify(parts.join(' '))});`,
            loader: 'ts',
            resolveDir: OURS,
          };
        });
        // yargs' own browser entry fetches cliui and yargs-parser from unpkg; src/yargs.ts
        // rebuilds it from these local copies instead.
        build.onResolve({ filter: /^yargs-internal\// }, (args) => ({
          path: join(yargsDir, 'build/lib', args.path.slice('yargs-internal/'.length)),
        }));
        build.onResolve({ filter: /^(cliui|yargs-parser\/browser)$/ }, (args) =>
          args.pluginData?.inner
            ? undefined
            : build.resolve(args.path, { kind: args.kind, resolveDir: yargsDir, pluginData: { inner: true } }),
        );
        // Upstream lib modules import each other relatively; send those to our shim too.
        build.onResolve({ filter: /^\.\.?\// }, (args) => {
          if (!args.importer.startsWith(UP)) return undefined;
          const target = tsFile(resolve(args.resolveDir, args.path));
          if (!target || !target.startsWith(upstreamLib + sep)) return undefined;
          const name = relative(upstreamLib, target).split(sep).join('/').replace(/(\/index)?\.ts$/, '');
          const ours = shimFor(name);
          if (ours) {
            shimmed.add(name);
            return { path: ours };
          }
          upstreamLibUsed.add(name);
          return undefined;
        });
        // The gate: no Node builtins, no packages beyond yargs and its own dependencies.
        build.onResolve({ filter: /^[^./#]/ }, async (args) => {
          if (args.pluginData?.inner || isAbsolute(args.path)) return undefined;
          const from = args.importer ? show(args.importer) : '(entry)';
          const fromUpstream = args.importer.startsWith(UP + sep) ? relative(UP, args.importer).split(sep).join('/') : '';
          if (NODE_STANDIN_MODULES.has(args.path) && NODE_STANDIN_IMPORTERS.has(fromUpstream)) {
            return { path: join(OURS, 'node-standins.ts') };
          }
          if (args.path.startsWith('node:') || BUILTINS.has(packageName(args.path))) {
            return { errors: [{ text: `Node builtin "${args.path}" imported by ${from}` }] };
          }
          if (!ALLOWED_PACKAGES.has(packageName(args.path))) {
            return { errors: [{ text: `package "${args.path}" imported by ${from} is not on the allowlist` }] };
          }
          // A value import of "yargs" itself reaches its Node entry, whose builtins fail above;
          // upstream only imports its types, which esbuild drops before resolving.
          return build.resolve(args.path, { kind: args.kind, resolveDir: args.resolveDir, pluginData: { inner: true } });
        });
      },
    },
  };
}

/** The cf version the pinned commit declares (packages/cli/package.json). */
function upstreamVersion(UP) {
  return JSON.parse(readFileSync(join(UP, '..', 'package.json'), 'utf8')).version;
}

async function main() {
  assertPinned();
  // The kernel signs in with cf's identity; it must come from the same pin as these commands.
  await writeIdentity({ check: true });
  const UP = upstreamSource();
  const { plugin, shimmed, upstreamLibUsed } = resolver(UP);

  await rm(OUT, { recursive: true, force: true });
  await mkdir(OUT, { recursive: true });
  const result = await esbuild.build({
    entryPoints: { index: join(OURS, 'index.ts') },
    outdir: OUT,
    bundle: true,
    splitting: true,
    format: 'esm',
    platform: 'browser',
    target: 'es2022',
    minify: true,
    chunkNames: 'chunks/[name]-[hash]',
    banner: { js: NOTICE },
    legalComments: 'none',
    metafile: true,
    logLevel: 'warning',
    // Upstream modules that are bundled as-is may still read a variable at run time.
    define: {
      'process.env.NODE_ENV': '"production"',
      // upstream schema-flags wraps help text at the terminal width; the console sets this per run.
      'process.stdout.columns': 'globalThis.__CF_COLUMNS__',
      __CF_VERSION__: JSON.stringify(upstreamVersion(UP)),
    },
    plugins: [plugin],
  });

  // Nothing may be left for the browser to resolve at run time.
  for (const [file, meta] of Object.entries(result.metafile.outputs)) {
    const leaked = meta.imports.filter((entry) => entry.external);
    if (leaked.length > 0) throw new Error(`${file} still imports ${leaked.map((entry) => entry.path).join(', ')}`);
  }

  // `.txt` so a static host serves them as text when the console links to them.
  for (const name of ['LICENSE-MIT', 'LICENSE-APACHE']) await copyFile(join(checkout, name), join(OUT, `${name}.txt`));

  const outputs = Object.entries(result.metafile.outputs).filter(([file]) => file.endsWith('.js'));
  const total = outputs.reduce((sum, [, meta]) => sum + meta.bytes, 0);
  const entry = outputs.find(([file]) => file.endsWith('dist/index.js'));
  const summary = {
    commit: pin.commit,
    chunks: outputs.length,
    totalKiB: Math.round(total / 1024),
    entryKiB: entry ? Math.round(entry[1].bytes / 1024) : null,
    shimmed: [...shimmed].sort(),
    upstreamLib: [...upstreamLibUsed].sort(),
  };
  await writeFile(join(OUT, 'build.json'), `${JSON.stringify(summary, null, 1)}\n`);
  console.log(`cf-browser: ${summary.chunks} chunks, ${summary.totalKiB} KiB total, entry ${summary.entryKiB} KiB`);
  console.log(`  shimmed #lib: ${summary.shimmed.join(', ')}`);
  console.log(`  upstream #lib: ${summary.upstreamLib.join(', ')}`);
}

/** The upstream source root this build reads (`.ref/cf/packages/cli/src`). */
export const upstreamSource = () => join(checkout, pin.path, 'src');

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) {
  main().catch((error) => {
    if (!error?.errors) console.error(error instanceof Error ? error.message : error);
    process.exit(1);
  });
}
