/**
 * SHA-1.
 *
 * Needed because Artemis PFS archives are commonly XOR-obfuscated with a
 * keystream derived from `SHA-1(table bytes)`. The Web Crypto API deliberately
 * omits SHA-1 (`crypto.subtle.digest` supports only SHA-2 and SHA-3), so the
 * algorithm has to be implemented here.
 *
 * This is used solely to *read* archives that a game shipped, never to produce
 * or verify a signature, so its cryptographic weaknesses are irrelevant to the
 * purpose. It is a straightforward transcription of FIPS 180-1.
 */

const BLOCK_BYTES = 64;

/** Reusable message schedule and state, to avoid per-call allocation. */
const schedule = new Uint32Array(80);

/**
 * Computes the SHA-1 digest of `bytes`.
 *
 * Returns the 20-byte digest as a `Uint8Array`.
 */
export function sha1(bytes: Uint8Array): Uint8Array {
  let h0 = 0x67452301;
  let h1 = 0xefcdab89;
  let h2 = 0x98badcfe;
  let h3 = 0x10325476;
  let h4 = 0xc3d2e1f0;

  // Message plus padding: `0x80`, zeroes, then the 64-bit big-endian bit length.
  const totalLength = bytes.byteLength;
  const withPadding = new Uint8Array(
    (((totalLength + 8) >> 6) + 1) * BLOCK_BYTES,
  );
  withPadding.set(bytes);
  withPadding[totalLength] = 0x80;

  const bitLength = BigInt(totalLength) * 8n;
  const lengthOffset = withPadding.byteLength - 8;
  for (let index = 0; index < 8; index += 1) {
    withPadding[lengthOffset + index] = Number(
      (bitLength >> BigInt(56 - index * 8)) & 0xffn,
    );
  }

  const view = new DataView(
    withPadding.buffer,
    withPadding.byteOffset,
    withPadding.byteLength,
  );

  for (let offset = 0; offset < withPadding.byteLength; offset += BLOCK_BYTES) {
    for (let index = 0; index < 16; index += 1) {
      schedule[index] = view.getUint32(offset + index * 4, false);
    }
    for (let index = 16; index < 80; index += 1) {
      const value =
        schedule[index - 3]! ^
        schedule[index - 8]! ^
        schedule[index - 14]! ^
        schedule[index - 16]!;
      schedule[index] = ((value << 1) | (value >>> 31)) >>> 0;
    }

    let a = h0;
    let b = h1;
    let c = h2;
    let d = h3;
    let e = h4;

    for (let index = 0; index < 80; index += 1) {
      let f: number;
      let k: number;
      if (index < 20) {
        f = (b & c) | (~b & d);
        k = 0x5a827999;
      } else if (index < 40) {
        f = b ^ c ^ d;
        k = 0x6ed9eba1;
      } else if (index < 60) {
        f = (b & c) | (b & d) | (c & d);
        k = 0x8f1bbcdc;
      } else {
        f = b ^ c ^ d;
        k = 0xca62c1d6;
      }

      const rotated = ((a << 5) | (a >>> 27)) >>> 0;
      const temp = (rotated + f + e + k + schedule[index]!) >>> 0;
      e = d;
      d = c;
      c = ((b << 30) | (b >>> 2)) >>> 0;
      b = a;
      a = temp;
    }

    h0 = (h0 + a) >>> 0;
    h1 = (h1 + b) >>> 0;
    h2 = (h2 + c) >>> 0;
    h3 = (h3 + d) >>> 0;
    h4 = (h4 + e) >>> 0;
  }

  const digest = new Uint8Array(20);
  const digestView = new DataView(digest.buffer);
  digestView.setUint32(0, h0, false);
  digestView.setUint32(4, h1, false);
  digestView.setUint32(8, h2, false);
  digestView.setUint32(12, h3, false);
  digestView.setUint32(16, h4, false);
  return digest;
}

/**
 * XORs `data` in place with `key`, repeating the key as needed.
 *
 * Returns a new buffer; the input is left untouched so a failed decode can be
 * retried against the original bytes.
 */
export function xorWithKey(data: Uint8Array, key: Uint8Array): Uint8Array {
  if (key.byteLength === 0) return data.slice();
  const out = new Uint8Array(data.byteLength);
  for (let index = 0; index < data.byteLength; index += 1) {
    out[index] = (data[index]! ^ key[index % key.byteLength]!) & 0xff;
  }
  return out;
}
