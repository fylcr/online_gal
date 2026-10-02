/**
 * Read-only source-upload guard. Worktree mode includes visible new files;
 * --staged reads the index, not newer working copies. Neither mode stages files,
 * builds engines, checks game behaviour, or grants redistribution permission.
 */
import { spawnSync } from 'node:child_process';
import { existsSync, readFileSync, statSync } from 'node:fs';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

export const SOURCE_ENTRIES = [
  'README.md', 'LICENSE', 'THIRD_PARTY_NOTICES.md', 'package.json', '.gitignore',
  'docs/source-layout.md', 'ports/README.md', 'ports/env.sh', 'ports/lib/ref.mjs',
  'ports/lib/engine-artifacts.json', '.github/upstream-pins.json',
  '.github/scripts/make-package.mjs', '.github/workflows/build-wasm-reusable.yml',
  '.github/scripts/build-ons.sh', '.github/workflows/build-wasm-ons.yml',
  'ports/ons/README.md', 'ports/ons/build/run-web-build-shimmed.sh',
  'ports/ons/build/ons_host_shim.h', 'ports/ons/build/ons_host_shim.cpp',
  '.github/scripts/build-krkr.sh', '.github/workflows/build-wasm-krkr.yml',
  'ports/krkr/README.md', 'ports/krkr/kirikiroid2/run-build.sh',
  'ports/krkr/kirikiroid2/patch-vlfs-host-source.mjs',
  'ports/krkr/kirikiroid2/overlay-ports/gettext/portfile.cmake',
  'ports/krkr/kirikiroid2/overlay-ports/libexif/portfile.cmake',
  '.github/scripts/build-artemis-bridge.sh', '.github/workflows/build-wasm-artemis-bridge.yml',
  'ports/artemis/README.md', 'ports/artemis/REWRITE.md', 'ports/artemis/rewrite-names.tsv',
  'ports/artemis/src/util/engine_context.h', 'ports/artemis/reverse/build-bridge-route.sh',
  'ports/artemis/reverse/rw-splice.py', 'ports/artemis/reverse/test-rw-splice-diagnostics.py',
  'ports/artemis/tools/assemble-rewrite-parts.py',
  'packages/runtime-artemis/src/index.ts', 'packages/runtime-artemis/src/host/boot.ts',
  'packages/runtime-ons/src/wasm-runtime.ts', 'packages/runtime-kag/src/wasm-runtime.ts',
  'packages/runtime-kag/src/wasm-config.ts', 'scripts/check-sources.mjs',
  'test/check-sources.test.mjs',
];

const ASSET_EXCEPTIONS = new Set([
  'packages/runtime-ons/assets/onsyuri.wasm',
  'ports/artemis/vendor/artemis-sdk/WebAssembly/artemis.wasm',
  'ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.wasm',
  'ports/artemis/vendor/artemis-sdk/Android/arm64-v8a/libartemis.so',
  'ports/artemis/vendor/artemis-sdk/Android/armeabi-v7a/libartemis.so',
]);

export function uploadProblem(file) {
  if (/(^|\/)(?:\.ref|\.test|node_modules|dist|out|build-final|__pycache__|\.vcpkg-binary-cache)(?:\/|$)/.test(file)) {
    return 'local checkout, test workbench, dependency or build output';
  }
  if (/^(?:\.au3|\.audit2|\.audit_f3633|\.work_[^/]*)(?:\/|$)/.test(file) ||
      /^(?:01,W01p|02,W02p|03,W03p)$/.test(file) ||
      /^ports\/artemis\/(?:rewrite-staging|rewrite-fixes|build\/wabt|reverse\/build)(?:\/|$)/.test(file)) {
    return 'local rewrite, audit or downloaded-toolchain workspace';
  }
  if (/^ports\/artemis\/(?:reverse\/(?:\.build-dbg\.sh|build-bridge-route-o1tmp\.sh)|tools\/(?:apply_part34_arms\.py|check_part34_region\.py|rg2\.mjs|rg-full-console\.mjs))$/.test(file)) {
    return 'local diagnostic scratch script';
  }
  if (ASSET_EXCEPTIONS.has(file)) return null;
  if (/\.(?:wasm|wat|o|obj|so|dll|exe|zip|7z|tgz|pyc|pfs(?:\.\d+)?|xp3|nsa|ns2|sar|rpa)$/i.test(file) ||
      /\.(?:tar\.gz|log|tsbuildinfo)$/i.test(file)) {
    return 'game container, downloaded binary or generated output outside documented exceptions';
  }
  return null;
}

/** Last C++ mapping for an id wins; placeholder and header rows are not products. */
export function mappedProducts(source) {
  const products = new Map();
  source.split(/\r?\n/).forEach((line, index) => {
    if (!line.trim() || line.startsWith('#')) return;
    const cols = line.split('\t');
    if (cols[0] !== 'fn') return;
    if (cols.length < 5 || !/^f\d+$/.test(cols[1])) {
      throw new Error(`rewrite-names.tsv:${index + 1}: malformed function row`);
    }
    const relative = cols[4];
    if (!relative.endsWith('.cpp')) return;
    if (!/^[a-zA-Z0-9_/-]+\.cpp$/.test(relative) ||
        relative.startsWith('/') || relative.split('/').some((part) => part === '..')) {
      throw new Error(`rewrite-names.tsv:${index + 1}: unsafe product path ${relative}`);
    }
    const fnid = `f${BigInt(cols[1].slice(1))}`;
    products.set(fnid, `ports/artemis/src/${relative}`);
  });
  return products;
}

function runGit(root, args, { input, statuses = [0] } = {}) {
  const result = spawnSync('git', args, {
    cwd: root, input, encoding: 'utf8', maxBuffer: 32 * 1024 * 1024,
    stdio: ['pipe', 'pipe', 'pipe'],
  });
  if (result.error) throw result.error;
  if (!statuses.includes(result.status)) {
    throw new Error(`git ${args[0]} exited ${result.status}: ${(result.stderr ?? '').trim()}`);
  }
  return result.stdout;
}

export function inspectSources(root, { staged = false } = {}) {
  const git = (args, options) => runGit(root, args, options);
  const list = (args) => git(args).split('\0').filter(Boolean);
  const tracked = new Set(list(['ls-files', '--cached', '-z']));
  const candidates = staged ? [...tracked] : [...new Set([
    ...tracked, ...list(['ls-files', '--others', '--exclude-standard', '-z']),
  ])];
  const errors = [];
  const warnings = [];
  const pending = new Set();
  const hasFile = (file) => staged ? tracked.has(file) :
    existsSync(join(root, file)) && statSync(join(root, file)).isFile();
  const content = (file) => staged ? git(['show', `:${file}`]) :
    readFileSync(join(root, file), 'utf8');
  const requireFile = (file) => {
    if (!hasFile(file)) {
      errors.push(`${file}: missing from ${staged ? 'index' : 'working tree'}`);
      return false;
    }
    if (!tracked.has(file)) pending.add(file);
    return true;
  };

  for (const file of SOURCE_ENTRIES) requireFile(file);
  for (const file of candidates) {
    const reason = uploadProblem(file);
    if (reason) errors.push(`${file}: ${reason}`);
  }
  if (git(['ls-files', '--unmerged', '-z']).length) {
    return { errors: [...errors, 'Git index has unresolved conflicts'], warnings,
      products: 0, productFiles: 0, pending: [...pending] };
  }

  const mapFile = 'ports/artemis/rewrite-names.tsv';
  const products = hasFile(mapFile) ? mappedProducts(content(mapFile)) : new Map();
  if (products.size === 0) errors.push('Artemis source ledger has no C++ products');
  const paths = [...new Set(products.values())];
  for (const file of paths) requireFile(file);
  if (!staged && paths.length) {
    // check-ignore returns 1 when nothing matches: that is a successful clean check.
    const ignored = git(['check-ignore', '--no-index', '--stdin', '-z'], {
      input: paths.join('\0') + '\0', statuses: [0, 1],
    });
    for (const file of ignored.split('\0').filter(Boolean)) errors.push(`${file}: product is ignored`);
  }

  const manifestFile = 'package.json';
  if (hasFile(manifestFile)) {
    const scripts = JSON.parse(content(manifestFile)).scripts;
    if (scripts?.['check:sources'] !== 'node scripts/check-sources.mjs' ||
        scripts?.['test:sources'] !== 'node --test test/check-sources.test.mjs') {
      errors.push(`${manifestFile}: missing source-check or regression-test entry`);
    }
  }
  for (const name of ['runtime-ons', 'runtime-kag', 'runtime-artemis']) {
    const file = `packages/${name}/package.json`;
    if (!requireFile(file)) continue;
    const pkg = JSON.parse(content(file));
    if (pkg.name !== `@online-gal/${name}` || pkg.private !== true ||
        pkg.license !== 'GPL-3.0-or-later') {
      errors.push(`${file}: runtime name/private/GPL-3.0-or-later declaration changed`);
    }
  }

  const pinsFile = '.github/upstream-pins.json';
  if (hasFile(pinsFile)) {
    const pins = JSON.parse(content(pinsFile));
    if (!pins.emsdk?.version) errors.push(`${pinsFile}: missing emsdk version`);
    for (const name of ['ons', 'krkr']) {
      const engine = pins.engines?.[name];
      if (!engine || !/^[a-f0-9]{40}$/.test(engine.commit ?? '') ||
          !engine.repository?.startsWith('https://github.com/')) {
        errors.push(`${pinsFile}: invalid ${name} pin`);
        continue;
      }
      for (const key of ['build-script', 'workflow']) {
        if (typeof engine[key] !== 'string' || !engine[key].startsWith('.github/')) {
          errors.push(`${pinsFile}: invalid ${name} ${key}`);
        } else requireFile(engine[key]);
      }
    }
    if (pins['artemis-bridge']?.target !== 'asyncify') {
      errors.push(`${pinsFile}: Artemis shipping target must be asyncify`);
    }
    const module = pins['artemis-bridge']?.module;
    if (module !== 'ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.wasm') {
      errors.push(`${pinsFile}: missing or unexpected Artemis asyncify input`);
    } else requireFile(module);
  }

  if (pending.size) warnings.push(`${pending.size} required files exist but are not in the index:\n` +
    [...pending].map((file) => `  ${file}`).join('\n'));
  warnings.push('Retained Artemis SDK and derived products require separate redistribution review; ' +
    'this guard does not establish licensing or game/package acceptance.');
  return { errors, warnings, products: products.size, productFiles: paths.length, pending: [...pending] };
}

function main() {
  const args = process.argv.slice(2);
  if (args.some((arg) => arg !== '--staged')) {
    console.error('usage: node scripts/check-sources.mjs [--staged]');
    process.exitCode = 2;
    return;
  }
  const staged = args.includes('--staged');
  const root = resolve(dirname(fileURLToPath(import.meta.url)), '..');
  try {
    const result = inspectSources(root, { staged });
    for (const warning of result.warnings) console.warn(`source warning: ${warning}`);
    for (const error of result.errors) console.error(`source error: ${error}`);
    console.log(`sources ${result.errors.length ? 'FAILED' : 'ok'} (${staged ? 'index' : 'worktree'}): ` +
      `${result.products} mapped functions, ${result.productFiles} product files`);
    if (result.errors.length) process.exitCode = 1;
  } catch (error) {
    console.error(`source check failed: ${error.message}`);
    process.exitCode = 1;
  }
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) main();
