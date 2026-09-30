#include "util/engine_context.h"

namespace artc {

// The stateful conversion this face delegates to (text/wide_char_to_multibyte.cpp).
uint32_t WideCharToMultibyte(EngineContext* ctx, uint32_t dest, uint32_t wide, uint32_t state);

// The stateless face of the encoder: a null destination answers zero without
// touching memory, otherwise the conversion runs with no conversion state.
uint32_t WideCharToMultibyteStateless(EngineContext* ctx, uint32_t dest, uint32_t wide) {
  EnterFrame();
  uint32_t length = 0;
  if (dest != 0) {
    length = WideCharToMultibyte(ctx, dest, wide, 0);
  }
  LeaveFrame();
  return length;
}

}  // namespace artc
