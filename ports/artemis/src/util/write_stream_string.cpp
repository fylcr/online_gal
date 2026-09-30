// write_stream_string.cpp -- a NUL-terminated string is written to an output stream: the string is measured with StringLength and the bytes are forwarded to Fwrite as size 1..
//
// WriteStreamString -- a NUL-terminated string is written to an output stream: the string is measured with StringLength and the bytes are forwarded to Fwrite as size 1.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 276873-276873 (f720); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t StringLength(EngineContext* ctx, uint32_t text);
uint32_t Fwrite(EngineContext* ctx, uint32_t ptr, uint32_t size, uint32_t count, uint32_t stream);

namespace {

constexpr uint32_t kF720ResumeWordBytes = 4u;

}  // namespace

void WriteStreamString(EngineContext* ctx, uint32_t str, uint32_t stream) {
  EnterFrame();
  uint32_t l2 = 0u; uint32_t l3 = 0u;
  uint32_t i0 = 0u; uint32_t i1 = 0u; uint32_t i2 = 0u; uint32_t i3 = 0u;
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = 12u;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    l2 = i0;
    i0 = LoadU32(ctx, i0);
    str = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 4u);
    stream = i0;
    i0 = l2;
    i0 = LoadU32(ctx, i0 + 8u);
    l2 = i0;
  }
  i0 = ResumePhase(ctx);
  i1 = 2u;
  i0 = i0 == i1;
  if (i0) {
    i0 = ReturnFrameSp(ctx);
    i1 = ReturnFrameSp(ctx);
    i1 = LoadU32(ctx, i1);
    i2 = kF720ResumeWordBytes;
    i1 -= i2;
    StoreU32(ctx, i0, i1);
    i0 = ReturnFrameSp(ctx);
    i0 = LoadU32(ctx, i0);
    i0 = LoadU32(ctx, i0);
    l3 = i0;
  }
  i0 = ResumePhase(ctx);
  i0 = !(i0);
  if (i0) {
    i0 = str;
    i0 = StringLength(ctx, i0);
    l2 = i0;
  }
  i0 = l3;
  i0 = !(i0);
  i1 = 1u;
  i2 = ResumePhase(ctx);
  i0 = i2 ? i0 : i1;
  if (i0) {
    i0 = str;
    i1 = 1u;
    i2 = l2;
    i3 = stream;
    i0 = Fwrite(ctx, i0, i1, i2, i3);
    i0 = 0u;
    i1 = ResumePhase(ctx);
    i2 = 1u;
    i1 = i1 == i2;
    if (i1) {goto spill_frame;}
  }
  goto func_exit;
  spill_frame:;
  l3 = i0;
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  i1 = l3;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = kF720ResumeWordBytes;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  i0 = ReturnFrameSp(ctx);
  i0 = LoadU32(ctx, i0);
  l3 = i0;
  i1 = str;
  StoreU32(ctx, i0, i1);
  i0 = l3;
  i1 = stream;
  StoreU32(ctx, i0 + 4u, i1);
  i0 = l3;
  i1 = l2;
  StoreU32(ctx, i0 + 8u, i1);
  i0 = ReturnFrameSp(ctx);
  i1 = ReturnFrameSp(ctx);
  i1 = LoadU32(ctx, i1);
  i2 = 12u;
  i1 += i2;
  StoreU32(ctx, i0, i1);
  func_exit:;
  LeaveFrame();
}
}
