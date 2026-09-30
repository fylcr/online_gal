# -*- coding: utf-8 -*-
"""tx_gen2.py -- batch product generator for the artemis R1/R2 rewrite.
Reads fNNN spans from the generated code, mechanically renames the
transcription layer, and emits one product file per function."""
import re, sys, os, traceback

# Paths are derived from this script's own location, never hardcoded: the
# generator lives in ports/artemis/reverse/ and writes exactly the tree the
# bridge build assembles (ports/artemis/src).
_HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(os.path.dirname(_HERE)))
REF = os.path.join(REPO, '.ref', 'artemis-official')
FLAVOUR = os.environ.get('ARTEMIS_FLAVOUR', 'asyncify')

GEN = os.environ.get('ARTEMIS_GEN_C') or os.path.join(REF, 'wasm', FLAVOUR, 'artemis.wasm2c.c')
ROOT = os.environ.get('ARTEMIS_REWRITE') or os.path.join(REPO, 'ports', 'artemis', 'src')

# --only=f293,f3744 emits just those products; --check writes to a scratch tree
# and diffs every product against the maintained tree instead of touching it.
ONLY = None
for _a in sys.argv[1:]:
    if _a.startswith('--only='):
        ONLY = set(_a.split('=', 1)[1].split(','))
CHECK = '--check' in sys.argv
if CHECK:
    import tempfile
    ROOT = tempfile.mkdtemp(prefix='tx-gen2-check-')

# ---------------- callee registry: fNNN/symbol -> (name, kind, sig) ----------
# kind: product (landing now or landed) | forwarder (engine_context.h) | nested
PROD = {
 'f200': ('CompareBytes', 'uint32_t', ['ctx','lhs','rhs','n']),
 'f201': ('AllocateStringBuffer', 'uint32_t', ['ctx','bytes']),
 'f202': ('CopyBytes', 'uint32_t', ['ctx','dst','src','n']),
 'f205': ('FillBytes', 'uint32_t', ['ctx','dst','value','length']),
 'f207': ('StringLength', 'uint32_t', ['ctx','text']),
 'f209': ('SetStackTop', 'void', ['ctx','stack','index']),
 'f204': ('ReleaseNodeTree', 'void', ['ctx','state','node']),
 'f218': ('RaiseAllocLimitError', 'void', ['ctx','state']),
 'f219': ('FormatPushString', 'uint32_t', ['ctx','state','format','args']),
 'f220': ('SkipWhitespace', 'uint32_t', ['ctx','text']),
 'f221': ('PrepareStringBuffer', 'uint32_t', ['ctx','buffer','size']),
 'f223': ('ReallocateBlock', 'uint32_t', ['ctx','state','block','old_size','new_size']),
 'f226': ('SetStringFromSource', 'uint32_t', ['ctx','text','source']),
 'f243': ('RaiseSyntaxError', 'void', ['ctx','lex_state','message']),
 'f247': ('InternString', 'uint32_t', ['ctx','state','bytes','length']),
 'f248': ('RaiseStringLengthError', 'void', ['ctx','text']),
 'f250': ('RaiseBlockTooBig', 'uint32_t', ['ctx','state']),
 'f253': ('RaiseAllocatorAssert', 'void', ['ctx','state']),
 'f265': ('AppendStringText', 'uint32_t', ['ctx','text','source']),
 'f285': ('NextCharSlow', 'uint32_t', ['ctx','reader']),
 'f289': ('GrowValueStack', 'void', ['ctx','a','b']),
 'f303': ('ClearFlagsRecursive', 'void', ['ctx','state','node']),
 'f313': ('RaiseScriptError', 'void', ['ctx','state','message','argument']),
 'f329': ('PollEventKind', 'uint32_t', ['ctx']),
 'f333': ('FetchObjectMetamethod', 'uint32_t', ['ctx','a','b','c']),
 'f340': ('EmitInstruction', 'uint32_t', ['ctx','func_state','instruction','line']),
 'f3544': ('RunEventStep', 'void', ['ctx','state']),
 'f364': ('ExpressionToRegister', 'uint32_t', ['ctx','func_state','expr']),
 'f372': ('CallValue', 'void', ['ctx','a','b','c']),
 'f399': ('ReallocateRawBlock', 'uint32_t', ['ctx','block','size']),
 'f414': ('EmitOpInstruction', 'uint32_t', ['ctx','func_state','op','arg_a','arg_b','line']),
 'f4180': ('UnorderedUpdateLoadFactor', 'void', ['ctx','map','count']),
 'f428': ('DischargeExpression', 'void', ['ctx','func_state','expr']),
 'f4579': ('AllocateGuardedBuffer', 'void', ['ctx','state','size']),
 'f463': ('CopyIfNonEmpty', 'void', ['ctx','dst','src','n']),
 'f476': ('EmitConditionJump', 'uint32_t', ['ctx','func_state','condition']),
 'f517': ('FetchMetamethod', 'uint32_t', ['ctx','a','b','c']),
 'f518': ('StoreListEntry', 'void', ['ctx','list','index','value']),
 'f523': ('CopyStringBytes', 'void', ['ctx','text','bytes','length']),
 'f560': ('DeallocateStringBuffer', 'void', ['ctx','text','old_data','old_cap']),
 'f592': ('UnorderedFindEntry', 'uint32_t', ['ctx','map','hash','key','out']),
 'f5721': ('ResizeRawBlock', 'uint32_t', ['ctx','state','block','old_size','new_size']),
 'f635': ('UnorderedRehash', 'void', ['ctx','map','buckets']),
 'f645': ('EncodeConstant', 'uint32_t', ['ctx','func_state','value']),
 'f704': ('GrowArray', 'uint32_t', ['ctx','state','data','capacity_addr','element_size','limit','message']),
 'f707': ('AppendFunctionCode', 'void', ['ctx','func_state','code','line']),
 'f708': ('AddFunctionConstant', 'uint32_t', ['ctx','func_state','key','value']),
 'f778': ('GetTableFieldByString', 'uint32_t', ['ctx','state','table','key']),
 'f805': ('GrowStringAndReplace', 'void', ['ctx','arg0','arg1','arg2','arg3','arg4','arg5','arg6','arg7']),
 'f896': ('IndexTableValue', 'void', ['ctx','state','table','key','result']),
 'f902': ('InclineNumber', 'void', ['ctx','lex_state']),
 'f907': ('RaiseTypeError', 'void', ['ctx','a','b','c']),
 'f910': ('ExpressionToOperand', 'void', ['ctx','func_state','expr','operand']),
 'f970': ('AppendTextBytes', 'void', ['ctx','text','bytes','length']),
 'f1071': ('UnorderedPrepareSlot', 'uint32_t', ['ctx','map','key','hash']),
 'f1111': ('ResolveTableField', 'uint32_t', ['ctx','state','table','key']),
 'f1116': ('AppendWithLimit', 'uint32_t', ['ctx','list','entry','limit','what']),
 'f1233': ('SetStringBytes', 'uint32_t', ['ctx','text','bytes','length']),
 'f1550': ('UnorderedCheckLoadFactor', 'void', ['ctx','map','count']),
 'f2207': ('AllocateTaggedBlock', 'uint32_t', ['ctx','header','size']),
 'f2801': ('PumpEventQueue', 'void', ['ctx']),
 'f3551': ('AutoReadTimer::Dispatch', 'void', ['ctx','p0','p1']),
 'f3784': ('UnorderedInsert', 'void', ['ctx','map','key','value']),
 'f1042': ('BacklogView::DispatchAux', 'void', ['ctx','p0','p1']),
 'Nc_0': ('AllocateRawBlock', 'uint32_t', ['ctx','bytes']),
 'Oc_0': ('DeallocateRawBlock', 'void', ['ctx','block']),
}
PROD.update({
 'f199': ('Panic', 'void', ['ctx','code']),
 'f236': ('ThrowScriptError', 'void', ['ctx','state','code']),
 'f397': ('LexError', 'void', ['ctx','lex_state','message','token']),
 'f901': ('SkipSep', 'uint32_t', ['ctx','lex_state']),
 'f1726': ('ReadLongString', 'void', ['ctx','lex_state','seminfo','sep']),
 'f5594': ('ReadNumeral', 'void', ['ctx','lex_state','seminfo']),
 'f312': ('FormatSourceId', 'void', ['ctx','dst','src','length']),
 'f363': ('GrowTableSlot', 'void', ['ctx','state']),
 # Batch-2 R0 forwarder names (see rewrite-names.tsv depth=R0 rows).
 'f244': ('StepRecordResume', 'void', ['ctx','record']),
 'f310': ('StoreGlobalEntry', 'void', ['ctx','globals','index','value']),
 'f347': ('FormatCharValue', 'uint32_t', ['ctx','state','value']),
 'f427': ('CheckExprComplexity', 'void', ['ctx','func_state','message']),
 'f547': ('CheckClosingMatch', 'void', ['ctx','lex_state','message','opening','line']),
 'f782': ('LoadProtoSlot', 'uint32_t', ['ctx','slot']),
 'f1114': ('ResizeTableArray', 'void', ['ctx','state','capacity']),
 'f1115': ('ExpectToken', 'void', ['ctx','lex_state','message']),
 'f1118': ('ParseParamList', 'void', ['ctx','func_state','lex_state','message','what']),
 'f1713': ('MatchKeyword', 'void', ['ctx','lex_state','keyword']),
 'f1714': ('ParseArguments', 'void', ['ctx','func_state','expr']),
 'f1717': ('CompileMainBody', 'void', ['ctx','func_state','message']),
 'f1731': ('ClearLocalFlags', 'void', ['ctx','state']),
 'f1732': ('ClearUpvalueFlags', 'void', ['ctx','state']),
 'f1734': ('InvokeMetaField', 'void', ['ctx','call']),
 'f1745': ('EmitMoveSlot', 'void', ['ctx','func_state','dest','source']),
 'f5671': ('EmitExpression', 'void', ['ctx','func_state','expr','opcode','message']),
 'f5672': ('EmitCondition', 'void', ['ctx','func_state','condition','message']),
 'f5673': ('EmitStatement', 'void', ['ctx','func_state','statement','message']),
 'f5675': ('EmitJumpIf', 'void', ['ctx','func_state','condition','message']),
 # Batch-2 product names.
 'f707': ('CompileExpression', 'void', ['ctx','func_state','code','line']),
 'f1119': ('CheckMatch', 'void', ['ctx','lex_state','expected']),
 'f346': ('CheckStatementLimits', 'uint32_t', ['ctx','state','func_state','message']),
 'f1125': ('CloseFunctionScope', 'uint32_t', ['ctx','state']),
 'f4082': ('CleanupPendingResources', 'void', ['ctx','p0','p1','p2','p3','p4']),
 # Landed batch-1/2 products the batch-4 bodies still call by generated name.
 'f206': ('PushTableField', 'void', ['ctx','state','index','key']),
 'f326': ('SetTableField', 'void', ['ctx','state','index','key']),
 'f777': ('GetIndexedValue', 'void', ['ctx','state','target','key','out_record']),
 # Batch-4 group A products (landing in this batch).
 'f215': ('UnlinkListNode', 'void', ['ctx','node','victim']),
 'f264': ('ResolveExprValue', 'uint32_t', ['ctx','state','tag']),
 'f341': ('LoadExprValue', 'void', ['ctx','state','tag']),
 'f401': ('ShiftWideDigits', 'void', ['ctx','dst','hi','lo','bytes']),
 'f493': ('CompareNumericLiteral', 'uint32_t', ['ctx','state','lhs','rhs']),
 'f603': ('RawValueEqual', 'uint32_t', ['ctx','a','b']),
 'f604': ('RelocateObjectHeader', 'void', ['ctx','state','source','tag']),
 'f611': ('ReadLiveNodePayload', 'uint32_t', ['ctx','node']),
 'f1120': ('BucketLog2', 'uint32_t', ['ctx','value']),
 'f1493': ('ClearIntegerConstant', 'uint32_t', ['ctx','slot','tag']),
 'f1702': ('CompileComparison', 'uint32_t', ['ctx','func_state','lhs','rhs']),
 'f1709': ('InitializeTableArray', 'void', ['ctx','func_state','table','size']),
 'f3514': ('DispatchClassMethod', 'uint32_t', ['ctx','state','key']),
 'f4230': ('SweepMarkedNodeList', 'void', ['ctx','pool']),
 'f4240': ('SweepSecondaryNodeList', 'void', ['ctx','pool']),
 'f4715': ('SyncPropertyVector', 'void', ['ctx','state','index','scale','flag']),
 'f4881': ('InvokeNodeHandler', 'void', ['ctx','node','slot','scale']),
 'f4882': ('InvokeSiblingNodeHandler', 'void', ['ctx','node','slot','scale']),
 'f5511': ('NextTableEntry', 'uint32_t', ['ctx','state','table','key']),
 'f5726': ('SetTableArrayElement', 'void', ['ctx','state','table','index']),
 'f5749': ('CompareNodePayloads', 'uint32_t', ['ctx','a','b']),
 # Batch-4 group B products.
 'f298': ('CheckArgumentType', 'void', ['ctx','state','index','tag']),
 'f304': ('FormatArgumentError', 'uint32_t', ['ctx','state','code','detail']),
 'f783': ('ReshapeTaggedNodeChain', 'uint32_t', ['ctx', 'state', 'node', 'tag']),
 'f1486': ('FindFreeBlockOnChain', 'uint32_t', ['ctx','pool','node','size']),
 'f1487': ('CoalesceFreeBlockRange', 'uint32_t', ['ctx', 'pool', 'block', 'size', 'node', 'limit']),
 'f3516': ('ReadInlineFlagBit3', 'uint32_t', ['ctx','state','index']),
 'f3804': ('AcquireBlockFromFreeList', 'uint32_t', ['ctx', 'pool', 'node', 'size', 'header']),
 'f5716': ('StepInvokeValueEntry', 'uint32_t', ['ctx','state']),
 'f262': ('FlushOutputRoomIfFull', 'uint32_t', ['ctx', 'state', 'limit', 'chunk']),
 'f309': ('FindPoolEntryByTagByte', 'uint32_t', ['ctx','pool','key']),
 'f377': ('InvokeCallbackWhenFlagUnset', 'void', ['ctx','state','target','callback']),
 'f605': ('ConcatJumpLists', 'uint32_t', ['ctx', 'state']),
 'f646': ('ReserveRegisters', 'void', ['ctx', 'state', 'count']),
 'f909': ('EmitVariableStore', 'void', ['ctx', 'state', 'variable']),
 'f911': ('AllocateExpressionRegister', 'void', ['ctx', 'state', 'instruction', 'expression']),
 'f912': ('PatchComparisonIntoJump', 'void', ['ctx', 'state', 'instruction', 'expression']),
 'f913': ('EmitInstructionABC', 'uint32_t', ['ctx', 'state', 'opcode', 'arg_a', 'arg_c']),
 'f1117': ('ReleaseExpressionSlots', 'void', ['ctx', 'state', 'instruction', 'count', 'unused']),
 'f1132': ('EmitVariableLoad', 'void', ['ctx', 'state', 'variable', 'register_hint']),
 'f1718': ('ParseExpression', 'void', ['ctx', 'state', 'token', 'slot', 'depth', 'flag']),
 'f1719': ('ParseAssignment', 'void', ['ctx', 'state', 'target_slot', 'value_slot']),
 'f1720': ('ParseSimpleAssignment', 'uint32_t', ['ctx', 'state']),
 'f1746': ('EmitJumpInstruction', 'void', ['ctx', 'state', 'register_hint', 'target']),
 'f5678': ('AddOneConstant', 'uint32_t', ['ctx', 'state']),
 'f5679': ('ReserveThreeRegisters', 'void', ['ctx', 'state']),
 'f5509': ('ReshapePowerOfTwoNode', 'void', ['ctx','state','node']),
 'f5536': ('ReshapeCountedArrayNode', 'void', ['ctx','state','node']),
 'f5628': ('ReshapeHeaderVariantNode', 'void', ['ctx','state','node']),
 'f5629': ('ReshapeFieldRecordNode', 'void', ['ctx','state','node']),
 'f5630': ('ReshapeInlinePairNode', 'void', ['ctx','state','node']),
 # Batch-4 group B: five group-external callees that already own a landed
 # product, so only the generator's name table was missing.
 'f203': ('ResizeValueStack', 'void', ['ctx','state','depth']),
 'f210': ('QueryValueType', 'uint32_t', ['ctx','state','index']),
 'f643': ('FinishTableIndex', 'uint32_t', ['ctx','state','table','key']),
 'f779': ('LookupTableField', 'uint32_t', ['ctx','table','key']),
 'f898': ('GetTableFieldByNumber', 'uint32_t', ['ctx','state','table','key']),
 # ---- batch-4B registration: the 19 items that still owed a name row ----
  'f255': ('AssignSharedOwner', 'void', ['ctx', 'owner', 'value', 'shared']),
  'f382': ('ResolveVariableSlot', 'uint32_t', ['ctx', 'state', 'variable']),
  'f442': ('DeclareLocalVariable', 'void', ['ctx', 'state', 'name', 'slot']),
  'f450': ('RelocateElementArray', 'void', ['ctx', 'state', 'table']),
  'f451': ('PatchJumpChain', 'void', ['ctx', 'state', 'instruction']),
  'f453': ('ConvertValueToInteger', 'uint32_t', ['ctx', 'state', 'slot']),
  'f521': ('AdvanceTableIterator', 'uint32_t', ['ctx', 'state', 'table']),
  'f548': ('BindUpvalueCell', 'uint32_t', ['ctx', 'state', 'local', 'upvalue']),
  'f781': ('CloseBlockScope', 'void', ['ctx', 'state']),
  'f784': ('FixPendingJumpTargets', 'void', ['ctx', 'state', 'instruction', 'target']),
  'f885': ('CreateSharedRecord', 'void', ['ctx', 'state', 'value']),
  'f897': ('MaterializeNumberValue', 'uint32_t', ['ctx', 'state', 'slot']),
  'f899': ('CompileStatement', 'uint32_t', ['ctx', 'state']),
  'f1112': ('LookupOrCreateTableField', 'void', ['ctx', 'state', 'table', 'key', 'value']),
  'f1128': ('CompactEntryQueue', 'uint32_t', ['ctx', 'state', 'queue']),
  'f1212': ('ReclaimFreeBlock', 'void', ['ctx', 'state', 'block']),
  'f1701': ('CompileOperation', 'void', ['ctx', 'state', 'expression']),
  'f1715': ('RaiseItemLimitError', 'void', ['ctx', 'state', 'limit']),
  'f1739': ('ReserveClosureRecords', 'void', ['ctx', 'state', 'count']),
 # ---- batch-4B external registration: 26 callees that only had a name in a naming table ----
  'f211': ('CompareNulTerminatedStrings', 'uint32_t', ['ctx', 'left', 'right']),
  'f238': ('RaiseTraceback', 'uint32_t', ['ctx', 'value', 'message', 'tag']),
  'f246': ('CombineComparisonResults', 'uint32_t', ['ctx', 'left', 'right', 'result', 'box']),
  'f371': ('CoerceToNumber', 'uint32_t', ['ctx', 'state', 'value']),
  'f373': ('TrimLocalSlotsAboveLevel', 'uint32_t', ['ctx', 'proto', 'level', 'result']),
  'f435': ('ComputeNumericPower', 'double', ['ctx', 'base', 'exponent']),
  'f519': ('FormatTracebackFrame', 'uint32_t', ['ctx', 'state', 'name', 'out']),
  'f520': ('ResolveBranchOffset', 'uint32_t', ['ctx', 'state', 'opcode', 'operands', 'extra', 'target']),
  'f549': ('ReleaseNodeChain', 'void', ['ctx', 'state', 'chain']),
  'f601': ('AllocateTable', 'uint32_t', ['ctx', 'state', 'entries', 'array']),
  'f644': ('RaiseArithmeticError', 'void', ['ctx', 'state', 'value', 'meta']),
  'f837': ('CompareScriptStrings', 'uint32_t', ['ctx', 'left', 'right']),
  'f900': ('FormatNumber', 'uint32_t', ['ctx', 'state', 'value', 'options']),
  'f962': ('ReadInputLine', 'uint32_t', ['ctx', 'state', 'stream', 'line']),
  'f1005': ('FindStringLength', 'uint32_t', ['ctx', 'value', 'literal']),
  'f1110': ('FormatNumberValue', 'void', ['ctx', 'state', 'value', 'options']),
  'f1700': ('DeclareUpvalue', 'void', ['ctx', 'state', 'local']),
  'f1703': ('CompareConstantOperands', 'uint32_t', ['ctx', 'state', 'left', 'right']),
  'f1704': ('NumberToString', 'uint32_t', ['ctx', 'value', 'options']),
  'f1707': ('FindNumericKeySlot', 'uint32_t', ['ctx', 'state']),
  'f1735': ('ReserveIntVector', 'uint32_t', ['ctx', 'state', 'vector', 'size']),
  'f1741': ('CheckComparableTypes', 'uint32_t', ['ctx', 'state', 'left', 'right']),
  'f5510': ('StoreTableField', 'void', ['ctx', 'state', 'table', 'key']),
  'f5631': ('AdjustNodeCapacity', 'uint32_t', ['ctx', 'state', 'node']),
  'f5677': ('PatchInstructionTarget', 'void', ['ctx', 'state', 'pending']),
  'f5680': ('EmitBranchWord', 'void', ['ctx', 'state', 'opcode', 'operands']),
  'f242': ('FindOrderedChainEntry', 'uint32_t', ['ctx', 'var_p0', 'var_p1']),
  'f263': ('InternKeyIntoTableSlot', 'void', ['ctx', 'var_p0', 'var_p1', 'var_p2']),
  'f268': ('MoveMemoryRange', 'uint32_t', ['ctx', 'var_p0', 'var_p1', 'var_p2']),
  'f272': ('UnorderedLinkBucketNode', 'void', ['ctx', 'var_p0', 'var_p1', 'var_p2', 'var_p3', 'var_p4', 'var_p5']),
  'f281': ('ReleaseLayerNodeTree', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f283': ('InitTaggedNodeDescriptor', 'uint32_t', ['ctx', 'var_p0']),
  'f293': ('RaiseNodeInitError', 'uint32_t', ['ctx', 'var_p0', 'var_p1']),
  'f302': ('StoreSuspendableEntry', 'void', ['ctx', 'var_p0', 'var_p1', 'var_p2']),
  'f325': ('InsertFormattedKeyEntry', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f334': ('StepCallValueFrame', 'uint32_t', ['ctx', 'var_p0']),
  'f351': ('MatchCStringPrefix', 'uint32_t', ['ctx', 'var_p0', 'var_p1', 'var_p2']),
  'f376': ('ReadPackedI64Value', 'uint32_t', ['ctx', 'var_p0', 'var_p1', 'var_p2']),
  'f390': ('RebuildUnorderedBucketArray', 'uint32_t', ['ctx', 'var_p0', 'var_p1']),
  'f391': ('ReleaseNoSaveNodeTree', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f392': ('FormatLuaEventFilterLine', 'uint32_t', ['ctx', 'var_p0', 'var_p1']),
  'f413': ('UnorderedInsertBucketNode', 'void', ['ctx', 'var_p0', 'var_p1', 'var_p2', 'var_p3', 'var_p4', 'var_p5']),
  'f422': ('ReleaseNestedNodePair', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f437': ('DestroyNodeObject', 'uint32_t', ['ctx', 'var_p0']),
  'f438': ('ReleaseNodeListTail', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f471': ('UnlinkOrderedChainNode', 'void', ['ctx', 'var_p0']),
  'f479': ('EncodeSignedOrderAsDouble', 'double', ['ctx', 'var_p0', 'var_p1']),
  'f490': ('ResetEventFilterState', 'void', ['ctx', 'var_p0']),
  'f491': ('ReleaseShallowNodePair', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f532': ('ClearLinkedNodeFields', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f533': ('PersistSessionState', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f534': ('ReleaseTaggedNodeTriple', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f550': ('CompileNumericPower', 'void', ['ctx', 'var_p0', 'var_p1', 'var_p2', 'var_p3']),
  'f582': ('InitNodeStorage', 'void', ['ctx', 'var_p0']),
  'f583': ('ReleaseTaggedChildNodes', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f671': ('MeasureFormattedNumber', 'uint32_t', ['ctx', 'var_p0', 'var_p1', 'var_p2']),
  'f678': ('ReleaseAssertedNodeTree', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f679': ('ReleaseSubtreeTaggedNodes', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f680': ('ReleasePlainNodeTree', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f681': ('FormatLuaTagFilterLine', 'void', ['ctx', 'var_p0', 'var_p1', 'var_p2', 'var_p3']),
  'f682': ('ReleaseNestedTaggedNodes', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f742': ('ApplyCommandSkipStatus', 'void', ['ctx', 'var_p0', 'var_p1', 'var_p2']),
  'f744': ('ApplyAutoModeStatus', 'void', ['ctx', 'var_p0', 'var_p1', 'var_p2']),
  'f761': ('ResetGlobalSlotTable', 'void', ['ctx', 'var_p0']),
  'f763': ('ClearStructFields', 'uint32_t', ['ctx', 'var_p0']),
  'f776': ('ReallocateFormatBuffer', 'uint32_t', ['ctx', 'var_p0', 'var_p1', 'var_p2']),
  'f829': ('TerminateStringAt', 'uint32_t', ['ctx', 'var_p0', 'var_p1']),
  'f839': ('ReleaseSharedObjectHolder', 'void', ['ctx', 'var_p0']),
  'f860': ('InitLayerStorage', 'void', ['ctx', 'var_p0']),
  'f908': ('ResolveVariableKind', 'uint32_t', ['ctx', 'var_p0', 'var_p1', 'var_p2', 'var_p3']),
  'f1004': ('PadStringBuffer', 'uint32_t', ['ctx', 'var_p0', 'var_p1', 'var_p2']),
  'f1006': ('ReadNumericFieldAsDouble', 'double', ['ctx', 'var_p0', 'var_p1']),
  'f1032': ('ReleaseChainedResourceNodes', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f1033': ('ReleaseChainedNodeBranches', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f1038': ('ApplyControlSkipStatus', 'void', ['ctx', 'var_p0', 'var_p1', 'var_p2']),
  'f1043': ('InvokeVirtualSlot', 'uint32_t', ['ctx', 'var_p0']),
  'f1129': ('RelocateUpvalueCells', 'void', ['ctx', 'var_p0', 'var_p1', 'var_p2']),
  'f1131': ('EmitPrototypePrologue', 'void', ['ctx', 'var_p0', 'var_p1', 'var_p2', 'var_p3']),
  'f1210': ('ResetInputLineState', 'uint32_t', ['ctx', 'var_p0']),
  'f1218': ('ResetSuspendableFrame', 'void', ['ctx']),
  'f1325': ('ScanBitMaskWord', 'uint32_t', ['ctx', 'var_p0', 'var_p1']),
  'f1408': ('ReleaseLayerRenderData', 'void', ['ctx', 'var_p0']),
  'f1457': ('UnorderedRebuildBucketChain', 'void', ['ctx', 'var_p0', 'var_p1', 'var_p2', 'var_p3']),
  'f1458': ('ApplyAvoidStatus', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f1459': ('RebucketUnorderedNodeArray', 'uint32_t', ['ctx', 'var_p0']),
  'f1463': ('AdvanceGainStatStage', 'void', ['ctx', 'var_p0']),
  'f1471': ('UnorderedInsertCheckedBucketNode', 'void', ['ctx', 'var_p0', 'var_p1', 'var_p2', 'var_p3', 'var_p4', 'var_p5']),
  'f1520': ('UnorderedReleaseBucketTable', 'void', ['ctx', 'var_p0']),
  'f1531': ('ReleaseNodeSlotArray', 'void', ['ctx', 'var_p0']),
  'f1708': ('LookupMaskedMetamethodSlot', 'uint32_t', ['ctx', 'var_p0', 'var_p1']),
  'f1721': ('PatchUpvalueJumpTarget', 'void', ['ctx', 'var_p0']),
  'f1723': ('MaskToBitWidth', 'uint32_t', ['ctx', 'var_p0']),
  'f1742': ('InvokeCallValueWithStack', 'void', ['ctx', 'var_p0']),
  'f2305': ('ReleaseStringBufferPending', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f2729': ('CopyTextBytes', 'uint32_t', ['ctx', 'var_p0', 'var_p1', 'var_p2']),
  'f2890': ('AdvanceEngineFrameTasks', 'void', ['ctx', 'var_p0']),
  'f2892': ('ReleaseEngineFrameResources', 'void', ['ctx', 'var_p0']),
  'f3192': ('AdvanceMessageEffectTimer', 'uint32_t', ['ctx', 'var_p0', 'var_p1', 'var_p2']),
  'f3294': ('AdvanceHttpFetchState', 'uint32_t', ['ctx', 'var_p0']),
  'f3872': ('PauseFullscreenVideo', 'void', ['ctx', 'var_p0']),
  'f3933': ('RebuildUnorderedIndexTable', 'void', ['ctx', 'var_p0']),
  'f5538': ('PrepareParameterFrame', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f5624': ('ResetNodeChainFlags', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f5645': ('FormatConcatenationError', 'void', ['ctx', 'var_p0', 'var_p1', 'var_p2']),
  'f5676': ('CompileKeywordCondition', 'void', ['ctx', 'var_p0', 'var_p1']),
  'f5733': ('FormatTracebackSlotEntry', 'void', ['ctx', 'var_p0', 'var_p1', 'var_p2']),
  'f904': ('Call', 'uint32_t', ['ctx', 'var_p0', 'var_p1', 'var_p2']),
 # ---- batch 5: dispatch units that were still only plan rows ----
 'f3661': ('Compositor::LyEventDispatch', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f3112': ('LuaEngine::VarDispatch', 'uint32_t', ['ctx', 'p0', 'p1']),
 'f3666': ('EngineMode::Dispatch', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f3655': ('Compositor::TweenDispatch', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f3219': ('MessageLayer::QueuePrintText', 'void', ['ctx', 'p0', 'p1']),
 'f5596': ('LuaKeywords::Init', 'void', ['ctx', 'p0']),
 # ---- batch 6: the container-release closure of LuaEngine::TagDispatch (f3744) ----
 'f746': ('ReleaseLinkedNodeTree', 'void', ['ctx', 'p0', 'p1']),
 'f747': ('ReleaseHashedNodeTable', 'void', ['ctx', 'p0', 'p1']),
 'f3403': ('UnorderedGetBucket', 'void', ['ctx', 'p0']),
 'f861': ('ReleaseNestedNodeChain', 'void', ['ctx', 'p0', 'p1']),
 'f1029': ('ResumeExceptionCleanup', 'void', ['ctx', 'p0', 'p1']),
 'f3744': ('LuaEngine::TagDispatch', 'uint32_t', ['ctx', 'p0', 'p1']),
 # ---- batch 7: the plan rows that were already satisfiable (2026-10-01) ----
 'f1033': ('ReleaseChainedNodeBranches', 'void', ['ctx', 'p0', 'p1']),
 'f1032': ('ReleaseChainedResourceNodes', 'void', ['ctx', 'p0', 'p1']),
 'f390': ('RebuildUnorderedBucketArray', 'uint32_t', ['ctx', 'p0', 'p1']),
 'f1459': ('RebucketUnorderedNodeArray', 'uint32_t', ['ctx', 'p0']),
 'f1457': ('UnorderedRebuildBucketChain', 'void', ['ctx', 'p0', 'p1', 'p2', 'p3']),
 'f413': ('UnorderedInsertBucketNode', 'void', ['ctx', 'p0', 'p1', 'p2', 'p3', 'p4', 'p5']),
 'f1471': ('UnorderedInsertCheckedBucketNode', 'void', ['ctx', 'p0', 'p1', 'p2', 'p3', 'p4', 'p5']),
 'f272': ('UnorderedLinkBucketNode', 'void', ['ctx', 'p0', 'p1', 'p2', 'p3', 'p4', 'p5']),
 'f1520': ('UnorderedReleaseBucketTable', 'void', ['ctx', 'p0']),
 'f678': ('ReleaseAssertedNodeTree', 'void', ['ctx', 'p0', 'p1']),
 'f680': ('ReleasePlainNodeTree', 'void', ['ctx', 'p0', 'p1']),
 'f682': ('ReleaseNestedTaggedNodes', 'void', ['ctx', 'p0', 'p1']),
 'f391': ('ReleaseNoSaveNodeTree', 'void', ['ctx', 'p0', 'p1']),
 'f422': ('ReleaseNestedNodePair', 'void', ['ctx', 'p0', 'p1']),
 'f491': ('ReleaseShallowNodePair', 'void', ['ctx', 'p0', 'p1']),
 'f534': ('ReleaseTaggedNodeTriple', 'void', ['ctx', 'p0', 'p1']),
 'f583': ('ReleaseTaggedChildNodes', 'void', ['ctx', 'p0', 'p1']),
 'f1531': ('ReleaseNodeSlotArray', 'void', ['ctx', 'p0']),
 'f2305': ('ReleaseStringBufferPending', 'void', ['ctx', 'p0', 'p1']),
 'f532': ('ClearLinkedNodeFields', 'void', ['ctx', 'p0', 'p1']),
 'f437': ('DestroyNodeObject', 'uint32_t', ['ctx', 'p0']),
 'f242': ('FindOrderedChainEntry', 'uint32_t', ['ctx', 'p0', 'p1']),
 'f471': ('UnlinkOrderedChainNode', 'void', ['ctx', 'p0']),
 'f268': ('MoveMemoryRange', 'uint32_t', ['ctx', 'p0', 'p1', 'p2']),
 'f860': ('InitLayerStorage', 'void', ['ctx', 'p0']),
 'f1408': ('ReleaseLayerRenderData', 'void', ['ctx', 'p0']),
 'f281': ('ReleaseLayerNodeTree', 'void', ['ctx', 'p0', 'p1']),
 'f2892': ('ReleaseEngineFrameResources', 'void', ['ctx', 'p0']),
 'f839': ('ReleaseSharedObjectHolder', 'void', ['ctx', 'p0']),
 'f243': ('RaiseSyntaxError', 'void', ['ctx', 'p0', 'p1']),
 'f313': ('RaiseScriptError', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f907': ('RaiseTypeError', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f248': ('RaiseStringLengthError', 'void', ['ctx', 'p0']),
 'f250': ('RaiseBlockTooBig', 'uint32_t', ['ctx', 'p0']),
 'f293': ('RaiseNodeInitError', 'uint32_t', ['ctx', 'p0', 'p1']),
 'f560': ('DeallocateStringBuffer', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f351': ('MatchCStringPrefix', 'uint32_t', ['ctx', 'p0', 'p1', 'p2']),
 'f1129': ('RelocateUpvalueCells', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f3192': ('AdvanceMessageEffectTimer', 'uint32_t', ['ctx', 'p0', 'p1', 'p2']),
 'f744': ('ApplyAutoModeStatus', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f742': ('ApplyCommandSkipStatus', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f1038': ('ApplyControlSkipStatus', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f372': ('CallValue', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f547': ('CheckClosingMatch', 'void', ['ctx', 'p0', 'p1', 'p2', 'p3']),
 'f427': ('CheckExprComplexity', 'void', ['ctx', 'p0', 'p1']),
 'f550': ('CompileNumericPower', 'void', ['ctx', 'p0', 'p1', 'p2', 'p3']),
 'f5676': ('CompileKeywordCondition', 'void', ['ctx', 'p0', 'p1']),
 'f2729': ('CopyTextBytes', 'uint32_t', ['ctx', 'p0', 'p1', 'p2']),
 'f5671': ('EmitExpression', 'void', ['ctx', 'p0', 'p1', 'p2', 'p3']),
 'f5672': ('EmitCondition', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f5675': ('EmitJumpIf', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f5673': ('EmitStatement', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f1745': ('EmitMoveSlot', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f1131': ('EmitPrototypePrologue', 'void', ['ctx', 'p0', 'p1', 'p2', 'p3']),
 'f479': ('EncodeSignedOrderAsDouble', 'double', ['ctx', 'p0', 'p1']),
 'f1115': ('ExpectToken', 'void', ['ctx', 'p0', 'p1']),
 'f517': ('FetchMetamethod', 'uint32_t', ['ctx', 'p0', 'p1', 'p2']),
 'f333': ('FetchObjectMetamethod', 'uint32_t', ['ctx', 'p0', 'p1', 'p2']),
 'f347': ('FormatCharValue', 'uint32_t', ['ctx', 'p0', 'p1']),
 'f5645': ('FormatConcatenationError', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f5733': ('FormatTracebackSlotEntry', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f704': ('GrowArray', 'uint32_t', ['ctx', 'p0', 'p1', 'p2', 'p3', 'p4', 'p5']),
 'f289': ('GrowValueStack', 'void', ['ctx', 'p0', 'p1']),
 'f325': ('InsertFormattedKeyEntry', 'void', ['ctx', 'p0', 'p1']),
 'f263': ('InternKeyIntoTableSlot', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f1742': ('InvokeCallValueWithStack', 'void', ['ctx', 'p0']),
 'f1734': ('InvokeMetaField', 'void', ['ctx', 'p0']),
 'f782': ('LoadProtoSlot', 'uint32_t', ['ctx', 'p0']),
 'f1708': ('LookupMaskedMetamethodSlot', 'uint32_t', ['ctx', 'p0', 'p1']),
 'f1723': ('MaskToBitWidth', 'uint32_t', ['ctx', 'p0']),
 'f1713': ('MatchKeyword', 'void', ['ctx', 'p0', 'p1']),
 'f1714': ('ParseArguments', 'void', ['ctx', 'p0', 'p1']),
 'f1118': ('ParseParamList', 'void', ['ctx', 'p0', 'p1', 'p2', 'p3']),
 'f1721': ('PatchUpvalueJumpTarget', 'void', ['ctx', 'p0']),
 'f1114': ('ResizeTableArray', 'void', ['ctx', 'p0', 'p1']),
 'f1325': ('ScanBitMaskWord', 'uint32_t', ['ctx', 'p0', 'p1']),
 'f334': ('StepCallValueFrame', 'uint32_t', ['ctx', 'p0']),
 'f1731': ('ClearLocalFlags', 'void', ['ctx', 'p0']),
 'f1732': ('ClearUpvalueFlags', 'void', ['ctx', 'p0']),
 'f5624': ('ResetNodeChainFlags', 'void', ['ctx', 'p0', 'p1']),
 'f763': ('ClearStructFields', 'uint32_t', ['ctx', 'p0']),
 'f761': ('ResetGlobalSlotTable', 'void', ['ctx', 'p0']),
 'f1210': ('ResetInputLineState', 'uint32_t', ['ctx', 'p0']),
 'f490': ('ResetEventFilterState', 'void', ['ctx', 'p0']),
 'f776': ('ReallocateFormatBuffer', 'uint32_t', ['ctx', 'p0', 'p1', 'p2']),
 'f582': ('InitNodeStorage', 'void', ['ctx', 'p0']),
 # ---- batch-8A: the three dispatch products that only needed a naming closure
 # (f859 LuaEngine::Dispatch, f3196 MessageLayer::TextFxDispatch, f3548
 # MessageLayer::RpDispatch) followed by the cross-cutting callees they share.
 'f859': ('LuaEngine::Dispatch', 'uint32_t', ['ctx', 'p0', 'p1']),
 'f3196': ('MessageLayer::TextFxDispatch', 'void', ['ctx', 'p0', 'p1', 'p2', 'p3', 'p4']),
 'f3548': ('MessageLayer::RpDispatch', 'void', ['ctx', 'p0']),
 # ---- batch-8C: the three giants unlocked by the batch-8B naming closure ----
 'f3651': ('LuaEngine::TagGenericSetOn', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f3293': ('FetchManager::HttpDispatch', 'void', ['ctx', 'p0', 'p1']),
 'f3650': ('LuaEngine::TagGenericDelOn', 'void', ['ctx', 'p0', 'p1', 'p2']),
 # ---- batch-9 ----
 'f850': ('FindUnorderedValueSlot', 'uint32_t', ['ctx', 'p0', 'p1']),
 'f1456': ('EmplaceHashedEntry', 'void', ['ctx', 'p0', 'p1', 'p2', 'p3']),
 'f753': ('CopyTaggedTreeKeyToText', 'void', ['ctx', 'p0', 'p1', 'p2', 'p3', 'p4', 'p5']),
 'f579': ('ApplyEventRecordTable', 'void', ['ctx', 'p0']),
 # ---- batch-10: the giants batch-9 unlocked (they already had tsv rows) ----
 'f3551': ('AutoReadTimer::Dispatch', 'void', ['ctx', 'p0', 'p1']),
 'f3698': ('VideoPlayer::Dispatch', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f533': ('PersistSessionState', 'void', ['ctx', 'p0', 'p1']),
 # ---- batch-11 ----
 'f290': ('AppendTableSlot', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f363': ('EnsureTableArrayRoom', 'void', ['ctx', 'p0']),
 'f288': ('MakeStringFromBytes', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f772': ('GetRegistryTableField', 'uint32_t', ['ctx', 'p0', 'p1']),
 # ---- batch-12: the four rows batch-11 unlocked ----
 'f3085': ('PackManager::FileDispatchA', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f3086': ('PackManager::FileDispatchB', 'void', ['ctx', 'p0', 'p1']),
 'f3087': ('PackManager::FileDispatchC', 'void', ['ctx', 'p0', 'p1']),
 'f3088': ('Compositor::LyPropDispatch', 'void', ['ctx', 'p0', 'p1']),
 # ---- batch-13 ----
 'f1072': ('ClearUnorderedBuckets', 'void', ['ctx', 'p0']),
 'f294': ('RethrowPendingException', 'void', ['ctx', 'p0']),
 'f366': ('Fread', 'uint32_t', ['ctx', 'p0', 'p1', 'p2', 'p3']),
 'f3631': ('FetchManager::CommandDispatch', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f1081': ('ResetSharedPtr01', 'void', ['ctx', 'p0', 'p1']),
 'f1406': ('ResetSharedPtr02', 'void', ['ctx', 'p0', 'p1']),
 'f1465': ('ResetSharedPtr03', 'void', ['ctx', 'p0', 'p1']),
 'f2936': ('ResetSharedPtr04', 'void', ['ctx', 'p0', 'p1']),
 'f3387': ('ResetSharedPtr05', 'void', ['ctx', 'p0', 'p1']),
 'f3395': ('ResetSharedPtr06', 'void', ['ctx', 'p0', 'p1']),
 'f3558': ('ResetSharedPtr07', 'void', ['ctx', 'p0', 'p1']),
 'f3626': ('ResetSharedPtr08', 'void', ['ctx', 'p0', 'p1']),
 'f4204': ('ResetSharedPtr09', 'void', ['ctx', 'p0', 'p1']),
 'f4553': ('ResetSharedPtr10', 'void', ['ctx', 'p0', 'p1']),
 'f4569': ('ResetSharedPtr11', 'void', ['ctx', 'p0', 'p1']),
 'f1593': ('ResetSharedArray', 'void', ['ctx', 'p0', 'p1']),
 # ---- batch-16: the f3633 door, take one -- the libc and STL leaves f3633 calls ----
 'f240': ('MemChr', 'uint32_t', ['ctx', 'area', 'byte', 'count']),
 'f420': ('FillWords', 'uint32_t', ['ctx', 'dst', 'value', 'count']),
 'f857': ('MersenneTwisterTwist', 'void', ['ctx', 'state']),
 'f3308': ('ReverseBits32', 'uint32_t', ['ctx', 'value']),
 'f1466': ('InitCrc32Table', 'void', ['ctx']),
 # These six are forwarder-only rows: the generated body keeps the definition,
 # the name is what the products call.  Evidence for each name is in
 # rewrite-names.tsv (no data-segment constants exist in any of these six, so the
 # names come from structure and call sites).
 'f295': ('ReleaseSharedRef', 'void', ['ctx', 'p0']),
 'f4704': ('ReleaseSharedRefChain', 'void', ['ctx', 'p0']),
 'f883': ('DestroyDispatchObject', 'uint32_t', ['ctx', 'p0']),
 'f336': ('FindTaggedTreeEntry', 'void', ['ctx', 'p0', 'p1', 'p2', 'p3', 'p4', 'p5']),
 'f580': ('ClearObjectDeque', 'void', ['ctx', 'p0']),
 'f1409': ('PushObjectDequeElement', 'uint32_t', ['ctx', 'p0', 'p1']),
 # ---- batch-8B: the tagged-tree / buffered-stream / sound-table closure (2026-10-02) ----
 'f284': ('CopyScriptBlock', 'uint32_t', ['ctx', 'p0', 'p1']),
 'f353': ('AssignTaggedTreeRange', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f3799': ('InsertTaggedTreeNode', 'void', ['ctx', 'p0', 'p1']),
 'f330': ('DestroyFileObject', 'uint32_t', ['ctx', 'p0']),
 'f360': ('ReleaseBufferedChainNode', 'uint32_t', ['ctx', 'p0']),
 'f673': ('FlushBufferedChain', 'uint32_t', ['ctx', 'p0']),
 'f1016': ('FlushOutputBuffer', 'uint32_t', ['ctx', 'p0']),
 'f1469': ('FindOrInsertSoundObjectSlot', 'void', ['ctx', 'p0', 'p1']),
 'f1058': ('InsertSoundObjectSlot', 'void', ['ctx', 'p0', 'p1', 'p2']),
 'f584': ('DestroySoundObject', 'uint32_t', ['ctx', 'p0']),
 'f852': ('EraseTaggedTreeEntry', 'uint32_t', ['ctx', 'p0', 'p1']),
 'f440': ('EraseTreeRebalance', 'void', ['ctx', 'p0', 'p1']),
 'f510': ('ReleaseNodeSubtreeA', 'void', ['ctx', 'p0', 'p1']),
 'f585': ('ReleaseNodeSubtreeB', 'void', ['ctx', 'p0', 'p1']),
 'f1527': ('ReleaseMapFieldSubtree', 'void', ['ctx', 'p0']),
 'f2269': ('FindLastByte', 'uint32_t', ['ctx', 'p0', 'p1', 'p2']),
 'f722': ('FindLastCharInString', 'uint32_t', ['ctx', 'p0', 'p1']),
})
NESTED = {'f3551': 'namespace AutoReadTimer {', 'f1042': 'namespace BacklogView {',
          'f904': 'namespace LuaEngine {'}

# ---------------- per-function product meta --------------------------------
METAS = {
 'f265': dict(ret='uint32_t', params=['text','source'], file='util/append_string_text.cpp',
   purpose='AppendStringText -- the suspendable append front. Grows the text by the source string bytes through the checked append worker and answers the updated handle.',
   loc={}, frame=16),
 'f5721': dict(ret='uint32_t', params=['state','block','old_size','new_size'], file='util/resize_raw_block.cpp',
   purpose='ResizeRawBlock -- the resize shim the allocator layer drives: a zero new size deallocates the block, anything else hands it to the raw reallocator.',
   loc={}, frame=12),
 'f910': dict(ret='void', params=['func_state','expr','operand'], file='script/expression_to_operand.cpp',
   purpose='ExpressionToOperand -- discharges one expression into an operand the emitter can fold into an instruction, spilling constants and registers as the expression demands.',
   loc={}, frame=24),
 'f708': dict(ret='uint32_t', params=['func_state','key','value'], file='script/add_function_constant.cpp',
   purpose='AddFunctionConstant -- interns one key/value pair into the function constant table: lookup first, append on miss, and raise on overflow.',
   loc={}, frame=32),
 'f364': dict(ret='uint32_t', params=['func_state','expr'], file='script/expression_to_register.cpp',
   purpose='ExpressionToRegister -- discharges one expression into its next free register, emitting whatever moves the expression shape demands.',
   loc={}, frame=20),
 'f645': dict(ret='uint32_t', params=['func_state','value'], file='script/encode_constant.cpp',
   purpose='EncodeConstant -- maps one value to its constant-table operand, adding the constant when it is not there yet.',
   loc={}, frame=12),
 'f223': dict(ret='uint32_t', params=['state','block','old_size','new_size'], file='util/reallocate_block.cpp',
   purpose='ReallocateBlock -- the guest block reallocator with the block-too-big guard; a zero new size frees.',
   loc={}, frame=0),
 'f476': dict(ret='uint32_t', params=['func_state','condition'], file='script/emit_condition_jump.cpp',
   purpose='EmitConditionJump -- emits the jump for one condition, raising the complexity error when the function grows past its limit. Answers the jump kind it emitted.',
   loc={}, frame=24),
 'f4579': dict(ret='void', params=['state','size'], file='util/allocate_guarded_buffer.cpp',
   purpose='AllocateGuardedBuffer -- allocates one buffer under the allocator guard: the limit raise and the assert path sit behind the raw allocation.',
   loc={}, frame=24),
 'f399': dict(ret='uint32_t', params=['block','size'], file='util/reallocate_raw_block.cpp',
   purpose='ReallocateRawBlock -- reallocates one raw block: a null block allocates fresh, a zero size frees, and the request folds to the aligned size behind the block header.',
   loc={}, frame=8),
 'f1116': dict(ret='uint32_t', params=['list','entry','limit','what'], file='script/append_with_limit.cpp',
   purpose='AppendWithLimit -- appends one entry to a growable parser list under a size limit, raising the named-limit error when the list is full.',
   loc={}, frame=16),
 'f204': dict(ret='void', params=['state','node'], file='util/release_node_tree.cpp',
   purpose='ReleaseNodeTree -- releases a two-child node tree depth-first and frees the owned block behind the tail field when the flag byte marks it owned.',
   loc={}, frame=0),
 'f207': dict(ret='uint32_t', params=['text'], file='util/string_length.cpp',
   purpose='StringLength -- the classic word-at-a-time zero-byte scan: aligned words first, then byte by byte.',
   loc={}, frame=0),
 'f414': dict(ret='uint32_t', params=['func_state','op','arg_a','arg_b','line'], file='script/emit_op_instruction.cpp',
   purpose='EmitOpInstruction -- assembles one instruction word from opcode and operands and hands it to the emitter.',
   loc={}, frame=12),
 'f970': dict(ret='void', params=['text','bytes','length'], file='util/append_text_bytes.cpp',
   purpose='AppendTextBytes -- appends bytes to the text block, growing the buffer first and raising the length error when the text would overflow.',
   loc={}, frame=28),
 'f303': dict(ret='void', params=['state','node'], file='util/clear_flags_recursive.cpp',
   purpose='ClearFlagsRecursive -- clears the low flag bits on one node and walks its typed children, recursing through the branch shapes.',
   loc={}, frame=0),
 'f1233': dict(ret='uint32_t', params=['text','bytes','length'], file='util/set_string_bytes.cpp',
   purpose='SetStringBytes -- replaces the string contents with the given bytes, growing the string block through the replace-and-copy worker.',
   loc={}, frame=32),
 'f592': dict(ret='uint32_t', params=['map','hash','key','out'], file='util/unordered_find_entry.cpp',
   purpose='UnorderedFindEntry -- the unordered-map find: hash mod bucket count, walk the bucket chain comparing keys.',
   loc={}, frame=20),
 'f226': dict(ret='uint32_t', params=['text','source'], file='util/set_string_from_source.cpp',
   purpose='SetStringFromSource -- replaces the string contents from a source string, delegating the grow-and-copy work.',
   loc={}, frame=12),
 'f1550': dict(ret='void', params=['map','count'], file='util/unordered_check_load_factor.cpp',
   purpose='UnorderedCheckLoadFactor -- the unordered-map load-factor guard: recalculates and rehashes when the map crosses its threshold.',
   loc={}, frame=12),
 'f2801': dict(ret='void', params=[], file='script/pump_event_queue.cpp',
   purpose='PumpEventQueue -- pumps one round of pending events through the dispatch steps and answers the round outcome.',
   loc={}, frame=8),
 'f1071': dict(ret='uint32_t', params=['map','key','hash'], file='util/unordered_prepare_slot.cpp',
   purpose='UnorderedPrepareSlot -- prepares an unordered-map slot for insertion, growing the buckets when the load factor demands it.',
   loc={}, frame=16),
 'f3784': dict(ret='void', params=['map','key','value'], file='util/unordered_insert.cpp',
   purpose='UnorderedInsert -- inserts one key/value pair into the unordered map: find, prepare the slot, and grow when the pair is new.',
   loc={}, frame=28),
 'f902': dict(ret='void', params=['lex_state'], file='script/incline_number.cpp',
   purpose='InclineNumber -- bumps the line counter on a newline pair, keeping the lookahead consistent.',
   loc={}, frame=0),
 'f221': dict(ret='uint32_t', params=['buffer','size'], file='util/prepare_string_buffer.cpp',
   purpose='PrepareStringBuffer -- makes the string buffer ready for the given size, switching to the external block past the inline threshold.',
   loc={}, frame=0),
 'f896': dict(ret='void', params=['state','table','key','result'], file='script/index_table_value.cpp',
   purpose='IndexTableValue -- the table index with metamethod fallback and call plumbing, answering the looked-up value.',
   loc={}, frame=36),
 'f220': dict(ret='uint32_t', params=['text'], file='util/skip_whitespace.cpp',
   purpose='SkipWhitespace -- advances past ASCII whitespace (space, tab through carriage return) and answers the first non-space byte pointer.',
   loc={}, frame=0),
 'f209': dict(ret='void', params=['stack','index'], file='script/set_stack_top.cpp',
   purpose='SetStackTop -- sets the stack top to the given slot index, filling newly exposed slots with the shared nil record.',
   loc={}, frame=0),
 # ---- batch 2 -------------------------------------------------------------
 'f707': dict(ret='void', params=['func_state','code','line'], file='script/compile_expression.cpp',
    purpose="CompileExpression -- the code generator's expression-compilation entry: reads the expression opcode and every shape it recognises discharges into the register and operand emitters it drives. Self-recursive on nested expressions.",
    loc={}, lines=1728, frame=68),
 'f1119': dict(ret='void', params=['lex_state','expected'], file='script/check_match.cpp',
    purpose='CheckMatch -- the luaY_check-shaped syntax match check: on a mismatch it names what was expected, the token that showed up instead and the position, then raises. Calls RaiseSyntaxError(lex_state, message) and FormatPushString.',
    loc={}, lines=739, frame=28),
 'f346': dict(ret='uint32_t', params=['state','func_state','message'], file='script/check_statement_limits.cpp',
    purpose='CheckStatementLimits -- the parser semantic and depth check: the syntax-level counter behind the "chunk has too many syntax levels" trap, the vararg guard, and the statement/condition/expression emit gates it drives.',
    loc={}, lines=944, frame=32),
 'f1125': dict(ret='uint32_t', params=['state'], file='script/close_function_scope.cpp',
    purpose='CloseFunctionScope -- the function-scope teardown: closes the open upvalues, restores the scope chain, clears the local and upvalue flag bytes, and flushes the proto and string slots the function left behind.',
    loc={}, lines=1094, frame=32),
 'f4082': dict(ret='void', params=['p0','p1','p2','p3','p4'], file='util/cleanup_pending_resources.cpp',
    purpose='CleanupPendingResources -- the C++ unwind and guard cleanup: drives each pending destructor through the try/resume handoff on the asyncify throw slot, frees what the guards own, and releases the collectors the frame held.',
    loc={}, lines=1305, frame=48),
 # ---- batch 4 group A: twenty products, names locked after a source read ----
 'f205': dict(ret='uint32_t', params=['dst','value','length'], file='util/fill_bytes.cpp',
    purpose='FillBytes -- the bounded zero-fill: a byte-at-a-time run for the short spans and a word-at-a-time run past the threshold, clamped at the long end.',
    loc={}, lines=198, frame=0),
 'f215': dict(ret='void', params=['node','victim'], file='util/unlink_list_node.cpp',
    purpose='UnlinkListNode -- splices a node out of a doubly linked chain in either order, walking back through the marking bytes the list keeps beside every link until the survivor is the node it started from.',
    loc={}, lines=262, frame=0),
 'f264': dict(ret='uint32_t', params=['state','tag'], file='script/resolve_expr_value.cpp',
    purpose='ResolveExprValue -- resolves the value slot an expression tag names, clamping each family of tags to its own low and high bound and falling back to the shared empty cell when the tag is out of range.',
    loc={}, lines=132, frame=0),
 'f341': dict(ret='void', params=['state','tag'], file='script/load_expr_value.cpp',
    purpose='LoadExprValue -- the suspendable twin of ResolveExprValue: the same tag switch, but the resolved slot is read through the indexed getter so the walk can suspend.',
    loc={}, lines=201, frame=16),
 'f401': dict(ret='void', params=['dst','hi','lo','bytes'], file='util/shift_wide_digits.cpp',
    purpose='ShiftWideDigits -- the wide digit step: the 128-bit pair is shifted left by the byte-derived count and written back as two 64-bit halves.',
    loc={}, lines=53, frame=0),
 'f493': dict(ret='uint32_t', params=['state','lhs','rhs'], file='script/compare_numeric_literal.cpp',
    purpose='CompareNumericLiteral -- scans both number literals past their digit runs, skips the whitespace between them and compares the byte ranges that remain.',
    loc={}, lines=457, frame=0),
 'f603': dict(ret='uint32_t', params=['a','b'], file='util/raw_value_equal.cpp',
    purpose='RawValueEqual -- the four-way raw comparison: the pointer test first, then the tag-selected compare, with the double slot the last of the cases.',
    loc={}, lines=53, frame=0),
 'f604': dict(ret='void', params=['state','source','tag'], file='util/relocate_object_header.cpp',
    purpose='RelocateObjectHeader -- moves a header down the slab, repacking the tag byte and the two flag bytes into the new position.',
    loc={}, lines=29, frame=0),
 'f611': dict(ret='uint32_t', params=['node'], file='util/read_live_node_payload.cpp',
    purpose='ReadLiveNodePayload -- the liveness guard on the payload read: a node of the live kind with its flag unset answers the payload address, anything else answers zero.',
    loc={}, lines=34, frame=0),
 'f1120': dict(ret='uint32_t', params=['value'], file='util/bucket_log2.cpp',
    purpose='BucketLog2 -- the byte length to bucket step: the logarithm is walked eight bits at a time and the base offset of the pool is added back.',
    loc={}, lines=44, frame=0),
 'f1493': dict(ret='uint32_t', params=['slot','tag'], file='util/clear_integer_constant.cpp',
    purpose='ClearIntegerConstant -- the inline-constant probe: the small tag range and the sentinel value clear the slot and answer zero, every other shape answers whether it survived.',
    loc={}, lines=38, frame=0),
 'f1702': dict(ret='uint32_t', params=['func_state','lhs','rhs'], file='script/compile_comparison.cpp',
    purpose='CompileComparison -- the comparison compiler: constants fold to a constant instruction, everything else takes the metamethod of both operands at the call site.',
    loc={}, lines=427, frame=20),
 'f1709': dict(ret='void', params=['func_state','table','size'], file='script/initialize_table_array.cpp',
    purpose='InitializeTableArray -- sizes a table array for a constructor, raising the overflow error when the hash depth the size implies cannot fit the table.',
    loc={}, lines=300, frame=20),
 'f3514': dict(ret='uint32_t', params=['state','key'], file='core/dispatch_class_method.cpp',
    purpose='DispatchClassMethod -- the class method table walk: a class id selects the per-class word that names the entry, the child path takes the low six bits and everything else indexes the slot table directly.',
    loc={}, lines=204, frame=12),
 'f4230': dict(ret='void', params=['pool'], file='util/sweep_marked_node_list.cpp',
    purpose='SweepMarkedNodeList -- the first sweep list: the mark byte is cleared, the mask takes the cleared bit back out, and every live node on the chain gets its finaliser called through the table.',
    loc={}, lines=159, frame=16),
 'f4240': dict(ret='void', params=['pool'], file='util/sweep_secondary_node_list.cpp',
    purpose='SweepSecondaryNodeList -- the same sweep at the second set of field offsets, kept as its own entry so the field layout reads plainly.',
    loc={}, lines=159, frame=16),
 'f4715': dict(ret='void', params=['state','index','scale','flag'], file='util/sync_property_vector.cpp',
    purpose='SyncPropertyVector -- compares the cached property vector against the incoming one, takes the equality adapter for the pair when it drifts, and folds the dirty bits back afterwards.',
    loc={}, lines=412, frame=8),
 'f4881': dict(ret='void', params=['node','slot','scale'], file='util/invoke_node_handler.cpp',
    purpose='InvokeNodeHandler -- reads the handler slot out of the node and calls it through the numeric builtin table with the node and the scale in hand.',
    loc={}, lines=115, frame=8),
 'f4882': dict(ret='void', params=['node','slot','scale'], file='util/invoke_sibling_node_handler.cpp',
    purpose='InvokeSiblingNodeHandler -- the same call after the back pointer is used to reach the owning node first.',
    loc={}, lines=122, frame=8),
 'f5511': dict(ret='uint32_t', params=['state','table','key'], file='script/next_table_entry.cpp',
    purpose='NextTableEntry -- the next-pair walk: the successor of the current entry is read and the missing key raises the next error.',
    loc={}, lines=429, frame=16),
 'f5726': dict(ret='void', params=['state','table','index'], file='script/set_table_array_element.cpp',
    purpose='SetTableArrayElement -- the array-part write: the key is pushed, the field is set through the shared writer, and a number value is boxed into the value record on the way out.',
    loc={}, lines=173, frame=12),
 'f5749': dict(ret='uint32_t', params=['a','b'], file='util/compare_node_payloads.cpp',
    purpose='CompareNodePayloads -- the three-way payload comparison behind a null check, with no side effect at all.',
    loc={}, lines=23, frame=0),
 # ---- batch-4B registration ----
 'f255': dict(ret='void', params=['owner', 'value', 'shared'], file='util/assign_shared_owner.cpp',
   purpose='AssignSharedOwner -- installs a shared owner on a value, walking the tag field the owner must be recorded in.',
   loc={}, frame=24),
 'f262': dict(ret='uint32_t', params=['state', 'limit', 'chunk'], file='util/flush_output_room_if_full.cpp',
   purpose='FlushOutputRoomIfFull -- makes room in a fixed output room and answers whether the chunk still fits.',
   loc={}, frame=16),
 'f298': dict(ret='void', params=['value', 'expected', 'index'], file='util/check_argument_type.cpp',
   purpose='CheckArgumentType -- validates one argument type and answers false as soon as the tag disagrees.',
   loc={}, frame=16),
 'f304': dict(ret='uint32_t', params=['state', 'index', 'actual'], file='util/format_argument_error.cpp',
   purpose='FormatArgumentError -- formats the bad argument #%d message into the pending message buffer.',
   loc={}, frame=20),
 'f309': dict(ret='uint32_t', params=['pool', 'key'], file='util/find_pool_entry_by_tag_byte.cpp',
   purpose='FindPoolEntryByTagByte -- one lookup step through the pool chain, comparing the entry tag byte.',
   loc={}, frame=0),
 'f377': dict(ret='void', params=['state', 'slot', 'callback'], file='util/invoke_callback_when_flag_unset.cpp',
   purpose='InvokeCallbackWhenFlagUnset -- runs the pending callback while the guard flag on the slot is still clear.',
   loc={}, frame=12),
 'f382': dict(ret='uint32_t', params=['state', 'variable'], file='script/resolve_variable_slot.cpp',
   purpose='ResolveVariableSlot -- turns a variable descriptor into the local or upvalue slot the emitter will write.',
   loc={}, frame=12),
 'f442': dict(ret='void', params=['state', 'name', 'slot'], file='script/declare_local_variable.cpp',
   purpose='DeclareLocalVariable -- reserves a local slot, records its name and resets its compile-time flags.',
   loc={}, frame=44),
 'f450': dict(ret='void', params=['state', 'table'], file='util/relocate_element_array.cpp',
   purpose='RelocateElementArray -- moves a table element array into a freshly allocated block and updates the owner pointer.',
   loc={}, frame=24),
 'f451': dict(ret='void', params=['state', 'instruction'], file='script/patch_jump_chain.cpp',
   purpose='PatchJumpChain -- walks a pending jump list and retargets every entry at the chain head.',
   loc={}, frame=24),
 'f453': dict(ret='uint32_t', params=['state', 'slot'], file='script/convert_value_to_integer.cpp',
   purpose='ConvertValueToInteger -- narrows a TValue to its integer payload, raising when the tag has no integer form.',
   loc={}, frame=8),
 'f521': dict(ret='uint32_t', params=['state', 'table'], file='script/advance_table_iterator.cpp',
   purpose='AdvanceTableIterator -- steps the table cursor to the next present element and answers whether one exists.',
   loc={}, frame=16),
 'f548': dict(ret='uint32_t', params=['state', 'local', 'upvalue'], file='script/bind_upvalue_cell.cpp',
   purpose='BindUpvalueCell -- binds a local slot to its upvalue cell and copies the current value across.',
   loc={}, frame=16),
 'f605': dict(ret='uint32_t', params=['state'], file='script/concat_jump_lists.cpp',
   purpose='ConcatJumpLists -- emits the branch head and splices the second jump list onto the tail of the first.',
   loc={}, frame=36),
 'f646': dict(ret='void', params=['state', 'count'], file='script/reserve_registers.cpp',
   purpose='ReserveRegisters -- reserves count registers and raises once the frame would pass the register ceiling.',
   loc={}, frame=20),
 'f781': dict(ret='void', params=['state'], file='script/close_block_scope.cpp',
   purpose='CloseBlockScope -- closes every scope opened past the current block, releasing their local slots and jumps.',
   loc={}, frame=28),
 'f783': dict(ret='uint32_t', params=['state', 'node', 'tag'], file='util/reshape_tagged_node_chain.cpp',
   purpose='ReshapeTaggedNodeChain -- reshapes a tagged node in place, dispatching on its tag to the matching node reshape.',
   loc={}, frame=32),
 'f784': dict(ret='void', params=['state', 'instruction', 'target'], file='script/fix_pending_jump_targets.cpp',
   purpose='FixPendingJumpTargets -- re-encodes every pending jump target of an instruction and seeds a new entry for a fresh line.',
   loc={}, frame=24),
 'f885': dict(ret='void', params=['state', 'value'], file='util/create_shared_record.cpp',
   purpose='CreateSharedRecord -- builds a shared user record for a value and links it into the shared owner list.',
   loc={}, frame=20),
 'f897': dict(ret='uint32_t', params=['state', 'slot'], file='script/materialize_number_value.cpp',
   purpose='MaterializeNumberValue -- turns a numeric slot into a TValue the arithmetic path can use directly.',
   loc={}, frame=16),
 'f899': dict(ret='uint32_t', params=['state'], file='script/compile_statement.cpp',
   purpose='CompileStatement -- the statement compiler proper: dispatches on the statement kind and emits the code for it.',
   loc={}, frame=56),
 'f909': dict(ret='void', params=['state', 'variable'], file='script/emit_variable_store.cpp',
   purpose='EmitVariableStore -- emits the store for one variable kind, from a local slot up to an upvalue cell.',
   loc={}, frame=44),
 'f911': dict(ret='void', params=['state', 'instruction', 'expression'], file='script/allocate_expression_register.cpp',
   purpose='AllocateExpressionRegister -- allocates the next free register for an expression and records it on the instruction.',
   loc={}, frame=16),
 'f912': dict(ret='void', params=['state', 'instruction', 'expression'], file='script/patch_comparison_into_jump.cpp',
   purpose='PatchComparisonIntoJump -- folds a comparison into the branch that tests it, retargeting the jump chain as needed.',
   loc={}, frame=36),
 'f913': dict(ret='uint32_t', params=['state', 'opcode', 'arg_a', 'arg_c'], file='script/emit_instruction_abc.cpp',
   purpose='EmitInstructionABC -- packs the three operand fields into an instruction word and appends it to the code.',
   loc={}, frame=12),
 'f1112': dict(ret='void', params=['state', 'table', 'key', 'value'], file='script/lookup_or_create_table_field.cpp',
   purpose='LookupOrCreateTableField -- looks a field up, creating the slot on a miss and running __index and __newindex as the table demands.',
   loc={}, frame=48),
 'f1117': dict(ret='void', params=['state', 'instruction', 'count', 'unused'], file='script/release_expression_slots.cpp',
   purpose='ReleaseExpressionSlots -- returns expression registers to the free list once the instruction has consumed them.',
   loc={}, frame=16),
 'f1128': dict(ret='uint32_t', params=['state', 'queue'], file='script/compact_entry_queue.cpp',
   purpose='CompactEntryQueue -- slides the live entries of a queue together and resets the tail past the holes.',
   loc={}, frame=40),
 'f1132': dict(ret='void', params=['state', 'variable', 'register_hint'], file='script/emit_variable_load.cpp',
   purpose='EmitVariableLoad -- emits the load for one variable kind into the register the caller reserved.',
   loc={}, frame=24),
 'f1212': dict(ret='void', params=['state', 'block'], file='util/reclaim_free_block.cpp',
   purpose='ReclaimFreeBlock -- returns one block to the free list, coalesging with the neighbours when they are free too.',
   loc={}, frame=0),
 'f1486': dict(ret='uint32_t', params=['pool', 'node', 'size'], file='util/find_free_block_on_chain.cpp',
   purpose='FindFreeBlockOnChain -- finds a free block of at least size bytes on the pool free chain.',
   loc={}, frame=0),
 'f1487': dict(ret='uint32_t', params=['pool', 'block', 'size', 'node', 'limit'], file='util/coalesce_free_block_range.cpp',
   purpose='CoalesceFreeBlockRange -- coalesces an adjacent run of free blocks into one and answers the block it produced.',
   loc={}, frame=0),
 'f1701': dict(ret='void', params=['state', 'expression'], file='script/compile_operation.cpp',
   purpose='CompileOperation -- compiles one expression: folds what it can at compile time and emits the rest.',
   loc={}, frame=72),
 'f1715': dict(ret='void', params=['state', 'limit'], file='script/raise_item_limit_error.cpp',
   purpose='RaiseItemLimitError -- formats the "too many ..." item limit error at the current source position.',
   loc={}, frame=28),
 'f1718': dict(ret='void', params=['state', 'token', 'slot', 'depth', 'flag'], file='script/parse_expression.cpp',
   purpose='ParseExpression -- the expression parser front: consumes the operator chain and discharges each operand.',
   loc={}, frame=44),
 'f1719': dict(ret='void', params=['state', 'target_slot', 'value_slot'], file='script/parse_assignment.cpp',
   purpose='ParseAssignment -- parses an assignment, reconciling the target list against the values on the right.',
   loc={}, frame=36),
 'f1720': dict(ret='uint32_t', params=['state'], file='script/parse_simple_assignment.cpp',
   purpose='ParseSimpleAssignment -- parses the single-target assignment form, with no target list to reconcile.',
   loc={}, frame=20),
 'f1739': dict(ret='void', params=['state', 'count'], file='script/reserve_closure_records.cpp',
   purpose='ReserveClosureRecords -- reserves the upvalue records a batch of closures needs, raising when the block cannot grow.',
   loc={}, frame=20),
 'f1746': dict(ret='void', params=['state', 'register_hint', 'target'], file='script/emit_jump_instruction.cpp',
   purpose='EmitJumpInstruction -- emits an unconditional jump with its full 23-bit offset field.',
   loc={}, frame=12),
 'f2207': dict(ret='uint32_t', params=['header', 'size'], file='util/allocate_tagged_block.cpp',
   purpose='AllocateTaggedBlock -- allocates a tagged block of the requested size and stamps its header.',
   loc={}, frame=0),
 'f3516': dict(ret='uint32_t', params=['state', 'index'], file='util/read_inline_flag_bit3.cpp',
   purpose='ReadInlineFlagBit3 -- reads bit 3 of the inline flag byte at an index and answers it as a flag.',
   loc={}, frame=0),
 'f3804': dict(ret='uint32_t', params=['pool', 'node', 'size', 'header'], file='util/acquire_block_from_free_list.cpp',
   purpose='AcquireBlockFromFreeList -- takes a block off the free list, splitting it when the request is smaller than the block.',
   loc={}, frame=24),
 'f5509': dict(ret='void', params=['state', 'node'], file='util/reshape_power_of_two_node.cpp',
   purpose='ReshapePowerOfTwoNode -- reshapes a node whose array part has to stay a power of two in size.',
   loc={}, frame=16),
 'f5536': dict(ret='void', params=['state', 'node'], file='util/reshape_counted_array_node.cpp',
   purpose='ReshapeCountedArrayNode -- reshapes a node whose payload is a counted array.',
   loc={}, frame=16),
 'f5628': dict(ret='void', params=['state', 'node'], file='util/reshape_header_variant_node.cpp',
   purpose='ReshapeHeaderVariantNode -- reshapes a node whose size is carried in the header variant field.',
   loc={}, frame=12),
 'f5629': dict(ret='void', params=['state', 'node'], file='util/reshape_field_record_node.cpp',
   purpose='ReshapeFieldRecordNode -- reshapes a node whose payload is a record of fixed-width fields.',
   loc={}, frame=16),
 'f5630': dict(ret='void', params=['state', 'node'], file='util/reshape_inline_pair_node.cpp',
   purpose='ReshapeInlinePairNode -- reshapes a node holding a single inline key/value pair.',
   loc={}, frame=8),
 'f5678': dict(ret='uint32_t', params=['state'], file='script/add_one_constant.cpp',
   purpose='AddOneConstant -- interns the constant 1.0 into the function constant table under its own key.',
   loc={}, frame=8),
 'f5679': dict(ret='void', params=['state'], file='script/reserve_three_registers.cpp',
   purpose='ReserveThreeRegisters -- reserves the three registers the one-operand form always needs.',
   loc={}, frame=12),
 'f5716': dict(ret='uint32_t', params=['state'], file='core/step_invoke_value_entry.cpp',
   purpose='StepInvokeValueEntry -- advances one entry of an invoke queue, running the value call it describes.',
   loc={}, frame=8),
 # ---- batch-4B external registration ----
 'f211': dict(ret='uint32_t', params=['left', 'right'], file='util/compare_nul_terminated_strings.cpp',
   purpose='CompareNulTerminatedStrings -- walks two NUL-terminated byte strings and answers the first difference, treating a NUL byte as the end of the shorter side.',
   loc={}, frame=0),
 'f238': dict(ret='uint32_t', params=['value', 'message', 'tag'], file='util/raise_traceback.cpp',
   purpose='RaiseTraceback -- prefixes an error message with the %s:%d traceback source position.',
   loc={}, frame=16),
 'f246': dict(ret='uint32_t', params=['left', 'right', 'result', 'box'], file='util/combine_comparison_results.cpp',
   purpose='CombineComparisonResults -- merges the two operand results of a three-way comparison into one value.',
   loc={}, frame=24),
 'f371': dict(ret='uint32_t', params=['state', 'value'], file='util/coerce_to_number.cpp',
   purpose='CoerceToNumber -- turns a value into a number for arithmetic, falling back through the string path and the metamethod path.',
   loc={}, frame=36),
 'f373': dict(ret='uint32_t', params=['proto', 'level', 'result'], file='util/trim_local_slots_above_level.cpp',
   purpose='TrimLocalSlotsAboveLevel -- walks a function prototype from the top and drops the local slots declared above a nesting level.',
   loc={}, frame=0),
 'f435': dict(ret='double', params=['base', 'exponent'], file='util/compute_numeric_power.cpp',
   purpose='ComputeNumericPower -- the double power operation behind the ^ operator.',
   loc={}, frame=0),
 'f519': dict(ret='uint32_t', params=['state', 'name', 'out'], file='util/format_traceback_frame.cpp',
   purpose='FormatTracebackFrame -- resolves one traceback frame, walking the locals and the name table for the description.',
   loc={}, frame=36),
 'f520': dict(ret='uint32_t', params=['state', 'opcode', 'operands', 'extra', 'target'], file='util/resolve_branch_offset.cpp',
   purpose='ResolveBranchOffset -- emits a branch and folds its jump offset, raising when the control structure outgrows the offset field.',
   loc={}, frame=36),
 'f549': dict(ret='void', params=['state', 'chain'], file='script/release_node_chain.cpp',
   purpose='ReleaseNodeChain -- unlinks a node from its chain and hands the block back to the allocator.',
   loc={}, frame=28),
 'f601': dict(ret='uint32_t', params=['state', 'entries', 'array'], file='util/allocate_table.cpp',
   purpose='AllocateTable -- allocates a table object and initialises its array part.',
   loc={}, frame=24),
 'f644': dict(ret='void', params=['state', 'value', 'meta'], file='util/raise_arithmetic_error.cpp',
   purpose='RaiseArithmeticError -- raises the attempt to perform arithmetic on error for a bad operand.',
   loc={}, frame=32),
 'f837': dict(ret='uint32_t', params=['left', 'right'], file='util/compare_script_strings.cpp',
   purpose='CompareScriptStrings -- the string comparison the parser uses for identifiers and keywords.',
   loc={}, frame=0),
 'f900': dict(ret='uint32_t', params=['state', 'value', 'options'], file='script/format_number.cpp',
   purpose='FormatNumber -- the number-to-string conversion, including the 14 significant digit default and the null fallback.',
   loc={}, frame=72),
 'f962': dict(ret='uint32_t', params=['state', 'stream', 'line'], file='util/read_input_line.cpp',
   purpose='ReadInputLine -- reads one line from the input stream, growing the buffer as it goes.',
   loc={}, frame=24),
 'f1005': dict(ret='uint32_t', params=['value', 'literal'], file='script/find_string_length.cpp',
   purpose='FindStringLength -- walks a NUL-terminated literal to find its length.',
   loc={}, frame=0),
 'f1110': dict(ret='void', params=['state', 'value', 'options'], file='script/format_number_value.cpp',
   purpose='FormatNumberValue -- formats a number for output, honouring the 14 significant digit default and the string length limit.',
   loc={}, frame=44),
 'f1700': dict(ret='void', params=['state', 'local'], file='core/declare_upvalue.cpp',
   purpose='DeclareUpvalue -- declares an upvalue, adding the local it closes over to the cell list.',
   loc={}, frame=20),
 'f1703': dict(ret='uint32_t', params=['state', 'left', 'right'], file='script/compare_constant_operands.cpp',
   purpose='CompareConstantOperands -- compares two constants at compile time, folding the comparison when both sides are known.',
   loc={}, frame=20),
 'f1704': dict(ret='uint32_t', params=['value', 'options'], file='script/number_to_string.cpp',
   purpose='NumberToString -- the numeric formatter behind string coercion, with the 14 significant digit default format.',
   loc={}, frame=20),
 'f1707': dict(ret='uint32_t', params=['state'], file='script/find_numeric_key_slot.cpp',
   purpose='FindNumericKeySlot -- walks a table key to the slot a numeric key would occupy.',
   loc={}, frame=0),
 'f1735': dict(ret='uint32_t', params=['state', 'vector', 'size'], file='util/reserve_int_vector.cpp',
   purpose='ReserveIntVector -- reserves an integer vector, reallocating and re-stamping the header on the way.',
   loc={}, frame=24),
 'f1741': dict(ret='uint32_t', params=['state', 'left', 'right'], file='script/check_comparable_types.cpp',
   purpose='CheckComparableTypes -- rejects operand types that cannot be compared, before the comparison is emitted.',
   loc={}, frame=16),
 'f5510': dict(ret='void', params=['state', 'table', 'key'], file='core/store_table_field.cpp',
   purpose='StoreTableField -- stores a value into a table field, choosing the direct slot or the metamethod path.',
   loc={}, frame=16),
 'f5631': dict(ret='uint32_t', params=['state', 'node'], file='util/adjust_node_capacity.cpp',
   purpose='AdjustNodeCapacity -- grows a node to the capacity a resize asked for, re-stamping the header.',
   loc={}, frame=16),
 'f5677': dict(ret='void', params=['state', 'pending'], file='script/patch_instruction_target.cpp',
   purpose='PatchInstructionTarget -- retargets one pending jump in an instruction word and settles the pending list.',
   loc={}, frame=0),
 'f5680': dict(ret='void', params=['state', 'opcode', 'operands'], file='script/emit_branch_word.cpp',
   purpose='EmitBranchWord -- emits one branch word, choosing the short and the long encoding.',
   loc={}, frame=12),
 # ---- batch 5: the dispatch units the plan had already named (2026-10-01) ----
 'f1042': dict(ret='void', params=['a', 'b'], file='script/backlog_view.cpp',
   purpose='BacklogView::DispatchAux -- the second dispatch unit of the backlog view: one suspendable step of the backlog tag family, transcribed call for call.',
   loc={}, frame=36),
 'f3661': dict(ret='void', params=['a', 'b', 'c'], file='render/layer_events.cpp',
   purpose='Compositor::LyEventDispatch -- the layer event registration dispatch: one suspendable step that carries a layer event through the compositor registration table.',
   loc={}, frame=56),
 'f3112': dict(ret='uint32_t', params=['a', 'b'], file='script/lua_tags_var.cpp',
   purpose='LuaEngine::VarDispatch -- the var tag dispatch: one suspendable step that wraps and reflects a script value through the object layer.',
   loc={}, frame=24),
 'f3666': dict(ret='void', params=['a', 'b', 'c'], file='config/engine_mode.cpp',
   purpose='EngineMode::Dispatch -- the engine mode dispatch: one suspendable step that carries a mode transition through the mode handler table.',
   loc={}, frame=36),
 'f3655': dict(ret='void', params=['a', 'b', 'c'], file='render/compositor_tween.cpp',
   purpose='Compositor::TweenDispatch -- the compositor tween dispatch: one suspendable step that carries a tween through the compositor tween table.',
   loc={}, frame=40),
 'f904': dict(ret='uint32_t', params=['a', 'b', 'c'], file='script/lua_engine.cpp',
   purpose='LuaEngine::Call -- the script call entry: one suspendable step that invokes a function value and answers its result count.',
   loc={}, frame=60),
 'f3219': dict(ret='void', params=['a', 'b'], file='script/lua_tags_text.cpp',
   purpose='MessageLayer::QueuePrintText -- the print text queue step: one suspendable step that appends a text run to the message layer print queue.',
   loc={}, frame=48),
 'f5596': dict(ret='void', params=['a'], file='script/lua_keywords.cpp',
   purpose='LuaKeywords::Init -- the keyword table initialiser: one suspendable step that builds the table the lexer matches keywords against.',
   loc={}, frame=8),
 # ---- batch 6: the container-release closure of LuaEngine::TagDispatch (f3744) ----
 'f746': dict(ret='void', params=['a', 'b'], file='util/release_linked_node_tree.cpp',
   purpose='ReleaseLinkedNodeTree -- frees a linked node tree: it recurses into both children, hands the tail link to the next slot before releasing it, drops the inline payload when the size byte has the high bit set, and returns the node to the pool.',
   loc={}, frame=0),
 'f747': dict(ret='void', params=['a', 'b'], file='util/release_hashed_node_table.cpp',
   purpose='ReleaseHashedNodeTable -- tears down a hashed node table: every bucket is resolved through UnorderedGetBucket, each node chain on it is released, and the bucket storage goes back to the pool last.',
   loc={}, frame=12),
 'f3403': dict(ret='void', params=['a'], file='util/unordered_get_bucket.cpp',
   purpose='UnorderedGetBucket -- answers the bucket that owns a hash: the index is recomputed from the table bucket count and the low-order mask, and the table not being queryable panics through ReleaseNodeTree\'s assertion route.',
   loc={}, frame=8),
 'f861': dict(ret='void', params=['a', 'b'], file='util/release_nested_node_chain.cpp',
   purpose='ReleaseNestedNodeChain -- frees a nested node chain: both children are released recursively and the tail chain is unwound through the exception cleanup path before the node itself is freed.',
   loc={}, frame=12),
 'f1029': dict(ret='void', params=['a', 'b'], file='core/exception_cleanup.cpp',
   purpose='ResumeExceptionCleanup -- the cleanup landing pad of a resumed unwind: it releases the slot pair, finds the catch frame that matches the throw, and answers the throw marker; with no matching frame it panics and traps.',
   loc={}, frame=12),
 'f3744': dict(ret='uint32_t', params=['a', 'b'], file='script/lua_tag_dispatch.cpp',
   purpose='LuaEngine::TagDispatch -- the tag dispatch entry: one suspendable step that reads the tag name and its attributes, walks the tag table, and answers how many values the handler left on the stack.',
   loc={}, frame=184),
 'f1033': dict(ret='void', params=['a', 'b'], file='util/release_chained_node_branches.cpp',
   purpose="ReleaseChainedNodeBranches -- frees the branch links of a chained node: every branch on the chain is released through the node release path and the chain head is cleared.",
   loc={}, frame=36),
 'f1032': dict(ret='void', params=['a', 'b'], file='util/release_chained_resource_nodes.cpp',
   purpose="ReleaseChainedResourceNodes -- frees a chain of resource nodes: the walk starts at the head, each node's payload is released, and the head slot is cleared last.",
   loc={}, frame=40),
 'f390': dict(ret='uint32_t', params=['a', 'b'], file='util/rebuild_unordered_bucket_array.cpp',
   purpose="RebuildUnorderedBucketArray -- rebuilds the unordered table's bucket array: the old buckets are released and the nodes are re-linked into a freshly allocated, larger array.",
   loc={}, frame=40),
 'f1459': dict(ret='uint32_t', params=['a'], file='util/rebucket_unordered_node_array.cpp',
   purpose="RebucketUnorderedNodeArray -- re-buckets the unordered table's node array: the bucket chain is released and every node is linked again under the new bucket count.",
   loc={}, frame=36),
 'f1457': dict(ret='void', params=['a', 'b', 'c', 'd'], file='util/unordered_rebuild_bucket_chain.cpp',
   purpose="UnorderedRebuildBucketChain -- rebuilds one bucket chain of the unordered table: the chain is unwound and its nodes are re-linked under the bucket index the new mask gives.",
   loc={}, frame=32),
 'f413': dict(ret='void', params=['a', 'b', 'c', 'd', 'e', 'f'], file='util/unordered_insert_bucket_node.cpp',
   purpose="UnorderedInsertBucketNode -- links a node into its bucket chain: the bucket is resolved from the hash and the node lands at the head of that chain.",
   loc={}, frame=32),
 'f1471': dict(ret='void', params=['a', 'b', 'c', 'd', 'e', 'f'], file='util/unordered_insert_checked_bucket_node.cpp',
   purpose="UnorderedInsertCheckedBucketNode -- the checked form of the bucket insert: the bucket is resolved, the load factor is verified, and only then is the node linked in.",
   loc={}, frame=32),
 'f272': dict(ret='void', params=['a', 'b', 'c', 'd', 'e', 'f'], file='util/unordered_link_bucket_node.cpp',
   purpose="UnorderedLinkBucketNode -- links a node onto a bucket chain and keeps the table's node count and load factor in step with it.",
   loc={}, frame=32),
 'f1520': dict(ret='void', params=['a'], file='util/unordered_release_bucket_table.cpp',
   purpose="UnorderedReleaseBucketTable -- releases a whole unordered table: every bucket chain is freed and the bucket storage goes back to the pool.",
   loc={}, frame=12),
 'f678': dict(ret='void', params=['a', 'b'], file='util/release_asserted_node_tree.cpp',
   purpose="ReleaseAssertedNodeTree -- releases a node tree under the assertion route: the tail payload's assertion flag decides whether the tagged tail is freed with the node.",
   loc={}, frame=12),
 'f680': dict(ret='void', params=['a', 'b'], file='util/release_plain_node_tree.cpp',
   purpose="ReleasePlainNodeTree -- releases a plain node tree: both children are freed and the node goes back to the pool without touching a tagged tail.",
   loc={}, frame=12),
 'f682': dict(ret='void', params=['a', 'b'], file='util/release_nested_tagged_nodes.cpp',
   purpose="ReleaseNestedTaggedNodes -- releases nested tagged nodes: the tail link's tagged payload is dropped before the nested children are freed.",
   loc={}, frame=0),
 'f391': dict(ret='void', params=['a', 'b'], file='util/release_no_save_node_tree.cpp',
   purpose="ReleaseNoSaveNodeTree -- releases a node tree marked no-save: the save-eligible payload is left where it is and only the node and its children are freed.",
   loc={}, frame=0),
 'f422': dict(ret='void', params=['a', 'b'], file='util/release_nested_node_pair.cpp',
   purpose="ReleaseNestedNodePair -- releases a nested node pair: the inner node is freed first, then the outer node's own storage.",
   loc={}, frame=0),
 'f491': dict(ret='void', params=['a', 'b'], file='util/release_shallow_node_pair.cpp',
   purpose="ReleaseShallowNodePair -- releases a shallow node pair: only the two node headers are freed, their payload being owned elsewhere.",
   loc={}, frame=0),
 'f534': dict(ret='void', params=['a', 'b'], file='util/release_tagged_node_triple.cpp',
   purpose="ReleaseTaggedNodeTriple -- releases a tagged node triple: the three node slots are freed through the node release path in order.",
   loc={}, frame=0),
 'f583': dict(ret='void', params=['a', 'b'], file='util/release_tagged_child_nodes.cpp',
   purpose="ReleaseTaggedChildNodes -- releases the tagged children of a node: the tail link is followed and every tagged child is freed.",
   loc={}, frame=0),
 'f1531': dict(ret='void', params=['a'], file='util/release_node_slot_array.cpp',
   purpose="ReleaseNodeSlotArray -- releases a node slot array: every occupied slot is freed and the array storage is dropped.",
   loc={}, frame=16),
 'f2305': dict(ret='void', params=['a', 'b'], file='util/release_string_buffer_pending.cpp',
   purpose="ReleaseStringBufferPending -- releases a pending string buffer: the buffer is freed when the pending slot still owns it.",
   loc={}, frame=0),
 'f532': dict(ret='void', params=['a', 'b'], file='util/clear_linked_node_fields.cpp',
   purpose="ClearLinkedNodeFields -- clears the linked node fields: both child links and the tail link are zeroed.",
   loc={}, frame=20),
 'f437': dict(ret='uint32_t', params=['a'], file='util/destroy_node_object.cpp',
   purpose="DestroyNodeObject -- destroys a node object: the payload is released, the header is cleared, and the storage returns to the pool.",
   loc={}, frame=8),
 'f242': dict(ret='uint32_t', params=['a', 'b'], file='util/find_ordered_chain_entry.cpp',
   purpose="FindOrderedChainEntry -- finds an entry on an ordered chain: the chain is walked in key order and the matching node is answered.",
   loc={}, frame=0),
 'f471': dict(ret='void', params=['a'], file='util/unlink_ordered_chain_node.cpp',
   purpose="UnlinkOrderedChainNode -- unlinks a node from an ordered chain: the neighbours are re-linked and the node's own links are cleared.",
   loc={}, frame=0),
 'f268': dict(ret='uint32_t', params=['a', 'b', 'c'], file='util/move_memory_range.cpp',
   purpose="MoveMemoryRange -- moves a memory range: the overlapping case is ordered so the copy survives a destination that sits inside the source.",
   loc={}, frame=0),
 'f860': dict(ret='void', params=['a'], file='render/init_layer_storage.cpp',
   purpose="InitLayerStorage -- initialises the layer storage: the layer slots are zeroed and the storage chain is primed.",
   loc={}, frame=28),
 'f1408': dict(ret='void', params=['a'], file='render/release_layer_render_data.cpp',
   purpose="ReleaseLayerRenderData -- releases a layer's render data: the render chain and the buffers hanging off it are freed.",
   loc={}, frame=20),
 'f281': dict(ret='void', params=['a', 'b'], file='render/release_layer_node_tree.cpp',
   purpose="ReleaseLayerNodeTree -- releases a layer's node tree: every node of the layer is freed and the layer's node head is cleared.",
   loc={}, frame=0),
 'f2892': dict(ret='void', params=['a'], file='render/release_engine_frame_resources.cpp',
   purpose="ReleaseEngineFrameResources -- releases the engine frame resources: the per-frame pools and the view state are freed at the end of a frame.",
   loc={}, frame=24),
 'f839': dict(ret='void', params=['a'], file='render/release_shared_object_holder.cpp',
   purpose="ReleaseSharedObjectHolder -- releases a shared object holder: the holder's reference is dropped and the object is freed when it was the last owner.",
   loc={}, frame=20),
 'f243': dict(ret='void', params=['a', 'b'], file='core/raise_syntax_error.cpp',
   purpose="RaiseSyntaxError -- raises a syntax error: the source position is formatted into the message and the error goes out through the script error route.",
   loc={}, frame=12),
 'f313': dict(ret='void', params=['a', 'b', 'c'], file='core/raise_script_error.cpp',
   purpose="RaiseScriptError -- raises a script error: the formatted message is handed to the error route together with the traceback frame.",
   loc={}, frame=24),
 'f907': dict(ret='void', params=['a', 'b', 'c'], file='core/raise_type_error.cpp',
   purpose="RaiseTypeError -- raises a type error: the expected and the actual type are formatted into the message.",
   loc={}, frame=20),
 'f248': dict(ret='void', params=['a'], file='core/raise_string_length_error.cpp',
   purpose="RaiseStringLengthError -- raises a string-length error: the rejected length is formatted and the error is thrown.",
   loc={}, frame=0),
 'f250': dict(ret='uint32_t', params=['a'], file='core/raise_block_too_big.cpp',
   purpose="RaiseBlockTooBig -- raises a block-too-big error: the requested size is formatted and the allocation is refused.",
   loc={}, frame=0),
 'f293': dict(ret='uint32_t', params=['a', 'b'], file='core/raise_node_init_error.cpp',
   purpose="RaiseNodeInitError -- raises a node initialisation error: the failed node initialisation reports through the error route.",
   loc={}, frame=12),
 'f560': dict(ret='void', params=['a', 'b', 'c'], file='core/deallocate_string_buffer.cpp',
   purpose="DeallocateStringBuffer -- deallocates a string buffer: the buffer storage is returned to the pool and the capacity is cleared.",
   loc={}, frame=8),
 'f351': dict(ret='uint32_t', params=['a', 'b', 'c'], file='core/match_c_string_prefix.cpp',
   purpose="MatchCStringPrefix -- matches a C string prefix: the prefix is compared without running past the end of the haystack.",
   loc={}, frame=0),
 'f1129': dict(ret='void', params=['a', 'b', 'c'], file='core/relocate_upvalue_cells.cpp',
   purpose="RelocateUpvalueCells -- relocates upvalue cells: every closed cell is re-pointed after the value stack moved.",
   loc={}, frame=28),
 'f3192': dict(ret='uint32_t', params=['a', 'b', 'c'], file='script/advance_message_effect_timer.cpp',
   purpose="AdvanceMessageEffectTimer -- advances the message effect timer: the pending effect step is timed and applied once its interval has elapsed.",
   loc={}, frame=28),
 'f744': dict(ret='void', params=['a', 'b', 'c'], file='script/apply_auto_mode_status.cpp',
   purpose="ApplyAutoModeStatus -- applies the auto-mode status: the auto-read flag is written into the status slot and the layer state follows it.",
   loc={}, frame=28),
 'f742': dict(ret='void', params=['a', 'b', 'c'], file='script/apply_command_skip_status.cpp',
   purpose="ApplyCommandSkipStatus -- applies the command-skip status: the skip flag is written and the pending command step is released.",
   loc={}, frame=20),
 'f1038': dict(ret='void', params=['a', 'b', 'c'], file='script/apply_control_skip_status.cpp',
   purpose="ApplyControlSkipStatus -- applies the control-skip status: the skip request is honoured and the control step is released.",
   loc={}, frame=20),
 'f372': dict(ret='void', params=['a', 'b', 'c'], file='script/call_value.cpp',
   purpose="CallValue -- calls a value: the callable is projected, the argument window is prepared, and the call is entered.",
   loc={}, frame=16),
 'f547': dict(ret='void', params=['a', 'b', 'c', 'd'], file='script/check_closing_match.cpp',
   purpose="CheckClosingMatch -- checks a closing match: the closer token is matched against the opener on the block stack.",
   loc={}, frame=28),
 'f427': dict(ret='void', params=['a', 'b'], file='script/check_expr_complexity.cpp',
   purpose="CheckExprComplexity -- checks the expression complexity: the register and depth budgets are verified before the expression is emitted.",
   loc={}, frame=24),
 'f550': dict(ret='void', params=['a', 'b', 'c', 'd'], file='script/compile_numeric_power.cpp',
   purpose="CompileNumericPower -- compiles a numeric power: the exponent form is folded through the constant path.",
   loc={}, frame=24),
 'f5676': dict(ret='void', params=['a', 'b'], file='script/compile_keyword_condition.cpp',
   purpose="CompileKeywordCondition -- compiles a keyword condition: the keyword test is emitted as a conditional jump.",
   loc={}, frame=8),
 'f2729': dict(ret='uint32_t', params=['a', 'b', 'c'], file='script/copy_text_bytes.cpp',
   purpose="CopyTextBytes -- copies text bytes: the source span is copied into the destination buffer.",
   loc={}, frame=0),
 'f5671': dict(ret='void', params=['a', 'b', 'c', 'd'], file='script/emit_expression.cpp',
   purpose="EmitExpression -- emits an expression: the operand tree is walked and the value lands in a register.",
   loc={}, frame=44),
 'f5672': dict(ret='void', params=['a', 'b', 'c'], file='script/emit_condition.cpp',
   purpose="EmitCondition -- emits a condition: the test becomes a comparison plus a jump.",
   loc={}, frame=44),
 'f5675': dict(ret='void', params=['a', 'b', 'c'], file='script/emit_jump_if.cpp',
   purpose="EmitJumpIf -- emits a conditional jump: the branch patch list is opened for the target it will resolve to.",
   loc={}, frame=32),
 'f5673': dict(ret='void', params=['a', 'b', 'c'], file='script/emit_statement.cpp',
   purpose="EmitStatement -- emits a statement: the statement kind selects the emit path and the register window is closed.",
   loc={}, frame=32),
 'f1745': dict(ret='void', params=['a', 'b', 'c'], file='script/emit_move_slot.cpp',
   purpose="EmitMoveSlot -- emits a move: one register is copied into another.",
   loc={}, frame=12),
 'f1131': dict(ret='void', params=['a', 'b', 'c', 'd'], file='script/emit_prototype_prologue.cpp',
   purpose="EmitPrototypePrologue -- emits a prototype prologue: the parameter slots and the upvalue binding words are laid out.",
   loc={}, frame=24),
 'f479': dict(ret='double', params=['a', 'b'], file='script/encode_signed_order_as_double.cpp',
   purpose="EncodeSignedOrderAsDouble -- encodes a signed order as a double: the order key is folded into a sortable float.",
   loc={}, frame=0),
 'f1115': dict(ret='void', params=['a', 'b'], file='script/expect_token.cpp',
   purpose="ExpectToken -- expects a token: the next token is matched and the parse position advances.",
   loc={}, frame=24),
 'f517': dict(ret='uint32_t', params=['a', 'b', 'c'], file='script/fetch_metamethod.cpp',
   purpose="FetchMetamethod -- fetches a metamethod: the operator's metamethod is looked up on the value.",
   loc={}, frame=0),
 'f333': dict(ret='uint32_t', params=['a', 'b', 'c'], file='script/fetch_object_metamethod.cpp',
   purpose="FetchObjectMetamethod -- fetches an object metamethod: the table field is read through the object layer.",
   loc={}, frame=0),
 'f347': dict(ret='uint32_t', params=['a', 'b'], file='script/format_char_value.cpp',
   purpose="FormatCharValue -- formats a character value: the byte is rendered into the debug output buffer.",
   loc={}, frame=12),
 'f5645': dict(ret='void', params=['a', 'b', 'c'], file='script/format_concatenation_error.cpp',
   purpose="FormatConcatenationError -- formats a concatenation error: the offending operand types are rendered into the message.",
   loc={}, frame=16),
 'f5733': dict(ret='void', params=['a', 'b', 'c'], file='script/format_traceback_slot_entry.cpp',
   purpose="FormatTracebackSlotEntry -- formats a traceback slot entry: one frame's source position is appended to the traceback.",
   loc={}, frame=12),
 'f704': dict(ret='uint32_t', params=['a', 'b', 'c', 'd', 'e', 'f'], file='script/grow_array.cpp',
   purpose="GrowArray -- grows an array: the storage is reallocated to the requested capacity and the elements are moved.",
   loc={}, frame=36),
 'f289': dict(ret='void', params=['a', 'b'], file='script/grow_value_stack.cpp',
   purpose="GrowValueStack -- grows the value stack: the stack is reallocated and the live slots are relocated.",
   loc={}, frame=8),
 'f325': dict(ret='void', params=['a', 'b'], file='script/insert_formatted_key_entry.cpp',
   purpose="InsertFormattedKeyEntry -- inserts a formatted key entry: the key is formatted into the table slot's entry list.",
   loc={}, frame=12),
 'f263': dict(ret='void', params=['a', 'b', 'c'], file='script/intern_key_into_table_slot.cpp',
   purpose="InternKeyIntoTableSlot -- interns a key into a table slot: an equal key is reused, otherwise a new entry is linked in.",
   loc={}, frame=16),
 'f1742': dict(ret='void', params=['a'], file='script/invoke_call_value_with_stack.cpp',
   purpose="InvokeCallValueWithStack -- invokes a callable with the stack window: the arguments come from the value stack and the call is entered.",
   loc={}, frame=12),
 'f1734': dict(ret='void', params=['a'], file='script/invoke_meta_field.cpp',
   purpose="InvokeMetaField -- invokes a meta field: the field is resolved and called with the projected arguments.",
   loc={}, frame=20),
 'f782': dict(ret='uint32_t', params=['a'], file='script/load_proto_slot.cpp',
   purpose="LoadProtoSlot -- loads a prototype slot: the prototype's slot word is fetched and projected onto the stack.",
   loc={}, frame=16),
 'f1708': dict(ret='uint32_t', params=['a', 'b'], file='script/lookup_masked_metamethod_slot.cpp',
   purpose="LookupMaskedMetamethodSlot -- looks up a masked metamethod slot: the slot word's mask selects the metamethod.",
   loc={}, frame=0),
 'f1723': dict(ret='uint32_t', params=['a'], file='script/mask_to_bit_width.cpp',
   purpose="MaskToBitWidth -- converts a mask to a bit width: the highest set bit is counted and the width is answered.",
   loc={}, frame=0),
 'f1713': dict(ret='void', params=['a', 'b'], file='script/match_keyword.cpp',
   purpose="MatchKeyword -- matches a keyword: the table the lexer matches against is consulted and the keyword id is answered.",
   loc={}, frame=16),
 'f1714': dict(ret='void', params=['a', 'b'], file='script/parse_arguments.cpp',
   purpose="ParseArguments -- parses an argument list: the argument expressions are compiled into the parameter slots.",
   loc={}, frame=28),
 'f1118': dict(ret='void', params=['a', 'b', 'c', 'd'], file='script/parse_param_list.cpp',
   purpose="ParseParamList -- parses a parameter list: the parameter names are bound as local slots.",
   loc={}, frame=40),
 'f1721': dict(ret='void', params=['a'], file='script/patch_upvalue_jump_target.cpp',
   purpose="PatchUpvalueJumpTarget -- patches upvalue jump targets: the pending upvalue jumps are resolved once the binding is known.",
   loc={}, frame=32),
 'f1114': dict(ret='void', params=['a', 'b'], file='script/resize_table_array.cpp',
   purpose="ResizeTableArray -- resizes a table array: the array part is reallocated and the entries are moved.",
   loc={}, frame=28),
 'f1325': dict(ret='uint32_t', params=['a', 'b'], file='script/scan_bit_mask_word.cpp',
   purpose="ScanBitMaskWord -- scans a bit-mask word: the first set bit is answered with its index.",
   loc={}, frame=0),
 'f334': dict(ret='uint32_t', params=['a'], file='script/step_call_value_frame.cpp',
   purpose="StepCallValueFrame -- steps a call frame: the callable is projected and the frame is prepared for the call.",
   loc={}, frame=0),
 'f1731': dict(ret='void', params=['a'], file='script/clear_local_flags.cpp',
   purpose="ClearLocalFlags -- clears the local flags: the local slot flags are reset for the enclosing scope.",
   loc={}, frame=0),
 'f1732': dict(ret='void', params=['a'], file='script/clear_upvalue_flags.cpp',
   purpose="ClearUpvalueFlags -- clears the upvalue flags: every upvalue's cached flag is reset.",
   loc={}, frame=0),
 'f5624': dict(ret='void', params=['a', 'b'], file='script/reset_node_chain_flags.cpp',
   purpose="ResetNodeChainFlags -- resets node chain flags: the flags of every node on the chain are cleared.",
   loc={}, frame=0),
 'f763': dict(ret='uint32_t', params=['a'], file='script/clear_struct_fields.cpp',
   purpose="ClearStructFields -- clears the struct fields: the field words of the structure are zeroed.",
   loc={}, frame=0),
 'f761': dict(ret='void', params=['a'], file='script/reset_global_slot_table.cpp',
   purpose="ResetGlobalSlotTable -- resets the global slot table: the table is rebuilt empty and its slot count is reset.",
   loc={}, frame=12),
 'f1210': dict(ret='uint32_t', params=['a'], file='script/reset_input_line_state.cpp',
   purpose="ResetInputLineState -- resets the input line state: the pending input line and its cursor are cleared.",
   loc={}, frame=0),
 'f490': dict(ret='void', params=['a'], file='script/reset_event_filter_state.cpp',
   purpose="ResetEventFilterState -- resets the event filter state: the latched event filters are cleared.",
   loc={}, frame=40),
 'f776': dict(ret='uint32_t', params=['a', 'b', 'c'], file='script/reallocate_format_buffer.cpp',
   purpose="ReallocateFormatBuffer -- reallocates the format buffer: the buffer grows to hold the text being formatted.",
   loc={}, frame=20),
 'f582': dict(ret='void', params=['a'], file='render/init_node_storage.cpp',
   purpose="InitNodeStorage -- initialises the node storage: the node slots are zeroed and the storage chain is primed.",
   loc={}, frame=28),
 # ---- batch-8A: the three dispatch products that needed only a naming closure ----
 'f859': dict(ret='uint32_t', params=['a', 'b'], file='script/lua_engine_dispatch.cpp',
   purpose="LuaEngine::Dispatch -- the Lua engine dispatch entry: one suspendable step drains the queued tag dispatch work and returns the resume status.",
   loc={}, frame=60),
 'f3196': dict(ret='void', params=['a', 'b', 'c', 'd', 'e'], file='script/text_fx_dispatch.cpp',
   purpose="MessageLayer::TextFxDispatch -- the text effect dispatch step: the pending text-effect slots are advanced one message layer step.",
   loc={}, frame=128),
 'f3548': dict(ret='void', params=['a'], file='script/message_rp_dispatch.cpp',
   purpose="MessageLayer::RpDispatch -- the page turn dispatch step: the queued page turn request is applied one message layer step.",
   loc={}, frame=44),

 # ---- batch-8B ----
 'f284': dict(ret='uint32_t', params=['a', 'b'], file='util/copy_script_block.cpp',
   purpose='CopyScriptBlock -- the CScriptBlock copy constructor: clones the name string, deep-copies the tagged map through the host insert slot, copies the boxed member, and restores the map on the throw path.',
   loc={}, frame=32),
 'f353': dict(ret='void', params=['a', 'b', 'c'], file='util/assign_tagged_tree_range.cpp',
   purpose='AssignTaggedTreeRange -- clears the destination tagged tree and re-inserts every node of the [first,last) range as a fresh forty-byte node with deep-copied members.',
   loc={}, frame=36),
 'f3799': dict(ret='void', params=['a', 'b'], file='util/insert_tagged_tree_node.cpp',
   purpose='InsertTaggedTreeNode -- allocates one forty-byte tagged-tree node, constructs its key and value through the host copy slot, descends by the tag byte and rebalances the tree.',
   loc={}, frame=20),
 'f330': dict(ret='uint32_t', params=['a'], file='util/destroy_file_object.cpp',
   purpose='DestroyFileObject -- the CFile teardown: restores the class vtable, releases the owned child stream and frees the self-owned write buffer.',
   loc={}, frame=8),
 'f360': dict(ret='uint32_t', params=['a'], file='util/release_buffered_chain_node.cpp',
   purpose='ReleaseBufferedChainNode -- unlinks one buffered-stream node from the global chain, flushes its pending bytes, runs the virtual teardown hook and frees the node.',
   loc={}, frame=20),
 'f673': dict(ret='uint32_t', params=['a'], file='util/flush_buffered_chain.cpp',
   purpose='FlushBufferedChain -- flushes the object\'s own write buffer and then walks the global chain flushing every node that still has pending bytes.',
   loc={}, frame=12),
 'f1016': dict(ret='uint32_t', params=['a'], file='util/flush_output_buffer.cpp',
   purpose='FlushOutputBuffer -- hands the pending bytes of one output buffer to its sink callback, resets the counters and reports whether anything was pending.',
   loc={}, frame=24),
 'f1469': dict(ret='void', params=['a', 'b'], file='util/find_or_insert_sound_object_slot.cpp',
   purpose='FindOrInsertSoundObjectSlot -- hashes the key into the sound table, emplaces the value on a miss and asserts the shared pointer reachable through the node.',
   loc={}, frame=16),
 'f1058': dict(ret='void', params=['a', 'b', 'c'], file='util/insert_sound_object_slot.cpp',
   purpose='InsertSoundObjectSlot -- allocates one unordered node, constructs the sound-object value in place and links it into the bucket chain.',
   loc={}, frame=28),
 'f584': dict(ret='uint32_t', params=['a'], file='util/destroy_sound_object.cpp',
   purpose='DestroySoundObject -- the CSoundObject teardown: releases the shared pointer and both owned maps before the storage is handed back.',
   loc={}, frame=20),
 'f852': dict(ret='uint32_t', params=['a', 'b'], file='util/erase_tagged_tree_entry.cpp',
   purpose='EraseTaggedTreeEntry -- unlinks one tagged-tree node, rebalances, destroys the CScriptBlock value it held and answers the successor.',
   loc={}, frame=0),
 'f440': dict(ret='void', params=['a', 'b'], file='util/erase_tree_rebalance.cpp',
   purpose='EraseTreeRebalance -- the red-black rebalance-for-erase core shared by every tagged-tree erase path.',
   loc={}, frame=0),
 'f510': dict(ret='void', params=['a', 'b'], file='util/release_node_subtree_a.cpp',
   purpose='ReleaseNodeSubtreeA -- post-order release of one map node subtree, the shared body instantiated for the multi-touch and point callers.',
   loc={}, frame=0),
 'f585': dict(ret='void', params=['a', 'b'], file='util/release_node_subtree_b.cpp',
   purpose='ReleaseNodeSubtreeB -- post-order release of one map node subtree, the shared body instantiated for the socket and sound callers.',
   loc={}, frame=0),
 'f1527': dict(ret='void', params=['a'], file='util/release_map_field_subtree.cpp',
   purpose='ReleaseMapFieldSubtree -- releases the map subtree rooted in the field at offset four of the owning object.',
   loc={}, frame=0),
 'f2269': dict(ret='uint32_t', params=['a', 'b', 'c'], file='util/find_last_byte.cpp',
   purpose='FindLastByte -- scans a byte range backwards for one byte and answers its index, or minus one.',
   loc={}, frame=0),
 'f722': dict(ret='uint32_t', params=['a', 'b'], file='util/find_last_char_in_string.cpp',
   purpose='FindLastCharInString -- string wrapper over the backwards byte scan: reads the base pointer and the short-string length out of the member string.',
   loc={}, frame=0),
 # ---- batch-8C: the three giants unlocked by the batch-8B naming closure ----
 'f3651': dict(ret='void', params=['a', 'b', 'c'], file='script/lua_tag_generic_set.cpp',
   purpose="TagGenericSetOn -- the seton* tag registry: matches the tag name against the setonautomodein .. setonwindowbutton handler table, deep-copies the script block and assigns it into the tagged tree.",
   loc={}, frame=52),
 'f3293': dict(ret='void', params=['a', 'b'], file='host/fetch_manager_http.cpp',
   purpose="HttpDispatch -- the httpget/httppost tag surface: validates the url, key, value and header parameters, builds the multipart or urlencoded body and drives the fetch step.",
   loc={}, frame=64),
 'f3650': dict(ret='void', params=['a', 'b', 'c'], file='script/lua_tag_generic_del.cpp',
   purpose="TagGenericDelOn -- the delon* tag registry: matches the tag name against the delonautomodein .. delonwindowbutton handler table, erases the stored script block and releases the sound object slot it owned.",
   loc={}, frame=40),
 # ---- batch-9 ----
 'f850': dict(ret='uint32_t', params=['a', 'b'], file='util/find_unordered_value_slot.cpp',
   purpose="FindUnorderedValueSlot -- hash table read: buckets the given hash, walks the bucket chain past the marked nodes and returns the stored value slot.",
   loc={}, frame=12),
 'f1456': dict(ret='void', params=['a', 'b', 'c', 'd'], file='util/emplace_hashed_entry.cpp',
   purpose="EmplaceHashedEntry -- hash table emplace: looks the hash up in the bucket chain and either reports the existing node through the out parameter or links a freshly allocated one.",
   loc={}, frame=24),
 'f753': dict(ret='void', params=['a', 'b', 'c', 'd', 'e', 'f'], file='util/copy_tagged_tree_key_to_text.cpp',
   purpose="CopyTaggedTreeKeyToText -- tagged tree lookup that assigns the matched entry key into the destination string, honouring the short string form.",
   loc={}, frame=32),
 'f579': dict(ret='void', params=['a'], file='util/apply_event_record_table.cpp',
   purpose="ApplyEventRecordTable -- walks the queued twenty byte event records of the engine object, resolves each one through the tagged tree and applies it, releasing the replaced node subtrees.",
   loc={}, frame=32),
 # ---- batch-10 ----
 'f3551': dict(ret='void', params=['a', 'b'], file='input/auto_read.cpp',
   purpose="AutoReadTimer::Dispatch -- the per-frame auto-read tick: it runs the onEnterFrame pass, applies the auto-mode wait, click-skip and avoid-status rules and refreshes the wasm sync progress fields.",
   loc={}, frame=140),
 'f3698': dict(ret='void', params=['a', 'b', 'c'], file='render/video_player.cpp',
   purpose="VideoPlayer::Dispatch -- the video element tag dispatch: it drives playback through the input and video handles and folds the player state back into the session.",
   loc={}, frame=44),
 'f533': dict(ret='void', params=['a', 'b'], file='core/persist_session_state.cpp',
   purpose="PersistSessionState -- saves the session: it sequences the autosave.dat write and drives the onClickWaitIn and onClickWaitOut Lua callbacks.",
   loc={}, frame=24),
 # ---- batch-11 ----
 'f290': dict(ret='void', params=['a', 'b', 'c'], file='util/append_table_slot.cpp',
   purpose="AppendTableSlot -- appends one 16-byte tagged table slot to the array part and grows the array first when it is full.",
   loc={}, frame=16),
 'f363': dict(ret='void', params=['a'], file='util/ensure_table_array_room.cpp',
   purpose="EnsureTableArrayRoom -- decides the next capacity of a table array part (1024-element steps plus a spent-credit percent fallback).",
   loc={}, frame=20),
 'f288': dict(ret='void', params=['a', 'b', 'c'], file='util/make_string_from_bytes.cpp',
   purpose="MakeStringFromBytes -- builds a std::string from a raw (ptr,len) pair, raises on an impossible length and notifies the two table-slot callbacks.",
   loc={}, frame=32),
 'f772': dict(ret='uint32_t', params=['a', 'b'], file='script/get_registry_table_field.cpp',
   purpose="GetRegistryTableField -- reads one field by key out of the Lua registry pseudo-table into two cleared value slots.",
   loc={}, frame=24),
 # ---- batch-12 ----
 'f3085': dict(ret='void', params=['a', 'b', 'c'], file='pack/pack_manager_a.cpp',
   purpose="PackManager::FileDispatchA -- the file and index tag dispatcher: it resolves the pseudo-object fields, builds the getScriptBlock result through the table helpers and reports the two Japanese getScriptBlock argument errors.",
   loc={}, frame=48),
 'f3086': dict(ret='void', params=['a', 'b'], file='pack/pack_manager_b.cpp',
   purpose="PackManager::FileDispatchB -- the second file and index tag body: it drives setScriptStack through the registry table helper and reports its Japanese argument error.",
   loc={}, frame=28),
 'f3087': dict(ret='void', params=['a', 'b'], file='pack/pack_manager_c.cpp',
   purpose="PackManager::FileDispatchC -- the third file and index tag body: the reservedCommands side of the file API.",
   loc={}, frame=48),
 'f3088': dict(ret='void', params=['a', 'b'], file='render/compositor_props.cpp',
   purpose="Compositor::LyPropDispatch -- the lyprop layer-property dispatcher: it matches the property tag against time, textTween, textClearTween, video and sound and applies the matching layer property write.",
   loc={}, frame=28),
  # ---- batch-13 ----
  'f1072': dict(ret='void', params=['a'], file='util/clear_unordered_buckets.cpp',
    purpose="ClearUnorderedBuckets -- clears a boost unordered bucket array: the bucket range is set back to the empty bucket through FillBytes and the old storage goes back to the pool.",
    loc={}, frame=8),
  'f294': dict(ret='void', params=['a'], file='core/rethrow_pending_exception.cpp',
    purpose="RethrowPendingException -- the rethrow landing pad: it reads the throw latch and returns unless it holds 1, and otherwise asks the host for the matching catch and panics with the recovered value.",
    loc={}, frame=0),
  'f366': dict(ret='uint32_t', params=['a', 'b', 'c', 'd'], file='util/fread.cpp',
    purpose="Fread -- the libc fread: size times count bytes are read into the buffer through ReadInputLine and the count of complete records is answered.",
    loc={}, frame=24),
  # ---- batch-14 ----
  'f3631': dict(ret='void', params=['a', 'b', 'c'], file='host/fetch_manager_command.cpp',
    purpose="FetchManager::CommandDispatch -- the async fetch command dispatcher: it matches the command word and serves clear_cache, baseurl and the wasm_sync add/delete_persistent commands, pulling through url / dst / src / target.",
    loc={}, frame=40),
  # ---- batch-15 ----
  'f1081': dict(ret='void', params=['a', 'b'], file='util/reset_shared_ptr_01.cpp',
    purpose="ResetSharedPtr01 -- one instantiation of boost::shared_ptr<T>::reset: the data segment carries the shared_ptr.hpp assert trio (__FILE__, 'p == 0 || p != px', __func__ 'reset') and the body is that reset -- the owned pointer goes through the swap/release the assert guards; f199 Panic is the only callee.",
    loc={}, frame=20),
  'f1406': dict(ret='void', params=['a', 'b'], file='util/reset_shared_ptr_02.cpp',
    purpose="ResetSharedPtr02 -- one instantiation of boost::shared_ptr<T>::reset: the data segment carries the shared_ptr.hpp assert trio (__FILE__, 'p == 0 || p != px', __func__ 'reset') and the body is that reset -- the owned pointer goes through the swap/release the assert guards; f199 Panic is the only callee.",
    loc={}, frame=20),
  'f1465': dict(ret='void', params=['a', 'b'], file='util/reset_shared_ptr_03.cpp',
    purpose="ResetSharedPtr03 -- one instantiation of boost::shared_ptr<T>::reset: the data segment carries the shared_ptr.hpp assert trio (__FILE__, 'p == 0 || p != px', __func__ 'reset') and the body is that reset -- the owned pointer goes through the swap/release the assert guards; f199 Panic is the only callee.",
    loc={}, frame=20),
  'f2936': dict(ret='void', params=['a', 'b'], file='util/reset_shared_ptr_04.cpp',
    purpose="ResetSharedPtr04 -- one instantiation of boost::shared_ptr<T>::reset: the data segment carries the shared_ptr.hpp assert trio (__FILE__, 'p == 0 || p != px', __func__ 'reset') and the body is that reset -- the owned pointer goes through the swap/release the assert guards; f199 Panic is the only callee.",
    loc={}, frame=20),
  'f3387': dict(ret='void', params=['a', 'b'], file='util/reset_shared_ptr_05.cpp',
    purpose="ResetSharedPtr05 -- one instantiation of boost::shared_ptr<T>::reset: the data segment carries the shared_ptr.hpp assert trio (__FILE__, 'p == 0 || p != px', __func__ 'reset') and the body is that reset -- the owned pointer goes through the swap/release the assert guards; f199 Panic is the only callee.",
    loc={}, frame=20),
  'f3395': dict(ret='void', params=['a', 'b'], file='util/reset_shared_ptr_06.cpp',
    purpose="ResetSharedPtr06 -- one instantiation of boost::shared_ptr<T>::reset: the data segment carries the shared_ptr.hpp assert trio (__FILE__, 'p == 0 || p != px', __func__ 'reset') and the body is that reset -- the owned pointer goes through the swap/release the assert guards; f199 Panic is the only callee.",
    loc={}, frame=20),
  'f3558': dict(ret='void', params=['a', 'b'], file='util/reset_shared_ptr_07.cpp',
    purpose="ResetSharedPtr07 -- one instantiation of boost::shared_ptr<T>::reset: the data segment carries the shared_ptr.hpp assert trio (__FILE__, 'p == 0 || p != px', __func__ 'reset') and the body is that reset -- the owned pointer goes through the swap/release the assert guards; f199 Panic is the only callee.",
    loc={}, frame=20),
  'f3626': dict(ret='void', params=['a', 'b'], file='util/reset_shared_ptr_08.cpp',
    purpose="ResetSharedPtr08 -- one instantiation of boost::shared_ptr<T>::reset: the data segment carries the shared_ptr.hpp assert trio (__FILE__, 'p == 0 || p != px', __func__ 'reset') and the body is that reset -- the owned pointer goes through the swap/release the assert guards; f199 Panic is the only callee.",
    loc={}, frame=20),
  'f4204': dict(ret='void', params=['a', 'b'], file='util/reset_shared_ptr_09.cpp',
    purpose="ResetSharedPtr09 -- one instantiation of boost::shared_ptr<T>::reset: the data segment carries the shared_ptr.hpp assert trio (__FILE__, 'p == 0 || p != px', __func__ 'reset') and the body is that reset -- the owned pointer goes through the swap/release the assert guards; f199 Panic is the only callee.",
    loc={}, frame=20),
  'f4553': dict(ret='void', params=['a', 'b'], file='util/reset_shared_ptr_10.cpp',
    purpose="ResetSharedPtr10 -- one instantiation of boost::shared_ptr<T>::reset: the data segment carries the shared_ptr.hpp assert trio (__FILE__, 'p == 0 || p != px', __func__ 'reset') and the body is that reset -- the owned pointer goes through the swap/release the assert guards; f199 Panic is the only callee.",
    loc={}, frame=20),
  'f4569': dict(ret='void', params=['a', 'b'], file='util/reset_shared_ptr_11.cpp',
    purpose="ResetSharedPtr11 -- one instantiation of boost::shared_ptr<T>::reset: the data segment carries the shared_ptr.hpp assert trio (__FILE__, 'p == 0 || p != px', __func__ 'reset') and the body is that reset -- the owned pointer goes through the swap/release the assert guards; f199 Panic is the only callee.",
    loc={}, frame=20),
  'f1593': dict(ret='void', params=['a', 'b'], file='util/reset_shared_array.cpp',
    purpose="ResetSharedArray -- one instantiation of boost::shared_array<T>::reset: the data segment carries the shared_array.hpp assert trio (__FILE__, 'p == 0 || p != px', __func__ 'reset') and the body clears the owned array through Oc_0; f199 Panic is the second callee.",
    loc={}, frame=24),
  # ---- batch-16 ----
  'f240': dict(ret='uint32_t', params=['a', 'b', 'c'], file='util/mem_chr.cpp',
    purpose="MemChr -- the libc memchr: a byte scan up to the alignment boundary, then the word-at-a-time haszero probe, and the address of the first matching byte, or zero when the range runs out.",
    loc={}, frame=0),
  'f420': dict(ret='uint32_t', params=['a', 'b', 'c'], file='util/fill_words.cpp',
    purpose="FillWords -- the word-granular fill: one value is written across the requested number of words and the destination is answered; a zero count writes nothing.",
    loc={}, frame=0),
  'f857': dict(ret='void', params=['a'], file='util/mersenne_twister_twist.cpp',
    purpose="MersenneTwisterTwist -- std::mt19937's state twist: the 624-word array is regenerated in place with the 0x9908B0DF matrix constant, the wrapped middle words are patched, and the index goes back to zero.",
    loc={}, frame=0),
  'f3308': dict(ret='uint32_t', params=['a'], file='util/reverse_bits32.cpp',
    purpose="ReverseBits32 -- the 32-bit reversal behind the CRC table: each output bit takes the input bit mirrored across the word.",
    loc={}, frame=0),
  'f1466': dict(ret='void', params=[], file='util/init_crc32_table.cpp',
    purpose="InitCrc32Table -- the lazy CRC-32 table builder: every one of the 256 entries reflects its own index byte, runs eight rounds of the 0x04C11DB7 polynomial and stores the reversed word, once, behind the initialised flag.",
    loc={}, frame=0),
}

def die(msg):
    sys.exit('tx_gen2: ' + msg)

lines = open(GEN, encoding='utf-8', errors='replace').read().splitlines()
N = len(lines)

RET_CXX = {'u32': 'uint32_t', 'u64': 'uint64_t', 's32': 'int32_t',
           's64': 'int64_t', 'f32': 'float', 'f64': 'double'}

DEFRE = re.compile(r'^(\w+) w2c_artemis_(f\d+)\(w2c_artemis\* instance((?:, (\w+) var_p\d+)*)\) \{$')
DECLRE = re.compile(
    r'^\s*(u32|u64|f32|f64|i32|i64)\s+'
    r'((?:var_\w+\s*=\s*[^,;]+,\s*)*var_\w+\s*=\s*[^;]+);')
spans = {}
DEFSIG = {}
for i, ln in enumerate(lines):
    m = DEFRE.match(ln)
    if not m:
        continue
    # group(3) is the whole parameter tail, e.g. " u32, f32, u32" once split.
    ptypes = [t for t, _ in re.findall(r'(?:^|, )(\w+) var_p(\d+)', m.group(3))]
    DEFSIG[m.group(2)] = (m.group(1), ptypes)
    if m.group(2) in METAS:
        spans.setdefault(m.group(2), i)
# The generated layer declares each local with its own type, e.g.
#   u64 var_l5 = 0;
#   f32 var_l6 = 0, var_l7 = 0, var_l8 = 0;
# Carry those types into the product instead of assuming every local is u32:
# a float local declared uint32_t silently truncates its value to integer bits.
# The table is per function -- a bare "l6" means something different in every
# body, so a single global map would be decided by whichever function happens
# to come first in the file.
LOCALTYPES = {}
_cur = None
for i, ln in enumerate(lines):
    dm = DECLRE.match(ln)
    if dm:
        bucket = LOCALTYPES.setdefault(_cur, {})
        for part in dm.group(2).split(','):
            nm = part.split('=')[0].strip()
            if nm.startswith('var_'):
                bucket[nm[4:]] = dm.group(1)
        continue
    hm = DEFRE.match(ln)
    if hm:
        _cur = hm.group(2)
print('spans found:', len(spans), 'signatures:', len(DEFSIG))
for fnid in sorted(spans, key=lambda k: int(k[1:])):
    gen_ret, gen_pt = DEFSIG[fnid]
    m = METAS[fnid]
    if RET_CXX.get(gen_ret, gen_ret) != m['ret']:
        print('  SIGN-MISMATCH %s: generated=%s meta=%s' % (fnid, gen_ret, m['ret']))
    if len(gen_pt) != len(m['params']):
        print('  PARAM-COUNT %s: generated=%s meta=%s' % (fnid, len(gen_pt), len(m['params'])))

# body-line transformer shared rules -----------------------------------------
LOC_GEN = re.compile(r'var_(p\d+|l\d+|i\d+|j\d+|f\d+|d\d+)\b')
LAB_RE = re.compile(r'var_(Bfunc|B1|B4|L8|B\d+|L\d+)\b')
LAB = {'Bfunc':'func_exit','B1':'spill_frame','B4':'running_exit','L8':'dispatch_loop'}
LOADS = [('i32_load_default32','LoadU32'), ('i32_load8_u_default32','LoadU8'),
         ('i32_load16_u_default32','LoadU16'), ('i64_load_default32','LoadU64'),
         ('f32_load_default32','LoadF32'), ('f64_load_default32','LoadF64'),
         ('i32_load8_s_default32','LoadS8'),
          ('i32_load16_s_default32','LoadS16')]
STORES = [('i32_store_default32','StoreU32'), ('i32_store8_default32','StoreU8'),
          ('i32_store16_default32','StoreU16'), ('i64_store_default32','StoreU64'),
          ('f32_store_default32','StoreF32'), ('f64_store_default32','StoreF64')]
load_re = {g: re.compile(re.escape(g) + r'\(wasm_rt_local_memory_base, wasm_rt_local_memory_size, instance->w2c_a_a, \(u64\)\(([^)]+)\)(?: \+ (\d+u))?\)') for g, _ in LOADS}
store_re = {g: re.compile(re.escape(g) + r'\(wasm_rt_local_memory_base, wasm_rt_local_memory_size, instance->w2c_a_a, \(u64\)\(([^)]+)\)(?: \+ (\d+u))?, ([^)]+)\)') for g, _ in STORES}
LOADNAME = dict(LOADS); STORENAME = dict(STORES)
call_re = re.compile(r'w2c_artemis_(f\d+|[A-Za-z]\w*_\d+)\(instance')
HOSTMAP = {'b':'HostTempRet0', 'c':'HostRet0', 'e':'HostFailAssertion',
           # a_f is the four-argument void table trampoline; engine_context.h
           # names it HostSyncSlot (A_d/HostReleaseSlotPair owns the other side).
           'f':'HostSyncSlot',
           'g':'FindMatchingCatch', 'i':'HostOsRelease', 'j':'HostOsSchedule',
           'k':'HostOsQuery', 'm':'HostDispatchInvoke', 'pc':'HostYieldToHost',
           'q':'HostAssertFail', 'd':'HostReleaseSlotPair', 'h':'HostFoldLayoutOffset', 'p':'HostMarkRegionSeen', 'r':'HostRestorePendingThrow',
           # the table trampolines and host APIs engine_context.h already wraps
           'l':'HostInvokeViiiiii', 'n':'HostInvokeIiiiii', 'o':'HostInvokeIiiii',
           's':'HostInitResources', 't':'HostInvokeFi', 'v':'HostInvokeVif',
           'x':'HostInvokeI', 'y':'HostInvokeViiiiiii',
           'u':'HostAllocateException', 'w':'HostThrowException',
           'fa':'HostAcquireResource', 'yb':'HostBulkCopy'}
TABLEMAP = {'t0':'CallTableT0', 't1':'CallTableT1', 't2':'CallTableT2', 't3':'CallTableT3', 't4':'CallTableT4', 't5':'CallTableT5', 't7':'CallTableT7', 't8':'CallTableT8', 't9':'CallTableT9', 't10':'CallTableT10', 't11':'CallTableT11', 't12':'CallTableT12', 't13':'CallTableT13', 't14':'CallTableT14', 't15':'CallTableT15', 't16':'CallTableT16', 't18':'CallTableT18', 't19':'CallTableT19', 't20':'CallTableT20', 't22':'CallTableT22', 't24':'CallTableT24', 't25':'CallTableT25', 't26':'CallTableT26', 't27':'CallTableT27', 't28':'CallTableT28', 't29':'CallTableT29', 't31':'CallTableT31', 't32':'CallTableT32', 't33':'CallTableT33', 't34':'CallTableT34', 't35':'CallTableT35', 't36':'CallTableT36', 't37':'CallTableT37', 't38':'CallTableT38', 't41':'CallTableT41', 't43':'CallTableT43', 't44':'CallTableT44', 't45':'CallTableT45', 't46':'CallTableT46', 't47':'CallTableT47', 't55':'CallTableT55', 't58':'CallTableT58', 't60':'CallTableT60',}
host_re = re.compile(r'w2c_a_(\w+)\(instance->w2c_a_instance')
ind_re = re.compile(r'CALL_INDIRECT\(instance->w2c_Lc, [^\n]*?, (w2c_artemis_(t\d+)), ([^,]+), instance->w2c_Lc\.data\[\3\]\.module_instance, ?([^;]*)\);')
TYPES = {'u32':'uint32_t','u64':'uint64_t','s32':'int32_t','s64':'int64_t',
         'f32':'float','f64':'double'}

used_constants = {}

def transform(fnid, src_start):
    meta = METAS[fnid]
    ret, params, locs = meta['ret'], meta['params'], meta['loc']
    out = []
    notes = []
    stats = {'mem':0,'call':0}
    i = src_start + 1
    body_end = src_start
    # find end: first line that is exactly '}'
    # the window has to clear the largest product: f3744 is 99,251 generated lines,
    # so the old 12,000-line cap made it die with 'no closing brace'.
    for j in range(src_start, min(src_start + 400000, N)):
        if lines[j] == '}':
            body_end = j
            break
    if body_end == src_start: die('no closing brace for ' + fnid)
    # gather callee set + resume codes first
    callees = set()
    for j in range(src_start, body_end):
        for m in call_re.finditer(lines[j]):
            callees.add(m.group(1))
    for c in sorted(callees):
        if c not in PROD: die('%s: unknown callee %s (line %d)' % (fnid, c, src_start))
    # resume code naming: per site, first callee within 30 lines after the gate
    codename = {}
    for j in range(src_start, body_end):
        ln = lines[j]
        m = re.match(r'\s*var_i[0-9] = (\d+)u;\s*$', ln)
        if not m: continue
        code = int(m.group(1))
        if code > 4096: continue
        win = '\n'.join(lines[j+1:j+7])
        if 'var_l9' in win and 'instance->w2c_g3' in win and re.search(r'goto var_B1;|goto var_Bfunc;', win):
            callee = None
            for k in range(j, min(j+30, body_end)):
                mm = call_re.search(lines[k])
                if mm: callee = mm.group(1); break
            if callee:
                codename.setdefault(code, 'kResume%s%02d' % (PROD[callee][0].replace('::','').replace('_',''), code))
    for j in range(src_start+1, body_end):
        ln = lines[j]
        # local decl lines: skip; re-emit ourselves
        if re.match(r'^\s*u\d+ var_', ln) or re.match(r'^\s*f\d+ var_', ln) or re.match(r'^\s*\(void\)wasm_rt_', ln):
            continue
        if re.match(r'^\s*wasm_rt_local_memory_(base|size) = ', ln):
            continue
        if re.match(r'^\s*FUNC_PROLOGUE;', ln):
            continue
        if re.match(r'^\s*FUNC_EPILOGUE;', ln):
            ln = re.sub(r'^(\s*)FUNC_EPILOGUE;', r'\1LeaveFrame();', ln)
        if re.match(r'^\s*uint(8_t|64_t)[^=]*wasm_rt_local_memory_(base|size)', ln):
            continue
        # resume-code site rename (before other rewrites)
        m = re.match(r'^(\s*)var_(i\d+) = (\d+)u;\s*$', ln)
        if m and int(m.group(3)) in codename:
            code = int(m.group(3))
            win = '\n'.join(lines[j+1:j+7])
            if 'var_l9' in win and 'instance->w2c_g3' in win and re.search(r'goto var_B1;|goto var_Bfunc;', win):
                ln = '%svar_%s = %s;' % (m.group(1), m.group(2), codename[code])
        ln = ln.replace('wasm_rt_segue_write_base((*instance->w2c_a_a).data);', 'SyncMemoryBase(ctx);')
        def indrep(mo):
            tn = mo.group(2)
            if tn not in TABLEMAP: die('%s: unknown table slot %s' % (fnid, tn))
            args = mo.group(4).strip()
            return '%s(ctx, %s%s);' % (TABLEMAP[tn], mo.group(3), (', ' + args) if args else '')
        ln = ind_re.sub(indrep, ln)
        def hostrep(mo):
            key = mo.group(1)
            if key not in HOSTMAP: die('%s: unknown host import a_%s' % (fnid, key))
            return HOSTMAP[key] + '(ctx'
        ln = host_re.sub(hostrep, ln)
        # comments
        ln = re.sub(r'// 分支块边界 (var_[BL]\w*)（.*）', r'// merge point \1 (wasm2c flattening boundary)', ln)
        ln = re.sub(r'// 字符串池 (\d+)u：(.*)', r'// pool text \1: \2', ln)
        # memory ops
        for g, rx in load_re.items():
            def lrep(mo, t=LOADNAME[g]):
                stats['mem'] += 1
                return '%s(ctx, %s%s)' % (t, mo.group(1), ' + ' + mo.group(2) if mo.group(2) else '')
            ln = rx.sub(lrep, ln)
        for g, rx in store_re.items():
            def srep(mo, t=STORENAME[g]):
                stats['mem'] += 1
                return '%s(ctx, %s%s, %s)' % (t, mo.group(1), ' + ' + mo.group(2) if mo.group(2) else '', mo.group(3))
            ln = rx.sub(srep, ln)
        # callee calls
        def crep(mo):
            stats['call'] += 1
            cfn = mo.group(1)
            nm = PROD[cfn][0]
            if cfn in NESTED:
                if '::' not in nm: nm = NESTED[cfn].split()[1] + '::' + nm
            return nm + '(ctx'
            return nm + '(ctx'
        ln = call_re.sub(crep, ln)
        # asyncify driver state
        ln = ln.replace('instance->w2c_g3', 'ResumePhase(ctx)').replace('instance->w2c_g4', 'ReturnFrameSp(ctx)').replace('instance->w2c_g0', 'GuestStackSp(ctx)')
        # labels
        ln = LAB_RE.sub(lambda mo: LAB.get(mo.group(1), ('block_' if mo.group(1).startswith('B') else 'loop_') + re.sub(r'\D', '', mo.group(1))), ln)
        # locals
        def lcrep(mo):
            key = mo.group(1)
            if key in locs: return locs[key]
            if key.startswith('p'):
                idx = int(key[1:])
                if idx < len(params): return params[idx]
                return key
            return key  # lN/iN/jN/fN/dN stay as-is
        ln = LOC_GEN.sub(lcrep, ln)
        # types
        ln = re.sub(r'\b(u32|u64|s32|s64|f32|f64)\b', lambda mo: TYPES[mo.group(1)], ln)
        # resume word + frame sizes: name the g4/g0 size stanzas
        m2 = re.match(r'^(\s*)(\w+) = (\d+)u;\s*$', ln)
        if m2 and m2.group(2).startswith(('i','j')):
            k = int(m2.group(3))
            nxt = '\n'.join(lines[j+1:j+5])
            prv = lines[j-1] if j > 0 else ''
            if k == 4 and ('w2c_g4' in nxt or 'w2c_g4' in prv or 'var_l9' in nxt):
                ln = '%s%s = kResumeWordBytes;' % (m2.group(1), m2.group(2))
            elif k == meta.get('frame') and k and 'w2c_g4' in (nxt + prv):
                ln = '%s%s = kSaveFrameBytes;' % (m2.group(1), m2.group(2))
        out.append(ln)
    body = '\n'.join(out)
    leftovers = [b for b in ('var_', 'w2c_', '_default32', 'wasm_rt_', 'FUNC_', 'UNREACHABLE', 'instance') if b in body]
    return body, stats, callees, codename, leftovers, body_end

# ---- run over all, collect global constant needs ---------------------------
BODIES = {}
problems = []
for fnid in sorted(METAS, key=lambda k: int(k[1:])):
    if fnid not in spans: die('missing span for ' + fnid)
    body, stats, callees, codename, leftovers, end = transform(fnid, spans[fnid])
    if leftovers: problems.append('%s leftovers: %s' % (fnid, leftovers))
    BODIES[fnid] = (body, callees, codename)
    print('%-6s lines=%-5d mem=%-4d calls=%-3d resume=%-3d callees=%d' % (
        fnid, len(body.splitlines()), stats['mem'], stats['call'], len(codename), len(callees)))
for p in problems: print('PROBLEM', p)
import json
_BODIES_OUT = os.environ.get('ARTEMIS_BODIES_OUT') or os.path.join(REF, 'batch1_bodies.json')
json.dump({k: [v[0], sorted(v[1]), v[2]] for k, v in BODIES.items()}, open(_BODIES_OUT, 'w'))
print('saved', _BODIES_OUT)

# ---------------- emit stage ----------------------------------------------
POOL = {}
try:
    for ln2 in open(os.path.join(REF, 'code', 'strings.tsv'), encoding='utf-8'):
        parts = ln2.rstrip('\n').split('\t')
        if len(parts) >= 2 and parts[0].isdigit():
            POOL[int(parts[0])] = parts[1]
except IOError:
    print('strings.tsv missing; pool addresses get generic names')

def slug_of(text):
    words = re.findall(r'[A-Za-z]+', text)[:4]
    s = ''.join(w.capitalize() for w in words)
    return s if s else 'Text'

DEC = re.compile(r'\b(\d{6,})u\b')

def emit(fnid):
    body, callees, codename = BODIES[fnid]
    body = body.replace('UNREACHABLE;', 'TrapUnreachable();')
    meta = METAS[fnid]
    name = PROD[fnid][0]
    # named constants for every 6+ digit decimal literal
    lit2name = {}
    def drep(mo):
        v = int(mo.group(1))
        if v in lit2name: return lit2name[v]
        if v in POOL:
            nm = 'kF%sMsg%s' % (fnid[1:], slug_of(POOL[v]))
        else:
            nm = 'kF%sConst%X' % (fnid[1:], v)
        while nm in lit2name.values():
            nm = nm + 'X'
        lit2name[v] = nm
        return nm
    body = DEC.sub(drep, body)
    save_const = 'kF%sSaveFrameBytes' % fnid[1:]
    word_const = 'kF%sResumeWordBytes' % fnid[1:]
    body = body.replace('kResumeWordBytes', word_const).replace('kSaveFrameBytes', save_const)
    consts = []
    for v in sorted(lit2name):
        consts.append('constexpr uint32_t %s = 0x%Xu;  // %s' % (lit2name[v], v, POOL.get(v, '')))
    if save_const in body:
        consts.append('constexpr uint32_t %s = %du;' % (save_const, meta['frame'] or 0))
    if word_const in body:
        consts.append('constexpr uint32_t %s = 4u;' % (word_const,))
    # resume code constants carry their code value
    consts.extend('constexpr uint32_t %s = %du;' % (codename[k], k) for k in sorted(codename))
    # locals actually used
    used = sorted(set(re.findall(r'\b([ijfdl]\d+)\b', body)))
    grp = {'i': [], 'j': [], 'f': [], 'd': [], 'l': []}
    ltyped = []
    for u in used:
        if u not in params_set(meta):
            grp[u[0]].append(u)
    # locals keep the type the generated layer gave them, not a blanket u32
    for x in grp['l']:
        cty = TYPES.get(LOCALTYPES.get(fnid, {}).get(x, 'u32'), 'uint32_t')
        lit = '0;' if cty in ('uint64_t', 'double') else '0u;' if cty == 'uint32_t' else '0;'
        ltyped.append('%s %s = %s' % (cty, x, lit))
    decls = []
    if ltyped: decls.append('  ' + ' '.join(ltyped))
    if grp['i']: decls.append('  ' + ' '.join('uint32_t %s = 0u;' % x for x in grp['i']))
    if grp['j']: decls.append('  ' + ' '.join('uint64_t %s = 0;' % x for x in grp['j']))
    if grp['f']: decls.append('  ' + ' '.join('float %s = 0;' % x for x in grp['f']))
    if grp['d']: decls.append('  ' + ' '.join('double %s = 0;' % x for x in grp['d']))
    # forward declarations
    decl_lines = []
    nested = {}
    for c in sorted(callees):
        if c == fnid: continue
        cn, cret, cparams = PROD[c]
        cptypes = DEFSIG[c][1] if c in DEFSIG else ['u32'] * len(cparams[1:])
        # PROD only ever holds var_pN placeholders; the readable names are in METAS.
        # A jump-board-only callee has no METAS row yet, so it gets a neutral name
        # rather than a generated one that the style gate would reject.
        cnames = (METAS[c]['params'] if c in METAS
                  else ['a%d' % n for n in range(len(cparams) - 1)])
        argt = ['EngineContext* ctx'] + [TYPES.get(t, 'uint32_t') + ' ' + p
                                          for t, p in zip(cptypes, cnames)]
        proto = '%s %s(%s);' % (cret, cn.split('::')[-1], ', '.join(argt))
        if '::' in cn:
            nested.setdefault(cn.split('::')[0], []).append(proto)
        else:
            decl_lines.append(proto)
    nested_lines = []
    for ns in sorted(nested):
        nested_lines.append('namespace %s {' % ns)
        nested_lines.extend('  ' + p for p in nested[ns])
        nested_lines.append('}')
    psig = ', '.join(['EngineContext* ctx'] + [TYPES.get(t, 'uint32_t') + ' ' + p
                                                for t, p in zip(DEFSIG[fnid][1], meta['params'])])
    if meta.get('frame', 0):
        suspend = ['// The step is suspendable: every gate below replays the call its resume',
                   '// code names. Judgment order is untouched -- the goto graph is the',
                   "// generated driver's own flattening, renamed labels only."]
    else:
        suspend = ['// The step is a plain call: the body holds no suspend point, so the',
                   '// frame pair is bookkeeping only. Judgment order is untouched -- the',
                   "// goto graph is the generated driver's own flattening, renamed labels",
                   '// only.']
    head = ['// %s -- %s' % (os.path.basename(meta['file']).replace('.cpp','') + '.cpp', meta['purpose'].split(' -- ', 1)[1].split('. ')[0] + '.'),
            '//',
            '// %s' % meta['purpose'],
            '//',
            ] + suspend + [
            '//',
            '// src: artemis.wasm2c.c %d-%d (%s); R1 transcribe-keep.' % (spans[fnid]+1, spans[fnid]+1+max(meta.get('lines', 0)-1, 0), fnid),
            '//',
            '// Written against artemis-compat, with the official WebAssembly decompile',
            "// as the reference: this body reproduces the asyncify module's generated C",
            '// call for call.  The prose below records what this particular step does.',
            '//',
            '',
            '#include "util/engine_context.h"',
            '',
            'namespace artc {',
            '']
    if decl_lines:
        head.append('// Forward declarations -- batch-mates and registered callees.')
        head.extend(decl_lines)
    if nested_lines:
        head.append('// PROVISIONAL nested-namespace forwarders (the dispatch products land later).')
        head.extend(nested_lines)
    head += ['', 'namespace {', '']
    head.extend(consts)
    head += ['', '}  // namespace', '']
    # a qualified product (AutoReadTimer::Dispatch) is defined inside its own
    # nested namespace, so the enclosing braces have to be opened and closed here.
    # A product whose METAS name is the short one (f904's `Call`) takes its
    # namespace from NESTED instead -- the same table that qualifies its callers.
    qual = name.split('::')[:-1]
    if not qual and fnid in NESTED:
        qual = [w for w in re.split(r'[^A-Za-z0-9_]+', NESTED[fnid]) if w and w != 'namespace']
    if qual:
        head.append('namespace %s {' % '::'.join(qual))
    head.append('%s %s(%s) {' % (meta['ret'], name.split('::')[-1], psig))
    head.append('  EnterFrame();')
    head.extend(decls)
    text = '\n'.join(head) + '\n' + body + '\n' + '}\n' * (len(qual) + 2)
    path = os.path.join(ROOT, meta['file'])
    os.makedirs(os.path.dirname(path), exist_ok=True)
    open(path, 'w', encoding='utf-8', newline='\n').write(text)
    bad = [b for b in ('var_', 'w2c_', '_default32', 'wasm_rt_', 'FUNC_', 'UNREACHABLE', 'instance') if b in text]
    return len(text.splitlines()), bad

def params_set(meta):
    return set(meta['params'])

ORDER = ['f265','f5721','f910','f708','f364','f645','f223','f476','f4579','f399','f1116','f204','f207','f414','f970','f303','f1233','f592','f226','f1550','f2801','f1071','f3784','f902','f221','f896','f220','f209',
         'f707','f1119','f346','f1125','f4082',
         'f205','f215','f264','f341','f401','f493','f603','f604','f611','f1120','f1493',
         'f1702','f1709','f3514','f4230','f4240','f4715','f4881','f4882','f5511','f5726','f5749',
         # ---- batch-4B: the 50 named items, then the 26 external callees ----
         # ---- batch-9 ----
         'f850', 'f1456', 'f753', 'f579',
         # ---- batch-10 ----
         'f3551', 'f3698', 'f533',
         # ---- batch-11 ----
         'f290', 'f363', 'f288', 'f772',
         # ---- batch-12 ----
         'f3085', 'f3086', 'f3087', 'f3088',
          'f1072', 'f294', 'f366',
          'f3631',
          'f1081', 'f1406', 'f1465', 'f2936', 'f3387', 'f3395', 'f3558', 'f3626', 'f4204', 'f4553', 'f4569', 'f1593',
         # ---- batch-16: the f3633 door, take one ----
         'f240', 'f420', 'f857', 'f3308', 'f1466',
         'f605','f646','f909','f911','f912','f913','f1117','f1132','f1718','f1719','f1720','f1746','f5678','f5679',
         'f262','f298','f304','f309','f377','f783','f1486','f1487','f3516','f3804','f5509','f5536','f5628','f5629','f5630','f5716',
         'f255','f382','f442','f450','f451','f453','f521','f548','f781','f784','f885','f897','f1112','f1128','f1212','f1715','f1739','f2207','f1701','f899',
         'f211','f238','f246','f371','f373','f435','f519','f520','f549','f601','f644','f837','f900','f962','f1005','f1110','f1700',
         'f1703','f1704','f1707','f1735','f1741','f5510','f5631','f5677','f5680',
         # ---- batch 5: the dispatch units (2026-10-01) ----
         'f1042','f3661','f3112','f3666','f3655','f904','f3219','f5596',
         # ---- batch 6: the container-release closure, then TagDispatch itself ----
         'f746','f747','f3403','f861','f1029',
         'f3744',
         # ---- batch 7: the plan rows that were already satisfiable ----
         # ---- batch-8C ----
         'f3651', 'f3293', 'f3650',
         'f1033', 'f1032', 'f390', 'f1459', 'f1457', 'f413', 'f1471', 'f272', 'f1520', 'f678', 'f680', 'f682',
         'f391', 'f422', 'f491', 'f534', 'f583', 'f1531', 'f2305', 'f532', 'f437', 'f242', 'f471', 'f268',
         'f860', 'f1408', 'f281', 'f2892', 'f839', 'f243', 'f313', 'f907', 'f248', 'f250', 'f293', 'f560',
         'f351', 'f1129', 'f3192', 'f744', 'f742', 'f1038', 'f372', 'f547', 'f427', 'f550', 'f5676', 'f2729',
         'f5671', 'f5672', 'f5675', 'f5673', 'f1745', 'f1131', 'f479', 'f1115', 'f517', 'f333', 'f347', 'f5645',
         'f5733', 'f704', 'f289', 'f325', 'f263', 'f1742', 'f1734', 'f782', 'f1708', 'f1723', 'f1713', 'f1714',
         'f1118', 'f1721', 'f1114', 'f1325', 'f334', 'f1731', 'f1732', 'f5624', 'f763', 'f761', 'f1210', 'f490',
         'f776', 'f582',
         # ---- batch-8A: the three dispatch products with a naming closure ----
         'f859', 'f3196', 'f3548',
         # ---- batch-8B ----
         'f284',
         'f353',
         'f3799',
         'f330',
         'f360',
         'f673',
         'f1016',
         'f1469',
         'f1058',
         'f584',
         'f852',
         'f440',
         'f510',
         'f585',
         'f1527',
         'f2269',
         'f722',
         ]
_TODO = [f for f in ORDER if not ONLY or f in ONLY]
for fnid in _TODO:
    try:
        nl, bad = emit(fnid)
        print('EMIT %-6s %-28s lines=%-5d %s' % (fnid, METAS[fnid]['file'], nl, ('LEFTOVER:' + ','.join(bad)) if bad else ''))
    except Exception as e:
        print('EMIT-FAIL', fnid, repr(e))
        if os.environ.get('TX_DEBUG'):
            traceback.print_exc()

if CHECK:
    same = diff = absent = 0
    for fnid in _TODO:
        rel = METAS[fnid]['file']
        a = os.path.join(ROOT, rel)
        b = os.path.join(REPO, 'ports', 'artemis', 'src', rel)
        if not os.path.exists(b):
            absent += 1; print('ABSENT  %s' % rel)
        elif open(a, 'rb').read() == open(b, 'rb').read():
            same += 1
        else:
            diff += 1
            print('DIFF    %-46s generated=%-6d tree=%-6d' % (rel, os.path.getsize(a), os.path.getsize(b)))
    print('CHECK: identical %d / differing %d / absent %d' % (same, diff, absent))
