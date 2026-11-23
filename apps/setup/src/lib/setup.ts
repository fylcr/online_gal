import type { CloudflareAccount, CloudflareUser, SiteAdmin } from '@online-gal/plugin-system/account';
import { microCaCommitment, microCaFingerprint } from '@online-gal/plugin-system/attest';
import { verifyOfficialReleaseAt, type VerifiedOfficialPackage } from '@online-gal/plugin-system/release-check';
import { cfRouteOwnerBinding, deriveCfRouteConfig, normalizeCfRouteHost, validCfOwnerId } from '../../../../functions/__cf/route-config.mjs';
import { SETUP_ITEMS, setupItems, type SetupItem } from './setup-items.ts';

export type Rebuild = { kind: 'pages-git' | 'manual' } | { kind: 'github-rerun'; repo: string; runId: string };
export interface SetupConfig { siteHost: string; rebuild: Rebuild; builtAt: string }
export type SetupStatus = { phase: 'setup'; initialized: boolean; v?: 1; warning?: string };

interface Deployment {
  id: string;
  url?: string;
  created_on?: string;
  latest_stage?: { name?: string; status?: string };
  deployment_trigger?: { metadata?: { branch?: string; commit_hash?: string } };
}
interface DeploymentIdentity { url: string; branch: string; commit?: string }
interface ReleaseProgress { project: string; writtenAt: string; bootstrapDeploymentId?: string; signingDeploymentId?: string }
interface SetupWrite {
  project: string;
  ownerId: string;
  record: string;
  previewRecord: string;
  fingerprint: string;
  values: Record<string, string>;
  trust?: ReleaseTrust;
  segment?: string;
}
type WriteResult = 'switched' | 'already-written' | { fingerprint: string; trust?: ReleaseTrust; requiresEntry: boolean };
export interface ReleaseRunResult { stage: 'awaiting-dns'; bootstrapDeploymentId: string; signingDeploymentId: string; dnsName: string; candidateTxt: string; packages: readonly VerifiedOfficialPackage[] }
interface ProjectEnvVar { value?: string; type?: string }
interface Project {
  name?: string;
  domains?: string[];
  production_branch?: string;
  deployment_configs?: {
    production?: { env_vars?: Record<string, ProjectEnvVar | null> | null };
    preview?: { env_vars?: Record<string, ProjectEnvVar | null> | null };
  };
}
interface PollOptions { intervalMs?: number; timeoutMs?: number }
type BuildResult = 'success' | 'failure' | 'canceled';

const TOKEN_GROUP = 'Account API Tokens Write';
const TOKEN_SCOPE = 'account_api_tokens:create';
const GENERATED_ITEMS = new Set(['CLOUDFLARE_ACCOUNT_TOKEN', 'ONLINE_GAL_CA_PRIVATE_JWK', 'ONLINE_GAL_CA_BUILD_TOKEN', 'ONLINE_GAL_CF_ROUTE_KEY']);
const RECOVER_RELEASE = '请使用已登录的 cf 命令行恢复发布。站点已切换时，初始化页需要本次生成的私有入口才能继续。';
const P256_FIELD = /^[A-Za-z0-9_-]{43}$/;

const WRITE_SCOPES = ['pages:read', 'pages:write', TOKEN_SCOPE] as const;

/**
 * Scopes missing from the token response. An empty list means the response carried no `scope` at
 * all, which is not a refusal: let the Pages requests decide instead of blocking a valid grant.
 */
export function missingScopes(scopes: readonly string[]): string[] {
  if (scopes.length === 0) return [];
  return WRITE_SCOPES.filter((scope) => !scopes.includes(scope));
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

const accountPath = (owner: SiteAdmin) => `/__cf/api/accounts/${encodeURIComponent(owner.accountId)}`;
const projectPath = (owner: SiteAdmin) => `${accountPath(owner)}/pages/projects/${encodeURIComponent(owner.projectName)}`;
const requestSignal = (signal?: AbortSignal, ms = 15_000) => signal
  ? AbortSignal.any([signal, AbortSignal.timeout(ms)])
  : AbortSignal.timeout(ms);

export function safeNext(value: string): string {
  return value.startsWith('/') && !value.startsWith('//') && !/[\\\u0000-\u001f\u007f]/.test(value) ? value : '/';
}

export function isLocalHost(host: string): boolean {
  return ['localhost', '127.0.0.1', '[::1]', '::1'].includes(host);
}

export interface ReleaseTrust { host: string; salt: string; hash: string }
export type DnsCandidateState = 'create' | 'reuse' | 'conflict';
export function candidateTxt(trust: ReleaseTrust): string {
  return `v=1; salt=${trust.salt}; hashes=${trust.hash}`;
}
export function candidateDnsName(trust: ReleaseTrust): string { return `_trust.${trust.host}`; }
export function classifyDnsRecords(records: readonly string[], candidate: string): DnsCandidateState {
  if (records.length === 0) return 'create';
  if (records.length === 1 && records[0] === candidate) return 'reuse';
  return 'conflict';
}
export interface SiteRecordOptions { productionBranch?: string; trust?: ReleaseTrust; at?: string; cfRoute?: { v: 1; host: string; ownerBinding: string } }

export function buildSiteRecord(owner: SiteAdmin, user: CloudflareUser | null, options?: SiteRecordOptions): string {
  const { productionBranch, trust, at, cfRoute } = options ?? {};
  return JSON.stringify({ v: 1, account: owner.accountId, project: owner.projectName, by: cfRoute ? '' : user?.id ?? '', at: at ?? new Date().toISOString(),
    ...(productionBranch && trust ? { productionBranch, trust } : {}),
    ...(cfRoute ? { cfRoute } : {}) });
}

/**
 * Items the setup page cannot write: their value would have to be typed in, and decision G removed
 * the form. A change that registers one must decide how its value is obtained before the site is
 * initialized (design.md S7).
 */
export function unsupportedItems(items: readonly SetupItem[] = SETUP_ITEMS): SetupItem[] {
  return items.filter(({ name, source }) => source === 'input' || (source === 'generated' && !GENERATED_ITEMS.has(name)));
}

/** Values for the `account` items: they all come from the confirmed Pages project owner. */
export function accountValues(accountId: string, items: readonly SetupItem[] = SETUP_ITEMS): Record<string, string> {
  return Object.fromEntries(items.filter(({ name, source }) => source === 'account' && name !== 'ONLINE_GAL_CF_OWNER_ID').map(({ name }) => [name, accountId]));
}

/**
 * The single PATCH body. Secret keys follow SETUP_ITEMS, so only the items plus the site record can
 * appear: never an OAuth access or refresh token, never a GitHub token.
 */
export interface EnvPatchOptions { bootstrap?: boolean; previewRecord?: string }

export function buildEnvPatch(record: string, values: Readonly<Record<string, string>>, items: readonly SetupItem[] = SETUP_ITEMS, options: EnvPatchOptions = {}) {
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
  const production: Record<string, { type: string; value: string }> = { ...secrets, ONLINE_GAL_SITE: marker };
  if (options.bootstrap) production.ONLINE_GAL_CA_BOOTSTRAP = { type: 'plain_text', value: '1' };
  return { deployment_configs: {
    production: { env_vars: production },
    preview: { env_vars: { ONLINE_GAL_SITE: { type: 'plain_text', value: options.previewRecord ?? record } } },
  } };
}

async function generateCaKey() {
  for (let attempt = 0; attempt < 3; attempt++) {
    const keys = await crypto.subtle.generateKey({ name: 'ECDSA', namedCurve: 'P-256' }, true, ['sign', 'verify']);
    const { kty, crv, x, y, d } = await crypto.subtle.exportKey('jwk', keys.privateKey);
    if (kty === 'EC' && crv === 'P-256' && x && y && d && [x, y, d].every((part) => P256_FIELD.test(part))) {
      return { privateJwk: JSON.stringify({ kty, crv, x, y, d }), fingerprint: await microCaFingerprint({ kty, crv, x, y }) };
    }
  }
  throw new Error('未能生成有效的微型 CA 私钥。');
}

async function generateBuildSecret(): Promise<string> {
  const bytes = crypto.getRandomValues(new Uint8Array(32));
  return btoa(String.fromCharCode(...bytes)).replace(/\+/g, '-').replace(/\//g, '_').replace(/=+$/, '');
}

function validReleaseTrust(value: unknown): value is ReleaseTrust {
  if (!value || typeof value !== 'object') return false;
  const trust = value as Partial<ReleaseTrust>;
  const labels = typeof trust.host === 'string' ? trust.host.split('.') : [];
  return typeof trust.host === 'string' && trust.host.length <= 253 && labels.length >= 2 &&
    !trust.host.endsWith('.pages.dev') && labels.every((label) => /^[a-z0-9](?:[a-z0-9-]{0,61}[a-z0-9])?$/.test(label)) &&
    typeof trust.salt === 'string' && P256_FIELD.test(trust.salt) &&
    typeof trust.hash === 'string' && /^[a-f0-9]{64}$/.test(trust.hash);
}

async function generateReleaseTrust(host: string, fingerprint: string): Promise<ReleaseTrust> {
  const labels = host.split('.');
  if (host.length > 253 || labels.length < 2 || labels.some((label) => !/^[a-z0-9](?:[a-z0-9-]{0,61}[a-z0-9])?$/.test(label)) ||
      host.endsWith('.pages.dev') || !/[a-z]/.test(labels.at(-1) ?? '') || new URL(`https://${host}`).hostname !== host) {
    throw new Error('初始化域名无效，不能发布 DNS 信任记录。');
  }
  const salt = await generateBuildSecret();
  return { host, salt, hash: await microCaCommitment(fingerprint, 'cf-pages', salt) };
}

function projectState(project: Project) {
  const env = project.deployment_configs?.production?.env_vars ?? {};
  const preview = project.deployment_configs?.preview?.env_vars ?? {};
  const marker = env.ONLINE_GAL_SITE;
  const previewMarker = preview.ONLINE_GAL_SITE;
  const record = marker?.value;
  let writtenAt: string | undefined;
  let productionBranch: string | undefined;
  let trust: ReleaseTrust | undefined;
  let routeHost: string | undefined;
  let routeConfigured = false;
  if (record) {
    try {
      const value = JSON.parse(record);
      const at = value?.at;
      if (typeof at === 'string' && Number.isFinite(Date.parse(at))) writtenAt = at;
      const storedTrust: unknown = value?.trust;
      routeConfigured = value?.cfRoute !== undefined;
      if (typeof value?.productionBranch === 'string' && value.productionBranch) productionBranch = value.productionBranch;
      if (productionBranch && validReleaseTrust(storedTrust)) trust = storedTrust;
      if (productionBranch && value?.cfRoute?.v === 1 && typeof value.cfRoute.host === 'string') {
        const host = normalizeCfRouteHost(value.cfRoute.host);
        if (host === value.cfRoute.host && (!trust || host === trust.host)) routeHost = host;
      }
    } catch { /* Keep malformed nonempty records initialized. */ }
  }
  const expectedItems = SETUP_ITEMS.filter(({ name }) => {
    if (name === 'ONLINE_GAL_CA_BUILD_TOKEN') return Boolean(trust);
    if (name === 'ONLINE_GAL_CF_ROUTE_KEY' || name === 'ONLINE_GAL_CF_OWNER_ID') return routeConfigured;
    return true;
  });
  return {
    written: marker != null && record !== '',
    record,
    writtenAt,
    trust,
    bootstrap: env.ONLINE_GAL_CA_BOOTSTRAP?.value === '1',
    bootstrapType: env.ONLINE_GAL_CA_BOOTSTRAP?.type,
    markerType: marker?.type,
    previewRecord: previewMarker?.value,
    previewMarkerType: previewMarker?.type,
    bootstrapPresent: env.ONLINE_GAL_CA_BOOTSTRAP != null,
    invalidSecretTypes: SETUP_ITEMS.filter(({ name }) => env[name] != null && env[name]?.type !== 'secret_text').map(({ name }) => name),
    domains: project.domains ?? [],
    productionBranch,
    routeHost,
    invalidRoute: routeConfigured && !routeHost,
    previewHasRouteSecrets: preview.ONLINE_GAL_CF_ROUTE_KEY != null || preview.ONLINE_GAL_CF_OWNER_ID != null,
    missing: expectedItems.filter(({ name }) => env[name] == null).map(({ name }) => name),
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
const FINGERPRINT_KEY = 'online-gal:setup-ca-fingerprint';
const DEPLOYMENT_KEY = 'online-gal:setup-deployment';
const RELEASE_PROGRESS_KEY = 'online-gal:setup-release-progress';

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

export function readCaFingerprint(owner: SiteAdmin, writtenAt: string | undefined): string | undefined {
  if (!writtenAt) return;
  try {
    const data = JSON.parse(readSession(FINGERPRINT_KEY) ?? 'null');
    if (data?.project === projectPath(owner) && data.at === writtenAt &&
        typeof data.fingerprint === 'string' && /^[a-f0-9]{64}$/.test(data.fingerprint)) {
      return data.fingerprint;
    }
  } catch { /* Storage is optional. */ }
}

export function clearSession(...keys: string[]): void {
  try { for (const key of keys) sessionStorage.removeItem(key); } catch { /* Storage is optional. */ }
}

export function saveReleaseProgress(owner: SiteAdmin, writtenAt: string, progress: Omit<ReleaseProgress, 'project' | 'writtenAt'>): void {
  writeSession(RELEASE_PROGRESS_KEY, JSON.stringify({ project: projectPath(owner), writtenAt, ...progress }));
}

export function readReleaseProgress(owner: SiteAdmin, writtenAt: string): ReleaseProgress | undefined {
  try {
    const data = JSON.parse(readSession(RELEASE_PROGRESS_KEY) ?? 'null');
    if (data?.project !== projectPath(owner) || data?.writtenAt !== writtenAt) return;
    return {
      project: data.project,
      writtenAt: data.writtenAt,
      ...(typeof data.bootstrapDeploymentId === 'string' ? { bootstrapDeploymentId: data.bootstrapDeploymentId } : {}),
      ...(typeof data.signingDeploymentId === 'string' ? { signingDeploymentId: data.signingDeploymentId } : {}),
    };
  } catch { return; }
}

export function clearSetupSession(): void {
  clearSession(RELOAD_KEY, WRITTEN_KEY, DEPLOYMENT_KEY, RELEASE_PROGRESS_KEY);
}

type SetupAccount = Pick<CloudflareAccount, 'forward'> & Partial<Pick<CloudflareAccount, 'status'>> & {
  setEntry?: (segment: string) => Promise<unknown> | unknown;
};

export class SetupClient {
  private readonly origin: string;
  private readonly account: SetupAccount;
  private readonly fetcher: typeof fetch;
  private readonly verifyRelease: (candidate: ReleaseTrust, origin: string) => Promise<VerifiedOfficialPackage[]>;
  private pendingWrite?: SetupWrite;
  private entry?: { project: string; segment: string; ownerId: string };
  private privateTransport = false;

  constructor(
    origin: string,
    account: SetupAccount,
    fetcher: typeof fetch = globalThis.fetch.bind(globalThis),
    verifyRelease: (candidate: ReleaseTrust, origin: string) => Promise<VerifiedOfficialPackage[]> = verifyOfficialReleaseAt,
  ) {
    this.origin = origin;
    this.account = account;
    this.fetcher = fetcher;
    this.verifyRelease = verifyRelease;
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
    const project = await this.api<Project>(projectPath(owner), signal);
    if (project?.name !== owner.projectName) throw new Error('Cloudflare 返回的 Pages 项目与当前选择不一致。');
    return projectState(project);
  }

  private async createAccountToken(owner: SiteAdmin, signal?: AbortSignal): Promise<string> {
    const path = `${accountPath(owner)}/tokens`;
    let groupId: string | undefined;
    try {
      const groups = await this.api<{ name?: string; id?: string }[]>(`${path}/permission_groups`, signal);
      groupId = groups.find(({ name }) => name === TOKEN_GROUP)?.id;
    } catch {
      signal?.throwIfAborted();
      throw new Error('无法读取 Cloudflare 账户令牌权限组。请检查当前账号权限后重试。');
    }
    if (!groupId) throw new Error('找不到 Cloudflare 权限组 Account API Tokens Write。');
    try {
      const token = await this.api<{ value?: string }>(path, signal, {
        method: 'POST', headers: { 'content-type': 'application/json' },
        body: JSON.stringify({
          name: 'online-gal-setup',
          policies: [{
            effect: 'allow',
            permission_groups: [{ id: groupId }],
            resources: { [`com.cloudflare.api.account.${owner.accountId}`]: '*' },
          }],
        }),
      });
      if (typeof token.value !== 'string' || !token.value.trim()) throw new Error();
      return token.value;
    } catch {
      signal?.throwIfAborted();
      throw new Error('无法创建 Cloudflare 账户令牌。请检查账户的 API 令牌管理权限后重试。');
    }
  }

  async write(owner: SiteAdmin, user: CloudflareUser | null, scopes: readonly string[], signal?: AbortSignal, releaseFlow = true) {
    const missing = missingScopes(scopes);
    if (missing.length) throw new Error(`这次 Cloudflare 授权缺少 ${missing.join('、')}。请重新授权。`);
    const items = setupItems(releaseFlow);
    if (unsupportedItems(items).length) throw new Error('清单中有初始化页暂不支持的项目。');
    if (isLocalHost(new URL(this.origin).hostname)) return 'local';
    const write = () => this.writeAndCheck(owner, user, signal, releaseFlow);
    const locks = globalThis.navigator?.locks;
    if (locks) return locks.request('online-gal:setup:' + projectPath(owner), { signal }, write);
    return write();
  }

  entryLink(owner: SiteAdmin): string | undefined {
    if (this.entry?.project === projectPath(owner)) return `/admin#cf-entry=${this.entry.segment}`;
  }

  clearEntry(): void {
    this.pendingWrite = undefined;
    this.entry = undefined;
  }

  private async writeAndCheck(owner: SiteAdmin, user: CloudflareUser | null, signal?: AbortSignal, releaseFlow = true): Promise<WriteResult> {
    const status = await this.readStatus(signal);
    if (!status) return 'switched';
    const project = await this.readProject(owner, signal);
    const host = releaseFlow ? normalizeCfRouteHost(new URL(this.origin).hostname) : undefined;
    if (releaseFlow && !project.branch) throw new Error('Pages 项目缺少生产分支，无法启用签名构建。');
    if (host && !project.domains.some((domain) => {
      try { return normalizeCfRouteHost(domain) === host; } catch { return false; }
    })) {
      throw new Error('当前站点域名不属于该 Pages 项目的自定义域名。');
    }
    if ((status.initialized || project.written) && this.pendingWrite?.record !== project.record) return 'already-written';
    const ownerId = host ? await this.verifySetupUser(user, signal) : user?.id ?? '';
    const path = projectPath(owner);
    if (this.pendingWrite?.project !== path || this.pendingWrite.ownerId !== ownerId) this.clearEntry();
    if (status.initialized || project.written) {
      const pending = this.pendingWrite;
      if (!pending || pending.record !== project.record) return 'already-written';
      return this.finishWrite(owner, pending, project);
    }
    const pending = this.pendingWrite ??= await this.prepareWrite(owner, user, ownerId, host, project.branch, signal);
    const patch = buildEnvPatch(pending.record, pending.values, setupItems(releaseFlow), { bootstrap: releaseFlow, previewRecord: pending.previewRecord });
    try {
      await this.api(path, signal, {
        method: 'PATCH', headers: { 'content-type': 'application/json' }, body: JSON.stringify(patch),
      });
      return this.finishWrite(owner, pending, await this.readProject(owner, signal));
    } catch {
      // Upstream errors can contain submitted values. Retain the same write in memory for readback/retry.
      throw new Error('无法确认 Pages 配置写入结果。请重试，页面会先核对项目状态。');
    }
  }

  private async verifySetupUser(user: CloudflareUser | null, signal?: AbortSignal): Promise<string> {
    const identity = await this.api<{ id?: unknown }>('/__cf/api/user', signal);
    if (!validCfOwnerId(identity.id) || identity.id !== user?.id) throw new Error('当前 Cloudflare 用户无法核对，请重新登录。');
    return identity.id;
  }

  private async prepareWrite(owner: SiteAdmin, user: CloudflareUser | null, ownerId: string, host?: string, branch?: string, signal?: AbortSignal): Promise<SetupWrite> {
    const ca = await generateCaKey();
    const trust = host ? await generateReleaseTrust(host, ca.fingerprint) : undefined;
    const buildToken = host ? await generateBuildSecret() : undefined;
    const routeKey = host ? await generateBuildSecret() : undefined;
    const route = host && routeKey ? await deriveCfRouteConfig({ accountId: owner.accountId, projectName: owner.projectName, host, routeKey }) : undefined;
    const cfRoute = route ? { v: 1 as const, host: route.host,
      ownerBinding: await cfRouteOwnerBinding(route.accountId, route.projectName, ownerId) } : undefined;
    signal?.throwIfAborted();
    const accountToken = await this.createAccountToken(owner, signal);
    const at = new Date().toISOString();
    return {
      project: projectPath(owner), ownerId, trust, segment: route?.segment,
      fingerprint: ca.fingerprint,
      record: buildSiteRecord(owner, user, { at, productionBranch: branch, trust, cfRoute }),
      previewRecord: buildSiteRecord(owner, host ? null : user, { at }),
      values: {
        ...accountValues(owner.accountId),
        CLOUDFLARE_ACCOUNT_TOKEN: accountToken,
        ONLINE_GAL_CA_PRIVATE_JWK: ca.privateJwk,
        ...(buildToken ? { ONLINE_GAL_CA_BUILD_TOKEN: buildToken } : {}),
        ...(routeKey ? { ONLINE_GAL_CF_ROUTE_KEY: routeKey, ONLINE_GAL_CF_OWNER_ID: ownerId } : {}),
      },
    };
  }

  private finishWrite(owner: SiteAdmin, pending: SetupWrite, saved: ReturnType<typeof projectState>): WriteResult {
    if (saved.written && saved.record !== pending.record) {
      this.clearEntry();
      return 'already-written';
    }
    if (!saved.written || saved.missing.length || saved.invalidSecretTypes.length || saved.markerType !== 'plain_text' ||
        saved.previewMarkerType !== 'plain_text' || saved.previewRecord !== pending.previewRecord || saved.previewHasRouteSecrets ||
        (pending.trust && (!saved.bootstrapPresent || !saved.bootstrap || saved.bootstrapType !== 'plain_text'))) {
      throw new Error('写入后未能核对全部初始化项，请检查 Pages 项目。');
    }
    const { fingerprint, trust, segment } = pending;
    this.entry = segment ? { project: pending.project, segment, ownerId: pending.ownerId } : undefined;
    this.pendingWrite = undefined;
    const at = saved.writtenAt ?? new Date().toISOString();
    clearSession(DEPLOYMENT_KEY, RELEASE_PROGRESS_KEY);
    writeSession(WRITTEN_KEY, at);
    writeSession(FINGERPRINT_KEY, JSON.stringify({ project: projectPath(owner), at, fingerprint }));
    return { fingerprint, trust, requiresEntry: Boolean(segment) };
  }

  private async deployment(owner: SiteAdmin, id: string, signal?: AbortSignal): Promise<Deployment> {
    return this.api<Deployment>(`${projectPath(owner)}/deployments/${encodeURIComponent(id)}`, signal);
  }

  private async deploymentProof(url: string, signal?: AbortSignal): Promise<DeploymentIdentity> {
    const response = await this.fetcher(new URL('/.well-known/online-gal-deployment.json', url), {
      cache: 'no-store', credentials: 'omit', redirect: 'error', signal: requestSignal(signal, 15_000),
    });
    const allowOrigin = response.headers.get('access-control-allow-origin');
    if (new URL(url).origin !== this.origin && allowOrigin !== '*' && allowOrigin !== this.origin) {
      throw new Error('部署身份端点未允许管理页跨域读取。');
    }
    if (!response.ok || response.headers.get('cache-control')?.toLowerCase().includes('no-store') !== true) {
      throw new Error('部署身份端点未返回成功的 no-store 响应。');
    }
    const identity = await response.json() as Partial<DeploymentIdentity>;
    if (typeof identity.url !== 'string' || typeof identity.branch !== 'string') throw new Error('部署身份响应无效。');
    return identity as DeploymentIdentity;
  }

  private async checkDeploymentIdentity(owner: SiteAdmin, id: string, trust: ReleaseTrust, branch: string, signal?: AbortSignal): Promise<string> {
    const deployment = await this.deployment(owner, id, signal);
    if (deployment.id !== id || !deployment.url) throw new Error('Cloudflare 部署身份与请求不匹配。');
    const url = new URL(deployment.url);
    const expectedHost = `${id.slice(0, 8)}.${owner.projectName}.pages.dev`;
    if (url.protocol !== 'https:' || url.hostname !== expectedHost || url.pathname !== '/' || url.search || url.hash) {
      throw new Error('Cloudflare 返回了非预期的 Pages 部署 URL。');
    }
    const [deploymentIdentity, productionIdentity] = await Promise.all([
      this.deploymentProof(url.origin, signal),
      this.deploymentProof(`https://${trust.host}`, signal),
    ]);
    const commit = deployment.deployment_trigger?.metadata?.commit_hash;
    for (const identity of [deploymentIdentity, productionIdentity]) {
      if (identity.url !== url.origin || identity.branch !== branch || (commit && identity.commit !== commit)) {
        throw new Error('生产主机未提供与该 Pages 部署一致的身份凭据。');
      }
    }
    return url.origin;
  }

  private async checkBootstrapReady(trust: ReleaseTrust, signal?: AbortSignal): Promise<void> {
    const response = await this.fetcher(`https://${trust.host}/__ca/issue`, {
      method: 'POST', headers: { 'content-type': 'application/json' }, body: '{}',
      cache: 'no-store', credentials: 'omit', redirect: 'error', signal: requestSignal(signal, 15_000),
    });
    const body = await response.json().catch(() => null) as { error?: string } | null;
    if (response.status !== 401 || body?.error !== 'unauthorized' ||
        response.headers.get('cache-control')?.toLowerCase().includes('no-store') !== true) {
      throw new Error('生产主机的 CA 引导路由未通过无凭据就绪检查。');
    }
  }

  private async activatePrivateTransport(owner: SiteAdmin, signal?: AbortSignal): Promise<void> {
    if (this.privateTransport) return;
    const entry = this.entry;
    if (entry?.project !== projectPath(owner) || !this.account.setEntry || !this.account.status) throw new Error(RECOVER_RELEASE);
    signal?.throwIfAborted();
    try {
      await this.account.setEntry(entry.segment);
      const status = await this.account.status();
      if (!status.signedIn || status.user?.id !== entry.ownerId || status.siteAdmin?.accountId !== owner.accountId ||
          status.siteAdmin.projectName !== owner.projectName) throw new Error();
    } catch {
      signal?.throwIfAborted();
      throw new Error('无法通过私有入口核对 Cloudflare 所有者和 Pages 项目。请重试，或使用已登录的 cf 命令行恢复发布。');
    }
    signal?.throwIfAborted();
    this.privateTransport = true;
  }

  private async clearBootstrap(owner: SiteAdmin, signal?: AbortSignal): Promise<void> {
    const before = await this.readProject(owner, signal);
    if (!before.bootstrap || before.bootstrapType !== 'plain_text') throw new Error('无法确认 Pages 的 CA 引导标记状态，拒绝继续删除。');
    const response = await this.api<unknown>(projectPath(owner), signal, {
      method: 'PATCH', headers: { 'content-type': 'application/json' },
      body: JSON.stringify({ deployment_configs: { production: { env_vars: { ONLINE_GAL_CA_BOOTSTRAP: null } } } }),
    });
    const state = await this.readProject(owner, signal);
    if (state.bootstrapPresent || state.record !== before.record || state.previewRecord !== before.previewRecord ||
        state.missing.length || state.invalidSecretTypes.length || state.previewHasRouteSecrets || state.markerType !== 'plain_text' ||
        state.previewMarkerType !== 'plain_text' || !state.previewRecord) {
      throw new Error('无法确认已清除 Pages 的 CA 引导标记并保留有效初始化配置。');
    }
    void response;
  }

  private async releaseDeployments(owner: SiteAdmin, state: ReturnType<typeof projectState>, branch: string, signal?: AbortSignal): Promise<Deployment[]> {
    const after = Date.parse(state.writtenAt ?? '');
    if (!Number.isFinite(after)) throw new Error('初始化记录缺少有效时间，无法从 Pages 部署历史恢复验收。');
    const deployments = await this.api<Deployment[]>(`${projectPath(owner)}/deployments?env=production`, signal);
    return deployments.filter(({ id, created_on, deployment_trigger }) =>
      typeof id === 'string' && Date.parse(created_on ?? '') > after && deployment_trigger?.metadata?.branch === branch)
      .sort((a, b) => Date.parse(a.created_on ?? '') - Date.parse(b.created_on ?? ''));
  }

  /** Run/resume the two production builds, verify identity and signed packages, then stop before DNS. */
  async advanceRelease(owner: SiteAdmin, trust: ReleaseTrust, signal?: AbortSignal, options?: PollOptions, restartBuild = false, requiresEntry = false): Promise<ReleaseRunResult> {
    if (isLocalHost(new URL(this.origin).hostname)) throw new Error('本地开发模式不支持 Pages 发布验收。');
    if (new URL(this.origin).hostname !== trust.host) throw new Error('待验收 CA 域名与当前管理页不一致。');
    if (requiresEntry) {
      if (this.entry?.project !== projectPath(owner)) throw new Error(RECOVER_RELEASE);
      if (!this.privateTransport && !(await this.readStatus(signal))) await this.activatePrivateTransport(owner, signal);
    }
    const state = await this.readProject(owner, signal);
    if (Boolean(state.routeHost) !== requiresEntry || state.invalidRoute || state.previewHasRouteSecrets || state.missing.length || state.invalidSecretTypes.length) {
      throw new Error('Pages 私有入口配置与当前初始化会话不一致，请使用已登录的 cf 命令行恢复发布。');
    }
    if (!state.written || !state.trust || state.trust.host !== trust.host || state.trust.salt !== trust.salt || state.trust.hash !== trust.hash) {
      throw new Error('Pages 项目中的初始化记录与待验收 CA 不一致。');
    }
    if (state.bootstrapPresent && (!state.bootstrap || state.bootstrapType !== 'plain_text')) {
      throw new Error('Pages 的 CA 引导标记无效，拒绝开始部署验收。');
    }
    const branch = state.productionBranch;
    if (!branch || branch !== state.branch) throw new Error('初始化记录的生产分支与 Pages 项目不一致。');
    if (!state.writtenAt) throw new Error('初始化记录缺少有效时间，无法恢复发布验签。');
    const progress = readReleaseProgress(owner, state.writtenAt) ?? { project: projectPath(owner), writtenAt: state.writtenAt };
    let bootstrapDeploymentId = progress.bootstrapDeploymentId;
    let signingDeploymentId = progress.signingDeploymentId;
    if (!bootstrapDeploymentId || (state.bootstrapPresent === false && !signingDeploymentId)) {
      const history = await this.releaseDeployments(owner, state, branch, signal);
      const usable = history.filter(({ latest_stage }) => latest_stage?.status !== 'failure' && latest_stage?.status !== 'canceled');
      bootstrapDeploymentId ??= usable[0]?.id;
      if (state.bootstrapPresent === false && !signingDeploymentId && bootstrapDeploymentId) {
        const bootstrap = history.find(({ id }) => id === bootstrapDeploymentId);
        const bootstrapAt = Date.parse(bootstrap?.created_on ?? '');
        signingDeploymentId = usable.filter(({ id, created_on }) => id !== bootstrapDeploymentId &&
          Number.isFinite(bootstrapAt) && Date.parse(created_on ?? '') > bootstrapAt).at(-1)?.id;
      }
    }
    if (restartBuild) {
      signingDeploymentId = undefined;
      if (state.bootstrapPresent !== false) bootstrapDeploymentId = undefined;
      saveReleaseProgress(owner, state.writtenAt, { ...(bootstrapDeploymentId ? { bootstrapDeploymentId } : {}) });
    } else if (bootstrapDeploymentId) {
      saveReleaseProgress(owner, state.writtenAt, { bootstrapDeploymentId, ...(signingDeploymentId ? { signingDeploymentId } : {}) });
    }
    if (state.bootstrapPresent !== false) {
      if (!bootstrapDeploymentId) {
        bootstrapDeploymentId = await this.startPagesBuild(owner, branch, signal) ?? undefined;
        if (!bootstrapDeploymentId) throw new Error('Cloudflare 未返回 CA 引导部署 ID。');
        saveReleaseProgress(owner, state.writtenAt, { bootstrapDeploymentId });
      }
      const handleSwitch = requiresEntry ? async () => {
        if (!this.privateTransport && !(await this.readStatus(signal))) await this.activatePrivateTransport(owner, signal);
      } : undefined;
      await this.waitForDeployment(owner, bootstrapDeploymentId, signal, options, handleSwitch);
      await this.checkDeploymentIdentity(owner, bootstrapDeploymentId, trust, branch, signal);
      await this.checkBootstrapReady(trust, signal);
      if (requiresEntry && !this.privateTransport) {
        await this.waitForSwitch(signal, options);
        await this.activatePrivateTransport(owner, signal);
      }
      await this.checkDeploymentIdentity(owner, bootstrapDeploymentId, trust, branch, signal);
      await this.clearBootstrap(owner, signal);
    } else if (!bootstrapDeploymentId) {
      throw new Error('缺少 CA 引导部署 ID；请先完成第一次 Pages 部署。');
    }
    if (!bootstrapDeploymentId) throw new Error('未能恢复 CA 引导部署 ID。');
    if (requiresEntry && !this.privateTransport) throw new Error(RECOVER_RELEASE);
    if (!signingDeploymentId) {
      signingDeploymentId = await this.startPagesBuild(owner, branch, signal) ?? undefined;
      if (!signingDeploymentId) throw new Error('Cloudflare 未返回签名部署 ID。');
      saveReleaseProgress(owner, state.writtenAt, { bootstrapDeploymentId, signingDeploymentId });
    }
    await this.waitForDeployment(owner, signingDeploymentId, signal, options);
    const signingOrigin = await this.checkDeploymentIdentity(owner, signingDeploymentId, trust, branch, signal);
    const packages = await this.verifyRelease(trust, signingOrigin);
    await this.checkDeploymentIdentity(owner, signingDeploymentId, trust, branch, signal);
    return { stage: 'awaiting-dns', bootstrapDeploymentId, signingDeploymentId, dnsName: candidateDnsName(trust), candidateTxt: candidateTxt(trust), packages };
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

  async waitForDeployment(owner: SiteAdmin, id: string, signal?: AbortSignal, options?: PollOptions, beforePoll?: () => Promise<void>): Promise<void> {
    const terminal = await poll(async () => {
      await beforePoll?.();
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
