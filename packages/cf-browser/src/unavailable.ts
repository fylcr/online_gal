import type { Argv } from 'yargs';

/**
 * Stands in for a hand-written command that a generated group mounts (scripts/build.mjs maps
 * every `#commands/*` import here). Those commands need the file system, child processes or
 * workerd, so whatever arguments follow, the answer is that the browser cannot run them.
 */
export function unavailableCommand(name: string, path: string) {
  return {
    command: `${name} [args..]`,
    describe: false as const,
    builder: (yargs: Argv) =>
      yargs.positional('args', { type: 'string', array: true }).strict(false).strictOptions(false),
    handler: () => {
      throw new Error(`cf ${path} needs a local environment and is not available in the browser`);
    },
  };
}
