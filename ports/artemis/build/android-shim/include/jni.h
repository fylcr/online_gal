/* Minimal JNI surface.
 *
 * Two conventions matter and both are easy to get wrong from the C side of jni.h:
 *   * `JNIEnv`/`JavaVM` are **structs** whose members are methods, so `JNIEnv *env` supports
 *     `env->GetObjectClass(...)`.
 *   * `JavaVM::GetEnv` takes **two** arguments — the implicit `this` is not written. Declaring them as
 *     function pointers (which carry an explicit receiver parameter) makes every call site wrong.
 *
 * Every method here returns a harmless default rather than being an uninitialised pointer, so a call
 * that we do not expect is a no-op instead of a trap. The web driver passes a null `activity->clazz`,
 * which short-circuits the engine's only JNI use (resolving the DebugBridge class). */
#pragma once
#include <stddef.h>
#include <stdint.h>

typedef int8_t   jbyte;
typedef int16_t  jshort;
typedef int32_t  jint;
typedef int64_t  jlong;
typedef uint8_t  jboolean;
typedef uint16_t jchar;
typedef float    jfloat;
typedef double   jdouble;
typedef jint     jsize;

struct _jobject; typedef struct _jobject *jobject;
typedef jobject jclass;
typedef jobject jstring;
typedef jobject jthrowable;
typedef jobject jarray;
struct _jmethodID; typedef struct _jmethodID *jmethodID;
struct _jfieldID;  typedef struct _jfieldID  *jfieldID;

#define JNIEXPORT
#define JNICALL
#define JNI_VERSION_1_6 0x00010006
#define JNI_OK 0
#define JNI_ERR (-1)
#define JNI_TRUE 1
#define JNI_FALSE 0

struct JavaVM_;

struct JNIEnv_ {
    jclass      FindClass(const char *) { return nullptr; }
    jclass      GetObjectClass(jobject) { return nullptr; }
    jmethodID   GetMethodID(jclass, const char *, const char *) { return nullptr; }
    jmethodID   GetStaticMethodID(jclass, const char *, const char *) { return nullptr; }
    jfieldID    GetFieldID(jclass, const char *, const char *) { return nullptr; }
    jfieldID    GetStaticFieldID(jclass, const char *, const char *) { return nullptr; }
    jobject     NewObject(jclass, jmethodID, ...) { return nullptr; }
    jobject     CallObjectMethod(jobject, jmethodID, ...) { return nullptr; }
    jobject     CallStaticObjectMethod(jclass, jmethodID, ...) { return nullptr; }
    jboolean    CallBooleanMethod(jobject, jmethodID, ...) { return JNI_FALSE; }
    jboolean    CallStaticBooleanMethod(jclass, jmethodID, ...) { return JNI_FALSE; }
    jint        CallIntMethod(jobject, jmethodID, ...) { return 0; }
    jint        CallStaticIntMethod(jclass, jmethodID, ...) { return 0; }
    void        CallVoidMethod(jobject, jmethodID, ...) {}
    void        CallStaticVoidMethod(jclass, jmethodID, ...) {}
    jobject     GetStaticObjectField(jclass, jfieldID) { return nullptr; }
    jstring     NewStringUTF(const char *) { return nullptr; }
    const char *GetStringUTFChars(jstring, jboolean *) { return ""; }
    void        ReleaseStringUTFChars(jstring, const char *) {}
    void        GetStringUTFRegion(jstring, jsize, jsize, char *) {}
    jobject     NewGlobalRef(jobject) { return nullptr; }
    void        DeleteGlobalRef(jobject) {}
    void        DeleteLocalRef(jobject) {}
    jboolean    ExceptionCheck() { return JNI_FALSE; }
    void        ExceptionClear() {}
    void        ExceptionDescribe() {}
    jint        GetJavaVM(JavaVM_ **) { return JNI_ERR; }
};
typedef struct JNIEnv_ JNIEnv;

struct JavaVM_ {
    jint GetEnv(void **, jint) { return JNI_ERR; }
    jint AttachCurrentThread(JNIEnv **, void *) { return JNI_ERR; }
    jint DetachCurrentThread() { return JNI_OK; }
};
typedef struct JavaVM_ JavaVM;
