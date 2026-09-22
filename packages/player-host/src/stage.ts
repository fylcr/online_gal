import type { DecodedImage } from '@online-gal/image-codec';
import { easingFor } from './loop.js';

/** How a layer's pixels are blended onto the frame. */
export type BlendMode =
  | 'normal'
  | 'add'
  | 'sub'
  | 'multiply'
  | 'screen'
  | 'overlay'
  | 'darken'
  | 'lighten';

/** A single source of pixels on the stage. */
export interface StageLayer {
  readonly id: string;
  /** Current image, or null when the layer has nothing to draw. */
  image: DecodedImage | null;
  /** Pixel rectangle of the source to draw (sprite-sheet support). */
  src: { x: number; y: number; width: number; height: number } | null;
  x: number;
  y: number;
  /** Destination size; defaults to the image's intrinsic size. */
  width: number;
  height: number;
  opacity: number;
  visible: boolean;
  blend: BlendMode;
  /** Tint colour applied as a multiply, or null for none. */
  tint: string | null;
  /** 0..1 desaturation used by KAG's colour filters. */
  saturate: number;
  /** Brightness multiplier, 1 = unchanged. */
  brightness: number;
  /** Region clipping; null means the whole surface. */
  clip: { x: number; y: number; width: number; height: number } | null;
  /** Per-layer transform origin, relative to the layer rect. */
  origin: { x: number; y: number };
  scaleX: number;
  scaleY: number;
  rotation: number;
  zIndex: number;
}

export function createLayer(id: string): StageLayer {
  return {
    id,
    image: null,
    src: null,
    x: 0,
    y: 0,
    width: 0,
    height: 0,
    opacity: 1,
    visible: true,
    blend: 'normal',
    tint: null,
    saturate: 1,
    brightness: 1,
    clip: null,
    origin: { x: 0, y: 0 },
    scaleX: 1,
    scaleY: 1,
    rotation: 0,
    zIndex: 0,
  };
}

type TransitionProperty =
  | 'x'
  | 'y'
  | 'opacity'
  | 'width'
  | 'height'
  | 'scaleX'
  | 'scaleY'
  | 'rotation'
  | 'saturate'
  | 'brightness';

interface Transition {
  readonly target: StageLayer;
  readonly property: TransitionProperty;
  readonly from: number;
  readonly to: number;
  readonly durationMs: number;
  elapsed: number;
  readonly easing: (t: number) => number;
  readonly onComplete?: () => void;
}

/**
 * The scene compositor.
 *
 * Layers are drawn in `zIndex` then insertion order, which is the ordering KAG
 * gives its `base`/`layer`/`message`/`fore` stacks. Transitions are numeric
 * tweens over layer properties rather than pixel cross-fades, so an animated
 * sprite costs the same as a static one.
 */
export class Stage {
  private readonly layers = new Map<string, StageLayer>();
  private order: string[] = [];
  private readonly transitions: Transition[] = [];
  /** Screen shake offset, applied to every layer. */
  private shake = { x: 0, y: 0, remainingMs: 0, magnitude: 0 };
  /** Camera offset, used by pan effects. */
  private camera = { x: 0, y: 0, zoom: 1 };

  constructor(
    readonly width: number,
    readonly height: number,
  ) {}

  /** Gets a layer, creating it when missing (KAG layers are implicit). */
  layer(id: string, zIndex = 0): StageLayer {
    let layer = this.layers.get(id);
    if (!layer) {
      layer = createLayer(id);
      layer.zIndex = zIndex;
      this.layers.set(id, layer);
      this.order.push(id);
    }
    return layer;
  }

  get(id: string): StageLayer | undefined {
    return this.layers.get(id);
  }

  has(id: string): boolean {
    return this.layers.has(id);
  }

  remove(id: string): void {
    this.layers.delete(id);
    this.order = this.order.filter((entry) => entry !== id);
    for (let i = this.transitions.length - 1; i >= 0; i -= 1) {
      if (this.transitions[i]!.target.id === id) this.transitions.splice(i, 1);
    }
  }

  /** Clears a layer's image but keeps its transform, matching KAG's `cl` tag. */
  clear(id: string): void {
    const layer = this.layer(id);
    layer.image = null;
    layer.src = null;
  }

  /** Ordered list of layers with content, back to front. */
  visibleLayers(): readonly StageLayer[] {
    return this.order
      .map((id) => this.layers.get(id)!)
      .filter((layer): layer is StageLayer => Boolean(layer))
      .sort((a, b) => a.zIndex - b.zIndex)
      .filter((layer) => layer.visible && layer.image && layer.opacity > 0.001);
  }

  /** Sizes a layer to its image, the default for a freshly assigned sprite. */
  fitToImage(layer: StageLayer): void {
    if (!layer.image) return;
    const source = layer.src;
    layer.width = source ? source.width : layer.image.width;
    layer.height = source ? source.height : layer.image.height;
  }

  /**
   * Starts a tween. Several tweens may run on one layer at once (a slide plus
   * a fade, for instance), which is why transitions are tracked per property.
   */
  animate(
    target: StageLayer,
    values: Partial<Record<TransitionProperty, number>>,
    durationMs: number,
    easing = 'easeInOut',
  ): void {
    const easingFn = easingFor(easing);
    for (const [property, to] of Object.entries(values)) {
      if (to === undefined) continue;
      const key = property as TransitionProperty;
      // Replace any in-flight tween for the same property.
      for (let i = this.transitions.length - 1; i >= 0; i -= 1) {
        const existing = this.transitions[i]!;
        if (existing.target === target && existing.property === key) {
          this.transitions.splice(i, 1);
        }
      }
      if (durationMs <= 0) {
        (target[key] as number) = to;
        continue;
      }
      this.transitions.push({
        target,
        property: key,
        from: target[key] as number,
        to,
        durationMs,
        elapsed: 0,
        easing: easingFn,
      });
    }
  }

  /** True while any transition is still running. */
  get isAnimating(): boolean {
    return this.transitions.length > 0;
  }

  /** Waits for all current transitions; resolves immediately when idle. */
  async waitForTransitions(signal?: AbortSignal): Promise<void> {
    while (this.transitions.length > 0) {
      if (signal?.aborted) return;
      await new Promise((resolve) => setTimeout(resolve, 16));
    }
  }

  cancelTransitions(layerId?: string): void {
    for (let i = this.transitions.length - 1; i >= 0; i -= 1) {
      const transition = this.transitions[i]!;
      if (!layerId || transition.target.id === layerId) {
        (transition.target[transition.property] as number) = transition.to;
        this.transitions.splice(i, 1);
      }
    }
  }

  startShake(magnitude: number, durationMs: number): void {
    this.shake = { x: 0, y: 0, remainingMs: durationMs, magnitude };
  }

  setCamera(x: number, y: number, zoom = 1): void {
    this.camera = { x, y, zoom };
  }

  get cameraState(): { x: number; y: number; zoom: number } {
    return { ...this.camera };
  }

  /** Advances transitions and effects. */
  update(deltaMs: number): void {
    for (let i = this.transitions.length - 1; i >= 0; i -= 1) {
      const transition = this.transitions[i]!;
      transition.elapsed += deltaMs;
      const progress = Math.min(1, transition.elapsed / transition.durationMs);
      const eased = transition.easing(progress);
      (transition.target[transition.property] as number) =
        transition.from + (transition.to - transition.from) * eased;
      if (progress >= 1) {
        this.transitions.splice(i, 1);
        transition.onComplete?.();
      }
    }

    if (this.shake.remainingMs > 0) {
      this.shake.remainingMs -= deltaMs;
      const magnitude = this.shake.magnitude * (this.shake.remainingMs > 0 ? 1 : 0);
      this.shake.x = (Math.random() * 2 - 1) * magnitude;
      this.shake.y = (Math.random() * 2 - 1) * magnitude;
    } else {
      this.shake.x = 0;
      this.shake.y = 0;
    }
  }

  /** Draws every visible layer into `context`. */
  render(context: CanvasRenderingContext2D): void {
    context.save();
    context.translate(
      this.shake.x + this.camera.x,
      this.shake.y + this.camera.y,
    );
    if (this.camera.zoom !== 1) {
      context.translate(this.width / 2, this.height / 2);
      context.scale(this.camera.zoom, this.camera.zoom);
      context.translate(-this.width / 2, -this.height / 2);
    }

    for (const layer of this.visibleLayers()) {
      this.drawLayer(context, layer);
    }

    context.restore();
  }

  private drawLayer(context: CanvasRenderingContext2D, layer: StageLayer): void {
    const image = layer.image;
    if (!image) return;

    context.save();
    context.globalAlpha = Math.min(1, Math.max(0, layer.opacity));
    context.globalCompositeOperation = compositeFor(layer.blend);

    if (layer.tint || layer.saturate !== 1 || layer.brightness !== 1) {
      context.filter = filterFor(layer);
    }

    if (layer.clip) {
      context.beginPath();
      context.rect(layer.clip.x, layer.clip.y, layer.clip.width, layer.clip.height);
      context.clip();
    }

    const width = layer.width || (layer.src?.width ?? image.width);
    const height = layer.height || (layer.src?.height ?? image.height);
    const originX = layer.x + layer.origin.x;
    const originY = layer.y + layer.origin.y;

    if (layer.scaleX !== 1 || layer.scaleY !== 1 || layer.rotation !== 0) {
      context.translate(originX, originY);
      context.rotate((layer.rotation * Math.PI) / 180);
      context.scale(layer.scaleX, layer.scaleY);
      context.translate(-layer.origin.x, -layer.origin.y);
      this.blit(context, image, layer, 0, 0, width, height);
    } else {
      this.blit(context, image, layer, layer.x, layer.y, width, height);
    }

    context.restore();
  }

  private blit(
    context: CanvasRenderingContext2D,
    image: DecodedImage,
    layer: StageLayer,
    x: number,
    y: number,
    width: number,
    height: number,
  ): void {
    if (layer.src) {
      context.drawImage(
        image.source,
        layer.src.x,
        layer.src.y,
        layer.src.width,
        layer.src.height,
        x,
        y,
        width,
        height,
      );
    } else {
      context.drawImage(image.source, x, y, width, height);
    }
  }

  clearAll(): void {
    this.layers.clear();
    this.order = [];
    this.transitions.length = 0;
  }

  /** Serialisable snapshot of layer transforms (images are re-resolved later). */
  describe(): readonly { id: string; x: number; y: number; opacity: number }[] {
    return this.order.map((id) => {
      const layer = this.layers.get(id)!;
      return { id, x: layer.x, y: layer.y, opacity: layer.opacity };
    });
  }
}

function compositeFor(blend: BlendMode): GlobalCompositeOperation {
  switch (blend) {
    case 'add':
      return 'lighter';
    case 'multiply':
      return 'multiply';
    case 'screen':
      return 'screen';
    case 'overlay':
      return 'overlay';
    case 'darken':
      return 'darken';
    case 'lighten':
      return 'lighten';
    case 'sub': {
      // Canvas has no subtractive blend; `difference` is the closest built-in.
      return 'difference';
    }
    default:
      return 'source-over';
  }
}

function filterFor(layer: StageLayer): string {
  const parts: string[] = [];
  if (layer.brightness !== 1) parts.push(`brightness(${layer.brightness})`);
  if (layer.saturate !== 1) parts.push(`saturate(${layer.saturate})`);
  // Tint is approximated by a hue-preserving colour overlay applied through a
  // CSS filter, which keeps tinting allocation-free per frame.
  if (layer.tint) parts.push(`drop-shadow(0 0 0 ${layer.tint})`);
  return parts.join(' ') || 'none';
}
