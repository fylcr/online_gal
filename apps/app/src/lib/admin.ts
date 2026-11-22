/**
 * Shared pieces of the /admin pages (openspec/changes/cf-account-admin, design.md A6 and A9):
 * the gate, the Cloudflare API helper and the Pages types the panels read.
 *
 * The gate only decides what the page shows. Cloudflare checks every request against the
 * signed-in account's own permissions, and the page holds no token: requests go to the kernel's
 * relay at `/__cf/api`, which adds it.
 */
import { account, AccountError, type CloudflareUser, type SiteAdmin, type Unusable } from './account';

export type AdminGate =
  | { readonly kind: 'admin'; readonly admin: SiteAdmin; readonly user: CloudflareUser | null }
  | { readonly kind: 'denied'; readonly title: string; readonly detail: string; readonly retry: boolean };

const UNUSABLE: Record<Unusable, { title: string; detail: string }> = {
  unready: {
    title: '插件系统未就绪',
    detail: '管理页需要插件系统的内核。请从已部署的站点打开本页；强制刷新后需要再普通刷新一次。本地开发服务器上没有内核。',
  },
  forbidden: { title: '当前管理器不能使用账号', detail: '只有官方管理器可以使用 Cloudflare 账号。' },
  unavailable: { title: '此浏览器不能保管凭据', detail: '凭据加密后存放在 IndexedDB 中，当前浏览器环境不可用。' },
};

/** Whether the signed-in account owns this site's Pages project, and why not when it does not. */
export async function checkAdmin(): Promise<AdminGate> {
  try {
    const status = await account.status();
    if (!status.signedIn) {
      return {
        kind: 'denied',
        title: '未登录 Cloudflare',
        detail: '在账号设置中登录拥有本站 Pages 项目的 Cloudflare 账号后才能管理站点。',
        retry: false,
      };
    }
    if (status.siteAdmin) return { kind: 'admin', admin: status.siteAdmin, user: status.user };
    if (status.siteAdminError) {
      return { kind: 'denied', title: '无法确认是否为本站管理员', detail: status.siteAdminError, retry: true };
    }
    const who = status.user?.email || status.user?.id || '当前账号';
    return {
      kind: 'denied',
      title: '不是本站管理员',
      detail: `${who} 下没有提供本站（${location.hostname}）的 Pages 项目。`,
      retry: false,
    };
  } catch (error) {
    if (error instanceof AccountError && error.unusable !== undefined) {
      return { kind: 'denied', ...UNUSABLE[error.unusable], retry: true };
    }
    return {
      kind: 'denied',
      title: '读取账号状态失败',
      detail: error instanceof Error ? error.message : String(error),
      retry: true,
    };
  }
}

export interface ResultInfo {
  readonly page?: number;
  readonly per_page?: number;
  readonly count?: number;
  readonly total_count?: number;
  readonly total_pages?: number;
}

interface Envelope<T> {
  readonly success?: boolean;
  readonly result?: T;
  readonly result_info?: ResultInfo;
  readonly errors?: ReadonlyArray<{ readonly code?: unknown; readonly message?: string }>;
}

/** A failed Cloudflare API call; `message` is ready to show. */
export class CfApiError extends Error {
  constructor(
    readonly status: number,
    readonly code: unknown,
    message: string,
  ) {
    super(message);
    this.name = 'CfApiError';
  }
}

/** One Cloudflare API call through the kernel's relay. `path` starts after `/client/v4`. */
export async function cfApi<T>(
  path: string,
  init: { method?: string; body?: unknown; signal?: AbortSignal } = {},
): Promise<{ result: T; info: ResultInfo | null }> {
  const response = await fetch(`/__cf/api${path}`, {
    method: init.method ?? 'GET',
    headers: init.body === undefined ? { accept: 'application/json' } : { accept: 'application/json', 'content-type': 'application/json' },
    body: init.body === undefined ? undefined : JSON.stringify(init.body),
    cache: 'no-store',
    signal: init.signal,
  });
  let body: Envelope<T> | null = null;
  try {
    body = (await response.json()) as Envelope<T>;
  } catch {
    body = null;
  }
  if (body === null || !response.ok || body.success !== true) {
    const first = body?.errors?.[0];
    const code = first?.code;
    const message =
      code === 'account-signed-out'
        ? '登录已失效，请到账号设置重新登录。'
        : code === 'account-forbidden'
          ? '只有官方管理器可以使用 Cloudflare 账号。'
          : (first?.message ?? `HTTP ${response.status}`);
    throw new CfApiError(response.status, code, message);
  }
  return { result: body.result as T, info: body.result_info ?? null };
}

export const errorText = (error: unknown): string => (error instanceof Error ? error.message : String(error));

/** `/accounts/<id>/pages/projects/<name>`, the prefix of every project call. */
export const projectPath = (admin: SiteAdmin): string =>
  `/accounts/${encodeURIComponent(admin.accountId)}/pages/projects/${encodeURIComponent(admin.projectName)}`;

export interface PagesDeployment {
  readonly id: string;
  readonly short_id?: string;
  readonly url?: string;
  readonly environment?: 'production' | 'preview' | string;
  readonly created_on?: string;
  readonly aliases?: readonly string[] | null;
  readonly is_skipped?: boolean;
  readonly latest_stage?: { readonly name?: string; readonly status?: string; readonly ended_on?: string | null };
  readonly deployment_trigger?: {
    readonly type?: string;
    readonly metadata?: { readonly branch?: string; readonly commit_hash?: string; readonly commit_message?: string };
  };
}

export interface PagesProject {
  readonly name: string;
  readonly subdomain?: string;
  readonly domains?: readonly string[];
  readonly production_branch?: string;
  readonly created_on?: string;
  readonly source?: {
    readonly type?: string;
    readonly config?: {
      readonly owner?: string;
      readonly repo_name?: string;
      readonly production_deployments_enabled?: boolean;
      readonly preview_deployment_setting?: string;
    };
  };
  readonly build_config?: {
    readonly build_command?: string | null;
    readonly destination_dir?: string | null;
    readonly root_dir?: string | null;
  };
  readonly latest_deployment?: PagesDeployment | null;
  readonly canonical_deployment?: PagesDeployment | null;
}

export interface PagesDomain {
  readonly id?: string;
  readonly name: string;
  readonly status?: string;
  readonly created_on?: string;
  readonly verification_data?: { readonly status?: string; readonly error_message?: string };
  readonly validation_data?: { readonly status?: string; readonly method?: string; readonly error_message?: string };
}

/** Colour of the status dot: done, failed, in progress, or neutral. */
export type Tone = 'success' | 'error' | 'working' | 'muted';

export const TONE_CLASS: Record<Tone, string> = {
  success: 'bg-success',
  error: 'bg-error',
  working: 'bg-warning',
  muted: 'bg-base-content/30',
};

const STAGE_NAME: Record<string, string> = {
  queued: '排队',
  initialize: '初始化',
  clone_repo: '拉取仓库',
  build: '构建',
  deploy: '发布',
};

/** A deployment's state as one label and tone, e.g. "构建失败". */
export function deploymentState(deployment: PagesDeployment): { label: string; tone: Tone } {
  if (deployment.is_skipped) return { label: '已跳过', tone: 'muted' };
  const stage = deployment.latest_stage;
  const name = STAGE_NAME[stage?.name ?? ''] ?? stage?.name ?? '';
  switch (stage?.status) {
    case 'success':
      return stage.name === 'deploy' ? { label: '已发布', tone: 'success' } : { label: `${name}完成`, tone: 'working' };
    case 'failure':
      return { label: `${name}失败`, tone: 'error' };
    case 'active':
      return { label: `${name}中`, tone: 'working' };
    case 'idle':
      return { label: `等待${name}`, tone: 'working' };
    case 'canceled':
      return { label: '已取消', tone: 'muted' };
    case 'skipped':
      return { label: '已跳过', tone: 'muted' };
    default:
      return { label: stage?.status ?? '未知', tone: 'muted' };
  }
}

const DOMAIN_STATUS: Record<string, { label: string; tone: Tone }> = {
  active: { label: '已生效', tone: 'success' },
  pending: { label: '等待验证', tone: 'working' },
  initializing: { label: '初始化中', tone: 'working' },
  deactivated: { label: '已停用', tone: 'muted' },
  blocked: { label: '已阻止', tone: 'error' },
  error: { label: '出错', tone: 'error' },
};

export const domainState = (domain: PagesDomain): { label: string; tone: Tone } =>
  DOMAIN_STATUS[domain.status ?? ''] ?? { label: domain.status ?? '未知', tone: 'muted' };

export const environmentLabel = (environment: string | undefined): string =>
  environment === 'production' ? '生产' : environment === 'preview' ? '预览' : (environment ?? '');

export function when(iso: string | null | undefined): string {
  return iso ? new Date(iso).toLocaleString('zh-CN', { hour12: false }) : '—';
}

/** First line of a commit message, or how the deployment was made when there is none. */
export function deploymentTitle(deployment: PagesDeployment): string {
  const message = deployment.deployment_trigger?.metadata?.commit_message?.split('\n')[0]?.trim();
  if (message) return message;
  return deployment.deployment_trigger?.type === 'ad_hoc' ? '直接上传' : '部署';
}
