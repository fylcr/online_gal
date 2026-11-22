export interface SetupItem {
  name: string;
  label: string;
  help: string;
  helpUrl?: string;
  source: 'account' | 'input';
  verify?: 'cloudflare' | 'github';
  owner: string;
}

/** Required Pages secrets, in the order shown on the setup form. */
export const SETUP_ITEMS: readonly SetupItem[] = [
  {
    name: 'CLOUDFLARE_ACCOUNT_ID', label: 'Cloudflare 账号 ID', source: 'account', owner: 'site-setup',
    help: '取自已确认的 Pages 项目所有者。',
  },
  {
    name: 'CLOUDFLARE_API_TOKEN', label: 'Cloudflare API Token', source: 'input', verify: 'cloudflare', owner: 'site-setup',
    help: '按后续将使用的 Cloudflare 功能授予权限。',
    helpUrl: 'https://dash.cloudflare.com/profile/api-tokens',
  },
  {
    name: 'GITHUB_TOKEN', label: 'GitHub Token', source: 'input', verify: 'github', owner: 'site-setup',
    help: '按后续将使用的 GitHub 功能授予权限。',
    helpUrl: 'https://github.com/settings/personal-access-tokens',
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
