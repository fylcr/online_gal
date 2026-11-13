import { KernelError } from './errors.js';

/**
 * Where a plugin comes from. The source, not the manifest, decides how far a
 * plugin is trusted:
 *
 * - `official:<id>` — the official registry on this site (Cloudflare Pages).
 *   The package must say `provenance: "official"` and match the registry entry.
 * - `github:<owner>/<repo>@<tag>` — a tagged release of a GitHub repository,
 *   read from `raw.githubusercontent.com` at that tag. The package must say
 *   `provenance: "third-party"` and may not use an `online-gal.` id.
 *
 * Only tags are accepted for GitHub, never branches: a tag names one release,
 * so installing it twice gives the same files.
 */
export type PluginSource =
  | { readonly kind: 'official'; readonly id: string }
  | { readonly kind: 'github'; readonly owner: string; readonly repo: string; readonly tag: string };

const ID = /^[a-z0-9]+(?:[.-][a-z0-9]+)*$/;
const OWNER = /^[A-Za-z0-9](?:[A-Za-z0-9-]{0,38})$/;
const REPO = /^[A-Za-z0-9._-]{1,100}$/;
const TAG = /^[A-Za-z0-9._+-]{1,128}$/;

/** Ids under this prefix are reserved for official plugins. */
export const OFFICIAL_ID_PREFIX = 'online-gal.';

function invalid(ref: string, why: string): KernelError {
  return new KernelError('source-invalid', `plugin source "${ref}" ${why}`);
}

export function parseSource(ref: string): PluginSource {
  if (typeof ref !== 'string') throw invalid(String(ref), 'must be a string');
  if (ref.startsWith('official:')) {
    const id = ref.slice('official:'.length);
    if (!ID.test(id)) throw invalid(ref, 'does not name a valid plugin id');
    return { kind: 'official', id };
  }
  if (ref.startsWith('github:')) {
    const match = /^github:([^/@]+)\/([^/@]+)@(.+)$/.exec(ref);
    if (match === null) throw invalid(ref, 'must look like github:<owner>/<repo>@<tag>');
    const [, owner = '', repo = '', tag = ''] = match;
    if (!OWNER.test(owner)) throw invalid(ref, 'has an invalid GitHub owner');
    if (!REPO.test(repo) || repo === '.' || repo === '..') throw invalid(ref, 'has an invalid repository name');
    if (!TAG.test(tag) || tag === '.' || tag === '..') throw invalid(ref, 'has an invalid tag');
    return { kind: 'github', owner, repo, tag };
  }
  throw invalid(ref, 'must start with "official:" or "github:"');
}

export function formatSource(source: PluginSource): string {
  return source.kind === 'official'
    ? `official:${source.id}`
    : `github:${source.owner}/${source.repo}@${source.tag}`;
}

/** Base URL (ending in `/`) of a GitHub tag on raw.githubusercontent.com. */
export function githubBase(source: Extract<PluginSource, { kind: 'github' }>, raw: string): string {
  const root = raw.endsWith('/') ? raw : `${raw}/`;
  return `${root}${encodeURIComponent(source.owner)}/${encodeURIComponent(source.repo)}/refs/tags/${encodeURIComponent(source.tag)}/`;
}
