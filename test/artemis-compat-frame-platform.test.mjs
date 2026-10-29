// Behaviour regression for the compat frame clock. Not a diagnostic self-check.
//
// Contract, read from the shipping sources (not assumed):
//   runtime.tick drops backlog and requests at most one native frame per 1000/60 ms.
//   artc_web_frame() takes no dt. Compositor::Update(now_ms) keys the tween off
//   (now_ms - start_ms), and LuaEngine::NowMs is steady_clock - init_time_.
// A present rate below 60 Hz is a dirty-frame count, not a second clock.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, readFileSync, realpathSync, rmSync, symlinkSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join, resolve } from 'node:path';
import { pathToFileURL, fileURLToPath } from 'node:url';

const root = resolve(fileURLToPath(new URL('..', import.meta.url)));
const temp = mkdtempSync(join(tmpdir(), 'artemis-frame-clock-'));
const out = join(temp, 'out');
process.on('exit', () => {
  if (realpathSync(dirname(temp)) === realpathSync(tmpdir()) && basename(temp).startsWith('artemis-frame-clock-')) {
    rmSync(temp, { recursive: true, force: true });
  }
});
execFileSync(process.execPath, [
  join(root, 'node_modules/typescript/bin/tsc'),
  'packages/engine/core/artemis-compat/src/index.ts',
  '--target', 'ES2022', '--module', 'ESNext', '--moduleResolution', 'bundler',
  '--lib', 'ES2022,DOM', '--strict', '--skipLibCheck',
  '--rootDir', 'packages/engine/core/artemis-compat/src', '--outDir', out,
], { cwd: root, stdio: 'inherit' });
writeFileSync(join(out, 'package.json'), '{"type":"module"}');
symlinkSync(join(root, 'packages/engine/core/artemis-compat/node_modules'), join(out, 'node_modules'), 'junction');

const { CompatArtemisRuntime } = await import(pathToFileURL(join(out, 'compat/runtime.js')).href);

const FRAME = 1000 / 60;

function runtime() {
  const frames = [];
  const host = { requestFrame() {}, log() {}, fatal() {} };
  const rt = new CompatArtemisRuntime(host, {
    gameConfig: { engineBase: 'http://invalid.local/engine/' },
  });
  // The production tick bails before frame() until load+page exist. Those two flags are the
  // gate under test; the page is a counter, not a second clock.
  rt.loaded = true;
  rt.page = { busy: false, frame: () => { frames.push(1); return Promise.resolve(true); } };
  return { rt, frames };
}

test('tick requests one native frame per 60 Hz of host time and drops a stall', () => {
  const { rt, frames } = runtime();
  rt.tick(FRAME - 0.01);
  assert.equal(frames.length, 0, 'a short tick must not request a frame');
  rt.tick(0.02);
  assert.equal(frames.length, 1, 'crossing one interval requests exactly one frame');
  rt.tick(1000);
  assert.equal(frames.length, 2, 'a 1000 ms stall is one frame, not ~60');
  for (let i = 0; i < 120; i += 1) rt.tick(FRAME / 2);
  assert.equal(frames.length, 2 + 60, 'sub-interval ticks sum to one frame per interval');
});

test('tick(dt) never reaches the guest: artc_web_frame takes no time argument', () => {
  const native = readFileSync(
    new URL('../ports/artemis/compat-web/overlay/host/web_main.cpp', import.meta.url), 'utf8');
  const worker = readFileSync(
    new URL('../packages/engine/core/artemis-compat/src/compat/compat-worker.js', import.meta.url), 'utf8');
  assert.match(native, /EMSCRIPTEN_KEEPALIVE int artc_web_frame\(void\)/);
  assert.match(native, /compositor\(\)\.Update\(ctx->lua\(\)\.NowMs\(\)\)/);
  assert.match(worker, /_artc_web_frame: \{ ret: 'number', args: \[\] \}/);
  assert.doesNotMatch(native, /artc_web_frame\([^)]*double/);
});

test('shipped Update(now) is absolute elapsed: 60/39/120 Hz and a stall share one scale', () => {
  const probe = new URL('../ports/artemis/compat-web/tests/fixtures/frame-clock/tween_probe.cpp', import.meta.url);
  const bin = join(temp, 'tween_probe.exe');
  execFileSync('g++', ['-std=c++17', '-O2', '-o', bin, fileURLToPath(probe)], { stdio: 'inherit' });
  const out = execFileSync(bin, { encoding: 'utf8' });
  assert.match(out, /probe-ok/);
  assert.match(out, /stall-same-absolute/);
  for (const line of out.split('\n')) {
    const m = line.match(/^(60Hz|39Hz|120Hz) .* end=([\d.]+)/);
    if (!m) continue;
    assert.equal(Number(m[2]), 100, `${m[1]} must finish a 2000 ms yscale at 100`);
  }
});

test('frame takes no dt; now_ms stays the HEAD query, not a second clock', () => {
  const worker = readFileSync(
    new URL('../packages/engine/core/artemis-compat/src/compat/compat-worker.js', import.meta.url), 'utf8');
  assert.match(worker, /_artc_web_frame: \{ ret: 'number', args: \[\] \}/);
  assert.match(worker, /_artc_web_now_ms: \{ ret: 'number', args: \[\] \}/);
  assert.match(worker, /function readNowMs\(/);
  const frame = worker.slice(worker.indexOf("request.type === 'frame'"), worker.indexOf("request.type === 'audio-pull'"));
  assert.match(frame, /nowMs: readNowMs\(\)/);
  assert.doesNotMatch(frame, /request\.dt|event\.dt/);
});
