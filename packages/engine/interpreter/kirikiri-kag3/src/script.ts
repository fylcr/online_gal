/**
 * KAG3 script parsing.
 *
 * A KAG3 `.ks` file is a flat sequence of lines, each of which is either:
 *  - a label              `*label|display name`
 *  - a tag line           `[command attr=value attr="quoted value"]`
 *  - a message line       `[cm]Text shown to the player[r]`
 *  - a comment            `;` or `//` or `#`
 *
 * Tags are separated by the first whitespace, and attribute values may be
 * quoted with `"` or `'`, which may themselves contain `]`. That last detail is
 * why this is a hand-written scanner instead of a regular expression.
 */

export interface KagTag {
  readonly name: string;
  readonly attrs: ReadonlyMap<string, string>;
  /** Newline-separated segments inside the tag body, for `[if]`/`[else]`. */
  readonly raw: string;
  readonly line: number;
}

export type KagStatement =
  | { readonly kind: 'label'; readonly name: string; readonly display: string; readonly line: number }
  | { readonly kind: 'tag'; readonly tag: KagTag; readonly line: number }
  | { readonly kind: 'text'; readonly text: string; readonly line: number }
  | { readonly kind: 'comment'; readonly line: number };

export interface ParsedScript {
  readonly statements: readonly KagStatement[];
  /** Label name → statement index, for `[jump]`/`[call]` resolution. */
  readonly labels: ReadonlyMap<string, number>;
  readonly source: string;
  readonly diagnostics: readonly string[];
}

const TAG_NAME_PATTERN = /^[A-Za-z_][A-Za-z0-9_]*$/;

/** Parses the inner body of a tag (without the surrounding brackets). */
export function parseTagBody(body: string, line: number): KagTag | null {
  let cursor = 0;
  while (cursor < body.length && /\s/.test(body[cursor]!)) cursor += 1;
  const nameStart = cursor;
  while (cursor < body.length && !/\s/.test(body[cursor]!)) cursor += 1;
  const name = body.slice(nameStart, cursor);
  if (!name || !TAG_NAME_PATTERN.test(name)) return null;

  const attrs = new Map<string, string>();
  while (cursor < body.length) {
    while (cursor < body.length && /\s/.test(body[cursor]!)) cursor += 1;
    if (cursor >= body.length) break;

    const keyStart = cursor;
    while (cursor < body.length && body[cursor] !== '=' && !/\s/.test(body[cursor]!)) {
      cursor += 1;
    }
    const key = body.slice(keyStart, cursor);
    if (!key) {
      cursor += 1;
      continue;
    }

    // A bare attribute (no `=`) is a boolean flag, e.g. `[wait]`.
    while (cursor < body.length && /\s/.test(body[cursor]!)) cursor += 1;
    if (cursor >= body.length || body[cursor] !== '=') {
      attrs.set(key, 'true');
      continue;
    }
    cursor += 1;
    while (cursor < body.length && /\s/.test(body[cursor]!)) cursor += 1;

    let value: string;
    const quote = body[cursor];
    if (quote === '"' || quote === "'") {
      cursor += 1;
      const valueStart = cursor;
      while (cursor < body.length && body[cursor] !== quote) cursor += 1;
      value = body.slice(valueStart, cursor);
      if (cursor < body.length) cursor += 1;
    } else {
      const valueStart = cursor;
      while (cursor < body.length && !/\s/.test(body[cursor]!)) cursor += 1;
      value = body.slice(valueStart, cursor);
    }
    attrs.set(key, value);
  }

  return { name, attrs, raw: body, line };
}

/**
 * Splits one physical line into statements.
 *
 * A line may hold text before a tag (`Hello[wait time=500]`), several tags, or
 * nothing but text. `[` only opens a tag when the body parses as one and the
 * matching `]` exists, so literal brackets in prose survive intact.
 */
function parseLine(line: string, lineNumber: number): readonly KagStatement[] {
  const trimmed = line.trim();
  if (!trimmed) return [];
  if (trimmed.startsWith(';') || trimmed.startsWith('//') || trimmed.startsWith('#')) {
    return [{ kind: 'comment', line: lineNumber }];
  }
  if (trimmed.startsWith('*')) {
    const body = trimmed.slice(1);
    const separator = body.indexOf('|');
    const name = (separator === -1 ? body : body.slice(0, separator)).trim();
    const display = separator === -1 ? name : body.slice(separator + 1).trim();
    return [{ kind: 'label', name, display, line: lineNumber }];
  }

  // KAG3 spells the same tag two ways. The bracket form `[tag attr=value]` is
  // the one usually seen in tutorials, but the at-mark form
  //
  //     @call storage="snow.ks"
  //
  // is at least as common in real titles, because it lets a tag own a whole
  // line without the reader having to match brackets. A line whose first
  // character is `@` is therefore *entirely* a tag.
  //
  // Skipping this form is a silent failure of the worst kind: the line is not
  // recognised as a tag, so it falls through to the text path and the game
  // displays its own script source as if it were dialogue. That is exactly what
  // happened against a real shipping scenario, and it is why this branch exists.
  if (trimmed.startsWith('@')) {
    const tag = parseTagBody(trimmed.slice(1), lineNumber);
    return tag
      ? [{ kind: 'tag', tag, line: lineNumber }]
      : [{ kind: 'text', text: trimmed, line: lineNumber }];
  }

  const statements: KagStatement[] = [];
  let buffer = '';
  let cursor = 0;

  const flushText = (): void => {
    if (!buffer) return;
    statements.push({ kind: 'text', text: buffer, line: lineNumber });
    buffer = '';
  };

  while (cursor < line.length) {
    const char = line[cursor]!;
    if (char !== '[') {
      buffer += char;
      cursor += 1;
      continue;
    }

    // Find the matching `]`, respecting quoted attribute values.
    let scan = cursor + 1;
    let quote: string | null = null;
    let closing = -1;
    while (scan < line.length) {
      const current = line[scan]!;
      if (quote) {
        if (current === quote) quote = null;
      } else if (current === '"' || current === "'") {
        quote = current;
      } else if (current === ']') {
        closing = scan;
        break;
      }
      scan += 1;
    }

    if (closing === -1) {
      // Unbalanced bracket: treat it as literal text.
      buffer += char;
      cursor += 1;
      continue;
    }

    const body = line.slice(cursor + 1, closing);
    const tag = parseTagBody(body, lineNumber);
    if (!tag) {
      buffer += line.slice(cursor, closing + 1);
      cursor = closing + 1;
      continue;
    }

    flushText();
    statements.push({ kind: 'tag', tag, line: lineNumber });
    cursor = closing + 1;
  }

  flushText();
  return statements;
}

export interface ParseOptions {
  /** Tags whose `[` does not open a tag (rarely needed). */
  readonly unknownTagIsText?: boolean;
}

/**
 * Parses a whole script. Tags spanning several physical lines are joined first,
 * because KAG allows a line to end in `\` to continue a tag.
 */
export function parseKagScript(source: string, options: ParseOptions = {}): ParsedScript {
  const diagnostics: string[] = [];
  const statements: KagStatement[] = [];
  const labels = new Map<string, number>();

  const normalised = source.replace(/\r\n?/g, '\n');
  const physicalLines = normalised.split('\n');

  let pending = '';
  let pendingLine = 1;

  const commit = (text: string, lineNumber: number): void => {
    let parsed: readonly KagStatement[];
    try {
      parsed = parseLine(text, lineNumber);
    } catch (error) {
      diagnostics.push(`line ${lineNumber}: ${String(error)}`);
      return;
    }
    for (const statement of parsed) {
      if (statement.kind === 'label') {
        if (labels.has(statement.name)) {
          diagnostics.push(
            `line ${lineNumber}: duplicate label "${statement.name}"`,
          );
        }
        labels.set(statement.name, statements.length);
      }
      statements.push(statement);
    }
  };

  for (let i = 0; i < physicalLines.length; i += 1) {
    const line = physicalLines[i]!;
    if (pending) {
      pending += `\n${line}`;
      if (line.trimEnd().endsWith('\\')) {
        pending = pending.slice(0, -1);
        continue;
      }
      commit(pending, pendingLine);
      pending = '';
      continue;
    }
    if (line.trimEnd().endsWith('\\')) {
      pending = line.trimEnd().slice(0, -1);
      pendingLine = i + 1;
      continue;
    }
    commit(line, i + 1);
  }

  if (pending) commit(pending, pendingLine);

  if (options.unknownTagIsText === false) {
    // Caller asked for strict behaviour; nothing extra to do, kept for clarity.
  }

  return { statements, labels, source, diagnostics };
}

/** Converts KAG's true/false spellings. */
export function parseKagBoolean(value: string | undefined, fallback = false): boolean {
  if (value === undefined) return fallback;
  const normalised = value.trim().toLowerCase();
  if (['true', '1', 'yes', 'on'].includes(normalised)) return true;
  if (['false', '0', 'no', 'off'].includes(normalised)) return false;
  return fallback;
}

export function parseKagNumber(value: string | undefined, fallback = 0): number {
  if (value === undefined || value === '') return fallback;
  const parsed = Number(value);
  return Number.isFinite(parsed) ? parsed : fallback;
}

/**
 * Expands KAG storage paths.
 *
 * KAG writes `file="bg/a.png"` relative to the game root, and `%`-prefixed
 * forms address storage roots. Only the game root is meaningful in a browser,
 * so prefixes are stripped and separators normalised.
 */
export function resolveStoragePath(value: string): string {
  let path = value.trim().replace(/\\/g, '/');
  if (path.startsWith('file://./')) path = path.slice('file://./'.length);
  else if (path.startsWith('file://')) path = path.slice('file://'.length);
  path = path.replace(/^\.\//, '').replace(/^\/+/, '');
  return path;
}
