/** Browser `#lib/local`: `--local` simulations need Miniflare, so the browser never runs them. */
export const LOCAL_ACCOUNT_ID = 'local';

export const NO_LOCAL_EQUIVALENT = 'This command has no local equivalent.';

export async function disposeLocalRuntime(): Promise<void> {}

export function createLocalFetch(): typeof fetch {
  throw new Error('--local is not available in the browser.');
}
