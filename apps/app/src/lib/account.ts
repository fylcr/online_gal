/**
 * The kernel's account API as the manager sees it (openspec/changes/cf-account-admin,
 * cloudflare-account spec). The kernel keeps the credentials; pages only ever get these views,
 * never a token. The shapes mirror `packages/plugin-system/src/account/cloudflare.ts`; the app talks
 * to the kernel over HTTP and does not import its sources.
 */

export interface CloudflareUser {
  readonly id: string;
  readonly email: string;
}

export interface SiteAdmin {
  readonly accountId: string;
  readonly accountName: string;
  readonly projectName: string;
}

export interface PendingLogin {
  readonly userCode: string;
  readonly verificationUri: string;
  readonly verificationUriComplete?: string;
  readonly expiresAt: number;
  /** Seconds to wait between polls. */
  readonly interval: number;
}

export type CloudflareStatus =
  | { readonly signedIn: false; readonly pending: PendingLogin | null }
  | {
      readonly signedIn: true;
      readonly user: CloudflareUser | null;
      readonly scopes: readonly string[];
      readonly expiresAt: number;
      readonly siteAdmin: SiteAdmin | null;
      readonly siteAdminError?: string;
    };

export type PollResult =
  | { readonly state: 'pending' | 'slow-down'; readonly interval: number }
  | { readonly state: 'expired' | 'denied' | 'none' }
  | { readonly state: 'signed-in'; readonly status: CloudflareStatus };

/** Why the account cannot be used here at all, as opposed to a request that failed. */
export type Unusable =
  /** No kernel controls this page: the dev server, a forced reload, or a browser without Service Workers. */
  | 'unready'
  /** The active manager is not the official one. */
  | 'forbidden'
  /** The browser cannot keep the credentials (no IndexedDB). */
  | 'unavailable';

export class AccountError extends Error {
  constructor(
    readonly code: string,
    message: string,
    readonly unusable?: Unusable,
  ) {
    super(message);
    this.name = 'AccountError';
  }
}

const BASE = '/__kernel/api/account/cloudflare/';

/** Whether the kernel answers this page's requests; without it there is no account API. */
export function kernelControlsPage(): boolean {
  return typeof navigator !== 'undefined' && navigator.serviceWorker?.controller != null;
}

async function call<T>(action: 'status' | 'login' | 'poll' | 'logout' | 'import-entry', data: object = {}): Promise<T> {
  if (!kernelControlsPage()) {
    throw new AccountError('unready', '插件系统未就绪', 'unready');
  }
  const post = action !== 'status';
  let response: Response;
  try {
    response = await fetch(`${BASE}${action}`, {
      method: post ? 'POST' : 'GET',
      cache: 'no-store',
      ...(post ? { headers: { 'content-type': 'application/json' }, body: JSON.stringify(data) } : {}),
    });
  } catch (error) {
    throw new AccountError('fetch-failed', error instanceof Error ? error.message : String(error));
  }
  // Anything but the kernel's JSON means the request reached a server instead of the kernel.
  if (!(response.headers.get('content-type') ?? '').includes('application/json')) {
    throw new AccountError('unready', '插件系统未就绪', 'unready');
  }
  const body = (await response.json()) as Record<string, unknown> & { error?: { code?: string; message?: string } };
  if (!response.ok) {
    const code = body.error?.code ?? `http-${response.status}`;
    const message = body.error?.message ?? `HTTP ${response.status}`;
    const unusable = code === 'account-forbidden' ? 'forbidden' : code === 'account-unavailable' ? 'unavailable' : undefined;
    throw new AccountError(code, message, unusable);
  }
  return body as T;
}

export const account = {
  importEntry: async (segment: string): Promise<void> => {
    if (!/^[A-Za-z0-9_-]{43}$/.test(segment)) throw new AccountError('invalid-entry', 'Cloudflare 管理入口格式无效');
    if ((await call<{ ok?: boolean }>('import-entry', { segment })).ok !== true) {
      throw new AccountError('import-failed', '无法导入 Cloudflare 管理入口');
    }
  },
  status: async (): Promise<CloudflareStatus> => (await call<{ status: CloudflareStatus }>('status')).status,
  login: async (): Promise<PendingLogin> => (await call<{ login: PendingLogin }>('login')).login,
  poll: async (): Promise<PollResult> => (await call<{ poll: PollResult }>('poll')).poll,
  logout: async (): Promise<CloudflareStatus> => (await call<{ status: CloudflareStatus }>('logout')).status,
};
