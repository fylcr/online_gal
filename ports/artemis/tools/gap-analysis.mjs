#!/usr/bin/env node
/**
 * Compare the official Artemis Engine's class/method inventory against the
 * third-party reimplementation, subsystem by subsystem.
 *
 * ## Why this exists
 *
 * "Write a high-performance emulation version" needs a target. The official
 * engine ships as binaries only, so its API surface has to be recovered from
 * the Android `.so` (13,058 dynamic symbols, 3,423 of them `artemis::`) and the
 * RTTI names left in the stripped `artemis.wasm`. `artemis-compat` is the
 * working browser implementation. The useful question is not "how many classes
 * each has" but **which subsystems of the official engine the reimplementation
 * does not model at all**, because those are the ones where its behaviour
 * silently diverges — and divergence in a visual novel shows up as a misplaced
 * click or a wrong frame, not as a crash.
 *
 * Class names do not match by design (`artc` calls its stage a `Compositor`,
 * the official calls it `CStage`), so matching is done on **subsystem keyword**
 * rather than on name. A missed match is a lead to check by hand, not a verdict.
 *
 * ## Usage
 *
 *   node gap-analysis.mjs --oracle .test/artemis-oracle/symbols.json
 *   node gap-analysis.mjs --oracle ... --subsystem render
 */
import { readFileSync, readdirSync, existsSync } from 'node:fs';
import { join, resolve } from 'node:path';

const REPO = resolve(import.meta.dirname, '..', '..', '..');

/**
 * Subsystems worth comparing separately. Each maps to phrases that appear in
 * *either* codebase's identifiers, so a keyword hit means "this subsystem is
 * modelled", independent of what the classes are called.
 */
const SUBSYSTEMS = {
  'pack/format': ['PackFile', 'pf8', 'pfs', 'Archive', 'Pack'],
  'script/VM': ['ScriptParser', 'ScriptBlock', 'Lua', 'Command', 'Expression', 'Variable'],
  'render/layer': ['Layer', 'Compositor', 'Stage', 'DisplayObject', 'Draw'],
  'render/glyph': ['Font', 'Glyph', 'Text', 'Atlas'],
  'render/gpu': ['Gpu', 'Shader', 'Program', 'Surface', 'Texture', 'Renderer'],
  'render/image': ['Image', 'Psd', 'Png', 'Jpeg', 'Loaders', 'Block'],
  'audio': ['Sound', 'Audio', 'Vorbis', 'Wave', 'Pcm', 'Mixer', 'Track'],
  'video': ['Theora', 'Video'],
  'input': ['Input', 'Touch', 'Mouse', 'Key', 'MultiTouch'],
  'save/serialise': ['Serializer', 'Save', 'Snapshot'],
  'platform/host': ['Platform', 'Activity', 'Android', 'Web', 'Jni', 'Http', 'Socket', 'File'],
};

function loadOracle(path) {
  if (!existsSync(path)) {
    throw new Error(
      `${path} not found. Run: node ports/artemis/tools/oracle-symbols.mjs --out ${path}`,
    );
  }
  return JSON.parse(readFileSync(path, 'utf8'));
}

/** Every identifier from the reimplementation, by subsystem keyword. */
function compatIdentifiers() {
  const root = join(REPO, '.ref', 'artemis-compat', 'src');
  const tokens = new Set();
  if (!existsSync(root)) return { tokens, files: 0, lines: 0 };
  let files = 0;
  let lines = 0;
  const walk = (dir) => {
    for (const dirent of readdirSync(dir, { withFileTypes: true })) {
      const full = join(dir, dirent.name);
      if (dirent.isDirectory()) {
        walk(full);
        continue;
      }
      if (!/\.(cpp|h|hpp)$/.test(dirent.name)) continue;
      files++;
      const text = readFileSync(full, 'utf8');
      lines += text.split('\n').length;
      // Identifiers, plus the file name itself: artc names features after the
      // format or subsystem (`pf8_reader.cpp`) rather than after the class.
      for (const m of text.matchAll(/[A-Za-z_][A-Za-z0-9_]{2,}/g)) tokens.add(m[0]);
      tokens.add(dirent.name.replace(/\.[^.]+$/, ''));
    }
  };
  walk(root);
  return { tokens, files, lines };
}

function hits(tokens, keywords) {
  const found = new Set();
  for (const keyword of keywords) {
    for (const token of tokens) {
      if (token.toLowerCase().includes(keyword.toLowerCase())) found.add(token);
    }
  }
  return found;
}

const args = process.argv.slice(2);
const oraclePath = args.includes('--oracle')
  ? resolve(args[args.indexOf('--oracle') + 1])
  : resolve(REPO, '.test', 'artemis-oracle', 'symbols.json');
const only = args.includes('--subsystem') ? args[args.indexOf('--subsystem') + 1] : null;

const oracle = loadOracle(oraclePath);
const officialClasses = Object.keys(oracle.officialClasses);
const officialMethods = Object.values(oracle.officialClasses).flat();
const compat = compatIdentifiers();

console.log(`oracle: ${oraclePath}`);
console.log(`  official classes ${officialClasses.length}, methods ${officialMethods.length}`);
console.log(`  artemis-compat ${compat.files} files, ${compat.lines.toLocaleString()} lines\n`);

const rows = [];
for (const [name, keywords] of Object.entries(SUBSYSTEMS)) {
  if (only && !name.includes(only)) continue;
  const officialHit = hits(new Set(officialClasses.concat(officialMethods)), keywords);
  const compatHit = hits(compat.tokens, keywords);
  // Classes, not raw identifier counts, are the comparable unit: the official
  // side has one big flattened list, the compat side has every local variable.
  const officialNamed = officialClasses.filter((c) => hits(new Set([c]), keywords).size > 0);
  rows.push({
    subsystem: name,
    officialClasses: officialNamed.length,
    officialClassNames: officialNamed,
    officialKeywords: officialHit.size,
    compatKeywords: compatHit.size,
  });
}

rows.sort((a, b) => a.compatKeywords / (a.officialKeywords || 1) - b.compatKeywords / (b.officialKeywords || 1));

console.log('SUBSYSTEM COVERAGE (official class count vs compat keyword breadth)');
console.log(`  ${'subsystem'.padEnd(17)} ${'official'.padStart(8)} ${'off.kw'.padStart(7)} ${'artc.kw'.padStart(8)}`);
for (const row of rows) {
  console.log(
    `  ${row.subsystem.padEnd(17)} ${String(row.officialClasses).padStart(8)} ` +
      `${String(row.officialKeywords).padStart(7)} ${String(row.compatKeywords).padStart(8)}`,
  );
}

console.log('');
console.log('OFFICIAL CLASSES PER SUBSYSTEM');
for (const row of rows) {
  if (row.officialClasses === 0) continue;
  console.log(`  ${row.subsystem}:`);
  console.log(`    ${row.officialClassNames.join(', ')}`);
}

// The wasm's own RTTI list is the ground truth for "what the browser build
// contains", which can be narrower than the Android build.
if (oracle.wasmRttiClasses?.length) {
  console.log('');
  console.log(`WASM RTTI CLASSES (${oracle.wasmRttiClasses.length}) — present in the browser build`);
  const short = oracle.wasmRttiClasses.map((c) => c.replace(/^artemis::/, ''));
  console.log('  ' + short.join(', '));
}
