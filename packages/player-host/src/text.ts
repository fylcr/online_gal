/**
 * Text layout and rendering for visual-novel message boxes.
 *
 * Handles the features KAG3 and ONScripter actually use: character-by-character
 * reveal, ruby (furigana) annotations, inline colour and size changes, bold
 * text, and CJK-aware line breaking that can start a new line anywhere except
 * before closing punctuation.
 */

export interface TextStyle {
  fontFamily: string;
  fontSize: number;
  fontWeight: string;
  fontStyle: string;
  color: string;
  outlineColor: string;
  outlineWidth: number;
  shadowColor: string;
  shadowBlur: number;
  shadowOffsetX: number;
  shadowOffsetY: number;
  lineSpacing: number;
  letterSpacing: number;
  rubySize: number;
}

export const DEFAULT_TEXT_STYLE: TextStyle = {
  fontFamily:
    '"Noto Sans CJK SC", "Noto Sans CJK JP", "Source Han Sans SC", "Microsoft YaHei", "Hiragino Sans", sans-serif',
  fontSize: 26,
  fontWeight: '400',
  fontStyle: 'normal',
  color: '#ffffff',
  outlineColor: 'rgba(0, 0, 0, 0.85)',
  outlineWidth: 2,
  shadowColor: 'rgba(0, 0, 0, 0.55)',
  shadowBlur: 4,
  shadowOffsetX: 0,
  shadowOffsetY: 1,
  lineSpacing: 1.35,
  letterSpacing: 0,
  rubySize: 0.5,
};

export interface TextRun {
  readonly text: string;
  /** Ruby annotation rendered above `text`. */
  readonly ruby?: string;
  /** Character offset in the original source, used for reveal effects. */
  readonly offset: number;
  readonly style: Partial<TextStyle>;
}

export interface LayoutOptions {
  /** Maximum line width in pixels. */
  readonly maxWidth: number;
  /** Emit a line break before a period/exclamation when there is room. */
  readonly kinsoku?: boolean;
}

/** Characters that may not begin a line (Japanese/Chinese line-break rules). */
const NO_LINE_START = new Set([
  '、', '。', '，', '．', '・', '：', '；', '？', '！', '）', '」', '』', '】', '〉', '》',
  'ぁ', 'ぃ', 'ぅ', 'ぇ', 'ぉ', 'っ', 'ゃ', 'ゅ', 'ょ', 'ゎ', 'ァ', 'ィ', 'ゥ', 'ェ',
  'ォ', 'ッ', 'ャ', 'ュ', 'ョ', 'ヮ', 'ー', '々', '〜', '…', '‥', ')', ']', '}', ',',
  '.', '?', '!', ':', ';', '%', '′', '″', '°', '℃',
]);

/** Characters that may not end a line. */
const NO_LINE_END = new Set(['（', '「', '『', '【', '〈', '《', '(', '[', '{']);

export interface PlacedGlyph {
  readonly char: string;
  readonly x: number;
  readonly y: number;
  readonly width: number;
  readonly ascent: number;
  readonly style: TextStyle;
  readonly offset: number;
  /** Ruby text drawn above this glyph. */
  readonly ruby?: string;
  readonly rubyWidth?: number;
  readonly rubyStyle?: TextStyle;
}

export interface TextLayout {
  readonly glyphs: readonly PlacedGlyph[];
  readonly lines: number;
  readonly lineHeight: number;
  readonly width: number;
  readonly height: number;
  readonly totalCharacters: number;
}

interface MeasuredGlyph {
  readonly char: string;
  readonly width: number;
  readonly style: TextStyle;
  readonly offset: number;
  readonly ruby?: string;
  readonly rubyWidth: number;
  readonly rubyStyle: TextStyle;
}

/** Resolves a run's partial style against the base style. */
export function resolveStyle(
  base: TextStyle,
  patch: Partial<TextStyle> | undefined,
): TextStyle {
  if (!patch) return base;
  return { ...base, ...patch };
}

export class TextRenderer {
  private measureContext: CanvasRenderingContext2D | null = null;

  constructor(private baseStyle: TextStyle = DEFAULT_TEXT_STYLE) {}

  setBaseStyle(style: Partial<TextStyle>): void {
    this.baseStyle = { ...this.baseStyle, ...style };
  }

  get style(): TextStyle {
    return this.baseStyle;
  }

  /** Detached 2D context used purely for `measureText`. */
  private get measurer(): CanvasRenderingContext2D {
    if (this.measureContext) return this.measureContext;
    const canvas =
      typeof OffscreenCanvas === 'function'
        ? new OffscreenCanvas(8, 8)
        : document.createElement('canvas');
    const context = canvas.getContext('2d') as CanvasRenderingContext2D | null;
    if (!context) throw new Error('cannot create a 2D context for text measurement');
    this.measureContext = context;
    return context;
  }

  private fontString(style: TextStyle): string {
    return `${style.fontStyle} ${style.fontWeight} ${style.fontSize}px ${style.fontFamily}`;
  }

  /**
   * Lays out runs into positioned glyphs.
   *
   * Every character is placed individually rather than as whole words, because
   * Japanese and Chinese have no spaces and the reveal effect needs per-glyph
   * positions anyway. Latin words are kept together by only breaking on spaces.
   */
  layout(runs: readonly TextRun[], options: LayoutOptions): TextLayout {
    const measured: MeasuredGlyph[] = [];
    for (const run of runs) {
      const style = resolveStyle(this.baseStyle, run.style);
      const rubyStyle = resolveStyle(style, {
        fontSize: Math.max(8, Math.round(style.fontSize * style.rubySize)),
      });
      const rubyText = run.ruby ?? '';
      let offset = run.offset;
      for (const char of run.text) {
        if (char === '\u0000') continue;
        measured.push({
          char,
          width: this.measureChar(char, style),
          style,
          offset,
          ruby: rubyText || undefined,
          rubyWidth: rubyText
            ? this.measureText(rubyText, rubyStyle)
            : 0,
          rubyStyle,
        });
        offset += 1;
      }
    }

    return this.flow(measured, options);
  }

  private measureChar(char: string, style: TextStyle): number {
    return this.measureText(char, style) + style.letterSpacing;
  }

  measureText(text: string, style: TextStyle): number {
    const context = this.measurer;
    context.font = this.fontString(style);
    return context.measureText(text).width;
  }

  private flow(
    glyphs: readonly MeasuredGlyph[],
    options: LayoutOptions,
  ): TextLayout {
    const kinsoku = options.kinsoku ?? true;
    const maxWidth = Math.max(1, options.maxWidth);
    const placed: PlacedGlyph[] = [];

    let x = 0;
    let line = 0;
    let lineHeight = 0;
    let maxLineWidth = 0;
    let index = 0;

    const styleOf = (candidate: MeasuredGlyph | undefined): TextStyle =>
      candidate?.style ?? this.baseStyle;

    while (index < glyphs.length) {
      const glyph = glyphs[index]!;
      if (glyph.char === '\n') {
        line += 1;
        x = 0;
        index += 1;
        continue;
      }
      if (glyph.char === '\r') {
        index += 1;
        continue;
      }

      const lineHeightFor = Math.max(
        glyph.style.fontSize * glyph.style.lineSpacing,
        glyph.ruby ? glyph.style.fontSize * glyph.style.lineSpacing + glyph.rubyStyle.fontSize : 0,
      );
      lineHeight = Math.max(lineHeight, lineHeightFor);

      const width = glyph.width;
      const overflows = x + width > maxWidth && x > 0;

      if (overflows) {
        // Kinsoku: pull the previous glyph down when the break would strand a
        // character that may not start a line, or start with one that may not
        // end a line.
        let breakAt = index;
        if (kinsoku && index > 0 && NO_LINE_START.has(glyph.char)) {
          const previous = glyphs[index - 1]!;
          if (!NO_LINE_END.has(previous.char) && previous.char !== ' ') {
            breakAt = index - 1;
          }
        }
        if (breakAt !== index) {
          // Move the already-placed glyph to the next line by rewinding.
          const moved = placed.pop();
          if (moved) {
            index -= 1;
            x = 0;
            line += 1;
            maxLineWidth = Math.max(maxLineWidth, moved.x + moved.width);
            continue;
          }
        }
        x = 0;
        line += 1;
        // Skip the space that caused the wrap.
        if (glyph.char === ' ') {
          index += 1;
          continue;
        }
      }

      const ascent = glyph.style.fontSize;
      placed.push({
        char: glyph.char,
        x,
        y: 0,
        width: glyph.char === ' ' ? width : glyph.width - glyph.style.letterSpacing,
        ascent,
        style: glyph.style,
        offset: glyph.offset,
        ...(glyph.ruby ? { ruby: glyph.ruby } : {}),
        ...(glyph.ruby ? { rubyWidth: glyph.rubyWidth } : {}),
        ...(glyph.ruby ? { rubyStyle: glyph.rubyStyle } : {}),
      });

      x += width;
      maxLineWidth = Math.max(maxLineWidth, x);
      index += 1;
    }

    // Second pass: assign baseline Y positions now that the row height is known.
    const lineHeightResolved = lineHeight || this.baseStyle.fontSize * this.baseStyle.lineSpacing;
    let currentLine = 0;
    let cursorX = 0;
    for (const glyph of placed) {
      if (glyph.x < cursorX - 0.001) currentLine += 1;
      cursorX = glyph.x + glyph.width;
    }

    return {
      glyphs: placed,
      lines: Math.max(1, currentLine + 1),
      lineHeight: lineHeightResolved,
      width: maxLineWidth,
      height: Math.max(1, currentLine + 1) * lineHeightResolved,
      totalCharacters: placed.length,
    };
  }

  /**
   * Draws a layout.
   *
   * `revealCount` limits how many characters are drawn, which is how the
   * typewriter effect is implemented: layout is computed once and only the
   * painted prefix changes per frame, so reveal speed never reflows text.
   */
  draw(
    context: CanvasRenderingContext2D,
    layout: TextLayout,
    originX: number,
    originY: number,
    revealCount = Number.POSITIVE_INFINITY,
    opacity = 1,
  ): number {
    const rowHeight = layout.lineHeight;
    let row = 0;
    let lastX = 0;
    let drawn = 0;

    for (const glyph of layout.glyphs) {
      if (glyph.x < lastX - 0.001) row += 1;
      lastX = glyph.x + glyph.width;

      if (drawn >= revealCount) break;
      drawn += 1;

      const baselineY = originY + row * rowHeight + glyph.style.fontSize;
      const x = originX + glyph.x;

      if (glyph.char === ' ') continue;

      context.save();
      context.globalAlpha = opacity;
      context.font = this.fontString(glyph.style);

      if (glyph.ruby && glyph.rubyStyle) {
        const rubyX =
          x + Math.max(0, (glyph.width - (glyph.rubyWidth ?? 0)) / 2);
        const rubyY =
          originY + row * rowHeight + glyph.rubyStyle.fontSize;
        this.paintText(context, glyph.ruby, rubyX, rubyY, glyph.rubyStyle);
      }

      this.paintText(context, glyph.char, x, baselineY, glyph.style);
      context.restore();
    }

    return drawn;
  }

  /** Paints text with outline then fill, the standard VN readability recipe. */
  paintText(
    context: CanvasRenderingContext2D,
    text: string,
    x: number,
    y: number,
    style: TextStyle,
  ): void {
    context.font = this.fontString(style);

    if (style.shadowBlur > 0 || style.shadowOffsetX !== 0 || style.shadowOffsetY !== 0) {
      context.shadowColor = style.shadowColor;
      context.shadowBlur = style.shadowBlur;
      context.shadowOffsetX = style.shadowOffsetX;
      context.shadowOffsetY = style.shadowOffsetY;
    }

    if (style.outlineWidth > 0) {
      context.lineJoin = 'round';
      context.miterLimit = 2;
      context.lineWidth = style.outlineWidth * 2;
      context.strokeStyle = style.outlineColor;
      context.strokeText(text, x, y);
    }

    context.fillStyle = style.color;
    context.fillText(text, x, y);

    context.shadowColor = 'transparent';
    context.shadowBlur = 0;
    context.shadowOffsetX = 0;
    context.shadowOffsetY = 0;
  }

  /** Measures how tall a wrapped paragraph will be, without placing glyphs. */
  measureHeight(runs: readonly TextRun[], maxWidth: number): number {
    return this.layout(runs, { maxWidth }).height;
  }
}

/**
 * Minimal inline markup parser.
 *
 * KAG3 message text carries `[ruby]`, `[color]`, `[size]`, `[b]` and `[i]`
 * style tags inside the message body (distinct from the tag lines that drive
 * the script). This converts the visible portion of a message into runs.
 */
export function parseInlineRuns(
  text: string,
  baseStyle: Partial<TextStyle> = {},
): readonly TextRun[] {
  const runs: TextRun[] = [];
  const stack: { style: Partial<TextStyle>; ruby?: string }[] = [{ style: baseStyle }];
  let buffer = '';
  let offset = 0;
  let cursor = 0;

  const flush = (): void => {
    if (!buffer) return;
    const top = stack[stack.length - 1]!;
    runs.push({
      text: buffer,
      offset,
      style: top.style,
      ...(top.ruby ? { ruby: top.ruby } : {}),
    });
    offset += buffer.length;
    buffer = '';
  };

  const tagPattern = /\[([a-z]+)(?:\s+([^\]]*))?\]/gi;
  let match: RegExpExecArray | null;

  while ((match = tagPattern.exec(text)) !== null) {
    buffer += text.slice(cursor, match.index);
    cursor = match.index + match[0].length;

    const name = match[1]!.toLowerCase();
    const value = (match[2] ?? '').trim();

    switch (name) {
      case 'ruby': {
        flush();
        const top = stack[stack.length - 1]!;
        stack.push({ style: top.style, ruby: value });
        break;
      }
      case '/ruby': {
        flush();
        if (stack.length > 1) stack.pop();
        break;
      }
      case 'color':
      case 'colour': {
        flush();
        const top = stack[stack.length - 1]!;
        stack.push({ style: { ...top.style, color: value }, ...(top.ruby ? { ruby: top.ruby } : {}) });
        break;
      }
      case 'size': {
        flush();
        const top = stack[stack.length - 1]!;
        const size = Number(value);
        stack.push({
          style: {
            ...top.style,
            ...(Number.isFinite(size) && size > 0 ? { fontSize: size } : {}),
          },
          ...(top.ruby ? { ruby: top.ruby } : {}),
        });
        break;
      }
      case 'b': {
        flush();
        const top = stack[stack.length - 1]!;
        stack.push({ style: { ...top.style, fontWeight: '700' }, ...(top.ruby ? { ruby: top.ruby } : {}) });
        break;
      }
      case 'i': {
        flush();
        const top = stack[stack.length - 1]!;
        stack.push({ style: { ...top.style, fontStyle: 'italic' }, ...(top.ruby ? { ruby: top.ruby } : {}) });
        break;
      }
      case '/color':
      case '/colour':
      case '/size':
      case '/b':
      case '/i': {
        flush();
        if (stack.length > 1) stack.pop();
        break;
      }
      case 'br': {
        buffer += '\n';
        break;
      }
      default:
        // Unknown inline tag: keep it visible rather than silently dropping it.
        buffer += match[0];
        break;
    }
  }

  buffer += text.slice(cursor);
  flush();

  if (runs.length === 0) {
    runs.push({ text: '', offset: 0, style: baseStyle });
  }
  return runs;
}
