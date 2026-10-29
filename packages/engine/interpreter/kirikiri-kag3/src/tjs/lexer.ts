/**
 * Lexer for TJS2, the scripting language embedded in KiriKiri.
 *
 * TJS2 is C-like with `#` pragmas, `//` and `/* *​/` comments, single-quoted
 * and double-quoted strings, and two dialects of array/object literal syntax
 * (JavaScript-style `[]`/`{}` and TJS-style `%[]`). All of that is handled
 * here so the parser only sees a flat token stream.
 */

export type TokenType =
  | 'number'
  | 'string'
  | 'identifier'
  | 'keyword'
  | 'punct'
  | 'eof';

export interface Token {
  readonly type: TokenType;
  readonly value: string;
  /** For string tokens: the decoded value, without quotes. */
  readonly literal?: string;
  readonly line: number;
  readonly column: number;
  /** True when no newline separated this token from the previous one. */
  readonly noNewlineBefore: boolean;
}

export const TJS_KEYWORDS = new Set([
  'var',
  'function',
  'return',
  'if',
  'else',
  'while',
  'for',
  'do',
  'break',
  'continue',
  'true',
  'false',
  'null',
  'void',
  'new',
  'delete',
  'typeof',
  'instanceof',
  'in',
  'this',
  'class',
  'extends',
  'property',
  'try',
  'catch',
  'finally',
  'throw',
  'switch',
  'case',
  'default',
  'const',
  'int',
  'real',
  'string',
  'octet',
  'global',
  'super',
  'with',
  'incontextof',
  'debugger',
]);

/** Multi-character operators, longest first so greedy matching works. */
const PUNCTUATORS = [
  '>>>=',
  '===',
  '!==',
  '>>>',
  '<<=',
  '>>=',
  '&&=',
  '||=',
  '**=',
  '==',
  '!=',
  '<=',
  '>=',
  '&&',
  '||',
  '++',
  '--',
  '+=',
  '-=',
  '*=',
  '/=',
  '%=',
  '&=',
  '|=',
  '^=',
  '<<',
  '>>',
  '=>',
  '**',
  '?.',
  '...',
  '{',
  '}',
  '(',
  ')',
  '[',
  ']',
  ';',
  ',',
  '.',
  ':',
  '?',
  '+',
  '-',
  '*',
  '/',
  '%',
  '=',
  '<',
  '>',
  '!',
  '~',
  '&',
  '|',
  '^',
  '@',
  '#',
  '$',
] as const;

export class TjsSyntaxError extends Error {
  constructor(
    message: string,
    readonly line: number,
    readonly column: number,
  ) {
    super(`TJS2 syntax error at ${line}:${column}: ${message}`);
    this.name = 'TjsSyntaxError';
  }
}

function isIdentifierStart(char: string): boolean {
  return /[A-Za-z_$\u0080-\uffff]/.test(char);
}

function isIdentifierPart(char: string): boolean {
  return /[A-Za-z0-9_$\u0080-\uffff]/.test(char);
}

function isDigit(char: string): boolean {
  return char >= '0' && char <= '9';
}

export interface TokenizeOptions {
  /**
   * Keep `#` lines as tokens instead of skipping them. KAG scripts use `#` for
   * preprocessor hints the runtime wants to see.
   */
  readonly keepPragmas?: boolean;
}

export function tokenize(source: string, options: TokenizeOptions = {}): readonly Token[] {
  const tokens: Token[] = [];
  let cursor = 0;
  let line = 1;
  let lineStart = 0;
  let sawNewline = true;

  const push = (
    type: TokenType,
    value: string,
    startLine: number,
    startColumn: number,
    literal?: string,
  ): void => {
    tokens.push({
      type,
      value,
      line: startLine,
      column: startColumn,
      noNewlineBefore: !sawNewline,
      ...(literal !== undefined ? { literal } : {}),
    });
    sawNewline = false;
  };

  while (cursor < source.length) {
    const char = source[cursor]!;

    if (char === '\n') {
      cursor += 1;
      line += 1;
      lineStart = cursor;
      sawNewline = true;
      continue;
    }
    if (char === ' ' || char === '\t' || char === '\r' || char === '\f' || char === '\v') {
      cursor += 1;
      continue;
    }

    const startLine = line;
    const startColumn = cursor - lineStart + 1;

    // Comments.
    if (char === '/' && source[cursor + 1] === '/') {
      while (cursor < source.length && source[cursor] !== '\n') cursor += 1;
      continue;
    }
    if (char === '/' && source[cursor + 1] === '*') {
      cursor += 2;
      while (cursor < source.length && !(source[cursor] === '*' && source[cursor + 1] === '/')) {
        if (source[cursor] === '\n') {
          line += 1;
          lineStart = cursor + 1;
        }
        cursor += 1;
      }
      cursor += 2;
      continue;
    }

    // `#` pragma or directive: runs to end of line.
    if (char === '#') {
      const start = cursor;
      while (cursor < source.length && source[cursor] !== '\n') cursor += 1;
      if (options.keepPragmas) {
        push('punct', source.slice(start, cursor), startLine, startColumn);
      }
      continue;
    }

    // Strings.
    if (char === '"' || char === "'") {
      const quote = char;
      cursor += 1;
      let value = '';
      while (cursor < source.length && source[cursor] !== quote) {
        const current = source[cursor]!;
        if (current === '\\') {
          const next = source[cursor + 1];
          cursor += 2;
          switch (next) {
            case 'n':
              value += '\n';
              break;
            case 't':
              value += '\t';
              break;
            case 'r':
              value += '\r';
              break;
            case '0':
              value += '\0';
              break;
            case '\\':
              value += '\\';
              break;
            case '"':
              value += '"';
              break;
            case "'":
              value += "'";
              break;
            case '\n':
              line += 1;
              lineStart = cursor;
              break;
            default:
              value += next ?? '';
              break;
          }
          continue;
        }
        if (current === '\n') {
          // TJS2 strings may span lines.
          line += 1;
          lineStart = cursor + 1;
        }
        value += current;
        cursor += 1;
      }
      cursor += 1;
      push('string', source.slice(cursor - value.length - 2, cursor), startLine, startColumn, value);
      continue;
    }

    // Numbers: decimal, hex, and the `1.0e3` form.
    if (isDigit(char) || (char === '.' && isDigit(source[cursor + 1] ?? ''))) {
      const start = cursor;
      if (char === '0' && (source[cursor + 1] === 'x' || source[cursor + 1] === 'X')) {
        cursor += 2;
        while (cursor < source.length && /[0-9a-fA-F]/.test(source[cursor]!)) cursor += 1;
      } else {
        while (cursor < source.length && isDigit(source[cursor]!)) cursor += 1;
        if (source[cursor] === '.') {
          cursor += 1;
          while (cursor < source.length && isDigit(source[cursor]!)) cursor += 1;
        }
        if (source[cursor] === 'e' || source[cursor] === 'E') {
          const save = cursor;
          cursor += 1;
          if (source[cursor] === '+' || source[cursor] === '-') cursor += 1;
          if (isDigit(source[cursor] ?? '')) {
            while (cursor < source.length && isDigit(source[cursor]!)) cursor += 1;
          } else {
            cursor = save;
          }
        }
      }
      push('number', source.slice(start, cursor), startLine, startColumn);
      continue;
    }

    // Identifiers and keywords.
    if (isIdentifierStart(char)) {
      const start = cursor;
      while (cursor < source.length && isIdentifierPart(source[cursor]!)) cursor += 1;
      const value = source.slice(start, cursor);
      push(TJS_KEYWORDS.has(value) ? 'keyword' : 'identifier', value, startLine, startColumn);
      continue;
    }

    // Punctuation, greediest match first.
    let matched = false;
    for (const punctuator of PUNCTUATORS) {
      if (source.startsWith(punctuator, cursor)) {
        cursor += punctuator.length;
        push('punct', punctuator, startLine, startColumn);
        matched = true;
        break;
      }
    }
    if (matched) continue;

    throw new TjsSyntaxError(`unexpected character ${JSON.stringify(char)}`, startLine, startColumn);
  }

  tokens.push({
    type: 'eof',
    value: '',
    line,
    column: cursor - lineStart + 1,
    noNewlineBefore: !sawNewline,
  });
  return tokens;
}
