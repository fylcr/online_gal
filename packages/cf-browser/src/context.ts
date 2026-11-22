/**
 * The command that is running and the page's console it writes to. Commands run one at a time,
 * so the `#lib` shims read the console from here instead of from `process`.
 */
import type { CfIo } from './public.ts';

interface Run {
  io: CfIo;
  quiet: boolean;
}

let current: Run | null = null;

export function run(): Run {
  if (!current) throw new Error('cf: no command is running');
  return current;
}

export function io(): CfIo {
  return run().io;
}

export function env(name: string): string | undefined {
  const value = current?.io.env?.[name];
  return value === undefined || value === '' ? undefined : value;
}

/**
 * cf reads `--quiet` from the raw argument list rather than from yargs (upstream `lib/args.ts`),
 * so output that happens before parsing finishes still honours it.
 */
export function quietFlag(argv: readonly string[]): boolean {
  let quiet = false;
  for (let index = 0; index < argv.length; index++) {
    const token = argv[index]!;
    if (token === '--') break;
    if (token === '--no-quiet') quiet = false;
    else if (token === '--quiet' || token === '-q') {
      const next = argv[index + 1];
      if (next === 'false' || next === 'true') {
        quiet = next === 'true';
        index++;
      } else quiet = true;
    } else if (/^(--quiet|-q)=(true|false)$/.test(token)) quiet = token.endsWith('=true');
  }
  return quiet;
}

export async function withRun<T>(io: CfIo, argv: readonly string[], task: () => Promise<T>): Promise<T> {
  if (current) throw new Error('cf: another command is still running');
  current = { io, quiet: quietFlag(argv) };
  try {
    return await task();
  } finally {
    current = null;
  }
}

/** `fetch` bound to the running command's abort signal. */
export function runFetch(): typeof fetch {
  const signal = io().signal;
  return (input, init) => {
    const signals = [signal, init?.signal].filter((value): value is AbortSignal => value instanceof AbortSignal);
    return fetch(input, { ...init, signal: signals.length > 1 ? AbortSignal.any(signals) : signals[0] });
  };
}

/** Absolute URL of the kernel's Cloudflare API relay (the API `Client` joins paths with `URL`). */
export function apiBaseUrl(): string {
  return new URL('/__cf/api', globalThis.location.origin).href;
}
