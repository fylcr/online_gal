import type { CloudflareAccount, CloudflareUser, SiteAdmin } from '@online-gal/plugin-system/account';
import { SETUP_ITEMS, type SetupItem } from './setup-items.ts';

export type Rebuild = { kind: 'pages-git' | 'manual' } | { kind: 'github-rerun'; repo: string; runId: string };
export interface SetupConfig { siteHost: string; rebuild: Rebuild; builtAt: string }
export type SetupStatus = { phase: 'setup'; initialized: boolean; v?: 1; warning?: string };

interface Deployment { id: string; created_on?: string; latest_stage?: { name?: string; status?: string } }
interface Project {
  production_branch?: string;
  deployment_configs?: { production?: { env_vars?: Record<string, { value?: string } | null> | null } };
}
interface PollOptions { intervalMs?: number; timeoutMs?: number }
type BuildResult = 'success' | 'failure' | 'canceled';

/** The Pages permissions the write and the rebuild need (decision G, design.md S5 step 2). */
export const PAGES_SCOPES = ['pages:read', 'pages:write'] as const;

/**
 * Scopes missing from the token response. An empty list means the response carried no `scope` at
 * all, which is not a refusal: let the Pages requests decide instead of blocking a valid grant.
 */
export function missingScopes(scopes: readonly string[]): string[] {
  if (scopes.length === 0) return [];
  return PAGES_SCOPES.filter((scope) => !scopes.includes(scope));
}
function buildResult(deployment: Deployment): BuildResult | undefined {
  const stage = deployment.latest_stage;
  if (stage?.status === 'failure' || stage?.status === 'canceled') return stage.status;
  if (stage?.name === 'deploy' && stage.status === 'success') return 'success';
}

function reportBuildFailure(result: BuildResult): void {
  if (result === 'failure') throw new Error('部署构建失败。');
  if (result === 'canceled') throw new Error('部署已取消。');
}

const projectPath = (owner: SiteAdmin) => `/__cf/api/accounts/${encodeURIComponent(owner.accountId)}/pages/projects/${encodeURIComponent(owner.projectName)}`;
const requestSignal = (signal?: AbortSignal, ms = 15_000) => signal
  ? AbortSignal.any([signal, AbortSignal.timeout(ms)])
  : AbortSignal.timeout(ms);

export function safeNext(value: string): string {
  return value.startsWith('/') && !value.startsWith('//') && !/[\\\u0000-\u001f\u007f]/.test(value) ? value : '/';
}

export function isLocalHost(host: string): boolean {
  return ['localhost', '127.0.0.1', '[::1]', '::1'].includes(host);
}

export function buildSiteRecord(owner: SiteAdmin, user: CloudflareUser | null): string {
  return JSON.stringify({ v: 1, account: owner.accountId, project: owner.projectName, by: user?.id ?? '', at: new Date().toISOString() });
}

/**
 * Items the setup page cannot write: their value would have to be typed in, and decision G removed
 * the form. A change that registers one must decide how its value is obtained before the site is
 * initialized (design.md S7).
 */
export function unsupportedItems(items: readonly SetupItem[] = SETUP_ITEMS): SetupItem[] {
  return items.filter(({ source }) => source !== 'account');
}

/** Values for the `account` items: they all come from the confirmed Pages project owner. */
export function accountValues(accountId: string, items: readonly SetupItem[] = SETUP_ITEMS): Record<string, string> {
  return Object.fromEntries(items.filter(({ source }) => source === 'account').map(({ name }) => [name, accountId]));
}

/**
 * The single PATCH body. Secret keys follow SETUP_ITEMS, so only the items plus the site record can
 * appear: never an OAuth access or refresh token, never a GitHub token.
 */
export function buildEnvPatch(record: string, values: Readonly<Record<string, string>>, items: readonly SetupItem[] = SETUP_ITEMS) {
  const unsupported = unsupportedItems(items);
  if (unsupported.length > 0) {
    throw new Error(`该项需手动填写，初始化页暂不支持：${unsupported.map(({ name }) => name).join(', ')}`);
  }
  const marker = { type: 'plain_text', value: record };
  const secrets = Object.fromEntries(items.map(({ name }) => {
    const value = values[name];
    if (!value?.trim()) throw new Error(`缺少初始化项 ${name}。`);
    return [name, { type: 'secret_text', value }];
  }));
  return { deployment_configs: {
    production: { env_vars: { ...secrets, ONLINE_GAL_SITE: marker } },
    preview: { env_vars: { ONLINE_GAL_SITE: marker } },
  } };
}

function projectState(project: Project) {
  const env = project.deployment_configs?.production?.env_vars ?? {};
  const marker = env.ONLINE_GAL_SITE;
  const record = marker?.value;
  let writtenAt: string | undefined;
  if (record) {
    try {
      const value = JSON.parse(record)?.at;
      if (typeof value === 'string' && Number.isFinite(Date.parse(value))) writtenAt = value;
    } catch { /* Keep malformed nonempty records initialized. */ }
  }
  return {
    written: marker != null && record !== '',
    record,
    writtenAt,
    missing: SETUP_ITEMS.filter(({ name }) => !Object.hasOwn(env, name)).map(({ name }) => name),
    branch: project.production_branch,
  };
}

function delay(ms: number, signal?: AbortSignal): Promise<void> {
  return new Promise((resolve, reject) => {
    signal?.throwIfAborted();
    const timer = setTimeout(() => { signal?.removeEventListener('abort', abort); resolve(); }, ms);
    function abort() { clearTimeout(timer); reject(signal?.reason); }
    signal?.addEventListener('abort', abort, { once: true });
  });
}

async function poll<T>(read: () => Promise<T | undefined>, timeout: number, signal?: AbortSignal, options: PollOptions = {}): Promise<T | undefined> {
  const deadline = Date.now() + (options.timeoutMs ?? timeout);
  do {
    signal?.throwIfAborted();
    const result = await read();
    if (result !== undefined) return result;
    const left = deadline - Date.now();
    if (left <= 0) break;
    await delay(Math.min(left, options.intervalMs ?? 5000), signal);
  } while (Date.now() < deadline);
}

export const RELOAD_KEY = 'online-gal:setup-reloaded';
export const WRITTEN_KEY = 'online-gal:setup-written-at';
const DEPLOYMENT_KEY = 'online-gal:setup-deployment';

export function readSession(key: string): string | null {
  try { return sessionStorage.getItem(key); } catch { return null; }
}

export function writeSession(key: string, value: string): void {
  try { sessionStorage.setItem(key, value); } catch { /* Retry after signing in again. */ }
}

export function savedDeployment(owner: SiteAdmin): string | null {
  try {
    const data = JSON.parse(readSession(DEPLOYMENT_KEY) ?? 'null');
    return data?.project === projectPath(owner) && typeof data.id === 'string' ? data.id : null;
  } catch { return null; }
}

export function saveDeployment(owner: SiteAdmin, id: string): void {
  writeSession(DEPLOYMENT_KEY, JSON.stringify({ project: projectPath(owner), id }));
}

export function clearSession(...keys: string[]): void {
  try { for (const key of keys) sessionStorage.removeItem(key); } catch { /* Storage is optional. */ }
}

export function clearSetupSession(): void {
  clearSession(RELOAD_KEY, WRITTEN_KEY, DEPLOYMENT_KEY);
}

export class SetupClient {
  private readonly origin: string;
  private readonly account: Pick<CloudflareAccount, 'forward'>;
  private readonly fetcher: typeof fetch;

  constructor(origin: string, account: Pick<CloudflareAccount, 'forward'>, fetcher: typeof fetch = globalThis.fetch.bind(globalThis)) {
    this.origin = origin;
    this.account = account;
    this.fetcher = fetcher;
  }

  async readStatus(signal?: AbortSignal): Promise<SetupStatus | null> {
    const response = await this.fetcher(new URL('/__setup/status', this.origin), { cache: 'no-store', signal: requestSignal(signal) });
    if (response.status === 404) return null;
    if (!response.ok) throw new Error(`读取站点状态失败（HTTP ${response.status}）。`);
    if (!response.headers.get('content-type')?.includes('application/json')) return null;
    const result: Partial<SetupStatus> | null = await response.json();
    if (result?.phase !== 'setup') return null;
    if (typeof result.initialized !== 'boolean') throw new Error('站点状态响应无效。');
    return { phase: 'setup', initialized: result.initialized, v: result.v, warning: result.warning };
  }

  private async api<T>(path: string, signal?: AbortSignal, init?: RequestInit): Promise<T> {
    const request = new Request(new URL(path, this.origin), { ...init, signal: requestSignal(signal, 30_000) });
    const response = await this.account.forward(request, path);
    const envelope = await response.json() as { success?: boolean; result?: T };
    if (!response.ok || envelope.success !== true || envelope.result === undefined) {
      throw new Error(`Cloudflare Pages 请求失败（HTTP ${response.status}）。`);
    }
    return envelope.result;
  }

  async readProject(owner: SiteAdmin, signal?: AbortSignal) {
    return projectState(await this.api<Project>(projectPath(owner), signal));
  }

  async write(owner: SiteAdmin, patch: ReturnType<typeof buildEnvPatch>, signal?: AbortSignal): Promise<'local' | 'switched' | 'already-written' | 'written'> {
    if (isLocalHost(new URL(this.origin).hostname)) return 'local';
    const locks = globalThis.navigator?.locks;
    if (locks) {
      return locks.request('online-gal:setup:' + projectPath(owner), { signal }, () => this.writeAndCheck(owner, patch, signal));
    }
    return this.writeAndCheck(owner, patch, signal);
  }

  private async writeAndCheck(owner: SiteAdmin, patch: ReturnType<typeof buildEnvPatch>, signal?: AbortSignal): Promise<'switched' | 'already-written' | 'written'> {
    const status = await this.readStatus(signal);
    if (!status) return 'switched';
    const project = await this.readProject(owner, signal);
    if (status.initialized || project.written) return 'already-written';
    try {
      await this.api(projectPath(owner), signal, {
        method: 'PATCH', headers: { 'content-type': 'application/json' }, body: JSON.stringify(patch),
      });
    } catch {
      // Upstream errors can contain submitted values.
      throw new Error('无法确认 Pages 配置写入结果。请重试，页面会先核对项目状态。');
    }
    const saved = await this.readProject(owner, signal);
    if (!saved.written || saved.missing.length) throw new Error('写入后未能核对全部初始化项，请检查 Pages 项目。');
    if (saved.record !== patch.deployment_configs.production.env_vars.ONLINE_GAL_SITE.value) return 'already-written';
    writeSession(WRITTEN_KEY, saved.writtenAt ?? new Date().toISOString());
    return 'written';
  }

  async startPagesBuild(owner: SiteAdmin, branch: string, signal?: AbortSignal): Promise<string | null> {
    if (isLocalHost(new URL(this.origin).hostname)) return null;
    if (!branch) throw new Error('Pages 项目缺少生产分支。');
    const body = new FormData();
    body.set('branch', branch);
    const result = await this.api<Deployment>(`${projectPath(owner)}/deployments`, signal, { method: 'POST', body });
    if (!result.id) throw new Error('Cloudflare 没有返回新部署 ID。');
    saveDeployment(owner, result.id);
    return result.id;
  }

  async waitForDeployment(owner: SiteAdmin, id: string, signal?: AbortSignal, options?: PollOptions): Promise<void> {
    const terminal = await poll(async () => {
      const result = await this.api<Deployment>(`${projectPath(owner)}/deployments/${encodeURIComponent(id)}`, signal);
      return buildResult(result);
    }, 30 * 60_000, signal, options);
    if (!terminal) throw new Error('等待部署超时，请到 Cloudflare 控制台查看。');
    reportBuildFailure(terminal);
  }

  async waitForNewProduction(owner: SiteAdmin, after: string | undefined, signal?: AbortSignal, options?: PollOptions): Promise<void> {
    const since = Date.parse(after ?? '');
    const result = await poll(async (): Promise<BuildResult | undefined> => {
      // Without a trustworthy write time, only the live route can confirm a switch.
      if (!Number.isFinite(since)) {
        if (!(await this.readStatus(signal).catch(() => ({ phase: 'setup' })))) return 'success';
        return;
      }
      const deployments = await this.api<Deployment[]>(`${projectPath(owner)}/deployments?env=production`, signal);
      const latest = deployments
        .filter(({ created_on }) => Date.parse(created_on ?? '') > since)
        .sort((a, b) => Date.parse(b.created_on ?? '') - Date.parse(a.created_on ?? ''))[0];
      const state = latest && buildResult(latest);
      if (state) return state;
      // A deployment may already be serving when a tab resumes.
      if (!(await this.readStatus(signal).catch(() => ({ phase: 'setup' })))) return 'success';
    }, 30 * 60_000, signal, options);
    if (!result) throw new Error('等待生产部署超时，请到 Cloudflare 控制台查看。');
    reportBuildFailure(result);
  }

  async waitForSwitch(signal?: AbortSignal, options?: PollOptions): Promise<void> {
    const switched = await poll(async () => {
      try { if (!(await this.readStatus(signal))) return true; }
      catch { signal?.throwIfAborted(); }
    }, 5 * 60_000, signal, options);
    if (!switched) throw new Error('站点还未切换，请稍后刷新。');
  }
}
