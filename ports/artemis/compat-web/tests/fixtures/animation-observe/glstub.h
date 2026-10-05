#pragma once
#include <GLES2/gl2.h>
#include <cstdint>
#include <cstring>
#include <map>
#include <vector>
struct Call { int fn, mode, first, count, loc; float f[64]; unsigned tex; };
enum { kDraw = 1, kUniform2 = 2, kBindTex = 3, kTexImage = 4 };
extern std::vector<Call> g_calls;
extern float g_ptr[64];
extern int g_stride, g_teximage;
extern std::map<unsigned, std::pair<int, int>> g_texsize;
extern unsigned g_bound;
#define REC(c) g_calls.push_back(c)
inline void glDrawArrays(int mode, int first, int count) {
    Call c{}; c.fn = kDraw; c.mode = mode; c.first = first; c.count = count; c.tex = g_bound;
    int n = g_stride ? (g_stride * count > 64 ? 64 : g_stride * count) : 64;
    std::memcpy(c.f, g_ptr, sizeof(float) * n); REC(c);
}
inline void glViewport(int, int, int, int) {}
inline void glScissor(int, int, int, int) {}
inline void glBindFramebuffer(int, unsigned) {}
inline void glBindTexture(int, unsigned t) { g_bound = t; Call c{}; c.fn = kBindTex; c.tex = t; REC(c); }
inline void glUniform2f(int loc, float x, float y) { Call c{}; c.fn = kUniform2; c.loc = loc; c.f[0] = x; c.f[1] = y; REC(c); }
inline void glUniform1f(int, float) {}
inline void glUniform2fv(int, int, const float *) {}
inline void glUniform3f(int, float, float, float) {}
inline void glUniform3fv(int, int, const float *) {}
inline void glUniform4fv(int, int, const float *) {}
inline void glUniform1fv(int, int, const float *) {}
inline void glUniform1iv(int, int, const int *) {}
inline void glUniform2iv(int, int, const int *) {}
inline void glUniform3iv(int, int, const int *) {}
inline void glUniform4iv(int, int, const int *) {}
inline void glUniformMatrix2fv(int, int, unsigned char, const float *) {}
inline void glUniformMatrix3fv(int, int, unsigned char, const float *) {}
inline void glUniformMatrix4fv(int, int, unsigned char, const float *) {}
inline void glVertexAttribPointer(unsigned index, int, unsigned, unsigned char, int stride, const void *p) {
    if (index != 0 || !p) return;
    g_stride = stride / 4; std::memcpy(g_ptr, p, 64);
}
inline void glTexImage2D(int, int, int, int w, int h, int, int, int, const void *) {
    ++g_teximage; g_texsize[g_bound] = {w, h}; Call c{}; c.fn = kTexImage; c.tex = g_bound; c.f[0] = (float)w; c.f[1] = (float)h; REC(c);
}
inline void glReadPixels(int, int, int, int, int, int, void *) {}
inline void glFinish() {}
inline void glFlush() {}
inline void glGenTextures(int n, unsigned *o) { static unsigned s = 10; for (int i = 0; i < n; ++i) o[i] = s++; }
inline void glGenFramebuffers(int n, unsigned *o) { static unsigned s = 1; for (int i = 0; i < n; ++i) o[i] = s++; }
inline void glDeleteTextures(int, const unsigned *) {}
inline void glDeleteFramebuffers(int, const unsigned *) {}
inline void glFramebufferTexture2D(int, int, int, unsigned, int) {}
inline unsigned glCheckFramebufferStatus(int) { return 0x8CD5; }
inline void glGetIntegerv(int pname, int *p) {
    if (pname == 0x8CA6) *p = 0;
    else if (pname == 0x0BA2) { p[0] = 0; p[1] = 0; p[2] = 1280; p[3] = 720; }
    else if (pname == 0x8872) *p = 8;
    else *p = 0;
}
inline void glTexParameteri(int, int, int) {}
inline void glTexSubImage2D(int, int, int, int, int, int, int, int, const void *) {}
inline void glCopyTexSubImage2D(int, int, int, int, int, int, int, int) {}
inline void glClear(unsigned) {}
inline void glClearColor(float, float, float, float) {}
inline void glEnable(int) {}
inline void glDisable(int) {}
inline void glBlendFuncSeparate(int, int, int, int) {}
inline void glBlendEquationSeparate(int, int) {}
inline void glUseProgram(unsigned) {}
inline void glActiveTexture(int) {}
inline void glUniform1i(int, int) {}
inline void glEnableVertexAttribArray(unsigned) {}
inline void glDisableVertexAttribArray(unsigned) {}
inline void glVertexAttrib1f(unsigned, float) {}
inline unsigned glCreateProgram() { return 1; }
inline unsigned glCreateShader(unsigned) { return 1; }
inline void glShaderSource(unsigned, int, const char *const *, const int *) {}
inline void glCompileShader(unsigned) {}
inline void glAttachShader(unsigned, unsigned) {}
inline void glBindAttribLocation(unsigned, unsigned, const char *) {}
inline void glLinkProgram(unsigned) {}
inline void glDeleteShader(unsigned) {}
inline void glDeleteProgram(unsigned) {}
inline void glPixelStorei(unsigned, int) {}
inline void glCopyTexImage2D(int, int, int, int, int, int, int, int) {}
inline int glGetAttribLocation(unsigned, const char *n) { return n[2] == 'p' ? 0 : n[2] == 'u' ? 1 : 2; }
inline int glGetUniformLocation(unsigned, const char *n) {
    if (n[2] == 's') return 10; if (n[2] == 't') return 11; if (n[2] == 'a') return 12; return 13;
}
inline unsigned glGetError() { return 0; }
inline void glGetShaderiv(unsigned, unsigned, int *p) { *p = 1; }
inline void glGetProgramiv(unsigned, unsigned pname, int *p) { *p = pname == 0x8B86u ? 0 : 1; }
inline void glGetShaderInfoLog(unsigned, int, int *, char *) {}
inline void glGetProgramInfoLog(unsigned, int, int *, char *) {}
inline void glGetActiveUniform(unsigned, unsigned, int, int *, int *length, unsigned *type, char *name) {
    if (length) *length = 0; if (type) *type = 0x1406u; if (name) name[0] = 0;
}
