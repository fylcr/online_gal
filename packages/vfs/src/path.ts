/**
 * Path helpers shared by every source and archive reader.
 *
 * All VFS paths are POSIX-style, relative to the game root, with no leading
 * slash and no `.` / `..` segments.
 */

/** Normalises a path to VFS form. An empty string is the game root. */
export function normalisePath(path: string): string {
  const replaced = path.replace(/\\/g, '/').replace(/^\/+/, '');
  const out: string[] = [];
  for (const segment of replaced.split('/')) {
    if (segment === '' || segment === '.') continue;
    if (segment === '..') {
      out.pop();
      continue;
    }
    out.push(segment);
  }
  return out.join('/');
}

/** Normalises a directory path, guaranteeing no trailing slash. */
export function normaliseDir(dir: string | undefined): string {
  return normalisePath(dir ?? '');
}

export function dirname(path: string): string {
  const key = normalisePath(path);
  const index = key.lastIndexOf('/');
  return index === -1 ? '' : key.slice(0, index);
}

export function basename(path: string): string {
  const key = normalisePath(path);
  const index = key.lastIndexOf('/');
  return index === -1 ? key : key.slice(index + 1);
}

export function extname(path: string): string {
  const base = basename(path);
  const index = base.lastIndexOf('.');
  return index <= 0 ? '' : base.slice(index).toLowerCase();
}

export function joinPath(dir: string | undefined, ...parts: string[]): string {
  return normalisePath([dir ?? '', ...parts].filter(Boolean).join('/'));
}

/** Returns every ancestor directory of `path`, root first. */
export function ancestorsOf(path: string): readonly string[] {
  const key = normalisePath(path);
  const parts = key.split('/').filter(Boolean);
  const out: string[] = [];
  for (let i = 1; i < parts.length; i += 1) {
    out.push(parts.slice(0, i).join('/'));
  }
  return out;
}

/** Case-insensitive exact match used by the ASCII-only archive formats. */
export function equalsIgnoreCase(a: string, b: string): boolean {
  return a.length === b.length && a.toLowerCase() === b.toLowerCase();
}
