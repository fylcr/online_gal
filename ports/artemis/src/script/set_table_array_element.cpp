#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the key pusher and the field writer.
void PushTableField(EngineContext* ctx, uint32_t state, uint32_t index, uint32_t key);
void SetTableField(EngineContext* ctx, uint32_t state, uint32_t index, uint32_t key);

// Writes the array part's element at an index: the key is pushed, the field set
// through the shared writer, and a number value boxed into a value record. The
// step is suspendable, so each call spills the frame and is replayed on resume.
void SetTableArrayElement(EngineContext* ctx, uint32_t state, uint32_t table, uint32_t index) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 12u;
  uint32_t const kResumeWordBytes = 4u;
  uint32_t frame = 0u;
  uint32_t resume_word = 0u;
  // The value record a boxed number is written into; on a resume its saved top
  // is what the gate below goes by.
  uint32_t number_record = 0u;
  uint32_t call_code = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 12-byte parameter frame and restore it in the order the
    // suspend path wrote it -- the state, the table, the index.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    state = LoadU32(ctx, frame);
    table = LoadU32(ctx, frame + 4u);
    index = LoadU32(ctx, frame + 8u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: the second pop names the call that was in flight. The two pops
    // stay separate -- one region is read back at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  // Running: only a non-negative array index proceeds; a resumed step goes by
  // the value-record top its frame kept instead.
  uint32_t const index_in_range = static_cast<uint32_t>(static_cast<int32_t>(index) >= 0);
  uint32_t const ready = ResumePhase(ctx) ? number_record : index_in_range;
  if (ready != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      if (table + 9999u <= 9999u) {
        // A small table number is a pseudo-index: rebase it onto the array part.
        const uint32_t used = LoadU32(ctx, state + 8u) - LoadU32(ctx, state + 12u);
        table += static_cast<uint32_t>(static_cast<int32_t>(used) >> 4u) + 1u;
      }
      PushTableField(ctx, state, table, 0u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 0u) : 1u) {
      SetTableField(ctx, state, table, index);
      if (ResumePhase(ctx) == kResumeSuspended) {
        call_code = 0u;
        goto suspend_frame;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // Box the index as a number: the tag first, the double in the record, then
      // the top advances a whole 16-byte record.
      number_record = LoadU32(ctx, state + 8u);
      StoreU32(ctx, number_record + 8u, 3u);
      StoreF64(ctx, number_record, static_cast<double>(static_cast<int32_t>(index)));
      // The top has to be re-read here: the tag store above went to the record,
      // not to the state, so this still names the record just boxed.
      StoreU32(ctx, state + 8u, LoadU32(ctx, state + 8u) + 16u);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_word == 1u) : 1u) {
      SetTableField(ctx, state, table, 0u);
      if (ResumePhase(ctx) == kResumeSuspended) {
        call_code = 1u;
        goto suspend_frame;
      }
    }
  }
  LeaveFrame();
  return;
suspend_frame:
  // Suspended: park the word naming the call above the frame, step past it, then
  // write the parameter frame a rewind pass reads back.
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), call_code);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, state);
  StoreU32(ctx, frame + 4u, table);
  StoreU32(ctx, frame + 8u, index);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  LeaveFrame();
}

}  // namespace artc
