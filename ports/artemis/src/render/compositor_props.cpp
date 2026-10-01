#include "util/engine_context.h"

namespace artc {

// Forward declarations -- the sibling steps this body drives.
void Panic(EngineContext* ctx, uint32_t a0);
void ResizeValueStack(EngineContext* ctx, uint32_t a0, uint32_t a1);
void SetStackTop(EngineContext* ctx, uint32_t stack, uint32_t index);
void AppendTableSlot(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);

namespace {

// The layer property tags the dispatcher matches, as pointers into the module's
// data section: "time", "textTween", "textClearTween", "sound" and "video".
constexpr uint32_t kLyPropTagTime = 0xBB304u;
constexpr uint32_t kLyPropTagTextTween = 0xBB309u;
constexpr uint32_t kLyPropTagTextClearTween = 0xBB313u;
constexpr uint32_t kLyPropTagSound = 0xBB322u;
constexpr uint32_t kLyPropTagVideo = 0xBB328u;
// The engine's pending-throw latch, cleared before every bridge call that can raise
// and read back once it returns.
constexpr uint32_t kLyPropThrowLatch = 0xD015Cu;
// The word every property query is handed as its absent-value answer, the marker the
// descriptor's +4 word keeps while no value has been published (it doubles as the
// value-stack index that takes the latest slot back off the stack), and the
// width-filling index handed to SetStackTop.
constexpr uint32_t kLyPropQueryFallback = 0xFFFFD8F0u;
constexpr uint32_t kLyPropNoSlot = 0xFFFFFFFEu;
constexpr uint32_t kLyPropTopIndex = 0xFFFFFFFFu;

}  // namespace

namespace Compositor {

// Dispatches one layer property write. The state the caller passes holds the layer's
// property table at +20; the five property tags time, textTween, textClearTween,
// sound and video are matched in that order through the table's words at +2768 and
// the tag bytes at +2772..+2776, and the first one present is published by a schedule
// call followed by the query that reads the value back. Both calls can run guest
// code, so the body is suspendable: a rewinding pass restores the descriptor, the
// state pointer and the five slots below from a saved frame and replays exactly the
// one call its resume code names.
void LyPropDispatch(EngineContext* ctx, uint32_t descriptor, uint32_t state) {
  EnterFrame();
  uint32_t const kSaveFrameBytes = 28u;
  uint32_t const kResumeWordBytes = 4u;
  // The codes the driver spills at the fourteen await sites below, naming the call
  // that was in flight: the slot append the body opens with, the probe query that
  // reads the layer, then each property's schedule and query call, the sync that
  // publishes the value, and the handoff that ends a failed dispatch.
  uint32_t const kAppendSlot = 0u;
  uint32_t const kProbeQuery = 1u;
  uint32_t const kTimeSchedule = 2u;
  uint32_t const kTimeQuery = 3u;
  uint32_t const kTextTweenSchedule = 4u;
  uint32_t const kTextTweenQuery = 5u;
  uint32_t const kTextClearTweenSchedule = 6u;
  uint32_t const kTextClearTweenQuery = 7u;
  uint32_t const kSoundSchedule = 8u;
  uint32_t const kSoundQuery = 9u;
  uint32_t const kVideoSchedule = 10u;
  uint32_t const kVideoQuery = 11u;
  uint32_t const kSyncSlot = 12u;
  uint32_t const kPanicHandoff = 13u;
  // The reused slots, all declared up here because a rewinding pass restores every
  // one of them from the saved frame and because the labels below are jumped past
  // their own assignment points. They stand in turn for several roles: the property
  // table (also the query's answer and the released handle), a value slot and the
  // value count, the 32-byte guest work area (also a comparison result), a call's raw
  // return (also the frame pointer while rewinding), and the pending-throw latch
  // (also a comparison result) -- plus one scratch for the comparisons whose result
  // outlives the running block that computed it.
  uint32_t props = 0u;
  uint32_t slot = 0u;
  uint32_t work = 0u;
  uint32_t ret = 0u;
  uint32_t count = 0u;
  uint32_t resume_code = 0u;
  uint32_t latch = 0u;
  uint32_t word = 0u;
  if (ResumePhase(ctx) == kResumeRewinding) {
    // Rewinding: pop the 28-byte frame the suspend path pushed and rebuild the two
    // pointers and the five slots from it, in the order they were written.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kSaveFrameBytes);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    descriptor = LoadU32(ctx, saved_frame);
    props = LoadU32(ctx, saved_frame + 8u);
    work = LoadU32(ctx, saved_frame + 12u);
    slot = LoadU32(ctx, saved_frame + 16u);
    count = LoadU32(ctx, saved_frame + 20u);
    latch = LoadU32(ctx, saved_frame + 24u);
    state = LoadU32(ctx, saved_frame + 4u);
  }
  if (ResumePhase(ctx) == kResumeRewinding) {
    // The second pop names the call that was in flight. The two pops stay separate
    // -- the driver keeps one region at a time.
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) - kResumeWordBytes);
    resume_code = LoadU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)));
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Take the 32-byte work area off the guest stack and point the dispatcher at the
    // layer's property table, which the state holds at +16.
    work = GuestStackSp(ctx) - 32u;
    GuestStackSp(ctx) = work;
    props = LoadU32(ctx, state + 16u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kAppendSlot) : 1u) {
    AppendTableSlot(ctx, props, 0u, 0u);
    resume_code = kAppendSlot;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Start the value slot: the descriptor's +4 word carries the no-slot marker, the
    // table goes into the descriptor's own word, the throw latch is armed, and the
    // stack top is set before the latch is read back.
    StoreU32(ctx, descriptor + 4u, kLyPropNoSlot);
    StoreU32(ctx, descriptor, props);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    SetStackTop(ctx, props, kLyPropTopIndex);
    slot = LoadU32(ctx, kLyPropThrowLatch);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    slot = static_cast<uint32_t>(slot == 1u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (slot) {
      goto probe_unwind;
    }
    StoreU32(ctx, kLyPropThrowLatch, 0u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kProbeQuery) : 1u) {
    ret = HostOsQuery(ctx, 1001u, props, kLyPropQueryFallback);
    resume_code = kProbeQuery;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    slot = ret;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // Publish the slot index the probe reported, then resize the value stack and
    // read the latch back: a pending throw unwinds, otherwise the dispatcher starts
    // matching the property tags.
    count = LoadU32(ctx, kLyPropThrowLatch);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    count = static_cast<uint32_t>(count == 1u);
    if (count) {
      goto probe_unwind;
    }
    StoreU32(ctx, descriptor + 4u, slot);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    ResizeValueStack(ctx, props, kLyPropNoSlot);
    props = LoadU32(ctx, kLyPropThrowLatch);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    props = static_cast<uint32_t>(props != 1u);
    if (props) {
      goto tag_dispatch;
    }
    goto throw_path;
  }
probe_unwind:;
  // Reached when the probe or the poke before it left a throw pending: hand the
  // result back, drop the value slot, and release the handle the dispatch held.
  if (ResumePhase(ctx) == kResumeRunning) {
    descriptor = HostRet0(ctx);
    (void)HostTempRet0(ctx);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    ResizeValueStack(ctx, props, kLyPropNoSlot);
    state = LoadU32(ctx, kLyPropThrowLatch);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    state = static_cast<uint32_t>(state == 1u);
    if (state) {
      goto throw_path;
    }
    HostOsRelease(ctx, descriptor);
    TrapUnreachable();
  }
tag_dispatch:;
  // The tag chain opens here, over the property table the state keeps at +20. The
  // byte at +2772 says whether the "time" property carries a value; if it does, the
  // +2768 word names the layer state the write goes through.
  if (ResumePhase(ctx) == kResumeRunning) {
    props = LoadU32(ctx, state + 20u);
    slot = LoadU8(ctx, props + 2772u);
  }
  if (slot || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      // The value's data pointer sits in the descriptor's first word; the count of
      // value words is its +8 minus its +12 word, halved twice, and the latch that
      // follows guards the schedule call.
      slot = LoadU32(ctx, descriptor);
      StoreU32(ctx, kLyPropThrowLatch, 0u);
      StoreU32(ctx, work + 16u, slot);
      count = LoadU32(ctx, slot + 8u) - LoadU32(ctx, slot + 12u);
      count = static_cast<uint32_t>(static_cast<int32_t>(count) >> 4);
      latch = LoadU32(ctx, kLyPropThrowLatch);
      StoreU32(ctx, kLyPropThrowLatch, 0u);
      latch = static_cast<uint32_t>(latch == 1u);
      if (latch) {
        goto time_schedule_result;
      }
      StoreU32(ctx, work + 24u, descriptor);
      StoreU32(ctx, kLyPropThrowLatch, 0u);
      count = count + 1u;
      StoreU32(ctx, work + 20u, count);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kTimeSchedule) : 1u) {
      HostOsSchedule(ctx, 969u, slot, kLyPropTagTime);
      resume_code = kTimeSchedule;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, kLyPropThrowLatch);
      StoreU32(ctx, kLyPropThrowLatch, 0u);
      slot = static_cast<uint32_t>(slot == 1u);
      if (slot) {
        goto time_schedule_result;
      }
      StoreU32(ctx, kLyPropThrowLatch, 0u);
      slot = work + 16u;
      props = props + 2768u;
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kTimeQuery) : 1u) {
      ret = HostOsQuery(ctx, 3389u, slot, props);
      resume_code = kTimeQuery;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
      props = ret;
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      // The schedule and the query are done; the result decides whether the value is
      // published or the walk moves on to the next tag.
      props = LoadU32(ctx, kLyPropThrowLatch);
      StoreU32(ctx, kLyPropThrowLatch, 0u);
      props = static_cast<uint32_t>(props == 1u);
      if (props) {
        goto time_query_result;
      }
      props = LoadU32(ctx, work + 16u);
      if (props) {
        StoreU32(ctx, kLyPropThrowLatch, 0u);
        ResizeValueStack(ctx, props, kLyPropNoSlot);
        props = LoadU32(ctx, kLyPropThrowLatch);
        StoreU32(ctx, kLyPropThrowLatch, 0u);
        if (props == 1u) {
          goto throw_path;
        }
      }
      props = LoadU32(ctx, state + 20u);
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    // The "textTween" tag: a set byte at +2773 means the property carries a value.
    // The work area's +12 word marks the tween as running and +16 keeps the value
    // pointer; the schedule then reads the value count out of the value itself.
    slot = static_cast<uint32_t>(LoadU8(ctx, props + 2773u) == 0u);
    if (slot) {
      goto tag_text_clear_tween;
    }
    StoreU32(ctx, work + 12u, 1u);
    props = LoadU32(ctx, descriptor);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    StoreU32(ctx, work + 16u, props);
    slot = LoadU32(ctx, props + 8u) - LoadU32(ctx, props + 12u);
    slot = static_cast<uint32_t>(static_cast<int32_t>(slot) >> 4);
    count = LoadU32(ctx, kLyPropThrowLatch);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    count = static_cast<uint32_t>(count != 1u);
  }
  if (count || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, work + 24u, descriptor);
      StoreU32(ctx, kLyPropThrowLatch, 0u);
      slot = slot + 1u;
      StoreU32(ctx, work + 20u, slot);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kTextTweenSchedule) : 1u) {
      HostOsSchedule(ctx, 969u, props, kLyPropTagTextTween);
      resume_code = kTextTweenSchedule;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      props = LoadU32(ctx, kLyPropThrowLatch);
      StoreU32(ctx, kLyPropThrowLatch, 0u);
      props = static_cast<uint32_t>(props != 1u);
      if (props) {
        goto text_tween_query;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    props = HostRet0(ctx);
    state = HostTempRet0(ctx);
    goto probe_result;
  }
time_query_result:;
  // The "time" probe came back: read the value slot the work area kept, drop it, and
  // follow the pending-throw latch out of the dispatch.
  if (ResumePhase(ctx) == kResumeRunning) {
    props = HostRet0(ctx);
    state = HostTempRet0(ctx);
    state = LoadU32(ctx, work + 16u);
    work = static_cast<uint32_t>(state == 0u);
    if (work) {
      goto probe_result;
    }
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    ResizeValueStack(ctx, state, kLyPropNoSlot);
    state = LoadU32(ctx, kLyPropThrowLatch);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    state = static_cast<uint32_t>(state != 1u);
    if (state) {
      goto probe_result;
    }
    goto throw_path;
  }
time_schedule_result:;
  if (ResumePhase(ctx) == kResumeRunning) {
    props = HostRet0(ctx);
    state = HostTempRet0(ctx);
    goto probe_result;
  }
text_tween_query:;
  // The "textTween" query goes through the work area's +12 word as its first
  // argument and the +16 word as its second.
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    slot = work + 12u;
    props = work + 16u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kTextTweenQuery) : 1u) {
    ret = HostOsQuery(ctx, 3390u, props, slot);
    resume_code = kTextTweenQuery;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    props = ret;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    props = LoadU32(ctx, kLyPropThrowLatch);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    props = static_cast<uint32_t>(props == 1u);
    if (props) {
      goto text_tween_query_result;
    }
    props = LoadU32(ctx, work + 16u);
    if (props) {
      StoreU32(ctx, kLyPropThrowLatch, 0u);
      ResizeValueStack(ctx, props, kLyPropNoSlot);
      props = LoadU32(ctx, kLyPropThrowLatch);
      StoreU32(ctx, kLyPropThrowLatch, 0u);
      if (props == 1u) {
        goto throw_path;
      }
    }
    props = LoadU32(ctx, state + 20u);
  }
tag_text_clear_tween:;
  // The "textClearTween" tag at +2774, written the same way the tween above writes
  // it: mark the work area, keep the value pointer, then count the value's words.
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = static_cast<uint32_t>(LoadU8(ctx, props + 2774u) == 0u);
    if (slot) {
      goto tag_sound;
    }
    StoreU32(ctx, work + 12u, 1u);
    props = LoadU32(ctx, descriptor);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    StoreU32(ctx, work + 16u, props);
    slot = LoadU32(ctx, props + 8u) - LoadU32(ctx, props + 12u);
    slot = static_cast<uint32_t>(static_cast<int32_t>(slot) >> 4);
    count = LoadU32(ctx, kLyPropThrowLatch);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    count = static_cast<uint32_t>(count != 1u);
  }
  if (count || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, work + 24u, descriptor);
      StoreU32(ctx, kLyPropThrowLatch, 0u);
      slot = slot + 1u;
      StoreU32(ctx, work + 20u, slot);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kTextClearTweenSchedule) : 1u) {
      HostOsSchedule(ctx, 969u, props, kLyPropTagTextClearTween);
      resume_code = kTextClearTweenSchedule;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      props = LoadU32(ctx, kLyPropThrowLatch);
      StoreU32(ctx, kLyPropThrowLatch, 0u);
      props = static_cast<uint32_t>(props != 1u);
      if (props) {
        goto text_clear_tween_query;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    props = HostRet0(ctx);
    state = HostTempRet0(ctx);
    goto probe_result;
  }
text_tween_query_result:;
  // The "textTween" query's answer.
  if (ResumePhase(ctx) == kResumeRunning) {
    props = HostRet0(ctx);
    state = HostTempRet0(ctx);
    state = LoadU32(ctx, work + 16u);
    work = static_cast<uint32_t>(state == 0u);
    if (work) {
      goto probe_result;
    }
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    ResizeValueStack(ctx, state, kLyPropNoSlot);
    state = LoadU32(ctx, kLyPropThrowLatch);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    state = static_cast<uint32_t>(state != 1u);
    if (state) {
      goto probe_result;
    }
    goto throw_path;
  }
text_clear_tween_query:;
  // The "textClearTween" query, with the same two work-area words as its arguments.
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    slot = work + 12u;
    props = work + 16u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kTextClearTweenQuery) : 1u) {
    ret = HostOsQuery(ctx, 3390u, props, slot);
    resume_code = kTextClearTweenQuery;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    props = ret;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    props = LoadU32(ctx, kLyPropThrowLatch);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    props = static_cast<uint32_t>(props == 1u);
    if (props) {
      goto text_clear_tween_query_result;
    }
    props = LoadU32(ctx, work + 16u);
    if (props) {
      StoreU32(ctx, kLyPropThrowLatch, 0u);
      ResizeValueStack(ctx, props, kLyPropNoSlot);
      props = LoadU32(ctx, kLyPropThrowLatch);
      StoreU32(ctx, kLyPropThrowLatch, 0u);
      if (props == 1u) {
        goto throw_path;
      }
    }
    props = LoadU32(ctx, state + 20u);
  }
tag_sound:;
  // The "sound" tag at +2775. On a rewinding pass the latch slot carries the value
  // this block would have computed, so the gate below only has to re-run its tail.
  if (ResumePhase(ctx) == kResumeRunning) {
    slot = static_cast<uint32_t>(LoadU8(ctx, props + 2775u) == 0u);
    if (slot) {
      goto tag_video;
    }
    slot = LoadU32(ctx, descriptor);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    StoreU32(ctx, work + 16u, slot);
    count = LoadU32(ctx, slot + 8u) - LoadU32(ctx, slot + 12u);
    count = static_cast<uint32_t>(static_cast<int32_t>(count) >> 4);
    latch = LoadU32(ctx, kLyPropThrowLatch);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    word = static_cast<uint32_t>(latch != 1u);
  }
  if (word || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, work + 24u, descriptor);
      StoreU32(ctx, kLyPropThrowLatch, 0u);
      count = count + 1u;
      StoreU32(ctx, work + 20u, count);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSoundSchedule) : 1u) {
      HostOsSchedule(ctx, 969u, slot, kLyPropTagSound);
      resume_code = kSoundSchedule;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      slot = LoadU32(ctx, kLyPropThrowLatch);
      StoreU32(ctx, kLyPropThrowLatch, 0u);
      slot = static_cast<uint32_t>(slot != 1u);
      if (slot) {
        goto sound_query;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    props = HostRet0(ctx);
    state = HostTempRet0(ctx);
    goto probe_result;
  }
text_clear_tween_query_result:;
  // The "textClearTween" query's answer.
  if (ResumePhase(ctx) == kResumeRunning) {
    props = HostRet0(ctx);
    state = HostTempRet0(ctx);
    state = LoadU32(ctx, work + 16u);
    work = static_cast<uint32_t>(state == 0u);
    if (work) {
      goto probe_result;
    }
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    ResizeValueStack(ctx, state, kLyPropNoSlot);
    state = LoadU32(ctx, kLyPropThrowLatch);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    state = static_cast<uint32_t>(state != 1u);
    if (state) {
      goto probe_result;
    }
    goto throw_path;
  }
sound_query:;
  // The "sound" query keeps its two arguments in the work area's +16 word and in the
  // property table's +2776 word.
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    slot = work + 16u;
    props = props + 2776u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSoundQuery) : 1u) {
    ret = HostOsQuery(ctx, 3391u, slot, props);
    resume_code = kSoundQuery;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    props = ret;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    props = LoadU32(ctx, kLyPropThrowLatch);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    props = static_cast<uint32_t>(props == 1u);
    if (props) {
      goto sound_query_result;
    }
    props = LoadU32(ctx, work + 16u);
    if (props) {
      StoreU32(ctx, kLyPropThrowLatch, 0u);
      ResizeValueStack(ctx, props, kLyPropNoSlot);
      props = LoadU32(ctx, kLyPropThrowLatch);
      StoreU32(ctx, kLyPropThrowLatch, 0u);
      if (props == 1u) {
        goto throw_path;
      }
    }
    props = LoadU32(ctx, state + 20u);
  }
tag_video:;
  // The "video" tag at +2792. On the last tag the dispatch stops matching: a set
  // byte means the property is absent and the dispatch finishes instead.
  if (ResumePhase(ctx) == kResumeRunning) {
    state = static_cast<uint32_t>(LoadU8(ctx, props + 2792u) == 0u);
    if (state) {
      goto dispatch_done;
    }
    state = LoadU32(ctx, descriptor);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    StoreU32(ctx, work + 16u, state);
    slot = LoadU32(ctx, state + 8u) - LoadU32(ctx, state + 12u);
    slot = static_cast<uint32_t>(static_cast<int32_t>(slot) >> 4);
    count = LoadU32(ctx, kLyPropThrowLatch);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    word = static_cast<uint32_t>(count != 1u);
  }
  if (word || ResumePhase(ctx) == kResumeRewinding) {
    if (ResumePhase(ctx) == kResumeRunning) {
      StoreU32(ctx, work + 24u, descriptor);
      StoreU32(ctx, kLyPropThrowLatch, 0u);
      slot = slot + 1u;
      StoreU32(ctx, work + 20u, slot);
    }
    if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kVideoSchedule) : 1u) {
      HostOsSchedule(ctx, 969u, state, kLyPropTagVideo);
      resume_code = kVideoSchedule;
      if (ResumePhase(ctx) == kResumeSuspended) {
        goto suspend_point;
      }
    }
    if (ResumePhase(ctx) == kResumeRunning) {
      state = LoadU32(ctx, kLyPropThrowLatch);
      StoreU32(ctx, kLyPropThrowLatch, 0u);
      state = static_cast<uint32_t>(state != 1u);
      if (state) {
        goto video_query;
      }
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    props = HostRet0(ctx);
    state = HostTempRet0(ctx);
    goto probe_result;
  }
sound_query_result:;
  // The "sound" query's answer.
  if (ResumePhase(ctx) == kResumeRunning) {
    props = HostRet0(ctx);
    state = HostTempRet0(ctx);
    state = LoadU32(ctx, work + 16u);
    work = static_cast<uint32_t>(state == 0u);
    if (work) {
      goto probe_result;
    }
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    ResizeValueStack(ctx, state, kLyPropNoSlot);
    state = LoadU32(ctx, kLyPropThrowLatch);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    state = static_cast<uint32_t>(state != 1u);
    if (state) {
      goto probe_result;
    }
    goto throw_path;
  }
video_query:;
  // The "video" query, whose two arguments are the work area's +16 word and the
  // property table's +2796 word.
  if (ResumePhase(ctx) == kResumeRunning) {
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    props = props + 2796u;
    state = work + 16u;
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kVideoQuery) : 1u) {
    ret = HostOsQuery(ctx, 3391u, state, props);
    resume_code = kVideoQuery;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
    state = ret;
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    state = LoadU32(ctx, kLyPropThrowLatch);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    state = static_cast<uint32_t>(state == 1u);
    if (state) {
      goto video_query_result;
    }
    descriptor = LoadU32(ctx, work + 16u);
    state = static_cast<uint32_t>(descriptor == 0u);
    if (state) {
      goto dispatch_done;
    }
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    ResizeValueStack(ctx, descriptor, kLyPropNoSlot);
    descriptor = LoadU32(ctx, kLyPropThrowLatch);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    descriptor = static_cast<uint32_t>(descriptor == 1u);
    if (descriptor) {
      goto throw_path;
    }
  }
dispatch_done:;
  // The dispatch is over: give the 32-byte work area back to the guest stack and
  // leave through the single exit.
  if (ResumePhase(ctx) == kResumeRunning) {
    work = work + 32u;
    GuestStackSp(ctx) = work;
    goto exit_frame;
  }
video_query_result:;
  // The "video" query's answer.
  if (ResumePhase(ctx) == kResumeRunning) {
    props = HostRet0(ctx);
    state = HostTempRet0(ctx);
    state = LoadU32(ctx, work + 16u);
    work = static_cast<uint32_t>(state == 0u);
    if (work) {
      goto probe_result;
    }
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    ResizeValueStack(ctx, state, kLyPropNoSlot);
    state = LoadU32(ctx, kLyPropThrowLatch);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    state = static_cast<uint32_t>(state == 1u);
    if (state) {
      goto throw_path;
    }
  }
probe_result:;
  // The value a probe produced is handed to the layer here. A null value or the
  // no-slot marker means there is nothing to publish, and the handle is released
  // instead.
  if (ResumePhase(ctx) == kResumeRunning) {
    descriptor = LoadU32(ctx, descriptor);
    work = static_cast<uint32_t>(descriptor == 0u);
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    if (work) {
      goto release_slot;
    }
    descriptor = LoadU32(ctx, descriptor + 4u);
    if (descriptor == kLyPropNoSlot) {
      goto release_slot;
    }
    StoreU32(ctx, kLyPropThrowLatch, 0u);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kSyncSlot) : 1u) {
    HostSyncSlot(ctx, 1002u, state, kLyPropQueryFallback, descriptor);
    resume_code = kSyncSlot;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    descriptor = LoadU32(ctx, kLyPropThrowLatch);
    StoreU32(ctx, kLyPropThrowLatch, 0u);
    descriptor = static_cast<uint32_t>(descriptor == 1u);
    if (descriptor) {
      goto throw_path;
    }
  }
release_slot:;
  if (ResumePhase(ctx) == kResumeRunning) {
    HostOsRelease(ctx, props);
    TrapUnreachable();
  }
throw_path:;
  // The exception is in flight. Ask the engine which catch it matches -- there is no
  // type filter at this site, so the filter word is zero -- read the runner's return
  // slot, and hand the catch to the failure bridge.
  if (ResumePhase(ctx) == kResumeRunning) {
    word = FindMatchingCatch(ctx, 0u);
    descriptor = word;
    (void)HostTempRet0(ctx);
  }
  if (ResumePhase(ctx) ? static_cast<uint32_t>(resume_code == kPanicHandoff) : 1u) {
    Panic(ctx, descriptor);
    resume_code = kPanicHandoff;
    if (ResumePhase(ctx) == kResumeSuspended) {
      goto suspend_point;
    }
  }
  if (ResumePhase(ctx) == kResumeRunning) {
    TrapUnreachable();
  }
  goto exit_frame;
suspend_point:;
  {
    // Suspend: push the code naming the call in flight, step past it, then push the
    // 28-byte frame a rewinding pass reads back -- the descriptor, the state pointer
    // and the five slots, in that order. The stack pointer lives in the word the
    // frame pointer itself names, so every push is a read-modify-write of that word.
    StoreU32(ctx, LoadU32(ctx, ReturnFrameSp(ctx)), resume_code);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kResumeWordBytes);
    uint32_t const saved_frame = LoadU32(ctx, ReturnFrameSp(ctx));
    StoreU32(ctx, saved_frame, descriptor);
    StoreU32(ctx, saved_frame + 4u, state);
    StoreU32(ctx, saved_frame + 8u, props);
    StoreU32(ctx, saved_frame + 12u, work);
    StoreU32(ctx, saved_frame + 16u, slot);
    StoreU32(ctx, saved_frame + 20u, count);
    StoreU32(ctx, saved_frame + 24u, latch);
    StoreU32(ctx, ReturnFrameSp(ctx), LoadU32(ctx, ReturnFrameSp(ctx)) + kSaveFrameBytes);
  }
exit_frame:;
  LeaveFrame();
}

}  // namespace Compositor

}  // namespace artc
