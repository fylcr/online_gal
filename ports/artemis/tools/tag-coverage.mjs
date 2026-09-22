#!/usr/bin/env node
/**
 * Which of the official engine's script tags does the browser
 * reimplementation not handle at all?
 *
 * ## Why this matters more than a class-count gap analysis
 *
 * The official SDK ships one documentation file per tag (`Docs/tag/<category>/
 * <tag>.txt`, 187 of them). That is the complete language the engine accepts.
 * `artemis-compat` is the implementation that actually runs in the browser, and
 * its dispatcher recognises a fixed list of names. A tag the official engine
 * accepts but the reimplementation ignores is NOT a clean failure: the script
 * keeps running and the visual novel silently diverges — a missing character
 * sprite, a skipped transition, a stuck wait. Those are the bugs that look like
 * the mouse-mapping defect.
 *
 * So this lists the official tag vocabulary against the reimplementation's
 * dispatch table, and prints what is missing, grouped the way the docs group it.
 *
 * ## Usage
 *
 *   node tag-coverage.mjs --docs .ref/artemis/Docs/tag
 *   node tag-coverage.mjs --docs .ref/artemis/Docs/tag --missing
 */
import { readdirSync, existsSync, readFileSync } from 'node:fs';
import { join, resolve, basename } from 'node:path';

const REPO = resolve(import.meta.dirname, '..', '..', '..');
const args = process.argv.slice(2);
const docsRoot = args.includes('--docs')
  ? resolve(args[args.indexOf('--docs') + 1])
  : resolve(REPO, '.ref', 'artemis', 'Docs', 'tag');
const onlyMissing = args.includes('--missing');

if (!existsSync(docsRoot)) {
  console.error(`tag docs not found: ${docsRoot}`);
  process.exit(1);
}

/** Every tag the official engine documents, grouped by doc category. */
const official = new Map();
for (const category of readdirSync(docsRoot, { withFileTypes: true })) {
  if (!category.isDirectory()) continue;
  const dir = join(docsRoot, category.name);
  const tags = readdirSync(dir)
    .filter((f) => f.endsWith('.txt'))
    .map((f) => basename(f, '.txt').toLowerCase());
  official.set(category.name, new Set(tags));
}

/**
 * Tag names the reimplementation dispatches. These are read out of the source
 * rather than hard-coded, so the list cannot drift from the code: any quoted
 * lowercase identifier in the script layer is a candidate tag name, and the
 * dispatch file's own string literals are the authoritative set.
 */
function compatTags() {
  const root = join(REPO, '.ref', 'artemis-compat', 'src', 'script');
  const names = new Set();
  if (!existsSync(root)) return names;
  for (const file of readdirSync(root)) {
    if (!/\.(cpp|h)$/.test(file)) continue;
    const text = readFileSync(join(root, file), 'utf8');
    // `tagname == "x"` and `{ "x", handler }` table entries both look like this.
    for (const m of text.matchAll(/"(?<t>[a-z_][a-z0-9_]{1,24})"/g)) names.add(m.groups.t);
  }
  return names;
}

const compat = compatTags();
const allOfficial = new Set([...official.values()].flatMap((s) => [...s]));

console.log(`official tag docs : ${allOfficial.size} tags in ${official.size} categories`);
console.log(`artemis-compat    : ${compat.size} quoted identifiers in src/script/\n`);

const notHandled = [...allOfficial].filter((t) => !compat.has(t)).sort();
const handled = [...allOfficial].filter((t) => compat.has(t));

console.log(`handled by name   : ${handled.length}/${allOfficial.size}`);
console.log(`NOT handled       : ${notHandled.length}\n`);

for (const [category, tags] of official) {
  const missing = [...tags].filter((t) => !compat.has(t)).sort();
  const line = `  ${category.padEnd(13)} ${String(tags.size).padStart(3)} total, ${String(missing.length).padStart(3)} missing`;
  console.log(line);
  if (onlyMissing && missing.length) {
    console.log(`      ${missing.join(' ')}`);
  }
}

// Cross-category aliases would make the per-category numbers double-count, so
// report the deduplicated union — that is the number that matters for "how much
// of the official language is unimplemented".
if (!onlyMissing) {
  console.log('');
  console.log('MISSING (union, deduplicated)');
  for (let i = 0; i < notHandled.length; i += 12) {
    console.log('  ' + notHandled.slice(i, i + 12).join(' '));
  }
}

// A tag the docs do not mention but the reimplementation handles is worth
// seeing too: it usually means the docs are incomplete, not that the tag is wrong.
const extra = [...compat].filter((t) => !allOfficial.has(t) && /^[a-z_]+$/.test(t)).sort();
console.log('');
console.log(`handled but undocumented (${extra.length}) — docs gap, not a bug:`);
for (let i = 0; i < extra.length; i += 14) {
  console.log('  ' + extra.slice(i, i + 14).join(' '));
}
