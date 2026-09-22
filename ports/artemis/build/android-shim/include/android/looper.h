#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct ALooper ALooper;
enum { ALOOPER_PREPARE_ALLOW_NON_CALLBACKS = 1 };
enum { ALOOPER_EVENT_INPUT = 1 };
ALooper *ALooper_prepare(int opts);
ALooper *ALooper_forThread(void);
int ALooper_pollOnce(int timeoutMillis, int *outFd, int *outEvents, void **outData);
#ifdef __cplusplus
}
#endif
