/**
 * TJS2 VM opcodes and instruction widths.
 *
 * Transcribed from the TJS2 engine's own tables in the BSD-licensed Kirikiroid2
 * distribution (`src/core/tjs2/tjsInterCodeGen.h` for the enum and
 * `src/core/tjs2/tjsByteCodeLoader.cpp` for the per-opcode operand count). That
 * project is BSD-style licensed, so the tables may be reproduced here as long as
 * the copyright notice is preserved — see `THIRD_PARTY_NOTICES.md`.
 *
 * Why a width table matters: TJS2 bytecode is a flat array of 16-bit words where
 * operands are *not* length-prefixed, so the only way to walk it is to know how
 * many words each opcode consumes. Getting one opcode wrong desynchronises the
 * entire rest of the stream, which is why the table is transcribed rather than
 * inferred.
 */

/**
 * Accessor variants.
 *
 * The engine declares most operators with a macro that emits four opcodes:
 * the plain form, the property-get form (`xPD`), the property-set form (`xPI`)
 * and a combined form (`xP`). Expanding them by hand is how the reference does
 * it, and it keeps the numbering honest because the C enum relies on the same
 * ordering.
 */
const ACCESSOR_VARIANTS = ['', 'PD', 'PI', 'P'] as const;

interface OpcodeGroup {
  /** Base name, without the accessor suffix. */
  readonly base: string;
  /** How many opcodes the group occupies. */
  readonly count: number;
}

/** Leading opcodes that are declared individually, in enum order. */
const LEADING: readonly string[] = [
  'VM_NOP',
  'VM_CONST',
  'VM_CP',
  'VM_CL',
  'VM_CCL',
  'VM_TT',
  'VM_TF',
  'VM_CEQ',
  'VM_CDEQ',
  'VM_CLT',
  'VM_CGT',
  'VM_SETF',
  'VM_SETNF',
  'VM_LNOT',
  'VM_NF',
  'VM_JF',
  'VM_JNF',
  'VM_JMP',
];

/** Operators emitted through `TJS_NORMAL_AND_PROPERTY_ACCESSER`. */
const ACCESSOR_OPERATORS: readonly string[] = [
  'VM_INC',
  'VM_DEC',
  'VM_LOR',
  'VM_LAND',
  'VM_BOR',
  'VM_BXOR',
  'VM_BAND',
  'VM_SAR',
  'VM_SAL',
  'VM_SR',
  'VM_ADD',
  'VM_SUB',
  'VM_MOD',
  'VM_DIV',
  'VM_IDIV',
  'VM_MUL',
];

/** Remaining opcodes, declared individually. */
const TRAILING: readonly string[] = [
  'VM_BNOT',
  'VM_TYPEOF',
  'VM_TYPEOFD',
  'VM_TYPEOFI',
  'VM_EVAL',
  'VM_EEXP',
  // VM_CHKINS is deliberately absent: it is declared with the leading group,
  // where the reference places it beside VM_CP/VM_CEQ/VM_CDEQ/VM_CLT/VM_CGT.
  // Listing it twice shifts every following opcode by one — which silently
  // mis-names the entire upper half of the table.
  'VM_ASC',
  'VM_CHR',
  'VM_NUM',
  'VM_CHS',
  'VM_INV',
  'VM_CHKINV',
  'VM_INT',
  'VM_REAL',
  'VM_STR',
  'VM_OCTET',
  'VM_CALL',
  'VM_CALLD',
  'VM_CALLI',
  'VM_NEW',
  'VM_GPD',
  'VM_SPD',
  'VM_SPDE',
  'VM_SPDEH',
  'VM_GPI',
  'VM_SPI',
  'VM_SPIE',
  'VM_GPDS',
  'VM_SPDS',
  'VM_GPIS',
  'VM_SPIS',
  'VM_SETP',
  'VM_GETP',
  'VM_DELD',
  'VM_DELI',
  'VM_SRV',
  'VM_RET',
  'VM_ENTRY',
  'VM_EXTRY',
  'VM_THROW',
  'VM_CHGTHIS',
  'VM_GLOBAL',
  'VM_ADDCI',
  'VM_REGMEMBER',
  'VM_DEBUGGER',
];

/** Builds the full opcode name list in enum order. */
function buildOpcodeNames(): readonly string[] {
  const names: string[] = [...LEADING];
  for (const operator of ACCESSOR_OPERATORS) {
    for (const variant of ACCESSOR_VARIANTS) {
      names.push(`${operator}${variant}`);
    }
  }
  names.push(...TRAILING);
  // The table must be dense and free of duplicates: a repeated name would not
  // fail loudly, it would just shift every later opcode's number and mis-name
  // the upper half of the table.
  if (new Set(names).size !== names.length) {
    const seen = new Set<string>();
    const repeated = names.filter((name) => (seen.has(name) ? true : (seen.add(name), false)));
    throw new Error(`duplicate opcode name(s) in the table: ${repeated.join(', ')}`);
  }
  return names;
}

/** Opcode name by numeric value. */
export const OPCODE_NAMES: readonly string[] = buildOpcodeNames();

/** Numeric opcode by name. */
export const OPCODE_BY_NAME: ReadonlyMap<string, number> = new Map(
  OPCODE_NAMES.map((name, index) => [name, index]),
);

export function opcodeName(code: number): string {
  return OPCODE_NAMES[code] ?? `VM_UNKNOWN_${code}`;
}

/** Context types carried in an object's header. */
export const CONTEXT_TYPES: readonly string[] = [
  'ctTopLevel',
  'ctFunction',
  'ctExprFunction',
  'ctProperty',
  'ctPropertySetter',
  'ctPropertyGetter',
  'ctClass',
  'ctSuperClassGetter',
];

export function contextTypeName(value: number): string {
  return CONTEXT_TYPES[value] ?? `ctUnknown_${value}`;
}

/** Function argument markers used by the call opcodes. */
export const FUNC_ARG_TYPES: readonly string[] = ['fatNormal', 'fatExpand', 'fatUnnamedExpand'];

/**
 * Smallest operand count for each opcode.
 *
 * This is `1 + operandWords`. Opcodes whose width depends on their operands
 * (`VM_CALL` and friends) are handled in `instructionWidth`, not here.
 */
const BASE_WIDTH: ReadonlyMap<string, number> = new Map<string, number>([
  ['VM_NOP', 1],
  ['VM_NF', 1],
  ['VM_CONST', 3],
  ['VM_CP', 3],
  ['VM_CEQ', 3],
  ['VM_CDEQ', 3],
  ['VM_CLT', 3],
  ['VM_CGT', 3],
  ['VM_CHKINS', 3],
  ['VM_TT', 2],
  ['VM_TF', 2],
  ['VM_SETF', 2],
  ['VM_SETNF', 2],
  ['VM_LNOT', 2],
  ['VM_BNOT', 2],
  ['VM_ASC', 2],
  ['VM_CHR', 2],
  ['VM_NUM', 2],
  ['VM_CHS', 2],
  ['VM_CL', 2],
  ['VM_INV', 2],
  ['VM_CHKINV', 2],
  ['VM_TYPEOF', 2],
  ['VM_EVAL', 2],
  ['VM_EEXP', 2],
  ['VM_INT', 2],
  ['VM_REAL', 2],
  ['VM_STR', 2],
  ['VM_OCTET', 2],
  ['VM_CCL', 3],
  ['VM_JF', 2],
  ['VM_JNF', 2],
  ['VM_JMP', 2],
  ['VM_GPD', 4],
  ['VM_GPDS', 4],
  ['VM_SPD', 4],
  ['VM_SPDE', 4],
  ['VM_SPDEH', 4],
  ['VM_SPDS', 4],
  ['VM_GPI', 4],
  ['VM_GPIS', 4],
  ['VM_SPI', 4],
  ['VM_SPIE', 4],
  ['VM_SPIS', 4],
  ['VM_SETP', 3],
  ['VM_GETP', 3],
  ['VM_DELD', 4],
  ['VM_DELI', 4],
  ['VM_TYPEOFD', 4],
  ['VM_TYPEOFI', 4],
  ['VM_SRV', 2],
  ['VM_RET', 1],
  ['VM_ENTRY', 3],
  ['VM_EXTRY', 1],
  ['VM_THROW', 2],
  ['VM_CHGTHIS', 3],
  ['VM_GLOBAL', 2],
  ['VM_ADDCI', 3],
  ['VM_REGMEMBER', 1],
  ['VM_DEBUGGER', 1],
]);

/**
 * Per-operator widths for the accessor-variant groups.
 *
 * The plain form takes one operand; `PD` and `PI` take three; `P` takes two —
 * exactly the operand counts the reference disassembler applies.
 */
const ACCESSOR_WIDTHS = [2, 4, 4, 3] as const;

/** The opcodes whose width is data-dependent. */
export const VARIABLE_WIDTH_OPCODES = ['VM_CALL', 'VM_CALLD', 'VM_CALLI', 'VM_NEW'] as const;

/**
 * Reinterprets a stored word as a signed 16-bit value.
 *
 * TJS2 bytecode words are `tjs_int16`, and the loader sign-extends each one when
 * it builds the instruction array. The distinction is load-bearing rather than
 * cosmetic: an argument count of `-1` or `-2` is a sentinel, so reading the word
 * as unsigned would turn it into 65535 or 65534 and the walk would run off the
 * end of the code array.
 */
function asInt16(value: number): number {
  const masked = value & 0xffff;
  return masked >= 0x8000 ? masked - 0x10000 : masked;
}

/**
 * Number of 16-bit words the instruction at `code[offset]` occupies.
 *
 * Returns 1 for an unrecognised opcode so the walk can continue and the caller
 * can report the unknown value, rather than desynchronising silently.
 */
export function instructionWidth(code: readonly number[], offset: number): number {
  const opcode = code[offset];
  if (opcode === undefined) return 1;
  const name = OPCODE_NAMES[opcode];
  if (name === undefined) return 1;

  const fixed = BASE_WIDTH.get(name);
  if (fixed !== undefined) return fixed;

  for (let index = 0; index < ACCESSOR_OPERATORS.length; index += 1) {
    const base = ACCESSOR_OPERATORS[index]!;
    for (let variant = 0; variant < ACCESSOR_VARIANTS.length; variant += 1) {
      if (name === `${base}${ACCESSOR_VARIANTS[variant]}`) {
        return ACCESSOR_WIDTHS[variant]!;
      }
    }
  }

  if ((VARIABLE_WIDTH_OPCODES as readonly string[]).includes(name)) {
    return callWidth(code, offset, name);
  }

  return 1;
}

/**
 * Width of a call-family instruction.
 *
 * Ported literally from the reference loader's `VM_CALL`/`VM_CALLD`/`VM_CALLI`/
 * `VM_NEW` case, because the layout is not self-describing:
 *
 * - `st` is where inline arguments begin (5 for the "D"/"I" forms, which carry
 *   one extra register operand, and 4 otherwise);
 * - the argument count sits at `st - 1`;
 * - `-1` means the arguments are already on the stack, so there are none inline;
 * - `-2` means a further count follows, then that many `(type, register)` pairs;
 * - any other value is a plain count of single-word register operands.
 */
function callWidth(code: readonly number[], offset: number, name: string): number {
  const isDi = name === 'VM_CALLD' || name === 'VM_CALLI';
  let st = isDi ? 5 : 4;
  const count = asInt16(code[offset + st - 1] ?? 0);

  if (count === -1) return st;
  if (count === -2) {
    st += 1;
    const declared = asInt16(code[offset + st - 1] ?? 0);
    return Math.max(st, st + declared * 2);
  }
  return Math.max(st, st + Math.max(0, count));
}

export { asInt16 };
