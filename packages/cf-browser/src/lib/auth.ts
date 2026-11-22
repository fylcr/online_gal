/**
 * Browser `#lib/auth`. Requests go to the kernel's same-origin relay `/__cf/api`, which adds the
 * signed-in user's token; nothing here ever sees one (cf-browser spec, "Requests carry no token
 * in the page"). Account, zone and Worker resolution follow upstream's order with the browser's
 * sources: session variables stand in for the environment, and the account that owns this site
 * is the default.
 */
import { ApiError, Client, withResultInfo } from '#api';
import listAccounts from '#operations/accounts/list.js';
import { isId, resolveZoneId } from '#lib/resolve.js';

import { apiBaseUrl, env, io, run, runFetch } from '../context.ts';
import { RawBytes } from './raw-fetch.ts';

export type Cloudflare = Client;

export interface CommandClientArgs {
  dryRun?: boolean;
  local?: boolean;
  persistTo?: string;
  [key: string]: unknown;
}

export async function createCommandClient(args: CommandClientArgs = {}): Promise<Client> {
  if (args.local) throw new Error('--local is not available in the browser.');
  return new Client({ baseUrl: apiBaseUrl(), timeoutInSeconds: 60, maxRetries: 0, fetch: runFetch() });
}

let selectedAccount: { forRun: object; id: string } | null = null;

/** Never prompts and never touches the network (used by generated dry runs). */
export async function resolveAccountIdSilent(): Promise<string | undefined> {
  return env('CLOUDFLARE_ACCOUNT_ID') ?? (selectedAccount?.forRun === run() ? selectedAccount.id : undefined);
}

/**
 * `CLOUDFLARE_ACCOUNT_ID`, then the account that owns this site, then the only account the user
 * can see. Anything else is ambiguous and the user must choose.
 */
export async function getAccountId(): Promise<string> {
  const silent = await resolveAccountIdSilent();
  if (silent) return silent;
  const status = await io().account.status();
  if (!status.signedIn) throw new Error("Not signed in. Run 'cf auth login' or sign in from the account settings.");
  let id = status.siteAdmin?.accountId;
  if (!id) {
    const client = await createCommandClient();
    const listed = (await client.send(listAccounts, { per_page: 50 })) as { result?: { id: string; name: string }[] };
    const accounts = listed.result ?? [];
    if (accounts.length === 1) id = accounts[0]!.id;
    else if (accounts.length === 0) throw new Error('This login cannot see any Cloudflare account.');
    else {
      const names = accounts.map((account) => `  ${account.id}  ${account.name}`).join('\n');
      throw new Error(
        `More than one account is available; choose one with\n  export CLOUDFLARE_ACCOUNT_ID=<id>\n\n${names}`,
      );
    }
  }
  selectedAccount = { forRun: run(), id };
  return id;
}

export async function getZoneId(
  argv: { zone?: string; zoneId?: string } = {},
  client?: Client,
  options: { quiet?: boolean } = {},
): Promise<string> {
  let value = argv.zoneId ?? argv.zone;
  if (!value) {
    value = env('CLOUDFLARE_ZONE_ID');
    if (value && !options.quiet && !run().quiet) io().write('stderr', `Using zone: ${value} (from CLOUDFLARE_ZONE_ID)\n`);
  }
  if (!value) {
    throw new Error(
      'No zone specified.\n\nPlease provide one of the following:\n' +
        '  1. Zone as a positional argument\n  2. --zone flag (or -z)\n  3. export CLOUDFLARE_ZONE_ID=<zone>',
    );
  }
  if (isId(value) || !client) return value;
  return resolveZoneId(client, await getAccountId(), value);
}

export async function getWorkerName(argv: { scriptName?: string; quiet?: boolean } = {}): Promise<string> {
  if (argv.scriptName) return argv.scriptName;
  throw new Error('Required Worker name missing. Please specify the Worker name with `--worker <name>`.');
}

type HttpMethod = 'GET' | 'POST' | 'PUT' | 'PATCH' | 'DELETE';

interface PassthroughOptions {
  body?: unknown;
  query?: Record<string, unknown>;
  headers?: Record<string, string>;
  signal?: AbortSignal;
  timeout?: number;
  preserveNonJsonBytes?: boolean;
}

function passthroughUrl(path: string, baseUrl: string, query: Record<string, unknown> = {}): URL {
  const base = new URL(`${baseUrl.replace(/\/+$/, '')}/`);
  const url = new URL(path.replace(/^\/+/, ''), base);
  if (url.origin !== base.origin) {
    throw new Error(`Refusing to send an authenticated request to ${url.origin} (expected ${base.origin})`);
  }
  for (const [key, value] of Object.entries(query)) {
    for (const item of Array.isArray(value) ? value : [value]) {
      if (item !== undefined && item !== null && item !== '') url.searchParams.append(key, String(item));
    }
  }
  return url;
}

const isRawBody = (body: unknown) =>
  body instanceof FormData ||
  body instanceof Blob ||
  body instanceof ArrayBuffer ||
  ArrayBuffer.isView(body) ||
  body instanceof ReadableStream ||
  body instanceof URLSearchParams;

/** Generic API passthrough, with upstream's body, envelope and error rules. */
export async function requestApi<T>(
  client: Client,
  method: HttpMethod,
  path: string,
  options: PassthroughOptions = {},
): Promise<T> {
  const url = passthroughUrl(path, client.baseUrl, options.query);
  const headers: Record<string, string> = { ...options.headers };
  const hasContentType = Object.keys(headers).some((key) => key.toLowerCase() === 'content-type');
  let body: BodyInit | undefined;
  if (options.body === undefined) body = undefined;
  else if (isRawBody(options.body)) body = options.body as BodyInit;
  else if (typeof options.body === 'string' && hasContentType) body = options.body;
  else {
    body = JSON.stringify(options.body);
    if (!hasContentType) headers['Content-Type'] = 'application/json';
  }
  const timeout = options.timeout ?? 60_000;
  let response: Response;
  try {
    response = await client.fetch(
      url,
      { method, headers, body, signal: options.signal },
      { timeoutInSeconds: timeout / 1000, abortSignal: options.signal },
    );
  } catch (error) {
    if (options.signal?.aborted || io().signal.aborted) throw new Error('Request aborted');
    if (error instanceof Error && error.name === 'ApiTimeoutError') throw new Error(`Request timed out after ${timeout}ms`);
    throw new Error(`Request failed: ${error instanceof Error ? error.message : String(error)}`);
  }
  const text = response.status === 204 ? '' : await response.clone().text();
  const json = (() => {
    try {
      return text ? (JSON.parse(text) as unknown) : undefined;
    } catch {
      return undefined;
    }
  })();
  if (!response.ok) throw new ApiError({ statusCode: response.status, body: json ?? text });
  if (!text) return null as T;
  if (json === undefined) {
    if (options.preserveNonJsonBytes) {
      return new RawBytes(await response.arrayBuffer()) as T;
    }
    return text as T;
  }
  const envelope = json as { success?: boolean; errors?: { message?: string }[]; result?: unknown; result_info?: unknown };
  if (envelope && typeof envelope === 'object' && envelope.success === false) {
    const messages = (envelope.errors ?? []).map((error) => error.message).filter(Boolean).join('; ');
    throw new ApiError({ message: `API error: ${messages || 'Cloudflare API request failed'}`, statusCode: response.status, body: json });
  }
  if (envelope && typeof envelope === 'object' && 'result' in envelope) {
    return withResultInfo(envelope.result as object, envelope.result_info as never) as T;
  }
  return json as T;
}
