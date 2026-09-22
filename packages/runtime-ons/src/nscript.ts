/**
 * Parser for the NScripter / ONScripter command language.
 *
 * The grammar is line oriented and deceptively irregular:
 *  - `*label` defines a jump target;
 *  - `command arg, arg, "quoted", effect` runs a command;
 *  - a bare line is displayed text;
 *  - `;` starts a comment, and `*define` … `game` wraps definition blocks;
 *  - `\` at end of line continues onto the next.
 *
 * Arguments are comma separated, but effect names, unquoted filenames and
 * numeric arguments all appear in the same position, so each argument is
 * captured as raw text plus a derived value.
 */

export interface NsArgument {
  /** Raw text with surrounding quotes removed and whitespace trimmed. */
  readonly text: string;
  /** Numeric interpretation, or null when the argument is not numeric. */
  readonly number: number | null;
  /** True when the source argument was quoted. */
  readonly quoted: boolean;
}

export type NsStatement =
  | { readonly kind: 'label'; readonly name: string; readonly line: number }
  | { readonly kind: 'command'; readonly name: string; readonly args: readonly NsArgument[]; readonly raw: string; readonly line: number }
  | { readonly kind: 'text'; readonly text: string; readonly line: number }
  | { readonly kind: 'comment'; readonly line: number }
  | { readonly kind: 'blank'; readonly line: number };

export interface ParsedNscript {
  readonly statements: readonly NsStatement[];
  /** Label name (without `*`) → statement index. */
  readonly labels: ReadonlyMap<string, number>;
  /** True when the script declared `*define`. */
  readonly hasDefineBlock: boolean;
  readonly diagnostics: readonly string[];
  readonly source: string;
}

function makeArgument(raw: string): NsArgument {
  let text = raw.trim();
  let quoted = false;
  if (
    text.length >= 2 &&
    ((text.startsWith('"') && text.endsWith('"')) ||
      (text.startsWith("'") && text.endsWith("'")))
  ) {
    text = text.slice(1, -1);
    quoted = true;
  }
  const number = /^-?\d+$/.test(text) ? Number.parseInt(text, 10) : null;
  return { text, number, quoted };
}

/** Splits an argument list on commas that are not inside quotes. */
export function splitNsArguments(source: string): readonly NsArgument[] {
  const parts: string[] = [];
  let buffer = '';
  let quote: string | null = null;

  for (let index = 0; index < source.length; index += 1) {
    const char = source[index]!;
    if (quote) {
      if (char === quote) quote = null;
      buffer += char;
      continue;
    }
    if (char === '"' || char === "'") {
      quote = char;
      buffer += char;
      continue;
    }
    if (char === ',') {
      parts.push(buffer);
      buffer = '';
      continue;
    }
    buffer += char;
  }
  if (buffer.length > 0) parts.push(buffer);
  return parts.map(makeArgument);
}

/**
 * A line is a command when its first token matches this shape: a lowercase
 * identifier, optionally with a leading `_`, followed by whitespace or nothing.
 */
const COMMAND_PATTERN = /^([a-z_][a-z0-9_]*)\s*(.*)$/i;

/** Commands that take no arguments and appear bare on their own line. */
const BARE_COMMANDS = new Set([
  'click',
  'clr',
  'clrall',
  'er',
  'indent',
  'line',
  'page',
  'resettimer',
  'return',
  'texec',
  'textoff',
  'texton',
  'wait',
  'waittimer',
  'end',
  'quit',
  'reset',
  'skip',
  'nbskip',
]);

/**
 * Commands the runtime implements.
 *
 * ONScripter tells a command line from narrative text by looking the first token
 * up in its command table, and so must we: a bare sentence like
 * `First message line.` parses structurally exactly like a command with one
 * argument, and guessing from punctuation or casing gets real scripts wrong.
 * Keeping the table here means the parser and the interpreter cannot drift.
 */
export const ONS_COMMANDS: ReadonlySet<string> = new Set([
  // control flow
  'goto', 'gosub', 'call', 'return', 'jumpb', 'jumpf', 'end', 'quit',
  'resettimer', 'wait', 'waittimer', 'click', 'skip', 'nbskip', 'delay',
  // conditionals and arithmetic
  'if', 'notif', 'mov', 'movl', 'add', 'addl', 'sub', 'subl', 'mul', 'div',
  'mod', 'inc', 'dec', 'numalias', 'stralias', 'itoa', 'itoa2', 'itoa3',
  'atoi', 'getparam', 'getreg', 'defsub', 'getspmode', 'isskip', 'intlimit',
  // message flow
  'clr', 'clrall', 'er', 'page', 'br', 'indent', 'line', 'texton', 'textoff',
  'ruby', 'flushout', 'monocro', 'cursorset', 'getcursorpos', 'automode',
  'lookback', 'lookbackflush',
  // graphics
  'bg', 'ld', 'ldl', 'csp', 'csp2', 'vsp', 'vsp2', 'cl', 'clc', 'cll',
  'effect', 'effectblank', 'window', 'windoweffect', 'bar', 'barclear',
  'prnum', 'prnumclear', 'quake', 'quakex', 'quakey', 'blt', 'cell', 'humanz',
  'trans', 'bgm', 'caption', 'defaultfont', 'font', 'deffont', 'texec',
  // audio
  'bgmfade', 'play', 'playonce', 'wave', 'mp3', 'mp3loop', 'mp3save',
  'bgmstop', 'stop', 'wavestop', 'mp3vol', 'bgmvol', 'sevol', 'midi',
  // animation, movies, engine integration
  'anime', 'movie', 'systemcall', 'getini', 'saveini', 'savename',
  'fileexist', 'getfileinfo', 'savescreenshot', 'savescreenshot2',
  'cdfadeout',
  // saves
  'save', 'load', 'saveon', 'saveoff', 'savedir',
  // selection
  'select', 'selnum',
  // definition block markers
  'define', 'game', 'definebg', 'definereset',
]);

export interface ParseNscriptOptions {
  /**
   * Command table used to tell commands from narrative text. Defaults to
   * `ONS_COMMANDS`; pass an empty set to fall back to the structural heuristic
   * (first token lowercase, followed by arguments) which is only suitable for
   * scripts known to contain no unquoted prose.
   */
  readonly commands?: ReadonlySet<string>;
}

export function parseNscript(
  source: string,
  options: ParseNscriptOptions = {},
): ParsedNscript {
  const commands = options.commands ?? ONS_COMMANDS;
  const statements: NsStatement[] = [];
  const labels = new Map<string, number>();
  const diagnostics: string[] = [];
  let hasDefineBlock = false;

  const normalised = source.replace(/\r\n?/g, '\n');
  const lines = normalised.split('\n');

  let pending = '';
  let pendingLine = 1;

  const commit = (line: string, lineNumber: number): void => {
    const statement = parseLine(line, lineNumber, commands);
    if (statement.kind === 'label') {
      if (labels.has(statement.name)) {
        diagnostics.push(`line ${lineNumber}: duplicate label "*${statement.name}"`);
      }
      labels.set(statement.name, statements.length);
    }
    if (
      statement.kind === 'command' &&
      ['define', 'game', 'definebg'].includes(statement.name)
    ) {
      hasDefineBlock = true;
    }
    statements.push(statement);
  };

  for (let index = 0; index < lines.length; index += 1) {
    const line = lines[index]!;
    if (pending) {
      pending += line;
      if (pending.endsWith('\\')) {
        pending = pending.slice(0, -1);
        continue;
      }
      commit(pending, pendingLine);
      pending = '';
      continue;
    }
    if (line.endsWith('\\')) {
      pending = line.slice(0, -1);
      pendingLine = index + 1;
      continue;
    }
    commit(line, index + 1);
  }
  if (pending) commit(pending, pendingLine);

  return { statements, labels, hasDefineBlock, diagnostics, source };
}

function parseLine(
  rawLine: string,
  lineNumber: number,
  commands: ReadonlySet<string>,
): NsStatement {
  const trimmed = rawLine.trim();

  if (!trimmed) return { kind: 'blank', line: lineNumber };
  if (trimmed.startsWith(';') || trimmed.startsWith('//')) {
    return { kind: 'comment', line: lineNumber };
  }
  if (trimmed.startsWith('*')) {
    // `*label` may be followed by a comment.
    const name = trimmed.slice(1).split(/[\s;]/)[0] ?? '';
    return { kind: 'label', name, line: lineNumber };
  }
  // `:` introduces an in-line comment in some scripts.
  if (trimmed.startsWith(':')) return { kind: 'comment', line: lineNumber };

  const match = COMMAND_PATTERN.exec(trimmed);
  if (match) {
    const name = match[1]!.toLowerCase();
    const rest = match[2]!.trim();

    // The first token must name a real command. Without this test a line of
    // prose whose first word is a bare identifier — `First message line.` —
    // would be misread as a command with one argument, which is exactly what
    // ONScripter's own table lookup prevents.
    if (commands.has(name) || BARE_COMMANDS.has(name)) {
      if (BARE_COMMANDS.has(name) && rest === '') {
        return { kind: 'command', name, args: [], raw: trimmed, line: lineNumber };
      }
      // A trailing `;`-comment is stripped before splitting arguments.
      const commentIndex = findUnquotedSemicolon(rest);
      const argumentSource = commentIndex === -1 ? rest : rest.slice(0, commentIndex);
      return {
        kind: 'command',
        name,
        args: splitNsArguments(argumentSource),
        raw: trimmed,
        line: lineNumber,
      };
    }
  }

  return { kind: 'text', text: rawLine.replace(/\s+$/, ''), line: lineNumber };
}

function findUnquotedSemicolon(source: string): number {
  let quote: string | null = null;
  let depth = 0;
  for (let index = 0; index < source.length; index += 1) {
    const char = source[index]!;
    if (quote) {
      if (char === quote) quote = null;
      continue;
    }
    if (char === '"' || char === "'") {
      quote = char;
      continue;
    }
    if (char === '(') depth += 1;
    if (char === ')') depth = Math.max(0, depth - 1);
    if (char === ';' && depth === 0) return index;
  }
  return -1;
}

/**
 * Expands the `\\` escapes ONScripter uses inside displayed text.
 *
 * Notably `\\n` is a hard line break, `\\\\` is a literal backslash, and a
 * trailing `@` suppresses the click-to-advance wait.
 */
export function expandNsText(text: string): { text: string; noWait: boolean } {
  let noWait = false;
  let result = '';
  for (let index = 0; index < text.length; index += 1) {
    const char = text[index]!;
    if (char !== '\\') {
      result += char;
      continue;
    }
    const next = text[index + 1];
    index += 1;
    switch (next) {
      case 'n':
        result += '\n';
        break;
      case '\\':
        result += '\\';
        break;
      case '@':
        noWait = true;
        break;
      case 't':
        result += '\t';
        break;
      default:
        // Unknown escapes are preserved so that literal backslashes survive.
        result += next === undefined ? '\\' : next;
        break;
    }
  }
  if (result.endsWith('@')) {
    noWait = true;
    result = result.slice(0, -1);
  }
  return { text: result, noWait };
}

/** Parses the `effect` argument of a display command. */
export interface NsEffect {
  readonly name: string;
  readonly durationMs: number;
}

const EFFECT_DURATIONS: Readonly<Record<string, number>> = {
  instant: 0,
  fade: 400,
  crossfade: 400,
  fadein: 400,
  fadeout: 400,
  dissolve: 600,
  dissolve2: 600,
  univ: 500,
  univ2: 500,
  univ3: 500,
  univ4: 500,
  universal: 500,
  slide: 400,
  slidein: 400,
  slideout: 400,
  wipe: 500,
  shaker: 400,
  quake: 400,
  blink: 400,
  tr: 300,
};

export function parseEffect(argument: NsArgument | undefined): NsEffect {
  if (!argument) return { name: 'instant', durationMs: 0 };
  const name = argument.text.toLowerCase();
  // `effect,80,2` — the second field overrides the duration in 20 ms steps.
  const duration = EFFECT_DURATIONS[name] ?? (name === '' ? 0 : 350);
  return { name, durationMs: duration };
}

/** `nsa`/`sar` archive detection helpers live in the VFS package. */
export const ONS_MARKERS = {
  scripts: ['nscript.dat', 'nscript.___', 'nscr_sec.dat', 'onscript.nt2', 'onscript.nt3', '00.txt'],
  archives: ['.nsa', '.ns2', '.sar'],
} as const;
