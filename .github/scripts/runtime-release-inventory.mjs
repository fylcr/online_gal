#!/usr/bin/env node
/**
 * Read-only exact-identity inventory for one ONS/KRKR version.
 *
 * An exact tag or release 404 is absence. The paginated release list is different:
 * it is 200 with an empty array on a repository that has no releases, so a non-200
 * list status -- including 404 -- is recorded as that status (or 0 on a network
 * error) and the policy fails closed. The exact version history comes from that
 * list. This file never creates, uploads, or deletes a release. Tests pass a fake
 * `gh` instead of calling GitHub.
 */
import { execFileSync } from 'node:child_process';
import { isSemVer, releaseIdentity } from './runtime-release-policy.mjs';

/**
 * `gh api --paginate` writes one JSON array per page back to back, e.g. `[...][...]`.
 * Extract each complete array without depending on a jq build or a specific gh version.
 */
function parsePages(text) {
  const entries = [];
  let index = 0;
  while (index < text.length) {
    while (index < text.length && /[\s,]/.test(text[index])) index += 1;
    if (index >= text.length) break;
    if (text[index] !== '[') throw new Error('gh api returned a page that is not a JSON array');
    let depth = 0;
    let inString = false;
    let escaped = false;
    const start = index;
    for (; index < text.length; index += 1) {
      const char = text[index];
      if (inString) {
        if (escaped) escaped = false;
        else if (char === '\\') escaped = true;
        else if (char === '"') inString = false;
        continue;
      }
      if (char === '"') inString = true;
      else if (char === '[') depth += 1;
      else if (char === ']') {
        depth -= 1;
        if (depth === 0) { index += 1; break; }
      }
    }
    if (depth !== 0) throw new Error('gh api returned an unterminated JSON page');
    const page = JSON.parse(text.slice(start, index));
    if (!Array.isArray(page)) throw new Error('gh api returned a page that is not a JSON array');
    entries.push(...page);
  }
  return entries;
}

export function lookup(repository, engine, version, execFile = execFileSync) {
  if (engine !== 'ons' && engine !== 'krkr') throw new Error(`not an immutable engine: ${engine}`);
  if (!repository || !/^[^/\s]+\/[^/\s]+$/.test(repository)) throw new Error('repository is not owner/name');
  if (!isSemVer(version)) throw new Error(`version is not an exact SemVer: ${version}`);
  const { tag, tarball } = releaseIdentity(engine, version);
  const encodedTag = encodeURIComponent(tag);
  const rows = { tag: [], release: [], asset: [], history: [] };
  const api = (path, extra = []) => {
    try {
      const stdout = execFile('gh', ['api', '--method', 'GET', ...extra, path], {
        encoding: 'utf8',
        stdio: ['ignore', 'pipe', 'pipe'],
        env: process.env,
      });
      return { status: 200, body: stdout };
    } catch (cause) {
      const text = `${cause.stderr ?? ''}\n${cause.stdout ?? ''}\n${cause.message ?? ''}`;
      const match = /\b(404|401|403|500|502|503)\b/.exec(text);
      return { status: match ? Number(match[1]) : 0, body: '' };
    }
  };
  const tagResult = api(`/repos/${repository}/git/ref/tags/${encodedTag}`);
  rows.tag.push({ status: tagResult.status, tag: tagResult.status === 200 ? tag : undefined });
  const releaseResult = api(`/repos/${repository}/releases/tags/${encodedTag}`);
  if (releaseResult.status === 200) {
    let body = {};
    try { body = JSON.parse(releaseResult.body); } catch { body = {}; }
    rows.release.push({ status: 200, tag, version });
    for (const asset of body.assets ?? []) rows.asset.push({ status: 200, name: asset.name, tag });
  } else {
    rows.release.push({ status: releaseResult.status });
  }
  // Version history: the full paginated release list, filtered to this exact identity.
  // A row qualifies only when its tag is exactly engine-<engine>-<version> or it carries
  // the exact scoped tarball for this version. Body text, release titles and loose asset
  // names (e.g. *.wasm) are never matched. The list answers 200 + [] when the exact
  // identity is absent; any non-200 status (including a 404, which means the
  // repository or token scope is unknown, not that history is empty), a network
  // error or a malformed page is stored with its actual status so collisionHits
  // fails closed.
  const historyResult = api(`/repos/${repository}/releases`, ['--paginate']);
  if (historyResult.status !== 200) {
    rows.history.push({ status: historyResult.status });
  } else {
    for (const release of parsePages(historyResult.body)) {
      if (release === null || typeof release !== 'object') continue;
      const sameTag = release.tag_name === tag;
      const assetNames = (Array.isArray(release.assets) ? release.assets : [])
        .map((asset) => (asset && typeof asset.name === 'string' ? asset.name : null));
      const bearsTarball = assetNames.includes(tarball);
      if (!sameTag && !bearsTarball) continue;
      rows.history.push({
        status: 200,
        tag: sameTag ? tag : release.tag_name,
        version,
        name: bearsTarball ? tarball : undefined,
      });
    }
  }
  return rows;
}

function arg(argv, name) {
  const index = argv.indexOf(name);
  return index >= 0 ? argv[index + 1] : undefined;
}

if (import.meta.url === new URL(`file://${process.argv[1]?.replace(/\\/g, '/') ?? ''}`).href
  || process.argv[1]?.endsWith('runtime-release-inventory.mjs')) {
  const repository = arg(process.argv, '--repository');
  const engine = arg(process.argv, '--engine');
  const version = arg(process.argv, '--version') ?? '';
  try {
    process.stdout.write(`${JSON.stringify(lookup(repository, engine, version))}\n`);
  } catch (cause) {
    process.stderr.write(`runtime-release-inventory: ${cause.message}\n`);
    process.exit(1);
  }
}
