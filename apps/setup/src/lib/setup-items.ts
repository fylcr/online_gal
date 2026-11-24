export interface SetupItem {
  name: string;
  label: string;
  help: string;
  helpUrl?: string;
  source: 'account' | 'input' | 'generated';
  verify?: 'cloudflare' | 'github';
  owner: string;
}

/**
 * Required Pages secrets, in the order shown on the setup page.
 *
 * Decision G (`site-setup`) removed the manual token inputs. Decision H adds two generated
 * credentials; the OAuth session itself never becomes a Pages secret.
 * A future `input` item still blocks initialization until its value source is supported.
 */
export const SETUP_ITEMS: readonly SetupItem[] = [
  {
    name: 'CLOUDFLARE_ACCOUNT_ID', label: 'Cloudflare 账号 ID', source: 'account', owner: 'site-setup',
    help: '取自已确认的 Pages 项目所有者。',
  },
  {
    name: 'CLOUDFLARE_ACCOUNT_TOKEN', label: 'Cloudflare 账户令牌', source: 'generated', owner: 'site-setup',
    help: '用 Cloudflare OAuth 申请，只能管理本站所属账户的 API 令牌。',
  },
  {
    name: 'ONLINE_GAL_CA_PRIVATE_JWK', label: '微型 CA 私钥', source: 'generated', owner: 'plugin-system',
    help: '由浏览器生成，用于签发官方插件的来源证明。',
  },
  {
    name: 'ONLINE_GAL_CA_BUILD_TOKEN', label: '微型 CA 构建令牌', source: 'generated', owner: 'plugin-system',
    help: '由浏览器生成，供 Pages 构建请求签发接口；只写入生产 secret。',
  },
  {
    name: 'ONLINE_GAL_CF_ROUTE_KEY', label: 'Cloudflare 私有入口密钥', source: 'generated', owner: 'site-setup',
    help: '由浏览器生成，只写入生产 secret。',
  },
  {
    name: 'ONLINE_GAL_CF_OWNER_ID', label: 'Cloudflare 所有者用户 ID', source: 'account', owner: 'site-setup',
    help: '通过当前 Cloudflare 登录会话核对，只写入生产 secret。',
  },
];

const releaseOnly = new Set(['ONLINE_GAL_CA_BUILD_TOKEN', 'ONLINE_GAL_CF_ROUTE_KEY', 'ONLINE_GAL_CF_OWNER_ID']);
export const setupItems = (releaseFlow: boolean): readonly SetupItem[] => releaseFlow
  ? SETUP_ITEMS : SETUP_ITEMS.filter(({ name }) => !releaseOnly.has(name));

export function validateSetupItems(items: readonly SetupItem[]): void {
  const names = new Set(['ONLINE_GAL_SITE']);
  for (const { name } of items) {
    if (!/^[A-Z][A-Z0-9_]*$/.test(name) || names.has(name)) throw new Error(`Invalid or duplicate setup item: ${name}`);
    names.add(name);
  }
}

validateSetupItems(SETUP_ITEMS);
