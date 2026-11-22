#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
int artc_web_open(const char *pack, const char *os_id);
int artc_web_frame(void);
void artc_web_pointer(float x, float y, int down);
void artc_web_pointer_move(float x, float y);
void artc_web_key(int key, int down);
void artc_web_resize(int css_w, int css_h);
int artc_web_started(void);
int artc_web_stage_width(void);
int artc_web_stage_height(void);
int artc_web_revision(void);
int artc_web_presented(void);
int artc_web_exit_requested(void);
const char *artc_web_last_error(void);
int artc_web_pause(int paused);
double artc_web_now_ms(void);
void artc_web_dispose(void);
int artc_web_save(const char *file);
int artc_web_load(const char *file);
void artc_web_log_sink(void (*sink)(int level, const char *message));
int artc_web_audio_pump(void);
int artc_web_audio_voice_count(void);
double artc_web_audio_voice_handle(int index);
const char *artc_web_audio_voice_key(double handle);
int artc_web_audio_voice_rate(double handle);
int artc_web_audio_read_stereo(double handle, uintptr_t heapDest, int frames, intptr_t outGain, intptr_t outPan);
int artc_web_audio_ended(double handle);
double artc_web_audio_played_frames(const char *key);
int artc_web_audio_ack(double handle, double cumulativeFrames);
#ifdef __cplusplus
}
#endif
