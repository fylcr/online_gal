#include "util/engine_context.h"

namespace artc {

// Settles one pending jump record: a record at state 13 retargets its stored
// target from the instruction word and moves to state 12, a record at state 14
// marks the instruction word itself and moves to state 11. Answers nothing.
void PatchInstructionTarget(EngineContext* ctx, uint32_t state, uint32_t pending) {
  EnterFrame();
  uint32_t const kSetTarget = 0x7FFFFFu;
  uint32_t const kSetFlag = 0x1000000u;
  switch (LoadU32(ctx, pending) - 13u) {
    case 0u: {
      StoreU32(ctx, pending, 12u);
      const uint32_t instruction = LoadU32(ctx, state);
      const uint32_t slot = LoadU32(ctx, pending + 8u);
      // The second byte of the instruction's argument word.
      const uint32_t target = (LoadU32(ctx, instruction + 12u + (slot << 2)) >> 6u) & 255u;
      StoreU32(ctx, pending + 8u, target);
      break;
    }
    case 1u: {
      // The instruction word's low 23 bits are kept and the flag bit above them
      // is set.
      const uint32_t instruction = LoadU32(ctx, state);
      const uint32_t slot = LoadU32(ctx, pending + 8u);
      const uint32_t word = instruction + 12u + (slot << 2);
      const uint32_t marked = (LoadU32(ctx, word) & kSetTarget) | kSetFlag;
      StoreU32(ctx, word, marked);
      StoreU32(ctx, pending, 11u);
      break;
    }
    default:
      break;
  }
  LeaveFrame();
}

}  // namespace artc
