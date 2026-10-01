#include "util/engine_context.h"

namespace artc {

// Defined elsewhere in the tree.
void FormatNumberValue(EngineContext* ctx, uint32_t state, uint32_t value, uint32_t options);
uint32_t InternString(EngineContext* ctx, uint32_t a0, uint32_t a1, uint32_t a2);
void EnsureTableArrayRoom(EngineContext* ctx, uint32_t holder);

namespace {

constexpr uint32_t kInsertFormattedKeyEntryString = 0x29D02u;  // the key string interned for a formatted entry
constexpr uint32_t kInsertFormattedKeyEntrySaveFrameBytes = 12u;  // holder, key, slot
constexpr uint32_t kInsertFormattedKeyEntryResumeWordBytes = 4u;
constexpr uint32_t kInsertFormattedKeyEntryRoomCall = 0u;  // the room check was in flight
constexpr uint32_t kInsertFormattedKeyEntryFormatCall = 1u;  // the number formatting was in flight
constexpr uint32_t kInsertFormattedKeyEntryInternCall = 2u;  // the string interning was in flight

}  // namespace

// Inserts a formatted key's entry into the object's key list: works out the slot
// the key belongs in, formats the number into it, and stores the entry pointer
// there. A key that arrives as a string is interned instead, and a non-zero key
// leaves the list alone. The step suspends around its three calls; each gate
// replays the one its resume code names.
void InsertFormattedKeyEntry(EngineContext* ctx, uint32_t holder, uint32_t key) {
  EnterFrame();
  uint32_t frame = 0u;
  uint32_t call_result = 0u;
  uint32_t entry = 0u;
  uint32_t resume_code = 0u;
  uint32_t slot = 0u;
  uint32_t resume_word = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kInsertFormattedKeyEntrySaveFrameBytes);
    frame = LoadU32(ctx, ReturnFrameSp(ctx));
    holder = LoadU32(ctx, frame);
    slot = LoadU32(ctx, frame + 8u);
    key = LoadU32(ctx, frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kInsertFormattedKeyEntryResumeWordBytes);
    resume_word = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = static_cast<uint32_t>((int32_t)key >= (int32_t)2u);
  }
  if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // A small integer key: the slot list has to have room for its entry.
      slot = LoadU32(ctx, holder + 16u);
      slot = static_cast<uint32_t>(LoadU32(ctx, slot + 68u) >= LoadU32(ctx, slot + 64u));
    }
    if (slot != 0u || ResumePhase(ctx) == kResumeRewinding) {
      if (ResumePhase(ctx) == kResumeRunning || resume_word == kInsertFormattedKeyEntryRoomCall) {
        EnsureTableArrayRoom(ctx, holder);
        resume_code = kInsertFormattedKeyEntryRoomCall;
        if (ResumePhase(ctx) == kResumeSuspended) {goto suspend_point;}
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = static_cast<uint32_t>((int32_t)(LoadU32(ctx, holder + 8u) - LoadU32(ctx, holder + 12u)) >> 4) - 1u;
    }
    if (ResumePhase(ctx) == kResumeRunning || resume_word == kInsertFormattedKeyEntryFormatCall) {
      FormatNumberValue(ctx, holder, key, slot);
      resume_code = kInsertFormattedKeyEntryFormatCall;
      if (ResumePhase(ctx) == kResumeSuspended) {goto suspend_point;}
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = holder + 8u;
      entry = LoadU32(ctx, holder + 8u) + ((1u - key) << 4);
      goto entry_ready;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (key != 0u) {goto skip_store;}
    key = LoadU32(ctx, holder + 8u);
  }
  if (ResumePhase(ctx) == kResumeRunning || resume_word == kInsertFormattedKeyEntryInternCall) {
    call_result = InternString(ctx, holder, kInsertFormattedKeyEntryString, 0u);
    resume_code = kInsertFormattedKeyEntryInternCall;
    if (ResumePhase(ctx) == kResumeSuspended) {goto suspend_point;}
    slot = call_result;
  }
  if (ResumePhase(ctx) != kResumeRunning) {
    entry = holder;
  } else {
    StoreU32(ctx, key + 8u, 4u);
    StoreU32(ctx, key, slot);
    slot = holder + 8u;
    entry = LoadU32(ctx, holder + 8u) + 16u;
  }
  entry_ready:;
  holder = entry;
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, slot, holder);
  }
  skip_store:;
  // The running pass is done; the frame write below belongs to a suspend only.
  goto exit_path;
  // Suspend here: the resume word naming the call in flight goes first, then the
  // three saved words, and the caller's frame pointer is left above both.
  suspend_point:;
  StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kInsertFormattedKeyEntryResumeWordBytes);
  frame = LoadU32(ctx, ReturnFrameSp(ctx));
  StoreU32(ctx, frame, holder);
  StoreU32(ctx, frame + 4u, key);
  StoreU32(ctx, frame + 8u, slot);
  StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kInsertFormattedKeyEntrySaveFrameBytes);
  exit_path:;
  LeaveFrame();
}
}  // namespace artc
