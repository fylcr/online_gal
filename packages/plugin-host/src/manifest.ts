import { isPluginCapability, isPrivilegedCapability, type PluginCapability } from './capabilities.js';
import { PluginError } from './errors.js';

/**
 * The plugin API version this host implements.
 *
 * v1 is the whole contract in this package; there is no compatibility layer, so
 * a manifest that says `2` is rejected instead of being run with missing or
 * different semantics. Bumping this is a deliberate breaking change.
 */
export const PLUGIN_API_VERSION = 1;

export type PluginProvenance = 'official' | 'third-party';

const PROVENANCES: readonly PluginProvenance[] = ['official', 'third-party'];

/**
 * What a plugin declares about itself before any of its code runs.
 *
 * `provenance` is a declaration by the distribution channel, not something the
 * plugin can use to unlock permissions: both provenances still go through the
 * same grant checks, and only the *defaults* differ.
 */
export interface PluginManifest {
  /** Stable identity, e.g. `online-gal.translation`. */
  readonly id: string;
  readonly name: string;
  /** Semantic version of the plugin itself. */
  readonly version: string;
  /** Version of the host API the plugin targets. */
  readonly apiVersion: number;
  readonly provenance: PluginProvenance;
  readonly description?: string;
  /** Capabilities the plugin wants; the host may grant a subset. */
  readonly capabilities?: readonly PluginCapability[];
  /** Hooks the plugin intends to answer. Dispatch still needs a handler. */
  readonly hooks?: readonly string[];
  /**
   * Services this plugin publishes to other plugins. `setup` must return an
   * object with exactly these keys.
   */
  readonly provides?: readonly string[];
  /**
   * Services this plugin needs. The kernel activates their providers first and
   * refuses to start this plugin when one is missing or failed.
   */
  readonly requires?: readonly string[];
  /**
   * `manager`: the plugin is a complete site the kernel serves as the page.
   * Exactly one installed manager is active at a time.
   */
  readonly role?: PluginRole;
}

export type PluginRole = 'manager';

const ROLES: readonly PluginRole[] = ['manager'];

const MANIFEST_KEYS = new Set([
  'id',
  'name',
  'version',
  'apiVersion',
  'provenance',
  'description',
  'capabilities',
  'hooks',
  'provides',
  'requires',
  'role',
]);

const ID_PATTERN = /^[a-z0-9]+(?:[.-][a-z0-9]+)*$/;
const VERSION_PATTERN = /^\d+\.\d+\.\d+(?:-[0-9A-Za-z.-]+)?(?:\+[0-9A-Za-z.-]+)?$/;
const HOOK_PATTERN = /^[a-z][a-z0-9]*(?:[.-][a-z0-9]+)*$/;
/** Service names share the hook shape: `engine.registry`, `game.library`. */
const SERVICE_PATTERN = HOOK_PATTERN;

function invalid(message: string): PluginError {
  return new PluginError('manifest-invalid', message);
}

function isRecord(value: unknown): value is Record<string, unknown> {
  return typeof value === 'object' && value !== null && !Array.isArray(value);
}

function stringField(record: Record<string, unknown>, key: string, max = 160): string {
  const value = record[key];
  if (typeof value !== 'string' || value.trim().length === 0) {
    throw invalid(`manifest field "${key}" must be a non-empty string`);
  }
  if (value.length > max) throw invalid(`manifest field "${key}" is longer than ${max} characters`);
  return value;
}

function nameList(
  record: Record<string, unknown>,
  key: string,
  pattern: RegExp,
): readonly string[] | undefined {
  const raw = record[key];
  if (raw === undefined) return undefined;
  if (!Array.isArray(raw)) throw invalid(`manifest field "${key}" must be an array`);
  const seen = new Set<string>();
  for (const entry of raw) {
    if (typeof entry !== 'string' || !pattern.test(entry)) {
      throw invalid(
        `manifest field "${key}" entry "${String(entry)}" must be dot separated lower-case alphanumerics`,
      );
    }
    if (seen.has(entry)) throw invalid(`manifest field "${key}" lists "${entry}" twice`);
    seen.add(entry);
  }
  return [...seen];
}

/**
 * Validates an untrusted value and returns a frozen manifest.
 *
 * Unknown fields are an error rather than being ignored: a typo such as
 * `capabilties` would otherwise silently produce a plugin that runs with fewer
 * permissions than its author expected, which is exactly the failure mode the
 * default-deny model is supposed to make visible.
 */
export function parseManifest(input: unknown): PluginManifest {
  if (!isRecord(input)) throw invalid('manifest must be an object');

  const unknown = Object.keys(input).filter((key) => !MANIFEST_KEYS.has(key));
  if (unknown.length > 0) {
    throw invalid(`manifest has unknown field(s): ${unknown.join(', ')}`);
  }

  const id = stringField(input, 'id', 128);
  if (!ID_PATTERN.test(id)) {
    throw invalid(
      `manifest field "id" must be dot/dash separated lower-case alphanumerics (got "${id}")`,
    );
  }

  const name = stringField(input, 'name', 80);

  const version = stringField(input, 'version', 64);
  if (!VERSION_PATTERN.test(version)) {
    throw invalid(`manifest field "version" must be semver (got "${version}")`);
  }

  const apiVersion = input.apiVersion;
  if (typeof apiVersion !== 'number' || !Number.isInteger(apiVersion) || apiVersion < 0) {
    throw invalid('manifest field "apiVersion" must be a non-negative integer');
  }

  const provenance = input.provenance;
  if (typeof provenance !== 'string' || !PROVENANCES.includes(provenance as PluginProvenance)) {
    throw invalid(`manifest field "provenance" must be one of ${PROVENANCES.join(', ')}`);
  }

  const description = input.description;
  if (description !== undefined && typeof description !== 'string') {
    throw invalid('manifest field "description" must be a string');
  }

  const rawCapabilities = input.capabilities;
  let capabilities: readonly PluginCapability[] | undefined;
  if (rawCapabilities !== undefined) {
    if (!Array.isArray(rawCapabilities)) {
      throw invalid('manifest field "capabilities" must be an array');
    }
    const seen = new Set<string>();
    for (const capability of rawCapabilities) {
      if (!isPluginCapability(capability)) {
        throw invalid(`manifest declares unknown capability "${String(capability)}"`);
      }
      if (seen.has(capability)) {
        throw invalid(`manifest declares capability "${capability}" twice`);
      }
      seen.add(capability);
    }
    capabilities = [...seen] as PluginCapability[];
    if (provenance === 'third-party') {
      const privileged = capabilities.filter(isPrivilegedCapability);
      if (privileged.length > 0) {
        throw invalid(
          `manifest field "capabilities" declares privileged ${privileged.join(', ')}, which only official plugins may request`,
        );
      }
    }
  }

  const hooks = nameList(input, 'hooks', HOOK_PATTERN);
  const provides = nameList(input, 'provides', SERVICE_PATTERN);
  const requires = nameList(input, 'requires', SERVICE_PATTERN);
  const selfRequired = requires?.find((service) => provides?.includes(service));
  if (selfRequired !== undefined) {
    throw invalid(`manifest field "requires" lists "${selfRequired}", which the plugin itself provides`);
  }

  const role = input.role;
  if (role !== undefined && (typeof role !== 'string' || !ROLES.includes(role as PluginRole))) {
    throw invalid(`manifest field "role" must be one of ${ROLES.join(', ')}`);
  }

  const manifest: PluginManifest = {
    id,
    name,
    version,
    apiVersion,
    provenance: provenance as PluginProvenance,
    ...(description === undefined ? {} : { description }),
    ...(capabilities === undefined ? {} : { capabilities }),
    ...(hooks === undefined ? {} : { hooks }),
    ...(provides === undefined ? {} : { provides }),
    ...(requires === undefined ? {} : { requires }),
    ...(role === undefined ? {} : { role: role as PluginRole }),
  };
  return Object.freeze(manifest);
}

/**
 * Rejects a manifest before any plugin code is touched.
 *
 * The check is exact equality on purpose: "close enough" version matching is how
 * a plugin written for a different contract ends up running against this one.
 */
export function assertApiCompatible(
  manifest: PluginManifest,
  apiVersion: number = PLUGIN_API_VERSION,
): void {
  if (manifest.apiVersion === apiVersion) return;
  throw new PluginError(
    'api-version-mismatch',
    `plugin "${manifest.id}" targets plugin API ${manifest.apiVersion}, this host implements ${apiVersion}`,
    manifest.id,
  );
}
