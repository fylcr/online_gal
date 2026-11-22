/**
 * `cf auth` in the browser. Signing in, out and the current identity belong to the kernel's
 * account API (cloudflare-account spec); these commands only drive it from the console.
 */
import type { CommandModule } from 'yargs';

import { io } from '../context.ts';
import { formatOutput, printSuccess } from '../lib/output.ts';

async function whoami(): Promise<void> {
  const status = await io().account.status();
  if (!status.signedIn) {
    io().write('stderr', "Not signed in. Run 'cf auth login'.\n");
    return;
  }
  formatOutput({
    email: status.user?.email,
    id: status.user?.id,
    scopes: status.scopes?.length,
    expiresAt: status.expiresAt ? new Date(status.expiresAt).toISOString() : undefined,
    siteAdmin: status.siteAdmin ?? null,
  });
}

const login: CommandModule = {
  command: 'login',
  describe: 'Sign in to Cloudflare with a device code',
  handler: async () => {
    await io().account.login((text) => io().write('stderr', text), io().signal);
    const status = await io().account.status();
    printSuccess(`Signed in as ${status.user?.email ?? 'unknown user'}`);
  },
};

const logout: CommandModule = {
  command: 'logout',
  describe: 'Sign out and revoke the refresh token',
  handler: async () => {
    await io().account.logout();
    printSuccess('Signed out');
  },
};

const whoamiCommand: CommandModule = {
  command: 'whoami',
  describe: 'Show the signed-in user and the account that owns this site',
  handler: whoami,
};

export const authCommand: CommandModule = {
  command: 'auth',
  describe: 'Sign in to Cloudflare and inspect the current identity',
  builder: (yargs) =>
    yargs.command(login).command(logout).command(whoamiCommand).demandCommand(1, 'Please specify a subcommand'),
  handler: () => {},
};
