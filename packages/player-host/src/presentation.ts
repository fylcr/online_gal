/**
 * Presentation geometry for widescreen ONScripter packages.
 *
 * Ported from AetherKiri's `bridge/onscripter_runtime/include/onscripter_presentation.h`.
 *
 * `ons.wide` builds keep their original 4:3 script coordinates — every `bg`,
 * `csp` and `text` command still addresses a 640×480 grid — but ask the wrapper
 * to present a widescreen viewport over the top of that surface. The visible
 * region is therefore a crop, and the engine keeps rendering at 4:3.
 *
 * The rule that matters is one-sided: crop only when the requested aspect is
 * *wider* than the source. A narrower request must not silently discard script
 * content, so it leaves the viewport at full height.
 */

export interface PresentationViewport {
  readonly x: number;
  readonly y: number;
  readonly width: number;
  readonly height: number;
}

/**
 * Resolves the visible viewport over a source surface.
 *
 * `aspectWidth`/`aspectHeight` express the presentation aspect the wrapper
 * wants (e.g. 16:9). The result is always anchored at the top of the surface,
 * which is what `ons.wide` packages expect — they compose for a cropped top
 * band and leave the lower area as letterbox filler the wrapper trims away.
 */
export function resolvePresentationViewport(
  sourceWidth: number,
  sourceHeight: number,
  aspectWidth: number,
  aspectHeight: number,
): PresentationViewport {
  const full: PresentationViewport = {
    x: 0,
    y: 0,
    width: sourceWidth,
    height: sourceHeight,
  };

  if (
    sourceWidth === 0 ||
    sourceHeight === 0 ||
    aspectWidth === 0 ||
    aspectHeight === 0
  ) {
    return full;
  }

  // Compare sourceWidth * aspectHeight against sourceHeight * aspectWidth to
  // avoid the rounding a division would introduce.
  const sourceAtTargetHeight = sourceWidth * aspectHeight;
  const targetAtSourceHeight = sourceHeight * aspectWidth;
  if (sourceAtTargetHeight >= targetAtSourceHeight) {
    // The source is already at least as wide as requested: nothing to crop.
    return full;
  }

  const croppedHeight = Math.floor(sourceAtTargetHeight / aspectWidth);
  if (croppedHeight > 0 && croppedHeight < sourceHeight) {
    return { x: 0, y: 0, width: sourceWidth, height: croppedHeight };
  }
  return full;
}

/**
 * Clamps a scroll offset while a tall surface is shown through a short viewport.
 *
 * Companion to `resolvePresentationViewport`, used by packages that scroll a
 * taller-than-viewport stage rather than cropping it.
 */
export function resolvePresentationScroll(
  sourceHeight: number,
  viewportY: number,
  viewportHeight: number,
  currentScroll: number,
  delta: number,
): number {
  if (
    sourceHeight === 0 ||
    viewportHeight === 0 ||
    viewportY >= sourceHeight ||
    viewportHeight > sourceHeight - viewportY
  ) {
    return 0;
  }
  const maxScroll = sourceHeight - viewportY - viewportHeight;
  const requested = currentScroll + delta;
  if (requested <= 0) return 0;
  if (requested >= maxScroll) return maxScroll;
  return requested;
}

/** Parses an aspect string such as `16:9` or `4:3`. */
export function parseAspect(
  value: string | undefined,
): { width: number; height: number } | null {
  if (!value) return null;
  const match = /^(\d+)\s*[:x/]\s*(\d+)$/.exec(value.trim());
  if (!match) return null;
  const width = Number(match[1]);
  const height = Number(match[2]);
  if (!Number.isFinite(width) || !Number.isFinite(height) || width <= 0 || height <= 0) {
    return null;
  }
  return { width, height };
}

/**
 * Detects the presentation aspect a game asks for.
 *
 * ONScripter records `wide` support in the game's configuration; the browser
 * reads it from the scenario's `*define` block or the per-game options, so this
 * only normalises the several spellings into an aspect pair.
 */
export function detectRequestedAspect(
  candidates: readonly (string | undefined)[],
): { width: number; height: number } | null {
  for (const candidate of candidates) {
    const parsed = parseAspect(candidate);
    if (parsed) return parsed;
  }
  return null;
}
