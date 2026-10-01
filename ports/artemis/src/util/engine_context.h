// engine_context.h — guest engine state, frame discipline and memory accessors.
//
// This header is the vocabulary boundary: it gives the guest state, the frame
// discipline and the memory accessors real names, and it is the one translation
// unit allowed to spell the generated module's own symbols.
//
// This header is that boundary: the context struct, its memory slot, the raw
// load/store helpers and the frame macros live here. Everything else in src/
// talks to the guest exclusively through the named accessors below, so call
// sites read as intent ("load a byte") rather than as raw address arithmetic.
// Accessors re-read the memory view on every call, which is behaviourally
// identical to the generated code's cached view (the region never moves and the
// cache is refreshed after every growth point).
#pragma once
#include <stdint.h>
namespace artc {
using EngineContext = ::w2c_artemis;
inline uint32_t LoadU8(EngineContext* ctx, uint64_t addr) {
  return i32_load8_u_default32(ctx->w2c_a_a->data, ctx->w2c_a_a->size, ctx->w2c_a_a, addr);
}
inline void StoreU8(EngineContext* ctx, uint64_t addr, uint32_t val) {
  i32_store8_default32(ctx->w2c_a_a->data, ctx->w2c_a_a->size, ctx->w2c_a_a, addr, val);
}
inline uint32_t LoadU32(EngineContext* ctx, uint64_t addr) {
  return i32_load_default32(ctx->w2c_a_a->data, ctx->w2c_a_a->size, ctx->w2c_a_a, addr);
}
inline void StoreU32(EngineContext* ctx, uint64_t addr, uint32_t val) {
  i32_store_default32(ctx->w2c_a_a->data, ctx->w2c_a_a->size, ctx->w2c_a_a, addr, val);
}
inline uint64_t LoadU64(EngineContext* ctx, uint64_t addr) {
  return i64_load_default32(ctx->w2c_a_a->data, ctx->w2c_a_a->size, ctx->w2c_a_a, addr);
}
inline void StoreU64(EngineContext* ctx, uint64_t addr, uint64_t val) {
  i64_store_default32(ctx->w2c_a_a->data, ctx->w2c_a_a->size, ctx->w2c_a_a, addr, val);
}
inline double LoadF64(EngineContext* ctx, uint64_t addr) {
  return f64_load_default32(ctx->w2c_a_a->data, ctx->w2c_a_a->size, ctx->w2c_a_a, addr);
}
inline uint64_t ReinterpretF64(double val) { return (uint64_t)i64_reinterpret_f64(val); }
// The guest's shared nil value record: the lookup miss sentinel and the
// fallback slot both point here, so a miss reads as a canonical nil.
inline uint32_t const kNilValueAddr = 176936u;
// Frame discipline: one call level per guest-visible entry — the generated
// FUNC_PROLOGUE/FUNC_EPILOGUE pair under its house names. EnterFrame() at the
// top of a product body, LeaveFrame() before every return, exactly once each.
inline void EnterFrame() { FUNC_PROLOGUE; }
inline void LeaveFrame() { FUNC_EPILOGUE; }
// Asyncify driver state: the resume phase（0 running、1 suspended、2 rewinding）
// and the event return-frame stack pointer, behind accessors because the
// generated struct itself is frozen. R2 may lift the phase into an enum class.
inline uint32_t& ResumePhase(EngineContext* ctx) { return ctx->w2c_g3; }
inline uint32_t& ReturnFrameSp(EngineContext* ctx) { return ctx->w2c_g4; }
inline uint32_t const kResumeRunning = 0u;
inline uint32_t const kResumeSuspended = 1u;
inline uint32_t const kResumeRewinding = 2u;
// The generated unreachable trap, wrapped so products never spell the macro.
inline void TrapUnreachable() { UNREACHABLE; }
// Guest linear-memory stack pointer: the transient frame allocator the
// generated code calls g0; frames grow down. Suspendable bodies must keep
// their frame traffic（it drives unwind/rewind）; pure leaves whose scratch
// frame is net-zero may use locals instead.
inline uint32_t& GuestStackSp(EngineContext* ctx) { return ctx->w2c_g0; }
inline void StoreF64(EngineContext* ctx, uint64_t addr, double val) {
  f64_store_default32(ctx->w2c_a_a->data, ctx->w2c_a_a->size, ctx->w2c_a_a, addr, val);
}
// Host bulk copy: the engine's big-block memory import, the winner past the
// size threshold where the host beats the guest copy loop.
inline void HostBulkCopy(EngineContext* ctx, uint32_t dst, uint32_t src, uint32_t n) {
  (void)w2c_a_yb(ctx->w2c_a_instance, dst, src, n);
}
// Entry-point forwarders: thin call-throughs for callees that still run as
// generated code（R1/R2 backlog）. Delete a forwarder once its real product
// lands, then align the caller's name via rewrite-names.tsv.
inline uint32_t FinishTableIndex(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c);
inline void GrowStringAndReplace(EngineContext* ctx, uint32_t arg0, uint32_t arg1, uint32_t arg2,
                                 uint32_t arg3, uint32_t arg4, uint32_t arg5, uint32_t arg6,
                                 uint32_t arg7);
// Batch-2 table-array resize (f1114). Declared before the legacy
// The string-table spelling the intern path already calls: same generated
// callee, product-facing name ResizeTableArray (batch 2). Delete this alias
// when script/intern_string.cpp takes the new name.
inline void ResizeStringTable(EngineContext* ctx, uint32_t state, uint32_t capacity) {
  ResizeTableArray(ctx, state, capacity);
}
// The raw guest-heap core underneath the allocator product: one block per
// request, zero on refusal. It stays generated code, so it keeps a forwarder.
inline uint32_t AllocateRawBlock(EngineContext* ctx, uint32_t bytes) {
  return w2c_artemis_Nc_0(ctx, bytes);
}
// f1727 lexer step's callees: the character/separator readers, the long
// string and numeral readers, the source-id formatter, the keyword-table
// grower and the error path. They stay generated code (R1/R2 backlog).
inline uint32_t NextCharSlow(EngineContext* ctx, uint32_t reader) {
  return w2c_artemis_f285(ctx, reader);
}
inline uint32_t SkipSep(EngineContext* ctx, uint32_t lex_state) {
  return w2c_artemis_f901(ctx, lex_state);
}
inline void ReadLongString(EngineContext* ctx, uint32_t lex_state, uint32_t seminfo, uint32_t sep) {
  w2c_artemis_f1726(ctx, lex_state, seminfo, sep);
}
inline void LexError(EngineContext* ctx, uint32_t lex_state, uint32_t message, uint32_t token) {
  w2c_artemis_f397(ctx, lex_state, message, token);
}
inline uint32_t FormatPushString(EngineContext* ctx, uint32_t state, uint32_t format, uint32_t args) {
  return w2c_artemis_f219(ctx, state, format, args);
}
inline void ThrowScriptError(EngineContext* ctx, uint32_t state, uint32_t code) {
  w2c_artemis_f236(ctx, state, code);
}
inline void GrowTableSlot(EngineContext* ctx, uint32_t state) {
  w2c_artemis_f363(ctx, state);
}
inline void ReadNumeral(EngineContext* ctx, uint32_t lex_state, uint32_t seminfo) {
  w2c_artemis_f5594(ctx, lex_state, seminfo);
}
inline void FormatSourceId(EngineContext* ctx, uint32_t dst, uint32_t src, uint32_t length) {
  w2c_artemis_f312(ctx, dst, src, length);
}
// Batch-1 lexer/parser/allocator callees (R1/R2 backlog): they stay generated
// code and keep call-throughs until their own rewrites land.
inline void DeallocateRawBlock(EngineContext* ctx, uint32_t block) {
  w2c_artemis_Oc_0(ctx, block);
}
inline void StoreListEntry(EngineContext* ctx, uint32_t list, uint32_t index, uint32_t value) {
  w2c_artemis_f518(ctx, list, index, value);
}
inline void CopyStringBytes(EngineContext* ctx, uint32_t text, uint32_t bytes, uint32_t length) {
  w2c_artemis_f523(ctx, text, bytes, length);
}
inline void RaiseAllocLimitError(EngineContext* ctx, uint32_t state) {
  w2c_artemis_f218(ctx, state);
}
inline void RaiseAllocatorAssert(EngineContext* ctx, uint32_t state) {
  w2c_artemis_f253(ctx, state);
}
inline void UnorderedRehash(EngineContext* ctx, uint32_t map, uint32_t buckets) {
  w2c_artemis_f635(ctx, map, buckets);
}
inline void UnorderedUpdateLoadFactor(EngineContext* ctx, uint32_t map, uint32_t count) {
  w2c_artemis_f4180(ctx, map, count);
}
inline uint32_t PollEventKind(EngineContext* ctx) {
  return w2c_artemis_f329(ctx);
}
inline void RunEventStep(EngineContext* ctx, uint32_t state) {
  w2c_artemis_f3544(ctx, state);
}
// Batch-2 parser/compiler/lod callees (R0 backlog): they stay generated code
inline void CompileMainBody(EngineContext* ctx, uint32_t func_state, uint32_t message) {
  w2c_artemis_f1717(ctx, func_state, message);
}
inline void StoreGlobalEntry(EngineContext* ctx, uint32_t globals, uint32_t index,
                             uint32_t value) {
  w2c_artemis_f310(ctx, globals, index, value);
}
// Batch-4B closure forwarders: the 50 products still in the backlog call
  inline uint32_t InitTaggedNodeDescriptor(EngineContext* ctx, uint32_t p0) { return w2c_artemis_f283(ctx, p0); }
  inline void StoreSuspendableEntry(EngineContext* ctx, uint32_t p0, uint32_t p1, uint32_t p2) { w2c_artemis_f302(ctx, p0, p1, p2); }
  inline uint32_t ReadPackedI64Value(EngineContext* ctx, uint32_t p0, uint32_t p1, uint32_t p2) { return w2c_artemis_f376(ctx, p0, p1, p2); }
  inline uint32_t FormatLuaEventFilterLine(EngineContext* ctx, uint32_t p0, uint32_t p1) { return w2c_artemis_f392(ctx, p0, p1); }
  inline void ReleaseNodeListTail(EngineContext* ctx, uint32_t p0, uint32_t p1) { w2c_artemis_f438(ctx, p0, p1); }
  inline uint32_t MeasureFormattedNumber(EngineContext* ctx, uint32_t p0, uint32_t p1, uint32_t p2) { return w2c_artemis_f671(ctx, p0, p1, p2); }
  inline void ReleaseSubtreeTaggedNodes(EngineContext* ctx, uint32_t p0, uint32_t p1) { w2c_artemis_f679(ctx, p0, p1); }
  inline void FormatLuaTagFilterLine(EngineContext* ctx, uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3) { w2c_artemis_f681(ctx, p0, p1, p2, p3); }
  inline uint32_t TerminateStringAt(EngineContext* ctx, uint32_t p0, uint32_t p1) { return w2c_artemis_f829(ctx, p0, p1); }
  inline uint32_t ResolveVariableKind(EngineContext* ctx, uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3) { return w2c_artemis_f908(ctx, p0, p1, p2, p3); }
  inline uint32_t PadStringBuffer(EngineContext* ctx, uint32_t p0, uint32_t p1, uint32_t p2) { return w2c_artemis_f1004(ctx, p0, p1, p2); }
  inline double ReadNumericFieldAsDouble(EngineContext* ctx, uint32_t p0, uint32_t p1) { return w2c_artemis_f1006(ctx, p0, p1); }
  inline uint32_t InvokeVirtualSlot(EngineContext* ctx, uint32_t p0) { return w2c_artemis_f1043(ctx, p0); }
  inline void ResetSuspendableFrame(EngineContext* ctx) { w2c_artemis_f1218(ctx); }
  inline void ApplyAvoidStatus(EngineContext* ctx, uint32_t p0, uint32_t p1) { w2c_artemis_f1458(ctx, p0, p1); }
  inline void AdvanceGainStatStage(EngineContext* ctx, uint32_t p0) { w2c_artemis_f1463(ctx, p0); }
  inline void AdvanceEngineFrameTasks(EngineContext* ctx, uint32_t p0) { w2c_artemis_f2890(ctx, p0); }
  inline uint32_t AdvanceHttpFetchState(EngineContext* ctx, uint32_t p0) { return w2c_artemis_f3294(ctx, p0); }
  inline void PauseFullscreenVideo(EngineContext* ctx, uint32_t p0) { w2c_artemis_f3872(ctx, p0); }
  inline void RebuildUnorderedIndexTable(EngineContext* ctx, uint32_t p0) { w2c_artemis_f3933(ctx, p0); }
  inline void PrepareParameterFrame(EngineContext* ctx, uint32_t p0, uint32_t p1) { w2c_artemis_f5538(ctx, p0, p1); }
// Batch-8A closure forwarders: the three dispatch products landing in this batch
  // (f859 LuaEngine::Dispatch, f3196 TextFxDispatch, f3548 RpDispatch) all call
  // these horizontally shared helpers, so they get names now and keep their
  // generated bodies until their own batch lands them.  Evidence per name is in
  // rewrite-names.tsv; none of the six has a data-segment constant.
  inline void ReleaseSharedRef(EngineContext* ctx, uint32_t p0) { w2c_artemis_f295(ctx, p0); }
  inline void ReleaseSharedRefChain(EngineContext* ctx, uint32_t p0) { w2c_artemis_f4704(ctx, p0); }
  inline uint32_t DestroyDispatchObject(EngineContext* ctx, uint32_t p0) { return w2c_artemis_f883(ctx, p0); }
  inline void FindTaggedTreeEntry(EngineContext* ctx, uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5) { w2c_artemis_f336(ctx, p0, p1, p2, p3, p4, p5); }
  inline void ClearObjectDeque(EngineContext* ctx, uint32_t p0) { w2c_artemis_f580(ctx, p0); }
  inline uint32_t PushObjectDequeElement(EngineContext* ctx, uint32_t p0, uint32_t p1) { return w2c_artemis_f1409(ctx, p0, p1); }
inline uint32_t HostRet0(EngineContext* ctx) {
  return w2c_a_c(ctx->w2c_a_instance);
}
inline void HostYieldToHost(EngineContext* ctx) {
  w2c_a_pc(ctx->w2c_a_instance);
}
inline void HostOsRelease(EngineContext* ctx, uint32_t slot) {
  w2c_a_i(ctx->w2c_a_instance, slot);
}
inline void HostOsSchedule(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  w2c_a_j(ctx->w2c_a_instance, a, b, c);
}
inline uint32_t HostOsQuery(EngineContext* ctx, uint32_t a, uint32_t b, uint32_t c) {
  return w2c_a_k(ctx->w2c_a_instance, a, b, c);
}
inline void HostAssertFail(EngineContext* ctx, uint32_t expression, uint32_t file,
                           uint32_t line, uint32_t function) {
  w2c_a_q(ctx->w2c_a_instance, expression, file, line, function);
}
// A_p takes one region id and returns nothing: the guest telling the host it has
// entered a region, which the host tracks for stack and profile attribution.
// Two ids dominate it -- 1019 and 1021, 120 and 104 sites respectively.
inline void HostMarkRegionSeen(EngineContext* ctx, uint32_t region) {
  w2c_a_p(ctx->w2c_a_instance, region);
}

// A_r restores the saved exception slot on a resume path. Every one of its 126
// call sites sits on the !g3 arm, so it runs when control comes back from the
// host, and the call that follows it clears the pending-throw address.
inline uint32_t HostRestorePendingThrow(EngineContext* ctx, uint32_t saved) {
  return w2c_a_r(ctx->w2c_a_instance, saved);
}
// The remaining function-table dispatch arities: same trampoline as
// HostDispatchInvoke below, one per call signature the guest actually uses.
// Each takes the table slot as its first argument, exactly as the generated
// import does -- these are the invoke_vi/viii/ii/viiiii arms, not named host
// APIs, so the slot is the honest part of the name and the rest is the arity.
// A_d takes two slots and returns nothing, on the release side of an
// ownership pair. The old name read the import letter as a function-table
// arity, which is what the section around it is about, and it was wrong.
inline void HostReleaseSlotPair(EngineContext* ctx, uint32_t slot, uint32_t count) {
  w2c_a_d(ctx->w2c_a_instance, slot, count);
}
inline void HostSyncSlot(EngineContext* ctx, uint32_t slot, uint32_t a, uint32_t b, uint32_t c) {
  w2c_a_f(ctx->w2c_a_instance, slot, a, b, c);
}
// The remaining table trampolines the giant dispatchers reach: the signature is
// the import's own, with the table slot in front. No call site has been read for
// a role yet, so the arity is the honest part of these names -- rename them the
// way A_d became HostReleaseSlotPair and A_f became HostSyncSlot once one is.
inline void HostInvokeViiiiii(EngineContext* ctx, uint32_t slot, uint32_t a, uint32_t b,
                              uint32_t c, uint32_t d, uint32_t e, uint32_t f) {
  w2c_a_l(ctx->w2c_a_instance, slot, a, b, c, d, e, f);
}
inline uint32_t HostInvokeIiiiii(EngineContext* ctx, uint32_t slot, uint32_t a, uint32_t b,
                                 uint32_t c, uint32_t d, uint32_t e) {
  return w2c_a_n(ctx->w2c_a_instance, slot, a, b, c, d, e);
}
inline uint32_t HostInvokeIiiii(EngineContext* ctx, uint32_t slot, uint32_t a, uint32_t b,
                                uint32_t c, uint32_t d) {
  return w2c_a_o(ctx->w2c_a_instance, slot, a, b, c, d);
}
inline float HostInvokeFi(EngineContext* ctx, uint32_t slot, uint32_t a) {
  return w2c_a_t(ctx->w2c_a_instance, slot, a);
}
inline void HostInvokeVif(EngineContext* ctx, uint32_t slot, uint32_t a, float f) {
  w2c_a_v(ctx->w2c_a_instance, slot, a, f);
}
inline uint32_t HostInvokeI(EngineContext* ctx, uint32_t slot) {
  return w2c_a_x(ctx->w2c_a_instance, slot);
}
inline void HostInvokeViiiiiii(EngineContext* ctx, uint32_t slot, uint32_t a, uint32_t b,
                               uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g) {
  w2c_a_y(ctx->w2c_a_instance, slot, a, b, c, d, e, f, g);
}
// A_h is handed a struct offset and a field count and returns a value --
// the fold that turns a field group into one address. The counts it gets
// are 8/16/20/24/28/32/36/4096, never addresses, so this is not a lookup.
inline uint32_t HostFoldLayoutOffset(EngineContext* ctx, uint32_t slot, uint32_t count) {
  return w2c_a_h(ctx->w2c_a_instance, slot, count);
}
inline void HostInitResources(EngineContext* ctx, uint32_t slot, uint32_t a, uint32_t b,
                              uint32_t c, uint32_t d, uint32_t e) {
  w2c_a_s(ctx->w2c_a_instance, slot, a, b, c, d, e);
}
// Not a table dispatch: the async host fetch, which fills the three guest slots
// it is handed（url, pbuffer, pnum, perror）with a fetched buffer, its length and
// an error code.
inline void HostAcquireResource(EngineContext* ctx, uint32_t url, uint32_t buffer,
                                uint32_t length, uint32_t error) {
  w2c_a_fa(ctx->w2c_a_instance, url, buffer, length, error);
}
inline uint32_t CallTableT2(EngineContext* ctx, uint32_t index, uint32_t a) {
  return CALL_INDIRECT(ctx->w2c_Lc, uint32_t (*)(void*, uint32_t), w2c_artemis_t2, index,
                       ctx->w2c_Lc.data[index].module_instance, a);
}
inline uint32_t CallTableT7(EngineContext* ctx, uint32_t index, uint32_t a, uint32_t b,
                            uint32_t c, uint32_t d) {
  return CALL_INDIRECT(ctx->w2c_Lc, uint32_t (*)(void*, uint32_t, uint32_t, uint32_t, uint32_t),
                       w2c_artemis_t7, index, ctx->w2c_Lc.data[index].module_instance,
                       a, b, c, d);
}
// The float-carrying table dispatch: (index, a, b, scale, flag). Forty-four
// table slots carry this signature -- the numeric builtins and the field
// adapters that hand them a scale factor. The scale rides in the third
// argument register, so the C++ face of these entries is a `float`.
inline void CallTableT16(EngineContext* ctx, uint32_t index, uint32_t a, uint32_t b,
                         float scale, uint32_t flag) {
  CALL_INDIRECT(ctx->w2c_Lc, void (*)(void*, uint32_t, uint32_t, float, uint32_t),
                w2c_artemis_t16, index, ctx->w2c_Lc.data[index].module_instance,
                a, b, scale, flag);
}
// The void-returning single-argument dispatch: (index, a). 883 table slots
// carry it -- the setters, the iterators and the shape writers that a caller
// reaches through a method handle and never reads back a value from.
inline void CallTableT1(EngineContext* ctx, uint32_t index, uint32_t a) {
  CALL_INDIRECT(ctx->w2c_Lc, void (*)(void*, uint32_t), w2c_artemis_t1, index,
                ctx->w2c_Lc.data[index].module_instance, a);
}
// The two-argument dispatch that answers with a value: (index, a, b). 572
// table slots carry it. The list walkers and the comparators are the bulk of
// them, so the returned cell is the whole point of the call.
inline uint32_t CallTableT3(EngineContext* ctx, uint32_t index, uint32_t a, uint32_t b) {
  return CALL_INDIRECT(ctx->w2c_Lc, uint32_t (*)(void*, uint32_t, uint32_t), w2c_artemis_t3,
                       index, ctx->w2c_Lc.data[index].module_instance, a, b);
}
// The three-word void dispatch: (index, a, b, c). 355 table slots carry it --
// the mutators whose handle takes a receiver and two arguments, and the
// table-builder entries that append a value to a cell.
inline void CallTableT5(EngineContext* ctx, uint32_t index, uint32_t a, uint32_t b,
                        uint32_t c) {
  CALL_INDIRECT(ctx->w2c_Lc, void (*)(void*, uint32_t, uint32_t, uint32_t), w2c_artemis_t5,
                index, ctx->w2c_Lc.data[index].module_instance, a, b, c);
}
// The widest dispatch in the table: (index, a, b, c, d, e, f), void. 45 slots
// carry it -- the Lua continuation and iterator families, which take a frame
// worth of words and hand nothing back.
inline void CallTableT15(EngineContext* ctx, uint32_t index, uint32_t a, uint32_t b,
                         uint32_t c, uint32_t d, uint32_t e, uint32_t f) {
  CALL_INDIRECT(ctx->w2c_Lc, void (*)(void*, uint32_t, uint32_t, uint32_t, uint32_t,
                                     uint32_t, uint32_t),
                w2c_artemis_t15, index, ctx->w2c_Lc.data[index].module_instance,
                a, b, c, d, e, f);
}
// The scale-taking dispatch without a trailing flag: (index, a, b, scale).
// Only a dozen table slots carry it: the node-handler upcalls, which the
// invokers reach after resolving a handler pair out of a node record.
inline void CallTableT13(EngineContext* ctx, uint32_t index, uint32_t a, uint32_t b,
                         float scale) {
  CALL_INDIRECT(ctx->w2c_Lc, void (*)(void*, uint32_t, uint32_t, float), w2c_artemis_t13,
                index, ctx->w2c_Lc.data[index].module_instance, a, b, scale);
}
// The table slots the backlog products dispatch through. Same
// thin-call-through rule as the forwarders above -- the C++ name is the
// slot, the types are the slot's own function-pointer type, and the
// argument order follows the callee's own signature.
// slot t0: void (*)(void*, u32, u32)
inline void CallTableT0(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1) {
  CALL_INDIRECT(ctx->w2c_Lc, void (*)(void*, u32, u32),
    w2c_artemis_t0, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1);
}
// slot t4: u32 (*)(void*, u32, u32, u32), 310 call sites in the rewritten spans
inline uint32_t CallTableT4(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2) {
  return CALL_INDIRECT(ctx->w2c_Lc, u32 (*)(void*, u32, u32, u32),
    w2c_artemis_t4, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2);
}
// slot t8: void (*)(void*, u32, u32, u32, u32), 66 call sites in the rewritten spans
inline void CallTableT8(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3) {
  CALL_INDIRECT(ctx->w2c_Lc, void (*)(void*, u32, u32, u32, u32),
    w2c_artemis_t8, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3);
}
// slot t9: u32 (*)(void*, u32, u32, u32, u32, u32), 48 call sites in the rewritten spans
inline uint32_t CallTableT9(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4) {
  return CALL_INDIRECT(ctx->w2c_Lc, u32 (*)(void*, u32, u32, u32, u32, u32),
    w2c_artemis_t9, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4);
}
// slot t10: f32 (*)(void*, u32), 88 call sites in the rewritten spans
inline float CallTableT10(EngineContext* ctx, uint32_t index, uint32_t a0) {
  return CALL_INDIRECT(ctx->w2c_Lc, f32 (*)(void*, u32),
    w2c_artemis_t10, index, ctx->w2c_Lc.data[index].module_instance,
    a0);
}
// slot t11: void (*)(void*, u32, u32, u32, u32, u32), 12 call sites in the rewritten spans
inline void CallTableT11(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4) {
  CALL_INDIRECT(ctx->w2c_Lc, void (*)(void*, u32, u32, u32, u32, u32),
    w2c_artemis_t11, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4);
}
// slot t12: void (*)(void*, u32, f32), 82 call sites in the rewritten spans
inline void CallTableT12(EngineContext* ctx, uint32_t index, uint32_t a0, float a1) {
  CALL_INDIRECT(ctx->w2c_Lc, void (*)(void*, u32, f32),
    w2c_artemis_t12, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1);
}
// slot t14: u32 (*)(void*, u32, u32, u32, u32, u32, u32), 9 call sites in the rewritten spans
inline uint32_t CallTableT14(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5) {
  return CALL_INDIRECT(ctx->w2c_Lc, u32 (*)(void*, u32, u32, u32, u32, u32, u32),
    w2c_artemis_t14, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4, a5);
}
// slot t18: void (*)(void*, u32, u32, u32, u32, u32, u32, u32), 7 call sites in the rewritten spans
inline void CallTableT18(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6) {
  CALL_INDIRECT(ctx->w2c_Lc, void (*)(void*, u32, u32, u32, u32, u32, u32, u32),
    w2c_artemis_t18, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4, a5, a6);
}
// slot t19: u32 (*)(void*, u32, u32, u32, u32, u32, u32, u32), 4 call sites in the rewritten spans
inline uint32_t CallTableT19(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6) {
  return CALL_INDIRECT(ctx->w2c_Lc, u32 (*)(void*, u32, u32, u32, u32, u32, u32, u32),
    w2c_artemis_t19, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4, a5, a6);
}
// slot t20: u32 (*)(void*, u32, u32, u32, u32, u32, u32, u32, u32), 5 call sites in the rewritten spans
inline uint32_t CallTableT20(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7) {
  return CALL_INDIRECT(ctx->w2c_Lc, u32 (*)(void*, u32, u32, u32, u32, u32, u32, u32, u32),
    w2c_artemis_t20, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4, a5, a6, a7);
}
// slot t22: u32 (*)(void*, u32, u64, u32), 6 call sites in the rewritten spans
inline uint32_t CallTableT22(EngineContext* ctx, uint32_t index, uint32_t a0, uint64_t a1, uint32_t a2) {
  return CALL_INDIRECT(ctx->w2c_Lc, u32 (*)(void*, u32, u64, u32),
    w2c_artemis_t22, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2);
}
// slot t24: void (*)(void*, u32, u32, u32, u32, u32, u32, u32, u32, u32), 1 call site in the rewritten spans
inline void CallTableT24(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8) {
  CALL_INDIRECT(ctx->w2c_Lc, void (*)(void*, u32, u32, u32, u32, u32, u32, u32, u32, u32),
    w2c_artemis_t24, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4, a5, a6, a7, a8);
}
// slot t25: void (*)(void*, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32), 1 call site in the rewritten spans
inline void CallTableT25(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8, uint32_t a9) {
  CALL_INDIRECT(ctx->w2c_Lc, void (*)(void*, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32),
    w2c_artemis_t25, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
}
// slot t26: void (*)(void*, u32, f32, u32), 5 call sites in the rewritten spans
inline void CallTableT26(EngineContext* ctx, uint32_t index, uint32_t a0, float a1, uint32_t a2) {
  CALL_INDIRECT(ctx->w2c_Lc, void (*)(void*, u32, f32, u32),
    w2c_artemis_t26, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2);
}
// slot t27: u32 (*)(void*, u32, u32, u32, u32, u32, u32, u32, u32, u32), 5 call sites in the rewritten spans
inline uint32_t CallTableT27(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8) {
  return CALL_INDIRECT(ctx->w2c_Lc, u32 (*)(void*, u32, u32, u32, u32, u32, u32, u32, u32, u32),
    w2c_artemis_t27, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4, a5, a6, a7, a8);
}
// slot t28: u32 (*)(void*, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32), 1 call site in the rewritten spans
inline uint32_t CallTableT28(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8, uint32_t a9) {
  return CALL_INDIRECT(ctx->w2c_Lc, u32 (*)(void*, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32),
    w2c_artemis_t28, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
}
// slot t29: u32 (*)(void*, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32), 2 call sites in the rewritten spans
inline uint32_t CallTableT29(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8, uint32_t a9, uint32_t a10) {
  return CALL_INDIRECT(ctx->w2c_Lc, u32 (*)(void*, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32),
    w2c_artemis_t29, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}
// slot t31: u64 (*)(void*, u32, u64, u32), 4 call sites in the rewritten spans
inline uint64_t CallTableT31(EngineContext* ctx, uint32_t index, uint32_t a0, uint64_t a1, uint32_t a2) {
  return CALL_INDIRECT(ctx->w2c_Lc, u64 (*)(void*, u32, u64, u32),
    w2c_artemis_t31, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2);
}
// slot t32: void (*)(void*, u32, u32, u64, u32, u32), 2 call sites in the rewritten spans
inline void CallTableT32(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint64_t a2, uint32_t a3, uint32_t a4) {
  CALL_INDIRECT(ctx->w2c_Lc, void (*)(void*, u32, u32, u64, u32, u32),
    w2c_artemis_t32, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4);
}
// slot t33: u32 (*)(void*, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32), 1 call site in the rewritten spans
inline uint32_t CallTableT33(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8, uint32_t a9, uint32_t a10, uint32_t a11) {
  return CALL_INDIRECT(ctx->w2c_Lc, u32 (*)(void*, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32),
    w2c_artemis_t33, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
}
// slot t34: u32 (*)(void*, u32, u32, u32, u32, u64), 1 call site in the rewritten spans
inline uint32_t CallTableT34(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint64_t a4) {
  return CALL_INDIRECT(ctx->w2c_Lc, u32 (*)(void*, u32, u32, u32, u32, u64),
    w2c_artemis_t34, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4);
}
// slot t35: u64 (*)(void*, u32, u32, u32, u32), 1 call site in the rewritten spans
inline uint64_t CallTableT35(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3) {
  return CALL_INDIRECT(ctx->w2c_Lc, u64 (*)(void*, u32, u32, u32, u32),
    w2c_artemis_t35, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3);
}
// slot t36: f64 (*)(void*, u32, u32), 1 call site in the rewritten spans
inline double CallTableT36(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1) {
  return CALL_INDIRECT(ctx->w2c_Lc, f64 (*)(void*, u32, u32),
    w2c_artemis_t36, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1);
}
// slot t37: void (*)(void*, u32, f32, f32), 3 call sites in the rewritten spans
inline void CallTableT37(EngineContext* ctx, uint32_t index, uint32_t a0, float a1, float a2) {
  CALL_INDIRECT(ctx->w2c_Lc, void (*)(void*, u32, f32, f32),
    w2c_artemis_t37, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2);
}
// slot t38: f64 (*)(void*, u32, u32, u32), 1 call site in the rewritten spans
inline double CallTableT38(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2) {
  return CALL_INDIRECT(ctx->w2c_Lc, f64 (*)(void*, u32, u32, u32),
    w2c_artemis_t38, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2);
}
// slot t41: void (*)(void*, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32), 1 call site in the rewritten spans
inline void CallTableT41(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8, uint32_t a9, uint32_t a10, uint32_t a11, uint32_t a12, uint32_t a13, uint32_t a14) {
  CALL_INDIRECT(ctx->w2c_Lc, void (*)(void*, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32),
    w2c_artemis_t41, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14);
}
// slot t43: void (*)(void*, u32, f64), 1 call site in the rewritten spans
inline void CallTableT43(EngineContext* ctx, uint32_t index, uint32_t a0, double a1) {
  CALL_INDIRECT(ctx->w2c_Lc, void (*)(void*, u32, f64),
    w2c_artemis_t43, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1);
}
// slot t44: u32 (*)(void*, u32, u32, u32, u32, u32, u64, u64), 1 call site in the rewritten spans
inline uint32_t CallTableT44(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint64_t a5, uint64_t a6) {
  return CALL_INDIRECT(ctx->w2c_Lc, u32 (*)(void*, u32, u32, u32, u32, u32, u64, u64),
    w2c_artemis_t44, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4, a5, a6);
}
// slot t45: u32 (*)(void*, u32, u32, u32, u32, u64, u64), 1 call site in the rewritten spans
inline uint32_t CallTableT45(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint64_t a4, uint64_t a5) {
  return CALL_INDIRECT(ctx->w2c_Lc, u32 (*)(void*, u32, u32, u32, u32, u64, u64),
    w2c_artemis_t45, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4, a5);
}
// slot t46: u32 (*)(void*, u32, u32, u32, u32, f64), 1 call site in the rewritten spans
inline uint32_t CallTableT46(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, double a4) {
  return CALL_INDIRECT(ctx->w2c_Lc, u32 (*)(void*, u32, u32, u32, u32, f64),
    w2c_artemis_t46, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4);
}
// slot t47: u32 (*)(void*, u32, f64, u32, u32, u32, u32), 2 call sites in the rewritten spans
inline uint32_t CallTableT47(EngineContext* ctx, uint32_t index, uint32_t a0, double a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5) {
  return CALL_INDIRECT(ctx->w2c_Lc, u32 (*)(void*, u32, f64, u32, u32, u32, u32),
    w2c_artemis_t47, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4, a5);
}
// slot t55: void (*)(void*, f32, f32, f32, f32, f32, f32, u32, u32, u32, u32), 1 call site in the rewritten spans
inline void CallTableT55(EngineContext* ctx, uint32_t index, float a0, float a1, float a2, float a3, float a4, float a5, uint32_t a6, uint32_t a7, uint32_t a8, uint32_t a9) {
  CALL_INDIRECT(ctx->w2c_Lc, void (*)(void*, f32, f32, f32, f32, f32, f32, u32, u32, u32, u32),
    w2c_artemis_t55, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
}
// slot t58: f32 (*)(void*, u32, u32, u32), 1 call site in the rewritten spans
inline float CallTableT58(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2) {
  return CALL_INDIRECT(ctx->w2c_Lc, f32 (*)(void*, u32, u32, u32),
    w2c_artemis_t58, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2);
}
// slot t60: f32 (*)(void*, u32, u32, u32, u32, u32), 1 call site in the rewritten spans
inline float CallTableT60(EngineContext* ctx, uint32_t index, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4) {
  return CALL_INDIRECT(ctx->w2c_Lc, f32 (*)(void*, u32, u32, u32, u32, u32),
    w2c_artemis_t60, index, ctx->w2c_Lc.data[index].module_instance,
    a0, a1, a2, a3, a4);
}
// The generic function-table call trampoline: (index, a, b, c) plus the
// saved-frame handoff the unwind paths pass. Its first argument is the table
// slot -- 860 in the C++ teardown product. Move it out once host/host_api exists.
inline uint32_t HostDispatchInvoke(EngineContext* ctx, uint32_t index, uint32_t a,
                                   uint32_t b, uint32_t c) {
  return w2c_a_m(ctx->w2c_a_instance, index, a, b, c);
}
// Batch-1 vocabulary additions: the narrow load/store widths the string,
// flag and float paths read (the wide forms landed earlier).
inline uint32_t LoadU16(EngineContext* ctx, uint64_t addr) {
  return i32_load16_u_default32(ctx->w2c_a_a->data, ctx->w2c_a_a->size, ctx->w2c_a_a, addr);
}
inline int32_t LoadS8(EngineContext* ctx, uint64_t addr) {
  return i32_load8_s_default32(ctx->w2c_a_a->data, ctx->w2c_a_a->size, ctx->w2c_a_a, addr);
}
inline int32_t LoadS16(EngineContext* ctx, uint64_t addr) {
  return i32_load16_s_default32(ctx->w2c_a_a->data, ctx->w2c_a_a->size, ctx->w2c_a_a, addr);
}
inline void StoreU16(EngineContext* ctx, uint64_t addr, uint32_t val) {
  i32_store16_default32(ctx->w2c_a_a->data, ctx->w2c_a_a->size, ctx->w2c_a_a, addr, val);
}
inline float LoadF32(EngineContext* ctx, uint64_t addr) {
  return f32_load_default32(ctx->w2c_a_a->data, ctx->w2c_a_a->size, ctx->w2c_a_a, addr);
}
inline void StoreF32(EngineContext* ctx, uint64_t addr, float val) {
  f32_store_default32(ctx->w2c_a_a->data, ctx->w2c_a_a->size, ctx->w2c_a_a, addr, val);
}
// The real symbol is a file-scope `static inline`, so no declaration of it can
// link from another translation unit. The body lives here instead: it is the
// no-op the guarded call compiles to, and callers guard it with the same macro,
// so the guarded call never reaches a missing symbol.
inline void SyncMemoryBase(EngineContext* ctx) {
#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE
  wasm_rt_segue_write_base((*ctx->w2c_a_a).data);
#else
  (void)ctx;
#endif
}
// f4551 teardown bridges: the shared_ptr precondition assertion, the catch
// matcher and the temp-return reader are named wraps of host imports, and the
// table dispatch runs one (i32) -> () entry of the function table（the macro's
// own type check keeps the native trap texts intact）. The failure handoff goes
// to f199 until util/panic.cpp lands — delete that forwarder when the product
// arrives, and move the three host wraps out once host/host_api exists.
// The boundary spells the generated name; code-assemble's rename layer injects
// a route-compat alias to the transcription's symbol, so both build routes see
// one spelling here.
inline void Panic(EngineContext* ctx, uint32_t code) {
  w2c_artemis_f199(ctx, code);
}
inline void HostFailAssertion(EngineContext* ctx, uint32_t word, uint32_t expression,
                              uint32_t file, uint32_t line, uint32_t function) {
  w2c_a_e(ctx->w2c_a_instance, word, expression, file, line, function);
}
inline uint32_t FindMatchingCatch(EngineContext* ctx, uint32_t type) {
  return w2c_a_g(ctx->w2c_a_instance, type);
}
inline uint32_t HostTempRet0(EngineContext* ctx) {
  return w2c_a_b(ctx->w2c_a_instance);
}
inline void CallTableFunction(EngineContext* ctx, uint32_t index, uint32_t target) {
  CALL_INDIRECT(ctx->w2c_Lc, void (*)(void*, u32), w2c_artemis_t1, index,
                ctx->w2c_Lc.data[index].module_instance, target);
}
// f201 allocator bridges: the collector step runs one zero-argument entry of
// the function table（the memory-pressure sweep the retry loop drives）, and the
// host exception pair is the engine's out-of-memory raise — a four-byte record
// the caller stamps and hands to the thrower, which does not return. Both are
// named wraps of host/table calls; move them out once host/host_api exists.
inline void CallCollectorStep(EngineContext* ctx, uint32_t index) {
  CALL_INDIRECT(ctx->w2c_Lc, void (*)(void*), w2c_artemis_t6, index,
                ctx->w2c_Lc.data[index].module_instance);
}
inline uint32_t HostAllocateException(EngineContext* ctx, uint32_t size) {
  return w2c_a_u(ctx->w2c_a_instance, size);
}
inline void HostThrowException(EngineContext* ctx, uint32_t record, uint32_t type_word,
                               uint32_t destructor_word) {
  w2c_a_w(ctx->w2c_a_instance, record, type_word, destructor_word);
}
// Guest scratch word that the dispatchers share; kept here so the name is defined once.
constexpr uint32_t kDataD015C = 0xD015Cu;
// Operand words and the size message the expression compilers share; kept here
// so both of them name one definition.
constexpr uint32_t kData1FFFF = 0x1FFFFu;
constexpr uint32_t kData20000 = 0x20000u;
constexpr uint32_t kData7FFFC000 = 0x7FFFC000u;
constexpr uint32_t kDataFFFFFFFF = 0xFFFFFFFFu;
constexpr uint32_t kControlStructureTooLongMessage = 0x2A394u;  // control structure too long

}  // namespace artc
