import { hex, sha256 } from './rekor.js';

export interface ReleaseTrust {
  readonly host: string;
  readonly salt: string;
  readonly hash: string;
}

export function parseReleaseTrust(value: unknown): ReleaseTrust {
  if (typeof value !== 'object' || value === null || !('host' in value) || !('salt' in value) || !('hash' in value)) {
    throw new Error('release trust requires a host, salt and commitment hash');
  }
  const { host, salt, hash } = value;
  // Accept canonical DNS hostnames only. This host can receive the build token.
  if (typeof host !== 'string' || host.length > 253 || !host.includes('.') ||
      host.split('.').some((label) => !/^[a-z0-9](?:[a-z0-9-]{0,61}[a-z0-9])?$/.test(label)) ||
      !/[a-z]/.test(host.split('.').at(-1) ?? '') || new URL(`https://${host}`).hostname !== host) {
    throw new Error('release trust host must be a canonical DNS hostname');
  }
  if (typeof salt !== 'string' || !/^[A-Za-z0-9_-]+$/.test(salt) ||
      typeof hash !== 'string' || !/^[0-9a-f]{64}$/.test(hash)) {
    throw new Error('release trust has an invalid salt or commitment hash');
  }
  return { host, salt, hash };
}

/** The manifest digests already bind every payload file, including lazy files. */
export async function officialPackagePath(id: string, envelope: BufferSource, manifest: BufferSource, bundle?: BufferSource): Promise<string> {
  const parts = await Promise.all((bundle === undefined ? [envelope, manifest] : [envelope, manifest, bundle]).map(sha256));
  const input = new Uint8Array(parts.length * 32);
  parts.forEach((part, i) => input.set(part, i * 32));
  return `p/${id}/${hex(await sha256(input)).slice(0, 16)}/`;
}
