/**
 * Type-checks the browser shims (`node packages/cf-browser/scripts/typecheck.mjs`).
 *
 * The check follows upstream's own relative type imports (e.g. `lib/resolve.ts` imports the
 * `Cloudflare` type from its `./auth.js`), which drags in upstream modules written for Node with
 * dependencies this package does not install. Those are not ours to fix and the build never
 * bundles them, so only diagnostics in this package fail the check.
 */
import { spawnSync } from 'node:child_process';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { createRequire } from 'node:module';

const pkgRoot = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const tsc = createRequire(join(pkgRoot, '..', '..', 'package.json')).resolve('typescript/bin/tsc');
const run = spawnSync(process.execPath, [tsc, '-p', pkgRoot, '--pretty', 'false'], { encoding: 'utf8' });
const lines = `${run.stdout}${run.stderr}`.split(/\r?\n/);

const ours = [];
let upstream = 0;
for (const line of lines) {
  if (!/error TS\d+/.test(line)) continue;
  if (/(^|[\\/])\.ref[\\/]/.test(line)) upstream++;
  else ours.push(line);
}

if (ours.length > 0) {
  console.error(ours.join('\n'));
  console.error(`\ncf-browser: ${ours.length} type error(s) in this package`);
  process.exit(1);
}
console.log(`cf-browser: types ok (${upstream} diagnostics in upstream modules ignored)`);
