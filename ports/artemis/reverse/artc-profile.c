/*
 * artc-profile.c -- records which of the rebuilt engine's functions actually ran.
 *
 * ## Why this exists
 *
 * The transpiled module has ~5,600 functions and no names: `w2c_artemis_f199` and
 * `w2c_artemis_f5783` are indistinguishable unless something says what they do. Rewriting
 * that into readable C++ is not a transcription job, it is a *triage* job -- and triage needs
 * the opposite of the static information, which is already plentiful. It needs to know which
 * functions a real game actually reaches, how often, and in what order they first run. On the
 * evidence so far that is a small fraction of the total; everything else can stay generated.
 *
 * ## What it records
 *
 * Per function index, two numbers:
 *
 *   * how many times it was entered, and
 *   * the ordinal of its *first* entry, which reconstructs the boot sequence.
 *
 * Both are plain `uint32_t` arrays, because a diagnostic that needs a serializer to read is a
 * diagnostic nobody reads. The arrays are exported by address (`artc_profile_base`), and since
 * the rebuilt module shares the host's linear memory, the embedder reads them as a typed array
 * over the same buffer -- no copying, no accessor per function.
 *
 * ## Cost
 *
 * The increment is inlined at each entry point (`instrument-profile.py` writes it into the
 * generated C++), so a hit is a load-add-store on an array that fits in cache. It is off by
 * default: this is a measurement tool, not part of the engine.
 */
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* One past the highest function index in the generated module. The instrumenter records the
 * highest index it saw and this constant is checked against it at build time, so a module that
 * grows past the array fails loudly instead of scribbling over whatever follows. */
#define ARTC_PROFILE_MAX 8192

uint32_t artc_profile_counts[ARTC_PROFILE_MAX];
uint32_t artc_profile_order[ARTC_PROFILE_MAX];

/* Monotonic counter handed out on first entry. Starts at 1 so that 0 means "never ran" in
 * both arrays, which makes a zeroed array self-describing. */
static uint32_t artc_profile_sequence = 1;

/**
 * Called from the instrumented entry of every function.
 *
 * `if (!artc_profile_counts[index])` is written unguarded for speed: two threads never share
 * this module (the official build is single-threaded, and so is this one), so the only way the
 * test and the increment can race is if that stops being true.
 */
void artc_profile_hit(uint32_t index) {
  if (index >= ARTC_PROFILE_MAX) return;
  artc_profile_counts[index] += 1;
  if (artc_profile_order[index] == 0) {
    artc_profile_order[index] = artc_profile_sequence++;
  }
}

__attribute__((export_name("artc_profile_base")))
uint32_t *artc_profile_base(void) { return artc_profile_counts; }

__attribute__((export_name("artc_profile_order_base")))
uint32_t *artc_profile_order_base(void) { return artc_profile_order; }

__attribute__((export_name("artc_profile_capacity")))
uint32_t artc_profile_capacity(void) { return ARTC_PROFILE_MAX; }

/* How many distinct functions have been entered -- the headline number, and the one worth
 * watching while the game is driven: it tells you whether a run exercised the engine or just
 * the start-up path. */
__attribute__((export_name("artc_profile_touched")))
uint32_t artc_profile_touched(void) {
  uint32_t n = 0;
  for (uint32_t i = 0; i < ARTC_PROFILE_MAX; i++) {
    if (artc_profile_counts[i]) n += 1;
  }
  return n;
}

__attribute__((export_name("artc_profile_reset")))
void artc_profile_reset(void) {
  for (uint32_t i = 0; i < ARTC_PROFILE_MAX; i++) {
    artc_profile_counts[i] = 0;
    artc_profile_order[i] = 0;
  }
  artc_profile_sequence = 1;
}

#ifdef __cplusplus
}
#endif
