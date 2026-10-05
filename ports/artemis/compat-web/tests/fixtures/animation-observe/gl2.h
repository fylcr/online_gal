#pragma once
#include <cstddef>
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef float GLfloat;
typedef float GLclampf;
typedef unsigned int GLuint;
typedef char GLchar;
#define GL_FALSE 0
#define GL_TRUE 1
#define GL_NO_ERROR 0u
#define GL_TRIANGLES 0x0004u
#define GL_TRIANGLE_STRIP 0x0005u
#define GL_SRC_ALPHA 0x0302u
#define GL_ONE_MINUS_SRC_ALPHA 0x0303u
#define GL_DST_COLOR 0x0306u
#define GL_ONE_MINUS_SRC_COLOR 0x0307u
#define GL_ONE 1u
#define GL_ZERO 0u
#define GL_BLEND 0x0BE2u
#define GL_TEXTURE_2D 0x0DE1u
#define GL_UNSIGNED_BYTE 0x1401u
#define GL_FLOAT 0x1406u
#define GL_RGBA 0x1908u
#define GL_NEAREST 0x2600u
#define GL_LINEAR 0x2601u
#define GL_TEXTURE_MAG_FILTER 0x2800u
#define GL_TEXTURE_MIN_FILTER 0x2801u
#define GL_TEXTURE_WRAP_S 0x2802u
#define GL_TEXTURE_WRAP_T 0x2803u
#define GL_CLAMP_TO_EDGE 0x812Fu
#define GL_TEXTURE0 0x84C0u
#define GL_TEXTURE1 0x84C1u
#define GL_VERTEX_SHADER 0x8B31u
#define GL_FRAGMENT_SHADER 0x8B30u
#define GL_COMPILE_STATUS 0x8B81u
#define GL_LINK_STATUS 0x8B82u
#define GL_INFO_LOG_LENGTH 0x8B84u
#define GL_ACTIVE_UNIFORMS 0x8B86u
#define GL_FLOAT_VEC2 0x8B50u
#define GL_FLOAT_VEC3 0x8B51u
#define GL_FLOAT_VEC4 0x8B52u
#define GL_INT_VEC2 0x8B53u
#define GL_INT_VEC3 0x8B54u
#define GL_INT_VEC4 0x8B55u
#define GL_BOOL 0x8B56u
#define GL_BOOL_VEC2 0x8B57u
#define GL_BOOL_VEC3 0x8B58u
#define GL_BOOL_VEC4 0x8B59u
#define GL_FLOAT_MAT2 0x8B5Au
#define GL_FLOAT_MAT3 0x8B5Bu
#define GL_FLOAT_MAT4 0x8B5Cu
#define GL_SAMPLER_2D 0x8B5Eu
#define GL_INT 0x1404u
#define GL_FRAMEBUFFER 0x8D40u
#define GL_COLOR_ATTACHMENT0 0x8CE0u
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5u
#define GL_COLOR_BUFFER_BIT 0x00004000u
#define GL_VIEWPORT 0x0BA2u
#define GL_PACK_ALIGNMENT 0x0D05u
#define GL_FRAMEBUFFER_BINDING 0x8CA6u
#define GL_FUNC_ADD 0x8006u
#define GL_FUNC_REVERSE_SUBTRACT 0x800Bu
#define GL_MAX_TEXTURE_IMAGE_UNITS 0x8872u
#define GLAPI
#define GL_APICALL
#define GL_APIENTRY
