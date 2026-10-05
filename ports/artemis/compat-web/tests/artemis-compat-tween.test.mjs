/**
 * loopdelay on the pinned Compositor, not a rewritten tween.
 *
 * Applies 0001+0002+0003 onto a copy of the pin, then calls the real
 * AddTween / Update. GL is the recorder shim. Missing pin or g++ is a fail.
 * Title 150→100 has no loop; this does not claim that replay is fixed.
 */
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { existsSync, mkdirSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { dirname, join, relative, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import test from 'node:test';
import { DEFAULT_PIN, assertExtractDir } from '../make-source-bundle.mjs';

const HERE = dirname(fileURLToPath(import.meta.url));
const ROOT = resolve(HERE, '..', '..', '..', '..');
const PORT = join(ROOT, 'ports/artemis/compat-web');
const LIST = join(PORT, 'PLATFORM-PATCHES.txt');

const SHIM = String.raw`#include "pack/pack_manager.h"
#include "render/compositor.h"
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <map>
#include <string>
#include <vector>
struct Call { int fn; float f[8]; };
static std::vector<Call> g_calls;
#define REC(c) g_calls.push_back(c)
extern "C" {
void glDrawArrays(int,int,int){ Call c{}; c.fn=1; REC(c); }
void glViewport(int,int,int,int){}
void glScissor(int,int,int,int){}
void glBindFramebuffer(int,unsigned){}
void glBindTexture(int,unsigned){}
void glUniform2f(int,float,float){}
void glUniform1f(int,float){}
void glVertexAttribPointer(unsigned,int,unsigned,unsigned char,int,const void*){}
void glTexImage2D(int,int,int,int,int,int,int,int,const void*){}
void glReadPixels(int,int,int,int,int,int,void*){}
void glFinish(){}
void glFlush(){}
void glGenTextures(int n, unsigned* o){ static unsigned s=10; for(int i=0;i<n;++i) o[i]=s++; }
void glGenFramebuffers(int n, unsigned* o){ static unsigned s=1; for(int i=0;i<n;++i) o[i]=s++; }
void glDeleteTextures(int,const unsigned*){}
void glDeleteFramebuffers(int,const unsigned*){}
void glFramebufferTexture2D(int,int,int,unsigned,int){}
unsigned glCheckFramebufferStatus(int){ return 0x8CD5; }
void glGetIntegerv(int p, int* o){ if(p==0x0BA2){ o[0]=0;o[1]=0;o[2]=1280;o[3]=720; } else *o=0; }
void glTexParameteri(int,int,int){}
void glTexSubImage2D(int,int,int,int,int,int,int,int,const void*){}
void glClear(unsigned){}
void glClearColor(float,float,float,float){}
void glEnable(int){}
void glDisable(int){}
void glBlendFuncSeparate(int,int,int,int){}
void glUseProgram(unsigned){}
void glActiveTexture(int){}
void glUniform1i(int,int){}
void glEnableVertexAttribArray(unsigned){}
void glDisableVertexAttribArray(unsigned){}
void glVertexAttrib1f(unsigned,float){}
unsigned glCreateProgram(){ return 1; }
unsigned glCreateShader(unsigned){ return 1; }
void glShaderSource(unsigned,int,const char* const*,const int*){}
void glCompileShader(unsigned){}
void glAttachShader(unsigned,unsigned){}
void glBindAttribLocation(unsigned,unsigned,const char*){}
void glLinkProgram(unsigned){}
void glDeleteShader(unsigned){}
void glDeleteProgram(unsigned){}
void glPixelStorei(unsigned,int){}
int glGetAttribLocation(unsigned,const char* n){ return n[2]=='p'?0:n[2]=='u'?1:2; }
int glGetUniformLocation(unsigned,const char* n){ return n[2]=='s'?10:13; }
int glGetError(){ return 0; }
void glCopyTexImage2D(int,int,int,int,int,int,int,int){}
}
namespace artc {
void Log(int, const std::string&) {}
bool PackManager::Read(const std::string&, std::vector<uint8_t>&) const { return false; }
std::string SavePath(const std::string&, const std::string&) { return {}; }
bool ReadSaveFile(const std::string&, std::vector<uint8_t>&) { return false; }
void LayerEffect::Set(const std::map<std::string,std::string>&) {}
bool LayerShaders::Load(const std::string&, const std::string&) { return false; }
void LayerShaders::ReleaseGl() {}
uint32_t LayerShaders::Begin(size_t, int, int, uint32_t, bool) { return 0; }
bool LayerShaders::End(size_t, const LayerEffect&, uint32_t, bool, float,
    const std::vector<std::pair<std::string,uint32_t>>&, const LayerCoverage&) { return false; }
}
static int g_fail = 0;
static void check(const char* name, bool ok) {
    std::printf("%s %s\n", ok ? "PASS" : "FAIL", name);
    if (!ok) ++g_fail;
}
static float left_of(artc::Compositor& c) {
    for (const auto& l : c.Layers()) if (l.id == "d") return l.x;
    return -9999;
}
static float scale_of(artc::Compositor& c) {
    for (const auto& l : c.Layers()) if (l.id == "500.ch.2") return l.sx * 100.f;
    return -9999;
}
static void layer(artc::Compositor& c, const char* id) {
    uint8_t px[4] = {255,0,0,255};
    c.SetPixels(id, px, 1, 1);
}
int main() {
    artc::Compositor c;
    c.Init(1280, 720);
    layer(c, "d");
    c.SetProps("d", {{"w","100"},{"h","100"},{"left","0"}});
    c.AddTween("d", {{"param","left"},{"from","0"},{"to","100"},{"time","1000"},
                     {"loop","2"},{"loopdelay","500"}}, 0);
    c.Update(1000); const float a = left_of(c);
    c.Update(1100); const float b = left_of(c);
    c.Update(1400); const float d = left_of(c);
    const double pend_gap = c.PendingAnimationMs(1400);
    c.Update(1499); const float e = left_of(c);
    c.Update(1500); const float f = left_of(c);
    c.Update(2000); const float g = left_of(c);
    c.Update(2500); const float h = left_of(c);
    c.Update(2999); const float i = left_of(c);
    c.Update(3000); const float j = left_of(c);
    c.Update(4000); const float k = left_of(c);
    const double pend_done = c.PendingAnimationMs(4000);
    std::printf("hold %g %g %g %g restart %g mid2 %g end2 %g gap2 %g end3 %g after %g\n",
                a,b,d,e,f,g,h,i,j,k);
    check("boundary t=1000 is 100", std::fabs(a-100)<1);
    check("gap t=1100 stays 100", std::fabs(b-100)<1);
    check("gap t=1400 stays 100", std::fabs(d-100)<1);
    check("gap t=1499 stays 100", std::fabs(e-100)<1);
    check("pass2 t=1500 restarts at 0", std::fabs(f)<1);
    check("pass2 t=2000 is 50", std::fabs(g-50)<1);
    check("pass2 t=2500 is 100", std::fabs(h-100)<1);
    check("gap2 t=2999 stays 100", std::fabs(i-100)<1);
    check("pass3 t=3000 restarts at 0", std::fabs(j)<1);
    check("done t=4000 stays 100", std::fabs(k-100)<1);
    std::printf("pending %g %g\n", pend_gap, pend_done);
    check("pending at 1400 still has pass 3", pend_gap > 1000);
    check("pending at 4000 is 0", pend_done == 0);

    artc::Compositor z;
    z.Init(1280, 720);
    layer(z, "d");
    z.SetProps("d", {{"w","100"},{"h","100"},{"left","0"}});
    z.AddTween("d", {{"param","left"},{"from","0"},{"to","100"},{"time","0"},
                     {"loop","2"},{"loopdelay","500"}}, 0);
    z.Update(0);
    check("zero time lands on 100", std::fabs(left_of(z)-100)<1);
    check("zero time pending is 0", z.PendingAnimationMs(0) == 0);

    artc::Compositor w;
    w.Init(1280, 720);
    layer(w, "d");
    w.SetProps("d", {{"w","100"},{"h","100"},{"left","9"}});
    w.AddTween("d", {{"param","left"},{"from","0"},{"to","100"},{"time","1000"},
                     {"delay","400"},{"loop","1"},{"loopdelay","200"}}, 0);
    w.Update(399);
    check("initial delay holds the prop", std::fabs(left_of(w)-9)<1);
    w.Update(400);
    check("motion starts after delay", std::fabs(left_of(w))<1);

    artc::Compositor y;
    y.Init(1280, 720);
    layer(y, "d");
    y.SetProps("d", {{"w","100"},{"h","100"},{"left","0"}});
    y.AddTween("d", {{"param","left"},{"from","0"},{"to","100"},{"time","1000"},
                     {"yoyo","1"},{"loopdelay","500"}}, 0);
    y.Update(1000); const float y1 = left_of(y);
    y.Update(1499); const float y2 = left_of(y);
    y.Update(1500); const float y3 = left_of(y);
    y.Update(2000); const float y4 = left_of(y);
    y.Update(2500); const float y5 = left_of(y);
    std::printf("yoyo %g %g %g %g %g\n", y1,y2,y3,y4,y5);
    check("yoyo ends pass1 at 100", std::fabs(y1-100)<1);
    check("yoyo gap holds 100", std::fabs(y2-100)<1);
    check("yoyo pass2 starts at 100", std::fabs(y3-100)<1);
    check("yoyo pass2 mid is 50", std::fabs(y4-50)<1);
    check("yoyo pass2 ends at 0", std::fabs(y5)<1);

    artc::Compositor inf;
    inf.Init(1280, 720);
    layer(inf, "d");
    inf.SetProps("d", {{"w","100"},{"h","100"},{"left","0"}});
    inf.AddTween("d", {{"param","left"},{"from","0"},{"to","100"},{"time","1000"},
                       {"loop","-1"},{"loopdelay","500"}}, 0);
    inf.Update(1200);
    check("infinite gap holds 100", std::fabs(left_of(inf)-100)<1);
    check("infinite pending stays open", inf.PendingAnimationMs(1200) > 1e6);
    inf.Update(1500);
    check("infinite restarts at 0", std::fabs(left_of(inf))<1);

    artc::Compositor t;
    t.Init(1280, 720);
    layer(t, "500.ch.2");
    t.SetProps("500.ch.2", {{"x","0"},{"y","0"},{"w","400"},{"h","600"},
                             {"xscale","150"},{"yscale","150"}});
    t.AddTween("500.ch.2", {{"param","xscale"},{"from","150"},{"to","100"},
                            {"time","2000"},{"ease","easeout_quad"}}, 0);
    t.Update(0); const float s0 = scale_of(t);
    t.Update(5000); const float s5 = scale_of(t);
    std::printf("nolooop %g %g\n", s0, s5);
    check("no-loop starts at 150", std::fabs(s0-150)<1);
    check("no-loop stays 100 at 5s", std::fabs(s5-100)<1);

    std::printf("RESULT %s failures=%d\n", g_fail ? "RED" : "GREEN", g_fail);
    return g_fail ? 3 : 0;
}
`;

function patchNames() {
  return readFileSync(LIST, 'utf8').split(/\r?\n/)
    .map((line) => line.replace(/#.*$/, '').trim()).filter(Boolean);
}

function repoOf() {
  const override = process.env.ARTC_REF || process.env.ARTC_SOURCE_REPO;
  const candidates = [join(ROOT, '.ref/artemis-compat'), join(ROOT, '.ref/artemis-compat-refresh/src')];
  if (override) candidates.unshift(override);
  for (const repo of candidates) {
    if (spawnSync('git', ['-C', repo, 'cat-file', '-e', `${DEFAULT_PIN}^{commit}`]).status === 0) return repo;
  }
  return null;
}

test('loopdelay rides the real compositor from the pin', () => {
  assert.equal(DEFAULT_PIN, '17200e64ea99681e3c27d694433c0ff78d7a7cee');
  const names = patchNames();
  assert.ok(names.includes('0003-tween-loopdelay.patch'),
    'PLATFORM-PATCHES.txt must name 0003-tween-loopdelay.patch');
  const gxx = spawnSync('g++', ['--version'], { encoding: 'utf8' });
  assert.equal(gxx.status, 0, 'g++ is required; a missing compiler is not a skip');
  const repo = repoOf();
  assert.ok(repo, `pin ${DEFAULT_PIN} is not in ARTC_REF or the local pristine checkouts; missing upstream source, not a skip`);

  mkdirSync(join(ROOT, '.test/artemis-tween-loopdelay-fix'), { recursive: true });
  const work = assertExtractDir(ROOT, join(ROOT, '.test/artemis-tween-loopdelay-fix/managed'));
  rmSync(work, { recursive: true, force: true });
  mkdirSync(work, { recursive: true });
  const tar = join(work, '..', 'pin.tar');
  const archived = spawnSync('git', ['-C', repo, 'archive', '--format=tar', '-o', tar, DEFAULT_PIN], { encoding: 'utf8' });
  assert.equal(archived.status, 0, archived.stderr);
  // GNU tar treats `D:` as a remote host. A repo-relative POSIX path is the form that extracts.
  const extracted = spawnSync('tar', ['-xf', relative(ROOT, tar).split('\\').join('/'), '-C', relative(ROOT, work).split('\\').join('/')], { cwd: ROOT, encoding: 'utf8' });
  assert.equal(extracted.status, 0, extracted.stderr);
  assert.equal(existsSync(join(work, 'src/render/compositor.cpp')), true, 'archived pin has no compositor.cpp');
  for (const name of names) {
    const file = join(PORT, 'patches', name);
    const dry = spawnSync('git', ['apply', '--check', '-p1', file], { cwd: work, encoding: 'utf8' });
    assert.equal(dry.status, 0, `git apply --check ${name}\n${dry.stderr}`);
    const app = spawnSync('git', ['apply', '-p1', file], { cwd: work, encoding: 'utf8' });
    assert.equal(app.status, 0, `git apply ${name}\n${app.stderr}`);
  }
  writeFileSync(join(work, 'probe.cpp'), SHIM);
  mkdirSync(join(work, 'GLES2'), { recursive: true });
  writeFileSync(join(work, 'GLES2/gl2.h'), '#pragma once\n');
  const bin = join(work, 'probe.exe');
  const cc = spawnSync('g++', [
    '-std=c++17', '-O0', '-o', bin, 'probe.cpp', 'src/render/compositor.cpp',
    '-Isrc', '-IGLES2',
  ], { cwd: work, encoding: 'utf8' });
  assert.equal(cc.status, 0, cc.stderr || cc.stdout);
  const run = spawnSync(bin, [], { cwd: work, encoding: 'utf8' });
  const out = `${run.stdout || ''}${run.stderr || ''}`;
  writeFileSync(join(ROOT, '.test/artemis-tween-loopdelay-fix/managed.log'),
    `EXIT:${run.status}\n${out}`);
  assert.equal(run.status, 0, out);
  assert.match(out, /RESULT GREEN/);
  assert.match(out, /gap t=1100 stays 100/);
  assert.match(out, /no-loop stays 100 at 5s/);
});
