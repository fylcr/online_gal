/**
 * Byte-level readers.
 *
 * Archive headers in the visual-novel world mix endianness freely (XP3 is
 * little-endian, NSA is little-endian, ASAR is little-endian JSON, ZIP is
 * little-endian), so every reader takes an explicit `DataView`.
 */

export function u8(view: DataView, offset: number): number {
  return view.getUint8(offset);
}

export function u16le(view: DataView, offset: number): number {
  return view.getUint16(offset, true);
}

export function u32le(view: DataView, offset: number): number {
  return view.getUint32(offset, true);
}

/**
 * Big-endian 16-bit read.
 *
 * NScripter's archive headers are big-endian: the engine reads them as
 * `buf[0] << 8 | buf[1]`. That is the opposite of every other format this
 * package handles, so the two orders are spelled out rather than defaulted.
 */
export function u16be(view: DataView, offset: number): number {
  return view.getUint16(offset, false);
}

/** Big-endian 32-bit read. See `u16be`. */
export function u32be(view: DataView, offset: number): number {
  return view.getUint32(offset, false);
}

export function i32le(view: DataView, offset: number): number {
  return view.getInt32(offset, true);
}

export function u64le(view: DataView, offset: number): bigint {
  return view.getBigUint64(offset, true);
}

/**
 * Reads a bigint safely down to a number, throwing for values that cannot be
 * addressed in a browser anyway (files above 2^53 bytes).
 */
export function u64leToNumber(view: DataView, offset: number): number {
  const value = view.getBigUint64(offset, true);
  if (value > BigInt(Number.MAX_SAFE_INTEGER)) {
    throw new Error(`offset ${value} exceeds the addressable range`);
  }
  return Number(value);
}

/**
 * XP3 variable-length integer.
 *
 * Layout, from the XP3 index format:
 *  - bits 0–1 of the first byte: total byte count (1–4);
 *  - bits 2–7 of the first byte plus every following byte, little-endian:
 *    the value's bits 2 and up.
 *
 * So `value` is reconstructed by dropping the two selector bits from the first
 * byte and appending the remaining bytes shifted by 6 + 8·i.
 */
export function readXp3VarInt(view: DataView, offset: number): {
  value: number;
  next: number;
} {
  if (offset >= view.byteLength) return { value: 0, next: view.byteLength };
  const first = view.getUint8(offset);
  const kind = first & 0x03;
  const low = (first & 0xfc) >> 2;
  const byteAt = (index: number): number =>
    offset + index < view.byteLength ? view.getUint8(offset + index) : 0;

  switch (kind) {
    case 0:
      return { value: low, next: offset + 1 };
    case 1:
      return { value: low | (byteAt(1) << 6), next: offset + 2 };
    case 2:
      return {
        value: low | (byteAt(1) << 6) | (byteAt(2) << 14),
        next: offset + 3,
      };
    default:
      return {
        value: low | (byteAt(1) << 6) | (byteAt(2) << 14) | (byteAt(3) << 22),
        next: offset + 4,
      };
  }
}

/**
 * Reads an XP3 chunk tag.
 *
 * Tags are four ASCII characters in file order, which makes the stored integer
 * big-endian — the opposite convention to the surrounding numeric fields. Some
 * writers instead emit the tag through the var-int path, so both readings are
 * tried and the one that yields a printable ASCII tag wins.
 */
export function readXp3Tag(
  view: DataView,
  offset: number,
): { tag: number; next: number } {
  const bigEndian =
    (view.getUint8(offset) << 24) |
    (view.getUint8(offset + 1) << 16) |
    (view.getUint8(offset + 2) << 8) |
    view.getUint8(offset + 3);

  if (isPrintableTag(bigEndian)) {
    return { tag: bigEndian >>> 0, next: offset + 4 };
  }

  const varInt = readXp3VarInt(view, offset);
  if (isPrintableTag(varInt.value)) {
    return { tag: varInt.value >>> 0, next: varInt.next };
  }

  // Fall back to the literal reading so the caller can report the mismatch.
  return { tag: bigEndian >>> 0, next: offset + 4 };
}

/** True when all four bytes of `tag` are printable ASCII. */
function isPrintableTag(tag: number): boolean {
  return (
    (tag >>> 24) >= 0x20 &&
    ((tag >>> 16) & 0xff) >= 0x20 &&
    ((tag >>> 8) & 0xff) >= 0x20 &&
    (tag & 0xff) >= 0x20
  );
}

/** Reads `length` UTF-16LE code units starting at `offset`. */
export function readUtf16le(
  bytes: Uint8Array,
  offset: number,
  length: number,
): string {
  let out = '';
  for (let i = 0; i < length; i += 1) {
    const code = bytes[offset + i * 2]! | (bytes[offset + i * 2 + 1]! << 8);
    out += String.fromCharCode(code);
  }
  return out;
}

/** Whether `bytes` starts with the ASCII `signature`. */
export function matchesAscii(
  bytes: Uint8Array,
  signature: string,
  offset = 0,
): boolean {
  if (bytes.length < offset + signature.length) return false;
  for (let i = 0; i < signature.length; i += 1) {
    if (bytes[offset + i] !== signature.charCodeAt(i)) return false;
  }
  return true;
}

export function asciiAt(
  view: DataView,
  offset: number,
  length: number,
): string {
  let out = '';
  for (let i = 0; i < length; i += 1) out += String.fromCharCode(view.getUint8(offset + i));
  return out;
}

/** Concatenates chunks into one contiguous buffer. */
export function concatChunks(chunks: readonly Uint8Array[]): Uint8Array {
  let total = 0;
  for (const chunk of chunks) total += chunk.byteLength;
  const out = new Uint8Array(total);
  let offset = 0;
  for (const chunk of chunks) {
    out.set(chunk, offset);
    offset += chunk.byteLength;
  }
  return out;
}
