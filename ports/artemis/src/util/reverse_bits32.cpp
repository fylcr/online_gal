#include "util/engine_context.h"

namespace artc {

namespace {

// The masks of the upper half of the reversal: each keeps the single bit its
// name names and drops everything else that shift dragged along.
constexpr uint32_t kReverseBits32Bit17 = 0x20000u;
constexpr uint32_t kReverseBits32Bit18 = 0x40000u;
constexpr uint32_t kReverseBits32Bit19 = 0x80000u;
constexpr uint32_t kReverseBits32Bit20 = 0x100000u;
constexpr uint32_t kReverseBits32Bit21 = 0x200000u;
constexpr uint32_t kReverseBits32Bit22 = 0x400000u;
constexpr uint32_t kReverseBits32Bit23 = 0x800000u;
constexpr uint32_t kReverseBits32Bit24 = 0x1000000u;
constexpr uint32_t kReverseBits32Bit25 = 0x2000000u;
constexpr uint32_t kReverseBits32Bit26 = 0x4000000u;
constexpr uint32_t kReverseBits32Bit27 = 0x8000000u;
constexpr uint32_t kReverseBits32Bit28 = 0x10000000u;
constexpr uint32_t kReverseBits32Bit29 = 0x20000000u;
constexpr uint32_t kReverseBits32Bit30 = 0x40000000u;

}  // namespace

// Mirrors the 32 bits of `value`: bit 31 of the answer is bit 0 of the input and
// the other way round. Every line below moves one input bit into its mirrored
// position and masks it to that single bit; the parts are then orred together.
// The shift counts are written masked to five bits, the way the engine masks
// them. The two outermost bits need no mask: nothing else can reach them.
uint32_t ReverseBits32(EngineContext* ctx, uint32_t value) {
  EnterFrame();
  uint32_t result = 0u;
  result |= (value >> (29u & 31u)) & 2u;
  result |= (value >> (27u & 31u)) & 4u;
  result |= (value >> (25u & 31u)) & 8u;
  result |= (value >> (23u & 31u)) & 16u;
  result |= (value >> (21u & 31u)) & 32u;
  result |= (value >> (19u & 31u)) & 64u;
  result |= (value >> (17u & 31u)) & 128u;
  result |= (value >> (15u & 31u)) & 256u;
  result |= (value >> (13u & 31u)) & 512u;
  result |= (value >> (11u & 31u)) & 1024u;
  result |= (value >> (9u & 31u)) & 2048u;
  result |= (value >> (7u & 31u)) & 4096u;
  result |= (value >> (5u & 31u)) & 8192u;
  result |= (value >> (3u & 31u)) & 16384u;
  result |= (value >> (1u & 31u)) & 32768u;
  result |= (value << (1u & 31u)) & 65536u;
  result |= (value << (3u & 31u)) & kReverseBits32Bit17;
  result |= (value << (5u & 31u)) & kReverseBits32Bit18;
  result |= (value << (7u & 31u)) & kReverseBits32Bit19;
  result |= (value << (9u & 31u)) & kReverseBits32Bit20;
  result |= (value << (11u & 31u)) & kReverseBits32Bit21;
  result |= (value << (13u & 31u)) & kReverseBits32Bit22;
  result |= (value << (15u & 31u)) & kReverseBits32Bit23;
  result |= (value << (17u & 31u)) & kReverseBits32Bit24;
  result |= (value << (19u & 31u)) & kReverseBits32Bit25;
  result |= (value << (21u & 31u)) & kReverseBits32Bit26;
  result |= (value << (23u & 31u)) & kReverseBits32Bit27;
  result |= (value << (25u & 31u)) & kReverseBits32Bit28;
  result |= (value << (27u & 31u)) & kReverseBits32Bit29;
  result |= (value << (29u & 31u)) & kReverseBits32Bit30;
  result |= (value << (31u & 31u));
  result |= (value >> (31u & 31u));
  LeaveFrame();
  return result;
}

}  // namespace artc
