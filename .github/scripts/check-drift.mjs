#!/usr/bin/env node
/**
 * Compare the pinned upstream commits in `.github/upstream-pins.json` against the live remotes.
 *
 * WHY THIS NEEDS TO BE A SCRIPT
 *
 * The check is three things at once: read a JSON file, ask each upstream for the tip of one branch
 * with `git ls-remote`, and emit a value a workflow can use as a matrix. Inline shell would have to
 * do all three without a JSON parser, and the piece that goes wrong silently is the last one -- a
 * matrix that emits `[]` when it meant "nothing moved" versus `[]` because the parse failed, which
 * is why a missing or unreadable `--out` file is an error here rather than an empty result.
 *
 * WHY IT DOES NOT WRITE THE PIN
 *
 * Moving a pin is a deliberate act: the commit recorded in the pins file is the one the published
 * bytes were built from, and the manifest a consumer verifies against names that commit. A bot that
 * silently advanced it would make the published manifest describe a commit nobody reviewed. So this
 * reports, and a human edits the file.
 *
 * Usage:
 *   node .github/scripts/check-drift.mjs                       human-readable report
 *   node .github/scripts/check-drift.mjs --out drift.json      matrix JSON (only when something moved)
 *   node .github/scripts/check-drift.mjs --markdown            GitHub step summary
 *
 * Exit codes: 0 nothing moved or report printed · 1 usage/config error · 2 something moved (only
 * with --exit-code-moved; used so a shell can branch without parsing output).
 */
import { execFileSync } from 'node:child_process';
import { existsSync, readFileSync, writeFileSync } from 'node:fs';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const HERE = dirname(fileURLToPath(import.meta.url));
const REPO_ROOT = resolve(HERE, '..', '..');
const PINS = join(REPO_ROOT, '.github', 'upstream-pins.json');

const options = { out: null, markdown: false, exitCodeMoved: false };
const argv = process.argv.slice(2);
for (let index = 0; index < argv.length; index += 1) {
  const arg = argv[index];
  if (arg === '--out') {
    options.out = argv[index + 1];
    if (!options.out) {
      process.stderr.write('check-drift: --out needs a path\n');
      process.exit(1);
    }
    index += 1;
  } else if (arg === '--markdown') {
    options.markdown = true;
  } else if (arg === '--exit-code-moved') {
    options.exitCodeMoved = true;
  } else if (arg === '--help' || arg === '-h') {
    process.stdout.write('usage: node .github/scripts/check-drift.mjs [--out FILE] [--markdown] [--exit-code-moved]\n');
    process.exit(0);
  } else {
    process.stderr.write(`check-drift: unknown argument ${arg}\n`);
    process.exit(1);
  }
}

if (!existsSync(PINS)) {
  process.stderr.write(`check-drift: no pins file at ${PINS}\n`);
  process.exit(1);
}

let pins;
try {
  pins = JSON.parse(readFileSync(PINS, 'utf8'));
} catch (error) {
  process.stderr.write(`check-drift: cannot parse ${PINS}: ${error.message}\n`);
  process.exit(1);
}

if (!pins.engines || typeof pins.engines !== 'object') {
  process.stderr.write(`check-drift: ${PINS} has no "engines" object\n`);
  process.exit(1);
}

/** A synchronous pause; `sleep` is not a command on every platform this runs on. */
function pause(ms) {
  Atomics.wait(new Int32Array(new SharedArrayBuffer(4)), 0, 0, ms);
}

/**
 * The tip of one branch, via `git ls-remote`.
 *
 * `--exit-code` makes a reference that does not exist a non-zero exit instead of an empty line,
 * which matters because a renamed branch upstream would otherwise look like "nothing moved".
 *
 * Retried three times with a growing pause. A TLS connection that drops mid-handshake is common
 * enough that a single attempt makes the job flaky, and the retry is confined to the transport:
 * a genuinely missing branch fails `--exit-code` every time and still surfaces as an error.
 */
function remoteHead(repository, branch) {
  let lastError;
  for (let attempt = 1; attempt <= 3; attempt += 1) {
    try {
      const output = execFileSync('git', ['ls-remote', '--exit-code', repository, `refs/heads/${branch}`], {
        encoding: 'utf8',
        stdio: ['ignore', 'pipe', 'pipe'],
        timeout: 60_000,
      });
      const line = output.split('\n').find((entry) => entry.trim() !== '');
      if (!line) throw new Error('ls-remote returned no matching ref');
      return line.split(/\s+/)[0];
    } catch (error) {
      lastError = error;
      if (attempt < 3) pause(attempt * 4000);
    }
  }
  throw lastError;
}

const entries = Object.entries(pins.engines);
const moved = [];
const checked = [];

for (const [name, engine] of entries) {
  if (!engine.repository || !engine.branch || !engine.commit) {
    process.stderr.write(`check-drift: engine "${name}" is missing repository/branch/commit\n`);
    process.exit(1);
  }
  let head;
  try {
    head = remoteHead(engine.repository, engine.branch);
  } catch (error) {
    // A network or auth failure is NOT drift. Reporting it as drift would dispatch a rebuild
    // against a commit nobody verified, so this fails the run instead.
    process.stderr.write(`check-drift: cannot read ${name} (${engine.repository} ${engine.branch}): ${error.message}\n`);
    process.exit(1);
  }

  const state = head === engine.commit ? 'current' : 'moved';
  checked.push({ name, state, pinned: engine.commit, head, branch: engine.branch });
  if (state === 'moved') {
    moved.push({
      name,
      workflow: engine.workflow,
      branch: engine.branch,
      pinned: engine.commit,
      head,
    });
  }
}

if (options.out) {
  // Written only when something moved, so the workflow's `if [ -f drift.json ]` is itself the
  // answer and does not have to parse anything.
  if (moved.length > 0) writeFileSync(options.out, `${JSON.stringify(moved, null, 2)}\n`);
}

if (options.markdown) {
  const lines = ['## Upstream drift', ''];
  if (moved.length === 0) {
    lines.push('No engine is behind its upstream. Nothing to rebuild.', '');
  } else {
    lines.push(`${moved.length} engine(s) have moved past the pinned commit:`, '');
    lines.push('| engine | branch | pinned | upstream head |', '| --- | --- | --- | --- |');
    for (const item of moved) {
      lines.push(`| ${item.name} | ${item.branch} | \`${item.pinned}\` | \`${item.head}\` |`);
    }
    lines.push('', 'Rebuilds have been dispatched for these engines. To make the new commit the pin,');
    lines.push('edit `.github/upstream-pins.json`, **verify the rebuilt artifacts**, and update');
    lines.push('`ports/lib/engine-artifacts.json` from the new release manifest.');
  }
  lines.push('', '<details><summary>All pins</summary>', '');
  for (const item of checked) {
    lines.push(`- **${item.name}** (${item.branch}): ${item.state} — pinned \`${item.pinned}\`, head \`${item.head}\``);
  }
  lines.push('', '</details>');
  process.stdout.write(`${lines.join('\n')}\n`);
} else {
  process.stdout.write(`upstream drift: ${moved.length} of ${checked.length} engine(s) moved\n`);
  for (const item of checked) {
    process.stdout.write(`  ${item.state.padEnd(8)} ${item.name.padEnd(8)} pinned ${item.pinned.slice(0, 12)}  head ${item.head.slice(0, 12)}\n`);
  }
  if (moved.length > 0) {
    process.stdout.write('\nnew commits to review:\n');
    for (const item of moved) {
      process.stdout.write(`  ${item.name.padEnd(8)} ${item.pinned} -> ${item.head}\n`);
    }
  }
}

process.exit(options.exitCodeMoved && moved.length > 0 ? 2 : 0);
