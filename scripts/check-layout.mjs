/**
 * Enforces the repository's layout rule:
 *
 *   **emulation/port code lives in the repository** (`packages/`, `apps/`, `ports/`),
 *   **only tests, game data and local scratch live in the gitignored `.test/`.**
 *
 * The rule exists because of a real, expensive failure: the entire WASM port — build
 * scripts, the Android shim, harnesses, even the engine patch — once lived under
 * `.test/`, which is gitignored. Nothing could be pushed, and a fresh clone could not
 * build anything at all. The fix was to move that code to `ports/`; this check is what
 * keeps it from drifting back.
 *
 * What it actually catches is a committed file depending on `.test/`. Comments are
 * stripped first, so prose that *explains* the split (there is some) does not trip it.
 * Only tracked files are inspected, so a clone without `.test/` passes trivially.
 */
import { execFileSync } from 'node:child_process';
import { readFileSync } from 'node:fs';

const ROOTS = ['packages/', 'apps/', 'ports/'];
const CODE = /\.(?:ts|tsx|mts|cts|js|jsx|mjs|cjs|sh|bash|py|rs|go|java|kt|c|h|cpp|hpp)$/i;

/** Strips `//`, `/* *\/`, `#` and `<!-- -->` comments so only real references remain. */
function stripComments(source) {
  return source
    .replace(/\/\*[\s\S]*?\*\//g, ' ')
    .replace(/<!--[\s\S]*?-->/g, ' ')
    .replace(/(^|[^:])\/\/[^\n]*/g, '$1')
    .replace(/(^|\s)#[^\n]*/g, '$1');
}

const tracked = execFileSync('git', ['ls-files', '-z'], { encoding: 'utf8' })
  .split('\0')
  .filter(Boolean);

const offences = [];

for (const file of tracked) {
  if (!ROOTS.some((root) => file.startsWith(root))) continue;
  if (!CODE.test(file)) continue;

  const body = stripComments(readFileSync(file, 'utf8'));
  body.split('\n').forEach((line, index) => {
    if (line.includes('.test/')) {
      offences.push(`${file}:${index + 1}: ${line.trim()}`);
    }
  });
}

if (offences.length > 0) {
  console.error(
    'Layout violation: committed code must not depend on `.test/`.\n\n' +
      '`.test/` is gitignored and holds only tests, game data and local scratch. Port and\n' +
      'emulation code belongs under `packages/`, `apps/` or `ports/`, so that a clone can\n' +
      'build it. Move the referenced code to its proper home and update the reference.\n',
  );
  for (const offence of offences) console.error(`  ${offence}`);
  console.error('');
  process.exit(1);
}

console.log(`layout ok: no \`.test/\` dependency in ${ROOTS.join(', ')}`);
