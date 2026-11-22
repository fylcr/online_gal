/**
 * Browser stand-ins for the three Node functions that upstream `body-parser`, `input-validation`
 * and `schema-flags` use. scripts/build.mjs only lets those modules reach these; any other
 * `node:*` import still fails the build.
 */

/** `@file` arguments need a local file system; the caller wraps this message in its own error. */
export function readFileSync(path: string): never {
  throw new Error(`the browser cannot read local files (${path}); pass the value directly`);
}

/** Paths only reach `readFileSync`, which refuses them, so resolving is a no-op. */
export function resolve(...segments: string[]): string {
  return segments.at(-1) ?? '';
}

/** Deep equality for the parsed JSON values cf compares (body fields against positionals). */
export function isDeepStrictEqual(a: unknown, b: unknown): boolean {
  if (Object.is(a, b)) return true;
  if (typeof a !== 'object' || typeof b !== 'object' || a === null || b === null) return false;
  if (Object.getPrototypeOf(a) !== Object.getPrototypeOf(b)) return false;
  if (Array.isArray(a)) {
    const other = b as unknown[];
    return a.length === other.length && a.every((value, index) => isDeepStrictEqual(value, other[index]));
  }
  const left = Object.keys(a);
  const right = Object.keys(b);
  return (
    left.length === right.length &&
    left.every(
      (key) =>
        Object.prototype.hasOwnProperty.call(b, key) &&
        isDeepStrictEqual((a as Record<string, unknown>)[key], (b as Record<string, unknown>)[key]),
    )
  );
}
