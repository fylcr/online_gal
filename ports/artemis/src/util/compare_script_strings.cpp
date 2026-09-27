// compare_script_strings.cpp -- the string comparison the parser uses for identifiers and keywords..
//
// CompareScriptStrings -- the string comparison the parser uses for identifiers and keywords.
//
// The step is a plain call: the body holds no suspend point, so the
// frame pair is bookkeeping only. Judgment order is untouched -- the
// goto graph is the generated driver's own flattening, renamed labels
// only.
//
// src: artemis.wasm2c.c 317969-317969 (f837); R1 transcribe-keep.
//
// Written against artemis-compat, with the official WebAssembly decompile
// as the reference: this body reproduces the asyncify module's generated C
// call for call.  The prose below records what this particular step does.
//

#include "util/engine_context.h"

namespace artc {

// Forward declarations -- batch-mates and registered callees.
uint32_t CompareNulTerminatedStrings(EngineContext* ctx, uint32_t left, uint32_t right);

namespace {


}  // namespace

uint32_t CompareScriptStrings(EngineContext* ctx, uint32_t left, uint32_t right) {
  EnterFrame();
  uint32_t i0 = 0u; uint32_t i1 = 0u;
  i0 = left;
  i1 = right;
  i0 = CompareNulTerminatedStrings(ctx, i0, i1);
  LeaveFrame();
  return i0;
}
}
