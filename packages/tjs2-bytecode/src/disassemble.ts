/**
 * TJS2 bytecode disassembly.
 *
 * Walks one code context's instruction array using the opcode width table,
 * resolving each operand against the constant pool so the output is readable
 * rather than a list of indices.
 *
 * Scope notes, because the distinction matters for what this can claim:
 *
 * - Operands that index the constant pool are resolved (registers, strings,
 *   integers, doubles, octets).
 * - Operands that are **code addresses** are reported as raw targets. The
 *   reference loader rewrites them into absolute pointers before execution
 *   (`TJS_TO_VM_CODE_ADDR`), and reproducing that mapping is only needed by an
 *   interpreter, not by a reader.
 * - Some opcodes take register operands that the loader offsets with
 *   `TJS_TO_VM_REG_ADDR` because the VM reserves a few registers at the bottom
 *   of the frame. Disassembly prints the raw index and notes the opcode, so the
 *   mapping stays visible instead of being silently baked in.
 */

import {
  VARIANT_TYPES,
  variantTypeName,
  type Tjs2Object,
  type Tjs2Program,
} from './container.js';
import {
  asInt16,
  contextTypeName,
  instructionWidth,
  opcodeName,
} from './opcodes.js';

export interface DisassembledInstruction {
  /** Index into the object's code array, in 16-bit words. */
  readonly offset: number;
  readonly opcode: number;
  readonly opcodeName: string;
  /** Raw operand words following the opcode. */
  readonly operands: readonly number[];
  /** Human-readable form, with constant operands resolved. */
  readonly text: string;
}

export interface DisassembledObject {
  readonly name: string;
  readonly contextType: string;
  readonly instructions: readonly DisassembledInstruction[];
  /** True when the walk consumed the array exactly. */
  readonly complete: boolean;
  /** Opcodes encountered that the table does not define. */
  readonly unknownOpcodes: readonly number[];
}

export interface DisassemblyResult {
  readonly version: string;
  readonly objects: readonly DisassembledObject[];
  readonly topLevel: number;
  /** Total instructions across every context. */
  readonly instructionCount: number;
  readonly unknownOpcodeCount: number;
}

/** Renders a variant slot as `type:value`, resolving the constant pool. */
function resolveVariant(program: Tjs2Program, object: Tjs2Object, slot: number): string {
  const variant = object.variants[slot];
  if (!variant) return `<slot ${slot}>`;
  const { type, index } = variant;
  const { constants } = program;
  switch (type) {
    case VARIANT_TYPES.STRING:
      return `str#${index}${constants.strings[index] !== undefined ? ` "${truncate(constants.strings[index]!)}"` : ''}`;
    case VARIANT_TYPES.INTEGER:
      return `int#${index}=${constants.longs[index] ?? '?'}`;
    case VARIANT_TYPES.REAL:
      return `real#${index}=${constants.doubles[index] ?? '?'}`;
    case VARIANT_TYPES.BYTE:
      return `byte#${index}=${constants.bytes[index] ?? '?'}`;
    case VARIANT_TYPES.SHORT:
      return `short#${index}=${constants.shorts[index] ?? '?'}`;
    case VARIANT_TYPES.LONG:
      return `long#${index}=${constants.longLongs[index] ?? '?'}n`;
    case VARIANT_TYPES.OCTET:
      return `octet#${index}(${constants.octets[index]?.byteLength ?? '?'}B)`;
    case VARIANT_TYPES.INTER_OBJECT:
      return `ctx#${index}`;
    case VARIANT_TYPES.OBJECT:
      return `obj#${index}`;
    case VARIANT_TYPES.INTER_GENERATOR:
      return `gen#${index}`;
    case VARIANT_TYPES.VOID:
      return 'void';
    default:
      return `${variantTypeName(type)}#${index}`;
  }
}

function truncate(value: string, limit = 40): string {
  const printable = value.replace(/[\r\n\t]/g, ' ');
  return printable.length <= limit ? printable : `${printable.slice(0, limit)}…`;
}

/**
 * Opcodes whose operands are constant-pool indices.
 *
 * `VM_CONST` copies a constant into a register; the arithmetic accessor forms
 * take `(result, left, right)` register triples. Both are printed with their
 * slots resolved.
 */
const CONSTANT_OPERAND_OPS = new Set([
  'VM_CP',
  'VM_CEQ',
  'VM_CDEQ',
  'VM_CLT',
  'VM_CGT',
  'VM_CHKINS',
]);

/** True when the width table knows this opcode. */
function isKnownOpcode(opcode: number): boolean {
  return opcode >= 0 && !opcodeName(opcode).startsWith('VM_UNKNOWN_');
}

/**
 * Renders one instruction's operand list.
 *
 * Only the forms whose operands are genuinely constant-pool or code-address
 * references get a resolved rendering; everything else prints its words as
 * plain indices, because guessing a role for an unknown operand shape would be
 * worse than showing the number.
 */
function renderInstruction(
  program: Tjs2Program,
  object: Tjs2Object,
  name: string,
  operands: readonly number[],
): string {
  if (name === 'VM_CONST' && operands.length >= 2) {
    return `r${operands[0]} <- ${resolveVariant(program, object, operands[1]!)}`;
  }
  if (CONSTANT_OPERAND_OPS.has(name) && operands.length >= 2) {
    return `r${operands[0]} <- ${resolveVariant(program, object, operands[1]!)}`;
  }
  if (name === 'VM_JF' || name === 'VM_JNF' || name === 'VM_JMP') {
    // The single operand is a code address in the pre-relocation index space.
    return `code+${operands[0]}`;
  }
  if (name === 'VM_ENTRY' && operands.length >= 2) {
    return `code+${operands[0]}, r${operands[1]}`;
  }
  if (operands.length === 0) return '';
  return operands.map((value) => String(value)).join(' ');
}

/**
 * Disassembles one code context.
 *
 * The walk is width-driven, so a single wrong width would desynchronise every
 * following instruction. `complete` reports whether the walk landed exactly on
 * the end of the array, which is the strongest available signal that the width
 * table agreed with reality for this script.
 */
export function disassembleObject(
  program: Tjs2Program,
  object: Tjs2Object,
): DisassembledObject {
  const instructions: DisassembledInstruction[] = [];
  const unknown = new Set<number>();
  const code = object.code;
  let offset = 0;

  while (offset < code.length) {
    const opcode = code[offset]!;
    const name = opcodeName(opcode);
    if (!isKnownOpcode(opcode)) unknown.add(opcode);

    const width = Math.max(1, instructionWidth(code, offset));
    const operands: number[] = [];
    for (let index = 1; index < width; index += 1) {
      const value = code[offset + index];
      if (value === undefined) break;
      operands.push(asInt16(value));
    }

    instructions.push({
      offset,
      opcode,
      opcodeName: name,
      operands,
      text: renderInstruction(program, object, name, operands),
    });
    offset += width;
  }

  return {
    name: object.name,
    contextType: contextTypeName(object.contextType),
    instructions,
    complete: offset === code.length,
    unknownOpcodes: [...unknown],
  };
}

/** Disassembles every context in a program. */
export function disassemble(program: Tjs2Program): DisassemblyResult {
  const objects = program.objects.map((object) => disassembleObject(program, object));
  return {
    version: program.version,
    objects,
    topLevel: program.topLevel,
    instructionCount: objects.reduce(
      (sum, object) => sum + object.instructions.length,
      0,
    ),
    unknownOpcodeCount: objects.reduce(
      (sum, object) => sum + object.unknownOpcodes.length,
      0,
    ),
  };
}

/** Renders a disassembly as text, one instruction per line. */
export function formatDisassembly(result: DisassemblyResult): string {
  const lines: string[] = [`; TJS2 bytecode version ${result.version}`];
  for (const object of result.objects) {
    lines.push(
      `; context ${object.name} (${object.contextType}), ${object.instructions.length} instruction(s)` +
        (object.complete ? '' : ' [INCOMPLETE WALK]'),
    );
    for (const instruction of object.instructions) {
      const operandText = instruction.text ? ` ${instruction.text}` : '';
      lines.push(
        `${String(instruction.offset).padStart(5)}  ${instruction.opcodeName}${operandText}`,
      );
    }
  }
  return lines.join('\n');
}

/** Summary counts, for a compatibility verdict without a full listing. */
export interface OpcodeHistogram {
  readonly byName: ReadonlyMap<string, number>;
  readonly total: number;
  readonly unknown: readonly number[];
}

export function opcodeHistogram(result: DisassemblyResult): OpcodeHistogram {
  const byName = new Map<string, number>();
  const unknown = new Set<number>();
  let total = 0;

  for (const object of result.objects) {
    for (const instruction of object.instructions) {
      total += 1;
      byName.set(instruction.opcodeName, (byName.get(instruction.opcodeName) ?? 0) + 1);
    }
    for (const opcode of object.unknownOpcodes) unknown.add(opcode);
  }

  return { byName, total, unknown: [...unknown] };
}
