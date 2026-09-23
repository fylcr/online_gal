/*
 * artc-trap-probe.c -- diagnostics for a trap inside the rebuilt engine.
 *
 * Two dead ends this turns into answers:
 *
 *   1. Without a trap handler, `wasm_rt_trap` discards its code and the embedder sees a
 *      bare `RuntimeError: unreachable`. Defining `WASM_RT_TRAP_HANDLER` stores the code
 *      so it can be read back: 1 is OOB *or* call-stack exhaustion (the runtime gives
 *      them the same value), 5 is an explicit unreachable, and so on (`wasm-rt.h`).
 *
 *   2. Knowing it was an out-of-bounds access still does not say *which* address, and
 *      that is the whole question when two address spaces share one linear memory. Every
 *      memory access the transpiled code makes goes through `RANGE_CHECK`, so recording
 *      its arguments (`offset`, `len`, and the memory size it was compared against) says
 *      exactly what the engine tried to reach.
 *
 * Both readers are exported as `artc_*`, so an embedder inspects them without a debugger.
 * The file is compiled into the module only on request (`build-bridge-route.sh
 * --debug-trap`); it is a diagnostic, never part of the engine.
 */
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Referenced from the wasm-rt runtime, which this build compiles as C. This file is
 * compiled as C++ (clang++ is handed the whole link), so without `extern "C"` the
 * definition lands under a mangled name and the link fails with
 * `undefined symbol: artc_trap_handler` -- which is what happened first. */
void artc_trap_handler(int32_t code);

static volatile int32_t artc_last_trap = -1;
static volatile uint64_t artc_oob_offset;
static volatile uint64_t artc_oob_len;
static volatile uint64_t artc_oob_size;

__attribute__((export_name("artc_get_trap")))
int32_t artc_get_trap(void) { return artc_last_trap; }

__attribute__((export_name("artc_clear_trap")))
void artc_clear_trap(void) {
  artc_last_trap = -1;
  artc_oob_offset = 0;
  artc_oob_len = 0;
  artc_oob_size = 0;
}

__attribute__((export_name("artc_get_oob_offset")))
uint64_t artc_get_oob_offset(void) { return artc_oob_offset; }

__attribute__((export_name("artc_get_oob_len")))
uint64_t artc_get_oob_len(void) { return artc_oob_len; }

__attribute__((export_name("artc_get_oob_size")))
uint64_t artc_get_oob_size(void) { return artc_oob_size; }

/* Called from the patched RANGE_CHECK in the transpiled module, immediately before the
 * trap. Records what was refused so the caller can see it. */
void artc_record_oob(uint64_t offset, uint64_t len, uint64_t size) {
  artc_oob_offset = offset;
  artc_oob_len = len;
  artc_oob_size = size;
}

void artc_trap_handler(int32_t code) { artc_last_trap = code; }

#ifdef __cplusplus
}
#endif
