/**
 * Just enough PNG to judge a screenshot, with no dependencies.
 *
 * The browser checks need to answer two questions the console cannot: *is anything drawn*
 * (the engine's canvas is black until it paints) and *did the frame change* (after input).
 * Both are pixel questions, and `Page.captureScreenshot` hands back a PNG — so this decodes
 * the one shape Chromium emits (8-bit, non-interlaced, RGB or RGBA) and exposes two cheap
 * measures instead of a full image API.
 *
 * PNG rows are zlib-compressed and per-row filtered; the filters reference the pixel to the
 * left (`a`), the row above (`b`) and the pixel above-left (`c`). All five are implemented,
 * because Chromium picks whichever is smallest per row.
 */
import { inflateSync } from 'node:zlib';

const PNG_SIGNATURE = 0x89504e47;

/** A decoded image: interleaved 8-bit samples, `channels` per pixel. */
export function decodePng(buffer) {
  if (buffer.length < 8 || buffer.readUInt32BE(0) !== PNG_SIGNATURE) {
    throw new Error('decodePng: not a PNG');
  }
  let offset = 8;
  let width = 0;
  let height = 0;
  let bitDepth = 0;
  let colorType = 0;
  let interlace = 0;
  const idat = [];
  while (offset + 8 <= buffer.length) {
    const length = buffer.readUInt32BE(offset);
    const type = buffer.toString('ascii', offset + 4, offset + 8);
    const data = buffer.subarray(offset + 8, offset + 8 + length);
    if (type === 'IHDR') {
      width = data.readUInt32BE(0);
      height = data.readUInt32BE(4);
      bitDepth = data[8];
      colorType = data[9];
      interlace = data[12];
    } else if (type === 'IDAT') {
      idat.push(data);
    } else if (type === 'IEND') {
      break;
    }
    offset += 12 + length;
  }
  if (bitDepth !== 8) throw new Error(`decodePng: unsupported bit depth ${bitDepth}`);
  if (interlace !== 0) throw new Error('decodePng: interlaced PNGs are not supported');
  const channels = { 0: 1, 2: 3, 4: 2, 6: 4 }[colorType];
  if (channels === undefined) throw new Error(`decodePng: unsupported colour type ${colorType}`);

  const raw = inflateSync(Buffer.concat(idat));
  const stride = width * channels;
  const pixels = Buffer.alloc(height * stride);
  let position = 0;
  for (let y = 0; y < height; y += 1) {
    const filter = raw[position];
    position += 1;
    const line = raw.subarray(position, position + stride);
    position += stride;
    const above = y === 0 ? null : pixels.subarray((y - 1) * stride, y * stride);
    const row = pixels.subarray(y * stride, (y + 1) * stride);
    for (let i = 0; i < stride; i += 1) {
      const a = i >= channels ? row[i - channels] : 0;
      const b = above === null ? 0 : above[i];
      const c = above === null || i < channels ? 0 : above[i - channels];
      let value = line[i];
      switch (filter) {
        case 0:
          break;
        case 1:
          value = (value + a) & 0xff;
          break;
        case 2:
          value = (value + b) & 0xff;
          break;
        case 3:
          value = (value + ((a + b) >> 1)) & 0xff;
          break;
        case 4: {
          const p = a + b - c;
          const pa = Math.abs(p - a);
          const pb = Math.abs(p - b);
          const pc = Math.abs(p - c);
          const predictor = pa <= pb && pa <= pc ? a : pb <= pc ? b : c;
          value = (value + predictor) & 0xff;
          break;
        }
        default:
          throw new Error(`decodePng: unknown row filter ${filter}`);
      }
      row[i] = value;
    }
  }
  return { width, height, channels, pixels };
}

/**
 * Fraction of pixels that are not near-black.
 *
 * The question this answers is "has the engine painted anything": its canvas starts black
 * and a visual novel's first scene is a full-bleed background, so a few percent is already
 * decisive (text alone sits well under one percent).
 */
export function litFraction(image, threshold = 24) {
  const { pixels, channels } = image;
  let lit = 0;
  for (let i = 0; i < pixels.length; i += channels) {
    if (pixels[i] > threshold || pixels[i + 1] > threshold || pixels[i + 2] > threshold) {
      lit += 1;
    }
  }
  return lit / (pixels.length / channels);
}

/**
 * Mean colour of the image.
 *
 * Exists because a frame signature cannot prove that *input* changed the screen on an
 * animated page: a moving background changes the signature on its own. The mean colour
 * temperature does not move with animation — the demo's title menu is neutral grey, its
 * narration scene is a warm sunset behind a light text box — so a jump in `red - blue`
 * is evidence of a scene change rather than another animation frame.
 */
export function averageColour(image) {
  const { pixels, channels } = image;
  let r = 0;
  let g = 0;
  let b = 0;
  const count = pixels.length / channels;
  for (let i = 0; i < pixels.length; i += channels) {
    r += pixels[i];
    g += pixels[i + 1] ?? pixels[i];
    b += pixels[i + 2] ?? pixels[i];
  }
  return {
    r: Number((r / count).toFixed(2)),
    g: Number((g / count).toFixed(2)),
    b: Number((b / count).toFixed(2)),
    warmth: Number(((r - b) / count).toFixed(2)),
  };
}

/**
 * Fraction of pixels brighter than `threshold` out of 255.
 *
 * The measure that separates a visual novel's *menu* from its *narration*: a narration
 * scene puts a large near-white text box over the artwork, so the count of genuinely bright
 * pixels jumps, while the mean colour barely moves (both frames are dominated by grey).
 */
export function brightFraction(image, threshold = 200) {
  const { pixels, channels } = image;
  let bright = 0;
  for (let i = 0; i < pixels.length; i += channels) {
    const luma = 0.299 * pixels[i] + 0.587 * (pixels[i + 1] ?? pixels[i]) + 0.114 * (pixels[i + 2] ?? pixels[i]);
    if (luma >= threshold) bright += 1;
  }
  return bright / (pixels.length / channels);
}

/**
 * Eight-bucket luminance histogram, as fractions.
 *
 * Kept for diagnostics: a heuristic that is not backed by the distribution it is cutting
 * is a guess, and this is what makes the choice checkable from a run's own output.
 */
export function luminanceHistogram(image) {
  const { pixels, channels } = image;
  const buckets = new Array(8).fill(0);
  for (let i = 0; i < pixels.length; i += channels) {
    const luma = 0.299 * pixels[i] + 0.587 * (pixels[i + 1] ?? pixels[i]) + 0.114 * (pixels[i + 2] ?? pixels[i]);
    buckets[Math.min(7, Math.floor(luma / 32))] += 1;
  }
  const count = pixels.length / channels;
  return buckets.map((value) => Number((value / count).toFixed(4)));
}

/**
 * Statistics for a sub-rectangle, given in fractions of the image.
 *
 * This exists for one job: the KAG narration box. A visual novel's narration scene puts a
 * large, light, uniform panel over the artwork, while the title menu has only small white
 * labels over a mid-grey background. Global statistics cannot tell those apart (both scenes
 * are dark, of similar tone — measured), so the comparison is made where the box appears.
 * Several thresholds are returned because the panel's exact value is not known in advance
 * and the run's own output should show which one separates the two.
 */
export function regionStats(image, region, thresholds = [120, 150, 180]) {
  const { width, height, pixels, channels } = image;
  const x0 = Math.max(0, Math.floor(region.x0 * width));
  const x1 = Math.min(width, Math.ceil(region.x1 * width));
  const y0 = Math.max(0, Math.floor(region.y0 * height));
  const y1 = Math.min(height, Math.ceil(region.y1 * height));
  const counts = thresholds.map(() => 0);
  let sum = 0;
  let total = 0;
  for (let y = y0; y < y1; y += 1) {
    for (let x = x0; x < x1; x += 1) {
      const i = (y * width + x) * channels;
      const luma =
        0.299 * pixels[i] + 0.587 * (pixels[i + 1] ?? pixels[i]) + 0.114 * (pixels[i + 2] ?? pixels[i]);
      sum += luma;
      total += 1;
      for (let t = 0; t < thresholds.length; t += 1) {
        if (luma >= thresholds[t]) counts[t] += 1;
      }
    }
  }
  if (total === 0) return null;
  return {
    pixels: total,
    meanLuma: Number((sum / total).toFixed(2)),
    fractions: Object.fromEntries(
      thresholds.map((threshold, index) => [String(threshold), Number((counts[index] / total).toFixed(4))]),
    ),
  };
}

/**
 * A coarse fingerprint of the pixels, for "did this frame change".
 *
 * Samples a fixed stride and mixes the samples, so it is stable for a static frame and
 * differs for a redrawn one. Meant for comparing two captures of the same region, not as a
 * content hash.
 */
export function frameSignature(image, stride = 997) {
  const { pixels, channels } = image;
  let hash = 2166136261;
  for (let i = 0; i < pixels.length; i += stride * channels) {
    hash ^= pixels[i];
    hash = Math.imul(hash, 16777619);
    hash ^= pixels[Math.min(i + 1, pixels.length - 1)];
    hash = Math.imul(hash, 16777619);
  }
  return (hash >>> 0).toString(16).padStart(8, '0');
}
