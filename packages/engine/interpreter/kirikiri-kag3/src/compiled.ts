/**
 * Compiled KAG/KiriKiri script containers.
 *
 * KiriKiri can ship its KAG scripts as plain UTF-16 `.ks`/`.tjs` text files or
 * as compiled `.ks` binaries obfuscated with a per-file key stored in the
 * 16-byte header. Scenarios, message text and macros are all carried this way,
 * so reading them is a prerequisite for running any packaged game.
 */

import { inflateZlib } from '@online-gal/vfs/archives';

/** Magic numbers the KiriKiri script compiler writes. */
const SIGNATURE_COMPRESSED = 'TJS\u0000\u0001\u0000';
const SIGNATURE_UNCOMPRESSED = 'TJS\u0000\u0000\u0000';

export interface DecodedScript {
  readonly text: string;
  /** Encoding the payload was decoded with, for diagnostics. */
  readonly encoding: string;
  /** True when the script was a compiled binary rather than plain text. */
  readonly compiled: boolean;
  /**
   * The payload after any container unwrapping, before text decoding.
   *
   * Present for compiled scripts. A compiled `.ks` may hold either the original
   * TJS2 source or TJS2 *bytecode* behind the same header, and only the raw bytes
   * can tell the two apart — which is what the bytecode analyser needs.
   */
  readonly bytes?: Uint8Array;
}

export class CompiledScriptError extends Error {
  constructor(
    message: string,
    readonly path?: string,
  ) {
    super(path ? `${path}: ${message}` : message);
    this.name = 'CompiledScriptError';
  }
}

/**
 * Detects whether a payload is text or a compiled script.
 *
 * Plain KAG scripts are frequently UTF-16LE with a BOM, and Shift_JIS without
 * one. Compiled scripts always begin with the `TJS\0` signature.
 */
export function sniffScriptFormat(bytes: Uint8Array): 'compiled' | 'utf16' | 'utf8' | 'sjis' {
  if (
    bytes.byteLength >= 4 &&
    bytes[0] === 0x54 &&
    bytes[1] === 0x4a &&
    bytes[2] === 0x53 &&
    bytes[3] === 0x00
  ) {
    return 'compiled';
  }
  if (bytes.byteLength >= 2 && bytes[0] === 0xff && bytes[1] === 0xfe) return 'utf16';
  if (bytes.byteLength >= 2 && bytes[0] === 0xfe && bytes[1] === 0xff) return 'utf16';
  // Heuristic: a NUL byte in the first 64 bytes of otherwise ASCII-ish content
  // means UTF-16 without a BOM.
  const limit = Math.min(bytes.byteLength, 64);
  let nulCount = 0;
  for (let i = 0; i < limit; i += 1) if (bytes[i] === 0) nulCount += 1;
  if (nulCount > limit / 4) return 'utf16';
  return 'utf8';
}

function decodeUtf16(bytes: Uint8Array, bigEndian = false): string {
  let offset = 0;
  if (bytes.byteLength >= 2) {
    if (bytes[0] === 0xff && bytes[1] === 0xfe) offset = 2;
    else if (bytes[0] === 0xfe && bytes[1] === 0xff) {
      offset = 2;
      bigEndian = true;
    }
  }
  let out = '';
  for (let i = offset; i + 1 < bytes.byteLength; i += 2) {
    const code = bigEndian
      ? (bytes[i]! << 8) | bytes[i + 1]!
      : bytes[i]! | (bytes[i + 1]! << 8);
    out += String.fromCharCode(code);
  }
  return out;
}

/** Decodes plain text, choosing UTF-16 or the best 8-bit encoding. */
export function decodePlainScript(bytes: Uint8Array, preferred: readonly string[] = []): DecodedScript {
  const format = sniffScriptFormat(bytes);
  if (format === 'utf16') {
    return { text: decodeUtf16(bytes), encoding: 'utf-16le', compiled: false };
  }
  const candidates = [...preferred, 'utf-8', 'shift_jis', 'gbk'];
  for (const encoding of candidates) {
    try {
      const text = new TextDecoder(encoding, { fatal: true }).decode(bytes);
      return { text, encoding, compiled: false };
    } catch {
      // Try the next candidate.
    }
  }
  return {
    text: new TextDecoder('utf-8', { fatal: false }).decode(bytes),
    encoding: 'utf-8 (lossy)',
    compiled: false,
  };
}

/**
 * Decodes a compiled `.ks` payload.
 *
 * Layout: a 16-byte header (signature, flags, original size, key length, key),
 * then the payload XORed with the header's key. Some builds store the payload
 * zlib-compressed; when the declared original size exceeds the remaining bytes
 * the payload is inflated.
 */
export function decodeCompiledScript(bytes: Uint8Array, path?: string): DecodedScript {
  if (bytes.byteLength < 16) {
    throw new CompiledScriptError('compiled script header is truncated', path);
  }
  const signature = String.fromCharCode(...bytes.subarray(0, 4));
  if (signature !== 'TJS\u0000') {
    throw new CompiledScriptError('not a compiled KiriKiri script', path);
  }

  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  const flags = view.getUint8(4);
  const compressed = (flags & 0x01) !== 0;
  const originalSize = view.getUint32(8, true);
  const keyLength = view.getUint8(12) ?? 0;

  if (keyLength > bytes.byteLength - 16) {
    throw new CompiledScriptError('compiled script key length is out of range', path);
  }
  const key = bytes.subarray(16, 16 + keyLength);
  const payload = bytes.subarray(16 + keyLength);

  const decoded = new Uint8Array(payload.byteLength);
  for (let i = 0; i < payload.byteLength; i += 1) {
    decoded[i] = payload[i]! ^ (key[i % key.byteLength] ?? 0);
  }

  const finish = (data: Uint8Array): DecodedScript => {
    const plain = decodePlainScript(data);
    return { ...plain, compiled: true, bytes: data };
  };

  if (!compressed && !looksCompressed(decoded)) {
    return finish(decoded);
  }

  // Decompression is asynchronous, so this path is exposed separately.
  throw new DeferredDecompression(decoded, originalSize, path);
}

/** Internal marker: the payload needs async inflation before decoding. */
export class DeferredDecompression extends Error {
  constructor(
    readonly payload: Uint8Array,
    readonly originalSize: number,
    path?: string,
  ) {
    super(`compiled script requires decompression${path ? ` (${path})` : ''}`);
    this.name = 'DeferredDecompression';
  }
}

/** Cheap zlib header check: CMF/FLG with a valid mod-31 check. */
function looksCompressed(bytes: Uint8Array): boolean {
  if (bytes.byteLength < 2) return false;
  const cmf = bytes[0]!;
  const flg = bytes[1]!;
  if ((cmf & 0x0f) !== 8) return false;
  return ((cmf << 8) | flg) % 31 === 0;
}

/**
 * Decodes a script payload, handling compiled, compressed and plain forms.
 * This is the entry point runtimes should call.
 */
export async function loadScriptBytes(
  bytes: Uint8Array,
  path?: string,
): Promise<DecodedScript> {
  const format = sniffScriptFormat(bytes);
  if (format !== 'compiled') return decodePlainScript(bytes);

  try {
    return decodeCompiledScript(bytes, path);
  } catch (error) {
    if (error instanceof DeferredDecompression) {
      const inflated = await inflateZlib(error.payload);
      const plain = decodePlainScript(inflated);
      return { ...plain, compiled: true, bytes: inflated };
    }
    throw error;
  }
}

/** Extracts the XOR key from a compiled script header, for `xp3` reuse. */
export function compiledScriptKey(bytes: Uint8Array): bigint | null {
  if (bytes.byteLength < 16) return null;
  if (!(bytes[0] === 0x54 && bytes[1] === 0x4a && bytes[2] === 0x53)) return null;
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  const keyLength = view.getUint8(12) ?? 0;
  if (keyLength === 0 || keyLength > 8 || 16 + keyLength > bytes.byteLength) return null;
  let key = 0n;
  for (let i = 0; i < keyLength; i += 1) {
    key |= BigInt(bytes[16 + i]!) << BigInt(8 * i);
  }
  return key;
}

export { SIGNATURE_COMPRESSED, SIGNATURE_UNCOMPRESSED };
