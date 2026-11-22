/**
 * yargs for the browser. yargs' own `browser.mjs` loads cliui and yargs-parser from unpkg at run
 * time; this builds the same factory from the copies installed with yargs, so the bundle has no
 * network dependency (scripts/build.mjs maps `yargs-internal/*` to yargs' `build/lib`).
 */
// @ts-expect-error untyped internal entry
import { YargsFactory } from 'yargs-internal/yargs-factory.js';
// @ts-expect-error untyped internal entry
import { getProcessArgvBin } from 'yargs-internal/utils/process-argv.js';
// @ts-expect-error untyped internal entry
import { YError } from 'yargs-internal/yerror.js';
// @ts-expect-error resolved from yargs' own dependencies by scripts/build.mjs
import cliui from 'cliui';
// @ts-expect-error untyped browser entry
import Parser from 'yargs-parser/browser';
import type { Argv } from 'yargs';

/** `%s` substitution that keeps falsy values such as 0 (yargs' browser shim drops them). */
function format(template: string, ...args: unknown[]): string {
  let index = 0;
  return template.replace(/%[sd]/g, () => (index < args.length ? String(args[index++]) : ''));
}

const unsupported = (what: string) => () => {
  throw new YError(`${what} is not supported in the browser`);
};

let columns = 100;

const platform = {
  assert: { notStrictEqual: () => {}, strictEqual: () => {} },
  cliui,
  findUp: () => undefined,
  getEnv: () => undefined,
  inspect: (...values: unknown[]) => console.log(...values),
  getCallerFile: unsupported('loading a directory of commands'),
  getProcessArgvBin,
  mainFilename: 'cf',
  Parser,
  path: {
    basename: (value: string) => value,
    dirname: (value: string) => value,
    extname: (value: string) => value,
    relative: (value: string) => value,
    join: (value: string) => value,
  },
  process: {
    argv: () => [],
    cwd: () => '',
    emitWarning: () => {},
    execPath: () => '',
    exit: () => {},
    nextTick: (callback: () => void) => queueMicrotask(callback),
    get stdColumns() {
      return columns;
    },
  },
  readFileSync: () => '',
  readdirSync: () => [],
  require: unsupported('require'),
  requireDirectory: unsupported('loading a directory of commands'),
  stringWidth: (value: string) => [...value].length,
  y18n: {
    __: (template = '', ...args: unknown[]) => format(template, ...args),
    __n: (singular: string, plural: string, count: number, ...args: unknown[]) =>
      format(count === 1 ? singular : plural, ...args),
    getLocale: () => 'en_US',
    setLocale: () => {},
    updateLocale: () => {},
  },
};

const factory = YargsFactory(platform) as (args?: readonly string[]) => Argv;

export function createYargs(args: readonly string[], width: number): Argv {
  columns = width;
  return factory([...args]);
}
