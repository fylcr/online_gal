/**
 * TJS2 bytecode container parsing.
 *
 * A compiled TJS2 script is a tagged container, not a bare instruction stream.
 * The layout is transcribed from the BSD-licensed Kirikiroid2 loader
 * (`src/core/tjs2/tjsByteCodeLoader.cpp`) 鈥?see `THIRD_PARTY_NOTICES.md`.
 *
 * ```text
 * 0x00  "TJS2"                 file tag
 * 0x04  "100\0"                format version
 * 0x08  <file size>            must equal the real byte length
 * 0x0C  "DATA"                 data-area tag
 * 0x10  <data size>
 * 0x14  <data area>            seven typed constant arrays
 *        "OBJS"                objects tag, immediately after the data area
 *        <objects size>
 *        <objects area>        one record per code context
 * ```
 *
 * Every integer is little-endian. The data area's arrays are what makes a
 * faithful disassembler possible: an instruction operand is an *index* into one
 * of them, so without the constants the operand values are just numbers.
 */

import { asInt16, contextTypeName } from './opcodes.js';

/** File tag, spelled as the little-endian integer `'T' | 'J'<<8 | 'S'<<16 | '2'<<24`. */
export const TJS2_FILE_TAG = 0x32534a54;
/**
 * Version tag, from `'1' | '0'<<8 | '0'<<16 | 0<<24` 鈥?i.e. the bytes
 * `31 30 30 00`, the string `"100"` and a NUL.
 *
 * Note the packing direction: the tag characters occupy successive bytes from
 * the least significant end, so the *byte* sequence is `"100\0"` but the
 * *integer* is `0x00303031`, not the big-endian reading of those bytes. Getting
 * this backwards makes every valid file fail the version check.
 */
export const TJS2_VER_TAG = 0x00303031;
export const DATA_TAG = 0x41544144; // 'DATA'
export const OBJS_TAG = 0x534a424f; // 'OBJS'

export interface Tjs2Constants {
  readonly bytes: Int8Array;
  readonly shorts: Int16Array;
  readonly longs: Int32Array;
  readonly longLongs: BigInt64Array;
  readonly doubles: Float64Array;
  readonly strings: readonly string[];
  readonly octets: readonly Uint8Array[];
}

/** A typed constant reference attached to a variant slot. */
export interface Tjs2VariantSlot {
  /** One of the `TYPE_*` constants. */
  readonly type: number;
  /** Index into the matching constants array. */
  readonly index: number;
}

/** One code context: a top-level script, function, property or class body. */
export interface Tjs2Object {
  readonly index: number;
  readonly name: string;
  readonly contextType: number;
  readonly contextTypeName: string;
  /** Index of the parent context, or -1. */
  readonly parent: number;
  readonly propSetter: number;
  readonly propGetter: number;
  readonly superClassGetter: number;
  readonly maxVariableCount: number;
  readonly variableReserveCount: number;
  readonly maxFrameCount: number;
  readonly funcDeclArgCount: number;
  readonly funcDeclUnnamedArgArrayBase: number;
  readonly funcDeclCollapseBase: number;
  /** Source positions, paired with code positions. */
  readonly sourcePositions: readonly { readonly codePos: number; readonly sourcePos: number }[];
  /** Instructions as signed 16-bit words, exactly as stored. */
  readonly code: readonly number[];
  /** Variant slots, one per constant/register the object references. */
  readonly variants: readonly Tjs2VariantSlot[];
  /** Super-class getter pointers. */
  readonly superClassGetterPointers: readonly number[];
  /** `(name index, object index)` pairs registered on the parent. */
  readonly properties: readonly { readonly nameIndex: number; readonly objectIndex: number }[];
}

export interface Tjs2Program {
  readonly version: string;
  readonly fileSize: number;
  readonly constants: Tjs2Constants;
  readonly objects: readonly Tjs2Object[];
  /** Index of the top-level context, or -1. */
  readonly topLevel: number;
}

export class Tjs2FormatError extends Error {
  constructor(message: string, readonly offset?: number) {
    super(offset === undefined ? message : `${message} (at 0x${offset.toString(16)})`);
    this.name = 'Tjs2FormatError';
  }
}

/** Variant type tags, from the loader's `TYPE_*` constants. */
export const VARIANT_TYPES = {
  VOID: 0,
  OBJECT: 1,
  INTER_OBJECT: 2,
  STRING: 3,
  OCTET: 4,
  REAL: 5,
  BYTE: 6,
  SHORT: 7,
  INTEGER: 8,
  LONG: 9,
  INTER_GENERATOR: 10,
  UNKNOWN: -1,
} as const;

const VARIANT_TYPE_NAMES: Readonly<Record<number, string>> = {
  0: 'void',
  1: 'object',
  2: 'inter-object',
  3: 'string',
  4: 'octet',
  5: 'real',
  6: 'byte',
  7: 'short',
  8: 'integer',
  9: 'long',
  10: 'inter-generator',
};

export function variantTypeName(type: number): string {
  return VARIANT_TYPE_NAMES[type] ?? `unknown(${type})`;
}

/** True when the payload begins with the TJS2 bytecode signature. */
export function isTjs2Bytecode(bytes: Uint8Array): boolean {
  if (bytes.byteLength < 12) return false;
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  return (
    view.getUint32(0, true) === TJS2_FILE_TAG &&
    view.getUint32(4, true) === TJS2_VER_TAG
  );
}

/** Decodes the version tag bytes into its printable form. */
function versionString(view: DataView, offset: number): string {
  let out = '';
  for (let index = 0; index < 4; index += 1) {
    const byte = view.getUint8(offset + index);
    if (byte === 0) break;
    out += String.fromCharCode(byte);
  }
  return out;
}

/** A bounds-checked cursor over the payload. */
class Reader {
  private offset: number;

  constructor(
    private readonly bytes: Uint8Array,
    private readonly view: DataView,
    start: number,
    private readonly limit: number,
  ) {
    this.offset = start;
  }

  get position(): number {
    return this.offset;
  }


  private need(count: number): void {
    if (this.offset + count > this.limit) {
      throw new Tjs2FormatError(
        `truncated: needed ${count} more byte(s)`,
        this.offset,
      );
    }
  }

  u8(): number {
    this.need(1);
    const value = this.view.getUint8(this.offset);
    this.offset += 1;
    return value;
  }

  i8(): number {
    this.need(1);
    const value = this.view.getInt8(this.offset);
    this.offset += 1;
    return value;
  }

  u16(): number {
    this.need(2);
    const value = this.view.getUint16(this.offset, true);
    this.offset += 2;
    return value;
  }

  i16(): number {
    this.need(2);
    const value = this.view.getInt16(this.offset, true);
    this.offset += 2;
    return value;
  }

  i32(): number {
    this.need(4);
    const value = this.view.getInt32(this.offset, true);
    this.offset += 4;
    return value;
  }

  i64(): bigint {
    this.need(8);
    const value = this.view.getBigInt64(this.offset, true);
    this.offset += 8;
    return value;
  }

  f64(): number {
    this.need(8);
    const value = this.view.getFloat64(this.offset, true);
    this.offset += 8;
    return value;
  }

  skip(count: number): void {
    this.need(count);
    this.offset += count;
  }

  /** Copies `count` bytes out of the payload. */
  bytesOf(count: number): Uint8Array {
    this.need(count);
    const start = this.offset;
    this.offset += count;
    return this.bytes.slice(start, start + count);
  }

  /** Reads `count` UTF-16LE code units, honouring the 2-byte alignment pad. */
  utf16(count: number): string {
    const bytesNeeded = count * 2;
    this.need(bytesNeeded);
    const start = this.offset;
    this.offset += bytesNeeded;
    // The arrays are 4-byte aligned, so an odd unit count carries a pad.
    if (count & 1) this.skip(2);
    let out = '';
    for (let index = 0; index < count; index += 1) {
      out += String.fromCharCode(
        this.bytes[start + index * 2]! | (this.bytes[start + index * 2 + 1]! << 8),
      );
    }
    return out;
  }
}

/**
 * Parses a compiled TJS2 script.
 *
 * Throws `Tjs2FormatError` with the offending offset rather than returning a
 * partially-populated structure: a half-parsed script is worse than a clear
 * failure, because a caller cannot tell which objects are trustworthy.
 */
export function parseTjs2Bytecode(bytes: Uint8Array): Tjs2Program {
  if (bytes.byteLength < 20) {
    throw new Tjs2FormatError('payload is too small to be TJS2 bytecode');
  }
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);

  if (view.getUint32(0, true) !== TJS2_FILE_TAG) {
    throw new Tjs2FormatError('missing TJS2 file tag', 0);
  }
  const version = versionString(view, 4);
  if (view.getUint32(4, true) !== TJS2_VER_TAG) {
    throw new Tjs2FormatError(`unsupported bytecode version "${version}"`, 4);
  }

  const fileSize = view.getInt32(8, true);
  if (fileSize !== bytes.byteLength) {
    throw new Tjs2FormatError(
      `declared file size ${fileSize} does not match the payload (${bytes.byteLength} bytes)`,
      8,
    );
  }

  if (view.getUint32(12, true) !== DATA_TAG) {
    throw new Tjs2FormatError('missing DATA tag', 12);
  }
  const dataSize = view.getInt32(16, true);

  const dataReader = new Reader(bytes, view, 20, 20 + dataSize);
  const constants = readDataArea(dataReader);

  const objectsTagOffset = 20 + dataSize;
  if (objectsTagOffset + 8 > bytes.byteLength) {
    throw new Tjs2FormatError('data area runs past the end of the payload', objectsTagOffset);
  }
  if (view.getUint32(objectsTagOffset, true) !== OBJS_TAG) {
    throw new Tjs2FormatError('missing OBJS tag', objectsTagOffset);
  }
  const objectsSize = view.getInt32(objectsTagOffset + 4, true);

  const objectsReader = new Reader(
    bytes,
    view,
    objectsTagOffset + 8,
    objectsTagOffset + 8 + objectsSize,
  );
  const { objects, topLevel } = readObjects(objectsReader, constants);

  return { version, fileSize, constants, objects, topLevel };
}

/**
 * Reads the seven typed constant arrays.
 *
 * Each is introduced by a 32-bit count. Three of them are padded to a 4-byte
 * boundary because the next array is read from an aligned offset 鈥?a detail that
 * cannot be skipped, since a missed pad shifts every following count.
 */
function readDataArea(reader: Reader): Tjs2Constants {

  const byteCount = reader.i32();
  const bytes = new Int8Array(Math.max(0, byteCount));
  for (let index = 0; index < bytes.length; index += 1) bytes[index] = reader.i8();
  if (byteCount & 3) reader.skip(4 - (byteCount & 3));

  const shortCount = reader.i32();
  const shorts = new Int16Array(Math.max(0, shortCount));
  for (let index = 0; index < shorts.length; index += 1) shorts[index] = reader.i16();
  if (shortCount & 1) reader.skip(2);

  const longCount = reader.i32();
  const longs = new Int32Array(Math.max(0, longCount));
  for (let index = 0; index < longs.length; index += 1) longs[index] = reader.i32();

  const longLongCount = reader.i32();
  const longLongs = new BigInt64Array(Math.max(0, longLongCount));
  for (let index = 0; index < longLongs.length; index += 1) longLongs[index] = reader.i64();

  const doubleCount = reader.i32();
  const doubles = new Float64Array(Math.max(0, doubleCount));
  for (let index = 0; index < doubles.length; index += 1) doubles[index] = reader.f64();

  const stringCount = reader.i32();
  const strings: string[] = [];
  for (let index = 0; index < stringCount; index += 1) {
    const length = reader.i32();
    strings.push(reader.utf16(length));
  }

  const octetCount = reader.i32();
  const octets: Uint8Array[] = [];
  for (let index = 0; index < octetCount; index += 1) {
    const length = reader.i32();
    octets.push(reader.bytesOf(length));
    // Octet payloads are also 4-byte aligned.
    if (length & 3) reader.skip(4 - (length & 3));
  }

  return { bytes, shorts, longs, longLongs, doubles, strings, octets };
}


/** Reads the code-context records. */
function readObjects(
  reader: Reader,
  constants: Tjs2Constants,
): { objects: Tjs2Object[]; topLevel: number } {
  const topLevel = reader.i32();
  const count = reader.i32();
  if (count < 0 || count > 1_000_000) {
    throw new Tjs2FormatError(`implausible object count ${count}`, reader.position);
  }

  const objects: Tjs2Object[] = [];
  for (let index = 0; index < count; index += 1) {
    const recordStart = reader.position;
    if (reader.i32() !== TJS2_FILE_TAG) {
      throw new Tjs2FormatError(
        `object ${index} is missing its file tag`,
        recordStart,
      );
    }
    reader.i32(); // per-object size, redundant with the record walk

    const parent = reader.i32();
    const nameIndex = reader.i32();
    const contextType = reader.i32();
    const maxVariableCount = reader.i32();
    const variableReserveCount = reader.i32();
    const maxFrameCount = reader.i32();
    const funcDeclArgCount = reader.i32();
    const funcDeclUnnamedArgArrayBase = reader.i32();
    const funcDeclCollapseBase = reader.i32();
    const propSetter = reader.i32();
    const propGetter = reader.i32();
    const superClassGetter = reader.i32();

    const sourceCount = reader.i32();
    const sourcePositions: { codePos: number; sourcePos: number }[] = [];
    if (sourceCount > 0) {
      const codePositions = new Int32Array(sourceCount);
      for (let i = 0; i < sourceCount; i += 1) codePositions[i] = reader.i32();
      for (let i = 0; i < sourceCount; i += 1) {
        sourcePositions.push({ codePos: codePositions[i]!, sourcePos: reader.i32() });
      }
    }

    const codeSize = reader.i32();
    const code: number[] = new Array(Math.max(0, codeSize));
    for (let i = 0; i < codeSize; i += 1) code[i] = asInt16(reader.u16());
    if (codeSize & 1) reader.skip(2);

    const variantCount = reader.i32();
    const variants: Tjs2VariantSlot[] = [];
    for (let i = 0; i < variantCount; i += 1) {
      variants.push({ type: asInt16(reader.u16()), index: asInt16(reader.u16()) });
    }

    const pointerCount = reader.i32();
    const superClassGetterPointers: number[] = [];
    for (let i = 0; i < pointerCount; i += 1) superClassGetterPointers.push(reader.i32());

    const propertyCount = reader.i32();
    const properties: { nameIndex: number; objectIndex: number }[] = [];
    for (let i = 0; i < propertyCount; i += 1) {
      properties.push({ nameIndex: reader.i32(), objectIndex: reader.i32() });
    }

    objects.push({
      index,
      name: constants.strings[nameIndex] ?? `<string ${nameIndex}>`,
      contextType,
      contextTypeName: contextTypeName(contextType),
      parent,
      propSetter,
      propGetter,
      superClassGetter,
      maxVariableCount,
      variableReserveCount,
      maxFrameCount,
      funcDeclArgCount,
      funcDeclUnnamedArgArrayBase,
      funcDeclCollapseBase,
      sourcePositions,
      code,
      variants,
      superClassGetterPointers,
      properties,
    });
  }

  return { objects, topLevel };
}
