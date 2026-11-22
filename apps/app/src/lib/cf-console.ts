/**
 * Command-line handling for the admin console (openspec/changes/cf-account-admin, cf-browser
 * spec "The admin console is a Svelte component without a terminal library"): shell-like word
 * splitting, the session history and the session variables passed to cf as its environment.
 * Both live in sessionStorage only and end with the tab.
 */

const HISTORY_KEY = 'online-gal.cf.history';
const ENV_KEY = 'online-gal.cf.env';
const HISTORY_LIMIT = 200;
const NAME = /^[A-Za-z_][A-Za-z0-9_]*$/;

/**
 * Splits a command line the way a POSIX shell splits words, without expansions: whitespace
 * separates words, `'…'` is literal, `"…"` keeps `\"`, `\\`, `\$` and `` \` `` escapes, and a
 * backslash outside quotes escapes the next character. A leading `cf` is dropped.
 */
export function splitCommandLine(line: string): string[] {
  const words: string[] = [];
  let word = '';
  let started = false;
  let quote: "'" | '"' | null = null;
  for (let i = 0; i < line.length; i++) {
    const char = line[i]!;
    if (quote === "'") {
      if (char === "'") quote = null;
      else word += char;
      continue;
    }
    if (quote === '"') {
      if (char === '"') quote = null;
      else if (char === '\\' && i + 1 < line.length && '"\\$`'.includes(line[i + 1]!)) word += line[++i];
      else word += char;
      continue;
    }
    if (char === '\\') {
      if (i + 1 < line.length) word += line[++i];
      started = true;
      continue;
    }
    if (char === "'" || char === '"') {
      quote = char;
      started = true;
      continue;
    }
    if (/\s/.test(char)) {
      if (started) words.push(word);
      word = '';
      started = false;
      continue;
    }
    word += char;
    started = true;
  }
  if (quote !== null) throw new Error(`引号 ${quote} 没有闭合`);
  if (started) words.push(word);
  if (words[0] === 'cf') words.shift();
  return words;
}

function read<T>(key: string, fallback: T): T {
  try {
    const raw = sessionStorage.getItem(key);
    return raw === null ? fallback : (JSON.parse(raw) as T);
  } catch {
    return fallback;
  }
}

function write(key: string, value: unknown): void {
  try {
    sessionStorage.setItem(key, JSON.stringify(value));
  } catch {
    // Storage full or unavailable: the session simply forgets.
  }
}

export function loadHistory(): string[] {
  const history = read<unknown>(HISTORY_KEY, []);
  return Array.isArray(history) ? history.filter((item): item is string => typeof item === 'string') : [];
}

/** Appends a line unless it repeats the last one; keeps the newest 200. */
export function remember(history: readonly string[], line: string): string[] {
  const next = history.at(-1) === line ? [...history] : [...history, line];
  const kept = next.slice(-HISTORY_LIMIT);
  write(HISTORY_KEY, kept);
  return kept;
}

export function loadEnv(): Record<string, string> {
  const env = read<unknown>(ENV_KEY, {});
  if (typeof env !== 'object' || env === null) return {};
  return Object.fromEntries(Object.entries(env).filter(([key, value]) => NAME.test(key) && typeof value === 'string'));
}

/**
 * `export` with no arguments lists the variables; `export NAME=value …` sets them, and an empty
 * value removes one. Returns the new variables and the text to show.
 */
export function exportVariables(env: Readonly<Record<string, string>>, args: readonly string[]): { env: Record<string, string>; output: string; error?: string } {
  const next = { ...env };
  if (args.length === 0) {
    const names = Object.keys(next).sort();
    return {
      env: next,
      output: names.length === 0 ? '（没有会话变量）\n' : names.map((name) => `export ${name}=${quote(next[name]!)}\n`).join(''),
    };
  }
  for (const arg of args) {
    const eq = arg.indexOf('=');
    const name = eq < 0 ? arg : arg.slice(0, eq);
    if (eq < 0 || !NAME.test(name)) {
      return { env: { ...env }, output: '', error: `export: 用法 export NAME=value（${arg} 不符合）\n` };
    }
    const value = arg.slice(eq + 1);
    if (value === '') delete next[name];
    else next[name] = value;
  }
  write(ENV_KEY, next);
  return { env: next, output: '' };
}

function quote(value: string): string {
  return /^[A-Za-z0-9_./:@-]+$/.test(value) ? value : `'${value.replaceAll("'", `'\\''`)}'`;
}
