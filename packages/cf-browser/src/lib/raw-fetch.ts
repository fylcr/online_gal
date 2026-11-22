/**
 * Browser `#lib/raw-fetch`: raw API requests through the kernel relay (which adds the token).
 * Callers treat the result as a Node `Buffer` and only call `toString("utf-8")` on it or hand it
 * back to `writeRawOutput`, so `RawBytes` provides exactly that.
 */
import { ApiError } from '#api';

import { apiBaseUrl, io, runFetch } from '../context.ts';

export class RawBytes extends Uint8Array {
  override toString(encoding?: string): string {
    if (encoding === undefined || /^utf-?8$/i.test(encoding)) return new TextDecoder().decode(this);
    if (encoding === 'base64') {
      let binary = '';
      for (const byte of this) binary += String.fromCharCode(byte);
      return btoa(binary);
    }
    throw new Error(`unsupported encoding ${encoding}`);
  }
}

export type RawFetchBody = FormData | Uint8Array | ArrayBuffer | string | Record<string, unknown> | unknown[];

export interface FetchRawOptions {
  method?: 'GET' | 'POST' | 'PUT' | 'PATCH' | 'DELETE';
  body?: RawFetchBody | null;
  contentType?: string;
  headers?: Record<string, string>;
  local?: boolean;
  persistTo?: string;
}

const isUpload = (body: unknown) =>
  body instanceof FormData || body instanceof Uint8Array || body instanceof ArrayBuffer;

export async function fetchRawBytes(path: string, options: FetchRawOptions = {}): Promise<RawBytes> {
  if (options.local) throw new Error('--local is not available in the browser.');
  const headers = new Headers(options.headers);
  let body: BodyInit | undefined;
  const value = options.body;
  if (value instanceof FormData) body = value;
  else if (value instanceof Uint8Array || value instanceof ArrayBuffer) {
    body = value instanceof Uint8Array ? new Uint8Array(value) : value;
    if (!headers.has('content-type')) headers.set('content-type', options.contentType ?? 'application/octet-stream');
  } else if (typeof value === 'string') {
    body = value;
    if (options.contentType && !headers.has('content-type')) headers.set('content-type', options.contentType);
  } else if (value !== undefined && value !== null) {
    body = JSON.stringify(value);
    if (!headers.has('content-type')) headers.set('content-type', 'application/json');
  }
  const url = `${apiBaseUrl().replace(/\/+$/, '')}${path}`;
  const signal = isUpload(value) ? undefined : AbortSignal.timeout(60_000);
  const response = await runFetch()(url, { method: options.method ?? 'GET', headers, body, signal });
  if (!response.ok) {
    const text = await response.text();
    let parsed: unknown = text;
    try {
      parsed = JSON.parse(text);
    } catch {}
    throw new ApiError({ statusCode: response.status, body: parsed });
  }
  return new RawBytes(await response.arrayBuffer());
}

export function writeRawOutput(value: Uint8Array | string): void {
  if (typeof value === 'string') {
    io().write('stdout', value);
    return;
  }
  const text = new TextDecoder('utf-8', { fatal: true });
  try {
    io().write('stdout', text.decode(value));
  } catch {
    io().writeBytes(value, 'cf-output.bin');
  }
}
