/**
 * The browser cf: upstream's generated commands under a yargs root set up like upstream's
 * `buildCli`, minus everything that needs a local environment (cf-browser spec).
 */
import type { Argv } from 'yargs';

import { ApiError } from '#api';
import { generatedCommands } from '#generated';
import { CliExit } from '#lib/cli-exit.js';
import { BUILTIN_OPTIONS, GLOBAL_OPTIONS } from '#lib/global-options.js';

import { authCommand } from './commands/auth.ts';
import { withRun } from './context.ts';
import type { CfIo } from './public.ts';
import { createYargs } from './yargs.ts';

export type * from './public.ts';

declare const __CF_VERSION__: string;
declare global {
  // Read by upstream schema-flags as `process.stdout.columns` (scripts/build.mjs defines it so).
  var __CF_COLUMNS__: number | undefined;
}

/** Global options that only make sense next to a local project or credentials store. */
const LOCAL_ONLY = ['local', 'persist-to', 'profile', 'mode', 'temporary-account', 'event-code'] as const;

class UsageError extends Error {
  constructor(message: string, readonly helpCommand: string) {
    super(message);
  }
}

function globalOptions() {
  const options: Record<string, object> = {};
  for (const [name, option] of Object.entries(GLOBAL_OPTIONS)) {
    options[name] = (LOCAL_ONLY as readonly string[]).includes(name) ? { ...option, hidden: true } : option;
  }
  return options;
}

function buildCli(args: readonly string[], width: number): Argv {
  const cli = createYargs(args, width)
    .scriptName('cf')
    .parserConfiguration({ 'sort-commands': true })
    .wrap(width)
    .usage('$0 <command> [options]')
    .usage('\nThe Cloudflare CLI, in the browser - manage this site and its Cloudflare account')
    .help('help', BUILTIN_OPTIONS.help.description)
    .alias(BUILTIN_OPTIONS.help.alias, 'help')
    .options(globalOptions())
    .group([...Object.keys(GLOBAL_OPTIONS), ...Object.keys(BUILTIN_OPTIONS)], 'Global flags')
    .command(authCommand);
  for (const generated of generatedCommands) {
    cli.command(generated.hideCommand ? { ...generated.command, describe: false } : generated.command);
  }
  return cli
    .middleware((argv) => {
      for (const name of LOCAL_ONLY) {
        const value = argv[name];
        if (value !== undefined && value !== false) throw new Error(`--${name} is not available in the browser.`);
      }
    })
    .demandCommand(1, 'You need to specify a command')
    .strictCommands()
    .strictOptions()
    .recommendCommands()
    .version('version', BUILTIN_OPTIONS.version.description, __CF_VERSION__)
    .alias(BUILTIN_OPTIONS.version.alias, 'version')
    .epilogue(
      'Commands that need a local environment (dev, build, deploy, local D1, tunnels) are not available here.\n' +
        'For more information, visit https://developers.cloudflare.com/',
    )
    .exitProcess(false)
    .fail((message, error, instance) => {
      if (error) throw error;
      if (message === 'Please specify a subcommand') {
        instance.showHelp('log');
        return;
      }
      const command = args.filter((arg) => !arg.startsWith('-')).slice(0, 2).join(' ');
      const text = message?.startsWith('Did you mean') ? `Unknown command: ${command}. ${message}` : message;
      throw new UsageError(text ?? 'Invalid command', `cf ${command ? `${command} ` : ''}--help`);
    });
}

function report(io: CfIo, error: unknown): number {
  if (error instanceof CliExit) {
    if (error.cancelled) io.write('stderr', 'Cancelled.\n');
    return error.code;
  }
  if (io.signal.aborted) {
    io.write('stderr', 'Cancelled.\n');
    return 130;
  }
  if (error instanceof UsageError) {
    io.write('stderr', `${error.message}\n\nFor more information, run ${error.helpCommand}\n`);
    return 2;
  }
  if (error instanceof ApiError) {
    io.write('stderr', `${error.message}\n`);
    return 1;
  }
  io.write('stderr', `${error instanceof Error ? error.message : String(error)}\n`);
  return 1;
}

export async function runCf(argv: readonly string[], io: CfIo): Promise<number> {
  return withRun(io, argv, async () => {
    const width = io.columns ?? 100;
    globalThis.__CF_COLUMNS__ = width;
    let failure: unknown;
    let output = '';
    try {
      await buildCli(argv, width).parseAsync([...argv], {}, (error: unknown, _argv: unknown, text: string) => {
        failure ??= error ?? undefined;
        output = text;
      });
    } catch (error) {
      failure = error;
    }
    if (output) io.write('stdout', output.endsWith('\n') ? output : `${output}\n`);
    return failure === undefined || failure === null ? 0 : report(io, failure);
  });
}
