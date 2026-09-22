#!/usr/bin/env node
/**
 * Which tags must the reimplementation dispatch, and which does it dispatch?
 *
 * ## Two vocabularies, and only one of them is ours
 *
 * A shipped Artemis game has two tag-ish layers and they are easy to confuse:
 *
 *   1. The `.ast` scenario format — `{"fg", file="...", ch="..."}` blocks. Those
 *      names (`fg`, `vo`, `text`, `quake`, ...) are the GAME's own scripting
 *      vocabulary. The game's Lua parses `.ast` and decides what to do; the
 *      engine never sees them. Measuring against these yields a scary and
 *      meaningless "32 of 37 tags missing".
 *
 *   2. Engine tags — what the game's Lua hands to the engine via `e:tag{...}`.
 *      These (`lyprop`, `lydel`, `chgmsg`, `trans`, `print`, ...) are the real
 *      interface, and they are what `artc` dispatches in
 *      `.ref/artemis-compat/src/script/lua_tag_dispatch.cpp`.
 *
 * Only (2) matters for the port. Measured on the real title, (2) is 59 distinct
 * tags over 478 calls — not the 186 of the documentation. Both layers are
 * reported below so the distinction stays visible.
 *
 * ## Usage
 *
 *   node tag-usage-gap.mjs --lua  .test/artemis-lua.txt   # all game Lua, concatenated
 *   node tag-usage-gap.mjs --ast  .test/artemis-tags.txt  # optional: the .ast layer
 *   node tag-usage-gap.mjs --lua ... --probe              # + check the official binary
 *
 * Produce `--lua` input with:
 *   node ports/artemis/tools/pfs-grep.mjs --filter '\.lua$' --dump <root.pfs> > .test/artemis-lua.txt
 */
import { readFileSync, readdirSync, existsSync } from 'node:fs';
import { join, resolve } from 'node:path';

const REPO = resolve(import.meta.dirname, '..', '..', '..');
const A = '.ref/artemis-compat/src/script';
const args = process.argv.slice(2);
const opt = (name) => (args.includes(name) ? resolve(args[args.indexOf(name) + 1]) : null);
const luaPath = opt('--lua');
const astPath = opt('--ast');
const probe = args.includes('--probe');

if (!luaPath && !astPath) {
  console.error('usage: tag-usage-gap.mjs --lua <game-lua-dump> [--ast <ast-tag-counts>] [--probe]');
  process.exit(1);
}

/** Names the reimplementation mentions in its script layer. */
function compatNames() {
  const dir = join(REPO, A);
  const names = new Set();
  if (!existsSync(dir)) return names;
  for (const f of readdirSync(dir)) {
    if (!/\.(cpp|h)$/.test(f)) continue;
    const src = readFileSync(join(dir, f), 'utf8');
    for (const m of src.matchAll(/"([a-z_][a-z0-9_]{1,24})"/g)) names.add(m[1]);
  }
  return names;
}

/**
 * A tag is dispatched if it is named exactly, or if it falls in the generic
 * `seton*` / `delon*` prefix registry (`TagGenericSetOn`), which stores the
 * suffix in `named_events_`. Reporting a prefix hit as "missing" would be
 * wrong, so the two are distinguished.
 */
function classify(tag, names) {
  if (names.has(tag)) return 'exact';
  if (/^(set|del)on[a-z]+$/.test(tag)) {
    const kind = tag.slice(3);
    if (!['push', 'soundfinish', 'videofinish', 'automodein', 'automodeout'].includes(kind)) {
      return 'prefix';
    }
    return names.has(tag) ? 'exact' : 'missing';
  }
  return 'missing';
}

const compat = compatNames();

function report(title, counts, note) {
  const rows = [...counts].sort((a, b) => b[1] - a[1]);
  const total = rows.reduce((s, r) => s + r[1], 0);
  const by = { exact: [], prefix: [], missing: [] };
  for (const [t, c] of rows) by[classify(t, compat)].push([t, c]);
  const sum = (xs) => xs.reduce((s, r) => s + r[1], 0);

  console.log(`\n${'='.repeat(72)}\n${title}\n${'='.repeat(72)}`);
  if (note) console.log(note);
  console.log(`\n${rows.length} distinct / ${total} calls`);
  console.log(`  exact handler      : ${by.exact.length} distinct / ${sum(by.exact)} calls`);
  console.log(`  seton*/delon* prefix: ${by.prefix.length} distinct / ${sum(by.prefix)} calls`);
  console.log(`  UNHANDLED          : ${by.missing.length} distinct / ${sum(by.missing)} calls`);
  if (by.missing.length) {
    console.log('\nunhandled, by call count:');
    for (const [t, c] of by.missing.sort((a, b) => b[1] - a[1])) console.log(`  ${String(c).padStart(5)}  ${t}`);
  }
  if (by.prefix.length) {
    console.log('\nhandled by the generic prefix registry:');
    console.log('  ' + by.prefix.map(([t]) => t).join(', '));
  }
  return by;
}

// ---- layer 2: engine tags (the interface we must implement) ----------------
let engineMissing = [];
if (luaPath) {
  const lua = readFileSync(luaPath, 'utf8');
  const counts = new Map();
  for (const m of lua.matchAll(/e:tag\{\s*"([a-zA-Z_]+)"/g)) {
    counts.set(m[1], (counts.get(m[1]) || 0) + 1);
  }
  const by = report(
    'ENGINE TAGS — sent by the real game\'s Lua via e:tag{}',
    counts,
    'This is the engine interface. What is unhandled here is what the port owes.',
  );
  engineMissing = by.missing.map(([t]) => t);

  if (probe) {
    // The official binary is the authority on whether a tag exists at all: the
    // command table sits in .rodata as NUL-terminated names. Scan the raw bytes
    // rather than shelling out, so this works on any platform.
    const so = join(REPO, '.ref', 'artemis', 'Android', 'app', 'src', 'main', 'jniLibs', 'arm64-v8a', 'libartemis.so');
    if (existsSync(so)) {
      const buf = readFileSync(so);
      const text = buf.toString('latin1');
      const present = (t) => text.includes(`\0${t}\0`) || text.includes(`\0${t}\u0000`);
      console.log('\n--- is each unhandled tag present in the official binary? ---');
      for (const t of engineMissing) {
        console.log(`  ${present(t) ? 'yes' : 'NO '}  ${t}`);
      }
      console.log('\nA "NO" means the official engine has no such tag either, so the game');
      console.log('reaches it only through a path the official build also ignores.');
    } else {
      console.log(`\n(probe skipped: ${so} not found)`);
    }
  }
}

// ---- layer 1: .ast tags (informational — NOT the engine interface) ---------
if (astPath) {
  const counts = new Map();
  for (const line of readFileSync(astPath, 'utf8').split('\n')) {
    const m = /^\s*(\d+)\s+([a-z_][a-z0-9_]*)\s*$/.exec(line);
    if (m) counts.set(m[2], Number(m[1]));
  }
  if (counts.size) {
    report(
      'AST TAGS — the game\'s own scenario vocabulary (informational only)',
      counts,
      'These are parsed by the GAME\'s Lua, not by the engine. A gap here is not\n' +
      'an engine gap: the game implements them in its own system/*.lua modules.\n' +
      'The "exact handler" count below is NOT meaningful — a short .ast name like\n' +
      '"text" or "bg" collides with an unrelated string literal in the compat\n' +
      'source by coincidence. Read the unhandled list as "the game\'s own job".',
    );
  }
}

console.log('');
