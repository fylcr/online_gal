/**
 * Native scene-restore regression.
 *
 * The failure it pins: SaveSnapshot writes an ARCV variable bank, LoadSnapshot
 * releases the compositor, and a slot with no layer record comes back as a
 * black stage. The check compiles the patched pin and runs the real
 * SaveSnapshot / LoadSnapshot against a generated 1x1 pack.
 *
 * Tier 1 always runs. Tier 2 needs g++ plus the extracted pin and is skip,
 * never a pass, when either is absent.
 */
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { existsSync, readFileSync } from 'node:fs';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import test from 'node:test';

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..', '..', '..');
const PATCH = join(ROOT, 'ports/artemis/compat-web/patches/0003-scene-checkpoint.patch');
const FIX = join(ROOT, '.test/artemis-compact-scene-restore-fix');

test('the scene patch is the ARCL trailer and nothing else', () => {
  const text = readFileSync(PATCH, 'utf8');
  const added = text.split('\n').filter((line) => line.startsWith('+')).join('\n');
  assert.match(added, /ARCL/);
  assert.match(added, /EncodeSceneCommands/);
  assert.match(added, /scene_journal_/);
  assert.doesNotMatch(added, /EncodeBow|SerializeVm/);
  for (const file of ['build.sh', 'RUN-BUILD.sh']) {
    assert.match(readFileSync(join(ROOT, 'ports/artemis/compat-web', file), 'utf8'),
      /0003-scene-checkpoint\.patch/);
  }
  assert.match(readFileSync(join(ROOT, 'ports/artemis/compat-web/make-source-bundle.mjs'), 'utf8'),
    /0003-scene-checkpoint\.patch/);
});

test('the native probe restores the saved room', (t) => {
  if (!existsSync(join(FIX, 'probe.cpp')) || !existsSync(join(FIX, 'pin-src/src/script/lua_engine.cpp'))) {
    t.skip('probe sources are local evidence, not in this checkout');
    return;
  }
  if (spawnSync('g++', ['--version']).status !== 0) {
    t.skip('g++ is absent; the compiled probe was not run');
    return;
  }
  const built = spawnSync('bash', [join(FIX, 'build-probe.sh')], { cwd: ROOT, encoding: 'utf8', timeout: 180000 });
  assert.equal(built.status, 0, built.stderr || built.stdout);
  const run = spawnSync(join(FIX, 'probe-out/probe.exe'), [], { encoding: 'utf8', timeout: 30000 });
  assert.equal(run.status, 0, run.stdout + run.stderr);
  assert.match(run.stdout, /failures=0/);
  assert.match(run.stderr, /scene=2/);
});
