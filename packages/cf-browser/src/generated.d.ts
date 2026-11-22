/**
 * Type of upstream `src/commands/_generated/index.ts` for the type check only (tsconfig.json
 * maps `#generated` here); the build bundles the real module.
 */
import type { CommandModule } from 'yargs';

export interface GeneratedCommand {
  command: CommandModule;
  hideCommand?: boolean;
}

export declare const generatedCommands: GeneratedCommand[];
