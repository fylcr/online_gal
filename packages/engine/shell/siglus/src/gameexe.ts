/**
 * `Gameexe.ini` reading.
 *
 * SiglusEngine keeps its configuration in `Gameexe.ini` at the game root: a
 * sectioned INI whose `#SCREEN_SIZE` tells a host what canvas the game expects.
 * AetherKiri reads exactly this value through
 * `siglus_ak_game_screen_size()` and keeps the renderer at the game-native size,
 * upscaling the resulting frame rather than resizing the engine — so parsing it
 * here reproduces that decision on the browser side.
 *
 * Many trial and retail builds ship `Gameexe.dat` instead, which is an
 * encrypted form of the same table. There is no public decoder for it, so the
 * reader reports the limitation rather than returning guesses.
 */

/** One `#KEY = value` entry. */
export interface GameexeEntry {
  readonly section: string;
  readonly key: string;
  readonly value: string;
}

export interface GameexeDocument {
  /** Entries keyed by `SECTION.KEY`, upper-cased. */
  readonly entries: ReadonlyMap<string, string>;
  readonly sections: readonly string[];
  /** First `#TITLE`/`#WINDOW_TITLE` found, if any. */
  readonly title: string | null;
  /** `#SCREEN_SIZE`, parsed. */
  readonly screenSize: { width: number; height: number } | null;
  /** Keys encountered, in file order, for diagnostics. */
  readonly keyCount: number;
}

const SCREEN_SIZE_KEYS = ['SCREEN_SIZE', 'SCREENSIZE'];
const TITLE_KEYS = ['WINDOW_TITLE', 'WINDOWTITLE', 'TITLE'];

/**
 * Parses a `Gameexe.ini` payload.
 *
 * The format is line-oriented: `#SECTION` opens a section, `#KEY = VALUE`
 * assigns within it, and `;` or `//` starts a comment. Values routinely contain
 * commas and quoted strings, so nothing is split beyond the first `=`.
 */
export function parseGameexe(source: string): GameexeDocument {
  const entries = new Map<string, string>();
  const sections: string[] = [];
  let currentSection = '';
  let title: string | null = null;
  let screenSize: { width: number; height: number } | null = null;
  let keyCount = 0;

  const lines = source.replace(/\r\n?/g, '\n').split('\n');
  for (const rawLine of lines) {
    const line = stripComment(rawLine).trim();
    if (!line || !line.startsWith('#')) continue;

    const body = line.slice(1).trim();
    const equals = body.indexOf('=');
    if (equals === -1) {
      // A bare `#NAME` opens a section.
      currentSection = body.toUpperCase();
      if (currentSection && !sections.includes(currentSection)) {
        sections.push(currentSection);
      }
      continue;
    }

    const key = body.slice(0, equals).trim().toUpperCase();
    const value = body.slice(equals + 1).trim();
    if (!key) continue;

    entries.set(`${currentSection}.${key}`, value);
    keyCount += 1;

    if (!title && TITLE_KEYS.includes(key)) {
      title = unquote(value);
    }
    if (!screenSize && SCREEN_SIZE_KEYS.includes(key)) {
      screenSize = parseSize(value);
    }
  }

  return { entries, sections, title, screenSize, keyCount };
}

/** Removes a trailing `;` or `//` comment that is not inside quotes. */
function stripComment(line: string): string {
  let quote: string | null = null;
  for (let index = 0; index < line.length; index += 1) {
    const char = line[index]!;
    if (quote) {
      if (char === quote) quote = null;
      continue;
    }
    if (char === '"') {
      quote = char;
      continue;
    }
    if (char === ';') return line.slice(0, index);
    if (char === '/' && line[index + 1] === '/') return line.slice(0, index);
  }
  return line;
}

function unquote(value: string): string {
  const trimmed = value.trim();
  if (
    trimmed.length >= 2 &&
    ((trimmed.startsWith('"') && trimmed.endsWith('"')) ||
      (trimmed.startsWith("'") && trimmed.endsWith("'")))
  ) {
    return trimmed.slice(1, -1);
  }
  return trimmed;
}

/** Parses the several spellings Siglus uses for a screen size. */
export function parseSize(value: string): { width: number; height: number } | null {
  const unquoted = unquote(value);
  const match = /(\d+)\s*[,xX*\s]\s*(\d+)/.exec(unquoted);
  if (!match) return null;
  const width = Number(match[1]);
  const height = Number(match[2]);
  if (!Number.isFinite(width) || !Number.isFinite(height)) return null;
  if (width < 160 || height < 120 || width > 8192 || height > 8192) return null;
  return { width, height };
}

/**
 * Looks a key up under any section.
 *
 * `Gameexe.ini` sections are not consistent between builds, so a lookup that
 * demanded the right section would miss real values.
 */
export function lookupAny(
  document: GameexeDocument,
  keys: readonly string[],
): string | undefined {
  for (const key of keys) {
    const upper = key.toUpperCase();
    for (const [compound, value] of document.entries) {
      if (compound.endsWith(`.${upper}`)) return value;
    }
  }
  return undefined;
}

/** True when the payload looks like the encrypted `Gameexe.dat` table. */
export function looksEncrypted(source: Uint8Array): boolean {
  // The encrypted form is not text: a valid plain INI is overwhelmingly ASCII
  // with `#` markers, so a high proportion of control/high bytes means the
  // reader is looking at the packed variant.
  if (source.byteLength === 0) return true;
  const sample = source.subarray(0, Math.min(512, source.byteLength));
  let binary = 0;
  for (const byte of sample) {
    if (byte === 0) return true;
    if (byte < 9 || (byte > 13 && byte < 32)) binary += 1;
  }
  return binary / sample.length > 0.1;
}
