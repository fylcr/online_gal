#include "util/engine_context.h"

namespace artc {

namespace {

constexpr uint32_t kMersenneTwisterTwistLowerMask = 0x7FFFFFFEu;  // low 31 bits
constexpr uint32_t kMersenneTwisterTwistUpperMask = 0x80000000u;  // the top bit
constexpr uint32_t kMersenneTwisterTwistMatrixA = 0x9908B0DFu;    // twist matrix

}  // namespace

// Regenerates the whole 624-word mt19937 state array in place and puts the
// generation index back to zero: two loops twist most of the array against the
// word 397 slots ahead (the second against the word 227 behind), and the five
// words that straddle the seam are unrolled by hand. Answers nothing.
void MersenneTwisterTwist(EngineContext* ctx, uint32_t state) {
  EnterFrame();
  uint32_t index = 0u;
  uint32_t word = 0u;
  uint32_t held = 0u;
  uint32_t next = 0u;
  uint32_t carry = 0u;
  uint32_t cell = 0u;
  uint32_t tail = 0u;
  uint32_t mixed = 0u;
  uint32_t halves = 0u;
  uint32_t folded = 0u;

  word = LoadU32(ctx, state);
  // Shift counts stay masked to the 31 bits a guest word can shift by.
  do {
    held = word & kMersenneTwisterTwistUpperMask;
    cell = state + (index << (2u & 31u));
    tail = LoadU32(ctx, cell + 1588u);
    index += 1u;
    word = LoadU32(ctx, state + (index << (2u & 31u)));
    mixed = tail ^ ((word & 1u) * kMersenneTwisterTwistMatrixA);
    halves = (word & kMersenneTwisterTwistLowerMask) | held;
    folded = halves >> (1u & 31u);
    StoreU32(ctx, cell, mixed ^ folded);
  } while (index != 222u);

  // The seam words run 222, 224, 223, 225, 226, so each step carries the word
  // it read forward and never loads the same neighbour twice.
  tail = LoadU32(ctx, state + 2476u);
  next = LoadU32(ctx, state + 892u);
  mixed = tail ^ ((next & 1u) * kMersenneTwisterTwistMatrixA);
  halves = next & kMersenneTwisterTwistLowerMask;
  held = LoadU32(ctx, state + 888u);
  halves |= held & kMersenneTwisterTwistUpperMask;
  folded = halves >> (1u & 31u);
  StoreU32(ctx, state + 888u, mixed ^ folded);
  held = next;

  tail = LoadU32(ctx, state + 2484u);
  carry = LoadU32(ctx, state + 900u);
  mixed = tail ^ ((carry & 1u) * kMersenneTwisterTwistMatrixA);
  halves = carry & kMersenneTwisterTwistLowerMask;
  next = LoadU32(ctx, state + 896u);
  halves |= next & kMersenneTwisterTwistUpperMask;
  folded = halves >> (1u & 31u);
  StoreU32(ctx, state + 896u, mixed ^ folded);

  tail = LoadU32(ctx, state + 2480u);
  mixed = tail ^ ((next & 1u) * kMersenneTwisterTwistMatrixA);
  halves = next & kMersenneTwisterTwistLowerMask;
  halves |= held & kMersenneTwisterTwistUpperMask;
  folded = halves >> (1u & 31u);
  StoreU32(ctx, state + 892u, mixed ^ folded);

  held = carry;
  tail = LoadU32(ctx, state + 2488u);
  next = LoadU32(ctx, state + 904u);
  mixed = tail ^ ((next & 1u) * kMersenneTwisterTwistMatrixA);
  halves = held & kMersenneTwisterTwistUpperMask;
  halves |= next & kMersenneTwisterTwistLowerMask;
  folded = halves >> (1u & 31u);
  StoreU32(ctx, state + 900u, mixed ^ folded);

  held = next;
  tail = LoadU32(ctx, state + 2492u);
  next = LoadU32(ctx, state + 908u);
  mixed = tail ^ ((next & 1u) * kMersenneTwisterTwistMatrixA);
  halves = (next & kMersenneTwisterTwistLowerMask) | (held & kMersenneTwisterTwistUpperMask);
  folded = halves >> (1u & 31u);
  StoreU32(ctx, state + 904u, mixed ^ folded);

  word = next;
  index = 227u;
  do {
    held = word & kMersenneTwisterTwistUpperMask;
    cell = state + (index << (2u & 31u));
    tail = LoadU32(ctx, cell - 908u);
    index += 1u;
    word = LoadU32(ctx, state + (index << (2u & 31u)));
    mixed = tail ^ ((word & 1u) * kMersenneTwisterTwistMatrixA);
    halves = (word & kMersenneTwisterTwistLowerMask) | held;
    folded = halves >> (1u & 31u);
    StoreU32(ctx, cell, mixed ^ folded);
  } while (index != 623u);

  StoreU32(ctx, state + 2496u, 0u);
  tail = LoadU32(ctx, state + 1584u);
  next = LoadU32(ctx, state);
  mixed = tail ^ ((next & 1u) * kMersenneTwisterTwistMatrixA);
  halves = next & kMersenneTwisterTwistLowerMask;
  held = LoadU32(ctx, state + 2492u);
  halves |= held & kMersenneTwisterTwistUpperMask;
  folded = halves >> (1u & 31u);
  StoreU32(ctx, state + 2492u, mixed ^ folded);
  LeaveFrame();
}

}  // namespace artc
