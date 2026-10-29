/**
 * NScripter scenario containers.
 *
 * NScripter ships its scenario as XOR-obfuscated or archived blobs rather than
 * plain text:
 *  - `nscript.dat`            XOR with the classic 0x84/0x85 seed
 *  - `nscript.___`            XOR with a secondary seed
 *  - `nscr_sec.dat`           an additional XOR layer used by some releases
 *  - `00.txt`, `nscript.txt`  plain text (Shift_JIS or UTF-8)
 *  - `onscript.nt2` / `nt3`   NScripter string archives
 *
 * The XOR scheme is a fixed two-byte keyed stream derived from the byte offset,
 * which is why these containers are trivially readable once the seed is known.
 */

/** XOR seeds used by the container flavours. */
const SEEDS = {
  /** The classic `nscript.dat` seed. */
  nscript: [0x84, 0x85] as const,
  /** `nscript.___` and `nscr_sec.dat`. */
  secondary: [0xa5, 0xa6] as const,
};

export type NsContainer =
  | 'nscript.dat'
  | 'nscript.___'
  | 'nscr_sec.dat'
  | 'nsa-text'
  | 'nt2'
  | 'nt3';

/**
 * Scenario containers in ONScripter's own search order: the encrypted `.dat`
 * first, then numbered text files, then the NT archives.
 */
export const NSCRIPT_CANDIDATES = [
  'nscript.dat',
  'nscript.___',
  'nscr_sec.dat',
  'onscript.nt3',
  'onscript.nt2',
  'nscript.txt',
  '00.txt',
  '0.txt',
] as const;

/** Classifies a path as one of the known containers, or null. */
export function classifyNsContainer(path: string): NsContainer | null {
  const name = path.slice(path.lastIndexOf('/') + 1).toLowerCase();
  if (name === 'nscript.dat') return 'nscript.dat';
  if (name === 'nscript.___') return 'nscript.___';
  if (name === 'nscr_sec.dat') return 'nscr_sec.dat';
  if (name === 'onscript.nt2') return 'nt2';
  if (name === 'onscript.nt3') return 'nt3';
  if (/^\d\d\.txt$/.test(name) || name === 'nscript.txt' || name === '0.txt') return 'nsa-text';
  return null;
}

/** Decodes XOR-obfuscated scenario bytes with the given two-byte seed. */
export function xorDecodeNs(
  bytes: Uint8Array,
  seed: readonly [number, number],
): Uint8Array {
  const out = new Uint8Array(bytes.byteLength);
  for (let index = 0; index < bytes.byteLength; index += 1) {
    out[index] = (bytes[index] ?? 0) ^ (index % 2 === 0 ? seed[0] : seed[1]);
  }
  return out;
}

function stripTrailingNul(bytes: Uint8Array): Uint8Array {
  let end = bytes.byteLength;
  while (end > 0 && bytes[end - 1] === 0) end -= 1;
  return bytes.subarray(0, end);
}

/**
 * Decodes one string, preferring Shift_JIS.
 *
 * NScripter scenario text is Shift_JIS in almost every release and GBK in
 * Chinese localisations; UTF-8 turns up in fan translations. Each is tried in
 * turn and the first failure falls through to the next.
 */
function decodeNsString(bytes: Uint8Array): string {
  for (const encoding of ['shift_jis', 'gbk', 'utf-8']) {
    try {
      return new TextDecoder(encoding, { fatal: encoding === 'utf-8' }).decode(bytes);
    } catch {
      // Try the next candidate.
    }
  }
  return new TextDecoder('utf-8', { fatal: false }).decode(bytes);
}

/**
 * Reads an NScripter string array (NT2/NT3).
 *
 * Layout: a 32-bit count followed by `(length, text)` pairs. NT3 interleaves a
 * pointer after each string that must be skipped to stay aligned.
 */
export function readNsStringArray(bytes: Uint8Array): readonly string[] {
  if (bytes.byteLength < 4) return [];
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  const count = view.getUint32(0, true);
  if (count === 0 || count > 5_000_000) return [];

  const out: string[] = [];
  let cursor = 4;
  for (let index = 0; index < count; index += 1) {
    if (cursor + 4 > bytes.byteLength) break;
    const length = view.getUint32(cursor, true);
    cursor += 4;
    if (length > bytes.byteLength - cursor) break;
    out.push(decodeNsString(bytes.subarray(cursor, cursor + length)));
    cursor += length;
    // NT3 stores a 32-bit pointer here; NI2 does not. Detect it by checking
    // whether the following 32-bit value could plausibly be the next length.
    if (cursor + 4 <= bytes.byteLength && index + 1 < count) {
      const nextLength = view.getUint32(cursor, true);
      if (nextLength > bytes.byteLength - cursor - 4) cursor += 4;
    }
  }
  return out;
}

export interface DecodedNscript {
  readonly text: string;
  readonly container: NsContainer;
  readonly encoding: string;
}

/**
 * Decodes an NScripter scenario container into text.
 *
 * `\\` escapes are preserved: ONScripter's parser consumes them, and stripping
 * them here would corrupt text containing literal backslashes.
 */
export function decodeNscript(
  bytes: Uint8Array,
  container: NsContainer,
): DecodedNscript {
  const payload = stripTrailingNul(bytes);
  switch (container) {
    case 'nt2':
    case 'nt3': {
      const strings = readNsStringArray(payload);
      return {
        text: strings.map(expandArchiveNewlines).join('\n'),
        container,
        encoding: `nscripter ${container} (shift_jis)`,
      };
    }
    case 'nscript.dat': {
      return {
        text: decodeNsString(xorDecodeNs(payload, SEEDS.nscript)),
        container,
        encoding: 'shift_jis (xor 84/85)',
      };
    }
    case 'nscript.___':
    case 'nscr_sec.dat': {
      return {
        text: decodeNsString(xorDecodeNs(payload, SEEDS.secondary)),
        container,
        encoding: 'shift_jis (xor a5/a6)',
      };
    }
    case 'nsa-text':
    default:
      return { text: decodeNsString(payload), container, encoding: 'plain' };
  }
}

/**
 * NT archives store `\n` as a two-character escape; the command parser needs
 * real separators to split commands.
 */
function expandArchiveNewlines(entry: string): string {
  return entry.replace(/\r\n/g, '\n');
}

/** Reports whether a payload still looks XOR-obfuscated after decoding. */
export function looksLikePlainScenario(text: string): boolean {
  // A decoded NScripter scenario is overwhelmingly printable ASCII plus
  // Shift_JIS text; a wrong seed produces long runs of control bytes.
  let control = 0;
  const sample = text.slice(0, 512);
  for (const char of sample) {
    const code = char.charCodeAt(0);
    if (code < 9 || (code > 13 && code < 32)) control += 1;
  }
  return sample.length === 0 || control / sample.length < 0.05;
}

export { SEEDS as NS_XOR_SEEDS };
