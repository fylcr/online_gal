#pragma once
#include <stddef.h>
#include <stdint.h>
#include <android/looper.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct AInputEvent AInputEvent;
typedef struct AInputQueue AInputQueue;
enum { AINPUT_EVENT_TYPE_KEY = 1, AINPUT_EVENT_TYPE_MOTION = 2 };
enum { AKEY_EVENT_ACTION_DOWN = 0, AKEY_EVENT_ACTION_UP = 1 };
enum {
    AMOTION_EVENT_ACTION_DOWN = 0, AMOTION_EVENT_ACTION_UP = 1,
    AMOTION_EVENT_ACTION_MOVE = 2, AMOTION_EVENT_ACTION_CANCEL = 3,
    AMOTION_EVENT_ACTION_POINTER_DOWN = 5, AMOTION_EVENT_ACTION_POINTER_UP = 6,
    AMOTION_EVENT_ACTION_MASK = 0xff,
};
enum {
    AKEYCODE_BACK = 4, AKEYCODE_DPAD_UP = 19, AKEYCODE_DPAD_DOWN = 20,
    AKEYCODE_DPAD_LEFT = 21, AKEYCODE_DPAD_RIGHT = 22, AKEYCODE_DPAD_CENTER = 23,
    AKEYCODE_VOLUME_UP = 24, AKEYCODE_VOLUME_DOWN = 25, AKEYCODE_ENTER = 66,
    AKEYCODE_MENU = 82,
};
int32_t AInputEvent_getType(const AInputEvent *event);
int32_t AKeyEvent_getKeyCode(const AInputEvent *event);
int32_t AKeyEvent_getAction(const AInputEvent *event);
int32_t AMotionEvent_getAction(const AInputEvent *event);
float AMotionEvent_getX(const AInputEvent *event, size_t pointer_index);
float AMotionEvent_getY(const AInputEvent *event, size_t pointer_index);
size_t AMotionEvent_getPointerCount(const AInputEvent *event);
int32_t AInputQueue_getEvent(AInputQueue *queue, AInputEvent **outEvent);
int32_t AInputQueue_preDispatchEvent(AInputQueue *queue, AInputEvent *event);
void AInputQueue_finishEvent(AInputQueue *queue, AInputEvent *event, int handled);
void AInputQueue_attachLooper(AInputQueue *queue, ALooper *looper, int ident,
                              int (*proc)(int, int, void *), void *data);
void AInputQueue_detachLooper(AInputQueue *queue);
#ifdef __cplusplus
}
#endif
