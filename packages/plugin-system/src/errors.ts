/**
 * Stable failure codes of the plugin system (the Service Worker kernel).
 *
 * Manifest problems keep the plugin-host codes (`manifest-invalid`,
 * `api-version-mismatch`); everything about fetching, verifying and storing a
 * plugin package is reported with one of these, so the manager and the
 * recovery page can react to a code instead of parsing messages.
 */
export type KernelErrorCode =
  | 'source-invalid'
  | 'package-invalid'
  | 'registry-invalid'
  | 'not-in-registry'
  | 'fetch-failed'
  | 'integrity-failed'
  | 'too-large'
  | 'provenance-mismatch'
  | 'reserved-id'
  | 'source-conflict'
  | 'not-installed'
  | 'not-a-manager'
  | 'manager-in-use'
  | 'request-invalid'
  | 'storage-missing'
  | 'attestation-invalid'
  | 'attestation-unavailable'
  // Accounts (openspec cf-account-admin): not the official manager calling, not signed in,
  // no vault in this kernel, Cloudflare refused or answered something unusable.
  | 'account-forbidden'
  | 'account-signed-out'
  | 'account-unavailable'
  | 'account-failed'
  | 'entry-required'
  | 'entry-invalid'
  | 'not-admin'
  | 'proof-invalid'
  | 'temporarily-unavailable';

export class KernelError extends Error {
  override readonly name = 'KernelError';

  constructor(
    readonly code: KernelErrorCode,
    message: string,
    readonly pluginId: string | null = null,
  ) {
    super(message);
  }
}
