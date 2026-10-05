/**
 * Real Lua l_tag + DispatchTag + TagLyTween + Compositor, GLES calls at a recorder boundary.
 * Compiles pin 17200e64 from a pristine git archive. GL is the managed recorder.
 * Missing pin, g++, or the declared zlib is a fail, never a skip.
 * Run: node --test ports/artemis/compat-web/tests/artemis-compat-animation-observe.test.mjs
 */
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { cpSync, existsSync, mkdirSync, readdirSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join, relative, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import test from 'node:test';
import { DEFAULT_PIN, assertExtractDir } from '../make-source-bundle.mjs';

const HERE = dirname(fileURLToPath(import.meta.url));
const ROOT = resolve(HERE, '..', '..', '..', '..');
const PORT = join(ROOT, 'ports/artemis/compat-web');
const FIX = join(HERE, 'fixtures/animation-observe');
const EVIDENCE = join(ROOT, '.' + 'test', 'artemis-animation-observe-portable');
const CORE = [
  'src/config/ini.cpp', 'src/log/logger.cpp', 'src/pack/pack_manager.cpp', 'src/pack/pf8_reader.cpp',
  'src/pack/psb.cpp', 'src/render/bc7decomp.cpp', 'src/render/block_decode.cpp', 'src/render/compositor.cpp',
  'src/render/emote_mesh.cpp', 'src/render/emote_model.cpp', 'src/render/emote_player.cpp',
  'src/render/emote_scene.cpp', 'src/render/glyph_atlas.cpp', 'src/render/hlsl_glsl.cpp',
  'src/render/layer_shader.cpp', 'src/render/renderer.cpp', 'src/render/video_decoder.cpp',
  'src/render/video_player.cpp', 'src/save/native_save.cpp', 'src/save/save_metadata.cpp',
  'src/script/asb_parser.cpp', 'src/script/expression.cpp', 'src/script/iet_interpreter.cpp',
  'src/script/iet_parser.cpp', 'src/script/lua_engine.cpp', 'src/script/lua_tag_dispatch.cpp',
  'src/script/lua_tags_audio.cpp', 'src/script/lua_tags_input.cpp', 'src/script/lua_tags_layer.cpp',
  'src/script/lua_tags_nav.cpp', 'src/script/lua_tags_text.cpp', 'src/script/lua_tags_var.cpp',
  'src/script/pluto_codec.cpp', 'src/script/preprocess.cpp', 'src/audio/audio.cpp',
  'src/audio/audio_channels.cpp', 'src/audio/vorbis_stream.cpp', 'src/util/encoding.cpp',
  'src/util/save_storage.cpp', 'src/util/snapshot_image.cpp', 'src/util/text.cpp',
];
const STUBS = ['src/core/engine_context.cpp', 'src/input/input_state.cpp', 'src/cli/artc.cpp', 'src/cli/host_drive.cpp', 'src/jni/jni_bridge.cpp', 'src/jni/native_activity.cpp', 'src/platform/ohos/audio_ohos.cpp', 'src/host/web_main.cpp'];

function log(name, result) {
  writeFileSync(join(EVIDENCE, name), `EXIT:${result.status}\n${result.stdout ?? ''}\n${result.stderr ?? ''}`);
}

function run(command, args, opts = {}) {
  const result = spawnSync(command, args, { encoding: 'utf8', maxBuffer: 64 * 1024 * 1024, ...opts });
  if (result.error) throw result.error;
  return result;
}

function repoOf() {
  const override = process.env.ARTC_REF || process.env.ARTC_SOURCE_REPO;
  const candidates = [
    join(ROOT, '.ref/artemis-compat'),
    join(ROOT, '.ref/artemis-compat-refresh/src'),
  ];
  if (override) candidates.unshift(override);
  for (const repo of candidates) {
    if (run('git', ['-C', repo, 'cat-file', '-e', `${DEFAULT_PIN}^{commit}`]).status === 0) return repo;
  }
  return null;
}

test('shared observer compiles the pin, keeps both real paths and separates CG fields', () => {
  assert.equal(DEFAULT_PIN, '17200e64ea99681e3c27d694433c0ff78d7a7cee');
  for (const name of ['gl2.h', 'glstub.h', 'probe.cpp']) assert.equal(existsSync(join(FIX, name)), true, name);
  const cxx = process.env.CXX || 'g++';
  const zlibRoot = process.env.ZLIB_ROOT || '';
  const zlibFlags = zlibRoot ? [`-I${join(zlibRoot, 'include')}`, `-L${join(zlibRoot, 'lib')}`] : [];
  const gxx = run(cxx, ['--version']);
  assert.equal(gxx.status, 0, 'g++ is required; a missing compiler is not a skip');
  const zlib = run(cxx, ['-c', ...zlibFlags, '-x', 'c++', '-o', join(tmpdir(), 'artc-zlib-probe.o'), '-'], {
    input: '#include <zlib.h>\n',
  });
  assert.equal(zlib.status, 0, 'zlib (libz-dev, or ZLIB_ROOT) is a declared build dependency; a missing header is not a skip\n' + (zlib.stderr || ''));
  const repo = repoOf();
  assert.ok(repo, `pin ${DEFAULT_PIN} is not in ARTC_REF or the local pristine checkouts; that is a missing upstream source, not a skip`);

  rmSync(EVIDENCE, { recursive: true, force: true });
  mkdirSync(EVIDENCE, { recursive: true });
  const work = assertExtractDir(ROOT, join(EVIDENCE, 'source'));
  mkdirSync(work, { recursive: true });
  const tar = join(EVIDENCE, 'pin.tar');
  const archived = run('git', ['-C', repo, 'archive', '--format=tar', '-o', tar, DEFAULT_PIN]);
  assert.equal(archived.status, 0, archived.stderr);
  const extracted = run('tar', ['-xf', '../pin.tar'], { cwd: work });
  assert.equal(extracted.status, 0, extracted.stderr);
  rmSync(tar, { force: true });
  const head = run('git', ['-C', repo, 'rev-parse', `${DEFAULT_PIN}^{commit}`]);
  writeFileSync(join(EVIDENCE, 'source-identity.txt'), `pin=${head.stdout.trim()}\nrepo=${repo}\n`);

  const names = readFileSync(join(PORT, 'PLATFORM-PATCHES.txt'), 'utf8').split(/\r?\n/)
    .map((line) => line.replace(/#.*$/, '').trim()).filter(Boolean);
  assert.equal(names.length, 4);
  for (const name of names) {
    const file = join(PORT, 'patches', name);
    const dry = run('git', ['apply', '--check', '-p1', file], { cwd: work });
    log(`apply-${name}.log`, dry);
    assert.equal(dry.status, 0, dry.stderr);
    assert.equal(run('git', ['apply', '-p1', file], { cwd: work }).status, 0);
    const repeat = run('git', ['apply', '--check', '-p1', file], { cwd: work });
    log(`reject-${name}.log`, repeat);
    assert.notEqual(repeat.status, 0, `${name} must reject a second apply`);
  }
  mkdirSync(join(work, 'GLES2'), { recursive: true });
  cpSync(join(FIX, 'gl2.h'), join(work, 'GLES2/gl2.h'));
  cpSync(join(FIX, 'glstub.h'), join(work, 'glstub.h'));
  const pack = readFileSync(join(work, 'src/pack/pack_manager.cpp'), 'utf8')
    .replaceAll('provider_.read(path, out)', 'provider_.read(path.string(), out)')
    .replaceAll('provider_.openRead(path)', 'provider_.openRead(path.string())');
  assert.notEqual(pack, readFileSync(join(work, 'src/pack/pack_manager.cpp'), 'utf8'));
  writeFileSync(join(work, 'src/pack/pack_manager.cpp'), pack);
  const open = readFileSync(join(work, 'src/pack/pf8_reader.cpp'), 'utf8');
  const at = open.indexOf('bool Pf8Reader::Open(const std::string &path, const std::vector<uint8_t> &key) {');
  assert.ok(at > 0);
  const next = open.indexOf('\nbool ', at + 10);
  assert.ok(next > at);
  writeFileSync(join(work, 'src/pack/pf8_reader.cpp'), open.slice(0, at) +
    'bool Pf8Reader::Open(const std::string &path, const std::vector<uint8_t> &) { path_ = path; return true; }\n' +
    open.slice(next));
  writeFileSync(join(work, 'glstate.cpp'), '#include "glstub.h"\nstd::vector<Call> g_calls; float g_ptr[64]; int g_stride=0,g_teximage=0; std::map<unsigned,std::pair<int,int>> g_texsize; unsigned g_bound=0;\n');

  const includes = ['-Isrc', '-Isrc/render', '-Ithird_party/lua-5.1.5/src', '-Ithird_party', '-I.'];
  const flags = ['-std=c++17', '-O1', '-DARTC_HAS_GLES=1', '-include', 'glstub.h', ...includes, ...zlibFlags];
  const objects = [];
  const compiled = [];
  const cc = (src, obj) => {
    const result = run(cxx, [...flags, '-c', src, '-o', obj], { cwd: work });
    log(`compile-${obj}.log`, result);
    assert.equal(result.status, 0, `${src}\n${result.stderr || result.stdout}`);
    objects.push(obj);
    compiled.push(src);
  };
  for (const src of CORE) cc(src, src.replaceAll('/', '_').replace('.cpp', '.o'));
  const lua = readdirSync(join(work, 'third_party/lua-5.1.5/src')).filter((n) => n.endsWith('.c') && n !== 'lua.c' && n !== 'luac.c').sort();
  const ccLua = process.env.CC || 'gcc';
  for (const name of lua) {
    const src = `third_party/lua-5.1.5/src/${name}`;
    const obj = `lua_${name}.o`;
    const result = run(ccLua, ['-std=c99', '-O1', '-c', src, '-o', obj, '-Ithird_party/lua-5.1.5/src'], { cwd: work });
    log(`compile-${obj}.log`, result);
    assert.equal(result.status, 0, `${src}\n${result.stderr || result.stdout}`);
    objects.push(obj);
    compiled.push(src);
  }
  const symbol = run('nm', ['src_render_compositor.o'], { cwd: work });
  const tween = symbol.stdout.match(/\bT (\S*Compositor8AddTween\S*)/);
  assert.ok(tween, symbol.stdout);
  const probe = readFileSync(join(FIX, 'probe.cpp'), 'utf8').replaceAll('@TWEEN@', tween[1]);
  writeFileSync(join(work, 'probe.cpp'), probe);
  cc('glstate.cpp', 'glstate.o');
  cc('probe.cpp', 'probe.o');
  writeFileSync(join(EVIDENCE, 'compiled.txt'), `compiled=${compiled.length}\nstubs=${STUBS.join(' ')}\n${compiled.join('\n')}\n`);

  const bin = 'probe.exe';
  const linked = run(cxx, ['-o', bin, ...objects, `-Wl,--wrap=${tween[1]}`, ...zlibFlags, '-lz'], { cwd: work });
  log('link.log', linked);
  assert.equal(linked.status, 0, linked.stderr || linked.stdout);
  const bare = { ...process.env };
  delete bare.ARTC_ANIMATION_TRACE;
  const rows = [];
  for (const [label, env] of [['off', { ...bare, ARTC_ANIMATION_TRACE: '0' }], ['bare', bare], ['on', { ...bare, ARTC_ANIMATION_TRACE: '1' }]]) {
    const result = run(join(work, bin), [], { cwd: work, env });
    log(`run-${label}.log`, result);
    assert.equal(result.status, 0, result.stderr || result.stdout);
    rows.push(result);
  }
  const lines = (result) => result.stderr.split(/\r?\n/).filter((line) => line.includes('anim seq='));
  assert.equal(lines(rows[0]).length, 0);
  assert.equal(lines(rows[1]).length, 0);
  const on = lines(rows[2]);
  assert.equal(on.length, 11, on.join('\n'));
  const repeats = on.filter((line) => line.includes('tag=lytween') && line.includes('id="repeat"'));
  assert.equal(repeats.length, 7);
  assert.ok(repeats.every((line) => line.includes('pc=unknown') && line.includes('script="unknown"')));
  assert.match(repeats[6], /waiting=1 timed_wait=1/);
  assert.ok(on.some((line) => line.includes('script="synthetic.iet"') && /pc=\d+/.test(line)));
  assert.ok(on.some((line) => line.includes('resource="image/cg\\nresource"')));
  const cg = on.find((line) => line.includes('tag=lyprop'));
  for (const field of ['xscale="125"', 'yscale="75"', 'zoom="110"', 'anchorx="12"', 'anchory="34"', 'clip="0,0,1,1"', 'alpha="128"']) {
    assert.ok(cg.includes(field), field);
  }
  assert.ok(on.some((line) => line.includes('tag=trans') && line.includes('phase=dispatch-start') && line.includes('time="100"')));
  assert.ok(on.every((line) => Buffer.byteLength(line) < 2304));
  assert.ok(on.find((line) => line.includes('id="bounded"')).includes('~"'));
  const semantics = rows.map((row) => row.stdout.match(/SEMANTICS .+/)[0]);
  assert.equal(semantics[0], semantics[1]);
  assert.equal(semantics[0], semantics[2]);
  assert.deepEqual(on.map((line) => Number(line.match(/seq=(\d+)/)[1])), Array.from({ length: 11 }, (_, i) => i + 1));
  writeFileSync(join(EVIDENCE, 'native-result.txt'), `off=0 bare=0 on=11 Lua=3 DispatchTag=3 wait=1 CG=2 trans=1 bounded=1\n${semantics[0]}\nGLES=managed-recorder; Lua/TagLyTween/AddTween/Update=real; objects compiled from ${DEFAULT_PIN}\nunmeasured-stubs=${STUBS.join(',')}\n`);
});
