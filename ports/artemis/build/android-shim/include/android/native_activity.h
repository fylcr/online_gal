#pragma once
#include <stddef.h>
#include <stdint.h>
#include <android/log.h>
#include <android/looper.h>
#include <android/input.h>
#include <jni.h>
#include <android/native_window.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct ANativeActivity ANativeActivity;
struct ANativeActivityCallbacks {
    void (*onStart)(ANativeActivity *);
    void (*onResume)(ANativeActivity *);
    void (*onPause)(ANativeActivity *);
    void (*onStop)(ANativeActivity *);
    void (*onDestroy)(ANativeActivity *);
    void (*onWindowResized)(ANativeActivity *, ANativeWindow *);
    void (*onNativeWindowResized)(ANativeActivity *, ANativeWindow *);
    void (*onNativeWindowRedrawNeeded)(ANativeActivity *, ANativeWindow *);
    void (*onNativeWindowCreated)(ANativeActivity *, ANativeWindow *);
    void (*onNativeWindowDestroyed)(ANativeActivity *, ANativeWindow *);
    void (*onInputQueueCreated)(ANativeActivity *, AInputQueue *);
    void (*onInputQueueDestroyed)(ANativeActivity *, AInputQueue *);
    void (*onContentRectChanged)(ANativeActivity *, const void *);
    void (*onConfigurationChanged)(ANativeActivity *);
    void (*onLowMemory)(ANativeActivity *);
    void (*onSaveInstanceState)(ANativeActivity *, void *, size_t);
};
struct ANativeActivity {
    struct ANativeActivityCallbacks *callbacks;
    JavaVM *vm;
    JNIEnv *env;
    jobject clazz;
    const char *internalDataPath;
    const char *externalDataPath;
    int32_t sdkVersion;
    void *instance;
    void *assetManager;
    void *oem;
};
void ANativeActivity_finish(ANativeActivity *activity);
void ANativeActivity_onCreate(ANativeActivity *activity, void *savedState, size_t savedStateSize);
#ifdef __cplusplus
}
#endif
