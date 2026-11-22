/**
 * Browser `#lib/prompt`. The console is always interactive, so the questions upstream asks in a
 * TTY are asked inline; cancelling one ends the command the way Ctrl+C does in the CLI.
 */
import { CliExit } from '#lib/cli-exit.js';

import { io } from '../context.ts';
import { sanitizeTerminalText } from '#lib/ui/sanitize.js';

const cancelled = () => new CliExit(130, { cancelled: true });

export async function confirmDelete(options: { force?: boolean; message?: string } = {}): Promise<boolean> {
  if (options.force === true) return true;
  const question = `${options.message ?? 'This permanently deletes the resource.'} Continue?`;
  const confirmed = await io().confirm(question);
  if (!confirmed) io().write('stderr', 'Aborted.\n');
  return confirmed;
}

export async function confirm(message: string): Promise<boolean> {
  return io().confirm(message);
}

export async function promptForRequiredField(
  flagName: string,
  description: string,
  options: { kind?: 'text' | 'secret'; question?: string; validate?: (value: string) => string | undefined } = {},
): Promise<string> {
  const question = description || options.question || `Enter value for --${flagName}`;
  for (;;) {
    const value = await io().ask(sanitizeTerminalText(question), { secret: options.kind === 'secret' });
    if (value === null) throw cancelled();
    const problem = value === '' ? 'Value cannot be empty' : options.validate?.(value);
    if (!problem) return value;
    io().write('stderr', `${problem}\n`);
  }
}

export async function promptForRequiredEnumField(
  flagName: string,
  description: string,
  choices: readonly string[],
  choiceLabels: Readonly<Record<string, string>> = {},
): Promise<string> {
  const labels = Object.fromEntries(choices.map((choice) => [choice, sanitizeTerminalText(choiceLabels[choice] ?? choice)]));
  const value = await io().ask(sanitizeTerminalText(description || `Choose a value for --${flagName}`), { choices, labels });
  if (value === null) throw cancelled();
  if (!choices.includes(value)) throw new Error(`--${flagName} must be one of: ${choices.join(', ')}`);
  return value;
}
