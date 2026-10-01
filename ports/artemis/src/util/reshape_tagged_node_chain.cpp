#include "util/engine_context.h"

namespace artc {

// Reshape helpers -- one per element kind, driven along the chain in turn.
uint32_t ReallocateBlock(EngineContext* ctx, uint32_t state, uint32_t block, uint32_t old_size, uint32_t new_size);
void ReshapePowerOfTwoNode(EngineContext* ctx, uint32_t state, uint32_t node);
void ReshapeCountedArrayNode(EngineContext* ctx, uint32_t state, uint32_t node);
void ReshapeHeaderVariantNode(EngineContext* ctx, uint32_t state, uint32_t node);
void ReshapeFieldRecordNode(EngineContext* ctx, uint32_t state, uint32_t node);
void ReshapeInlinePairNode(EngineContext* ctx, uint32_t state, uint32_t node);

namespace {

// The tag the nested chain step is driven with.
constexpr uint32_t kReshapeTaggedNodeChainNested = 0xFFFFFFFDu;
constexpr uint32_t kReshapeTaggedNodeChainSaveFrameBytes = 32u;
constexpr uint32_t kReshapeTaggedNodeChainResumeWordBytes = 4u;

}  // namespace

// Walks the chain of elements hanging off one node and reshapes every element
// in place, picking the reshape that matches the element's kind.  A running
// pass advances one element per round; a rewinding pass replays the single
// reshape named by its resume code instead.
uint32_t ReshapeTaggedNodeChain(EngineContext* ctx, uint32_t state, uint32_t node, uint32_t tag) {
  uint32_t scratch = 0u;       // kind byte, element address, reallocation size
  uint32_t elem = 0u;          // the element currently being reshaped
  uint32_t frame_base = 0u;    // where a suspended call left its frame
  uint32_t resume_code = 0u;   // which reshape a rewinding pass replays
  uint32_t tag_differs = 0u;   // element tag byte differs from the state's
  uint32_t table = 0u;         // the element table held in the state
  uint32_t flag_mask = 0u;     // masked state flags compared against the tag
  uint32_t result = 0u;        // the node handed back once the walk finishes
  uint32_t call_result = 0u;   // what a resumable call returned
  uint32_t suspend_code = 0u;  // resume word pushed when a call suspends
  uint32_t tmp = 0u;
  uint32_t tmp2 = 0u;
  uint32_t tmp3 = 0u;
  EnterFrame();
  // Rewinding: pop this step's saved frame and restore the live words.
  if (ResumePhase(ctx) == kResumeRewinding) {
    tmp = ReturnFrameSp(ctx);
    tmp2 = ReturnFrameSp(ctx);
    tmp2 = LoadU32(ctx, tmp2);
    tmp3 = kReshapeTaggedNodeChainSaveFrameBytes;
    tmp2 -= tmp3;
    StoreU32(ctx, tmp, tmp2);
    frame_base = ReturnFrameSp(ctx);
    frame_base = LoadU32(ctx, frame_base);
    state = LoadU32(ctx, frame_base);
    tag = LoadU32(ctx, frame_base + 8u);
    scratch = LoadU32(ctx, frame_base + 12u);
    elem = LoadU32(ctx, frame_base + 16u);
    tag_differs = LoadU32(ctx, frame_base + 20u);
    table = LoadU32(ctx, frame_base + 24u);
    flag_mask = LoadU32(ctx, frame_base + 28u);
    node = LoadU32(ctx, frame_base + 4u);
  }
  // Rewinding: drop the pushed resume word and reload the resume code.
  if (ResumePhase(ctx) == kResumeRewinding) {
    tmp = ReturnFrameSp(ctx);
    tmp2 = ReturnFrameSp(ctx);
    tmp2 = LoadU32(ctx, tmp2);
    tmp3 = kReshapeTaggedNodeChainResumeWordBytes;
    tmp2 -= tmp3;
    StoreU32(ctx, tmp, tmp2);
    tmp = ReturnFrameSp(ctx);
    tmp = LoadU32(ctx, tmp);
    tmp = LoadU32(ctx, tmp);
    resume_code = tmp;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    elem = LoadU32(ctx, node);
    scratch = (elem == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (scratch != 0u) {
      goto chain_end;
    }
    table = LoadU32(ctx, state + 16u);
    scratch = LoadU8(ctx, table + 20u);
    flag_mask = scratch ^ 3u;
  }
  for (;;) {
    if (ResumePhase(ctx) == kResumeRunning) {
      if (tag == 0u) {
        break;
      }
      scratch = (LoadU8(ctx, elem + 4u) == 8u);
    }
    if ((scratch != 0u) | (ResumePhase(ctx) == kResumeRewinding)) {
      // Keep the restored argument while rewinding, take the element's inline
      // array otherwise.
      scratch = (ResumePhase(ctx) != 0u) ? scratch : (elem + 104u);
      if ((ResumePhase(ctx) == kResumeRunning) | (resume_code == 0u)) {
        call_result = ReshapeTaggedNodeChain(ctx, state, scratch, kReshapeTaggedNodeChainNested);
        suspend_code = 0u;
        if (ResumePhase(ctx) == kResumeSuspended) {
          goto save_suspend_frame;
        }
        scratch = call_result;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      scratch = LoadU8(ctx, elem + 5u);
      tag_differs = flag_mask & (scratch ^ 3u);
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      if (tag_differs != 0u) {
        scratch = (scratch & 248u) | (LoadU8(ctx, table + 20u) & 3u);
        StoreU8(ctx, elem + 5u, scratch);
        node = elem;
        goto element_done;
      }
      tmp = LoadU32(ctx, elem);
      StoreU32(ctx, node, tmp);
      tmp = LoadU32(ctx, table + 28u);
      if (elem == tmp) {
        tmp2 = LoadU32(ctx, elem);
        StoreU32(ctx, table + 28u, tmp2);
      }
      scratch = LoadU8(ctx, elem + 4u) - 4u;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      switch (scratch) {
        case 0u: goto reshape_6;
        case 1u: goto reshape_4;
        case 2u: goto reshape_2;
        case 3u: goto reshape_7;
        case 4u: goto reshape_5;
        case 5u: goto reshape_1;
        case 6u: goto reshape_3;
        default: goto element_done;
      }
    }
  reshape_1:;
    if ((ResumePhase(ctx) == kResumeRunning) | (resume_code == 1u)) {
      ReshapeFieldRecordNode(ctx, state, elem);
      suspend_code = 1u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_suspend_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      goto element_done;
    }
  reshape_2:;
    if ((ResumePhase(ctx) == kResumeRunning) | (resume_code == 2u)) {
      ReshapeHeaderVariantNode(ctx, state, elem);
      suspend_code = 2u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_suspend_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      goto element_done;
    }
  reshape_3:;
    if ((ResumePhase(ctx) == kResumeRunning) | (resume_code == 3u)) {
      ReshapeInlinePairNode(ctx, state, elem);
      suspend_code = 3u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_suspend_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      goto element_done;
    }
  reshape_4:;
    if ((ResumePhase(ctx) == kResumeRunning) | (resume_code == 4u)) {
      ReshapePowerOfTwoNode(ctx, state, elem);
      suspend_code = 4u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_suspend_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      goto element_done;
    }
  reshape_5:;
    if ((ResumePhase(ctx) == kResumeRunning) | (resume_code == 5u)) {
      ReshapeCountedArrayNode(ctx, state, elem);
      suspend_code = 5u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_suspend_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      goto element_done;
    }
  reshape_6:;
    if (ResumePhase(ctx) == kResumeRunning) {
      scratch = LoadU32(ctx, state + 16u);
      tmp = LoadU32(ctx, scratch + 4u) - 1u;
      StoreU32(ctx, scratch + 4u, tmp);
      scratch = LoadU32(ctx, elem + 12u) + 17u;
    }
    if ((ResumePhase(ctx) == kResumeRunning) | (resume_code == 6u)) {
      call_result = ReallocateBlock(ctx, state, elem, scratch, 0u);
      suspend_code = 6u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_suspend_frame;
      }
      elem = call_result;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      goto element_done;
    }
  reshape_7:;
    if (ResumePhase(ctx) == kResumeRunning) {
      scratch = LoadU32(ctx, elem + 16u) + 24u;
    }
    if ((ResumePhase(ctx) == kResumeRunning) | (resume_code == 7u)) {
      ReallocateBlock(ctx, state, elem, scratch, 0u);
      suspend_code = 7u;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto save_suspend_frame;
      }
    }
  element_done:;
    if (ResumePhase(ctx) == kResumeRunning) {
      tag -= 1u;
      elem = LoadU32(ctx, node);
      if (elem != 0u) {
        continue;
      }
    }
    break;
  }
chain_end:;
  if (ResumePhase(ctx) == kResumeRunning) {
    result = node;
    goto frame_stored;
  }
  TrapUnreachable();
save_suspend_frame:;
  // A call suspended: store the resume word, push it, then hand the live
  // words to the frame above the stack pointer.
  tmp = ReturnFrameSp(ctx);
  tmp = LoadU32(ctx, tmp);
  StoreU32(ctx, tmp, suspend_code);
  tmp = ReturnFrameSp(ctx);
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  tmp3 = kReshapeTaggedNodeChainResumeWordBytes;
  tmp2 += tmp3;
  StoreU32(ctx, tmp, tmp2);
  tmp = ReturnFrameSp(ctx);
  tmp = LoadU32(ctx, tmp);
  frame_base = tmp;
  tmp2 = state;
  StoreU32(ctx, frame_base, tmp2);
  StoreU32(ctx, frame_base + 4u, node);
  StoreU32(ctx, frame_base + 8u, tag);
  StoreU32(ctx, frame_base + 12u, scratch);
  StoreU32(ctx, frame_base + 16u, elem);
  StoreU32(ctx, frame_base + 20u, tag_differs);
  StoreU32(ctx, frame_base + 24u, table);
  StoreU32(ctx, frame_base + 28u, flag_mask);
  tmp = ReturnFrameSp(ctx);
  tmp2 = ReturnFrameSp(ctx);
  tmp2 = LoadU32(ctx, tmp2);
  tmp3 = kReshapeTaggedNodeChainSaveFrameBytes;
  tmp2 += tmp3;
  StoreU32(ctx, tmp, tmp2);
  result = 0u;
frame_stored:;
  LeaveFrame();
  return result;
}
}  // namespace artc
