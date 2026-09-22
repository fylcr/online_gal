import { extname } from '@online-gal/vfs';

/** A decoded image plus its intrinsic size. */
export interface DecodedImage {
  readonly source: CanvasImageSource;
  readonly width: number;
  readonly height: number;
  /** RGBA pixels; present only when the decoder had to produce them. */
  readonly rgba?: Uint8Array;
  readonly codec: string;
}

/** Pluggable decoder for a format the browser cannot handle natively. */
export interface ImageDecoder {
  readonly name: string;
  /** Extensions (with dot, lowercase) this decoder claims. */
  readonly extensions: readonly string[];
  decode(bytes: Uint8Array): Promise<DecodedImage>;
}

/**
 * Formats every mainstream browser already decodes.
 *
 * KAG/KiriKiri titles overwhelmingly ship PNG/JPEG, so the fast path is the
 * browser's own decoder and custom codecs only handle the long tail.
 */
const NATIVE_EXTENSIONS = new Set([
  '.png',
  '.jpg',
  '.jpeg',
  '.jfif',
  '.gif',
  '.webp',
  '.avif',
  '.bmp',
  '.ico',
  '.svg',
]);

const registry: ImageDecoder[] = [];

/** Registers a custom decoder. Later registrations win over native handling. */
export function registerImageDecoder(decoder: ImageDecoder): void {
  registry.unshift(decoder);
}

export function listImageDecoders(): readonly string[] {
  return registry.map((decoder) => decoder.name);
}

/** Heuristic: does the payload look like an uncompressed Truevision TGA? */
function looksLikeTga(bytes: Uint8Array): boolean {
  if (bytes.byteLength < 18) return false;
  const imageType = bytes[2]!;
  // 1 = colour-mapped, 2 = truecolour, 3 = greyscale, plus RLE variants 9/10/11.
  if (![1, 2, 3, 9, 10, 11].includes(imageType)) return false;
  const depth = bytes[16]!;
  return depth === 8 || depth === 15 || depth === 16 || depth === 24 || depth === 32;
}

/** Truevision TGA / TARGA decoder — the classic KAG background container. */
export const tgaDecoder: ImageDecoder = {
  name: 'tga',
  extensions: ['.tga', '.targa', '.icb', '.vda', '.vst'],
  async decode(bytes: Uint8Array): Promise<DecodedImage> {
    const image = decodeTgaToRgba(bytes);
    return {
      source: await rgbaToImageSource(image.rgba, image.width, image.height),
      width: image.width,
      height: image.height,
      rgba: image.rgba,
      codec: 'tga',
    };
  },
};

registry.push(tgaDecoder);

let bitmapSupport: boolean | null = null;

async function canUseCreateImageBitmap(): Promise<boolean> {
  if (bitmapSupport !== null) return bitmapSupport;
  if (typeof createImageBitmap !== 'function') {
    bitmapSupport = false;
    return false;
  }
  try {
    // A 1x1 GIF exercises the code path without needing a fake file.
    const probe = new Blob([
      new Uint8Array([
        0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x01, 0x00, 0x01, 0x00, 0x80, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x21, 0xf9, 0x04, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00,
        0x00, 0x02, 0x02, 0x44, 0x01, 0x00, 0x3b,
      ]),
    ]);
    const bitmap = await createImageBitmap(probe);
    bitmap.close();
    bitmapSupport = true;
  } catch {
    bitmapSupport = false;
  }
  return bitmapSupport;
}

export interface DecodeOptions {
  /** Path is used only to pick a decoder; the bytes are authoritative. */
  readonly path?: string;
}

/**
 * Decodes image bytes into something drawable.
 *
 * Order: a registered custom decoder whose extension matches, then the
 * browser's own decoder, then a magic-number sniff across all custom
 * decoders. Failures are explicit so the runtime can substitute a placeholder
 * instead of silently drawing nothing.
 */
export async function decodeImage(
  bytes: Uint8Array,
  options: DecodeOptions = {},
): Promise<DecodedImage> {
  const extension = options.path ? extname(options.path) : '';

  for (const decoder of registry) {
    if (extension && decoder.extensions.includes(extension)) {
      return decoder.decode(bytes);
    }
  }

  if ((!extension || NATIVE_EXTENSIONS.has(extension)) && (await canUseCreateImageBitmap())) {
    try {
      const blob = new Blob([bytes as BlobPart]);
      const bitmap = await createImageBitmap(blob);
      return {
        source: bitmap,
        width: bitmap.width,
        height: bitmap.height,
        codec: extension ? extension.slice(1) : 'bitmap',
      };
    } catch (error) {
      // Fall through to the custom decoders before giving up.
      if (extension && NATIVE_EXTENSIONS.has(extension)) {
        throw new Error(
          `failed to decode "${options.path ?? 'image'}" (${String(error)})`,
        );
      }
    }
  }

  if (looksLikeTga(bytes)) return tgaDecoder.decode(bytes);

  for (const decoder of registry) {
    if (decoder.name === 'tga') continue;
    try {
      return await decoder.decode(bytes);
    } catch {
      // Try the next one.
    }
  }

  throw new Error(
    `no decoder available for "${options.path ?? 'image'}" — supported: ${[
      ...NATIVE_EXTENSIONS,
      ...registry.flatMap((decoder) => decoder.extensions),
    ].join(', ')}`,
  );
}

/** Wraps raw RGBA pixels in an `ImageBitmap`. */
export async function rgbaToImageSource(
  rgba: Uint8Array,
  width: number,
  height: number,
): Promise<CanvasImageSource> {
  if (typeof createImageBitmap === 'function') {
    // Copy into a fresh buffer so the clamped view is backed by a plain
    // ArrayBuffer, which is what `ImageData` requires.
    const clamped = new Uint8ClampedArray(rgba.byteLength);
    clamped.set(rgba);
    const data = new ImageData(clamped, width, height);
    return createImageBitmap(data);
  }
  throw new Error('createImageBitmap is unavailable; cannot build an image source');
}

interface TgaImage {
  readonly rgba: Uint8Array;
  readonly width: number;
  readonly height: number;
}

/** Decodes TGA (uncompressed and RLE, 8/15/16/24/32 bpp) to top-down RGBA. */
export function decodeTgaToRgba(bytes: Uint8Array): TgaImage {
  if (bytes.byteLength < 18) throw new Error('TGA header is truncated');
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);

  const idLength = view.getUint8(0);
  const colourMapType = view.getUint8(1);
  const imageType = view.getUint8(2);
  const colourMapOrigin = view.getUint16(3, true);
  const colourMapLength = view.getUint16(5, true);
  const colourMapDepth = view.getUint8(7);
  const width = view.getUint16(12, true);
  const height = view.getUint16(14, true);
  const pixelDepth = view.getUint8(16);
  const descriptor = view.getUint8(17);

  if (width === 0 || height === 0) throw new Error('TGA has zero dimensions');

  const bytesPerPixel = Math.ceil(pixelDepth / 8);
  let cursor = 18 + idLength;

  const colourMap = new Uint8Array(colourMapLength * 4);
  if (colourMapType === 1) {
    const entryBytes = Math.ceil(colourMapDepth / 8);
    for (let i = 0; i < colourMapLength; i += 1) {
      const base = cursor + i * entryBytes;
      const [r, g, b, a] = readTgaPixel(
        bytes,
        base,
        colourMapDepth,
        false,
      );
      colourMap[i * 4] = r;
      colourMap[i * 4 + 1] = g;
      colourMap[i * 4 + 2] = b;
      colourMap[i * 4 + 3] = a;
    }
    cursor += colourMapLength * entryBytes;
  }

  const isRle = imageType === 9 || imageType === 10 || imageType === 11;
  const baseType = isRle ? imageType - 8 : imageType;
  const topDown = (descriptor & 0x20) !== 0;

  const pixelCount = width * height;
  const rgba = new Uint8Array(pixelCount * 4);
  let written = 0;

  const emit = (pixelIndex: number, dataOffset: number): void => {
    if (pixelIndex >= pixelCount) return;
    let r = 0;
    let g = 0;
    let b = 0;
    let a = 255;

    if (baseType === 1) {
      // Colour-mapped: the pixel is an index into the palette.
      const index =
        bytesPerPixel === 1
          ? bytes[dataOffset]!
          : view.getUint16(dataOffset, true);
      const slot = (index - colourMapOrigin) * 4;
      r = colourMap[slot] ?? 0;
      g = colourMap[slot + 1] ?? 0;
      b = colourMap[slot + 2] ?? 0;
      a = colourMap[slot + 3] ?? 255;
    } else if (baseType === 3) {
      const grey = bytes[dataOffset]!;
      r = grey;
      g = grey;
      b = grey;
      a = 255;
    } else {
      const pixel = readTgaPixel(
        bytes,
        dataOffset,
        pixelDepth,
        (descriptor & 0x0f) !== 0,
      );
      r = pixel[0];
      g = pixel[1];
      b = pixel[2];
      a = pixel[3];
    }

    const row = Math.floor(pixelIndex / width);
    const column = pixelIndex % width;
    const targetRow = topDown ? row : height - 1 - row;
    const target = (targetRow * width + column) * 4;
    rgba[target] = r;
    rgba[target + 1] = g;
    rgba[target + 2] = b;
    rgba[target + 3] = a;
  };

  if (!isRle) {
    for (let i = 0; i < pixelCount; i += 1) {
      emit(i, cursor + i * bytesPerPixel);
    }
  } else {
    while (written < pixelCount && cursor < bytes.byteLength) {
      const packet = bytes[cursor]!;
      cursor += 1;
      const count = (packet & 0x7f) + 1;
      if (packet & 0x80) {
        for (let i = 0; i < count && written < pixelCount; i += 1) {
          emit(written, cursor);
          written += 1;
        }
        cursor += bytesPerPixel;
      } else {
        for (let i = 0; i < count && written < pixelCount; i += 1) {
          emit(written, cursor + i * bytesPerPixel);
          written += 1;
        }
        cursor += count * bytesPerPixel;
      }
    }
  }

  return { rgba, width, height };
}

function readTgaPixel(
  bytes: Uint8Array,
  offset: number,
  depth: number,
  hasAlphaBits: boolean,
): [number, number, number, number] {
  if (depth === 8) {
    const grey = bytes[offset] ?? 0;
    return [grey, grey, grey, 255];
  }
  if (depth === 15 || depth === 16) {
    const value = (bytes[offset] ?? 0) | ((bytes[offset + 1] ?? 0) << 8);
    const r = ((value >> 10) & 0x1f) * 255 / 31;
    const g = ((value >> 5) & 0x1f) * 255 / 31;
    const b = (value & 0x1f) * 255 / 31;
    // TGA stores a single attribute bit; honour it only when the descriptor
    // advertised alpha, otherwise 16bpp is opaque.
    const a = hasAlphaBits ? (value & 0x8000 ? 255 : 0) : 255;
    return [Math.round(r), Math.round(g), Math.round(b), a];
  }
  if (depth === 24) {
    return [
      bytes[offset + 2] ?? 0,
      bytes[offset + 1] ?? 0,
      bytes[offset] ?? 0,
      255,
    ];
  }
  if (depth === 32) {
    return [
      bytes[offset + 2] ?? 0,
      bytes[offset + 1] ?? 0,
      bytes[offset] ?? 0,
      bytes[offset + 3] ?? 255,
    ];
  }
  throw new Error(`unsupported TGA pixel depth: ${depth}`);
}
