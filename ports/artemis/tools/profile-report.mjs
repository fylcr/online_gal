#!/usr/bin/env node
/**
 * Which of the engine's functions does a real game actually reach?
 *
 * ## Why
 *
 * The transpiled module has ~5,600 functions with no names. Rewriting that into readable C++
 * is not a transcription exercise -- transcribing 5,600 machine-generated functions is not
 * worth doing, and it is not what makes the code readable. It is a *triage* problem: find the
 * few hundred that a real game runs, name those, and leave the rest generated. This turns a
 * measured profile into that list.
 *
 * ## Input
 *
 * The JSON `real-game-test.mjs --json-out` writes, when the engine was built with
 * `build-bridge-route.sh --profile`. The profile rides in that report under
 * `candidate.profile` / `control.profile`.
 *
 * ## Output
 *
 * Groups the reached functions by how hot they are, joins whatever names are known, and
 * writes a worklist. The purpose is to make the order obvious: the top of the hot list is
 * where a readable rewrite buys the most, and the long tail at count 1 is where it buys the
 * least.
 *
 *     node profile-report.mjs report.json [--map profile-map.tsv] [--out worklist.md]
 *                                            [--tags tag-map.json] [--top 60]
 *
 * `--tags` joins the tag map, which carries `tag_owner` (function -> engine subsystem, e.g.
 * `reset`, `anime`, `file`) and is the closest thing to real symbol names this module has.
 */

import { existsSync, readFileSync, writeFileSync } from 'node:fs';

function parseArgs(argv) {
  const out = { top: 60, map: null, tags: null, out: null, json: false };
  for (let i = 0; i < argv.length; i += 1) {
    const a = argv[i];
    const next = () => argv[++i];
    if (a === '--map') out.map = next();
    else if (a === '--tags') out.tags = next();
    else if (a === '--out') out.out = next();
    else if (a === '--top') out.top = Number(next());
    else if (a === '--json') out.json = true;
    else if (a === '-h' || a === '--help') out.help = true;
    else if (!out.report) out.report = a;
    else throw new Error(`unknown argument: ${a}`);
  }
  return out;
}

/** `# func_index<TAB>name`, blank names left blank. */
/**
 * `# func_index<TAB>name`, blank names left blank.
 *
 * Returns both the names and how many rows the table has. The row count alone is the number
 * of functions the instrumenter touched, which is what a coverage denominator needs -- the
 * name count is not, because most functions have no name yet. That is the premise of the
 * worklist, so using the name count would report ~2% coverage for a complete run.
 */
function loadMap(path) {
  if (!path || !existsSync(path)) return { names: new Map(), rows: 0 };
  const names = new Map();
  let rows = 0;
  for (const line of readFileSync(path, 'utf8').split('\n')) {
    if (!line || line.startsWith('#')) continue;
    const [index, name] = line.split('\t');
    if (!index || !/^[0-9]+$/.test(index)) continue;
    rows += 1;
    if (name && name.trim()) names.set(Number(index), name.trim());
  }
  return { names, rows };
}

/**
 * The tag map's `tag_owner` maps `w2c_artemis_fNNNN` to the subsystems that reference the
 * function, which is the nearest thing to a symbol this module has.
 */
function loadTags(path) {
  if (!path || !existsSync(path)) return new Map();
  const raw = JSON.parse(readFileSync(path, 'utf8'));
  const tags = new Map();
  for (const [symbol, owners] of Object.entries(raw.tag_owner || {})) {
    const match = /^w2c_artemis_f(\d+)$/.exec(symbol);
    if (match) tags.set(Number(match[1]), owners);
  }
  return tags;
}

const KM = (n) => (n >= 1e6 ? `${(n / 1e6).toFixed(1)}M` : n >= 1e3 ? `${(n / 1e3).toFixed(1)}K` : String(n));

function main() {
  const args = parseArgs(process.argv.slice(2));
  if (args.help || !args.report) {
    console.log('usage: node profile-report.mjs <report.json> [--map FILE] [--tags FILE] [--out FILE] [--top N]');
    process.exit(args.help ? 0 : 2);
  }

  const report = JSON.parse(readFileSync(args.report, 'utf8'));
  const side = report.candidate && report.candidate.profile ? report.candidate
    : report.control && report.control.profile ? report.control
      : null;
  if (!side) {
    console.error('no profile in this report. Build the engine with --profile:');
    console.error('  ARTC_PROFILE=1 bash ports/artemis/reverse/build-bridge-route.sh asyncify');
    console.error('then deploy it with --engine-wasm and run real-game-test.mjs --json-out.');
    process.exit(2);
  }

  // Prefer the map the build wrote next to the module, since that is the one that matches this
  // exact binary; `--map` is for pointing at a different table (another flavour, or a later
  // renaming pass).
  let table = args.map ? loadMap(args.map) : { names: new Map(), rows: 0 };
  if (!args.map) {
    // The build writes the map beside the module it instrumented. Prefer the profiled
    // directory; the shipping one has no map because it carries no counters.
    for (const guess of [
      'ports/artemis/reverse/out/bridge-asyncify-profile/profile-map.tsv',
      'ports/artemis/reverse/out/bridge-asyncify/profile-map.tsv',
    ]) {
      if (existsSync(guess)) { table = loadMap(guess); break; }
    }
  }
  const map = table.names;
  const tags = loadTags(args.tags);
  const hits = side.profile.hits;
  if (!Array.isArray(hits)) {
    console.error(`profile error: ${JSON.stringify(side.profile)}`);
    process.exit(2);
  }

  const named = (index) => map.get(index) || '';
  const tagged = (index) => (tags.get(index) || []).join(', ');

  // Sorted by call count: the hot list is where a readable rewrite pays.
  const hot = [...hits].sort((a, b) => b[1] - a[1]);
  const touched = hot.length;

  // Buckets, because "how many functions are hot" is the decision this report exists to
  // support: a rewrite plan for 80 functions is a different project from one for 3,000.
  const buckets = [
    ['>= 1e6', (n) => n >= 1e6],
    ['1e5 .. 1e6', (n) => n >= 1e5 && n < 1e6],
    ['1e4 .. 1e5', (n) => n >= 1e4 && n < 1e5],
    ['1e3 .. 1e4', (n) => n >= 1e3 && n < 1e4],
    ['100 .. 1e3', (n) => n >= 100 && n < 1e3],
    ['10 .. 100', (n) => n >= 10 && n < 100],
    ['2 .. 10', (n) => n >= 2 && n < 10],
    ['1', (n) => n === 1],
  ];
  const bucketCounts = buckets.map(([label, test]) => [label, hits.filter((h) => test(h[1])).length]);

  const lines = [];
  // The denominator is the number of functions the instrumenter actually touched, not the
  // array capacity: the array is sized for the largest module the tool supports (8192) while
  // this module has 5,579 functions, so using the capacity would understate coverage by a
  // third and make two flavours look incomparable.
  const instrumented = table.rows || side.profile.capacity;
  lines.push('# Reached functions, by call count');
  lines.push('');
  lines.push(`Of ${instrumented} instrumented functions, **${touched}** were entered`);
  lines.push(`during this run (${((touched / instrumented) * 100).toFixed(1)}%).`);
  lines.push('');
  lines.push('This is the triage list for a readable rewrite: the top of the table is where');
  lines.push('naming and restructuring pays, the count-1 tail is where it does not.');
  lines.push('');
  lines.push('| call count | functions |');
  lines.push('| --- | --- |');
  for (const [label, n] of bucketCounts) lines.push(`| ${label} | ${n} |`);
  lines.push('');
  lines.push(`## Hottest ${Math.min(args.top, hot.length)}`);
  lines.push('');
  lines.push('| # | index | calls | first-run order | name | subsystems |');
  lines.push('| --- | --- | --- | --- | --- | --- |');
  for (const [i, [index, count, order]] of hot.slice(0, args.top).entries()) {
    lines.push(`| ${i + 1} | ${index} | ${KM(count)} | ${order} | ${named(index) || '—'} | ${tagged(index) || '—'} |`);
  }
  lines.push('');
  const namedHits = hot.filter(([i]) => named(i));
  const taggedHits = hot.filter(([i]) => tagged(i));
  lines.push('## Naming coverage');
  lines.push('');
  lines.push(`- functions with a name in the map: **${namedHits.length} / ${touched}**`);
  lines.push(`- functions with a subsystem tag: **${taggedHits.length} / ${touched}**`);
  lines.push('');
  if (taggedHits.length) {
    lines.push('### Named or tagged, by call count');
    lines.push('');
    lines.push('| index | calls | name | subsystems |');
    lines.push('| --- | --- | --- | --- |');
    for (const [index, count, order] of taggedHits.slice(0, 80)) {
      lines.push(`| ${index} | ${KM(count)} | ${named(index) || '—'} | ${tagged(index)} |`);
    }
    lines.push('');
  }

  const text = lines.join('\n');
  if (args.out) {
    writeFileSync(args.out, text);
    console.log(`wrote ${args.out}`);
  } else {
    console.log(text);
  }
  if (args.json) {
    console.log(JSON.stringify({ touched, instrumented, capacity: side.profile.capacity, buckets: bucketCounts,
      hot: hot.slice(0, args.top).map(([index, count, order]) => ({ index, count, order, name: named(index), tags: tags.get(index) || [] })) }, null, 2));
  }
}

main();
