export interface SetupItem {
  name: string;
  label: string;
  help: string;
  helpUrl?: string;
  source: 'account' | 'input';
  verify?: 'cloudflare' | 'github';
  owner: string;
}

/**
 * Required Pages secrets, in the order shown on the setup page.
 *
 * Decision G (`site-setup`): the owner signs in with Cloudflare OAuth, so the first batch is the
 * account id alone, taken from the confirmed Pages project owner. `CLOUDFLARE_API_TOKEN` and
 * `GITHUB_TOKEN` were removed — an OAuth access token expires within the hour and a refresh token
 * is rotated on every renewal, so neither can live on as a Pages secret.
 *
 * `source: 'input'` and `verify` stay in the type for a later change that registers an item and
 * decides for itself where the long-lived value comes from. No current item uses them; the page
 * refuses to write while one does.
 */
export const SETUP_ITEMS: readonly SetupItem[] = [
  {
    name: 'CLOUDFLARE_ACCOUNT_ID', label: 'Cloudflare 账号 ID', source: 'account', owner: 'site-setup',
    help: '取自已确认的 Pages 项目所有者。',
  },
];

export function validateSetupItems(items: readonly SetupItem[]): void {
  const names = new Set(['ONLINE_GAL_SITE']);
  for (const { name } of items) {
    if (!/^[A-Z][A-Z0-9_]*$/.test(name) || names.has(name)) throw new Error(`Invalid or duplicate setup item: ${name}`);
    names.add(name);
  }
}

validateSetupItems(SETUP_ITEMS);
