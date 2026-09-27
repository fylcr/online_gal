// compile_operation.cpp -- compiles one expression: folds what it can at compile time and emits the rest..
//
// CompileOperation -- compiles one expression: folds what it can at compile time and emits the rest.
//
// The step is suspendable: every gate below replays the call its resume
// code names. Judgment order is untouched -- the goto graph is the
// generated driver's own flattening, renamed labels only.
//
// src: artemis.wasm2c.c 780576-780576 (f1701); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
void FormatNumberValue(EngineContext* ctx, uint32_t state, uint32_t value, uint32_t options);
uint32_t CompactEntryQueue(EngineContext* ctx, uint32_t state, uint32_t queue);
void DeclareUpvalue(EngineContext* ctx, uint32_t state, uint32_t local);
uint32_t CompileComparison(EngineContext* ctx, uint32_t func_state, uint32_t lhs, uint32_t rhs);
uint32_t CompareConstantOperands(EngineContext* ctx, uint32_t state, uint32_t left, uint32_t right);
uint32_t FindNumericKeySlot(EngineContext* ctx, uint32_t state);
uint32_t ReserveIntVector(EngineContext* ctx, uint32_t state, uint32_t vector, uint32_t size);
uint32_t CheckComparableTypes(EngineContext* ctx, uint32_t state, uint32_t left, uint32_t right);
uint32_t StringLength(EngineContext* ctx, uint32_t text);
void GrowValueStack(EngineContext* ctx, uint32_t a0, uint32_t a1);
void RaiseScriptError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t FetchObjectMetamethod(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void GrowTableSlot(EngineContext* ctx, uint32_t a0);
uint32_t CoerceToNumber(EngineContext* ctx, uint32_t state, uint32_t value);
void CallValue(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
double ComputeNumericPower(EngineContext* ctx, double base, double exponent);
void StoreListEntry(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void ReleaseNodeChain(EngineContext* ctx, uint32_t state, uint32_t chain);
void StoreTableField(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key);
uint32_t AdjustNodeCapacity(EngineContext* ctx, uint32_t state, uint32_t node);
uint32_t AllocateTable(EngineContext* ctx, uint32_t state, uint32_t entries, uint32_t array);
uint32_t RawValueEqual(EngineContext* ctx, uint32_t a, uint32_t b);
void RaiseArithmeticError(EngineContext* ctx, uint32_t state, uint32_t value, uint32_t meta);
void GetIndexedValue(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);
uint32_t CompareScriptStrings(EngineContext* ctx, uint32_t left, uint32_t right);
void IndexTableValue(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t key, uint32_t result);
uint32_t GetTableFieldByNumber(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
uint32_t Call(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void RaiseTypeError(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);

namespace {

constexpr uint32_t kF1701Const2B328 = 0x2B328u;  // 
constexpr uint32_t kF1701Const2B330 = 0x2B330u;  // 
constexpr uint32_t kF1701MsgGetLengthOf = 0x2BF1Eu;  // get length of
constexpr uint32_t kF1701MsgForInitialValueMust = 0x2BF2Cu;  // 'for' initial value must be a number
constexpr uint32_t kF1701MsgForLimitMustBe = 0x2BF51u;  // 'for' limit must be a number
constexpr uint32_t kF1701MsgForStepMustBe = 0x2BF6Eu;  // 'for' step must be a number
constexpr uint32_t kF1701Const7FFFC = 0x7FFFCu;  // 
constexpr uint32_t kF1701ConstFFFFC = 0xFFFFCu;  // 
constexpr uint32_t kF1701Const3FFFF0 = 0x3FFFF0u;  // 
constexpr uint32_t kF1701Const400000 = 0x400000u;  // 
constexpr uint32_t kF1701Const7FC000 = 0x7FC000u;  // 
constexpr uint32_t kF1701ConstFFFFFFC0 = 0xFFFFFFC0u;  // 
constexpr uint32_t kF1701ConstFFFFFFFF = 0xFFFFFFFFu;  // 
constexpr uint32_t kF1701SaveFrameBytes = 72u;
constexpr uint32_t kF1701ResumeWordBytes = 4u;

}  // namespace

void CompileOperation(EngineContext* ctx, uint32_t state, uint32_t expression) {
  EnterFrame();
  uint32_t l10 = 0u; uint32_t l11 = 0u; uint32_t l12 = 0u; uint32_t l13 = 0u; uint32_t l14 = 0u; uint32_t l15 = 0u; uint32_t l16 = 0u; uint32_t l17 = 0u; uint64_t l18 = 0; uint64_t l19 = 0; uint32_t l2 = 0u; double l20 = 0; double l21 = 0; double l22 = 0; uint32_t l3 = 0u; uint32_t l4 = 0u; uint32_t l5 = 0u; uint32_t l6 = 0u; uint32_t l7 = 0u; uint32_t l8 = 0u; uint32_t l9 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u; uint32_t i4 = 0u; uint32_t i5 = 0u;
  uint64_t j0 = 0; uint64_t j1 = 0; uint64_t j2 = 0;
  double d0 = 0; double d1 = 0; double d2 = 0; double d3 = 0;
      l10 = 0, l11 = 0, l12 = 0, l13 = 0, l14 = 0, l15 = 0, l16 = 0, l17 = 0;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1701SaveFrameBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l8 = i0;
    i0 = LoadU32(ctx, i0);
    state = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 12u);
    l3 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 16u);
    l4 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 20u);
    l5 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 24u);
    l6 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 28u);
    l7 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 32u);
    l9 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 36u);
    l11 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 40u);
    l12 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 44u);
    l13 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 48u);
    l14 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 52u);
    l15 = i0;
    i0 = l8;
    j0 = LoadU64(ctx, i0 + 56u);
    l18 = j0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 64u);
    l16 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 68u);
    l17 = i0;
    i0 = l8;
    i0 = LoadU32(ctx, i0 + 4u);
    expression = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF1701ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l10 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = GuestStackSp(ctx);
    l3 = i0;
    i1 = 32u;
    i0 -= i1;
    l11 = i0;
    GuestStackSp(ctx) = i0;
    i0 = kF1701Const2B330;
    i0 = LoadU32(ctx, i0);
    l17 = i0;
    i0 = kF1701Const2B328;
    j0 = LoadU64(ctx, i0);
    l18 = j0;
  }
  loop_4: 
    i0 = ResumePhase(ctx);
    i0 = !(i0);
    if (i0) {
      i0 = state;
      i0 = LoadU32(ctx, i0 + 12u);
      l6 = i0;
      i0 = state;
      i0 = LoadU32(ctx, i0 + 20u);
      i0 = LoadU32(ctx, i0 + 4u);
      i0 = LoadU32(ctx, i0);
      l14 = i0;
      i0 = LoadU32(ctx, i0 + 16u);
      l3 = i0;
      i0 = LoadU32(ctx, i0 + 8u);
      l13 = i0;
      i0 = state;
      i0 = LoadU32(ctx, i0 + 24u);
      l9 = i0;
    }
    loop_6: 
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l9;
        l4 = i0;
        i1 = kF1701ResumeWordBytes;
        i0 += i1;
        l9 = i0;
        i0 = state;
        i0 = LoadU8(ctx, i0 + 56u);
        l2 = i0;
        i1 = 12u;
        i0 &= i1;
        i0 = !(i0);
        l5 = i0;
        i0 = l4;
        i0 = LoadU32(ctx, i0);
        l3 = i0;
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l5;
        if (i0) {goto block_12;}
        i0 = state;
        i1 = state;
        i1 = LoadU32(ctx, i1 + 64u);
        i2 = 1u;
        i1 -= i2;
        l5 = i1;
        StoreU32(ctx, i0 + 64u, i1);
        i0 = l2;
        i1 = 4u;
        i0 &= i1;
        i0 = !(i0);
        l2 = i0;
        i1 = 0u;
        i2 = l5;
        i0 = i2 ? i0 : i1;
        l5 = i0;
        if (i0) {goto block_12;}
      }
      i0 = l10;
      i0 = !(i0);
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      if (i0) {
        i0 = state;
        i1 = l9;
        DeclareUpvalue(ctx, i0, i1);
        i0 = 0u;
        i1 = ResumePhase(ctx);
        i2 = 1u;
        i1 = i1 == i2;
        if (i1) {goto spill_frame;}
      }
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = state;
        i0 = LoadU8(ctx, i0 + 6u);
        i1 = 1u;
        i0 = i0 == i1;
        l5 = i0;
        if (i0) {goto block_10;}
        i0 = 0u;
        goto block_11;
      }
      block_12:;
      i0 = l2;
      i1 = 1u;
      i2 = ResumePhase(ctx);
      i0 = i2 ? i0 : i1;
      block_11:;
      l2 = i0;
      loop_16: 
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l2;
          i0 = !(i0);
          l5 = i0;
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            i0 = 1u;
            l2 = i0;
            goto loop_16;
          }
          i0 = l3;
          i1 = 63u;
          i0 &= i1;
          l2 = i0;
          i1 = 38u;
          i0 = i0 != i1;
          l5 = i0;
        }
        i0 = l5;
        i1 = ResumePhase(ctx);
        i2 = 2u;
        i1 = i1 == i2;
        i0 |= i1;
        if (i0) {
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            switch (i0) {
              case 0: goto block_61;
              case 1: goto block_60;
              case 2: goto block_59;
              case 3: goto block_58;
              case 4: goto block_57;
              case 5: goto block_56;
              case 6: goto block_54;
              case 7: goto block_53;
              case 8: goto block_52;
              case 9: goto block_51;
              case 10: goto block_50;
              case 11: goto block_49;
              case 12: goto block_48;
              case 13: goto block_47;
              case 14: goto block_46;
              case 15: goto block_45;
              case 16: goto block_44;
              case 17: goto block_43;
              case 18: goto block_42;
              case 19: goto block_41;
              case 20: goto block_40;
              case 21: goto block_39;
              case 22: goto block_38;
              case 23: goto block_37;
              case 24: goto block_36;
              case 25: goto block_35;
              case 26: goto block_34;
              case 27: goto block_33;
              case 28: goto block_32;
              case 29: goto block_31;
              case 30: goto block_30;
              case 31: goto block_29;
              case 32: goto block_28;
              case 33: goto block_27;
              case 34: goto block_26;
              case 35: goto block_25;
              case 36: goto block_24;
              case 37: goto block_23;
              default: goto loop_6;
            }
            block_61:;
            i0 = l6;
            i1 = l3;
            i2 = 2u;
            i1 >>= (i2 & 31);
            i2 = 4080u;
            i1 &= i2;
            l4 = i1;
            i0 += i1;
            l2 = i0;
            i1 = l6;
            i2 = l3;
            i3 = 19u;
            i2 >>= (i3 & 31);
            i3 = 8176u;
            i2 &= i3;
            i1 += i2;
            l5 = i1;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l2;
            i1 = l5;
            i1 = LoadU32(ctx, i1 + 8u);
            l3 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            goto loop_6;
            block_60:;
            i0 = l6;
            i1 = l3;
            i2 = 2u;
            i1 >>= (i2 & 31);
            i2 = 4080u;
            i1 &= i2;
            l4 = i1;
            i0 += i1;
            l2 = i0;
            i1 = l13;
            i2 = l3;
            i3 = 10u;
            i2 >>= (i3 & 31);
            i3 = kF1701Const3FFFF0;
            i2 &= i3;
            i1 += i2;
            l5 = i1;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l2;
            i1 = l5;
            i1 = LoadU32(ctx, i1 + 8u);
            l3 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            goto loop_6;
            block_59:;
            i0 = l6;
            i1 = l3;
            i2 = 2u;
            i1 >>= (i2 & 31);
            i2 = 4080u;
            i1 &= i2;
            i0 += i1;
            l2 = i0;
            i1 = 1u;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = l2;
            i1 = l3;
            i2 = 23u;
            i1 >>= (i2 & 31);
            l5 = i1;
            StoreU32(ctx, i0, i1);
            i0 = l4;
            i1 = 8u;
            i0 += i1;
            l4 = i0;
            i1 = l9;
            i2 = l3;
            i3 = kF1701Const7FC000;
            i2 &= i3;
            l3 = i2;
            i0 = i2 ? i0 : i1;
            l9 = i0;
            goto loop_6;
            block_58:;
            i0 = l6;
            i1 = l3;
            i2 = 19u;
            i1 >>= (i2 & 31);
            i2 = 8176u;
            i1 &= i2;
            l4 = i1;
            i0 += i1;
            l2 = i0;
            i0 = l6;
            i1 = l3;
            i2 = 2u;
            i1 >>= (i2 & 31);
            i2 = 4080u;
            i1 &= i2;
            i0 += i1;
            l5 = i0;
            loop_62: 
              i0 = l2;
              i1 = 0u;
              StoreU32(ctx, i0 + 8u, i1);
              i0 = l5;
              i1 = l2;
              i2 = 16u;
              i1 -= i2;
              l2 = i1;
              i0 = i0 <= i1;
              l3 = i0;
              if (i0) {goto loop_62;}
            goto loop_6;
            block_57:;
            i0 = l6;
            i1 = l3;
            i2 = 2u;
            i1 >>= (i2 & 31);
            i2 = 4080u;
            i1 &= i2;
            l4 = i1;
            i0 += i1;
            l2 = i0;
            i1 = l14;
            i2 = l3;
            i3 = 21u;
            i2 >>= (i3 & 31);
            i3 = 2044u;
            i2 &= i3;
            i1 += i2;
            i1 = LoadU32(ctx, i1 + 20u);
            i1 = LoadU32(ctx, i1 + 8u);
            l5 = i1;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l2;
            i1 = l5;
            i1 = LoadU32(ctx, i1 + 8u);
            l3 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            goto loop_6;
            block_56:;
            i0 = l14;
            i0 = LoadU32(ctx, i0 + 12u);
            l2 = i0;
            i0 = l11;
            i1 = 5u;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = l11;
            i1 = l2;
            StoreU32(ctx, i0, i1);
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = l13;
            i1 = l3;
            i2 = 10u;
            i1 >>= (i2 & 31);
            i2 = kF1701Const3FFFF0;
            i1 &= i2;
            i0 += i1;
            l4 = i0;
            i0 = l6;
            i1 = l3;
            i2 = 2u;
            i1 >>= (i2 & 31);
            i2 = 4080u;
            i1 &= i2;
            i0 += i1;
            l3 = i0;
          }
          i0 = l10;
          i1 = 1u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l11;
            i2 = l4;
            i3 = l3;
            GetIndexedValue(ctx, i0, i1, i2, i3);
            i0 = 1u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_54:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = l6;
            i1 = l3;
            i2 = 19u;
            i1 >>= (i2 & 31);
            i2 = 8176u;
            i1 &= i2;
            i0 += i1;
            l4 = i0;
            i0 = l13;
            i1 = l3;
            i2 = 10u;
            i1 >>= (i2 & 31);
            l2 = i1;
            i2 = 4080u;
            i1 &= i2;
            i0 += i1;
            i1 = l6;
            i2 = l2;
            i3 = 8176u;
            i2 &= i3;
            i1 += i2;
            l2 = i1;
            i2 = l3;
            i3 = kF1701Const400000;
            i2 &= i3;
            l12 = i2;
            i0 = i2 ? i0 : i1;
            l5 = i0;
            i0 = l6;
            i1 = l3;
            i2 = 2u;
            i1 >>= (i2 & 31);
            i2 = 4080u;
            i1 &= i2;
            i0 += i1;
            l3 = i0;
          }
          i0 = l10;
          i1 = 2u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l4;
            i2 = l5;
            i3 = l3;
            GetIndexedValue(ctx, i0, i1, i2, i3);
            i0 = 2u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_53:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l14;
            i0 = LoadU32(ctx, i0 + 12u);
            l2 = i0;
            i0 = l11;
            i1 = 5u;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = l11;
            i1 = l2;
            StoreU32(ctx, i0, i1);
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = l13;
            i1 = l3;
            i2 = 10u;
            i1 >>= (i2 & 31);
            i2 = kF1701Const3FFFF0;
            i1 &= i2;
            i0 += i1;
            l4 = i0;
            i0 = l6;
            i1 = l3;
            i2 = 2u;
            i1 >>= (i2 & 31);
            i2 = 4080u;
            i1 &= i2;
            i0 += i1;
            l3 = i0;
          }
          i0 = l10;
          i1 = 3u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l11;
            i2 = l4;
            i3 = l3;
            IndexTableValue(ctx, i0, i1, i2, i3);
            i0 = 3u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_52:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l14;
            i1 = l3;
            i2 = 21u;
            i1 >>= (i2 & 31);
            i2 = 2044u;
            i1 &= i2;
            i0 += i1;
            i0 = LoadU32(ctx, i0 + 20u);
            l5 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            l4 = i0;
            i1 = l6;
            i2 = l3;
            i3 = 2u;
            i2 >>= (i3 & 31);
            i3 = 4080u;
            i2 &= i3;
            i1 += i2;
            l2 = i1;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l4;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 8u, i1);
            i0 = l2;
            i0 = LoadU32(ctx, i0 + 8u);
            i1 = 4u;
            i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
            l3 = i0;
            if (i0) {goto loop_6;}
            i0 = l2;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = LoadU8(ctx, i0 + 5u);
            i1 = 3u;
            i0 &= i1;
            i0 = !(i0);
            l3 = i0;
            if (i0) {goto loop_6;}
            i0 = l5;
            i0 = LoadU8(ctx, i0 + 5u);
            i1 = 4u;
            i0 &= i1;
            i0 = !(i0);
            l3 = i0;
            if (i0) {goto loop_6;}
            i0 = state;
            i1 = l5;
            i2 = l2;
            StoreListEntry(ctx, i0, i1, i2);
            goto loop_6;
          }
          block_51:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = l6;
            i1 = l3;
            i2 = 2u;
            i1 >>= (i2 & 31);
            i2 = 4080u;
            i1 &= i2;
            i0 += i1;
            l4 = i0;
            i0 = l6;
            i1 = l3;
            i2 = 19u;
            i1 >>= (i2 & 31);
            l2 = i1;
            i2 = 8176u;
            i1 &= i2;
            i0 += i1;
            i1 = l13;
            i2 = l2;
            i3 = 4080u;
            i2 &= i3;
            i1 += i2;
            i2 = l3;
            i3 = kF1701ConstFFFFFFFF;
            i2 = (uint32_t)((int32_t)i2 > (int32_t)i3);
            i0 = i2 ? i0 : i1;
            l5 = i0;
            i0 = l13;
            i1 = l3;
            i2 = 10u;
            i1 >>= (i2 & 31);
            l2 = i1;
            i2 = 4080u;
            i1 &= i2;
            i0 += i1;
            l12 = i0;
            i1 = l6;
            i2 = l2;
            i3 = 8176u;
            i2 &= i3;
            i1 += i2;
            l2 = i1;
            i2 = l3;
            i3 = kF1701Const400000;
            i2 &= i3;
            i0 = i2 ? i0 : i1;
            l3 = i0;
          }
          i0 = l10;
          i1 = 4u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l4;
            i2 = l5;
            i3 = l3;
            IndexTableValue(ctx, i0, i1, i2, i3);
            i0 = 4u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_50:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i1 = 23u;
            i0 >>= (i1 & 31);
            l2 = i0;
            i1 = 7u;
            i0 &= i1;
            i1 = 8u;
            i0 |= i1;
            i1 = l2;
            i2 = 3u;
            i1 >>= (i2 & 31);
            i2 = 31u;
            i1 &= i2;
            l5 = i1;
            i2 = 1u;
            i1 -= i2;
            i0 <<= (i1 & 31);
            i1 = l2;
            i2 = l5;
            i0 = i2 ? i0 : i1;
            l4 = i0;
            i0 = l3;
            i1 = 14u;
            i0 >>= (i1 & 31);
            i1 = 511u;
            i0 &= i1;
            l2 = i0;
            i1 = 7u;
            i0 &= i1;
            i1 = 8u;
            i0 |= i1;
            i1 = l2;
            i2 = 3u;
            i1 >>= (i2 & 31);
            i2 = 31u;
            i1 &= i2;
            l5 = i1;
            i2 = 1u;
            i1 -= i2;
            l7 = i1;
            i0 <<= (i1 & 31);
            l12 = i0;
            i1 = l2;
            i2 = l5;
            i0 = i2 ? i0 : i1;
            l5 = i0;
          }
          i0 = l10;
          i1 = 5u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l4;
            i2 = l5;
            i0 = AllocateTable(ctx, i0, i1, i2);
            l8 = i0;
            i0 = 5u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l2 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l6;
            i1 = l3;
            i2 = 2u;
            i1 >>= (i2 & 31);
            i2 = 4080u;
            i1 &= i2;
            i0 += i1;
            l5 = i0;
            i1 = 5u;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = l5;
            i1 = l2;
            StoreU32(ctx, i0, i1);
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 16u);
            l2 = i0;
            i0 = LoadU32(ctx, i0 + 68u);
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 64u);
            l4 = i1;
            i0 = i0 >= i1;
            l3 = i0;
          }
          i0 = l3;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = l10;
            i1 = 6u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = state;
              GrowTableSlot(ctx, i0);
              i0 = 6u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_49:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l6;
            i1 = l3;
            i2 = 2u;
            i1 >>= (i2 & 31);
            i2 = 4080u;
            i1 &= i2;
            i0 += i1;
            l2 = i0;
            i1 = l6;
            i2 = l3;
            i3 = 19u;
            i2 >>= (i3 & 31);
            i3 = 8176u;
            i2 &= i3;
            i1 += i2;
            l5 = i1;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 16u, j1);
            i0 = l2;
            i1 = l5;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 24u, i1);
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = l5;
            l4 = i0;
            i0 = l13;
            i1 = l3;
            i2 = 10u;
            i1 >>= (i2 & 31);
            l5 = i1;
            i2 = 4080u;
            i1 &= i2;
            i0 += i1;
            l12 = i0;
            i1 = l6;
            i2 = l5;
            i3 = 8176u;
            i2 &= i3;
            i1 += i2;
            l5 = i1;
            i2 = l3;
            i3 = kF1701Const400000;
            i2 &= i3;
            i0 = i2 ? i0 : i1;
            l3 = i0;
          }
          i0 = l10;
          i1 = 7u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l4;
            i2 = l3;
            i3 = l2;
            GetIndexedValue(ctx, i0, i1, i2, i3);
            i0 = 7u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_48:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l13;
            i1 = l3;
            i2 = 10u;
            i1 >>= (i2 & 31);
            l2 = i1;
            i2 = 4080u;
            i1 &= i2;
            i0 += i1;
            i1 = l6;
            i2 = l2;
            i3 = 8176u;
            i2 &= i3;
            i1 += i2;
            i2 = l3;
            i3 = kF1701Const400000;
            i2 &= i3;
            i0 = i2 ? i0 : i1;
            l2 = i0;
            i0 = l6;
            i1 = l3;
            i2 = 6u;
            i1 >>= (i2 & 31);
            i2 = 255u;
            i1 &= i2;
            l5 = i1;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l12 = i0;
            i0 = l3;
            i1 = 19u;
            i0 >>= (i1 & 31);
            l4 = i0;
            i1 = 8176u;
            i0 &= i1;
            i1 = l6;
            i0 += i1;
            i1 = l13;
            i2 = l4;
            i3 = 4080u;
            i2 &= i3;
            i1 += i2;
            i2 = l3;
            i3 = kF1701ConstFFFFFFFF;
            i2 = (uint32_t)((int32_t)i2 > (int32_t)i3);
            i0 = i2 ? i0 : i1;
            l4 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            l3 = i0;
            i1 = 3u;
            i0 = i0 == i1;
            l7 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l7;
            if (i0) {
              i0 = l2;
              i0 = LoadU32(ctx, i0 + 8u);
              i1 = 3u;
              i0 = i0 != i1;
              if (i0) {
                i0 = state;
                i1 = l9;
                StoreU32(ctx, i0 + 24u, i1);
                i0 = l4;
                goto block_87;
              }
              i0 = l12;
              i1 = l4;
              d1 = LoadF64(ctx, i1);
              i2 = l2;
              d2 = LoadF64(ctx, i2);
              d1 += d2;
              StoreF64(ctx, i0, d1);
              i0 = l6;
              i1 = l5;
              i2 = 4u;
              i1 <<= (i2 & 31);
              i0 += i1;
              l3 = i0;
              i1 = 3u;
              StoreU32(ctx, i0 + 8u, i1);
              goto loop_6;
            }
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = l3;
            i1 = 4u;
            i0 = i0 != i1;
            l3 = i0;
            if (i0) {goto block_86;}
            i0 = l4;
            i0 = LoadU32(ctx, i0);
            i1 = 16u;
            i0 += i1;
            l3 = i0;
          }
          i0 = l10;
          i1 = 8u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l3;
            i1 = l11;
            i0 = CoerceToNumber(ctx, i0, i1);
            l8 = i0;
            i0 = 8u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          if (i0) {
            i0 = l3;
          } else {
            i0 = l3;
            i0 = !(i0);
            l3 = i0;
            if (i0) {goto block_86;}
            i0 = l11;
            i1 = l11;
            d1 = LoadF64(ctx, i1);
            StoreF64(ctx, i0 + 24u, d1);
            i0 = l11;
            i1 = 24u;
            i0 += i1;
          }
          block_87:;
          l3 = i0;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            i0 = LoadU32(ctx, i0 + 8u);
            i1 = 3u;
            i0 -= i1;
            l7 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l7;
            switch (i0) {
              case 0: goto block_94;
              case 1: goto block_96;
              default: goto block_86;
            }
            block_96:;
            i0 = l2;
            i0 = LoadU32(ctx, i0);
            i1 = 16u;
            i0 += i1;
            l7 = i0;
          }
          i0 = l10;
          i1 = 9u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l7;
            i1 = l11;
            i0 = CoerceToNumber(ctx, i0, i1);
            l8 = i0;
            i0 = 9u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l7 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l7;
            i0 = !(i0);
            l7 = i0;
            if (i0) {goto block_86;}
            i0 = l11;
            i1 = l11;
            d1 = LoadF64(ctx, i1);
            StoreF64(ctx, i0 + 16u, d1);
            i0 = l11;
            i1 = 16u;
            i0 += i1;
            l2 = i0;
          }
          block_94:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l12;
            i1 = l3;
            d1 = LoadF64(ctx, i1);
            i2 = l2;
            d2 = LoadF64(ctx, i2);
            d1 += d2;
            StoreF64(ctx, i0, d1);
            i0 = l6;
            i1 = l5;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l3 = i0;
            i1 = 3u;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_86:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l4;
            i2 = 5u;
            i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
            l5 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            i0 = !(i0);
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            if (i0) {
              i0 = state;
              i1 = l2;
              i2 = 5u;
              i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
              l5 = i0;
              i0 = LoadU32(ctx, i0 + 8u);
              i0 = !(i0);
              l3 = i0;
              if (i0) {goto block_101;}
            }
            i0 = state;
            i0 = LoadU32(ctx, i0 + 32u);
            l3 = i0;
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l6 = i0;
            i1 = l5;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l6;
            i1 = l5;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l5 = i0;
            i1 = l4;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 16u, j1);
            i0 = l5;
            i1 = l4;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 24u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l5 = i0;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 32u, j1);
            i0 = l5;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 40u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 28u);
            i1 = state;
            i1 = LoadU32(ctx, i1 + 8u);
            l2 = i1;
            i0 -= i1;
            i1 = 48u;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            l4 = i0;
          }
          i0 = l4;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = l10;
            i1 = 10u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = state;
              i1 = 3u;
              GrowValueStack(ctx, i0, i1);
              i0 = 10u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            if (i0) {
              i0 = l2;
            } else {
              i0 = state;
              i0 = LoadU32(ctx, i0 + 8u);
            }
            l2 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = 48u;
            i1 += i2;
            l4 = i1;
            StoreU32(ctx, i0 + 8u, i1);
          }
          i0 = l10;
          i1 = 11u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = 1u;
            CallValue(ctx, i0, i1, i2);
            i0 = 11u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = state;
            i1 = LoadU32(ctx, i1 + 8u);
            i2 = 16u;
            i1 -= i2;
            l2 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 32u);
            l4 = i0;
            i1 = l12;
            i2 = l3;
            i1 -= i2;
            i0 += i1;
            l5 = i0;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l5;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            l3 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_101:;
          i0 = l10;
          i1 = 12u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l4;
            i2 = l2;
            RaiseArithmeticError(ctx, i0, i1, i2);
            i0 = 12u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_47:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l13;
            i1 = l3;
            i2 = 10u;
            i1 >>= (i2 & 31);
            l2 = i1;
            i2 = 4080u;
            i1 &= i2;
            i0 += i1;
            i1 = l6;
            i2 = l2;
            i3 = 8176u;
            i2 &= i3;
            i1 += i2;
            i2 = l3;
            i3 = kF1701Const400000;
            i2 &= i3;
            i0 = i2 ? i0 : i1;
            l2 = i0;
            i0 = l6;
            i1 = l3;
            i2 = 6u;
            i1 >>= (i2 & 31);
            i2 = 255u;
            i1 &= i2;
            l5 = i1;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l12 = i0;
            i0 = l3;
            i1 = 19u;
            i0 >>= (i1 & 31);
            l4 = i0;
            i1 = 8176u;
            i0 &= i1;
            i1 = l6;
            i0 += i1;
            i1 = l13;
            i2 = l4;
            i3 = 4080u;
            i2 &= i3;
            i1 += i2;
            i2 = l3;
            i3 = kF1701ConstFFFFFFFF;
            i2 = (uint32_t)((int32_t)i2 > (int32_t)i3);
            i0 = i2 ? i0 : i1;
            l4 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            l3 = i0;
            i1 = 3u;
            i0 = i0 == i1;
            l7 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l7;
            if (i0) {
              i0 = l2;
              i0 = LoadU32(ctx, i0 + 8u);
              i1 = 3u;
              i0 = i0 != i1;
              if (i0) {
                i0 = state;
                i1 = l9;
                StoreU32(ctx, i0 + 24u, i1);
                i0 = l4;
                goto block_114;
              }
              i0 = l12;
              i1 = l4;
              d1 = LoadF64(ctx, i1);
              i2 = l2;
              d2 = LoadF64(ctx, i2);
              d1 -= d2;
              StoreF64(ctx, i0, d1);
              i0 = l6;
              i1 = l5;
              i2 = 4u;
              i1 <<= (i2 & 31);
              i0 += i1;
              l3 = i0;
              i1 = 3u;
              StoreU32(ctx, i0 + 8u, i1);
              goto loop_6;
            }
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = l3;
            i1 = 4u;
            i0 = i0 != i1;
            l3 = i0;
            if (i0) {goto block_113;}
            i0 = l4;
            i0 = LoadU32(ctx, i0);
            i1 = 16u;
            i0 += i1;
            l3 = i0;
          }
          i0 = l10;
          i1 = 13u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l3;
            i1 = l11;
            i0 = CoerceToNumber(ctx, i0, i1);
            l8 = i0;
            i0 = 13u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          if (i0) {
            i0 = l3;
          } else {
            i0 = l3;
            i0 = !(i0);
            l3 = i0;
            if (i0) {goto block_113;}
            i0 = l11;
            i1 = l11;
            d1 = LoadF64(ctx, i1);
            StoreF64(ctx, i0 + 24u, d1);
            i0 = l11;
            i1 = 24u;
            i0 += i1;
          }
          block_114:;
          l3 = i0;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            i0 = LoadU32(ctx, i0 + 8u);
            i1 = 3u;
            i0 -= i1;
            l7 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l7;
            switch (i0) {
              case 0: goto block_121;
              case 1: goto block_123;
              default: goto block_113;
            }
            block_123:;
            i0 = l2;
            i0 = LoadU32(ctx, i0);
            i1 = 16u;
            i0 += i1;
            l7 = i0;
          }
          i0 = l10;
          i1 = 14u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l7;
            i1 = l11;
            i0 = CoerceToNumber(ctx, i0, i1);
            l8 = i0;
            i0 = 14u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l7 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l7;
            i0 = !(i0);
            l7 = i0;
            if (i0) {goto block_113;}
            i0 = l11;
            i1 = l11;
            d1 = LoadF64(ctx, i1);
            StoreF64(ctx, i0 + 16u, d1);
            i0 = l11;
            i1 = 16u;
            i0 += i1;
            l2 = i0;
          }
          block_121:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l12;
            i1 = l3;
            d1 = LoadF64(ctx, i1);
            i2 = l2;
            d2 = LoadF64(ctx, i2);
            d1 -= d2;
            StoreF64(ctx, i0, d1);
            i0 = l6;
            i1 = l5;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l3 = i0;
            i1 = 3u;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_113:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l4;
            i2 = 6u;
            i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
            l5 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            i0 = !(i0);
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            if (i0) {
              i0 = state;
              i1 = l2;
              i2 = 6u;
              i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
              l5 = i0;
              i0 = LoadU32(ctx, i0 + 8u);
              i0 = !(i0);
              l3 = i0;
              if (i0) {goto block_128;}
            }
            i0 = state;
            i0 = LoadU32(ctx, i0 + 32u);
            l3 = i0;
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l6 = i0;
            i1 = l5;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l6;
            i1 = l5;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l5 = i0;
            i1 = l4;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 16u, j1);
            i0 = l5;
            i1 = l4;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 24u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l5 = i0;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 32u, j1);
            i0 = l5;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 40u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 28u);
            i1 = state;
            i1 = LoadU32(ctx, i1 + 8u);
            l2 = i1;
            i0 -= i1;
            i1 = 48u;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            l4 = i0;
          }
          i0 = l4;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = l10;
            i1 = 15u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = state;
              i1 = 3u;
              GrowValueStack(ctx, i0, i1);
              i0 = 15u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            if (i0) {
              i0 = l2;
            } else {
              i0 = state;
              i0 = LoadU32(ctx, i0 + 8u);
            }
            l2 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = 48u;
            i1 += i2;
            l4 = i1;
            StoreU32(ctx, i0 + 8u, i1);
          }
          i0 = l10;
          i1 = 16u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = 1u;
            CallValue(ctx, i0, i1, i2);
            i0 = 16u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = state;
            i1 = LoadU32(ctx, i1 + 8u);
            i2 = 16u;
            i1 -= i2;
            l2 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 32u);
            l4 = i0;
            i1 = l12;
            i2 = l3;
            i1 -= i2;
            i0 += i1;
            l5 = i0;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l5;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            l3 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_128:;
          i0 = l10;
          i1 = 17u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l4;
            i2 = l2;
            RaiseArithmeticError(ctx, i0, i1, i2);
            i0 = 17u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_46:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l13;
            i1 = l3;
            i2 = 10u;
            i1 >>= (i2 & 31);
            l2 = i1;
            i2 = 4080u;
            i1 &= i2;
            i0 += i1;
            i1 = l6;
            i2 = l2;
            i3 = 8176u;
            i2 &= i3;
            i1 += i2;
            i2 = l3;
            i3 = kF1701Const400000;
            i2 &= i3;
            i0 = i2 ? i0 : i1;
            l2 = i0;
            i0 = l6;
            i1 = l3;
            i2 = 6u;
            i1 >>= (i2 & 31);
            i2 = 255u;
            i1 &= i2;
            l5 = i1;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l12 = i0;
            i0 = l3;
            i1 = 19u;
            i0 >>= (i1 & 31);
            l4 = i0;
            i1 = 8176u;
            i0 &= i1;
            i1 = l6;
            i0 += i1;
            i1 = l13;
            i2 = l4;
            i3 = 4080u;
            i2 &= i3;
            i1 += i2;
            i2 = l3;
            i3 = kF1701ConstFFFFFFFF;
            i2 = (uint32_t)((int32_t)i2 > (int32_t)i3);
            i0 = i2 ? i0 : i1;
            l4 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            l3 = i0;
            i1 = 3u;
            i0 = i0 == i1;
            l7 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l7;
            if (i0) {
              i0 = l2;
              i0 = LoadU32(ctx, i0 + 8u);
              i1 = 3u;
              i0 = i0 != i1;
              if (i0) {
                i0 = state;
                i1 = l9;
                StoreU32(ctx, i0 + 24u, i1);
                i0 = l4;
                goto block_141;
              }
              i0 = l12;
              i1 = l4;
              d1 = LoadF64(ctx, i1);
              i2 = l2;
              d2 = LoadF64(ctx, i2);
              d1 *= d2;
              StoreF64(ctx, i0, d1);
              i0 = l6;
              i1 = l5;
              i2 = 4u;
              i1 <<= (i2 & 31);
              i0 += i1;
              l3 = i0;
              i1 = 3u;
              StoreU32(ctx, i0 + 8u, i1);
              goto loop_6;
            }
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = l3;
            i1 = 4u;
            i0 = i0 != i1;
            l3 = i0;
            if (i0) {goto block_140;}
            i0 = l4;
            i0 = LoadU32(ctx, i0);
            i1 = 16u;
            i0 += i1;
            l3 = i0;
          }
          i0 = l10;
          i1 = 18u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l3;
            i1 = l11;
            i0 = CoerceToNumber(ctx, i0, i1);
            l8 = i0;
            i0 = 18u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          if (i0) {
            i0 = l3;
          } else {
            i0 = l3;
            i0 = !(i0);
            l3 = i0;
            if (i0) {goto block_140;}
            i0 = l11;
            i1 = l11;
            d1 = LoadF64(ctx, i1);
            StoreF64(ctx, i0 + 24u, d1);
            i0 = l11;
            i1 = 24u;
            i0 += i1;
          }
          block_141:;
          l3 = i0;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            i0 = LoadU32(ctx, i0 + 8u);
            i1 = 3u;
            i0 -= i1;
            l7 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l7;
            switch (i0) {
              case 0: goto block_148;
              case 1: goto block_150;
              default: goto block_140;
            }
            block_150:;
            i0 = l2;
            i0 = LoadU32(ctx, i0);
            i1 = 16u;
            i0 += i1;
            l7 = i0;
          }
          i0 = l10;
          i1 = 19u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l7;
            i1 = l11;
            i0 = CoerceToNumber(ctx, i0, i1);
            l8 = i0;
            i0 = 19u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l7 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l7;
            i0 = !(i0);
            l7 = i0;
            if (i0) {goto block_140;}
            i0 = l11;
            i1 = l11;
            d1 = LoadF64(ctx, i1);
            StoreF64(ctx, i0 + 16u, d1);
            i0 = l11;
            i1 = 16u;
            i0 += i1;
            l2 = i0;
          }
          block_148:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l12;
            i1 = l3;
            d1 = LoadF64(ctx, i1);
            i2 = l2;
            d2 = LoadF64(ctx, i2);
            d1 *= d2;
            StoreF64(ctx, i0, d1);
            i0 = l6;
            i1 = l5;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l3 = i0;
            i1 = 3u;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_140:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l4;
            i2 = 7u;
            i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
            l5 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            i0 = !(i0);
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            if (i0) {
              i0 = state;
              i1 = l2;
              i2 = 7u;
              i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
              l5 = i0;
              i0 = LoadU32(ctx, i0 + 8u);
              i0 = !(i0);
              l3 = i0;
              if (i0) {goto block_155;}
            }
            i0 = state;
            i0 = LoadU32(ctx, i0 + 32u);
            l3 = i0;
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l6 = i0;
            i1 = l5;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l6;
            i1 = l5;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l5 = i0;
            i1 = l4;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 16u, j1);
            i0 = l5;
            i1 = l4;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 24u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l5 = i0;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 32u, j1);
            i0 = l5;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 40u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 28u);
            i1 = state;
            i1 = LoadU32(ctx, i1 + 8u);
            l2 = i1;
            i0 -= i1;
            i1 = 48u;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            l4 = i0;
          }
          i0 = l4;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = l10;
            i1 = 20u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = state;
              i1 = 3u;
              GrowValueStack(ctx, i0, i1);
              i0 = 20u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            if (i0) {
              i0 = l2;
            } else {
              i0 = state;
              i0 = LoadU32(ctx, i0 + 8u);
            }
            l2 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = 48u;
            i1 += i2;
            l4 = i1;
            StoreU32(ctx, i0 + 8u, i1);
          }
          i0 = l10;
          i1 = 21u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = 1u;
            CallValue(ctx, i0, i1, i2);
            i0 = 21u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = state;
            i1 = LoadU32(ctx, i1 + 8u);
            i2 = 16u;
            i1 -= i2;
            l2 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 32u);
            l4 = i0;
            i1 = l12;
            i2 = l3;
            i1 -= i2;
            i0 += i1;
            l5 = i0;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l5;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            l3 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_155:;
          i0 = l10;
          i1 = 22u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l4;
            i2 = l2;
            RaiseArithmeticError(ctx, i0, i1, i2);
            i0 = 22u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_45:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l13;
            i1 = l3;
            i2 = 10u;
            i1 >>= (i2 & 31);
            l2 = i1;
            i2 = 4080u;
            i1 &= i2;
            i0 += i1;
            i1 = l6;
            i2 = l2;
            i3 = 8176u;
            i2 &= i3;
            i1 += i2;
            i2 = l3;
            i3 = kF1701Const400000;
            i2 &= i3;
            i0 = i2 ? i0 : i1;
            l2 = i0;
            i0 = l6;
            i1 = l3;
            i2 = 6u;
            i1 >>= (i2 & 31);
            i2 = 255u;
            i1 &= i2;
            l5 = i1;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l12 = i0;
            i0 = l3;
            i1 = 19u;
            i0 >>= (i1 & 31);
            l4 = i0;
            i1 = 8176u;
            i0 &= i1;
            i1 = l6;
            i0 += i1;
            i1 = l13;
            i2 = l4;
            i3 = 4080u;
            i2 &= i3;
            i1 += i2;
            i2 = l3;
            i3 = kF1701ConstFFFFFFFF;
            i2 = (uint32_t)((int32_t)i2 > (int32_t)i3);
            i0 = i2 ? i0 : i1;
            l4 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            l3 = i0;
            i1 = 3u;
            i0 = i0 == i1;
            l7 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l7;
            if (i0) {
              i0 = l2;
              i0 = LoadU32(ctx, i0 + 8u);
              i1 = 3u;
              i0 = i0 != i1;
              if (i0) {
                i0 = state;
                i1 = l9;
                StoreU32(ctx, i0 + 24u, i1);
                i0 = l4;
                goto block_168;
              }
              i0 = l12;
              i1 = l4;
              d1 = LoadF64(ctx, i1);
              i2 = l2;
              d2 = LoadF64(ctx, i2);
              d1 /= d2;
              StoreF64(ctx, i0, d1);
              i0 = l6;
              i1 = l5;
              i2 = 4u;
              i1 <<= (i2 & 31);
              i0 += i1;
              l3 = i0;
              i1 = 3u;
              StoreU32(ctx, i0 + 8u, i1);
              goto loop_6;
            }
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = l3;
            i1 = 4u;
            i0 = i0 != i1;
            l3 = i0;
            if (i0) {goto block_167;}
            i0 = l4;
            i0 = LoadU32(ctx, i0);
            i1 = 16u;
            i0 += i1;
            l3 = i0;
          }
          i0 = l10;
          i1 = 23u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l3;
            i1 = l11;
            i0 = CoerceToNumber(ctx, i0, i1);
            l8 = i0;
            i0 = 23u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          if (i0) {
            i0 = l3;
          } else {
            i0 = l3;
            i0 = !(i0);
            l3 = i0;
            if (i0) {goto block_167;}
            i0 = l11;
            i1 = l11;
            d1 = LoadF64(ctx, i1);
            StoreF64(ctx, i0 + 24u, d1);
            i0 = l11;
            i1 = 24u;
            i0 += i1;
          }
          block_168:;
          l3 = i0;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            i0 = LoadU32(ctx, i0 + 8u);
            i1 = 3u;
            i0 -= i1;
            l7 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l7;
            switch (i0) {
              case 0: goto block_175;
              case 1: goto block_177;
              default: goto block_167;
            }
            block_177:;
            i0 = l2;
            i0 = LoadU32(ctx, i0);
            i1 = 16u;
            i0 += i1;
            l7 = i0;
          }
          i0 = l10;
          i1 = 24u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l7;
            i1 = l11;
            i0 = CoerceToNumber(ctx, i0, i1);
            l8 = i0;
            i0 = 24u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l7 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l7;
            i0 = !(i0);
            l7 = i0;
            if (i0) {goto block_167;}
            i0 = l11;
            i1 = l11;
            d1 = LoadF64(ctx, i1);
            StoreF64(ctx, i0 + 16u, d1);
            i0 = l11;
            i1 = 16u;
            i0 += i1;
            l2 = i0;
          }
          block_175:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l12;
            i1 = l3;
            d1 = LoadF64(ctx, i1);
            i2 = l2;
            d2 = LoadF64(ctx, i2);
            d1 /= d2;
            StoreF64(ctx, i0, d1);
            i0 = l6;
            i1 = l5;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l3 = i0;
            i1 = 3u;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_167:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l4;
            i2 = 8u;
            i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
            l5 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            i0 = !(i0);
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            if (i0) {
              i0 = state;
              i1 = l2;
              i2 = 8u;
              i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
              l5 = i0;
              i0 = LoadU32(ctx, i0 + 8u);
              i0 = !(i0);
              l3 = i0;
              if (i0) {goto block_182;}
            }
            i0 = state;
            i0 = LoadU32(ctx, i0 + 32u);
            l3 = i0;
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l6 = i0;
            i1 = l5;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l6;
            i1 = l5;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l5 = i0;
            i1 = l4;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 16u, j1);
            i0 = l5;
            i1 = l4;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 24u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l5 = i0;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 32u, j1);
            i0 = l5;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 40u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 28u);
            i1 = state;
            i1 = LoadU32(ctx, i1 + 8u);
            l2 = i1;
            i0 -= i1;
            i1 = 48u;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            l4 = i0;
          }
          i0 = l4;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = l10;
            i1 = 25u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = state;
              i1 = 3u;
              GrowValueStack(ctx, i0, i1);
              i0 = 25u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            if (i0) {
              i0 = l2;
            } else {
              i0 = state;
              i0 = LoadU32(ctx, i0 + 8u);
            }
            l2 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = 48u;
            i1 += i2;
            l4 = i1;
            StoreU32(ctx, i0 + 8u, i1);
          }
          i0 = l10;
          i1 = 26u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = 1u;
            CallValue(ctx, i0, i1, i2);
            i0 = 26u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = state;
            i1 = LoadU32(ctx, i1 + 8u);
            i2 = 16u;
            i1 -= i2;
            l2 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 32u);
            l4 = i0;
            i1 = l12;
            i2 = l3;
            i1 -= i2;
            i0 += i1;
            l5 = i0;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l5;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            l3 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_182:;
          i0 = l10;
          i1 = 27u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l4;
            i2 = l2;
            RaiseArithmeticError(ctx, i0, i1, i2);
            i0 = 27u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_44:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l13;
            i1 = l3;
            i2 = 10u;
            i1 >>= (i2 & 31);
            l2 = i1;
            i2 = 4080u;
            i1 &= i2;
            i0 += i1;
            i1 = l6;
            i2 = l2;
            i3 = 8176u;
            i2 &= i3;
            i1 += i2;
            i2 = l3;
            i3 = kF1701Const400000;
            i2 &= i3;
            i0 = i2 ? i0 : i1;
            l2 = i0;
            i0 = l6;
            i1 = l3;
            i2 = 6u;
            i1 >>= (i2 & 31);
            i2 = 255u;
            i1 &= i2;
            l5 = i1;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l12 = i0;
            i0 = l3;
            i1 = 19u;
            i0 >>= (i1 & 31);
            l4 = i0;
            i1 = 8176u;
            i0 &= i1;
            i1 = l6;
            i0 += i1;
            i1 = l13;
            i2 = l4;
            i3 = 4080u;
            i2 &= i3;
            i1 += i2;
            i2 = l3;
            i3 = kF1701ConstFFFFFFFF;
            i2 = (uint32_t)((int32_t)i2 > (int32_t)i3);
            i0 = i2 ? i0 : i1;
            l4 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            l3 = i0;
            i1 = 3u;
            i0 = i0 == i1;
            l7 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l7;
            if (i0) {
              i0 = l2;
              i0 = LoadU32(ctx, i0 + 8u);
              i1 = 3u;
              i0 = i0 != i1;
              if (i0) {
                i0 = state;
                i1 = l9;
                StoreU32(ctx, i0 + 24u, i1);
                i0 = l4;
                goto block_195;
              }
              i0 = l4;
              d0 = LoadF64(ctx, i0);
              l20 = d0;
              i1 = l2;
              d1 = LoadF64(ctx, i1);
              l21 = d1;
              d0 /= d1;
              d0 = wasm_floor(d0);
              l22 = d0;
              i0 = l12;
              d1 = l20;
              d2 = l21;
              d3 = l22;
              d2 *= d3;
              d1 -= d2;
              StoreF64(ctx, i0, d1);
              i0 = l6;
              i1 = l5;
              i2 = 4u;
              i1 <<= (i2 & 31);
              i0 += i1;
              l3 = i0;
              i1 = 3u;
              StoreU32(ctx, i0 + 8u, i1);
              goto loop_6;
            }
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = l3;
            i1 = 4u;
            i0 = i0 != i1;
            l3 = i0;
            if (i0) {goto block_194;}
            i0 = l4;
            i0 = LoadU32(ctx, i0);
            i1 = 16u;
            i0 += i1;
            l3 = i0;
          }
          i0 = l10;
          i1 = 28u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l3;
            i1 = l11;
            i0 = CoerceToNumber(ctx, i0, i1);
            l8 = i0;
            i0 = 28u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          if (i0) {
            i0 = l3;
          } else {
            i0 = l3;
            i0 = !(i0);
            l3 = i0;
            if (i0) {goto block_194;}
            i0 = l11;
            i1 = l11;
            d1 = LoadF64(ctx, i1);
            StoreF64(ctx, i0 + 24u, d1);
            i0 = l11;
            i1 = 24u;
            i0 += i1;
          }
          block_195:;
          l3 = i0;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            i0 = LoadU32(ctx, i0 + 8u);
            i1 = 3u;
            i0 -= i1;
            l7 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l7;
            switch (i0) {
              case 0: goto block_202;
              case 1: goto block_204;
              default: goto block_194;
            }
            block_204:;
            i0 = l2;
            i0 = LoadU32(ctx, i0);
            i1 = 16u;
            i0 += i1;
            l7 = i0;
          }
          i0 = l10;
          i1 = 29u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l7;
            i1 = l11;
            i0 = CoerceToNumber(ctx, i0, i1);
            l8 = i0;
            i0 = 29u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l7 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l7;
            i0 = !(i0);
            l7 = i0;
            if (i0) {goto block_194;}
            i0 = l11;
            i1 = l11;
            d1 = LoadF64(ctx, i1);
            StoreF64(ctx, i0 + 16u, d1);
            i0 = l11;
            i1 = 16u;
            i0 += i1;
            l2 = i0;
          }
          block_202:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            d0 = LoadF64(ctx, i0);
            l20 = d0;
            i1 = l2;
            d1 = LoadF64(ctx, i1);
            l21 = d1;
            d0 /= d1;
            d0 = wasm_floor(d0);
            l22 = d0;
            i0 = l12;
            d1 = l20;
            d2 = l21;
            d3 = l22;
            d2 *= d3;
            d1 -= d2;
            StoreF64(ctx, i0, d1);
            i0 = l6;
            i1 = l5;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l3 = i0;
            i1 = 3u;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_194:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l4;
            i2 = 9u;
            i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
            l5 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            i0 = !(i0);
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            if (i0) {
              i0 = state;
              i1 = l2;
              i2 = 9u;
              i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
              l5 = i0;
              i0 = LoadU32(ctx, i0 + 8u);
              i0 = !(i0);
              l3 = i0;
              if (i0) {goto block_209;}
            }
            i0 = state;
            i0 = LoadU32(ctx, i0 + 32u);
            l3 = i0;
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l6 = i0;
            i1 = l5;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l6;
            i1 = l5;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l5 = i0;
            i1 = l4;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 16u, j1);
            i0 = l5;
            i1 = l4;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 24u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l5 = i0;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 32u, j1);
            i0 = l5;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 40u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 28u);
            i1 = state;
            i1 = LoadU32(ctx, i1 + 8u);
            l2 = i1;
            i0 -= i1;
            i1 = 48u;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            l4 = i0;
          }
          i0 = l4;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = l10;
            i1 = 30u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = state;
              i1 = 3u;
              GrowValueStack(ctx, i0, i1);
              i0 = 30u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            if (i0) {
              i0 = l2;
            } else {
              i0 = state;
              i0 = LoadU32(ctx, i0 + 8u);
            }
            l2 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = 48u;
            i1 += i2;
            l4 = i1;
            StoreU32(ctx, i0 + 8u, i1);
          }
          i0 = l10;
          i1 = 31u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = 1u;
            CallValue(ctx, i0, i1, i2);
            i0 = 31u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = state;
            i1 = LoadU32(ctx, i1 + 8u);
            i2 = 16u;
            i1 -= i2;
            l2 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 32u);
            l4 = i0;
            i1 = l12;
            i2 = l3;
            i1 -= i2;
            i0 += i1;
            l5 = i0;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l5;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            l3 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_209:;
          i0 = l10;
          i1 = 32u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l4;
            i2 = l2;
            RaiseArithmeticError(ctx, i0, i1, i2);
            i0 = 32u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_43:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l13;
            i1 = l3;
            i2 = 10u;
            i1 >>= (i2 & 31);
            l2 = i1;
            i2 = 4080u;
            i1 &= i2;
            i0 += i1;
            i1 = l6;
            i2 = l2;
            i3 = 8176u;
            i2 &= i3;
            i1 += i2;
            i2 = l3;
            i3 = kF1701Const400000;
            i2 &= i3;
            i0 = i2 ? i0 : i1;
            l2 = i0;
            i0 = l6;
            i1 = l3;
            i2 = 6u;
            i1 >>= (i2 & 31);
            i2 = 255u;
            i1 &= i2;
            l5 = i1;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l12 = i0;
            i0 = l3;
            i1 = 19u;
            i0 >>= (i1 & 31);
            l4 = i0;
            i1 = 8176u;
            i0 &= i1;
            i1 = l6;
            i0 += i1;
            i1 = l13;
            i2 = l4;
            i3 = 4080u;
            i2 &= i3;
            i1 += i2;
            i2 = l3;
            i3 = kF1701ConstFFFFFFFF;
            i2 = (uint32_t)((int32_t)i2 > (int32_t)i3);
            i0 = i2 ? i0 : i1;
            l4 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            l3 = i0;
            i1 = 3u;
            i0 = i0 == i1;
            l7 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l7;
            if (i0) {
              i0 = l2;
              i0 = LoadU32(ctx, i0 + 8u);
              i1 = 3u;
              i0 = i0 != i1;
              if (i0) {
                i0 = state;
                i1 = l9;
                StoreU32(ctx, i0 + 24u, i1);
                i0 = l4;
                goto block_222;
              }
              i0 = l12;
              i1 = l4;
              d1 = LoadF64(ctx, i1);
              i2 = l2;
              d2 = LoadF64(ctx, i2);
              d1 = ComputeNumericPower(ctx, d1, d2);
              StoreF64(ctx, i0, d1);
              i0 = l6;
              i1 = l5;
              i2 = 4u;
              i1 <<= (i2 & 31);
              i0 += i1;
              l3 = i0;
              i1 = 3u;
              StoreU32(ctx, i0 + 8u, i1);
              goto loop_6;
            }
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = l3;
            i1 = 4u;
            i0 = i0 != i1;
            l3 = i0;
            if (i0) {goto block_221;}
            i0 = l4;
            i0 = LoadU32(ctx, i0);
            i1 = 16u;
            i0 += i1;
            l3 = i0;
          }
          i0 = l10;
          i1 = 33u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l3;
            i1 = l11;
            i0 = CoerceToNumber(ctx, i0, i1);
            l8 = i0;
            i0 = 33u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          if (i0) {
            i0 = l3;
          } else {
            i0 = l3;
            i0 = !(i0);
            l3 = i0;
            if (i0) {goto block_221;}
            i0 = l11;
            i1 = l11;
            d1 = LoadF64(ctx, i1);
            StoreF64(ctx, i0 + 24u, d1);
            i0 = l11;
            i1 = 24u;
            i0 += i1;
          }
          block_222:;
          l3 = i0;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            i0 = LoadU32(ctx, i0 + 8u);
            i1 = 3u;
            i0 -= i1;
            l7 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l7;
            switch (i0) {
              case 0: goto block_229;
              case 1: goto block_231;
              default: goto block_221;
            }
            block_231:;
            i0 = l2;
            i0 = LoadU32(ctx, i0);
            i1 = 16u;
            i0 += i1;
            l7 = i0;
          }
          i0 = l10;
          i1 = 34u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l7;
            i1 = l11;
            i0 = CoerceToNumber(ctx, i0, i1);
            l8 = i0;
            i0 = 34u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l7 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l7;
            i0 = !(i0);
            l7 = i0;
            if (i0) {goto block_221;}
            i0 = l11;
            i1 = l11;
            d1 = LoadF64(ctx, i1);
            StoreF64(ctx, i0 + 16u, d1);
            i0 = l11;
            i1 = 16u;
            i0 += i1;
            l2 = i0;
          }
          block_229:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l12;
            i1 = l3;
            d1 = LoadF64(ctx, i1);
            i2 = l2;
            d2 = LoadF64(ctx, i2);
            d1 = ComputeNumericPower(ctx, d1, d2);
            StoreF64(ctx, i0, d1);
            i0 = l6;
            i1 = l5;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l3 = i0;
            i1 = 3u;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_221:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l4;
            i2 = 10u;
            i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
            l5 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            i0 = !(i0);
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            if (i0) {
              i0 = state;
              i1 = l2;
              i2 = 10u;
              i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
              l5 = i0;
              i0 = LoadU32(ctx, i0 + 8u);
              i0 = !(i0);
              l3 = i0;
              if (i0) {goto block_236;}
            }
            i0 = state;
            i0 = LoadU32(ctx, i0 + 32u);
            l3 = i0;
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l6 = i0;
            i1 = l5;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l6;
            i1 = l5;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l5 = i0;
            i1 = l4;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 16u, j1);
            i0 = l5;
            i1 = l4;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 24u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l5 = i0;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 32u, j1);
            i0 = l5;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 40u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 28u);
            i1 = state;
            i1 = LoadU32(ctx, i1 + 8u);
            l2 = i1;
            i0 -= i1;
            i1 = 48u;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            l4 = i0;
          }
          i0 = l4;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = l10;
            i1 = 35u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = state;
              i1 = 3u;
              GrowValueStack(ctx, i0, i1);
              i0 = 35u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            if (i0) {
              i0 = l2;
            } else {
              i0 = state;
              i0 = LoadU32(ctx, i0 + 8u);
            }
            l2 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = 48u;
            i1 += i2;
            l4 = i1;
            StoreU32(ctx, i0 + 8u, i1);
          }
          i0 = l10;
          i1 = 36u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = 1u;
            CallValue(ctx, i0, i1, i2);
            i0 = 36u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = state;
            i1 = LoadU32(ctx, i1 + 8u);
            i2 = 16u;
            i1 -= i2;
            l2 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 32u);
            l4 = i0;
            i1 = l12;
            i2 = l3;
            i1 -= i2;
            i0 += i1;
            l5 = i0;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l5;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            l3 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_236:;
          i0 = l10;
          i1 = 37u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l4;
            i2 = l2;
            RaiseArithmeticError(ctx, i0, i1, i2);
            i0 = 37u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_42:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l6;
            i1 = l3;
            i2 = 6u;
            i1 >>= (i2 & 31);
            i2 = 255u;
            i1 &= i2;
            l4 = i1;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l5 = i0;
            i0 = l6;
            i1 = l3;
            i2 = 19u;
            i1 >>= (i2 & 31);
            i2 = 8176u;
            i1 &= i2;
            i0 += i1;
            l2 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            l3 = i0;
            i1 = 3u;
            i0 = i0 == i1;
            l12 = i0;
            if (i0) {
              i0 = l5;
              i1 = l2;
              d1 = LoadF64(ctx, i1);
              d1 = -(d1);
              StoreF64(ctx, i0, d1);
              i0 = l5;
              i1 = 3u;
              StoreU32(ctx, i0 + 8u, i1);
              goto loop_6;
            }
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = l3;
            i1 = 4u;
            i0 = i0 != i1;
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            if (i0) {goto block_249;}
            i0 = l2;
            i0 = LoadU32(ctx, i0);
            i1 = 16u;
            i0 += i1;
            l3 = i0;
          }
          i0 = l10;
          i1 = 38u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l3;
            i1 = l11;
            i0 = CoerceToNumber(ctx, i0, i1);
            l8 = i0;
            i0 = 38u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i0 = !(i0);
            l3 = i0;
            if (i0) {goto block_249;}
            i0 = l11;
            i1 = l11;
            d1 = LoadF64(ctx, i1);
            StoreF64(ctx, i0 + 24u, d1);
            i0 = l2;
            i0 = LoadU32(ctx, i0 + 8u);
            i1 = 3u;
            i0 -= i1;
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            switch (i0) {
              case 0: goto block_253;
              case 1: goto block_255;
              default: goto block_249;
            }
            block_255:;
            i0 = l2;
            i0 = LoadU32(ctx, i0);
            i1 = 16u;
            i0 += i1;
            l3 = i0;
          }
          i0 = l10;
          i1 = 39u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l3;
            i1 = l11;
            i0 = CoerceToNumber(ctx, i0, i1);
            l8 = i0;
            i0 = 39u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i0 = !(i0);
            l3 = i0;
            if (i0) {goto block_249;}
          }
          block_253:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l5;
            i1 = l11;
            d1 = LoadF64(ctx, i1 + 24u);
            d1 = -(d1);
            StoreF64(ctx, i0, d1);
            i0 = l6;
            i1 = l4;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l3 = i0;
            i1 = 3u;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_249:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = 11u;
            i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
            l3 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            i0 = !(i0);
            l4 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l4;
            if (i0) {
              i0 = state;
              i1 = l2;
              i2 = 11u;
              i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
              l3 = i0;
              i0 = LoadU32(ctx, i0 + 8u);
              i0 = !(i0);
              l4 = i0;
              if (i0) {goto block_260;}
            }
            i0 = state;
            i0 = LoadU32(ctx, i0 + 32u);
            l4 = i0;
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l6 = i0;
            i1 = l3;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l6;
            i1 = l3;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l3 = i0;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 16u, j1);
            i0 = l3;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            l6 = i1;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l3 = i0;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 32u, j1);
            i0 = l3;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 40u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 28u);
            i1 = state;
            i1 = LoadU32(ctx, i1 + 8u);
            l2 = i1;
            i0 -= i1;
            i1 = 48u;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            l3 = i0;
          }
          i0 = l3;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = l10;
            i1 = 40u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = state;
              i1 = 3u;
              GrowValueStack(ctx, i0, i1);
              i0 = 40u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            if (i0) {
              i0 = l2;
            } else {
              i0 = state;
              i0 = LoadU32(ctx, i0 + 8u);
            }
            l2 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = 48u;
            i1 += i2;
            l3 = i1;
            StoreU32(ctx, i0 + 8u, i1);
          }
          i0 = l10;
          i1 = 41u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = 1u;
            CallValue(ctx, i0, i1, i2);
            i0 = 41u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = state;
            i1 = LoadU32(ctx, i1 + 8u);
            i2 = 16u;
            i1 -= i2;
            l2 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = l5;
            i1 = l4;
            i0 -= i1;
            l4 = i0;
            i1 = state;
            i1 = LoadU32(ctx, i1 + 32u);
            i0 += i1;
            l5 = i0;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l5;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            l3 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_260:;
          i0 = l10;
          i1 = 42u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = l2;
            RaiseArithmeticError(ctx, i0, i1, i2);
            i0 = 42u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_41:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l6;
            i1 = l3;
            i2 = 6u;
            i1 >>= (i2 & 31);
            i2 = 255u;
            i1 &= i2;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l2 = i0;
            i0 = 1u;
            l5 = i0;
            i0 = l6;
            i1 = l3;
            i2 = 23u;
            i1 >>= (i2 & 31);
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l4 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            l3 = i0;
            switch (i0) {
              case 0: goto block_272;
              case 1: goto block_274;
              default: goto block_273;
            }
            block_274:;
            i0 = l4;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i0 = !(i0);
            l5 = i0;
            goto block_272;
            block_273:;
            i0 = 0u;
            l5 = i0;
            block_272:;
            i0 = l2;
            i1 = l5;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 1u;
            StoreU32(ctx, i0 + 8u, i1);
            goto loop_6;
          }
          block_40:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l6;
            i1 = l3;
            i2 = 6u;
            i1 >>= (i2 & 31);
            i2 = 255u;
            i1 &= i2;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l4 = i0;
            i0 = l6;
            i1 = l3;
            i2 = 19u;
            i1 >>= (i2 & 31);
            i2 = 8176u;
            i1 &= i2;
            i0 += i1;
            l2 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            i1 = 4u;
            i0 -= i1;
            switch (i0) {
              case 0: goto block_277;
              case 1: goto block_278;
              default: goto block_276;
            }
            block_278:;
            i0 = l4;
            i1 = l2;
            i1 = LoadU32(ctx, i1);
            i1 = FindNumericKeySlot(ctx, i1);
            l3 = i1;
            d1 = (double)(int32_t)(i1);
            StoreF64(ctx, i0, d1);
            i0 = l4;
            i1 = 3u;
            StoreU32(ctx, i0 + 8u, i1);
            goto loop_6;
            block_277:;
            i0 = l4;
            i1 = l2;
            i1 = LoadU32(ctx, i1);
            i1 = LoadU32(ctx, i1 + 12u);
            l3 = i1;
            d1 = (double)(i1);
            StoreF64(ctx, i0, d1);
            i0 = l4;
            i1 = 3u;
            StoreU32(ctx, i0 + 8u, i1);
            goto loop_6;
            block_276:;
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = state;
            i1 = l2;
            i2 = 12u;
            i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
            l5 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            i0 = !(i0);
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            if (i0) {
              i0 = state;
              i1 = kF1701Const2B328;
              i2 = 12u;
              i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
              l5 = i0;
              i0 = LoadU32(ctx, i0 + 8u);
              i0 = !(i0);
              l3 = i0;
              if (i0) {goto block_279;}
            }
            i0 = state;
            i0 = LoadU32(ctx, i0 + 32u);
            l6 = i0;
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l3 = i0;
            i1 = l5;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l3;
            i1 = l5;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l5 = i0;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 16u, j1);
            i0 = l5;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 24u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l2 = i0;
            j1 = l18;
            StoreU64(ctx, i0 + 32u, j1);
            i0 = l2;
            i1 = l17;
            StoreU32(ctx, i0 + 40u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 28u);
            i1 = state;
            i1 = LoadU32(ctx, i1 + 8u);
            l3 = i1;
            i0 -= i1;
            i1 = 48u;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            l5 = i0;
          }
          i0 = l5;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = l10;
            i1 = 43u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = state;
              i1 = 3u;
              GrowValueStack(ctx, i0, i1);
              i0 = 43u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            if (i0) {
              i0 = l3;
            } else {
              i0 = state;
              i0 = LoadU32(ctx, i0 + 8u);
            }
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l3;
            i2 = 48u;
            i1 += i2;
            l5 = i1;
            StoreU32(ctx, i0 + 8u, i1);
          }
          i0 = l10;
          i1 = 44u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l3;
            i2 = 1u;
            CallValue(ctx, i0, i1, i2);
            i0 = 44u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = state;
            i1 = LoadU32(ctx, i1 + 8u);
            i2 = 16u;
            i1 -= i2;
            l2 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = l4;
            i1 = l6;
            i0 -= i1;
            l4 = i0;
            i1 = state;
            i1 = LoadU32(ctx, i1 + 32u);
            i0 += i1;
            l5 = i0;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l5;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            l3 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_279:;
          i0 = l10;
          i1 = 45u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = kF1701MsgGetLengthOf;
            RaiseTypeError(ctx, i0, i1, i2);
            i0 = 45u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_39:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = l3;
            i1 = 14u;
            i0 >>= (i1 & 31);
            i1 = 511u;
            i0 &= i1;
            l2 = i0;
            i1 = l3;
            i2 = 23u;
            i1 >>= (i2 & 31);
            l5 = i1;
            i0 -= i1;
            i1 = 1u;
            i0 += i1;
            l4 = i0;
          }
          i0 = l10;
          i1 = 46u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l4;
            i2 = l2;
            FormatNumberValue(ctx, i0, i1, i2);
            i0 = 46u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0 + 16u);
            l2 = i0;
            i0 = LoadU32(ctx, i0 + 68u);
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 64u);
            l2 = i1;
            i0 = i0 >= i1;
            l4 = i0;
          }
          i0 = l4;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = l10;
            i1 = 47u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = state;
              GrowTableSlot(ctx, i0);
              i0 = 47u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            i1 = l3;
            i2 = 6u;
            i1 >>= (i2 & 31);
            i2 = 255u;
            i1 &= i2;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l2 = i0;
            i1 = l6;
            i2 = l5;
            i3 = 4u;
            i2 <<= (i3 & 31);
            i1 += i2;
            l5 = i1;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l2;
            i1 = l5;
            i1 = LoadU32(ctx, i1 + 8u);
            l3 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            goto loop_6;
          }
          block_38:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l9;
            i1 = l3;
            i2 = 12u;
            i1 >>= (i2 & 31);
            i2 = kF1701ConstFFFFC;
            i1 &= i2;
            i0 += i1;
            l3 = i0;
            i1 = kF1701Const7FFFC;
            i0 -= i1;
            l9 = i0;
            goto loop_6;
          }
          block_37:;
          i0 = l16;
          i1 = ResumePhase(ctx);
          i1 = !(i1);
          if (i1) {
            i1 = state;
            i2 = l9;
            StoreU32(ctx, i1 + 24u, i2);
            i1 = l6;
            i2 = l3;
            i3 = 19u;
            i2 >>= (i3 & 31);
            l2 = i2;
            i3 = 8176u;
            i2 &= i3;
            i1 += i2;
            i2 = l13;
            i3 = l2;
            i4 = 4080u;
            i3 &= i4;
            i2 += i3;
            i3 = l3;
            i4 = kF1701ConstFFFFFFFF;
            i3 = (uint32_t)((int32_t)i3 > (int32_t)i4);
            i1 = i3 ? i1 : i2;
            l2 = i1;
            i1 = LoadU32(ctx, i1 + 8u);
            i2 = l13;
            i3 = l3;
            i4 = 10u;
            i3 >>= (i4 & 31);
            l5 = i3;
            i4 = 4080u;
            i3 &= i4;
            i2 += i3;
            l12 = i2;
            i3 = l6;
            i4 = l5;
            i5 = 8176u;
            i4 &= i5;
            i3 += i4;
            i4 = l3;
            i5 = kF1701Const400000;
            i4 &= i5;
            i2 = i4 ? i2 : i3;
            l5 = i2;
            i2 = LoadU32(ctx, i2 + 8u);
            l6 = i2;
            i1 = i1 == i2;
            l4 = i1;
          }
          i1 = l4;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          l16 = i0;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = l10;
            i1 = 48u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = state;
              i1 = l2;
              i2 = l5;
              i0 = CompileComparison(ctx, i0, i1, i2);
              l8 = i0;
              i0 = 48u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
              i0 = l8;
              l4 = i0;
            }
            i0 = l4;
            i1 = l4;
            i2 = 0u;
            i1 = i1 != i2;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            l4 = i0;
          }
          i0 = l4;
          i1 = 0u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          i1 = l4;
          i2 = l16;
          i2 = !(i2);
          i3 = ResumePhase(ctx);
          i4 = 2u;
          i3 = i3 == i4;
          i2 |= i3;
          i0 = i2 ? i0 : i1;
          l4 = i0;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l4;
            i1 = l3;
            i2 = 6u;
            i1 >>= (i2 & 31);
            i2 = 255u;
            i1 &= i2;
            i0 = i0 == i1;
            l3 = i0;
            if (i0) {
              i0 = l9;
              i1 = l9;
              i1 = LoadU32(ctx, i1);
              i2 = 12u;
              i1 >>= (i2 & 31);
              i2 = kF1701ConstFFFFC;
              i1 &= i2;
              i0 += i1;
              l3 = i0;
              i1 = kF1701Const7FFFC;
              i0 -= i1;
            } else {
              i0 = l9;
            }
            i1 = kF1701ResumeWordBytes;
            i0 += i1;
            l9 = i0;
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_36:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = l6;
            i1 = l3;
            i2 = 19u;
            i1 >>= (i2 & 31);
            l2 = i1;
            i2 = 8176u;
            i1 &= i2;
            i0 += i1;
            i1 = l13;
            i2 = l2;
            i3 = 4080u;
            i2 &= i3;
            i1 += i2;
            i2 = l3;
            i3 = kF1701ConstFFFFFFFF;
            i2 = (uint32_t)((int32_t)i2 > (int32_t)i3);
            i0 = i2 ? i0 : i1;
            l4 = i0;
            i0 = l13;
            i1 = l3;
            i2 = 10u;
            i1 >>= (i2 & 31);
            l2 = i1;
            i2 = 4080u;
            i1 &= i2;
            i0 += i1;
            i1 = l6;
            i2 = l2;
            i3 = 8176u;
            i2 &= i3;
            i1 += i2;
            l2 = i1;
            i2 = l3;
            i3 = kF1701Const400000;
            i2 &= i3;
            l6 = i2;
            i0 = i2 ? i0 : i1;
            l5 = i0;
          }
          i0 = l10;
          i1 = 49u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l4;
            i2 = l5;
            i0 = CompareConstantOperands(ctx, i0, i1, i2);
            l8 = i0;
            i0 = 49u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l4 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l4;
            i1 = l3;
            i2 = 6u;
            i1 >>= (i2 & 31);
            i2 = 255u;
            i1 &= i2;
            i0 = i0 == i1;
            l3 = i0;
            if (i0) {
              i0 = l9;
              i1 = l9;
              i1 = LoadU32(ctx, i1);
              i2 = 12u;
              i1 >>= (i2 & 31);
              i2 = kF1701ConstFFFFC;
              i1 &= i2;
              i0 += i1;
              l3 = i0;
              i1 = kF1701Const7FFFC;
              i0 -= i1;
            } else {
              i0 = l9;
            }
            i1 = kF1701ResumeWordBytes;
            i0 += i1;
            l9 = i0;
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_35:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = l3;
            i1 = 6u;
            i0 >>= (i1 & 31);
            i1 = 255u;
            i0 &= i1;
            l12 = i0;
            i0 = l6;
            i1 = l3;
            i2 = 19u;
            i1 >>= (i2 & 31);
            l2 = i1;
            i2 = 8176u;
            i1 &= i2;
            i0 += i1;
            i1 = l13;
            i2 = l2;
            i3 = 4080u;
            i2 &= i3;
            i1 += i2;
            i2 = l3;
            i3 = kF1701ConstFFFFFFFF;
            i2 = (uint32_t)((int32_t)i2 > (int32_t)i3);
            i0 = i2 ? i0 : i1;
            l2 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            l4 = i0;
            i1 = l13;
            i2 = l3;
            i3 = 10u;
            i2 >>= (i3 & 31);
            l5 = i2;
            i3 = 4080u;
            i2 &= i3;
            i1 += i2;
            l7 = i1;
            i2 = l6;
            i3 = l5;
            i4 = 8176u;
            i3 &= i4;
            i2 += i3;
            i3 = l3;
            i4 = kF1701Const400000;
            i3 &= i4;
            i1 = i3 ? i1 : i2;
            l5 = i1;
            i1 = LoadU32(ctx, i1 + 8u);
            i0 = i0 != i1;
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            if (i0) {goto block_312;}
            i0 = l4;
            i1 = 3u;
            i0 -= i1;
            l3 = i0;
            switch (i0) {
              case 0: goto block_316;
              case 1: goto block_315;
              default: goto block_314;
            }
            block_316:;
            i0 = l2;
            d0 = LoadF64(ctx, i0);
            i1 = l5;
            d1 = LoadF64(ctx, i1);
            i0 = d0 <= d1;
            l2 = i0;
            goto block_311;
            block_315:;
            i0 = l2;
            i0 = LoadU32(ctx, i0);
            l4 = i0;
            i1 = 16u;
            i0 += i1;
            l3 = i0;
            i1 = l5;
            i1 = LoadU32(ctx, i1);
            l5 = i1;
            i2 = 16u;
            i1 += i2;
            l6 = i1;
            i0 = CompareScriptStrings(ctx, i0, i1);
            l2 = i0;
            if (i0) {goto block_317;}
            i0 = l4;
            i0 = LoadU32(ctx, i0 + 12u);
            l4 = i0;
            i0 = l5;
            i0 = LoadU32(ctx, i0 + 12u);
            l5 = i0;
            loop_318: 
              i0 = l5;
              i1 = l3;
              i1 = StringLength(ctx, i1);
              l2 = i1;
              i0 = i0 == i1;
              l7 = i0;
              if (i0) {
                i0 = l4;
                i1 = l5;
                i0 = i0 != i1;
                l2 = i0;
                goto block_317;
              }
              i0 = l2;
              i1 = l4;
              i0 = i0 == i1;
              l7 = i0;
              if (i0) {
                i0 = kF1701ConstFFFFFFFF;
                l2 = i0;
                goto block_317;
              }
              i0 = l5;
              i1 = l2;
              i2 = 1u;
              i1 += i2;
              l2 = i1;
              i0 -= i1;
              l5 = i0;
              i0 = l4;
              i1 = l2;
              i0 -= i1;
              l4 = i0;
              i0 = l2;
              i1 = l3;
              i0 += i1;
              l3 = i0;
              i1 = l2;
              i2 = l6;
              i1 += i2;
              l6 = i1;
              i0 = CompareScriptStrings(ctx, i0, i1);
              l2 = i0;
              i0 = !(i0);
              l7 = i0;
              if (i0) {goto loop_318;}
            block_317:;
            i0 = l2;
            i1 = 1u;
            i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
            l2 = i0;
            goto block_311;
            block_314:;
            i0 = state;
            i1 = l2;
            i2 = 14u;
            i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
            l4 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            i0 = !(i0);
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            if (i0) {goto block_321;}
            i0 = l4;
            i1 = state;
            i2 = l5;
            i3 = 14u;
            i1 = FetchObjectMetamethod(ctx, i1, i2, i3);
            i0 = RawValueEqual(ctx, i0, i1);
            i0 = !(i0);
            l3 = i0;
            if (i0) {goto block_321;}
            i0 = state;
            i0 = LoadU32(ctx, i0 + 32u);
            l6 = i0;
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l3 = i0;
            i1 = l4;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l3;
            i1 = l4;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l4 = i0;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 16u, j1);
            i0 = l4;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 24u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l2 = i0;
            i1 = l5;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 32u, j1);
            i0 = l2;
            i1 = l5;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 40u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 28u);
            i1 = state;
            i1 = LoadU32(ctx, i1 + 8u);
            l2 = i1;
            i0 -= i1;
            i1 = 48u;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            l4 = i0;
          }
          i0 = l4;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = l10;
            i1 = 50u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = state;
              i1 = 3u;
              GrowValueStack(ctx, i0, i1);
              i0 = 50u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            if (i0) {
              i0 = l2;
            } else {
              i0 = state;
              i0 = LoadU32(ctx, i0 + 8u);
            }
            l2 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = 48u;
            i1 += i2;
            l4 = i1;
            StoreU32(ctx, i0 + 8u, i1);
          }
          i0 = l10;
          i1 = 51u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = 1u;
            CallValue(ctx, i0, i1, i2);
            i0 = 51u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = state;
            i1 = LoadU32(ctx, i1 + 8u);
            i2 = 16u;
            i1 -= i2;
            l2 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 32u);
            l4 = i0;
            i1 = l3;
            i2 = l6;
            i1 -= i2;
            i0 += i1;
            l5 = i0;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l5;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            l3 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l5 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            l2 = i0;
            switch (i0) {
              case 0: goto block_311;
              case 1: goto block_330;
              default: goto block_329;
            }
            block_330:;
            i0 = l5;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i1 = 0u;
            i0 = i0 != i1;
            l2 = i0;
            goto block_311;
            block_329:;
            i0 = 1u;
            l2 = i0;
            goto block_311;
          }
          block_321:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l5;
            i2 = 13u;
            i0 = FetchObjectMetamethod(ctx, i0, i1, i2);
            l4 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            i0 = !(i0);
            l3 = i0;
            if (i0) {goto block_312;}
            i0 = l4;
            i1 = state;
            i2 = l2;
            i3 = 13u;
            i1 = FetchObjectMetamethod(ctx, i1, i2, i3);
            i0 = RawValueEqual(ctx, i0, i1);
            i0 = !(i0);
            l3 = i0;
            if (i0) {goto block_312;}
            i0 = state;
            i0 = LoadU32(ctx, i0 + 32u);
            l7 = i0;
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l6 = i0;
            i1 = l4;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l6;
            i1 = l4;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l4 = i0;
            i1 = l5;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 16u, j1);
            i0 = l4;
            i1 = l5;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 24u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l5 = i0;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 32u, j1);
            i0 = l5;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 40u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 28u);
            i1 = state;
            i1 = LoadU32(ctx, i1 + 8u);
            l3 = i1;
            i0 -= i1;
            i1 = 48u;
            i0 = (uint32_t)((int32_t)i0 <= (int32_t)i1);
            l4 = i0;
          }
          i0 = l4;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = l10;
            i1 = 52u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = state;
              i1 = 3u;
              GrowValueStack(ctx, i0, i1);
              i0 = 52u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
            i0 = ResumePhase(ctx);
            if (i0) {
              i0 = l3;
            } else {
              i0 = state;
              i0 = LoadU32(ctx, i0 + 8u);
            }
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l3;
            i2 = 48u;
            i1 += i2;
            l4 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = 1u;
            l2 = i0;
          }
          i0 = l10;
          i1 = 53u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l3;
            i2 = 1u;
            CallValue(ctx, i0, i1, i2);
            i0 = 53u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = state;
            i1 = LoadU32(ctx, i1 + 8u);
            i2 = 16u;
            i1 -= i2;
            l5 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 32u);
            i1 = l6;
            i2 = l7;
            i1 -= i2;
            i0 += i1;
            l4 = i0;
            i1 = l5;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0, j1);
            i0 = l4;
            i1 = l5;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 8u);
            l5 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            l3 = i0;
            switch (i0) {
              case 0: goto block_311;
              case 1: goto block_339;
              default: goto block_338;
            }
            block_339:;
            i0 = l5;
            i0 = LoadU32(ctx, i0);
            l3 = i0;
            i0 = !(i0);
            l2 = i0;
            goto block_311;
            block_338:;
            i0 = 0u;
            l2 = i0;
            goto block_311;
          }
          block_312:;
          i0 = l10;
          i1 = 54u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = l5;
            i0 = CheckComparableTypes(ctx, i0, i1, i2);
            l8 = i0;
            i0 = 54u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l2 = i0;
          }
          block_311:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            i1 = l12;
            i0 = i0 == i1;
            l3 = i0;
            if (i0) {
              i0 = l9;
              i1 = l9;
              i1 = LoadU32(ctx, i1);
              i2 = 12u;
              i1 >>= (i2 & 31);
              i2 = kF1701ConstFFFFC;
              i1 &= i2;
              i0 += i1;
              l3 = i0;
              i1 = kF1701Const7FFFC;
              i0 -= i1;
            } else {
              i0 = l9;
            }
            i1 = kF1701ResumeWordBytes;
            i0 += i1;
            l9 = i0;
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_34:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = 1u;
            l2 = i0;
            i0 = l6;
            i1 = l3;
            i2 = 6u;
            i1 >>= (i2 & 31);
            i2 = 255u;
            i1 &= i2;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l5 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            l4 = i0;
            switch (i0) {
              case 0: goto block_344;
              case 1: goto block_346;
              default: goto block_345;
            }
            block_346:;
            i0 = l5;
            i0 = LoadU32(ctx, i0);
            l4 = i0;
            i0 = !(i0);
            l2 = i0;
            goto block_344;
            block_345:;
            i0 = 0u;
            l2 = i0;
            block_344:;
            i0 = l2;
            i1 = l3;
            i2 = 14u;
            i1 >>= (i2 & 31);
            i2 = 511u;
            i1 &= i2;
            i0 = i0 != i1;
            l3 = i0;
            if (i0) {
              i0 = l9;
              i1 = l9;
              i1 = LoadU32(ctx, i1);
              i2 = 12u;
              i1 >>= (i2 & 31);
              i2 = kF1701ConstFFFFC;
              i1 &= i2;
              i0 += i1;
              l3 = i0;
              i1 = kF1701Const7FFFC;
              i0 -= i1;
            } else {
              i0 = l9;
            }
            i1 = kF1701ResumeWordBytes;
            i0 += i1;
            l9 = i0;
            goto loop_6;
          }
          block_33:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = 1u;
            l2 = i0;
            i0 = l6;
            i1 = l3;
            i2 = 19u;
            i1 >>= (i2 & 31);
            i2 = 8176u;
            i1 &= i2;
            i0 += i1;
            l5 = i0;
            i0 = LoadU32(ctx, i0 + 8u);
            l4 = i0;
            switch (i0) {
              case 0: goto block_349;
              case 1: goto block_351;
              default: goto block_350;
            }
            block_351:;
            i0 = l5;
            i0 = LoadU32(ctx, i0);
            i0 = !(i0);
            l2 = i0;
            goto block_349;
            block_350:;
            i0 = 0u;
            l2 = i0;
            block_349:;
            i0 = l3;
            i1 = 14u;
            i0 >>= (i1 & 31);
            i1 = 511u;
            i0 &= i1;
            l12 = i0;
            i1 = l2;
            i0 = i0 != i1;
            l2 = i0;
            if (i0) {
              i0 = l6;
              i1 = l3;
              i2 = 6u;
              i1 >>= (i2 & 31);
              i2 = 255u;
              i1 &= i2;
              l2 = i1;
              i2 = 4u;
              i1 <<= (i2 & 31);
              i0 += i1;
              i1 = l5;
              j1 = LoadU64(ctx, i1);
              StoreU64(ctx, i0, j1);
              i0 = l6;
              i1 = l2;
              i2 = 4u;
              i1 <<= (i2 & 31);
              i0 += i1;
              i1 = l4;
              StoreU32(ctx, i0 + 8u, i1);
              i0 = l9;
              i1 = l9;
              i1 = LoadU32(ctx, i1);
              i2 = 12u;
              i1 >>= (i2 & 31);
              i2 = kF1701ConstFFFFC;
              i1 &= i2;
              i0 += i1;
              l3 = i0;
              i1 = kF1701Const7FFFC;
              i0 -= i1;
            } else {
              i0 = l9;
            }
            i1 = kF1701ResumeWordBytes;
            i0 += i1;
            l9 = i0;
            goto loop_6;
          }
          block_32:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i1 = 14u;
            i0 >>= (i1 & 31);
            i1 = 511u;
            i0 &= i1;
            l5 = i0;
            i1 = 1u;
            i0 -= i1;
            l4 = i0;
            i0 = l6;
            i1 = l3;
            i2 = 2u;
            i1 >>= (i2 & 31);
            i2 = 4080u;
            i1 &= i2;
            i0 += i1;
            l2 = i0;
            i0 = l3;
            i1 = 23u;
            i0 >>= (i1 & 31);
            l3 = i0;
            if (i0) {
              i0 = state;
              i1 = l2;
              i2 = l3;
              i3 = 4u;
              i2 <<= (i3 & 31);
              i1 += i2;
              l3 = i1;
              StoreU32(ctx, i0 + 8u, i1);
            }
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
          }
          i0 = l10;
          i1 = 55u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l2;
            i2 = l4;
            i0 = LuaEngine::Call(ctx, i0, i1, i2);
            l8 = i0;
            i0 = 55u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l2 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            i1 = 1u;
            i0 = i0 != i1;
            l3 = i0;
            if (i0) {
              i0 = l2;
              if (i0) {goto block_9;}
              i0 = expression;
              i1 = 1u;
              i0 += i1;
              expression = i0;
              goto loop_4;
            }
            i0 = l5;
            if (i0) {
              i0 = state;
              i1 = state;
              i1 = LoadU32(ctx, i1 + 20u);
              i1 = LoadU32(ctx, i1 + 8u);
              l3 = i1;
              StoreU32(ctx, i0 + 8u, i1);
            }
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_31:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l6;
            i1 = l3;
            i2 = 2u;
            i1 >>= (i2 & 31);
            i2 = 4080u;
            i1 &= i2;
            l4 = i1;
            i0 += i1;
            l2 = i0;
            i0 = l3;
            i1 = 23u;
            i0 >>= (i1 & 31);
            l5 = i0;
            if (i0) {
              i0 = state;
              i1 = l2;
              i2 = l5;
              i3 = 4u;
              i2 <<= (i3 & 31);
              i1 += i2;
              l3 = i1;
              StoreU32(ctx, i0 + 8u, i1);
            }
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
          }
          i0 = l3;
          i1 = l10;
          i2 = 56u;
          i1 = i1 == i2;
          i2 = 1u;
          i3 = ResumePhase(ctx);
          i1 = i3 ? i1 : i2;
          if (i1) {
            i1 = state;
            i2 = l2;
            i3 = kF1701ConstFFFFFFFF;
            i1 = LuaEngine::Call(ctx, i1, i2, i3);
            l8 = i1;
            i1 = 56u;
            i2 = ResumePhase(ctx);
            i3 = 1u;
            i2 = i2 == i3;
            if (i2) {i0 = i1; goto spill_frame;}
            i1 = l8;
            l2 = i1;
          }
          i1 = l2;
          i2 = 1u;
          i1 = i1 != i2;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          l3 = i0;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l2;
              if (i0) {goto block_9;}
              i0 = state;
              i0 = LoadU32(ctx, i0 + 20u);
              l4 = i0;
              i1 = 24u;
              i0 -= i1;
              l2 = i0;
              i0 = l4;
              i0 = LoadU32(ctx, i0 + 4u);
              l9 = i0;
              i0 = state;
              i0 = LoadU32(ctx, i0 + 104u);
              l12 = i0;
              i0 = l4;
              i1 = 20u;
              i0 -= i1;
              l6 = i0;
              i0 = LoadU32(ctx, i0);
              l3 = i0;
              l5 = i0;
            }
            i0 = l12;
            i1 = ResumePhase(ctx);
            i2 = 2u;
            i1 = i1 == i2;
            i0 |= i1;
            if (i0) {
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = l2;
                i0 = LoadU32(ctx, i0);
                l5 = i0;
              }
              i0 = l10;
              i1 = 57u;
              i0 = i0 == i1;
              i1 = 1u;
              i2 = ResumePhase(ctx);
              i0 = i2 ? i0 : i1;
              if (i0) {
                i0 = state;
                i1 = l5;
                ReleaseNodeChain(ctx, i0, i1);
                i0 = 57u;
                i1 = ResumePhase(ctx);
                i2 = 1u;
                i1 = i1 == i2;
                if (i1) {goto spill_frame;}
              }
              i0 = ResumePhase(ctx);
              if (i0) {
                i0 = l5;
              } else {
                i0 = l6;
                i0 = LoadU32(ctx, i0);
              }
              l5 = i0;
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l2;
              i1 = l5;
              i2 = l4;
              i2 = LoadU32(ctx, i2);
              i3 = l9;
              i2 -= i3;
              i1 += i2;
              l2 = i1;
              StoreU32(ctx, i0, i1);
              i0 = state;
              i1 = l2;
              StoreU32(ctx, i0 + 12u, i1);
              i0 = 0u;
              l2 = i0;
              i0 = l9;
              l5 = i0;
              i1 = state;
              i1 = LoadU32(ctx, i1 + 8u);
              i0 = i0 < i1;
              l6 = i0;
              if (i0) {
                loop_371: 
                  i0 = l2;
                  i1 = 4u;
                  i0 <<= (i1 & 31);
                  l6 = i0;
                  i1 = l3;
                  i0 += i1;
                  l13 = i0;
                  i1 = l5;
                  j1 = LoadU64(ctx, i1);
                  StoreU64(ctx, i0, j1);
                  i0 = l13;
                  i1 = l6;
                  i2 = l9;
                  i1 += i2;
                  i1 = LoadU32(ctx, i1 + 8u);
                  StoreU32(ctx, i0 + 8u, i1);
                  i0 = l9;
                  i1 = l2;
                  i2 = 1u;
                  i1 += i2;
                  l2 = i1;
                  i2 = 4u;
                  i1 <<= (i2 & 31);
                  i0 += i1;
                  l5 = i0;
                  i1 = state;
                  i1 = LoadU32(ctx, i1 + 8u);
                  i0 = i0 < i1;
                  l6 = i0;
                  if (i0) {goto loop_371;}
              }
              i0 = state;
              i1 = l2;
              i2 = 4u;
              i1 <<= (i2 & 31);
              i2 = l3;
              i1 += i2;
              l2 = i1;
              StoreU32(ctx, i0 + 8u, i1);
              i0 = l4;
              i1 = 16u;
              i0 -= i1;
              i1 = l2;
              StoreU32(ctx, i0, i1);
              i0 = l4;
              i1 = 12u;
              i0 -= i1;
              i1 = state;
              i1 = LoadU32(ctx, i1 + 24u);
              l9 = i1;
              StoreU32(ctx, i0, i1);
              i0 = l4;
              i1 = 4u;
              i0 -= i1;
              l2 = i0;
              i0 = LoadU32(ctx, i0);
              i1 = 1u;
              i0 += i1;
              l3 = i0;
              i0 = l2;
              i1 = l3;
              StoreU32(ctx, i0, i1);
              i0 = state;
              i1 = state;
              i1 = LoadU32(ctx, i1 + 20u);
              i2 = 24u;
              i1 -= i2;
              l3 = i1;
              StoreU32(ctx, i0 + 20u, i1);
              goto loop_4;
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_30:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l6;
            i1 = l3;
            i2 = 2u;
            i1 >>= (i2 & 31);
            i2 = 4080u;
            i1 &= i2;
            l4 = i1;
            i0 += i1;
            l2 = i0;
            i0 = l3;
            i1 = 23u;
            i0 >>= (i1 & 31);
            l5 = i0;
            if (i0) {
              i0 = state;
              i1 = l2;
              i2 = l5;
              i3 = 4u;
              i2 <<= (i3 & 31);
              i1 += i2;
              i2 = 16u;
              i1 -= i2;
              StoreU32(ctx, i0 + 8u, i1);
            }
            i0 = state;
            i0 = LoadU32(ctx, i0 + 104u);
            l3 = i0;
          }
          i0 = l3;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = l10;
            i1 = 58u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = state;
              i1 = l6;
              ReleaseNodeChain(ctx, i0, i1);
              i0 = 58u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
          }
          i0 = l10;
          i1 = 59u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l2;
            i0 = CompactEntryQueue(ctx, i0, i1);
            l8 = i0;
            i0 = 59u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l2 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = expression;
            i1 = 1u;
            i0 -= i1;
            expression = i0;
            i0 = !(i0);
            l3 = i0;
            if (i0) {goto block_9;}
            i0 = l2;
            i0 = !(i0);
            l3 = i0;
            if (i0) {goto loop_4;}
            i0 = state;
            i1 = state;
            i1 = LoadU32(ctx, i1 + 20u);
            i1 = LoadU32(ctx, i1 + 8u);
            l3 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            goto loop_4;
          }
          block_29:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l6;
            i1 = l3;
            i2 = 6u;
            i1 >>= (i2 & 31);
            i2 = 255u;
            i1 &= i2;
            l5 = i1;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l2 = i0;
            d0 = LoadF64(ctx, i0 + 32u);
            l22 = d0;
            i1 = l2;
            d1 = LoadF64(ctx, i1);
            d0 += d1;
            l20 = d0;
            i0 = l2;
            d0 = LoadF64(ctx, i0 + 16u);
            l21 = d0;
            d0 = l22;
            d1 = 0.0;
            i0 = d0 > d1;
            i1 = 1u;
            i0 ^= i1;
            i0 = !(i0);
            if (i0) {
              d0 = l20;
              d1 = l21;
              i0 = d0 <= d1;
              i1 = 1u;
              i0 ^= i1;
              l4 = i0;
              if (i0) {goto loop_6;}
              goto block_382;
            }
            d0 = l20;
            d1 = l21;
            i0 = d0 >= d1;
            i1 = 1u;
            i0 ^= i1;
            l4 = i0;
            if (i0) {goto loop_6;}
            block_382:;
            i0 = l2;
            d1 = l20;
            StoreF64(ctx, i0, d1);
            i0 = l6;
            i1 = l5;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l4 = i0;
            i1 = 3u;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = l2;
            i1 = 3u;
            StoreU32(ctx, i0 + 56u, i1);
            i0 = l2;
            d1 = l20;
            StoreF64(ctx, i0 + 48u, d1);
            i0 = l9;
            i1 = l3;
            i2 = 12u;
            i1 >>= (i2 & 31);
            i2 = kF1701ConstFFFFC;
            i1 &= i2;
            i0 += i1;
            l3 = i0;
            i1 = kF1701Const7FFFC;
            i0 -= i1;
            l9 = i0;
            goto loop_6;
          }
          block_28:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = l6;
            i1 = l3;
            i2 = 2u;
            i1 >>= (i2 & 31);
            i2 = 4080u;
            i1 &= i2;
            i0 += i1;
            l2 = i0;
            l5 = i0;
            i0 = l2;
            i0 = LoadU32(ctx, i0 + 8u);
            l4 = i0;
            i1 = 3u;
            i0 = i0 != i1;
            l12 = i0;
          }
          i0 = l12;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l4;
              i1 = 4u;
              i0 = i0 != i1;
              l4 = i0;
              if (i0) {goto block_384;}
              i0 = l2;
              i0 = LoadU32(ctx, i0);
              i1 = 16u;
              i0 += i1;
              l4 = i0;
            }
            i0 = l10;
            i1 = 60u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = l4;
              i1 = l11;
              i0 = CoerceToNumber(ctx, i0, i1);
              l8 = i0;
              i0 = 60u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
              i0 = l8;
              l4 = i0;
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l4;
              i0 = !(i0);
              l4 = i0;
              if (i0) {goto block_384;}
              i0 = l2;
              i1 = l11;
              d1 = LoadF64(ctx, i1);
              StoreF64(ctx, i0, d1);
              i0 = l5;
              i1 = 3u;
              StoreU32(ctx, i0 + 8u, i1);
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            i0 = LoadU32(ctx, i0 + 24u);
            l4 = i0;
            i1 = 3u;
            i0 = i0 != i1;
            l12 = i0;
          }
          i0 = l12;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l4;
              i1 = 4u;
              i0 = i0 != i1;
              l4 = i0;
              if (i0) {goto block_18;}
              i0 = l2;
              i0 = LoadU32(ctx, i0 + 16u);
              i1 = 16u;
              i0 += i1;
              l4 = i0;
            }
            i0 = l10;
            i1 = 61u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = l4;
              i1 = l11;
              i0 = CoerceToNumber(ctx, i0, i1);
              l8 = i0;
              i0 = 61u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
              i0 = l8;
              l4 = i0;
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l4;
              i0 = !(i0);
              l4 = i0;
              if (i0) {goto block_18;}
              i0 = l11;
              d0 = LoadF64(ctx, i0);
              l20 = d0;
              i0 = l2;
              i1 = 3u;
              StoreU32(ctx, i0 + 24u, i1);
              i0 = l2;
              d1 = l20;
              StoreF64(ctx, i0 + 16u, d1);
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            i0 = LoadU32(ctx, i0 + 40u);
            l4 = i0;
            i1 = 3u;
            i0 = i0 == i1;
            l12 = i0;
            if (i0) {goto block_8;}
            i0 = l4;
            i1 = 4u;
            i0 = i0 != i1;
            l4 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l4;
            if (i0) {goto block_398;}
            i0 = l2;
            i0 = LoadU32(ctx, i0 + 32u);
            i1 = 16u;
            i0 += i1;
            l4 = i0;
          }
          i0 = l10;
          i1 = 62u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = l4;
            i1 = l11;
            i0 = CoerceToNumber(ctx, i0, i1);
            l8 = i0;
            i0 = 62u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l4 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l4;
            i0 = !(i0);
            l4 = i0;
            if (i0) {goto block_398;}
            i0 = l11;
            d0 = LoadF64(ctx, i0);
            l20 = d0;
            i0 = l2;
            i1 = 3u;
            StoreU32(ctx, i0 + 40u, i1);
            i0 = l2;
            d1 = l20;
            StoreF64(ctx, i0 + 32u, d1);
            goto block_8;
          }
          block_398:;
          i0 = l10;
          i1 = 63u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = kF1701MsgForStepMustBe;
            i2 = 0u;
            RaiseScriptError(ctx, i0, i1, i2);
            i0 = 63u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            TrapUnreachable();
          }
          block_384:;
          i0 = l10;
          i1 = 64u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = kF1701MsgForInitialValueMust;
            i2 = 0u;
            RaiseScriptError(ctx, i0, i1, i2);
            i0 = 64u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {goto block_8;}
          block_27:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l6;
            i1 = l3;
            i2 = 2u;
            i1 >>= (i2 & 31);
            i2 = 4080u;
            i1 &= i2;
            l5 = i1;
            i0 += i1;
            l2 = i0;
            j0 = LoadU64(ctx, i0 + 32u);
            l19 = j0;
            i0 = l2;
            j1 = l19;
            StoreU64(ctx, i0 + 80u, j1);
            i0 = l2;
            i1 = kF1701ConstFFFFFFC0;
            i0 -= i1;
            i1 = l2;
            j1 = LoadU64(ctx, i1 + 16u);
            StoreU64(ctx, i0, j1);
            i0 = l2;
            i1 = l2;
            j1 = LoadU64(ctx, i1);
            StoreU64(ctx, i0 + 48u, j1);
            i0 = l2;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 8u);
            StoreU32(ctx, i0 + 56u, i1);
            i0 = l2;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 40u);
            StoreU32(ctx, i0 + 88u, i1);
            i0 = l2;
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 24u);
            StoreU32(ctx, i0 + 72u, i1);
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = state;
            i1 = l2;
            i2 = 96u;
            i1 += i2;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = l2;
            i1 = 48u;
            i0 += i1;
            l4 = i0;
            i0 = l3;
            i1 = 14u;
            i0 >>= (i1 & 31);
            i1 = 511u;
            i0 &= i1;
            l3 = i0;
          }
          i0 = l10;
          i1 = 65u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l4;
            i2 = l3;
            CallValue(ctx, i0, i1, i2);
            i0 = 65u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i1 = state;
            i1 = LoadU32(ctx, i1 + 20u);
            i1 = LoadU32(ctx, i1 + 8u);
            l3 = i1;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            i1 = l5;
            i0 += i1;
            l2 = i0;
            i0 = LoadU32(ctx, i0 + 56u);
            l5 = i0;
            if (i0) {
              i0 = l2;
              i1 = l5;
              StoreU32(ctx, i0 + 40u, i1);
              i0 = l2;
              i1 = l2;
              j1 = LoadU64(ctx, i1 + 48u);
              StoreU64(ctx, i0 + 32u, j1);
              i0 = l9;
              i1 = l9;
              i1 = LoadU32(ctx, i1);
              i2 = 12u;
              i1 >>= (i2 & 31);
              i2 = kF1701ConstFFFFC;
              i1 &= i2;
              i0 += i1;
              l3 = i0;
              i1 = kF1701Const7FFFC;
              i0 -= i1;
            } else {
              i0 = l9;
            }
            i1 = kF1701ResumeWordBytes;
            i0 += i1;
            l9 = i0;
            goto loop_6;
          }
          block_26:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i1 = 14u;
            i0 >>= (i1 & 31);
            i1 = 511u;
            i0 &= i1;
            l5 = i0;
            i0 = l6;
            i1 = l3;
            i2 = 6u;
            i1 >>= (i2 & 31);
            i2 = 255u;
            i1 &= i2;
            l7 = i1;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l12 = i0;
            i0 = l3;
            i1 = 23u;
            i0 >>= (i1 & 31);
            l2 = i0;
            i0 = !(i0);
            if (i0) {
              i0 = state;
              i0 = LoadU32(ctx, i0 + 8u);
              l2 = i0;
              i0 = state;
              i1 = state;
              i1 = LoadU32(ctx, i1 + 20u);
              i1 = LoadU32(ctx, i1 + 8u);
              StoreU32(ctx, i0 + 8u, i1);
              i0 = l2;
              i1 = l12;
              i0 -= i1;
              i1 = 4u;
              i0 = (uint32_t)((int32_t)i0 >> (i1 & 31));
              i1 = 1u;
              i0 -= i1;
              l2 = i0;
            }
            i0 = l5;
            i0 = !(i0);
            if (i0) {
              i0 = l4;
              i1 = 8u;
              i0 += i1;
              l9 = i0;
              i0 = l4;
              i0 = LoadU32(ctx, i0 + 4u);
              l5 = i0;
            }
            i0 = l6;
            i1 = l7;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            i0 = LoadU32(ctx, i0 + 8u);
            i1 = 5u;
            i0 = i0 != i1;
            l3 = i0;
            if (i0) {goto loop_6;}
            i0 = l2;
            i1 = l5;
            i2 = 50u;
            i1 *= i2;
            i0 += i1;
            i1 = 50u;
            i0 -= i1;
            l3 = i0;
            i1 = l12;
            i1 = LoadU32(ctx, i1);
            l4 = i1;
            i1 = LoadU32(ctx, i1 + 28u);
            i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
            l5 = i0;
          }
          i0 = l5;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = l10;
            i1 = 66u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = state;
              i1 = l4;
              i2 = l3;
              StoreTableField(ctx, i0, i1, i2);
              i0 = 66u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l2;
            i1 = 1u;
            i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
            l5 = i0;
            if (i0) {goto loop_6;}
          }
          loop_416: 
            i0 = l10;
            i1 = 67u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = state;
              i1 = l4;
              i2 = l3;
              i0 = GetTableFieldByNumber(ctx, i0, i1, i2);
              l8 = i0;
              i0 = 67u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
              i0 = l8;
              l7 = i0;
            }
            i0 = ResumePhase(ctx);
            i0 = !(i0);
            if (i0) {
              i0 = l2;
              l5 = i0;
              i0 = l7;
              i1 = l12;
              i2 = l2;
              i3 = 4u;
              i2 <<= (i3 & 31);
              i1 += i2;
              l2 = i1;
              j1 = LoadU64(ctx, i1);
              StoreU64(ctx, i0, j1);
              i0 = l7;
              i1 = l2;
              i1 = LoadU32(ctx, i1 + 8u);
              l15 = i1;
              StoreU32(ctx, i0 + 8u, i1);
              i0 = l2;
              i0 = LoadU32(ctx, i0 + 8u);
              i1 = 4u;
              i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
              l7 = i0;
              if (i0) {goto block_419;}
              i0 = l2;
              i0 = LoadU32(ctx, i0);
              i0 = LoadU8(ctx, i0 + 5u);
              i1 = 3u;
              i0 &= i1;
              i0 = !(i0);
              if (i0) {goto block_419;}
              i0 = l4;
              i0 = LoadU8(ctx, i0 + 5u);
              i1 = 4u;
              i0 &= i1;
              i0 = !(i0);
              if (i0) {goto block_419;}
              i0 = state;
              i0 = LoadU32(ctx, i0 + 16u);
              l2 = i0;
              i0 = l4;
              i1 = l4;
              i1 = LoadU8(ctx, i1 + 5u);
              i2 = 251u;
              i1 &= i2;
              StoreU8(ctx, i0 + 5u, i1);
              i0 = l4;
              i1 = l2;
              i1 = LoadU32(ctx, i1 + 40u);
              l7 = i1;
              StoreU32(ctx, i0 + 24u, i1);
              i0 = l2;
              i1 = l4;
              StoreU32(ctx, i0 + 40u, i1);
              block_419:;
              i0 = l3;
              i1 = 1u;
              i0 -= i1;
              l3 = i0;
              i0 = l5;
              i1 = 1u;
              i0 -= i1;
              l2 = i0;
              i0 = l5;
              i1 = 1u;
              i0 = (uint32_t)((int32_t)i0 > (int32_t)i1);
              l5 = i0;
              if (i0) {goto loop_416;}
            }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {goto loop_6;}
          block_25:;
          i0 = l3;
          i1 = l6;
          i2 = l3;
          i3 = 2u;
          i2 >>= (i3 & 31);
          i3 = 4080u;
          i2 &= i3;
          i1 += i2;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          l3 = i0;
          i0 = l10;
          i1 = 68u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l3;
            ReleaseNodeChain(ctx, i0, i1);
            i0 = 68u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {goto loop_6;}
          block_24:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l14;
            i0 = LoadU32(ctx, i0 + 16u);
            i0 = LoadU32(ctx, i0 + 16u);
            i1 = l3;
            i2 = 12u;
            i1 >>= (i2 & 31);
            i2 = kF1701ConstFFFFC;
            i1 &= i2;
            i0 += i1;
            i0 = LoadU32(ctx, i0);
            l2 = i0;
            i0 = LoadU8(ctx, i0 + 72u);
            l5 = i0;
            i0 = l14;
            i0 = LoadU32(ctx, i0 + 12u);
            l4 = i0;
          }
          i0 = l10;
          i1 = 69u;
          i0 = i0 == i1;
          i1 = 1u;
          i2 = ResumePhase(ctx);
          i0 = i2 ? i0 : i1;
          if (i0) {
            i0 = state;
            i1 = l5;
            i2 = l4;
            i0 = ReserveIntVector(ctx, i0, i1, i2);
            l8 = i0;
            i0 = 69u;
            i1 = ResumePhase(ctx);
            i2 = 1u;
            i1 = i1 == i2;
            if (i1) {goto spill_frame;}
            i0 = l8;
            l4 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l4;
            i1 = l2;
            StoreU32(ctx, i0 + 16u, i1);
            i0 = 0u;
            l2 = i0;
          }
          i0 = l5;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            loop_425: 
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = l9;
                i0 = LoadU32(ctx, i0);
                l7 = i0;
                i1 = 23u;
                i0 >>= (i1 & 31);
                l12 = i0;
                i0 = l2;
                i1 = 2u;
                i0 <<= (i1 & 31);
                i1 = l4;
                i0 += i1;
                l15 = i0;
                i0 = l7;
                i1 = 63u;
                i0 &= i1;
                i1 = 4u;
                i0 = i0 == i1;
                l7 = i0;
              }
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = l7;
                if (i0) {
                  i0 = l14;
                  i1 = l12;
                  i2 = 2u;
                  i1 <<= (i2 & 31);
                  i0 += i1;
                  i0 = LoadU32(ctx, i0 + 20u);
                  goto block_427;
                }
                i0 = l12;
                i1 = 4u;
                i0 <<= (i1 & 31);
                i1 = l6;
                i0 += i1;
                l12 = i0;
              }
              i0 = l10;
              i1 = 70u;
              i0 = i0 == i1;
              i1 = 1u;
              i2 = ResumePhase(ctx);
              i0 = i2 ? i0 : i1;
              if (i0) {
                i0 = state;
                i1 = l12;
                i0 = AdjustNodeCapacity(ctx, i0, i1);
                l8 = i0;
                i0 = 70u;
                i1 = ResumePhase(ctx);
                i2 = 1u;
                i1 = i1 == i2;
                if (i1) {goto spill_frame;}
                i0 = l8;
              } else {
                i0 = l12;
              }
              block_427:;
              l12 = i0;
              i0 = ResumePhase(ctx);
              i0 = !(i0);
              if (i0) {
                i0 = l15;
                i1 = l12;
                StoreU32(ctx, i0 + 20u, i1);
                i0 = l9;
                i1 = kF1701ResumeWordBytes;
                i0 += i1;
                l9 = i0;
                i0 = l5;
                i1 = l2;
                i2 = 1u;
                i1 += i2;
                l2 = i1;
                i0 = i0 != i1;
                l12 = i0;
                if (i0) {goto loop_425;}
              }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l6;
            i1 = l3;
            i2 = 6u;
            i1 >>= (i2 & 31);
            i2 = 255u;
            i1 &= i2;
            i2 = 4u;
            i1 <<= (i2 & 31);
            i0 += i1;
            l2 = i0;
            i1 = l4;
            StoreU32(ctx, i0, i1);
            i0 = l2;
            i1 = 6u;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = state;
            i0 = LoadU32(ctx, i0 + 16u);
            l2 = i0;
            i0 = LoadU32(ctx, i0 + 68u);
            i1 = l2;
            i1 = LoadU32(ctx, i1 + 64u);
            l4 = i1;
            i0 = i0 >= i1;
            l3 = i0;
          }
          i0 = l3;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = l10;
            i1 = 71u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = state;
              GrowTableSlot(ctx, i0);
              i0 = 71u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = state;
            i0 = LoadU32(ctx, i0 + 12u);
            l6 = i0;
            goto loop_6;
          }
          block_23:;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i1 = 6u;
            i0 >>= (i1 & 31);
            l5 = i0;
            i1 = 255u;
            i0 &= i1;
            l2 = i0;
            i0 = l14;
            i0 = LoadU32(ctx, i0 + 16u);
            i0 = LoadU8(ctx, i0 + 73u);
            i1 = kF1701ConstFFFFFFFF;
            i0 ^= i1;
            i1 = state;
            i1 = LoadU32(ctx, i1 + 20u);
            l12 = i1;
            i1 = LoadU32(ctx, i1);
            i2 = l12;
            i2 = LoadU32(ctx, i2 + 4u);
            i1 -= i2;
            i2 = 4u;
            i1 = (uint32_t)((int32_t)i1 >> (i2 & 31));
            i0 += i1;
            l4 = i0;
            i0 = l3;
            i1 = 23u;
            i0 >>= (i1 & 31);
            l3 = i0;
          }
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            if (i0) {
              i0 = l6;
              i1 = l2;
              i2 = 4u;
              i1 <<= (i2 & 31);
              i0 += i1;
              l5 = i0;
              i0 = l3;
              i1 = 1u;
              i0 -= i1;
              goto block_438;
            }
            i0 = state;
            i1 = l9;
            StoreU32(ctx, i0 + 24u, i1);
            i0 = l4;
            i1 = 4u;
            i0 <<= (i1 & 31);
            l5 = i0;
            i1 = state;
            i1 = LoadU32(ctx, i1 + 28u);
            i2 = state;
            i2 = LoadU32(ctx, i2 + 8u);
            i1 -= i2;
            i0 = (uint32_t)((int32_t)i0 >= (int32_t)i1);
            l3 = i0;
          }
          i0 = l3;
          i1 = ResumePhase(ctx);
          i2 = 2u;
          i1 = i1 == i2;
          i0 |= i1;
          if (i0) {
            i0 = l10;
            i1 = 72u;
            i0 = i0 == i1;
            i1 = 1u;
            i2 = ResumePhase(ctx);
            i0 = i2 ? i0 : i1;
            if (i0) {
              i0 = state;
              i1 = l4;
              GrowValueStack(ctx, i0, i1);
              i0 = 72u;
              i1 = ResumePhase(ctx);
              i2 = 1u;
              i1 = i1 == i2;
              if (i1) {goto spill_frame;}
            }
          }
          i0 = ResumePhase(ctx);
          if (i0) {
            i0 = l3;
          } else {
            i0 = state;
            i1 = state;
            i1 = LoadU32(ctx, i1 + 12u);
            l6 = i1;
            i2 = l2;
            i3 = 4u;
            i2 <<= (i3 & 31);
            i1 += i2;
            l5 = i1;
            i2 = l4;
            i3 = 4u;
            i2 <<= (i3 & 31);
            i1 += i2;
            StoreU32(ctx, i0 + 8u, i1);
            i0 = l4;
          }
          block_438:;
          l3 = i0;
          i0 = ResumePhase(ctx);
          i0 = !(i0);
          if (i0) {
            i0 = l3;
            i1 = 1u;
            i0 = (uint32_t)((int32_t)i0 < (int32_t)i1);
            l2 = i0;
            if (i0) {goto loop_6;}
            i0 = 0u;
            l2 = i0;
            i0 = 0u;
            i1 = l4;
            i0 -= i1;
            l7 = i0;
            loop_446: 
              i0 = l2;
              i1 = 4u;
              i0 <<= (i1 & 31);
              i1 = l5;
              i0 += i1;
              i1 = l2;
              i2 = l4;
              i1 = (uint32_t)((int32_t)i1 < (int32_t)i2);
              if (i1) {
                i1 = l2;
                i2 = 4u;
                i1 <<= (i2 & 31);
                l15 = i1;
                i2 = l5;
                i1 += i2;
                i2 = l12;
                i2 = LoadU32(ctx, i2);
                i3 = l7;
                i4 = 4u;
                i3 <<= (i4 & 31);
                i2 += i3;
                i3 = l15;
                i2 += i3;
                l15 = i2;
                j2 = LoadU64(ctx, i2);
                StoreU64(ctx, i1, j2);
                i1 = l15;
                i1 = LoadU32(ctx, i1 + 8u);
              } else {
                i1 = 0u;
              }
              StoreU32(ctx, i0 + 8u, i1);
              i0 = l2;
              i1 = 1u;
              i0 += i1;
              l2 = i0;
              i1 = l3;
              i0 = i0 != i1;
              l15 = i0;
              if (i0) {goto loop_446;}
            goto loop_6;
          }
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = l14;
          i0 = LoadU32(ctx, i0 + 16u);
          i0 = LoadU32(ctx, i0 + 76u);
          i1 = l3;
          i2 = 10u;
          i1 >>= (i2 & 31);
          i2 = kF1701Const3FFFF0;
          i1 &= i2;
          i0 += i1;
          l2 = i0;
          i0 = LoadU32(ctx, i0);
          l3 = i0;
          i0 = state;
          i0 = LoadU32(ctx, i0 + 68u);
          l5 = i0;
          i0 = state;
          i1 = l2;
          i1 = LoadU32(ctx, i1 + 8u);
          l2 = i1;
          StoreU32(ctx, i0 + 68u, i1);
          i0 = state;
          i1 = state;
          i1 = LoadU8(ctx, i1 + 56u);
          i2 = 32u;
          i1 |= i2;
          l12 = i1;
          StoreU8(ctx, i0 + 56u, i1);
        }
        i0 = l10;
        i1 = 73u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = state;
          i1 = l9;
          DeclareUpvalue(ctx, i0, i1);
          i0 = 73u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = state;
          i0 = LoadU8(ctx, i0 + 56u);
          l12 = i0;
          i1 = 32u;
          i0 &= i1;
          l7 = i0;
          if (i0) {
            i0 = state;
            i1 = l12;
            i2 = 32u;
            i1 ^= i2;
            StoreU8(ctx, i0 + 56u, i1);
          }
          i0 = state;
          i0 = LoadU32(ctx, i0 + 68u);
          l12 = i0;
          i1 = l2;
          i0 = i0 == i1;
          l2 = i0;
          if (i0) {
            i0 = state;
            i1 = l5;
            StoreU32(ctx, i0 + 68u, i1);
          }
          i0 = state;
          i0 = LoadU8(ctx, i0 + 6u);
          i1 = 1u;
          i0 = i0 != i1;
          l5 = i0;
          if (i0) {goto block_17;}
          goto block_10;
        }
        block_18:;
        i0 = l10;
        i1 = 74u;
        i0 = i0 == i1;
        i1 = 1u;
        i2 = ResumePhase(ctx);
        i0 = i2 ? i0 : i1;
        if (i0) {
          i0 = state;
          i1 = kF1701MsgForLimitMustBe;
          i2 = 0u;
          RaiseScriptError(ctx, i0, i1, i2);
          i0 = 74u;
          i1 = ResumePhase(ctx);
          i2 = 1u;
          i1 = i1 == i2;
          if (i1) {goto spill_frame;}
        }
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {goto block_8;}
        block_17:;
        i0 = ResumePhase(ctx);
        i0 = !(i0);
        if (i0) {
          i0 = 0u;
          l2 = i0;
          goto loop_16;
        }
      block_10:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = state;
        i1 = l4;
        StoreU32(ctx, i0 + 24u, i1);
      }
      block_9:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l11;
        i1 = 32u;
        i0 += i1;
        GuestStackSp(ctx) = i0;
        goto func_exit;
      }
      block_8:;
      i0 = ResumePhase(ctx);
      i0 = !(i0);
      if (i0) {
        i0 = l2;
        i1 = l2;
        d1 = LoadF64(ctx, i1);
        i2 = l2;
        d2 = LoadF64(ctx, i2 + 32u);
        d1 -= d2;
        StoreF64(ctx, i0, d1);
        i0 = l5;
        i1 = 3u;
        StoreU32(ctx, i0 + 8u, i1);
        i0 = l9;
        i1 = l3;
        i2 = 12u;
        i1 >>= (i2 & 31);
        i2 = kF1701ConstFFFFC;
        i1 &= i2;
        i0 += i1;
        l3 = i0;
        i1 = kF1701Const7FFFC;
        i0 -= i1;
        l9 = i0;
        goto loop_6;
      }
  goto func_exit;
  spill_frame:;
  l8 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l8;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF1701ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l8 = i0;
  i1 = state;
  StoreU32(ctx, i0, i1);
  i0 = l8;
  i1 = expression;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l8;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = l8;
  i1 = l3;
  StoreU32(ctx, i0 + 12u, i1);
  i0 = l8;
  i1 = l4;
  StoreU32(ctx, i0 + 16u, i1);
  i0 = l8;
  i1 = l5;
  StoreU32(ctx, i0 + 20u, i1);
  i0 = l8;
  i1 = l6;
  StoreU32(ctx, i0 + 24u, i1);
  i0 = l8;
  i1 = l7;
  StoreU32(ctx, i0 + 28u, i1);
  i0 = l8;
  i1 = l9;
  StoreU32(ctx, i0 + 32u, i1);
  i0 = l8;
  i1 = l11;
  StoreU32(ctx, i0 + 36u, i1);
  i0 = l8;
  i1 = l12;
  StoreU32(ctx, i0 + 40u, i1);
  i0 = l8;
  i1 = l13;
  StoreU32(ctx, i0 + 44u, i1);
  i0 = l8;
  i1 = l14;
  StoreU32(ctx, i0 + 48u, i1);
  i0 = l8;
  i1 = l15;
  StoreU32(ctx, i0 + 52u, i1);
  i0 = l8;
  j1 = l18;
  StoreU64(ctx, i0 + 56u, j1);
  i0 = l8;
  i1 = l16;
  StoreU32(ctx, i0 + 64u, i1);
  i0 = l8;
  i1 = l17;
  StoreU32(ctx, i0 + 68u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 72u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
