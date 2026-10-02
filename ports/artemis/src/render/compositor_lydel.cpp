#include "util/engine_context.h"

namespace artc {

// Forward declarations for the callees this dispatcher drives.
uint32_t CompareBytes(EngineContext* ctx, uint32_t lhs, uint32_t rhs, uint32_t n);
uint32_t CopyBytes(EngineContext* ctx, uint32_t dst, uint32_t src, uint32_t n);
uint32_t StringLength(EngineContext* ctx, uint32_t text);
uint32_t SkipWhitespace(EngineContext* ctx, uint32_t text);
uint32_t FindOrderedChainEntry(EngineContext* ctx, uint32_t key, uint32_t node);
uint32_t CombineComparisonResults(EngineContext* ctx, uint32_t left, uint32_t right,
                                  uint32_t result, uint32_t box);
void UnlinkListNode(EngineContext* ctx, uint32_t node, uint32_t victim);
void ReleaseNodeTree(EngineContext* ctx, uint32_t state, uint32_t node);
void ReleaseLayerNodeTree(EngineContext* ctx, uint32_t owner, uint32_t node);
void Panic(EngineContext* ctx, uint32_t code);
// Reads the number a tween tag carries; no product file yet.
double UnorderedLookupNumber(EngineContext* ctx, uint32_t tag);

namespace {

// Sign-extends the low byte of a word loaded from guest memory.
int32_t SignExtend8(uint32_t value) {
  return static_cast<int32_t>(static_cast<int8_t>(value));
}

}  // namespace

namespace Compositor {

// Deletes the tween and transition entries a layer asked to drop and releases the
// nodes and slot pairs those entries held.
void LydelDispatch(EngineContext* ctx, uint32_t p0, uint32_t p1, uint32_t p2) {
  EnterFrame();
  // Shared operands of the shared_ptr null-dereference assertion every member read carries.
  constexpr uint32_t kAssertWord = 857u;
  constexpr uint32_t kAssertExpression = 745009u;
  constexpr uint32_t kAssertFile = 745017u;
  constexpr uint32_t kAssertLine = 734u;
  constexpr uint32_t kAssertFunction = 745073u;

  // The event return frame this body spills into: a four-byte word naming the call
  // a rewinding pass resumes at, followed by 100 bytes of saved locals.
  uint32_t const kSaveFrameBytes = 100u;
  uint32_t const kResumeWordBytes = 4u;
  uint32_t const kLayerFrameBytes = 512u;
  uint32_t const kAfterFailAssertion0 = 0u;
  uint32_t const kAfterFailAssertion1 = 1u;
  uint32_t const kAfterInvokeViiiiii2 = 2u;
  uint32_t const kAfterSyncSlot3 = 3u;
  uint32_t const kAfterCombineComparisonResults4 = 4u;
  uint32_t const kAfterReleaseSlotPair5 = 5u;
  uint32_t const kAfterReleaseSlotPair6 = 6u;
  uint32_t const kAfterInvokeViiiiii7 = 7u;
  uint32_t const kAfterInvokeIiiiii8 = 8u;
  uint32_t const kAfterFailAssertion9 = 9u;
  uint32_t const kAfterReleaseSlotPair10 = 10u;
  uint32_t const kAfterReleaseSlotPair11 = 11u;
  uint32_t const kAfterInvokeViiiiii12 = 12u;
  uint32_t const kAfterFailAssertion13 = 13u;
  uint32_t const kAfterInvokeViiiiii14 = 14u;
  uint32_t const kAfterInvokeIiiiii15 = 15u;
  uint32_t const kAfterFailAssertion16 = 16u;
  uint32_t const kAfterReleaseSlotPair17 = 17u;
  uint32_t const kAfterReleaseSlotPair18 = 18u;
  uint32_t const kAfterInitResources19 = 19u;
  uint32_t const kAfterInvokeViiiiii20 = 20u;
  uint32_t const kAfterInvokeIiiiii21 = 21u;
  uint32_t const kAfterInvokeViiiiii22 = 22u;
  uint32_t const kAfterInvokeIiiiii23 = 23u;
  uint32_t const kAfterInvokeViiiiii24 = 24u;
  uint32_t const kAfterInvokeIiiiii25 = 25u;
  uint32_t const kAfterInvokeViiiiii26 = 26u;
  uint32_t const kAfterInvokeIiiiii27 = 27u;
  uint32_t const kAfterInvokeViiiiii28 = 28u;
  uint32_t const kAfterInvokeIiiiii29 = 29u;
  uint32_t const kAfterInvokeViiiiii30 = 30u;
  uint32_t const kAfterInvokeIiiiii31 = 31u;
  uint32_t const kAfterInvokeViiiiii32 = 32u;
  uint32_t const kAfterInvokeIiiiii33 = 33u;
  uint32_t const kAfterInvokeViiiiii34 = 34u;
  uint32_t const kAfterInvokeIiiiii35 = 35u;
  uint32_t const kAfterInvokeViiiiii36 = 36u;
  uint32_t const kAfterInvokeIiiiii37 = 37u;
  uint32_t const kAfterInvokeViiiiii38 = 38u;
  uint32_t const kAfterInvokeIiiiii39 = 39u;
  uint32_t const kAfterInvokeViiiiii40 = 40u;
  uint32_t const kAfterInvokeIiiiii41 = 41u;
  uint32_t const kAfterInvokeViiiiii42 = 42u;
  uint32_t const kAfterInvokeIiiiii43 = 43u;
  uint32_t const kAfterInvokeViiiiii44 = 44u;
  uint32_t const kAfterInvokeIiiiii45 = 45u;
  uint32_t const kAfterInvokeViiiiii46 = 46u;
  uint32_t const kAfterInvokeIiiiii47 = 47u;
  uint32_t const kAfterInvokeViiiiii48 = 48u;
  uint32_t const kAfterInvokeIiiiii49 = 49u;
  uint32_t const kAfterInvokeViiiiii50 = 50u;
  uint32_t const kAfterInvokeIiiiii51 = 51u;
  uint32_t const kAfterInvokeViiiiii52 = 52u;
  uint32_t const kAfterInvokeIiiiii53 = 53u;
  uint32_t const kAfterInvokeViiiiii54 = 54u;
  uint32_t const kAfterInvokeIiiiii55 = 55u;
  uint32_t const kAfterInvokeViiiiii56 = 56u;
  uint32_t const kAfterInvokeIiiiii57 = 57u;
  uint32_t const kAfterInvokeViiiiii58 = 58u;
  uint32_t const kAfterInvokeIiiiii59 = 59u;
  uint32_t const kAfterInvokeViiiiii60 = 60u;
  uint32_t const kAfterInvokeIiiiii61 = 61u;
  uint32_t const kAfterInvokeViiiiii62 = 62u;
  uint32_t const kAfterInvokeIiiiii63 = 63u;
  uint32_t const kAfterInvokeViiiiii64 = 64u;
  uint32_t const kAfterInvokeIiiiii65 = 65u;
  uint32_t const kAfterInvokeViiiiii66 = 66u;
  uint32_t const kAfterInvokeIiiiii67 = 67u;
  uint32_t const kAfterInvokeViiiiii68 = 68u;
  uint32_t const kAfterInvokeIiiiii69 = 69u;
  uint32_t const kAfterInvokeViiiiii70 = 70u;
  uint32_t const kAfterInvokeIiiiii71 = 71u;
  uint32_t const kAfterInvokeViiiiii72 = 72u;
  uint32_t const kAfterInvokeIiiiii73 = 73u;
  uint32_t const kAfterInvokeViiiiii74 = 74u;
  uint32_t const kAfterInvokeIiiiii75 = 75u;
  uint32_t const kAfterInvokeViiiiii76 = 76u;
  uint32_t const kAfterInvokeIiiiii77 = 77u;
  uint32_t const kAfterInvokeViiiiii78 = 78u;
  uint32_t const kAfterInvokeIiiiii79 = 79u;
  uint32_t const kAfterInvokeViiiiii80 = 80u;
  uint32_t const kAfterCall81 = 81u;
  uint32_t const kAfterInvokeViiiiii82 = 82u;
  uint32_t const kAfterCall83 = 83u;
  uint32_t const kAfterInvokeViiiiii84 = 84u;
  uint32_t const kAfterInvokeViiiiii85 = 85u;
  uint32_t const kAfterInvokeViiiiii86 = 86u;
  uint32_t const kAfterInvokeViiiiii87 = 87u;
  uint32_t const kAfterInvokeIiiiii88 = 88u;
  uint32_t const kAfterInvokeViiiiii89 = 89u;
  uint32_t const kAfterInvokeIiiiii90 = 90u;
  uint32_t const kAfterInvokeViiiiii91 = 91u;
  uint32_t const kAfterInvokeIiiiii92 = 92u;
  uint32_t const kAfterInvokeViiiiii93 = 93u;
  uint32_t const kAfterInvokeIiiiii94 = 94u;
  uint32_t const kAfterFoldLayoutOffset95 = 95u;
  uint32_t const kAfterInvokeVif96 = 96u;
  uint32_t const kAfterInvokeViiiiii97 = 97u;
  uint32_t const kAfterInvokeIiiiii98 = 98u;
  uint32_t const kAfterFoldLayoutOffset99 = 99u;
  uint32_t const kAfterInvokeVif100 = 100u;
  uint32_t const kAfterInvokeViiiiii101 = 101u;
  uint32_t const kAfterInvokeIiiiii102 = 102u;
  uint32_t const kAfterFoldLayoutOffset103 = 103u;
  uint32_t const kAfterInvokeVif104 = 104u;
  uint32_t const kAfterInvokeViiiiii105 = 105u;
  uint32_t const kAfterInvokeIiiiii106 = 106u;
  uint32_t const kAfterFoldLayoutOffset107 = 107u;
  uint32_t const kAfterInvokeVif108 = 108u;
  uint32_t const kAfterInvokeViiiiii109 = 109u;
  uint32_t const kAfterInvokeIiiiii110 = 110u;
  uint32_t const kAfterFoldLayoutOffset111 = 111u;
  uint32_t const kAfterInvokeVif112 = 112u;
  uint32_t const kAfterInvokeViiiiii113 = 113u;
  uint32_t const kAfterInvokeIiiiii114 = 114u;
  uint32_t const kAfterFoldLayoutOffset115 = 115u;
  uint32_t const kAfterInvokeVif116 = 116u;
  uint32_t const kAfterInvokeViiiiii117 = 117u;
  uint32_t const kAfterInvokeIiiiii118 = 118u;
  uint32_t const kAfterInvokeViiiiii119 = 119u;
  uint32_t const kAfterInvokeViiiiii120 = 120u;
  uint32_t const kAfterInvokeViiiiii121 = 121u;
  uint32_t const kAfterInvokeViiiiii122 = 122u;
  uint32_t const kAfterSyncSlot123 = 123u;
  uint32_t const kAfterSyncSlot124 = 124u;
  uint32_t const kAfterOsSchedule125 = 125u;
  uint32_t const kAfterFailAssertion126 = 126u;
  uint32_t const kAfterInvokeViiiiii127 = 127u;
  uint32_t const kAfterInvokeIiiiii128 = 128u;
  uint32_t const kAfterInvokeViiiiii129 = 129u;
  uint32_t const kAfterFoldLayoutOffset130 = 130u;
  uint32_t const kAfterReleaseSlotPair131 = 131u;
  uint32_t const kAfterInvokeViiiiii132 = 132u;
  uint32_t const kAfterFailAssertion133 = 133u;
  uint32_t const kAfterInvokeViiiiii134 = 134u;
  uint32_t const kAfterInvokeIiiiii135 = 135u;
  uint32_t const kAfterFoldLayoutOffset136 = 136u;
  uint32_t const kAfterOsQuery137 = 137u;
  uint32_t const kAfterSyncSlot138 = 138u;
  uint32_t const kAfterFailAssertion139 = 139u;
  uint32_t const kAfterOsQuery140 = 140u;
  uint32_t const kAfterInitResources141 = 141u;
  uint32_t const kAfterDispatchInvoke142 = 142u;
  uint32_t const kAfterOsQuery143 = 143u;
  uint32_t const kAfterSyncSlot144 = 144u;
  uint32_t const kAfterOsSchedule145 = 145u;
  uint32_t const kAfterSyncSlot146 = 146u;
  uint32_t const kAfterSyncSlot147 = 147u;
  uint32_t const kAfterOsQuery148 = 148u;
  uint32_t const kAfterInvokeViiiiii149 = 149u;
  uint32_t const kAfterDispatchInvoke150 = 150u;
  uint32_t const kAfterOsQuery151 = 151u;
  uint32_t const kAfterSyncSlot152 = 152u;
  uint32_t const kAfterSyncSlot153 = 153u;
  uint32_t const kAfterFailAssertion154 = 154u;
  uint32_t const kAfterInvokeViiiiii155 = 155u;
  uint32_t const kAfterInvokeViiiiii156 = 156u;
  uint32_t const kAfterOsQuery157 = 157u;
  uint32_t const kAfterOsSchedule158 = 158u;
  uint32_t const kAfterOsQuery159 = 159u;
  uint32_t const kAfterInvokeViiiiii160 = 160u;
  uint32_t const kAfterInvokeViiiiii161 = 161u;
  uint32_t const kAfterOsQuery162 = 162u;
  uint32_t const kAfterReleaseSlotPair163 = 163u;
  uint32_t const kAfterFoldLayoutOffset164 = 164u;
  uint32_t const kAfterInvokeViiiiii165 = 165u;
  uint32_t const kAfterInvokeViiiiii166 = 166u;
  uint32_t const kAfterInvokeViiiiii167 = 167u;
  uint32_t const kAfterOsQuery168 = 168u;
  uint32_t const kAfterFailAssertion169 = 169u;
  uint32_t const kAfterFailAssertion170 = 170u;
  uint32_t const kAfterReleaseSlotPair171 = 171u;
  uint32_t const kAfterReleaseSlotPair172 = 172u;
  uint32_t const kAfterReleaseSlotPair173 = 173u;
  uint32_t const kAfterReleaseSlotPair174 = 174u;
  uint32_t const kAfterReleaseSlotPair175 = 175u;
  uint32_t const kAfterReleaseSlotPair176 = 176u;
  uint32_t const kAfterReleaseSlotPair177 = 177u;
  uint32_t const kAfterReleaseSlotPair178 = 178u;
  uint32_t const kAfterPanic179 = 179u;
  // Locals, in the order the save frame stores them.
  uint32_t frame = 0u;        // guest stack pointer of this call's 512-byte frame
  uint32_t node = 0u;         // tween or transition record under inspection
  uint32_t slot = 0u;         // table slot of that record, or a membership flag
  uint32_t word = 0u;         // count or tag word read out of the record
  uint32_t ret = 0u;          // result of the callee that just ran
  uint32_t flag = 0u;         // truth value a branch leaves behind
  uint32_t aux = 0u;          // second scratch the branch logic reuses
  uint32_t spill = 0u;        // holds a callee result while the next callee runs
  uint32_t resume_code = 0u;  // the awaiting call a rewinding pass resumes at
  uint32_t handle = 0u;       // reference-counted object being released
  uint32_t result = 0u;       // value the release helper leaves behind
  uint32_t scratch_a = 0u;
  uint32_t scratch_b = 0u;
  uint32_t scratch_c = 0u;
  uint32_t scratch_d = 0u;
  uint32_t scratch_e = 0u;
  uint32_t scratch_f = 0u;
  uint32_t scratch_g = 0u;
  uint32_t scratch_h = 0u;
  float float_a = 0.0f;
  float float_b = 0.0f;
  double double_a = 0.0;
  double double_b = 0.0;
  double double_c = 0.0;

  if (ResumePhase(ctx) == kResumeRewinding) {
    // A suspended pass left its frame behind: pop it and put the locals back.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    p0 = LoadU32(ctx, saved_frame);
    p2 = LoadU32(ctx, saved_frame + 8u);
    frame = LoadU32(ctx, saved_frame + 12u);
    node = LoadU32(ctx, saved_frame + 16u);
    slot = LoadU32(ctx, saved_frame + 20u);
    word = LoadU32(ctx, saved_frame + 24u);
    ret = LoadU32(ctx, saved_frame + 28u);
    flag = LoadU32(ctx, saved_frame + 32u);
    aux = LoadU32(ctx, saved_frame + 36u);
    scratch_a = LoadU32(ctx, saved_frame + 40u);
    scratch_b = LoadU32(ctx, saved_frame + 44u);
    scratch_c = LoadU32(ctx, saved_frame + 48u);
    handle = LoadU32(ctx, saved_frame + 52u);
    result = LoadU32(ctx, saved_frame + 56u);
    scratch_d = LoadU32(ctx, saved_frame + 60u);
    scratch_e = LoadU32(ctx, saved_frame + 64u);
    scratch_f = LoadU32(ctx, saved_frame + 68u);
    scratch_g = LoadU32(ctx, saved_frame + 72u);
    float_a = LoadF32(ctx, saved_frame + 76u);
    float_b = LoadF32(ctx, saved_frame + 80u);
    double_a = LoadF64(ctx, saved_frame + 84u);
    double_b = LoadF64(ctx, saved_frame + 92u);
    p1 = LoadU32(ctx, saved_frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The resume word sits above the frame; it names the call that split off.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }

  if (ResumePhase(ctx) == kResumeRunning) {
    frame = GuestStackSp(ctx) - kLayerFrameBytes;
    GuestStackSp(ctx) = frame;
    node = LoadU32(ctx, p0 + 16u);
    slot = static_cast<uint32_t>(node == 0u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == 0u) {
      HostFailAssertion(ctx, kAssertWord, kAssertExpression, kAssertFile, kAssertLine,
                        kAssertFunction);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterFailAssertion0;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0) {
        goto trap_2;
      }
      goto rethrow_1;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, node + 24u);
    slot = static_cast<uint32_t>(node == 0u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterFailAssertion1) {
      HostFailAssertion(ctx, kAssertWord, kAssertExpression, kAssertFile, kAssertLine,
                        kAssertFunction);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterFailAssertion1;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0) {
        goto trap_2;
      }
      goto rethrow_1;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 26u, 0u);
    StoreU16(ctx, frame + 24u, 25705u);
    StoreU8(ctx, frame + 35u, 2u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 472u, frame + 24u);
    scratch_c = frame + 496u;
    scratch_b = p1 + 16u;
    ret = frame + 24u;
    handle = frame + 472u;
    slot = frame + 464u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii2) {
    HostInvokeViiiiii(ctx, 1959u, scratch_c, scratch_b, ret, 745008u, handle, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii2;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot == 1u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (slot) {
      goto release_2;
    }
    slot = LoadU32(ctx, frame + 496u);
    handle = LoadU32(ctx, LoadU32(ctx, node) + 304u);
    StoreU32(ctx, kDataD015C, 0u);
    ret = frame + 480u;
    slot = slot + 28u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterSyncSlot3) {
    HostSyncSlot(ctx, handle, ret, node, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterSyncSlot3;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node == 1u);
    if (node) {
      goto release_2;
    }
    node = LoadU32(ctx, frame + 480u);
    slot = static_cast<uint32_t>(node == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (slot) {
      handle = 0u;
      goto merge_1;
    }
    handle = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterCombineComparisonResults4) {
    spill = CombineComparisonResults(ctx, node, 574820u, 757644u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterCombineComparisonResults4;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    scratch_g = spill;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = static_cast<uint32_t>(scratch_g == 0u);
    if (node) {
      scratch_g = 0u;
      goto merge_1;
    }
    node = LoadU32(ctx, frame + 484u);
    slot = static_cast<uint32_t>(node == 0u);
    if (slot) {
      goto merge_1;
    }
    slot = LoadU32(ctx, node + 4u) + 1u;
    StoreU32(ctx, node + 4u, slot);
    handle = node;
  }
  // Merge point merge_1.
  merge_1:;
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, frame + 484u);
    slot = static_cast<uint32_t>(node == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (slot) {
      goto release_1;
    }
    slot = LoadU32(ctx, node + 4u);
    ret = slot - 1u;
    StoreU32(ctx, node + 4u, ret);
    slot = static_cast<uint32_t>(slot != 1u);
    if (slot) {
      goto release_1;
    }
    slot = LoadU32(ctx, LoadU32(ctx, node) + 8u);
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterReleaseSlotPair5) {
    HostReleaseSlotPair(ctx, slot, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterReleaseSlotPair5;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (slot == 1u) {
      goto rethrow_1;
    }
    slot = LoadU32(ctx, node + 8u);
    ret = slot - 1u;
    StoreU32(ctx, node + 8u, ret);
    slot = static_cast<uint32_t>(slot != 1u);
    if (slot) {
      goto release_1;
    }
    slot = LoadU32(ctx, LoadU32(ctx, node) + 12u);
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterReleaseSlotPair6) {
    HostReleaseSlotPair(ctx, slot, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterReleaseSlotPair6;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node == 1u);
    if (node) {
      goto rethrow_1;
    }
  }
  // Merge point release_1.
  release_1:;
  if (ResumePhase(ctx) == kResumeRunning) {
    node = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
    if (node) {
      node = LoadU32(ctx, frame + 24u);
      DeallocateRawBlock(ctx, node);
    }
    if (scratch_g) {
      goto step_1;
    }
    slot = 0u;
    StoreU8(ctx, frame + 26u, 0u);
    StoreU16(ctx, frame + 24u, 25705u);
    StoreU8(ctx, frame + 35u, 2u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    flag = frame + 496u;
    scratch_c = frame + 24u;
    ret = frame + 480u;
    node = frame + 472u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii7) {
    HostInvokeViiiiii(ctx, 1959u, flag, scratch_b, scratch_c, 745008u, ret, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii7;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node == 1u);
    if (node) {
      goto release_3;
    }
    node = LoadU32(ctx, frame + 496u);
    flag = LoadU32(ctx, node + 32u);
    scratch_c = LoadU8(ctx, node + 39u);
    ret = static_cast<uint32_t>(SignExtend8(scratch_c) < 0);
    ret = ret ? flag : scratch_c == 1u;
  }
  if (ret || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      node = node + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii8) {
      spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741989u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeIiiiii8;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      node = spill;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (slot == 1u) {
        goto rethrow_1;
      }
      slot = static_cast<uint32_t>(node == 0u);
    }
  else {
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 24u));
    }
    scratch_g = 0u;
    node = 1u;
    slot = static_cast<uint32_t>(slot == 0u);
    if (slot) {
      goto merge_3;
    }
    slot = LoadU32(ctx, p0 + 16u);
    ret = static_cast<uint32_t>(slot == 0u);
  }
  if (ret || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterFailAssertion9) {
      HostFailAssertion(ctx, kAssertWord, kAssertExpression, kAssertFile, kAssertLine,
                        kAssertFunction);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterFailAssertion9;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0) {
        goto trap_2;
      }
      goto rethrow_1;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch_g = LoadU32(ctx, slot + 24u);
    slot = LoadU32(ctx, slot + 28u);
    if (slot) {
      StoreU32(ctx, slot + 4u, (LoadU32(ctx, slot + 4u) + 1u));
    }
    ret = static_cast<uint32_t>(handle == 0u);
    if (ret) {
      goto merge_2;
    }
    scratch_c = LoadU32(ctx, handle + 4u);
    StoreU32(ctx, handle + 4u, scratch_c - 1u);
    ret = static_cast<uint32_t>(scratch_c != 1u);
    if (ret) {
      goto merge_2;
    }
    ret = LoadU32(ctx, handle);
    scratch_c = LoadU32(ctx, ret + 8u);
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterReleaseSlotPair10) {
    HostReleaseSlotPair(ctx, scratch_c, handle);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterReleaseSlotPair10;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch_c = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    ret = static_cast<uint32_t>(scratch_c != 1u);
  }
  if (ret || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      scratch_c = LoadU32(ctx, handle + 8u);
      StoreU32(ctx, handle + 8u, scratch_c - 1u);
      ret = static_cast<uint32_t>(scratch_c != 1u);
      if (ret) {
        goto merge_2;
      }
      ret = LoadU32(ctx, handle);
      scratch_c = LoadU32(ctx, ret + 12u);
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterReleaseSlotPair11) {
      HostReleaseSlotPair(ctx, scratch_c, handle);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterReleaseSlotPair11;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      handle = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      handle = static_cast<uint32_t>(handle != 1u);
      if (handle) {
        goto merge_2;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto rethrow_1;
  }
  // Merge point release_2.
  release_2:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto os_release_1;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto os_release_1;
  }
  release_3:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_46;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_46;
  }
  // Merge point merge_2.
  merge_2:;
  if (ResumePhase(ctx) == kResumeRunning) {
    handle = static_cast<uint32_t>(scratch_g == 0u);
    if (handle) {
      goto step_2;
    }
    handle = slot;
  }
  step_1:;
  if (ResumePhase(ctx) == kResumeRunning) {
    node = 0u;
    slot = LoadU32(ctx, (LoadU32(ctx, scratch_g) - 152u));
    goto merge_3;
  }
  step_2:;
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch_g = 0u;
    handle = slot;
    ret = 0u;
  }
  else {
  }
  // Merge point merge_3.
  merge_3:;
  if (ResumePhase(ctx) == kResumeRunning) {
    if (handle) {
      slot = LoadU32(ctx, handle + 4u) + 1u;
      StoreU32(ctx, handle + 4u, slot);
    }
    node = static_cast<uint32_t>(node == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (node) {
      goto init_resources_1;
    }
    StoreU8(ctx, frame + 26u, 0u);
    StoreU16(ctx, frame + 24u, 25705u);
    StoreU8(ctx, frame + 35u, 2u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    flag = frame + 496u;
    scratch_c = frame + 24u;
    slot = frame + 480u;
    node = frame + 472u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii12) {
    HostInvokeViiiiii(ctx, 1959u, flag, scratch_b, scratch_c, 745008u, slot, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii12;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node != 1u);
  }
  if (node || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      scratch_c = slot + 28u;
      if (static_cast<int32_t>(LoadS8(ctx, slot + 39u)) < 0) {
        scratch_c = LoadU32(ctx, slot + 28u);
      }
      node = LoadU8(ctx, scratch_c);
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node != 126u);
      if (node) {
        goto init_resources_1;
      }
      slot = LoadU32(ctx, p0 + 16u);
      node = static_cast<uint32_t>(slot == 0u);
    }
    if (node || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterFailAssertion13) {
        HostFailAssertion(ctx, kAssertWord, kAssertExpression, kAssertFile, kAssertLine,
                          kAssertFunction);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterFailAssertion13;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0) {
          goto trap_2;
        }
        goto rethrow_1;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU8(ctx, frame + 498u, 0u);
      StoreU16(ctx, frame + 496u, 25705u);
      StoreU8(ctx, frame + 507u, 2u);
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, frame + 472u, frame + 496u);
      word = frame + 480u;
      flag = frame + 496u;
      scratch_c = frame + 472u;
      node = frame + 464u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii14) {
      HostInvokeViiiiii(ctx, 1959u, word, scratch_b, flag, 745008u, scratch_c, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeViiiiii14;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      node = static_cast<uint32_t>(node == 1u);
      if (node) {
        goto merge_4;
      }
      StoreU32(ctx, kDataD015C, 0u);
      scratch_c = frame + 24u;
      node = LoadU32(ctx, frame + 480u) + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii15) {
      spill = HostInvokeIiiiii(ctx, 854u, scratch_c, node, 1u, 4294967295u, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeIiiiii15;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      node = spill;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      scratch_c = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      scratch_c = static_cast<uint32_t>(scratch_c == 1u);
      if (scratch_c) {
        goto merge_4;
      }
      StoreU32(ctx, kDataD015C, 0u);
      slot = FindOrderedChainEntry(ctx, slot + 76u, node);
      scratch_c = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      scratch_c = static_cast<uint32_t>(scratch_c == 1u);
      if (scratch_c) {
        goto release_4;
      }
      if (static_cast<int32_t>(LoadS8(ctx, node + 11u)) < 0) {
        DeallocateRawBlock(ctx, LoadU32(ctx, node));
      }
      if (static_cast<int32_t>(LoadS8(ctx, frame + 507u)) < 0) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 496u));
      }
      node = LoadU32(ctx, p0 + 16u);
      scratch_c = static_cast<uint32_t>(node == 0u);
    }
    if (scratch_c || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterFailAssertion16) {
        HostFailAssertion(ctx, kAssertWord, kAssertExpression, kAssertFile, kAssertLine,
                          kAssertFunction);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterFailAssertion16;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0) {
          goto trap_2;
        }
        goto rethrow_1;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      scratch_c = handle;
      node = static_cast<uint32_t>((node + 80u) == slot);
      if (node) {
        goto init_resources_2;
      }
      ret = 0u;
      node = LoadU32(ctx, slot + 28u);
      if (node) {
        ret = LoadU32(ctx, (LoadU32(ctx, node) - 152u)) + node;
      }
      scratch_c = LoadU32(ctx, slot + 32u);
      if (scratch_c) {
        StoreU32(ctx, scratch_c + 4u, (LoadU32(ctx, scratch_c + 4u) + 1u));
      }
      node = static_cast<uint32_t>(handle == 0u);
      if (node) {
        goto init_resources_2;
      }
      node = LoadU32(ctx, handle + 4u);
      slot = node - 1u;
      StoreU32(ctx, handle + 4u, slot);
      node = static_cast<uint32_t>(node != 1u);
      if (node) {
        goto init_resources_2;
      }
      node = LoadU32(ctx, LoadU32(ctx, handle) + 8u);
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterReleaseSlotPair17) {
      HostReleaseSlotPair(ctx, node, handle);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterReleaseSlotPair17;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      node = static_cast<uint32_t>(node != 1u);
    }
    if (node || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        node = LoadU32(ctx, handle + 8u);
        slot = node - 1u;
        StoreU32(ctx, handle + 8u, slot);
        node = static_cast<uint32_t>(node != 1u);
        if (node) {
          goto init_resources_2;
        }
        node = LoadU32(ctx, LoadU32(ctx, handle) + 12u);
        StoreU32(ctx, kDataD015C, 0u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterReleaseSlotPair18) {
        HostReleaseSlotPair(ctx, node, handle);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterReleaseSlotPair18;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        node = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        node = static_cast<uint32_t>(node != 1u);
        if (node) {
          goto init_resources_2;
        }
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      goto rethrow_1;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
    if (p0) {
      p0 = LoadU32(ctx, frame + 24u);
      DeallocateRawBlock(ctx, p0);
    }
    scratch_c = handle;
    goto merge_45;
  }
  // Merge point merge_4.
  merge_4:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
    goto release_5;
  }
  release_4:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, node + 11u)) >= 0);
    if (p0) {
      goto release_5;
    }
    p0 = LoadU32(ctx, node);
    DeallocateRawBlock(ctx, p0);
  }
  // Merge point release_5.
  release_5:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 507u)) < 0);
    if (p0) {
      p0 = LoadU32(ctx, frame + 496u);
      DeallocateRawBlock(ctx, p0);
    }
    scratch_c = handle;
    goto merge_45;
  }
  // Merge point init_resources_1.
  init_resources_1:;
  scratch_c = (ResumePhase(ctx) == kResumeRunning) ? handle : scratch_c;
  // Merge point init_resources_2.
  init_resources_2:;
  node = (ResumePhase(ctx) == kResumeRunning) ? (ret == 0u) : node;
  if (node || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInitResources19) {
      HostInitResources(ctx, 2590u, p0, 1u, p1, 741952u, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInitResources19;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0) {
        goto merge_40;
      }
      ret = HostRet0(ctx);
      p0 = HostTempRet0(ctx);
      goto merge_45;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    node = LoadU32(ctx, p1 + 20u);
  }
  if (node || ResumePhase(ctx) == kResumeRewinding) {
  step_3:;
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU8(ctx, node + 27u);
      flag = static_cast<uint32_t>(SignExtend8(slot));
      scratch_f = static_cast<int32_t>(flag) < static_cast<int32_t>(0u);
      slot = scratch_f ? LoadU32(ctx, node + 20u) : slot;
      scratch_a = static_cast<uint32_t>(slot < 4u);
      word = scratch_a ? slot : 4u;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (word) {
        scratch_d = frame + 24u;
        flag = node + 16u;
        scratch_f = scratch_f ? LoadU32(ctx, flag) : flag;
        flag = CompareBytes(ctx, scratch_d, scratch_f, word);
        aux = static_cast<uint32_t>(flag == 0u);
        if (aux) {
          slot = static_cast<uint32_t>(slot <= 4u);
          if (slot) {
            goto compare_1;
          }
          goto merge_6;
        }
        slot = (static_cast<int32_t>(flag) < 0);
        if (slot) {
          goto merge_6;
        }
  compare_1:;
        slot = CompareBytes(ctx, scratch_f, frame + 24u, word);
        flag = static_cast<uint32_t>(slot == 0u);
        if (flag) {
          goto step_4;
        }
        slot = (static_cast<int32_t>(slot) >= 0);
        if (slot) {
          goto step_5;
        }
        goto merge_5;
      }
      slot = static_cast<uint32_t>(slot > 4u);
      if (slot) {
        goto merge_6;
      }
  step_4:;
      if (scratch_a) {
        goto merge_5;
      }
  step_5:;
      StoreU8(ctx, frame + 28u, 0u);
      StoreU32(ctx, frame + 24u, 1702060389u);
      StoreU8(ctx, frame + 35u, 4u);
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, frame + 480u, frame + 24u);
      aux = frame + 496u;
      word = frame + 24u;
      flag = frame + 480u;
      slot = frame + 472u;
      node = 0u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii20) {
      HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeViiiiii20;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      slot = static_cast<uint32_t>(slot == 1u);
      if (slot) {
        goto release_6;
      }
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 11u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii21) {
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 740914u, 11u);  // "easein_quad"
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii21;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_6;
      }
      result = 1u;
      goto merge_7;
    }
  // Merge point merge_5.
  merge_5:;
    node = (ResumePhase(ctx) == kResumeRunning) ? (node + 4u) : node;
  // Merge point merge_6.
  merge_6:;
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, node);
      if (node) {
        goto step_3;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto merge_7;
  }
  release_6:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_6:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii22) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii22;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 12u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii23) {
        // "easeout_quad"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 740926u, 12u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii23;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_7;
      }
      result = 2u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_7:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii24) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii24;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 14u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii25) {
  // "easeinout_quad"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 740939u, 14u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii25;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_8;
      }
      result = 3u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_8:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii26) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii26;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 12u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii27) {
        // "easein_cubic"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 740954u, 12u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii27;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_9;
      }
      result = 4u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_9:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii28) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii28;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 13u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii29) {
        // "easeout_cubic"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 740967u, 13u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii29;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_10;
      }
      result = 5u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_10:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii30) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii30;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 15u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii31) {
  // "easeinout_cubic"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 740981u, 15u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii31;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_11;
      }
      result = 6u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_11:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii32) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii32;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 12u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii33) {
        // "easein_quart"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 740997u, 12u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii33;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_12;
      }
      result = 7u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_12:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii34) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii34;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 13u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii35) {
        // "easeout_quart"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741010u, 13u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii35;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_13;
      }
      result = 8u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_13:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii36) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii36;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 15u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii37) {
  // "easeinout_quart"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741024u, 15u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii37;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_14;
      }
      result = 9u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_14:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii38) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii38;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 12u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii39) {
        // "easein_quint"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741040u, 12u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii39;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_15;
      }
      result = 10u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_15:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii40) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii40;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 13u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii41) {
        // "easeout_quint"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741053u, 13u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii41;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_16;
      }
      result = 11u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_16:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii42) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii42;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 15u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii43) {
  // "easeinout_quint"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741067u, 15u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii43;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_17;
      }
      result = 12u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_17:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii44) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii44;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 11u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii45) {
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741083u, 11u);  // "easein_expo"
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii45;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_18;
      }
      result = 13u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_18:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii46) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii46;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 12u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii47) {
        // "easeout_expo"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741095u, 12u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii47;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_19;
      }
      result = 14u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_19:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii48) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii48;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 14u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii49) {
  // "easeinout_expo"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741108u, 14u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii49;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_20;
      }
      result = 15u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_20:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii50) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii50;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 11u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii51) {
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741123u, 11u);  // "easein_circ"
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii51;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_21;
      }
      result = 16u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_21:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii52) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii52;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 12u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii53) {
        // "easeout_circ"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741135u, 12u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii53;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_22;
      }
      result = 17u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_22:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii54) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii54;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 14u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii55) {
  // "easeinout_circ"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741148u, 14u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii55;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_23;
      }
      result = 18u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_23:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii56) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii56;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 11u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii57) {
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741163u, 11u);  // "easein_sine"
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii57;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_24;
      }
      result = 19u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_24:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii58) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii58;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 12u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii59) {
        // "easeout_sine"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741175u, 12u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii59;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_25;
      }
      result = 20u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_25:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii60) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii60;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 14u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii61) {
  // "easeinout_sine"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741188u, 14u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii61;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_26;
      }
      result = 21u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_26:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii62) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii62;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 11u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii63) {
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741203u, 11u);  // "easein_back"
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii63;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_27;
      }
      result = 22u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_27:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii64) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii64;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 12u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii65) {
        // "easeout_back"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741215u, 12u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii65;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_28;
      }
      result = 23u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_28:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii66) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii66;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 14u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii67) {
  // "easeinout_back"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741228u, 14u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii67;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_29;
      }
      result = 24u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_29:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii68) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii68;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 14u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii69) {
  // "easein_elastic"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741243u, 14u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii69;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_30;
      }
      result = 25u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_30:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii70) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii70;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 15u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii71) {
  // "easeout_elastic"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741258u, 15u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii71;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_31;
      }
      result = 26u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_31:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii72) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii72;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 17u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii73) {
  // "easeinout_elastic"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741274u, 17u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii73;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_32;
      }
      result = 27u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_32:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii74) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii74;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 13u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii75) {
        // "easein_bounce"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741292u, 13u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii75;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_33;
      }
      result = 28u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_33:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii76) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii76;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      aux = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      flag = static_cast<uint32_t>(SignExtend8(word) < 0);
      flag = flag ? aux : word == 14u;
    }
    if (flag || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii77) {
  // "easeout_bounce"
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741306u, 14u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii77;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_34;
      }
      result = 29u;
      goto merge_7;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_34:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1702060389u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    flag = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii78) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, flag, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii78;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot == 1u);
    if (slot) {
      goto release_7;
    }
    slot = LoadU32(ctx, frame + 496u);
    aux = LoadU32(ctx, slot + 32u);
    word = LoadU8(ctx, slot + 39u);
    flag = static_cast<uint32_t>(SignExtend8(word) < 0);
    flag = flag ? aux : word == 16u;
  }
  if (flag || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      node = slot + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii79) {
  // "easeinout_bounce"
      spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 741321u, 16u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeIiiiii79;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      node = spill;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      slot = static_cast<uint32_t>(slot == 1u);
      if (slot) {
        goto rethrow_1;
      }
      node = static_cast<uint32_t>(node == 0u);
    }
  else {
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
    if (slot) {
      slot = LoadU32(ctx, frame + 24u);
      DeallocateRawBlock(ctx, slot);
    }
    result = node ? 30u : 0u;
    flag = node ^ 1u;
  }
  else {
  }
  // Merge point merge_7.
  merge_7:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1836020326u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    slot = frame + 480u;
    node = frame + 472u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii80) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, slot, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii80;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node == 1u);
    if (node) {
      goto release_11;
    }
    slot = LoadU32(ctx, frame + 496u);
    node = slot + 28u;
    slot = (static_cast<int32_t>(LoadS8(ctx, slot + 39u)) < 0);
    if (slot) {
      node = LoadU32(ctx, node);
    } else {
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterCall81) {
    double_c = UnorderedLookupNumber(ctx, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterCall81;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    double_a = double_c;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 24u));
    }
    StoreU8(ctx, frame + 26u, 0u);
    StoreU16(ctx, frame + 24u, 28532u);
    StoreU8(ctx, frame + 35u, 2u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    slot = frame + 480u;
    node = frame + 472u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii82) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, slot, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii82;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node == 1u);
    if (node) {
      goto release_10;
    }
    slot = LoadU32(ctx, frame + 496u);
    node = slot + 28u;
    slot = (static_cast<int32_t>(LoadS8(ctx, slot + 39u)) < 0);
    if (slot) {
      node = LoadU32(ctx, node);
    } else {
    }
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterCall83) {
    double_c = UnorderedLookupNumber(ctx, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterCall83;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    double_b = double_c;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 24u));
    }
    StoreU8(ctx, frame + 28u, LoadU8(ctx, 741342u));
    StoreU32(ctx, frame + 24u, LoadU32(ctx, 741338u));  // "delay"
    StoreU8(ctx, frame + 35u, 5u);
    StoreU8(ctx, frame + 29u, 0u);
    node = LoadU32(ctx, p1 + 20u);
    if (node) {
      goto step_35;
    }
    scratch_f = 0u;
    goto step_39;
  }
  release_7:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_35:;
  step_36:;
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU8(ctx, node + 27u);
    scratch_f = static_cast<uint32_t>(SignExtend8(slot) < 0);
    aux = LoadU32(ctx, node + 20u);
    slot = scratch_f ? aux : slot;
    scratch_a = static_cast<uint32_t>(slot < 5u);
    word = scratch_a ? slot : 5u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (word) {
      scratch_d = frame + 24u;
      scratch_e = node + 16u;
      scratch_f = scratch_f ? LoadU32(ctx, scratch_e) : scratch_e;
      scratch_e = CompareBytes(ctx, scratch_d, scratch_f, word);
      aux = static_cast<uint32_t>(scratch_e == 0u);
      if (aux) {
        slot = static_cast<uint32_t>(slot <= 5u);
        if (slot) {
          goto compare_2;
        }
        goto merge_9;
      }
      slot = (static_cast<int32_t>(scratch_e) < 0);
      if (slot) {
        goto merge_9;
      }
  compare_2:;
      slot = CompareBytes(ctx, scratch_f, frame + 24u, word);
      word = static_cast<uint32_t>(slot == 0u);
      if (word) {
        goto step_37;
      }
      slot = (static_cast<int32_t>(slot) >= 0);
      if (slot) {
        goto step_38;
      }
      goto merge_8;
    }
    slot = static_cast<uint32_t>(slot > 5u);
    if (slot) {
      goto merge_9;
    }
  step_37:;
    if (scratch_a) {
      goto merge_8;
    }
  step_38:;
    StoreU8(ctx, frame + 507u, 5u);
    StoreU8(ctx, frame + 501u, 0u);
    StoreU32(ctx, frame + 496u, LoadU32(ctx, 741338u));  // "delay"
    StoreU8(ctx, frame + 500u, LoadU8(ctx, 741342u));
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 472u, frame + 496u);
    aux = frame + 480u;
    word = frame + 496u;
    slot = frame + 472u;
    node = frame + 464u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii84) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, slot, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii84;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node == 1u);
    if (node) {
      goto release_9;
    }
    slot = LoadU32(ctx, frame + 480u);
    node = slot + 28u;
    slot = (static_cast<int32_t>(LoadS8(ctx, slot + 39u)) < 0);
    if (slot) {
    } else {
    }
    scratch_f = SkipWhitespace(ctx, node);
    node = (static_cast<int32_t>(LoadS8(ctx, frame + 507u)) >= 0);
    if (node) {
      goto merge_10;
    }
    node = LoadU32(ctx, frame + 496u);
    DeallocateRawBlock(ctx, node);
    goto merge_10;
  }
  // Merge point merge_8.
  merge_8:;
  node = (ResumePhase(ctx) == kResumeRunning) ? (node + 4u) : node;
  // Merge point merge_9.
  merge_9:;
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, node);
    if (node) {
      goto step_36;
    }
  }
  scratch_f = (ResumePhase(ctx) == kResumeRunning) ? 0u : scratch_f;
  // Merge point merge_10.
  merge_10:;
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, p1 + 20u);
  }
  else {
  }
  step_39:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1701669236u);
    StoreU8(ctx, frame + 35u, 4u);
    if (slot) {
      node = slot;
  step_40:;
      word = LoadU8(ctx, node + 27u);
      aux = static_cast<uint32_t>(SignExtend8(word));
      scratch_e = static_cast<int32_t>(aux) < static_cast<int32_t>(0u);
      scratch_d = LoadU32(ctx, node + 20u);
      word = scratch_e ? scratch_d : word;
      spill = static_cast<uint32_t>(word < 4u);
      scratch_a = spill ? word : 4u;
      if (scratch_a) {
        scratch_d = frame + 24u;
        scratch_h = node + 16u;
        scratch_e = scratch_e ? LoadU32(ctx, scratch_h) : scratch_h;
        scratch_h = CompareBytes(ctx, scratch_d, scratch_e, scratch_a);
        aux = static_cast<uint32_t>(scratch_h == 0u);
        if (aux) {
          word = static_cast<uint32_t>(word <= 4u);
          if (word) {
            goto compare_3;
          }
          goto merge_11;
        }
        word = (static_cast<int32_t>(scratch_h) < 0);
        if (word) {
          goto merge_11;
        }
  compare_3:;
        word = CompareBytes(ctx, scratch_e, frame + 24u, scratch_a);
        aux = static_cast<uint32_t>(word == 0u);
        if (aux) {
          goto step_41;
        }
        word = (static_cast<int32_t>(word) >= 0);
        if (word) {
          goto merge_14;
        }
        goto step_42;
      }
      word = static_cast<uint32_t>(word > 4u);
      if (word) {
        goto merge_11;
      }
  step_41:;
      word = static_cast<uint32_t>(spill == 0u);
      if (word) {
        goto merge_14;
      }
  step_42:;
      node = node + 4u;
  // Merge point merge_11.
  merge_11:;
      node = LoadU32(ctx, node);
      if (node) {
        goto step_40;
      }
    }
    StoreU8(ctx, frame + 500u, 0u);
    StoreU32(ctx, frame + 496u, 1667593069u);
    StoreU8(ctx, frame + 507u, 4u);
    node = static_cast<uint32_t>(slot == 0u);
    if (node) {
      goto merge_14;
    }
  }
  step_43:;
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU8(ctx, slot + 27u);
    scratch_a = static_cast<uint32_t>(SignExtend8(node) < 0);
    aux = LoadU32(ctx, slot + 20u);
    node = scratch_a ? aux : node;
    scratch_e = static_cast<uint32_t>(node < 4u);
    word = scratch_e ? node : 4u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (word) {
      scratch_d = frame + 496u;
      spill = slot + 16u;
      scratch_a = scratch_a ? LoadU32(ctx, spill) : spill;
      spill = CompareBytes(ctx, scratch_d, scratch_a, word);
      aux = static_cast<uint32_t>(spill == 0u);
      if (aux) {
        node = static_cast<uint32_t>(node <= 4u);
        if (node) {
          goto compare_4;
        }
        goto merge_13;
      }
      node = (static_cast<int32_t>(spill) < 0);
      if (node) {
        goto merge_13;
      }
  compare_4:;
      node = CompareBytes(ctx, scratch_a, frame + 496u, word);
      word = static_cast<uint32_t>(node == 0u);
      if (word) {
        goto step_44;
      }
      node = (static_cast<int32_t>(node) >= 0);
      if (node) {
        goto step_45;
      }
      goto merge_12;
    }
    node = static_cast<uint32_t>(node > 4u);
    if (node) {
      goto merge_13;
    }
  step_44:;
    if (scratch_e) {
      goto merge_12;
    }
  step_45:;
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1667593069u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    slot = frame + 480u;
    node = frame + 472u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii85) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, slot, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii85;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node == 1u);
    if (node) {
      goto release_8;
    }
    slot = LoadU32(ctx, frame + 496u);
    node = slot + 28u;
    slot = (static_cast<int32_t>(LoadS8(ctx, slot + 39u)) < 0);
    if (slot) {
    } else {
    }
    scratch_a = SkipWhitespace(ctx, node);
    node = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (node) {
      goto merge_15;
    }
    node = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, node);
    goto merge_15;
  }
  // Merge point merge_12.
  merge_12:;
  slot = (ResumePhase(ctx) == kResumeRunning) ? (slot + 4u) : slot;
  // Merge point merge_13.
  merge_13:;
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, slot);
    if (slot) {
      goto step_43;
    }
    goto merge_14;
  }
  release_8:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  release_9:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 507u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 496u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  release_10:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  release_11:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  // Merge point merge_14.
  merge_14:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1701669236u);
    StoreU8(ctx, frame + 35u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    slot = frame + 480u;
    node = frame + 472u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii86) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, slot, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii86;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node == 1u);
    if (node) {
      goto release_13;
    }
    slot = LoadU32(ctx, frame + 496u);
    node = slot + 28u;
    slot = (static_cast<int32_t>(LoadS8(ctx, slot + 39u)) < 0);
    if (slot) {
    } else {
    }
    scratch_a = SkipWhitespace(ctx, node);
    node = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (node) {
      goto merge_15;
    }
    node = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, node);
  }
  // Merge point merge_15.
  merge_15:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 35u, 5u);
    StoreU8(ctx, frame + 29u, 0u);
    StoreU32(ctx, frame + 24u, LoadU32(ctx, 740257u));  // "param"
    StoreU8(ctx, frame + 28u, LoadU8(ctx, 740261u));
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    scratch_d = frame + 496u;
    aux = frame + 24u;
    word = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii87) {
    HostInvokeViiiiii(ctx, 1959u, scratch_d, scratch_b, aux, 745008u, word, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii87;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot == 1u);
    if (slot) {
      goto release_14;
    }
    slot = LoadU32(ctx, frame + 496u);
    scratch_d = LoadU32(ctx, slot + 32u);
    word = LoadU8(ctx, slot + 39u);
    aux = static_cast<uint32_t>(SignExtend8(word) < 0);
    word = aux ? scratch_d : word == 5u;
  }
  if (word || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      node = slot + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii88) {
      spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 740810u, 5u);  // "alpha"
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeIiiiii88;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      node = spill;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      slot = static_cast<uint32_t>(slot == 1u);
      if (slot) {
        goto rethrow_1;
      }
      node = static_cast<uint32_t>(node == 0u);
    }
  else {
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 24u));
    }
    float_a = (f32)wasm_quiet(double_a);
    float_b = (f32)wasm_quiet(double_b);
    if (node) {
      float_b = float_b / 255.0f;
      float_a = float_a / 255.0f;
    }
    StoreU8(ctx, frame + 35u, 5u);
    StoreU8(ctx, frame + 29u, 0u);
    StoreU32(ctx, frame + 24u, LoadU32(ctx, 740257u));  // "param"
    StoreU8(ctx, frame + 28u, LoadU8(ctx, 740261u));
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    word = frame + 24u;
    slot = frame + 480u;
    node = frame + 472u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii89) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, word, 745008u, slot, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii89;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node == 1u);
    if (node) {
      goto step_46;
    }
    node = LoadU32(ctx, frame + 496u);
    aux = LoadU32(ctx, node + 32u);
    slot = LoadU8(ctx, node + 39u);
    word = static_cast<uint32_t>(SignExtend8(slot) < 0);
    slot = word ? aux : slot == 6u;
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      node = node + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii90) {
      spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 740816u, 6u);  // "xscale"
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeIiiiii90;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      slot = spill;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      word = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      node = 1u;
      word = static_cast<uint32_t>(word == 1u);
      if (word) {
        goto rethrow_1;
      }
      slot = static_cast<uint32_t>(slot == 0u);
      if (slot) {
        goto release_12;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 507u, 5u);
    StoreU8(ctx, frame + 501u, 0u);
    StoreU32(ctx, frame + 496u, LoadU32(ctx, 740257u));  // "param"
    StoreU8(ctx, frame + 500u, LoadU8(ctx, 740261u));
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 472u, frame + 496u);
    scratch_d = frame + 480u;
    aux = frame + 496u;
    word = frame + 472u;
    slot = frame + 464u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii91) {
    HostInvokeViiiiii(ctx, 1959u, scratch_d, scratch_b, aux, 745008u, word, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii91;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot == 1u);
    if (slot) {
      goto release_15;
    }
    slot = LoadU32(ctx, frame + 480u);
    scratch_d = LoadU32(ctx, slot + 32u);
    word = LoadU8(ctx, slot + 39u);
    aux = static_cast<uint32_t>(SignExtend8(word) < 0);
    word = aux ? scratch_d : word == 6u;
  }
  if (word || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      node = slot + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii92) {
      spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 740823u, 6u);  // "yscale"
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeIiiiii92;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      node = spill;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      slot = static_cast<uint32_t>(slot == 1u);
      if (slot) {
        goto rethrow_1;
      }
      node = static_cast<uint32_t>(node == 0u);
    }
  else {
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = (static_cast<int32_t>(LoadS8(ctx, frame + 507u)) >= 0);
    if (slot) {
      goto release_12;
    }
    slot = LoadU32(ctx, frame + 496u);
    DeallocateRawBlock(ctx, slot);
  }
  // Merge point release_12.
  release_12:;
  if (ResumePhase(ctx) == kResumeRunning) {
    if (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 24u));
    }
    if (node) {
      float_b = float_b / 100.0f;
      float_a = float_a / 100.0f;
    }
    StoreU8(ctx, frame + 35u, 5u);
    StoreU8(ctx, frame + 29u, 0u);
    StoreU32(ctx, frame + 24u, LoadU32(ctx, 740257u));  // "param"
    StoreU8(ctx, frame + 28u, LoadU8(ctx, 740261u));
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    scratch_d = frame + 496u;
    aux = frame + 24u;
    word = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii93) {
    HostInvokeViiiiii(ctx, 1959u, scratch_d, scratch_b, aux, 745008u, word, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii93;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot == 1u);
    if (slot) {
      goto release_17;
    }
    slot = LoadU32(ctx, frame + 496u);
    scratch_d = LoadU32(ctx, slot + 32u);
    word = LoadU8(ctx, slot + 39u);
    aux = static_cast<uint32_t>(SignExtend8(word) < 0);
    word = aux ? scratch_d : word == 4u;
  }
  if (word || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      node = slot + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii94) {
      spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 740801u, 4u);  // "left"
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeIiiiii94;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      node = spill;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      slot = static_cast<uint32_t>(slot == 1u);
      if (slot) {
        goto rethrow_1;
      }
      node = static_cast<uint32_t>(node == 0u);
    }
  else {
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
    if (slot) {
      slot = LoadU32(ctx, frame + 24u);
      DeallocateRawBlock(ctx, slot);
    }
    node = static_cast<uint32_t>(node == 0u);
    if (node) {
      goto step_47;
    }
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterFoldLayoutOffset95) {
    spill = HostFoldLayoutOffset(ctx, 1032u, 112u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterFoldLayoutOffset95;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    word = spill;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node == 1u);
    if (node) {
      goto merge_16;
    }
    StoreU32(ctx, frame + 452u, ret);
    StoreU64(ctx, frame + 444u, 4294967380ull);
    StoreU64(ctx, word + 4u, 0ull);
    StoreU32(ctx, word, 581044u);
    StoreU64(ctx, word + 12u, 0ull);
    StoreU32(ctx, word + 20u, 0u);
    StoreU64(ctx, word + 48u, 0ull);
    StoreU32(ctx, word + 44u, scratch_f);
    StoreU32(ctx, word + 40u, scratch_a);
    StoreF64(ctx, word + 32u, (f64)float_b);
    StoreF64(ctx, word + 24u, (f64)float_a);
    StoreU64(ctx, word + 56u, 0ull);
    StoreU64(ctx, word + 61u, 0ull);
    StoreU32(ctx, word + 72u, 588917u);
    StoreU64(ctx, word + 76u, LoadU64(ctx, frame + 444u));
    StoreU32(ctx, word + 84u, LoadU32(ctx, frame + 452u));
    StoreU8(ctx, word + 104u, 0u);
    StoreU32(ctx, word + 88u, 0u);
    StoreU32(ctx, frame + 440u, 0u);
    node = LoadU32(ctx, p0 + 13704u);
    if (node) {
      goto merge_17;
    }
    node = LoadU32(ctx, LoadU32(ctx, ret) + 84u);
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeVif96) {
    HostInvokeVif(ctx, node, ret, float_a);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeVif96;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node != 1u);
    if (node) {
      goto merge_17;
    }
    goto merge_16;
  }
  release_13:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  release_14:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_46:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
    goto release_16;
  }
  release_15:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 507u)) >= 0);
    if (p0) {
      goto release_16;
    }
    p0 = LoadU32(ctx, frame + 496u);
    DeallocateRawBlock(ctx, p0);
  }
  // Merge point release_16.
  release_16:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  release_17:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_47:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 35u, 5u);
    StoreU8(ctx, frame + 29u, 0u);
    StoreU32(ctx, frame + 24u, LoadU32(ctx, 740257u));  // "param"
    StoreU8(ctx, frame + 28u, LoadU8(ctx, 740261u));
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    scratch_d = frame + 496u;
    aux = frame + 24u;
    word = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii97) {
    HostInvokeViiiiii(ctx, 1959u, scratch_d, scratch_b, aux, 745008u, word, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii97;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      scratch_d = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      aux = static_cast<uint32_t>(SignExtend8(word) < 0);
      word = aux ? scratch_d : word == 3u;
    }
    if (word || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii98) {
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 740806u, 3u);  // "top"
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii98;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_48;
      }
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterFoldLayoutOffset99) {
      spill = HostFoldLayoutOffset(ctx, 1032u, 112u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterFoldLayoutOffset99;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      word = spill;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      node = static_cast<uint32_t>(node == 1u);
      if (node) {
        goto merge_16;
      }
      StoreU32(ctx, frame + 436u, ret);
      StoreU64(ctx, frame + 428u, 4294967384ull);
      StoreU64(ctx, word + 4u, 0ull);
      StoreU32(ctx, word, 581044u);
      StoreU64(ctx, word + 12u, 0ull);
      StoreU32(ctx, word + 20u, 0u);
      StoreU64(ctx, word + 48u, 0ull);
      StoreU32(ctx, word + 44u, scratch_f);
      StoreU32(ctx, word + 40u, scratch_a);
      StoreF64(ctx, word + 32u, (f64)float_b);
      StoreF64(ctx, word + 24u, (f64)float_a);
      StoreU64(ctx, word + 56u, 0ull);
      StoreU64(ctx, word + 61u, 0ull);
      StoreU32(ctx, word + 72u, 588917u);
      StoreU64(ctx, word + 76u, LoadU64(ctx, frame + 428u));
      StoreU32(ctx, word + 84u, LoadU32(ctx, frame + 436u));
      StoreU8(ctx, word + 104u, 0u);
      StoreU32(ctx, word + 88u, 0u);
      StoreU32(ctx, frame + 424u, 0u);
      node = LoadU32(ctx, p0 + 13704u);
      if (node) {
        goto merge_17;
      }
      node = LoadU32(ctx, LoadU32(ctx, ret) + 88u);
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeVif100) {
      HostInvokeVif(ctx, node, ret, float_a);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeVif100;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      node = static_cast<uint32_t>(node != 1u);
      if (node) {
        goto merge_17;
      }
      goto merge_16;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_48:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 35u, 5u);
    StoreU8(ctx, frame + 29u, 0u);
    StoreU32(ctx, frame + 24u, LoadU32(ctx, 740257u));  // "param"
    StoreU8(ctx, frame + 28u, LoadU8(ctx, 740261u));
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    scratch_d = frame + 496u;
    aux = frame + 24u;
    word = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii101) {
    HostInvokeViiiiii(ctx, 1959u, scratch_d, scratch_b, aux, 745008u, word, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii101;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      scratch_d = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      aux = static_cast<uint32_t>(SignExtend8(word) < 0);
      word = aux ? scratch_d : word == 5u;
    }
    if (word || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii102) {
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 740810u, 5u);  // "alpha"
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii102;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_49;
      }
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterFoldLayoutOffset103) {
      spill = HostFoldLayoutOffset(ctx, 1032u, 112u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterFoldLayoutOffset103;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      word = spill;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      node = static_cast<uint32_t>(node == 1u);
      if (node) {
        goto merge_16;
      }
      StoreU32(ctx, frame + 420u, ret);
      StoreU64(ctx, frame + 412u, 4294967388ull);
      StoreU64(ctx, word + 4u, 0ull);
      StoreU32(ctx, word, 581044u);
      StoreU64(ctx, word + 12u, 0ull);
      StoreU32(ctx, word + 20u, 0u);
      StoreU64(ctx, word + 48u, 0ull);
      StoreU32(ctx, word + 44u, scratch_f);
      StoreU32(ctx, word + 40u, scratch_a);
      StoreF64(ctx, word + 32u, (f64)float_b);
      StoreF64(ctx, word + 24u, (f64)float_a);
      StoreU64(ctx, word + 56u, 0ull);
      StoreU64(ctx, word + 61u, 0ull);
      StoreU32(ctx, word + 72u, 588917u);
      StoreU64(ctx, word + 76u, LoadU64(ctx, frame + 412u));
      StoreU32(ctx, word + 84u, LoadU32(ctx, frame + 420u));
      StoreU8(ctx, word + 104u, 0u);
      StoreU32(ctx, word + 88u, 0u);
      StoreU32(ctx, frame + 408u, 0u);
      node = LoadU32(ctx, p0 + 13704u);
      if (node) {
        goto merge_17;
      }
      node = LoadU32(ctx, LoadU32(ctx, ret) + 92u);
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeVif104) {
      HostInvokeVif(ctx, node, ret, float_a);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeVif104;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      node = static_cast<uint32_t>(node != 1u);
      if (node) {
        goto merge_17;
      }
      goto merge_16;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_49:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 35u, 5u);
    StoreU8(ctx, frame + 29u, 0u);
    StoreU32(ctx, frame + 24u, LoadU32(ctx, 740257u));  // "param"
    StoreU8(ctx, frame + 28u, LoadU8(ctx, 740261u));
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    scratch_d = frame + 496u;
    aux = frame + 24u;
    word = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii105) {
    HostInvokeViiiiii(ctx, 1959u, scratch_d, scratch_b, aux, 745008u, word, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii105;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      scratch_d = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      aux = static_cast<uint32_t>(SignExtend8(word) < 0);
      word = aux ? scratch_d : word == 6u;
    }
    if (word || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii106) {
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 740816u, 6u);  // "xscale"
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii106;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_50;
      }
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterFoldLayoutOffset107) {
      spill = HostFoldLayoutOffset(ctx, 1032u, 112u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterFoldLayoutOffset107;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      word = spill;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      node = static_cast<uint32_t>(node == 1u);
      if (node) {
        goto merge_16;
      }
      StoreU32(ctx, frame + 404u, ret);
      StoreU64(ctx, frame + 396u, 4294967400ull);
      StoreU64(ctx, word + 4u, 0ull);
      StoreU32(ctx, word, 581044u);
      StoreU64(ctx, word + 12u, 0ull);
      StoreU32(ctx, word + 20u, 0u);
      StoreU64(ctx, word + 48u, 0ull);
      StoreU32(ctx, word + 44u, scratch_f);
      StoreU32(ctx, word + 40u, scratch_a);
      StoreF64(ctx, word + 32u, (f64)float_b);
      StoreF64(ctx, word + 24u, (f64)float_a);
      StoreU64(ctx, word + 56u, 0ull);
      StoreU64(ctx, word + 61u, 0ull);
      StoreU32(ctx, word + 72u, 588917u);
      StoreU64(ctx, word + 76u, LoadU64(ctx, frame + 396u));
      StoreU32(ctx, word + 84u, LoadU32(ctx, frame + 404u));
      StoreU8(ctx, word + 104u, 0u);
      StoreU32(ctx, word + 88u, 0u);
      StoreU32(ctx, frame + 392u, 0u);
      node = LoadU32(ctx, p0 + 13704u);
      if (node) {
        goto merge_17;
      }
      node = LoadU32(ctx, LoadU32(ctx, ret) + 104u);
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeVif108) {
      HostInvokeVif(ctx, node, ret, float_a);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeVif108;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      node = static_cast<uint32_t>(node != 1u);
      if (node) {
        goto merge_17;
      }
      goto merge_16;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_50:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 35u, 5u);
    StoreU8(ctx, frame + 29u, 0u);
    StoreU32(ctx, frame + 24u, LoadU32(ctx, 740257u));  // "param"
    StoreU8(ctx, frame + 28u, LoadU8(ctx, 740261u));
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    scratch_d = frame + 496u;
    aux = frame + 24u;
    word = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii109) {
    HostInvokeViiiiii(ctx, 1959u, scratch_d, scratch_b, aux, 745008u, word, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii109;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot != 1u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, frame + 496u);
      scratch_d = LoadU32(ctx, slot + 32u);
      word = LoadU8(ctx, slot + 39u);
      aux = static_cast<uint32_t>(SignExtend8(word) < 0);
      word = aux ? scratch_d : word == 6u;
    }
    if (word || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        node = slot + 28u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii110) {
        spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 740823u, 6u);  // "yscale"
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterInvokeIiiiii110;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        node = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        slot = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        slot = static_cast<uint32_t>(slot == 1u);
        if (slot) {
          goto rethrow_1;
        }
        node = static_cast<uint32_t>(node == 0u);
      }
    else {
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (slot) {
        slot = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, slot);
      }
      node = static_cast<uint32_t>(node == 0u);
      if (node) {
        goto step_51;
      }
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterFoldLayoutOffset111) {
      spill = HostFoldLayoutOffset(ctx, 1032u, 112u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterFoldLayoutOffset111;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      word = spill;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      node = static_cast<uint32_t>(node == 1u);
      if (node) {
        goto merge_16;
      }
      StoreU32(ctx, frame + 388u, ret);
      StoreU64(ctx, frame + 380u, 4294967404ull);
      StoreU64(ctx, word + 4u, 0ull);
      StoreU32(ctx, word, 581044u);
      StoreU64(ctx, word + 12u, 0ull);
      StoreU32(ctx, word + 20u, 0u);
      StoreU64(ctx, word + 48u, 0ull);
      StoreU32(ctx, word + 44u, scratch_f);
      StoreU32(ctx, word + 40u, scratch_a);
      StoreF64(ctx, word + 32u, (f64)float_b);
      StoreF64(ctx, word + 24u, (f64)float_a);
      StoreU64(ctx, word + 56u, 0ull);
      StoreU64(ctx, word + 61u, 0ull);
      StoreU32(ctx, word + 72u, 588917u);
      StoreU64(ctx, word + 76u, LoadU64(ctx, frame + 380u));
      StoreU32(ctx, word + 84u, LoadU32(ctx, frame + 388u));
      StoreU8(ctx, word + 104u, 0u);
      StoreU32(ctx, word + 88u, 0u);
      StoreU32(ctx, frame + 376u, 0u);
      node = LoadU32(ctx, p0 + 13704u);
      if (node) {
        goto merge_17;
      }
      node = LoadU32(ctx, LoadU32(ctx, ret) + 108u);
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeVif112) {
      HostInvokeVif(ctx, node, ret, float_a);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeVif112;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      node = static_cast<uint32_t>(node == 1u);
      if (node) {
        goto merge_16;
      }
      goto merge_17;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_51:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 35u, 5u);
    StoreU8(ctx, frame + 29u, 0u);
    StoreU32(ctx, frame + 24u, LoadU32(ctx, 740257u));  // "param"
    StoreU8(ctx, frame + 28u, LoadU8(ctx, 740261u));
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    scratch_d = frame + 496u;
    aux = frame + 24u;
    word = frame + 480u;
    slot = frame + 472u;
    node = 0u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii113) {
    HostInvokeViiiiii(ctx, 1959u, scratch_d, scratch_b, aux, 745008u, word, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii113;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot == 1u);
    if (slot) {
      goto release_18;
    }
    slot = LoadU32(ctx, frame + 496u);
    scratch_d = LoadU32(ctx, slot + 32u);
    word = LoadU8(ctx, slot + 39u);
    aux = static_cast<uint32_t>(SignExtend8(word) < 0);
    word = aux ? scratch_d : word == 6u;
  }
  if (word || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      node = slot + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii114) {
      spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 740830u, 6u);  // "rotate"
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeIiiiii114;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      node = spill;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      slot = static_cast<uint32_t>(slot == 1u);
      if (slot) {
        goto rethrow_1;
      }
      node = static_cast<uint32_t>(node == 0u);
    }
  else {
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
    if (slot) {
      slot = LoadU32(ctx, frame + 24u);
      DeallocateRawBlock(ctx, slot);
    }
    node = static_cast<uint32_t>(node == 0u);
    if (node) {
      goto merge_40;
    }
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterFoldLayoutOffset115) {
    spill = HostFoldLayoutOffset(ctx, 1032u, 112u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterFoldLayoutOffset115;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    word = spill;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node == 1u);
    if (node) {
      goto merge_16;
    }
    StoreU32(ctx, frame + 372u, ret);
    StoreU64(ctx, frame + 364u, 4294967408ull);
    StoreU64(ctx, word + 4u, 0ull);
    StoreU32(ctx, word, 581044u);
    StoreU64(ctx, word + 12u, 0ull);
    StoreU32(ctx, word + 20u, 0u);
    StoreU64(ctx, word + 48u, 0ull);
    StoreU32(ctx, word + 44u, scratch_f);
    StoreU32(ctx, word + 40u, scratch_a);
    StoreF64(ctx, word + 32u, (f64)float_b);
    StoreF64(ctx, word + 24u, (f64)float_a);
    StoreU64(ctx, word + 56u, 0ull);
    StoreU64(ctx, word + 61u, 0ull);
    StoreU32(ctx, word + 72u, 588917u);
    StoreU64(ctx, word + 76u, LoadU64(ctx, frame + 364u));
    StoreU32(ctx, word + 84u, LoadU32(ctx, frame + 372u));
    StoreU8(ctx, word + 104u, 0u);
    StoreU32(ctx, word + 88u, 0u);
    StoreU32(ctx, frame + 360u, 0u);
    node = LoadU32(ctx, p0 + 13704u);
    if (node) {
      goto merge_17;
    }
    node = LoadU32(ctx, LoadU32(ctx, ret) + 112u);
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeVif116) {
    HostInvokeVif(ctx, node, ret, float_a);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeVif116;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node != 1u);
    if (node) {
      goto merge_17;
    }
  }
  // Merge point merge_16.
  merge_16:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
    goto merge_45;
  }
  release_18:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  // Merge point merge_17.
  merge_17:;
  if (ResumePhase(ctx) == kResumeRunning) {
    if (flag == 0u) {
      StoreU32(ctx, word + 4u, result);
    }
    StoreU8(ctx, frame + 35u, 6u);
    StoreU8(ctx, frame + 30u, 0u);
    StoreU16(ctx, frame + 28u, LoadU16(ctx, 742287u));  // "te"
    StoreU32(ctx, frame + 24u, LoadU32(ctx, 742283u));  // "delete"
    node = LoadU32(ctx, p1 + 20u);
    slot = static_cast<uint32_t>(node == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (slot) {
      flag = 0u;
      goto step_55;
    }
  }
  step_52:;
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU8(ctx, node + 27u);
    result = static_cast<uint32_t>(SignExtend8(slot) < 0);
    flag = LoadU32(ctx, node + 20u);
    slot = result ? flag : slot;
    scratch_f = static_cast<uint32_t>(slot < 6u);
    ret = scratch_f ? slot : 6u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (ret) {
      aux = frame + 24u;
      scratch_a = node + 16u;
      result = result ? LoadU32(ctx, scratch_a) : scratch_a;
      scratch_a = CompareBytes(ctx, aux, result, ret);
      flag = static_cast<uint32_t>(scratch_a == 0u);
      if (flag) {
        slot = static_cast<uint32_t>(slot <= 6u);
        if (slot) {
          goto compare_5;
        }
        goto merge_19;
      }
      slot = (static_cast<int32_t>(scratch_a) < 0);
      if (slot) {
        goto merge_19;
      }
  compare_5:;
      slot = CompareBytes(ctx, result, frame + 24u, ret);
      ret = static_cast<uint32_t>(slot == 0u);
      if (ret) {
        goto step_53;
      }
      slot = (static_cast<int32_t>(slot) >= 0);
      if (slot) {
        goto step_54;
      }
      goto merge_18;
    }
    slot = static_cast<uint32_t>(slot > 6u);
    if (slot) {
      goto merge_19;
    }
  step_53:;
    if (scratch_f) {
      goto merge_18;
    }
  step_54:;
    StoreU8(ctx, frame + 507u, 6u);
    StoreU8(ctx, frame + 502u, 0u);
    StoreU32(ctx, frame + 496u, LoadU32(ctx, 742283u));  // "delete"
    StoreU16(ctx, frame + 500u, LoadU16(ctx, 742287u));  // "te"
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 472u, frame + 496u);
    flag = frame + 480u;
    ret = frame + 496u;
    slot = frame + 472u;
    node = frame + 464u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii117) {
    HostInvokeViiiiii(ctx, 1959u, flag, scratch_b, ret, 745008u, slot, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii117;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    flag = 1u;
    node = static_cast<uint32_t>(node == 1u);
    if (node) {
      goto release_22;
    }
    node = LoadU32(ctx, frame + 480u);
    aux = LoadU32(ctx, node + 32u);
    slot = LoadU8(ctx, node + 39u);
    ret = static_cast<uint32_t>(SignExtend8(slot) < 0);
    slot = ret ? aux : slot == 1u;
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      node = node + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii118) {
      spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 740137u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeIiiiii118;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      node = spill;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      slot = static_cast<uint32_t>(slot == 1u);
      if (slot) {
        goto rethrow_1;
      }
      flag = static_cast<uint32_t>(node != 0u);
    }
  else {
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = (static_cast<int32_t>(LoadS8(ctx, frame + 507u)) >= 0);
    if (node) {
      goto merge_20;
    }
    node = LoadU32(ctx, frame + 496u);
    DeallocateRawBlock(ctx, node);
    goto merge_20;
  }
  // Merge point merge_18.
  merge_18:;
  node = (ResumePhase(ctx) == kResumeRunning) ? (node + 4u) : node;
  // Merge point merge_19.
  merge_19:;
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, node);
    if (node) {
      goto step_52;
    }
  }
  flag = (ResumePhase(ctx) == kResumeRunning) ? 0u : flag;
  // Merge point merge_20.
  merge_20:;
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, p1 + 20u);
  }
  else {
  }
  step_55:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1886351212u);
    StoreU8(ctx, frame + 35u, 4u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    node = (ResumePhase(ctx) == kResumeRunning) ? slot : node;
  step_56:;
    if (ResumePhase(ctx) == kResumeRunning) {
      ret = LoadU8(ctx, node + 27u);
      aux = static_cast<uint32_t>(SignExtend8(ret));
      scratch_f = static_cast<int32_t>(aux) < static_cast<int32_t>(0u);
      scratch_d = LoadU32(ctx, node + 20u);
      ret = scratch_f ? scratch_d : ret;
      scratch_a = static_cast<uint32_t>(ret < 4u);
      result = scratch_a ? ret : 4u;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (result) {
        scratch_d = frame + 24u;
        scratch_e = node + 16u;
        scratch_f = scratch_f ? LoadU32(ctx, scratch_e) : scratch_e;
        scratch_e = CompareBytes(ctx, scratch_d, scratch_f, result);
        aux = static_cast<uint32_t>(scratch_e == 0u);
        if (aux) {
          ret = static_cast<uint32_t>(ret <= 4u);
          if (ret) {
            goto compare_6;
          }
          goto merge_22;
        }
        ret = (static_cast<int32_t>(scratch_e) < 0);
        if (ret) {
          goto merge_22;
        }
  compare_6:;
        ret = CompareBytes(ctx, scratch_f, frame + 24u, result);
        aux = static_cast<uint32_t>(ret == 0u);
        if (aux) {
          goto step_57;
        }
        ret = (static_cast<int32_t>(ret) >= 0);
        if (ret) {
          goto step_58;
        }
        goto merge_21;
      }
      ret = static_cast<uint32_t>(ret > 4u);
      if (ret) {
        goto merge_22;
      }
  step_57:;
      if (scratch_a) {
        goto merge_21;
      }
  step_58:;
      StoreU8(ctx, frame + 28u, 0u);
      StoreU32(ctx, frame + 24u, 1886351212u);
      StoreU8(ctx, frame + 35u, 4u);
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, frame + 480u, frame + 24u);
      aux = frame + 496u;
      ret = frame + 24u;
      slot = frame + 480u;
      node = frame + 472u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii119) {
      HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, ret, 745008u, slot, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeViiiiii119;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      node = static_cast<uint32_t>(node == 1u);
      if (node) {
        goto release_21;
      }
      slot = LoadU32(ctx, frame + 496u);
      node = slot + 28u;
      slot = (static_cast<int32_t>(LoadS8(ctx, slot + 39u)) < 0);
      if (slot) {
      } else {
      }
      scratch_f = SkipWhitespace(ctx, node);
      node = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
      if (node) {
        goto merge_27;
      }
      node = LoadU32(ctx, frame + 24u);
      DeallocateRawBlock(ctx, node);
      goto merge_27;
    }
  // Merge point merge_21.
  merge_21:;
    node = (ResumePhase(ctx) == kResumeRunning) ? (node + 4u) : node;
  // Merge point merge_22.
  merge_22:;
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, node);
      if (node) {
        goto step_56;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 35u, 6u);
    StoreU8(ctx, frame + 30u, 0u);
    StoreU32(ctx, frame + 24u, LoadU32(ctx, 742290u));  // "repeat"
    node = LoadU16(ctx, 742294u);  // "at"
    StoreU16(ctx, frame + 28u, node);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    node = (ResumePhase(ctx) == kResumeRunning) ? slot : node;
  step_59:;
    if (ResumePhase(ctx) == kResumeRunning) {
      ret = LoadU8(ctx, node + 27u);
      aux = static_cast<uint32_t>(SignExtend8(ret));
      scratch_f = static_cast<int32_t>(aux) < static_cast<int32_t>(0u);
      scratch_d = LoadU32(ctx, node + 20u);
      ret = scratch_f ? scratch_d : ret;
      scratch_a = static_cast<uint32_t>(ret < 6u);
      result = scratch_a ? ret : 6u;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (result) {
        scratch_d = frame + 24u;
        scratch_e = node + 16u;
        scratch_f = scratch_f ? LoadU32(ctx, scratch_e) : scratch_e;
        scratch_e = CompareBytes(ctx, scratch_d, scratch_f, result);
        aux = static_cast<uint32_t>(scratch_e == 0u);
        if (aux) {
          ret = static_cast<uint32_t>(ret <= 6u);
          if (ret) {
            goto compare_7;
          }
          goto merge_24;
        }
        ret = (static_cast<int32_t>(scratch_e) < 0);
        if (ret) {
          goto merge_24;
        }
  compare_7:;
        ret = CompareBytes(ctx, scratch_f, frame + 24u, result);
        aux = static_cast<uint32_t>(ret == 0u);
        if (aux) {
          goto step_60;
        }
        ret = (static_cast<int32_t>(ret) >= 0);
        if (ret) {
          goto step_61;
        }
        goto merge_23;
      }
      ret = static_cast<uint32_t>(ret > 6u);
      if (ret) {
        goto merge_24;
      }
  step_60:;
      if (scratch_a) {
        goto merge_23;
      }
  step_61:;
      StoreU8(ctx, frame + 35u, 6u);
      StoreU32(ctx, frame + 24u, LoadU32(ctx, 742290u));  // "repeat"
      StoreU16(ctx, frame + 28u, LoadU16(ctx, 742294u));  // "at"
      StoreU8(ctx, frame + 30u, 0u);
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, frame + 480u, frame + 24u);
      aux = frame + 496u;
      ret = frame + 24u;
      slot = frame + 480u;
      node = frame + 472u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii120) {
      HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, ret, 745008u, slot, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeViiiiii120;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      node = static_cast<uint32_t>(node == 1u);
      if (node) {
        goto release_20;
      }
      slot = LoadU32(ctx, frame + 496u);
      node = slot + 28u;
      slot = (static_cast<int32_t>(LoadS8(ctx, slot + 39u)) < 0);
      if (slot) {
      } else {
      }
      scratch_f = SkipWhitespace(ctx, node);
      node = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
      if (node) {
        goto merge_27;
      }
      node = LoadU32(ctx, frame + 24u);
      DeallocateRawBlock(ctx, node);
      goto merge_27;
    }
  // Merge point merge_23.
  merge_23:;
    node = (ResumePhase(ctx) == kResumeRunning) ? (node + 4u) : node;
  // Merge point merge_24.
  merge_24:;
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, node);
      if (node) {
        goto step_59;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1870229369u);
    StoreU8(ctx, frame + 35u, 4u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
  step_62:;
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU8(ctx, slot + 27u);
      result = static_cast<uint32_t>(SignExtend8(node) < 0);
      aux = LoadU32(ctx, slot + 20u);
      node = result ? aux : node;
      scratch_f = static_cast<uint32_t>(node < 4u);
      ret = scratch_f ? node : 4u;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (ret) {
        scratch_d = frame + 24u;
        scratch_a = slot + 16u;
        result = result ? LoadU32(ctx, scratch_a) : scratch_a;
        scratch_a = CompareBytes(ctx, scratch_d, result, ret);
        aux = static_cast<uint32_t>(scratch_a == 0u);
        if (aux) {
          node = static_cast<uint32_t>(node <= 4u);
          if (node) {
            goto compare_8;
          }
          goto merge_26;
        }
        node = (static_cast<int32_t>(scratch_a) < 0);
        if (node) {
          goto merge_26;
        }
  compare_8:;
        node = CompareBytes(ctx, result, frame + 24u, ret);
        ret = static_cast<uint32_t>(node == 0u);
        if (ret) {
          goto step_63;
        }
        node = (static_cast<int32_t>(node) >= 0);
        if (node) {
          goto step_64;
        }
        goto merge_25;
      }
      node = static_cast<uint32_t>(node > 4u);
      if (node) {
        goto merge_26;
      }
  step_63:;
      if (scratch_f) {
        goto merge_25;
      }
  step_64:;
      StoreU8(ctx, frame + 28u, 0u);
      StoreU32(ctx, frame + 24u, 1870229369u);
      StoreU8(ctx, frame + 35u, 4u);
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, frame + 480u, frame + 24u);
      aux = frame + 496u;
      ret = frame + 24u;
      slot = frame + 480u;
      node = frame + 472u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii121) {
      HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, ret, 745008u, slot, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeViiiiii121;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      node = static_cast<uint32_t>(node == 1u);
      if (node) {
        goto release_19;
      }
      slot = LoadU32(ctx, frame + 496u);
      node = slot + 28u;
      slot = (static_cast<int32_t>(LoadS8(ctx, slot + 39u)) < 0);
      if (slot) {
      } else {
      }
      result = SkipWhitespace(ctx, node);
      node = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) < 0);
      if (node) {
        node = LoadU32(ctx, frame + 24u);
        DeallocateRawBlock(ctx, node);
      }
      StoreU32(ctx, word + 60u, result);
      scratch_f = 0u;
      goto step_66;
    }
  // Merge point merge_25.
  merge_25:;
    slot = (ResumePhase(ctx) == kResumeRunning) ? (slot + 4u) : slot;
  // Merge point merge_26.
  merge_26:;
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, slot);
      if (slot) {
        goto step_62;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch_f = 0u;
    goto step_65;
  }
  release_19:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  release_20:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  release_21:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  release_22:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 507u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 496u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  // Merge point merge_27.
  merge_27:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, word + 56u, scratch_f);
  }
  step_65:;
  result = (ResumePhase(ctx) == kResumeRunning) ? 0u : result;
  step_66:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 33u, 0u);
    StoreU8(ctx, frame + 32u, LoadU8(ctx, 742305u));
    StoreU8(ctx, frame + 35u, 9u);
    StoreU64(ctx, frame + 24u, LoadU64(ctx, 742297u));  // "loopdelay"
    node = LoadU32(ctx, p1 + 20u);
    slot = static_cast<uint32_t>(node == 0u);
  }
  if (slot ? (ResumePhase(ctx) == kResumeRunning) : 0u) {
    goto merge_30;
  }
  step_67:;
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU8(ctx, node + 27u);
    scratch_a = static_cast<uint32_t>(SignExtend8(slot) < 0);
    aux = LoadU32(ctx, node + 20u);
    slot = scratch_a ? aux : slot;
    scratch_e = static_cast<uint32_t>(slot < 9u);
    ret = scratch_e ? slot : 9u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (ret) {
      scratch_d = frame + 24u;
      spill = node + 16u;
      scratch_a = scratch_a ? LoadU32(ctx, spill) : spill;
      spill = CompareBytes(ctx, scratch_d, scratch_a, ret);
      aux = static_cast<uint32_t>(spill == 0u);
      if (aux) {
        slot = static_cast<uint32_t>(slot <= 9u);
        if (slot) {
          goto compare_9;
        }
        goto merge_29;
      }
      slot = (static_cast<int32_t>(spill) < 0);
      if (slot) {
        goto merge_29;
      }
  compare_9:;
      slot = CompareBytes(ctx, scratch_a, frame + 24u, ret);
      ret = static_cast<uint32_t>(slot == 0u);
      if (ret) {
        goto step_68;
      }
      slot = (static_cast<int32_t>(slot) >= 0);
      if (slot) {
        goto step_69;
      }
      goto merge_28;
    }
    slot = static_cast<uint32_t>(slot > 9u);
    if (slot) {
      goto merge_29;
    }
  step_68:;
    if (scratch_e) {
      goto merge_28;
    }
  step_69:;
    StoreU8(ctx, frame + 32u, LoadU8(ctx, 742305u));
    StoreU8(ctx, frame + 35u, 9u);
    StoreU64(ctx, frame + 24u, LoadU64(ctx, 742297u));  // "loopdelay"
    StoreU8(ctx, frame + 33u, 0u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    ret = frame + 24u;
    slot = frame + 480u;
    node = frame + 472u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii122) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, ret, 745008u, slot, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii122;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node == 1u);
    if (node) {
      goto release_23;
    }
    slot = LoadU32(ctx, frame + 496u);
    node = slot + 28u;
    ret = word;
    slot = (static_cast<int32_t>(LoadS8(ctx, slot + 39u)) < 0);
    if (slot) {
    } else {
    }
    StoreU32(ctx, ret + 64u, SkipWhitespace(ctx, node));
    node = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (node) {
      goto merge_30;
    }
    node = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, node);
    goto merge_30;
  }
  // Merge point merge_28.
  merge_28:;
  node = (ResumePhase(ctx) == kResumeRunning) ? (node + 4u) : node;
  // Merge point merge_29.
  merge_29:;
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, node);
    if (node) {
      goto step_67;
    }
  }
  // Merge point merge_30.
  merge_30:;
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, p0 + 13704u);
    if (node) {
      goto step_72;
    }
    node = LoadU8(ctx, 848200u);
    if (node) {
      goto sync_1;
    }
    StoreU32(ctx, kDataD015C, 0u);
    node = frame + 24u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterSyncSlot123) {
    HostSyncSlot(ctx, 1800u, node, 848176u, 819656u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterSyncSlot123;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node != 1u);
    if (node) {
      goto step_70;
    }
    goto assert_fail_1;
  }
  release_23:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  sync_1:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, kDataD015C, 0u);
    node = frame + 24u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterSyncSlot124) {
    HostSyncSlot(ctx, 1800u, node, 848204u, 819656u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterSyncSlot124;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node == 1u);
    if (node) {
      goto assert_fail_1;
    }
  }
  step_70:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, LoadU32(ctx, frame + 24u) + 12u, word);
    scratch_e = LoadU32(ctx, 819656u);
    StoreU32(ctx, 819656u, scratch_e + 1u);
    node = static_cast<uint32_t>(scratch_g == 0u);
    if (node) {
      goto assert_fail_2;
    }
    node = LoadU32(ctx, LoadU32(ctx, scratch_g) + 380u);
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterOsSchedule125) {
    HostOsSchedule(ctx, node, scratch_g, scratch_e);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterOsSchedule125;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node != 1u);
    if (node) {
      goto merge_32;
    }
  }
  // Merge point assert_fail_1.
  assert_fail_1:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
    goto merge_45;
  }
  assert_fail_2:;
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, p0 + 16u);
    slot = static_cast<uint32_t>(node == 0u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterFailAssertion126) {
      HostFailAssertion(ctx, kAssertWord, kAssertExpression, kAssertFile, kAssertLine,
                        kAssertFunction);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterFailAssertion126;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0) {
        goto trap_2;
      }
      goto rethrow_1;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 498u, 0u);
    StoreU16(ctx, frame + 496u, 25705u);
    StoreU8(ctx, frame + 507u, 2u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 472u, frame + 496u);
    scratch_d = frame + 480u;
    aux = frame + 496u;
    ret = frame + 472u;
    slot = frame + 464u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii127) {
    HostInvokeViiiiii(ctx, 1959u, scratch_d, scratch_b, aux, 745008u, ret, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii127;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot == 1u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (slot) {
      goto merge_31;
    }
    StoreU32(ctx, kDataD015C, 0u);
    ret = frame + 24u;
    slot = LoadU32(ctx, frame + 480u) + 28u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii128) {
    spill = HostInvokeIiiiii(ctx, 854u, ret, slot, 1u, 4294967295u, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeIiiiii128;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    scratch_g = spill;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot == 1u);
    if (slot) {
      goto merge_31;
    }
    StoreU32(ctx, frame + 472u, scratch_g);
    StoreU32(ctx, kDataD015C, 0u);
    aux = frame + 480u;
    ret = node + 88u;
    slot = frame + 472u;
    node = frame + 464u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii129) {
    HostInvokeViiiiii(ctx, 2658u, aux, ret, scratch_g, 745008u, slot, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii129;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node == 1u);
    if (node) {
      goto release_24;
    }
    scratch_a = LoadU32(ctx, frame + 480u);
    slot = scratch_a + 32u;
    node = LoadU32(ctx, slot);
    if (node) {
      slot = scratch_a + 32u;
  step_71:;
      ret = LoadU32(ctx, node + 16u);
      aux = static_cast<uint32_t>(scratch_e < ret);
      if (aux) {
        ret = LoadU32(ctx, node);
        if (ret) {
          goto trap_1;
        }
        slot = node;
        goto fold_offset_1;
      }
      if (ret >= scratch_e) {
        goto fold_offset_1;
      }
      slot = node + 4u;
      ret = LoadU32(ctx, node + 4u);
      aux = static_cast<uint32_t>(ret == 0u);
      if (aux) {
        goto fold_offset_1;
      }
      node = slot;
  trap_1:;
      slot = node;
      node = ret;
      goto step_71;
      TrapUnreachable();
    }
    node = slot;
  // Merge point fold_offset_1.
  fold_offset_1:;
    ret = static_cast<uint32_t>(LoadU32(ctx, slot) == 0u);
  }
  if (ret || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterFoldLayoutOffset130) {
      spill = HostFoldLayoutOffset(ctx, 1032u, 20u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterFoldLayoutOffset130;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      ret = spill;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      spill = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      aux = static_cast<uint32_t>(spill == 1u);
      if (aux) {
        goto release_24;
      }
      StoreU32(ctx, ret + 8u, node);
      StoreU64(ctx, ret, 0ull);
      StoreU32(ctx, ret + 16u, scratch_e);
      StoreU32(ctx, slot, ret);
      node = LoadU32(ctx, LoadU32(ctx, scratch_a + 28u));
      if (node) {
        StoreU32(ctx, scratch_a + 28u, node);
        ret = LoadU32(ctx, slot);
      }
      UnlinkListNode(ctx, LoadU32(ctx, scratch_a + 32u), ret);
      node = LoadU32(ctx, scratch_a + 36u) + 1u;
      StoreU32(ctx, scratch_a + 36u, node);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (static_cast<int32_t>(LoadS8(ctx, scratch_g + 11u)) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, scratch_g));
    }
    node = (static_cast<int32_t>(LoadS8(ctx, frame + 507u)) >= 0);
    if (node) {
      goto merge_32;
    }
    node = LoadU32(ctx, frame + 496u);
    DeallocateRawBlock(ctx, node);
    goto merge_32;
  }
  // Merge point merge_31.
  merge_31:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
    goto release_25;
  }
  // Merge point release_24.
  release_24:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, scratch_g + 11u)) >= 0);
    if (p0) {
      goto release_25;
    }
    p0 = LoadU32(ctx, scratch_g);
    DeallocateRawBlock(ctx, p0);
  }
  // Merge point release_25.
  release_25:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 507u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 496u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_72:;
  if (ResumePhase(ctx) == kResumeRunning) {
    scratch_a = LoadU32(ctx, node + 24u);
    slot = scratch_a + LoadU32(ctx, node + 20u);
    ret = LoadU32(ctx, node + 8u);
    scratch_g = LoadU32(ctx, node + 12u) - ret;
    aux = slot == scratch_g ? ((scratch_g << 8u) - 1u) : 0u;
  }
  if (aux || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      slot = node + 4u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterReleaseSlotPair131) {
      HostReleaseSlotPair(ctx, 2659u, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterReleaseSlotPair131;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (slot == 1u) {
        goto merge_44;
      }
      ret = LoadU32(ctx, node + 8u);
      scratch_a = LoadU32(ctx, node + 24u);
      slot = scratch_a + LoadU32(ctx, node + 20u);
    }
  else {
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, (LoadU32(ctx, ((slot >> 8u) & 16777212u) + ret) + (slot & 1023u) << 2u), word);
    StoreU32(ctx, node + 24u, scratch_a + 1u);
    StoreU8(ctx, frame + 26u, 0u);
    StoreU16(ctx, frame + 24u, 25705u);
    StoreU8(ctx, frame + 35u, 2u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 480u, frame + 24u);
    aux = frame + 496u;
    ret = frame + 24u;
    slot = frame + 480u;
    node = frame + 472u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii132) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, ret, 745008u, slot, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii132;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node != 1u);
  }
  if (node || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      ret = frame + 496u;
      slot = p0 + 13708u;
      node = LoadU32(ctx, frame + 496u) + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterFailAssertion133) {
      HostFailAssertion(ctx, 2388u, ret, slot, node, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterFailAssertion133;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      node = static_cast<uint32_t>(node == 1u);
      if (node) {
        goto release_26;
      }
      node = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
      if (node) {
        goto merge_32;
      }
      node = LoadU32(ctx, frame + 24u);
      DeallocateRawBlock(ctx, node);
      goto merge_32;
    }
  release_26:;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 24u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  // Merge point merge_32.
  merge_32:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1668184435u);
    StoreU8(ctx, frame + 35u, 4u);
    node = LoadU32(ctx, p1 + 20u);
    slot = static_cast<uint32_t>(node == 0u);
  }
  if (slot ? (ResumePhase(ctx) == kResumeRunning) : 0u) {
    goto os_probe_1;
  }
  step_73:;
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU8(ctx, node + 27u);
    scratch_g = static_cast<uint32_t>(SignExtend8(slot) < 0);
    aux = LoadU32(ctx, node + 20u);
    slot = scratch_g ? aux : slot;
    scratch_a = static_cast<uint32_t>(slot < 4u);
    ret = scratch_a ? slot : 4u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (ret) {
      scratch_e = node + 16u;
      scratch_g = scratch_g ? LoadU32(ctx, scratch_e) : scratch_e;
      scratch_e = CompareBytes(ctx, frame + 24u, scratch_g, ret);
      aux = static_cast<uint32_t>(scratch_e == 0u);
      if (aux) {
        slot = static_cast<uint32_t>(slot <= 4u);
        if (slot) {
          goto compare_10;
        }
        goto merge_35;
      }
      slot = (static_cast<int32_t>(scratch_e) < 0);
      if (slot) {
        goto merge_35;
      }
  compare_10:;
      slot = CompareBytes(ctx, scratch_g, frame + 24u, ret);
      ret = static_cast<uint32_t>(slot == 0u);
      if (ret) {
        goto step_74;
      }
      slot = (static_cast<int32_t>(slot) >= 0);
      if (slot) {
        goto step_75;
      }
      goto merge_34;
    }
    slot = static_cast<uint32_t>(slot > 4u);
    if (slot) {
      goto merge_35;
    }
  step_74:;
    if (scratch_a) {
      goto merge_34;
    }
  step_75:;
    StoreU8(ctx, frame + 500u, 0u);
    StoreU32(ctx, frame + 496u, 1668184435u);
    StoreU8(ctx, frame + 507u, 4u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 472u, frame + 496u);
    aux = frame + 480u;
    ret = frame + 496u;
    slot = frame + 472u;
    node = frame + 464u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii134) {
    HostInvokeViiiiii(ctx, 1959u, aux, scratch_b, ret, 745008u, slot, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii134;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = 1u;
    node = static_cast<uint32_t>(node == 1u);
    if (node) {
      goto release_28;
    }
    node = LoadU32(ctx, frame + 480u);
    scratch_d = LoadU32(ctx, node + 32u);
    ret = LoadU8(ctx, node + 39u);
    aux = static_cast<uint32_t>(SignExtend8(ret) < 0);
    ret = aux ? scratch_d : ret == 1u;
  }
  if (ret || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      node = node + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeIiiiii135) {
      spill = HostInvokeIiiiii(ctx, 1354u, node, 0u, 4294967295u, 740137u, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeIiiiii135;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      node = spill;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (slot == 1u) {
        goto rethrow_1;
      }
      slot = static_cast<uint32_t>(node != 0u);
    }
  else {
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (static_cast<int32_t>(LoadS8(ctx, frame + 507u)) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 496u));
    }
    node = static_cast<uint32_t>(slot == 0u);
    if (node) {
      goto os_probe_1;
    }
    StoreU32(ctx, kDataD015C, 0u);
    StoreU32(ctx, frame + 344u, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterFoldLayoutOffset136) {
    spill = HostFoldLayoutOffset(ctx, 1032u, 16u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterFoldLayoutOffset136;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    node = spill;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (slot == 1u) {
      goto merge_44;
    }
    StoreU32(ctx, node + 12u, 0u);
    StoreU32(ctx, node + 8u, p0 + 2748u);
    StoreU64(ctx, node, 2660ull);
    StoreU32(ctx, frame + 344u, 752856u);
    StoreU32(ctx, frame + 348u, node);
    StoreU32(ctx, kDataD015C, 0u);
    slot = word + 88u;
    node = frame + 344u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterOsQuery137) {
    spill = HostOsQuery(ctx, 2661u, slot, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterOsQuery137;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    node = spill;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = (frame + 344u) | 4u;
    slot = static_cast<uint32_t>(slot == 1u);
    if (slot) {
      goto step_76;
    }
    slot = LoadU32(ctx, frame + 344u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    ret = (ResumePhase(ctx) == kResumeRunning) ? (slot & 1u) : ret;
    if (ResumePhase(ctx) == kResumeRunning) {
      if (ret) {
        goto merge_33;
      }
      slot = LoadU32(ctx, slot & 4294967294u);
      ret = static_cast<uint32_t>(slot == 0u);
      if (ret) {
        goto merge_33;
      }
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterSyncSlot138) {
      HostSyncSlot(ctx, slot, node, node, 2u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterSyncSlot138;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      node = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      node = static_cast<uint32_t>(node == 1u);
      if (node) {
        goto rethrow_1;
      }
    }
  // Merge point merge_33.
  merge_33:;
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, frame + 344u, 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, p0 + 2876u);
    slot = static_cast<uint32_t>(node == 13u);
    if (slot) {
      goto merge_39;
    }
    StoreU32(ctx, p0 + 2876u, 4u);
    slot = p0 + 2880u;
    StoreU32(ctx, slot, node);
    node = LoadU32(ctx, p0 + 8u);
    if (node) {
      goto os_wait_1;
    }
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterFailAssertion139) {
    HostFailAssertion(ctx, kAssertWord, kAssertExpression, kAssertFile, kAssertLine,
                      kAssertFunction);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterFailAssertion139;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p0 = static_cast<uint32_t>(p0 != 1u);
    if (p0) {
      goto trap_2;
    }
    goto rethrow_1;
  }
  // Merge point merge_34.
  merge_34:;
  node = (ResumePhase(ctx) == kResumeRunning) ? (node + 4u) : node;
  // Merge point merge_35.
  merge_35:;
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, node);
    if (node) {
      goto step_73;
    }
  }
  // Merge point os_probe_1.
  os_probe_1:;
  if (ResumePhase(ctx) == kResumeRunning) {
    node = static_cast<uint32_t>(flag == 0u);
    if (node) {
      goto step_80;
    }
    StoreU32(ctx, kDataD015C, 0u);
    node = frame + 216u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterOsQuery140) {
    spill = HostOsQuery(ctx, 2591u, node, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterOsQuery140;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    node = spill;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (slot == 1u) {
      goto merge_44;
    }
    StoreU32(ctx, frame + 212u, 0u);
    StoreU32(ctx, frame + 208u, 2538u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU64(ctx, frame + 8u, LoadU64(ctx, frame + 208u));
    ret = frame + 264u;
    slot = frame + 8u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInitResources141) {
    HostInitResources(ctx, 2662u, ret, slot, p0, node, p2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInitResources141;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot == 1u);
    if (slot) {
      goto step_78;
    }
    StoreU32(ctx, kDataD015C, 0u);
    ret = frame + 328u;
    slot = frame + 264u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterDispatchInvoke142) {
    spill = HostDispatchInvoke(ctx, 2663u, ret, slot, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterDispatchInvoke142;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    slot = spill;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    ret = static_cast<uint32_t>(ret == 1u);
    if (ret) {
      goto release_31;
    }
    StoreU32(ctx, kDataD015C, 0u);
    ret = word + 88u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterOsQuery143) {
    spill = HostOsQuery(ctx, 2661u, ret, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterOsQuery143;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    ret = spill;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    ret = static_cast<uint32_t>(ret == 1u);
    if (ret) {
      goto step_79;
    }
    ret = LoadU32(ctx, slot);
  }
  if (ret || ResumePhase(ctx) == kResumeRewinding) {
    flag = (ResumePhase(ctx) == kResumeRunning) ? (ret & 1u) : flag;
    if (ResumePhase(ctx) == kResumeRunning) {
      if (flag) {
        goto release_27;
      }
      ret = LoadU32(ctx, ret & 4294967294u);
      flag = static_cast<uint32_t>(ret == 0u);
      if (flag) {
        goto release_27;
      }
      StoreU32(ctx, kDataD015C, 0u);
      flag = ret;
      ret = slot + 4u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterSyncSlot144) {
      HostSyncSlot(ctx, flag, ret, ret, 2u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterSyncSlot144;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      ret = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      ret = static_cast<uint32_t>(ret == 1u);
      if (ret) {
        goto rethrow_1;
      }
    }
  // Merge point release_27.
  release_27:;
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, slot, 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, frame + 276u, 745280u);
    if (static_cast<int32_t>(LoadS8(ctx, frame + 315u)) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 304u));
    }
    ReleaseNodeTree(ctx, frame + 292u, LoadU32(ctx, frame + 296u));
    if (static_cast<int32_t>(LoadS8(ctx, frame + 291u)) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 280u));
    }
    StoreU32(ctx, node, 745280u);
    if (static_cast<int32_t>(LoadS8(ctx, node + 39u)) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, node + 28u));
    }
    ret = node + 16u;
    ReleaseNodeTree(ctx, ret, LoadU32(ctx, node + 20u));
    flag = 1u;
    slot = (static_cast<int32_t>(LoadS8(ctx, node + 15u)) >= 0);
    if (slot) {
      goto merge_39;
    }
    node = LoadU32(ctx, node + 4u);
    DeallocateRawBlock(ctx, node);
    goto merge_39;
  }
  os_wait_1:;
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, LoadU32(ctx, node) + 28u);
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterOsSchedule145) {
    HostOsSchedule(ctx, slot, node, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterOsSchedule145;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    node = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    node = static_cast<uint32_t>(node != 1u);
    if (node) {
      goto merge_39;
    }
    goto merge_44;
  }
  release_28:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 507u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 496u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_76:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = LoadU32(ctx, frame + 344u);
    if (p0 == 0u) {
      goto merge_45;
    }
    p1 = p0 & 1u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (p1) {
      goto merge_36;
    }
    p0 = LoadU32(ctx, p0 & 4294967294u);
    p1 = static_cast<uint32_t>(p0 == 0u);
    if (p1) {
      goto merge_36;
    }
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterSyncSlot146) {
    HostSyncSlot(ctx, p0, node, node, 2u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterSyncSlot146;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p0 = static_cast<uint32_t>(p0 == 1u);
    if (p0) {
      goto step_77;
    }
  }
  // Merge point merge_36.
  merge_36:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, frame + 344u, 0u);
    goto merge_45;
  }
  step_77:;
  if (ResumePhase(ctx) == kResumeRunning) {
    goto rethrow_1;
  }
  step_78:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
    goto release_33;
  }
  step_79:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = LoadU32(ctx, slot);
    p1 = static_cast<uint32_t>(p0 == 0u);
    if (p1) {
      goto release_32;
    }
    p1 = p0 & 1u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (p1) {
      goto release_29;
    }
    p0 = LoadU32(ctx, p0 & 4294967294u);
    p1 = static_cast<uint32_t>(p0 == 0u);
    if (p1) {
      goto release_29;
    }
    StoreU32(ctx, kDataD015C, 0u);
    p1 = p0;
    p0 = slot + 4u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterSyncSlot147) {
    HostSyncSlot(ctx, p1, p0, p0, 2u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterSyncSlot147;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p0 = static_cast<uint32_t>(p0 == 1u);
    if (p0) {
      goto release_30;
    }
  }
  // Merge point release_29.
  release_29:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, slot, 0u);
    goto release_32;
  }
  release_30:;
  if (ResumePhase(ctx) == kResumeRunning) {
    goto rethrow_1;
  }
  release_31:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
  }
  // Merge point release_32.
  release_32:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, frame + 276u, 745280u);
    if (static_cast<int32_t>(LoadS8(ctx, frame + 315u)) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 304u));
    }
    p1 = frame + 292u;
    ReleaseNodeTree(ctx, p1, LoadU32(ctx, frame + 296u));
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 291u)) >= 0);
    if (p0) {
      goto release_33;
    }
    p0 = LoadU32(ctx, frame + 280u);
    DeallocateRawBlock(ctx, p0);
  }
  // Merge point release_33.
  release_33:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, node, 745280u);
    if (static_cast<int32_t>(LoadS8(ctx, node + 39u)) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, node + 28u));
    }
    ReleaseNodeTree(ctx, node + 16u, LoadU32(ctx, node + 20u));
    p0 = (static_cast<int32_t>(LoadS8(ctx, node + 15u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, node + 4u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_80:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU8(ctx, frame + 28u, 0u);
    StoreU32(ctx, frame + 24u, 1701603686u);
    StoreU8(ctx, frame + 35u, 4u);
    slot = LoadU32(ctx, p1 + 20u);
    if (slot) {
      node = slot;
  step_81:;
      ret = LoadU8(ctx, node + 27u);
      scratch_a = static_cast<uint32_t>(SignExtend8(ret) < 0);
      ret = scratch_a ? LoadU32(ctx, node + 20u) : ret;
      flag = static_cast<uint32_t>(ret < 4u);
      scratch_g = flag ? ret : 4u;
      if (scratch_g) {
        scratch_e = node + 16u;
        scratch_a = scratch_a ? LoadU32(ctx, scratch_e) : scratch_e;
        scratch_e = CompareBytes(ctx, frame + 24u, scratch_a, scratch_g);
        if (scratch_e == 0u) {
          ret = static_cast<uint32_t>(ret <= 4u);
          if (ret) {
            goto compare_11;
          }
          goto merge_37;
        }
        ret = (static_cast<int32_t>(scratch_e) < 0);
        if (ret) {
          goto merge_37;
        }
  compare_11:;
        ret = CompareBytes(ctx, scratch_a, frame + 24u, scratch_g);
        if (ret == 0u) {
          goto step_82;
        }
        ret = (static_cast<int32_t>(ret) >= 0);
        if (ret) {
          goto os_probe_4;
        }
        goto step_83;
      }
      ret = static_cast<uint32_t>(ret > 4u);
      if (ret) {
        goto merge_37;
      }
  step_82:;
      ret = static_cast<uint32_t>(flag == 0u);
      if (ret) {
        goto os_probe_4;
      }
  step_83:;
      node = node + 4u;
  // Merge point merge_37.
  merge_37:;
      node = LoadU32(ctx, node);
      if (node) {
        goto step_81;
      }
    }
    StoreU8(ctx, frame + 507u, 5u);
    StoreU8(ctx, frame + 501u, 0u);
    StoreU32(ctx, frame + 496u, LoadU32(ctx, 740123u));  // "label"
    StoreU8(ctx, frame + 500u, LoadU8(ctx, 740127u));
    if (slot) {
      node = slot;
  step_84:;
      ret = LoadU8(ctx, node + 27u);
      scratch_a = static_cast<uint32_t>(SignExtend8(ret) < 0);
      ret = scratch_a ? LoadU32(ctx, node + 20u) : ret;
      flag = static_cast<uint32_t>(ret < 5u);
      scratch_g = flag ? ret : 5u;
      if (scratch_g) {
        scratch_e = node + 16u;
        scratch_a = scratch_a ? LoadU32(ctx, scratch_e) : scratch_e;
        scratch_e = CompareBytes(ctx, frame + 496u, scratch_a, scratch_g);
        if (scratch_e == 0u) {
          ret = static_cast<uint32_t>(ret <= 5u);
          if (ret) {
            goto compare_12;
          }
          goto merge_38;
        }
        ret = (static_cast<int32_t>(scratch_e) < 0);
        if (ret) {
          goto merge_38;
        }
  compare_12:;
        ret = CompareBytes(ctx, scratch_a, frame + 496u, scratch_g);
        if (ret == 0u) {
          goto step_85;
        }
        ret = (static_cast<int32_t>(ret) >= 0);
        if (ret) {
          goto os_probe_4;
        }
        goto step_86;
      }
      ret = static_cast<uint32_t>(ret > 5u);
      if (ret) {
        goto merge_38;
      }
  step_85:;
      ret = static_cast<uint32_t>(flag == 0u);
      if (ret) {
        goto os_probe_4;
      }
  step_86:;
      node = node + 4u;
  // Merge point merge_38.
  merge_38:;
      node = LoadU32(ctx, node);
      if (node) {
        goto step_84;
      }
    }
    StoreU8(ctx, frame + 491u, 7u);
    flag = 0u;
    StoreU8(ctx, frame + 487u, 0u);
    StoreU32(ctx, frame + 480u, LoadU32(ctx, 740129u));  // "handler"
    StoreU32(ctx, frame + 483u, LoadU32(ctx, 740132u));  // "dler"
    node = static_cast<uint32_t>(slot == 0u);
    if (node) {
      goto merge_39;
    }
  step_87:;
    node = LoadU8(ctx, slot + 27u);
    scratch_g = static_cast<uint32_t>(SignExtend8(node) < 0);
    node = scratch_g ? LoadU32(ctx, slot + 20u) : node;
    scratch_a = static_cast<uint32_t>(node < 7u);
    ret = scratch_a ? node : 7u;
    if (ret) {
      scratch_e = slot + 16u;
      scratch_g = scratch_g ? LoadU32(ctx, scratch_e) : scratch_e;
      scratch_e = CompareBytes(ctx, frame + 480u, scratch_g, ret);
      if (scratch_e == 0u) {
        node = static_cast<uint32_t>(node <= 7u);
        if (node) {
          goto compare_13;
        }
        goto os_probe_3;
      }
      node = (static_cast<int32_t>(scratch_e) < 0);
      if (node) {
        goto os_probe_3;
      }
  compare_13:;
      node = CompareBytes(ctx, scratch_g, frame + 480u, ret);
      ret = static_cast<uint32_t>(node == 0u);
      if (ret) {
        goto step_88;
      }
      node = (static_cast<int32_t>(node) >= 0);
      if (node) {
        goto os_probe_4;
      }
      goto os_probe_2;
    }
    node = static_cast<uint32_t>(node > 7u);
    if (node) {
      goto os_probe_3;
    }
  step_88:;
    node = static_cast<uint32_t>(scratch_a == 0u);
    if (node) {
      goto os_probe_4;
    }
  os_probe_2:;
    slot = slot + 4u;
  // Merge point os_probe_3.
  os_probe_3:;
    slot = LoadU32(ctx, slot);
    if (slot) {
      goto step_87;
    }
    goto merge_39;
  // Merge point os_probe_4.
  os_probe_4:;
    StoreU32(ctx, kDataD015C, 0u);
    node = frame + 80u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterOsQuery148) {
    spill = HostOsQuery(ctx, 2591u, node, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterOsQuery148;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    node = spill;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    if (slot == 1u) {
      goto merge_44;
    }
    StoreU32(ctx, frame + 76u, 0u);
    StoreU32(ctx, frame + 72u, 2648u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU64(ctx, frame + 16u, LoadU64(ctx, frame + 72u));
    ret = frame + 128u;
    slot = frame + 16u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii149) {
    HostInvokeViiiiii(ctx, 2649u, ret, slot, p0, 0u, 1u, node);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterInvokeViiiiii149;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    slot = static_cast<uint32_t>(slot == 1u);
    if (slot) {
      goto step_89;
    }
    StoreU32(ctx, kDataD015C, 0u);
    ret = frame + 192u;
    slot = frame + 128u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterDispatchInvoke150) {
    spill = HostDispatchInvoke(ctx, 2650u, ret, slot, 0u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterDispatchInvoke150;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    slot = spill;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    ret = static_cast<uint32_t>(ret == 1u);
    if (ret) {
      goto release_37;
    }
    StoreU32(ctx, kDataD015C, 0u);
    ret = word + 88u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterOsQuery151) {
    spill = HostOsQuery(ctx, 2661u, ret, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterOsQuery151;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    ret = spill;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    ret = static_cast<uint32_t>(ret == 1u);
    if (ret) {
      goto step_90;
    }
    ret = LoadU32(ctx, slot);
  }
  if (ret || ResumePhase(ctx) == kResumeRewinding) {
    flag = (ResumePhase(ctx) == kResumeRunning) ? (ret & 1u) : flag;
    if (ResumePhase(ctx) == kResumeRunning) {
      if (flag) {
        goto release_34;
      }
      ret = LoadU32(ctx, ret & 4294967294u);
      flag = static_cast<uint32_t>(ret == 0u);
      if (flag) {
        goto release_34;
      }
      StoreU32(ctx, kDataD015C, 0u);
      flag = ret;
      ret = slot + 4u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterSyncSlot152) {
      HostSyncSlot(ctx, flag, ret, ret, 2u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterSyncSlot152;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      ret = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      ret = static_cast<uint32_t>(ret == 1u);
      if (ret) {
        goto rethrow_1;
      }
    }
  // Merge point release_34.
  release_34:;
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, slot, 0u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, frame + 148u, 745280u);
    if (static_cast<int32_t>(LoadS8(ctx, frame + 187u)) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 176u));
    }
    ReleaseNodeTree(ctx, frame + 164u, LoadU32(ctx, frame + 168u));
    if (static_cast<int32_t>(LoadS8(ctx, frame + 163u)) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 152u));
    }
    StoreU32(ctx, node, 745280u);
    if (static_cast<int32_t>(LoadS8(ctx, node + 39u)) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, node + 28u));
    }
    ret = node + 16u;
    ReleaseNodeTree(ctx, ret, LoadU32(ctx, node + 20u));
    flag = 0u;
    slot = (static_cast<int32_t>(LoadS8(ctx, node + 15u)) >= 0);
    if (slot) {
      goto merge_39;
    }
    node = LoadU32(ctx, node + 4u);
    DeallocateRawBlock(ctx, node);
  }
  // Merge point merge_39.
  merge_39:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p2 = static_cast<uint32_t>(p2 == 0u);
    if (p2) {
      goto assert_fail_3;
    }
    goto merge_40;
  }
  step_89:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
    goto release_39;
  }
  step_90:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = LoadU32(ctx, slot);
    p1 = static_cast<uint32_t>(p0 == 0u);
    if (p1) {
      goto release_38;
    }
    p1 = p0 & 1u;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (p1) {
      goto release_35;
    }
    p0 = LoadU32(ctx, p0 & 4294967294u);
    p1 = static_cast<uint32_t>(p0 == 0u);
    if (p1) {
      goto release_35;
    }
    StoreU32(ctx, kDataD015C, 0u);
    p1 = p0;
    p0 = slot + 4u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterSyncSlot153) {
    HostSyncSlot(ctx, p1, p0, p0, 2u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterSyncSlot153;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p0 = static_cast<uint32_t>(p0 == 1u);
    if (p0) {
      goto release_36;
    }
  }
  // Merge point release_35.
  release_35:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, slot, 0u);
    goto release_38;
  }
  release_36:;
  if (ResumePhase(ctx) == kResumeRunning) {
    goto rethrow_1;
  }
  release_37:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
  }
  // Merge point release_38.
  release_38:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, frame + 148u, 745280u);
    if (static_cast<int32_t>(LoadS8(ctx, frame + 187u)) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 176u));
    }
    p1 = frame + 164u;
    ReleaseNodeTree(ctx, p1, LoadU32(ctx, frame + 168u));
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 163u)) >= 0);
    if (p0) {
      goto release_39;
    }
    p0 = LoadU32(ctx, frame + 152u);
    DeallocateRawBlock(ctx, p0);
  }
  // Merge point release_39.
  release_39:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, node, 745280u);
    if (static_cast<int32_t>(LoadS8(ctx, node + 39u)) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, node + 28u));
    }
    ReleaseNodeTree(ctx, node + 16u, LoadU32(ctx, node + 20u));
    p0 = (static_cast<int32_t>(LoadS8(ctx, node + 15u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, node + 4u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  assert_fail_3:;
  if (ResumePhase(ctx) == kResumeRunning) {
    node = static_cast<uint32_t>(scratch_f == 4294967295u);
    p2 = static_cast<uint32_t>(flag | node | result == 4294967295u);
  }
  if (p2 || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, p0 + 128u);
      p2 = static_cast<uint32_t>(p0 == 0u);
    }
    if (p2 || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterFailAssertion154) {
        HostFailAssertion(ctx, kAssertWord, kAssertExpression, kAssertFile, kAssertLine,
                          kAssertFunction);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterFailAssertion154;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0) {
          goto trap_2;
        }
        goto rethrow_1;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU8(ctx, frame + 26u, 0u);
      StoreU16(ctx, frame + 24u, 25705u);
      StoreU8(ctx, frame + 35u, 2u);
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, frame + 480u, frame + 24u);
      ret = frame + 496u;
      slot = frame + 24u;
      node = frame + 480u;
      p2 = frame + 472u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii155) {
      HostInvokeViiiiii(ctx, 1959u, ret, scratch_b, slot, 745008u, node, p2);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeViiiiii155;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p2 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p2 = static_cast<uint32_t>(p2 == 1u);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (p2) {
        goto release_41;
      }
      p2 = LoadU32(ctx, frame + 496u) + 28u;
      StoreU32(ctx, frame + 480u, p2);
      StoreU32(ctx, kDataD015C, 0u);
      ret = frame + 496u;
      slot = p0 + 76u;
      node = frame + 480u;
      p0 = frame + 472u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii156) {
      HostInvokeViiiiii(ctx, 2613u, ret, slot, p2, 745008u, node, p0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeViiiiii156;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 == 1u);
      if (p0) {
        goto release_41;
      }
      p0 = LoadU32(ctx, frame + 496u);
      p2 = LoadU32(ctx, p0 + 32u);
      node = static_cast<uint32_t>(p2 != LoadU32(ctx, p0 + 36u));
    }
    if (node || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterOsQuery157) {
        spill = HostOsQuery(ctx, 2591u, p2, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterOsQuery157;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        p1 = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        node = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        node = static_cast<uint32_t>(node == 1u);
        if (node) {
          ret = HostRet0(ctx);
          p1 = HostTempRet0(ctx);
          StoreU32(ctx, p0 + 32u, p2);
          goto release_42;
        }
        p1 = p1 + 44u;
        StoreU32(ctx, p0 + 32u, p1);
        goto release_40;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, kDataD015C, 0u);
      p0 = p0 + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterOsSchedule158) {
      HostOsSchedule(ctx, 2614u, p0, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterOsSchedule158;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 == 1u);
      if (p0) {
        goto release_41;
      }
    }
  release_40:;
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
      if (p0) {
        goto merge_40;
      }
      p0 = LoadU32(ctx, frame + 24u);
      DeallocateRawBlock(ctx, p0);
      goto merge_40;
    }
  // Merge point release_41.
  release_41:;
    if (ResumePhase(ctx) == kResumeRunning) {
      ret = HostRet0(ctx);
      p0 = HostTempRet0(ctx);
    }
  release_42:;
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 35u)) >= 0);
      if (p0) {
        goto merge_45;
      }
      p0 = LoadU32(ctx, frame + 24u);
      DeallocateRawBlock(ctx, p0);
      goto merge_45;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, frame + 24u, 745280u);
    StoreU32(ctx, kDataD015C, 0u);
    StoreU8(ctx, frame + 507u, 6u);
    StoreU32(ctx, frame + 496u, LoadU32(ctx, 718417u));  // "lyprop"
    StoreU16(ctx, frame + 500u, LoadU16(ctx, 718421u));  // "op"
    StoreU8(ctx, frame + 502u, 0u);
    p2 = (frame + 24u) | 4u;
    p1 = frame + 496u;
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterOsQuery159) {
    spill = HostOsQuery(ctx, 873u, p2, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterOsQuery159;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    p2 = spill;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p1 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p1 = static_cast<uint32_t>(p1 != 1u);
  }
  if (p1 || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = frame + 44u;
      StoreU64(ctx, p1, 0ull);
      StoreU64(ctx, frame + 60u, 0ull);
      StoreU64(ctx, frame + 52u, 0ull);
      StoreU32(ctx, frame + 40u, p1);
      if (static_cast<int32_t>(LoadS8(ctx, frame + 507u)) < 0) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 496u));
      }
      StoreU8(ctx, frame + 498u, 0u);
      StoreU16(ctx, frame + 496u, 25705u);
      StoreU8(ctx, frame + 507u, 2u);
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, frame + 472u, frame + 496u);
      ret = frame + 480u;
      slot = frame + 496u;
      node = frame + 472u;
      p1 = frame + 464u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii160) {
      HostInvokeViiiiii(ctx, 1959u, ret, scratch_b, slot, 745008u, node, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeViiiiii160;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p1 == 1u) {
        goto step_91;
      }
      p1 = LoadU32(ctx, frame + 480u);
      StoreU8(ctx, frame + 482u, 0u);
      StoreU16(ctx, frame + 480u, 25705u);
      StoreU8(ctx, frame + 491u, 2u);
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, frame + 464u, frame + 480u);
      word = frame + 472u;
      flag = frame + 480u;
      ret = frame + 464u;
      slot = frame + 456u;
      node = frame + 40u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii161) {
      HostInvokeViiiiii(ctx, 1959u, word, node, flag, 745008u, ret, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeViiiiii161;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (slot == 1u) {
        goto release_43;
      }
      StoreU32(ctx, kDataD015C, 0u);
      slot = LoadU32(ctx, frame + 472u) + 28u;
      p1 = p1 + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterOsQuery162) {
      spill = HostOsQuery(ctx, 870u, slot, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterOsQuery162;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      p1 = spill;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p1 == 1u) {
        goto release_43;
      }
      if (static_cast<int32_t>(LoadS8(ctx, frame + 491u)) < 0) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 480u));
      }
      if (static_cast<int32_t>(LoadS8(ctx, frame + 507u)) < 0) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 496u));
      }
      ret = scratch_f ? 741949u : (result & 1u) ? 742278u : 741949u;  // "to"
      p1 = StringLength(ctx, ret);
      slot = static_cast<uint32_t>(p1 >= 4294967280u);
    }
    if (slot || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        p0 = frame + 496u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterReleaseSlotPair163) {
        HostReleaseSlotPair(ctx, 1286u, p0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterReleaseSlotPair163;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0) {
          goto trap_2;
        }
        goto release_47;
      }
    }
    slot = (ResumePhase(ctx) == kResumeRunning) ? p1 >= 11u : slot;
    if (slot || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
        slot = p1 + 16u;
        word = slot & 4294967280u;
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterFoldLayoutOffset164) {
        spill = HostFoldLayoutOffset(ctx, 1032u, word);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterFoldLayoutOffset164;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
        slot = spill;
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        result = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        if (result == 1u) {
          goto release_47;
        }
        StoreU32(ctx, frame + 496u, slot);
        StoreU32(ctx, frame + 500u, p1);
        flag = word | 2147483648u;
        StoreU32(ctx, frame + 504u, flag);
        goto copy_1;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU8(ctx, frame + 507u, p1);
      slot = frame + 496u;
      flag = static_cast<uint32_t>(p1 == 0u);
      if (flag) {
        goto apply_1;
      }
    }
  copy_1:;
    if (ResumePhase(ctx) == kResumeRunning) {
      ret = CopyBytes(ctx, slot, ret, p1);
    }
  apply_1:;
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU8(ctx, p1 + slot, 0u);
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, frame + 472u, frame + 496u);
      flag = frame + 480u;
      ret = frame + 496u;
      slot = frame + 472u;
      p1 = frame + 464u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii165) {
      HostInvokeViiiiii(ctx, 1959u, flag, scratch_b, ret, 745008u, slot, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeViiiiii165;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p1 == 1u) {
        goto step_92;
      }
      p1 = LoadU32(ctx, frame + 480u);
      StoreU8(ctx, frame + 491u, 5u);
      StoreU32(ctx, frame + 480u, LoadU32(ctx, 740257u));  // "param"
      StoreU8(ctx, frame + 484u, LoadU8(ctx, 740261u));
      StoreU8(ctx, frame + 485u, 0u);
      StoreU32(ctx, kDataD015C, 0u);
      StoreU32(ctx, frame + 464u, frame + 480u);
      word = frame + 472u;
      flag = frame + 480u;
      ret = frame + 464u;
      slot = frame + 456u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii166) {
      HostInvokeViiiiii(ctx, 1959u, word, scratch_b, flag, 745008u, ret, slot);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeViiiiii166;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (slot == 1u) {
        goto release_45;
      }
      slot = LoadU32(ctx, frame + 472u) + 28u;
      StoreU32(ctx, frame + 464u, slot);
      StoreU32(ctx, kDataD015C, 0u);
      word = frame + 472u;
      flag = frame + 464u;
      ret = frame + 456u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterInvokeViiiiii167) {
      HostInvokeViiiiii(ctx, 1640u, word, node, slot, 745008u, flag, ret);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterInvokeViiiiii167;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (slot == 1u) {
        goto release_45;
      }
      StoreU32(ctx, kDataD015C, 0u);
      slot = LoadU32(ctx, frame + 472u) + 28u;
      p1 = p1 + 28u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterOsQuery168) {
      spill = HostOsQuery(ctx, 870u, slot, p1);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterOsQuery168;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      p1 = spill;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p1 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p1 == 1u) {
        goto release_45;
      }
      if (static_cast<int32_t>(LoadS8(ctx, frame + 491u)) < 0) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 480u));
      }
      if (static_cast<int32_t>(LoadS8(ctx, frame + 507u)) < 0) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 496u));
      }
      p0 = LoadU32(ctx, p0 + 128u);
      p1 = static_cast<uint32_t>(p0 == 0u);
    }
    if (p1 || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning) {
        StoreU32(ctx, kDataD015C, 0u);
      }
      if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterFailAssertion169) {
        HostFailAssertion(ctx, kAssertWord, kAssertExpression, kAssertFile, kAssertLine,
                          kAssertFunction);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
        SyncMemoryBase(ctx);
#endif
        resume_code = kAfterFailAssertion169;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto suspend_point;
        }
      }
      if (ResumePhase(ctx) == kResumeRunning) {
        p0 = LoadU32(ctx, kDataD015C);
        StoreU32(ctx, kDataD015C, 0u);
        p0 = static_cast<uint32_t>(p0 != 1u);
        if (p0) {
          goto trap_2;
        }
        goto rethrow_1;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU64(ctx, frame + 484u, 0ull);
      StoreU32(ctx, kDataD015C, 0u);
      StoreU64(ctx, frame + 500u, 0ull);
      StoreU32(ctx, frame + 496u, (frame + 496u) | 4u);
      StoreU32(ctx, frame + 480u, (frame + 480u) | 4u);
      slot = frame + 24u;
      p1 = frame + 480u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterFailAssertion170) {
      HostFailAssertion(ctx, 2645u, p0, slot, p1, 1u);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterFailAssertion170;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      if (p0 == 1u) {
        ret = HostRet0(ctx);
        HostTempRet0(ctx);
        ReleaseLayerNodeTree(ctx, frame + 480u, LoadU32(ctx, frame + 484u));
        p0 = LoadU32(ctx, frame + 500u);
        ReleaseLayerNodeTree(ctx, frame + 496u, p0);
        goto release_48;
      }
      ReleaseLayerNodeTree(ctx, frame + 480u, LoadU32(ctx, frame + 484u));
      ReleaseLayerNodeTree(ctx, frame + 496u, LoadU32(ctx, frame + 500u));
      StoreU32(ctx, frame + 24u, 745280u);
      if (static_cast<int32_t>(LoadS8(ctx, frame + 63u)) < 0) {
        DeallocateRawBlock(ctx, LoadU32(ctx, frame + 52u));
      }
      ReleaseNodeTree(ctx, node, LoadU32(ctx, frame + 44u));
      p0 = (static_cast<int32_t>(LoadS8(ctx, p2 + 11u)) >= 0);
      if (p0) {
        goto merge_40;
      }
      p0 = LoadU32(ctx, p2);
      DeallocateRawBlock(ctx, p0);
      goto merge_40;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 507u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, frame + 496u);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  step_91:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
    goto release_44;
  }
  // Merge point release_43.
  release_43:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 491u)) >= 0);
    if (p0) {
      goto release_44;
    }
    p0 = LoadU32(ctx, frame + 480u);
    DeallocateRawBlock(ctx, p0);
  }
  // Merge point release_44.
  release_44:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 507u)) >= 0);
    if (p0) {
      goto release_48;
    }
    p0 = LoadU32(ctx, frame + 496u);
    DeallocateRawBlock(ctx, p0);
    goto release_48;
  }
  step_92:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
    goto release_46;
  }
  // Merge point release_45.
  release_45:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    HostTempRet0(ctx);
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 491u)) >= 0);
    if (p0) {
      goto release_46;
    }
    p0 = LoadU32(ctx, frame + 480u);
    DeallocateRawBlock(ctx, p0);
  }
  // Merge point release_46.
  release_46:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = (static_cast<int32_t>(LoadS8(ctx, frame + 507u)) >= 0);
    if (p0) {
      goto release_48;
    }
    p0 = LoadU32(ctx, frame + 496u);
    DeallocateRawBlock(ctx, p0);
    goto release_48;
  }
  // Merge point trap_2.
  trap_2:;
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  // Merge point release_47.
  release_47:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
  }
  // Merge point release_48.
  release_48:;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, frame + 24u, 745280u);
    if (static_cast<int32_t>(LoadS8(ctx, frame + 63u)) < 0) {
      DeallocateRawBlock(ctx, LoadU32(ctx, frame + 52u));
    }
    ReleaseNodeTree(ctx, frame + 40u, LoadU32(ctx, frame + 44u));
    p0 = (static_cast<int32_t>(LoadS8(ctx, p2 + 11u)) >= 0);
    if (p0) {
      goto merge_45;
    }
    p0 = LoadU32(ctx, p2);
    DeallocateRawBlock(ctx, p0);
    goto merge_45;
  }
  // Merge point merge_40.
  merge_40:;
  p0 = (ResumePhase(ctx) == kResumeRunning) ? (scratch_c == 0u) : p0;
  if (ResumePhase(ctx) == kResumeRunning) {
    if (p0) {
      goto merge_41;
    }
    p0 = LoadU32(ctx, scratch_c + 4u);
    StoreU32(ctx, scratch_c + 4u, p0 - 1u);
    p0 = static_cast<uint32_t>(p0 != 1u);
    if (p0) {
      goto merge_41;
    }
    p0 = LoadU32(ctx, LoadU32(ctx, scratch_c) + 8u);
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterReleaseSlotPair171) {
    HostReleaseSlotPair(ctx, p0, scratch_c);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterReleaseSlotPair171;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p0 = static_cast<uint32_t>(p0 != 1u);
  }
  if (p0 || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, scratch_c + 8u);
      StoreU32(ctx, scratch_c + 8u, p0 - 1u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0) {
        goto merge_41;
      }
      p0 = LoadU32(ctx, LoadU32(ctx, scratch_c) + 12u);
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterReleaseSlotPair172) {
      HostReleaseSlotPair(ctx, p0, scratch_c);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterReleaseSlotPair172;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0) {
        goto merge_41;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto rethrow_1;
  }
  // Merge point merge_41.
  merge_41:;
  p0 = (ResumePhase(ctx) == kResumeRunning) ? (handle == 0u) : p0;
  if (ResumePhase(ctx) == kResumeRunning) {
    if (p0) {
      goto merge_42;
    }
    p0 = LoadU32(ctx, handle + 4u);
    StoreU32(ctx, handle + 4u, p0 - 1u);
    p0 = static_cast<uint32_t>(p0 != 1u);
    if (p0) {
      goto merge_42;
    }
    p0 = LoadU32(ctx, LoadU32(ctx, handle) + 8u);
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterReleaseSlotPair173) {
    HostReleaseSlotPair(ctx, p0, handle);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterReleaseSlotPair173;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p0 = static_cast<uint32_t>(p0 == 1u);
    if (p0) {
      goto merge_43;
    }
    p0 = LoadU32(ctx, handle + 8u);
    StoreU32(ctx, handle + 8u, p0 - 1u);
    p0 = static_cast<uint32_t>(p0 != 1u);
    if (p0) {
      goto merge_42;
    }
    p0 = LoadU32(ctx, LoadU32(ctx, handle) + 12u);
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterReleaseSlotPair174) {
    HostReleaseSlotPair(ctx, p0, handle);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterReleaseSlotPair174;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p0 = static_cast<uint32_t>(p0 == 1u);
    if (p0) {
      goto merge_43;
    }
  }
  // Merge point merge_42.
  merge_42:;
  if (ResumePhase(ctx) == kResumeRunning) {
    GuestStackSp(ctx) = (frame + 512u);
    goto done;
  }
  // Merge point merge_43.
  merge_43:;
  if (ResumePhase(ctx) == kResumeRunning) {
    goto rethrow_1;
  }
  // Merge point merge_44.
  merge_44:;
  if (ResumePhase(ctx) == kResumeRunning) {
    ret = HostRet0(ctx);
    p0 = HostTempRet0(ctx);
  }
  // Merge point merge_45.
  merge_45:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = static_cast<uint32_t>(scratch_c == 0u);
    if (p0) {
      goto merge_46;
    }
    p0 = LoadU32(ctx, scratch_c + 4u);
    StoreU32(ctx, scratch_c + 4u, p0 - 1u);
    p0 = static_cast<uint32_t>(p0 != 1u);
    if (p0) {
      goto merge_46;
    }
    p0 = LoadU32(ctx, LoadU32(ctx, scratch_c) + 8u);
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterReleaseSlotPair175) {
    HostReleaseSlotPair(ctx, p0, scratch_c);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterReleaseSlotPair175;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p0 = static_cast<uint32_t>(p0 != 1u);
  }
  if (p0 || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, scratch_c + 8u);
      StoreU32(ctx, scratch_c + 8u, p0 - 1u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0) {
        goto merge_46;
      }
      p0 = LoadU32(ctx, LoadU32(ctx, scratch_c) + 12u);
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterReleaseSlotPair176) {
      HostReleaseSlotPair(ctx, p0, scratch_c);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterReleaseSlotPair176;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0) {
        goto merge_46;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto rethrow_1;
  }
  // Merge point merge_46.
  merge_46:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = static_cast<uint32_t>(handle == 0u);
    if (p0) {
      goto os_release_1;
    }
    p0 = LoadU32(ctx, handle + 4u);
    StoreU32(ctx, handle + 4u, p0 - 1u);
    p0 = static_cast<uint32_t>(p0 != 1u);
    if (p0) {
      goto os_release_1;
    }
    p0 = LoadU32(ctx, LoadU32(ctx, handle) + 8u);
    StoreU32(ctx, kDataD015C, 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterReleaseSlotPair177) {
    HostReleaseSlotPair(ctx, p0, handle);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterReleaseSlotPair177;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = LoadU32(ctx, kDataD015C);
    StoreU32(ctx, kDataD015C, 0u);
    p0 = static_cast<uint32_t>(p0 != 1u);
  }
  if (p0 || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, handle + 8u);
      StoreU32(ctx, handle + 8u, p0 - 1u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0) {
        goto os_release_1;
      }
      p0 = LoadU32(ctx, LoadU32(ctx, handle) + 12u);
      StoreU32(ctx, kDataD015C, 0u);
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterReleaseSlotPair178) {
      HostReleaseSlotPair(ctx, p0, handle);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
      SyncMemoryBase(ctx);
#endif
      resume_code = kAfterReleaseSlotPair178;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      p0 = LoadU32(ctx, kDataD015C);
      StoreU32(ctx, kDataD015C, 0u);
      p0 = static_cast<uint32_t>(p0 != 1u);
      if (p0) {
        goto os_release_1;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    goto rethrow_1;
  }
  // Merge point os_release_1.
  os_release_1:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostOsRelease(ctx, ret);
    TrapUnreachable();
  }
  // Merge point rethrow_1.
  rethrow_1:;
  if (ResumePhase(ctx) == kResumeRunning) {
    p0 = FindMatchingCatch(ctx, 0u);
    HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_code == kAfterPanic179) {
    Panic(ctx, p0);
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
    SyncMemoryBase(ctx);
#endif
    resume_code = kAfterPanic179;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  goto done;

  // Suspend: push the code naming the call in flight, step past it, then push the
  // 100-byte frame a rewinding pass reads back -- the arguments, the frame pointer
  // and the locals, in the order the prologue pops them.
  suspend_point:;
  {
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, saved_frame, p0);
    StoreU32(ctx, saved_frame + 4u, p1);
    StoreU32(ctx, saved_frame + 8u, p2);
    StoreU32(ctx, saved_frame + 12u, frame);
    StoreU32(ctx, saved_frame + 16u, node);
    StoreU32(ctx, saved_frame + 20u, slot);
    StoreU32(ctx, saved_frame + 24u, word);
    StoreU32(ctx, saved_frame + 28u, ret);
    StoreU32(ctx, saved_frame + 32u, flag);
    StoreU32(ctx, saved_frame + 36u, aux);
    StoreU32(ctx, saved_frame + 40u, scratch_a);
    StoreU32(ctx, saved_frame + 44u, scratch_b);
    StoreU32(ctx, saved_frame + 48u, scratch_c);
    StoreU32(ctx, saved_frame + 52u, handle);
    StoreU32(ctx, saved_frame + 56u, result);
    StoreU32(ctx, saved_frame + 60u, scratch_d);
    StoreU32(ctx, saved_frame + 64u, scratch_e);
    StoreU32(ctx, saved_frame + 68u, scratch_f);
    StoreU32(ctx, saved_frame + 72u, scratch_g);
    StoreF32(ctx, saved_frame + 76u, float_a);
    StoreF32(ctx, saved_frame + 80u, float_b);
    StoreF64(ctx, saved_frame + 84u, double_a);
    StoreF64(ctx, saved_frame + 92u, double_b);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  }
  done:;
  LeaveFrame();
}

}  // namespace Compositor
}  // namespace artc

