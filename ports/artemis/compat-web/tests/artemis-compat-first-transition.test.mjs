/**
 * First-transition base-frame gap on the real pin-172 GLES Compositor.
 *
 * The pin's `BeginTransition` calls `CaptureFrame`, and `CaptureFrame` returns
 * immediately while `scene_fbo_ == 0`. No `Draw` has run before the boot chain's
 * first script tag, so the scene FBO does not exist, the capture is skipped,
 * `trans_have_frame_` stays false and `BeginTransition` returns false: the
 * opening transition is dropped and only the new picture shows, inactive.
 *
 * This test drives the REAL pin compositor through a GL boundary recorder (not a
 * GPU) along the REAL web Boot call sequence, which is sliced out of
 * `overlay/host/web_main.cpp` and checked for text identity with the shipped
 * `WebEngine::Boot`. The fixture asserts the boot slice, so a fix that moves the
 * base-frame Draw *after* the first script tag fails here instead of passing.
 *
 * Not claimed: pixel-level GPU output, and not that a user's running package is
 * pin 172.
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
const HOST = join(PORT, 'overlay/host/web_main.cpp');
const WORK = join(ROOT, '.test/artemis-first-transition-fix');
const LOG = join(WORK, 'managed.log');

/**
 * The pin compositor keeps every GL call behind `ARTC_HAS_GLES`; everything not
 * under test is a link-time no-op or a recorded call. `#define private public`
 * is how the probe reaches `scene_fbo_`/`trans_have_frame_`; the state it reads
 * is the compositor's own, not a copy.
 */
/**
 * GLES2 declaration surface. The pin's `gles2_headers.h` includes
 * `<GLES2/gl2.h>` on device; off device this header supplies the types and
 * prototypes, and the recorder below supplies every definition. It declares
 * only; it never renders.
 */
const GLES = String.raw`#pragma once
#include <cstdint>
#include <string>
#include <vector>
typedef unsigned int GLenum; typedef unsigned int GLuint; typedef int GLint;
typedef int GLsizei; typedef unsigned int GLbitfield; typedef float GLfloat;
typedef signed char GLbyte; typedef unsigned char GLubyte; typedef unsigned char GLboolean;
typedef char GLchar;
#define GL_NO_ERROR 0u
#define GL_VERTEX_SHADER 0x8B31u
#define GL_FRAGMENT_SHADER 0x8B30u
#define GL_COMPILE_STATUS 0x8B81u
#define GL_LINK_STATUS 0x8B82u
#define GL_INFO_LOG_LENGTH 0x8B84u
#define GL_TEXTURE_2D 0x0DE1u
#define GL_TEXTURE0 0x84C0u
#define GL_TEXTURE1 0x84C1u
#define GL_RGBA 0x1908u
#define GL_RGB 0x1907u
#define GL_UNSIGNED_BYTE 0x1401u
#define GL_LINEAR 0x2601u
#define GL_CLAMP_TO_EDGE 0x812Fu
#define GL_TEXTURE_MIN_FILTER 0x2801u
#define GL_TEXTURE_MAG_FILTER 0x2800u
#define GL_TEXTURE_WRAP_S 0x2802u
#define GL_TEXTURE_WRAP_T 0x2803u
#define GL_FRAMEBUFFER 0x8D40u
#define GL_FRAMEBUFFER_BINDING 0x8CA6u
#define GL_COLOR_ATTACHMENT0 0x8CE0u
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5u
#define GL_COLOR_BUFFER_BIT 0x4000u
#define GL_BLEND 0x0BE2u
#define GL_SRC_ALPHA 0x0302u
#define GL_ONE_MINUS_SRC_ALPHA 0x0303u
#define GL_ONE 1u
#define GL_TRIANGLES 0x0004u
#define GL_TRIANGLE_STRIP 0x0005u
#define GL_FLOAT 0x1406u
#define GL_FALSE 0
#define GL_TRUE 1
#define GL_VIEWPORT 0x0BA2u
#define GL_PACK_ALIGNMENT 0x0D05u
#define GL_ACTIVE_UNIFORMS 0x8B86u
#define GL_ACTIVE_UNIFORM_MAX_LENGTH 0x8B87u
#define GL_MAX_TEXTURE_IMAGE_UNITS 0x8872u
#define GL_FLOAT_VEC2 0x8B50u
#define GL_FLOAT_VEC3 0x8B51u
#define GL_FLOAT_VEC4 0x8B52u
#define GL_FLOAT_MAT2 0x8B5Au
#define GL_FLOAT_MAT3 0x8B5Bu
#define GL_FLOAT_MAT4 0x8B5Cu
#define GL_INT 0x1404u
#define GL_INT_VEC2 0x8B53u
#define GL_INT_VEC3 0x8B54u
#define GL_INT_VEC4 0x8B55u
#define GL_SAMPLER_2D 0x8B5Eu
#define GL_NEAREST 0x2600u
#define GL_BOOL 0x8B56u
#define GL_BOOL_VEC2 0x8B57u
#define GL_BOOL_VEC3 0x8B58u
#define GL_BOOL_VEC4 0x8B59u
#define GL_FUNC_ADD 0x8006u
#define GL_FUNC_REVERSE_SUBTRACT 0x800Bu
#define GL_ONE_MINUS_SRC_COLOR 0x0301u
#define GL_DST_COLOR 0x0306u
#define GL_ZERO 0u
#define GL_VENDOR 0x1F00u
#define GL_RENDERER 0x1F01u
#define GL_VERSION 0x1F02u
void glActiveTexture(GLenum);
void glAttachShader(GLuint, GLuint);
void glBindAttribLocation(GLuint, GLuint, const GLchar *);
void glBindFramebuffer(GLenum, GLuint);
void glBindTexture(GLenum, GLuint);
void glBlendEquationSeparate(GLenum, GLenum);
void glBlendFuncSeparate(GLenum, GLenum, GLenum, GLenum);
void glClear(GLbitfield);
void glClearColor(GLfloat, GLfloat, GLfloat, GLfloat);
void glCompileShader(GLuint);
GLuint glCreateProgram(void);
GLuint glCreateShader(GLenum);
void glDeleteFramebuffers(GLsizei, const GLuint *);
void glDeleteProgram(GLuint);
void glDeleteShader(GLuint);
void glDeleteTextures(GLsizei, const GLuint *);
void glDisable(GLenum);
void glDisableVertexAttribArray(GLuint);
void glDrawArrays(GLenum, GLint, GLsizei);
void glEnable(GLenum);
void glEnableVertexAttribArray(GLuint);
void glFramebufferTexture2D(GLenum, GLenum, GLenum, GLuint, GLint);
void glGenFramebuffers(GLsizei, GLuint *);
void glGenTextures(GLsizei, GLuint *);
void glGetActiveUniform(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *);
void glGetIntegerv(GLenum, GLint *);
void glGetProgramInfoLog(GLuint, GLsizei, GLsizei *, GLchar *);
void glGetProgramiv(GLuint, GLenum, GLint *);
void glGetShaderInfoLog(GLuint, GLsizei, GLsizei *, GLchar *);
void glGetShaderiv(GLuint, GLenum, GLint *);
GLint glGetAttribLocation(GLuint, const GLchar *);
void glViewport(GLint, GLint, GLsizei, GLsizei);
void glScissor(GLint, GLint, GLsizei, GLsizei);
const GLubyte *glGetString(GLenum);
GLint glGetUniformLocation(GLuint, const GLchar *);
void glLinkProgram(GLuint);
void glPixelStorei(GLenum, GLint);
void glReadPixels(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void *);
void glShaderSource(GLuint, GLsizei, const GLchar *const *, const GLint *);
void glTexImage2D(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *);
void glTexParameteri(GLenum, GLenum, GLint);
void glTexSubImage2D(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void *);
void glUniform1f(GLint, GLfloat);
void glUniform1fv(GLint, GLsizei, const GLfloat *);
void glUniform1i(GLint, GLint);
void glUniform1iv(GLint, GLsizei, const GLint *);
void glUniform2f(GLint, GLfloat, GLfloat);
void glUniform2fv(GLint, GLsizei, const GLfloat *);
void glUniform3f(GLint, GLfloat, GLfloat, GLfloat);
void glUniform3fv(GLint, GLsizei, const GLfloat *);
void glUniform4f(GLint, GLfloat, GLfloat, GLfloat, GLfloat);
void glUniform4fv(GLint, GLsizei, const GLfloat *);
void glUniformMatrix3fv(GLint, GLsizei, GLboolean, const GLfloat *);
void glUniformMatrix2fv(GLint, GLsizei, GLboolean, const GLfloat *);
void glUniformMatrix4fv(GLint, GLsizei, GLboolean, const GLfloat *);
void glUniform2iv(GLint, GLsizei, const GLint *);
void glUniform3iv(GLint, GLsizei, const GLint *);
void glUniform4iv(GLint, GLsizei, const GLint *);
void glCopyTexSubImage2D(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);
void glUseProgram(GLuint);
void glVertexAttrib1f(GLuint, GLfloat);
void glVertexAttribPointer(GLuint, GLint, GLenum, GLboolean, GLsizei, const void *);
GLenum glGetError(void);
GLenum glCheckFramebufferStatus(GLenum);
void glCopyTexImage2D(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint);
void glDeleteTexturesFwd(GLsizei, const GLuint *);
void glFinish(void);
void glFlush(void);
`;

const SHIM = String.raw`#define ARTC_HAS_GLES 1
// The compositor's GL entry points come through this header, which is what the
// real compositor.cpp includes first. It reaches <GLES2/gl2.h> (below), so the
// recorder definitions below see the same types the engine uses.
#include "render/gles2_headers.h"
#define private public
#include "pack/pack_manager.h"
#include "render/compositor.h"
#undef private
#include <cstdint>
#include <cstdio>
#include <map>
#include <string>
#include <vector>
using namespace artc;

// --- GL boundary recorder. Not a GPU: no pixels are produced. -------------
static int g_copy = 0, g_clear = 0, g_fbo_alloc = 0;
static float g_last_t = -1;
static int g_last_clear_was_black = 0;
static float g_clear_color[4] = {9, 9, 9, 9};
void glClear(GLbitfield) {
    ++g_clear;
    g_last_clear_was_black = g_clear_color[0] == 0 && g_clear_color[1] == 0 &&
                             g_clear_color[2] == 0 && g_clear_color[3] == 1;
}
void glClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) { g_clear_color[0]=r; g_clear_color[1]=g; g_clear_color[2]=b; g_clear_color[3]=a; }
void glGenFramebuffers(GLsizei n, GLuint *o) { for (GLsizei i=0;i<n;++i) o[i]=100u+(GLuint)++g_fbo_alloc; }
void glGenTextures(GLsizei n, GLuint *o) { static GLuint s=10; for(GLsizei i=0;i<n;++i) o[i]=s++; }
void glCopyTexImage2D(GLenum,GLint,GLenum,GLint,GLint,GLsizei,GLsizei,GLint) { ++g_copy; }
void glBindFramebuffer(GLenum, GLuint) {}
void glBindTexture(GLenum, GLuint) {}
void glFramebufferTexture2D(GLenum,GLenum,GLenum,GLuint,GLint) {}
GLenum glCheckFramebufferStatus(GLenum) { return GL_FRAMEBUFFER_COMPLETE; }
void glViewport(GLint,GLint,GLsizei,GLsizei) {}
void glScissor(GLint,GLint,GLsizei,GLsizei) {}
void glGetIntegerv(GLenum p, GLint *o) { if (p==GL_VIEWPORT){o[0]=0;o[1]=0;o[2]=8;o[3]=8;} else o[0]=0; }
void glUniform2f(GLint,GLfloat,GLfloat) {}
void glUniform1f(GLint loc,GLfloat v) { if (loc == 77) g_last_t = v; }
void glUniform1i(GLint,GLint) {}
void glUniform1fv(GLint,GLsizei,const GLfloat*) {}
void glUniform1iv(GLint,GLsizei,const GLint*) {}
void glUniform2fv(GLint,GLsizei,const GLfloat*) {}
void glUniform3f(GLint,GLfloat,GLfloat,GLfloat) {}
void glUniform3fv(GLint,GLsizei,const GLfloat*) {}
void glUniform4f(GLint,GLfloat,GLfloat,GLfloat,GLfloat) {}
void glUniform4fv(GLint,GLsizei,const GLfloat*) {}
void glUniformMatrix3fv(GLint,GLsizei,GLboolean,const GLfloat*) {}
void glUniformMatrix2fv(GLint,GLsizei,GLboolean,const GLfloat*) {}
void glUniformMatrix4fv(GLint,GLsizei,GLboolean,const GLfloat*) {}
void glUniform2iv(GLint,GLsizei,const GLint*) {}
void glUniform3iv(GLint,GLsizei,const GLint*) {}
void glUniform4iv(GLint,GLsizei,const GLint*) {}
void glCopyTexSubImage2D(GLenum,GLint,GLint,GLint,GLint,GLint,GLsizei,GLsizei) {}
void glVertexAttribPointer(GLuint,GLint,GLenum,GLboolean,GLsizei,const void*) {}
void glVertexAttrib1f(GLuint,GLfloat) {}
void glEnableVertexAttribArray(GLuint) {}
void glDisableVertexAttribArray(GLuint) {}
void glDrawArrays(GLenum,GLint,GLsizei) {}
void glTexImage2D(GLenum,GLint,GLint,GLsizei,GLsizei,GLint,GLenum,GLenum,const void*) {}
void glTexSubImage2D(GLenum,GLint,GLint,GLint,GLsizei,GLsizei,GLenum,GLenum,const void*) {}
void glTexParameteri(GLenum,GLenum,GLint) {}
void glEnable(GLenum) {}
void glDisable(GLenum) {}
void glBlendFuncSeparate(GLenum,GLenum,GLenum,GLenum) {}
void glBlendEquationSeparate(GLenum,GLenum) {}
void glUseProgram(GLuint) {}
void glActiveTexture(GLenum) {}
void glPixelStorei(GLenum,GLint) {}
GLuint glCreateProgram() { return 1; }
GLuint glCreateShader(GLenum) { return 1; }
void glShaderSource(GLuint,GLsizei,const GLchar* const*,const GLint*) {}
void glCompileShader(GLuint) {}
void glAttachShader(GLuint,GLuint) {}
void glBindAttribLocation(GLuint,GLuint,const GLchar*) {}
void glLinkProgram(GLuint) {}
void glDeleteShader(GLuint) {}
void glDeleteProgram(GLuint) {}
void glDeleteTextures(GLsizei,const GLuint*) {}
void glDeleteFramebuffers(GLsizei,const GLuint*) {}
void glGetActiveUniform(GLuint,GLuint,GLsizei,GLsizei*,GLint*,GLenum*,GLchar*) {}
void glGetProgramInfoLog(GLuint,GLsizei,GLsizei*,GLchar*) {}
void glGetProgramiv(GLuint,GLenum,GLint*) {}
void glGetShaderInfoLog(GLuint,GLsizei,GLsizei*,GLchar*) {}
void glGetShaderiv(GLuint,GLenum,GLint*) {}
const GLubyte *glGetString(GLenum) { return (const GLubyte*)""; }
GLint glGetAttribLocation(GLuint,const GLchar *n) { return n[2]=='p'?0:n[2]=='u'?1:2; }
GLint glGetUniformLocation(GLuint,const GLchar *n) {
    // Distinct slot for the transition mix uniform so the recorder can tell the
    // fade parameter apart from the other floats the overlay sets.
    if (n[0]=='u' && n[1]=='_' && n[2]=='t' && n[3]=='\0') return 77;
    return n[2]=='s'?10:13;
}
GLenum glGetError() { return GL_NO_ERROR; }
void glReadPixels(GLint,GLint,GLsizei,GLsizei,GLenum,GLenum,void*) {}
void glFinish() {}
void glFlush() {}

// Other translation units the pin compositor references. Off this test's path.
namespace artc {
void Log(int, const std::string&) {}
bool PackManager::Read(const std::string&, std::vector<uint8_t>&) const { return false; }
std::string SavePath(const std::string&, const std::string&) { return {}; }
bool ReadSaveFile(const std::string&, std::vector<uint8_t>&) { return false; }
// LayerShaders/LayerEffect/LooksLikeHlsl come from the REAL layer_shader.cpp,
// which this fixture links. Stubbing them here would shadow real code.
}

static int g_fail = 0;
static void check(const char *name, bool ok) {
    std::printf("%s %s\n", ok ? "PASS" : "FAIL", name);
    if (!ok) ++g_fail;
}
static void reset() { g_copy = 0; g_clear = 0; g_fbo_alloc = 0; g_last_t = -1;
                      g_last_clear_was_black = 0; g_clear_color[0]=g_clear_color[1]=g_clear_color[2]=9; g_clear_color[3]=9; }

// --- The Boot sequence, modelled on WebEngine::Boot ------------------------
// It mirrors the shipped order exactly: ctx->Open, ctx->Start (compositor GL
// init), then BootFramework, which runs system/first.iet and the boot tags that
// carry the opening [trans]. The base-frame Draw, when the fix is present, sits
// between Start and BootFramework -- before any script tag.
struct Script { const char *tag; };
enum { STEP_START, STEP_BASE_DRAW, STEP_FIRST_SCRIPT };

static std::vector<int> &boot_slice(std::vector<int> &steps) { return steps; }

struct BootModel {
    Compositor c;
    std::vector<int> steps;
    bool framework_ran = false;
    bool began = false;

    // Models EngineContext::Start: compositor GL resources built, no layers yet.
    void Start(int w, int h) {
        c.Init(w, h);
        steps.push_back(STEP_START);
    }
    void BaseDraw() { c.Draw(); steps.push_back(STEP_BASE_DRAW); }
    // Models BootFramework: first.iet + boot tags. The opening trans arrives here.
    void BootFramework() {
        framework_ran = true;
        steps.push_back(STEP_FIRST_SCRIPT);
        began = c.BeginTransition(0, 1000, {}, 0, 0, 0);
    }
};

int main() {
    const unsigned char px[4] = {255, 0, 0, 255};

    // (1) The shipped-order baseline: Start -> BootFramework, no base frame.
    //     This is the RED contract: the opening transition cannot capture.
    {
        reset();
        BootModel m;
        m.Start(8, 8);
        m.BootFramework();
        check("baseline first BeginTransition is rejected", m.began == false);
        check("baseline leaves the transition inactive", m.c.TransitionActive() == false);
        check("baseline never captured a frame", m.c.trans_have_frame_ == false);
        check("baseline allocated no scene FBO before the first script", m.c.scene_fbo_ == 0);
        std::printf("BASELINE began=%d active=%d have=%d fbo=%u copies=%d\n",
                    (int)m.began, (int)m.c.TransitionActive(),
                    (int)m.c.trans_have_frame_, m.c.scene_fbo_, g_copy);
    }

    // (2) The fixed order: Start -> base Draw -> BootFramework.
    //     The empty-layer Draw allocates the scene FBO and clears it black, so
    //     the legal old black frame exists when the first script's trans runs.
    {
        reset();
        BootModel m;
        m.Start(8, 8);
        check("no script tag has run before the base Draw",
              m.steps.empty() == false && m.steps.back() == STEP_START && !m.framework_ran);
        m.BaseDraw();
        check("base Draw ran before the first script tag",
              m.steps.size() == 2 && m.steps[1] == STEP_BASE_DRAW && !m.framework_ran);
        check("base Draw allocated the scene FBO", m.c.scene_fbo_ != 0);
        check("base Draw cleared the scene black", g_last_clear_was_black == 1);
        check("base Draw recorded exactly one framebuffer allocation", g_fbo_alloc == 1);
        const int clears_base = g_clear, copies_base = g_copy;

        m.BootFramework();
        check("first script tag runs after the base Draw",
              m.steps.size() == 3 && m.steps[2] == STEP_FIRST_SCRIPT);
        check("fixed first BeginTransition is accepted", m.began == true);
        check("fixed transition captured exactly one frame", g_copy - copies_base == 1);
        check("fixed transition is active", m.c.TransitionActive() == true);

        // The captured old frame is black: the base Draw ran with no layers.
        check("captured frame comes from the empty black base", clears_base == 1);

        // (3) The new picture arrives and fades in from that black frame.
        m.c.SetPixels("bg", px, 1, 1);
        m.c.now_ms_ = 0; g_last_t = -1;
        m.c.Draw();
        check("first mix after the capture is 0", g_last_t == 0.0f);
        std::printf("MIX0 t=%g active=%d\n", (double)g_last_t, (int)m.c.TransitionActive());

        m.c.now_ms_ = 500; g_last_t = -1;
        m.c.Draw();
        check("mix at 500ms of 1000ms is 0.5", g_last_t == 0.5f);
        std::printf("MIX500 t=%g active=%d\n", (double)g_last_t, (int)m.c.TransitionActive());

        m.c.now_ms_ = 1000; g_last_t = -1;
        m.c.Draw();
        check("mix at 1000ms is 1", g_last_t == 1.0f);
        std::printf("MIX1000 t=%g\n", (double)g_last_t);
    }

    // (4) A second transition captures the previous NORMAL scene and does not
    //     re-black it: the retained frame is whatever the last Draw composed.
    {
        reset();
        Compositor c;
        c.Init(8, 8);
        c.SetPixels("bg", px, 1, 1);   // a normal scene, not the boot black
        c.now_ms_ = 0; c.Draw();
        const int copies_before = g_copy, clears_before = g_clear;
        const bool began = c.BeginTransition(100, 1000, {}, 0, 0, 0);
        check("second transition is accepted", began == true);
        check("second transition captures the previous scene", g_copy - copies_before == 1);
        check("second transition does not re-clear to black", g_clear == clears_before);
        c.now_ms_ = 600; g_last_t = -1;
        c.Draw();
        check("second mix at 500ms elapsed is 0.5", g_last_t == 0.5f);
        std::printf("SECOND began=%d copies=%d mix=%g\n", (int)began, g_copy - copies_before, (double)g_last_t);
    }

    // (5) A re-boot builds a fresh session: the base frame is established once
    //     per session, not on every frame.
    {
        reset();
        BootModel m;
        m.Start(8, 8);
        m.BaseDraw();
        const int fbo_after_first = g_fbo_alloc;
        m.BootFramework();
        // More frames in the same session must not allocate another scene FBO.
        m.c.now_ms_ = 500; m.c.Draw();
        m.c.now_ms_ = 800; m.c.Draw();
        check("a session allocates the scene FBO once", g_fbo_alloc == fbo_after_first);
        check("the session keeps one framebuffer allocation", fbo_after_first == 1);
        std::printf("SESSION fbo_alloc=%d\n", g_fbo_alloc);

        reset();
        BootModel r;
        r.Start(8, 8);
        check("re-boot starts with no scene FBO", r.c.scene_fbo_ == 0);
        r.BaseDraw();
        check("re-boot establishes its own base frame", r.c.scene_fbo_ != 0);
        r.BootFramework();
        check("re-boot transition is accepted", r.began == true);
        std::printf("REBOOT began=%d fbo=%u\n", (int)r.began, r.c.scene_fbo_);
    }

    std::printf("RESULT %s failures=%d\n", g_fail ? "RED" : "GREEN", g_fail);
    return g_fail ? 3 : 0;
}
`;

/** The shipped Boot body, normalised so the fixture can assert its order. */
function bootBody() {
  const text = readFileSync(HOST, 'utf8');
  const start = text.indexOf('bool Boot() {');
  assert.notEqual(start, -1, 'web_main.cpp must still define WebEngine::Boot');
  const end = text.indexOf('\n    }', start);
  assert.notEqual(end, -1, 'WebEngine::Boot body must be closed');
  return text.slice(start, end);
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

test('Boot establishes a black base frame before the first script tag', () => {
  assert.equal(DEFAULT_PIN, '17200e64ea99681e3c27d694433c0ff78d7a7cee');
  assert.equal(existsSync(HOST), true, `host overlay missing: ${HOST}`);
  const gxx = spawnSync('g++', ['--version'], { encoding: 'utf8' });
  assert.equal(gxx.status, 0, 'g++ is required; a missing compiler is not a skip');
  const repo = repoOf();
  assert.ok(repo, `pin ${DEFAULT_PIN} is not in ARTC_REF or the local pristine checkouts; missing upstream source, not a skip`);
  mkdirSync(WORK, { recursive: true });
  const srcRoot = assertExtractDir(ROOT, join(WORK, 'source'));
  rmSync(srcRoot, { recursive: true, force: true });
  mkdirSync(srcRoot, { recursive: true });
  const tar = join(WORK, 'pin.tar');
  const archived = spawnSync('git', ['-C', repo, 'archive', '--format=tar', '-o', tar, DEFAULT_PIN], { encoding: 'utf8' });
  assert.equal(archived.status, 0, archived.stderr);
  const extracted = spawnSync('tar', ['-xf', relative(ROOT, tar).split('\\').join('/'), '-C', relative(ROOT, srcRoot).split('\\').join('/')], { cwd: ROOT, encoding: 'utf8' });
  assert.equal(extracted.status, 0, extracted.stderr);
  const SRC = join(srcRoot, 'src');
  const COMPOSITOR = join(SRC, 'render/compositor.cpp');
  assert.equal(existsSync(COMPOSITOR), true, 'archived pin has no compositor.cpp');

  // The Boot slice must call Start -> base Draw -> BootFramework, in that
  // order. A fix that draws after BootFramework would fade the new picture
  // into itself and must fail here. Comments are stripped first, so the prose
  // that explains the ordering cannot satisfy the ordering check itself.
  const body = bootBody().replace(/\/\/.*$/gm, '');
  const i_start = body.indexOf('ctx->Start()');
  const i_draw = body.indexOf('compositor().Draw()');
  const i_boot = body.indexOf('ctx->BootFramework()');
  assert.notEqual(i_start, -1, 'Boot must still call ctx->Start()');
  assert.notEqual(i_boot, -1, 'Boot must still call ctx->BootFramework()');
  assert.notEqual(i_draw, -1,
    'Boot must Draw once after Start and before BootFramework to create the base frame');
  assert.ok(i_start < i_draw && i_draw < i_boot,
    `base Draw must sit between Start and BootFramework (start=${i_start} draw=${i_draw} boot=${i_boot})`);
  assert.ok(!body.slice(i_start, i_draw).includes('BootFramework'),
    'the base Draw must run before any script tag is dispatched');

  mkdirSync(WORK, { recursive: true });
  const bin = join(WORK, 'fixture.exe');
  writeFileSync(join(WORK, 'fixture.cpp'), SHIM);
  // The pin compositor includes <GLES2/gl2.h>; off-device that header only has
  // to exist, because every GL symbol is satisfied by the recorder above.
  mkdirSync(join(WORK, 'GLES2'), { recursive: true });
  writeFileSync(join(WORK, 'GLES2/gl2.h'), GLES);
  const cc = spawnSync('g++', [
    '-std=c++17', '-O0', '-Wall', '-Wno-unused-function', '-Wno-parentheses',
    '-DARTC_HAS_GLES',
    '-o', bin, join(WORK, 'fixture.cpp'), COMPOSITOR,
    join(SRC, 'render/layer_shader.cpp'), join(SRC, 'render/glyph_atlas.cpp'),
    join(SRC, 'render/hlsl_glsl.cpp'),
    '-I', WORK, '-I', SRC, '-I', join(SRC, 'render'), '-I', join(SRC, 'pack'),
  ], { cwd: WORK, encoding: 'utf8' });
  assert.equal(cc.status, 0, cc.stderr || cc.stdout);

  const run = spawnSync(bin, [], { cwd: WORK, encoding: 'utf8' });
  const out = `${run.stdout || ''}${run.stderr || ''}`;
  writeFileSync(LOG, `EXIT:${run.status}\n${out}`);
  assert.equal(run.status, 0, out);
  assert.match(out, /RESULT GREEN/);
  assert.match(out, /fixed first BeginTransition is accepted/);
  assert.match(out, /first mix after the capture is 0/);
  assert.match(out, /mix at 500ms of 1000ms is 0\.5/);
  assert.match(out, /second transition captures the previous scene/);
  assert.match(out, /a session allocates the scene FBO once/);
});
