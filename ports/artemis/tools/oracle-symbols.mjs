#!/usr/bin/env node
/**
 * Build an authoritative inventory of the official Artemis Engine's symbols and
 * compare it against the third-party reimplementation in `.ref/artemis-compat`.
 *
 * ## Why this exists
 *
 * `.ref/artemis/` ships the official engine **only as binaries** — the Windows
 * `.exe`, the Android `.so` files, and two `artemis.wasm` builds — with no
 * source. The third-party reimplementation is what actually runs in a browser,
 * so "what does the official engine have that artemis-compat lacks" is the
 * question that decides where porting effort goes.
 *
 * Two sources answer it, and they have different strengths:
 *
 * - **`libartemis.so` (arm64-v8a)**: 13,058 dynamic symbols, of which 3,423 are
 *   `artemis::`. This is a complete, named inventory of the engine's classes and
 *   methods — the authoritative oracle. It is a *dynamic* symbol table, so it
 *   covers what the Android host can reach; a few file-local helpers are absent.
 *
 * - **`artemis.wasm`**: stripped, no `name` section and no DWARF, so it cannot
 *   give function names. But C++ RTTI typeinfo names survive in the data
 *   section (`N7artemis10CTextLayerE` style), which confirms which classes the
 *   wasm build actually contains and pins the class *hierarchy*.
 *
 * Symbols are demangled with `llvm-cxxfilt` from the emsdk rather than a JS
 * demangler, because HandBrake's Itanium mangling includes template arguments
 * and nested classes that a small demangler gets wrong, and a wrong class name
 * silently looks like a missing feature.
 *
 * ## Usage
 *
 *   node oracle-symbols.mjs --out .test/artemis-oracle/symbols.json
 *   node oracle-symbols.mjs --report        # human-readable coverage summary
 */
import { execFileSync } from 'node:child_process';
import { mkdir, writeFile } from 'node:fs/promises';
import { existsSync, readFileSync, readdirSync } from 'node:fs';
import { dirname, join, resolve } from 'node:path';

const REPO = resolve(import.meta.dirname, '..', '..', '..');
const ARTEMIS = join(REPO, '.ref', 'artemis');

/** Find a usable `llvm-nm`/`llvm-cxxfilt` pair, preferring the emsdk's own. */
function findLlvmTool(name) {
  const candidates = [
    process.env.EMSDK_DIR && join(process.env.EMSDK_DIR, 'upstream', 'bin', `${name}.exe`),
    'D:/emsdk/upstream/bin/' + name + '.exe',
    '/d/emsdk/upstream/bin/' + name + '.exe',
  ].filter(Boolean);
  for (const candidate of candidates) if (existsSync(candidate)) return candidate;
  try {
    return execFileSync('command', ['-v', name], { shell: true, encoding: 'utf8' }).trim();
  } catch {
    return null;
  }
}

const NM = findLlvmTool('llvm-nm');
const CXXFILT = findLlvmTool('llvm-cxxfilt');
if (!NM || !CXXFILT) {
  throw new Error('llvm-nm / llvm-cxxfilt not found; set EMSDK_DIR or put them on PATH');
}

function run(tool, args, input) {
  return execFileSync(tool, args, {
    encoding: 'utf8',
    maxBuffer: 256 * 1024 * 1024,
    input,
  });
}

/**
 * Pull the defined dynamic symbols out of the Android shared libraries.
 *
 * `--dynamic --defined-only` is the right filter: `--defined-only` with no
 * `--dynamic` reports the (stripped) static table, which for these files is a
 * single line, and an unqualified `llvm-nm` prints every undefined import too,
 * which would bury the engine's own API under libc and fontconfig.
 */
function androidSymbols() {
  const out = { symbols: [], byClass: {}, files: [] };
  const libs = [
    'Android/app/src/main/jniLibs/arm64-v8a/libartemis.so',
    'Android/app/src/main/jniLibs/armeabi-v7a/libartemis.so',
  ];
  for (const lib of libs) {
    const path = join(ARTEMIS, lib);
    if (!existsSync(path)) continue;
    const raw = run(NM, ['--dynamic', '--defined-only', path]);
    const demangled = run(CXXFILT, [], raw);
    const names = demangled
      .split('\n')
      .map((line) => line.trim())
      .filter(Boolean)
      .map((line) => line.replace(/^[0-9a-f]+\s+[A-Za-z]\s+/, ''));
    out.files.push({ path: lib, count: names.length });
    if (lib.includes('arm64')) out.symbols = names;
  }
  return out;
}

/**
 * The wasm keeps no function names, so the only structural evidence is RTTI.
 * `__ZTI`/`__ZTS` payloads hold mangled *type* names, which demangle to class
 * names and reveal the class hierarchy the linker kept.
 */
function wasmRtti() {
  const out = { classes: [], files: [] };
  for (const rel of ['WebAssembly/artemis.wasm', 'WebAssembly/asyncify/artemis.wasm']) {
    const path = join(ARTEMIS, rel);
    if (!existsSync(path)) continue;
    const buf = readFileSync(path);
    const text = buf.toString('latin1');
    const seen = new Set();
    // Mangled type names for a class look like `N7artemis10CTextLayerE`.
    const re = /N[0-9]+artemis[0-9A-Za-z_]+?E/g;
    for (const match of text.matchAll(re)) {
      const mangled = match[0];
      let demangled;
      try {
        demangled = run(CXXFILT, [mangled]).trim();
      } catch {
        continue;
      }
      if (demangled && !seen.has(demangled)) {
        seen.add(demangled);
        out.classes.push(demangled);
      }
    }
    out.files.push({ path: rel, rttiClasses: seen.size });
  }
  out.classes.sort();
  return out;
}

/** Group `artemis::Cls::Method(...)` into `Cls -> [Method, ...]`. */
function classify(symbols) {
  const byClass = {};
  for (const symbol of symbols) {
    const match = /(?:^|_)artemis::([A-Za-z_][A-Za-z0-9_]*)(?:::([A-Za-z_~][A-Za-z0-9_]*))?/.exec(symbol);
    if (!match) continue;
    const cls = match[1];
    const member = match[2] ?? '(typeinfo/vtable)';
    (byClass[cls] ??= new Set()).add(member);
  }
  const result = {};
  for (const [cls, members] of Object.entries(byClass)) {
    result[cls] = [...members].sort();
  }
  return result;
}

/**
 * The reimplementation's own vocabulary: every `artc::` symbol and every
 * class-like identifier, so a report can say "official has CBackLog, we have
 * Compositor" instead of just listing both.
 */
function compatVocabulary() {
  const srcDir = join(REPO, '.ref', 'artemis-compat', 'src');
  const out = { files: 0, lines: 0, namespaces: new Set(), classes: new Set() };
  if (!existsSync(srcDir)) return { files: 0, lines: 0, namespaces: [], classes: [] };
  const walk = (dir) => {
    for (const dirent of readdirSync(dir, { withFileTypes: true })) {
      const full = join(dir, dirent.name);
      if (dirent.isDirectory()) walk(full);
      else if (/\.(cpp|h|hpp)$/.test(dirent.name)) {
        out.files++;
        const text = readFileSync(full, 'utf8');
        out.lines += text.split('\n').length;
        for (const m of text.matchAll(/\bnamespace\s+([A-Za-z_][A-Za-z0-9_]*)/g)) out.namespaces.add(m[1]);
        for (const m of text.matchAll(/\b(?:class|struct)\s+([A-Z][A-Za-z0-9_]*)/g)) out.classes.add(m[1]);
      }
    }
  };
  walk(srcDir);
  return {
    files: out.files,
    lines: out.lines,
    namespaces: [...out.namespaces].sort(),
    classes: [...out.classes].sort(),
  };
}

const report = process.argv.includes('--report');
const outIdx = process.argv.indexOf('--out');
const outPath = outIdx >= 0 ? resolve(process.argv[outIdx + 1]) : null;

const android = androidSymbols();
const byClass = classify(android.symbols);
const wasm = wasmRtti();
const compat = compatVocabulary();

const inventory = {
  generatedFrom: {
    android: android.files,
    wasm: wasm.files,
    compat: { files: compat.files, lines: compat.lines },
  },
  counts: {
    androidSymbols: android.symbols.length,
    androidArtemisSymbols: android.symbols.filter((s) => s.includes('artemis::')).length,
    officialClasses: Object.keys(byClass).length,
    wasmRttiClasses: wasm.classes.length,
    compatClasses: compat.classes.length,
  },
  officialClasses: byClass,
  wasmRttiClasses: wasm.classes,
  compat: { namespaces: compat.namespaces, classes: compat.classes },
};

if (outPath) {
  await mkdir(dirname(outPath), { recursive: true });
  await writeFile(outPath, JSON.stringify(inventory, null, 2));
  console.log(`wrote ${outPath}`);
}

console.log('OFFICIAL ENGINE INVENTORY');
console.log(`  android symbols            ${inventory.counts.androidSymbols.toLocaleString()}`);
console.log(`  of which artemis::         ${inventory.counts.androidArtemisSymbols.toLocaleString()}`);
console.log(`  distinct artemis classes   ${inventory.counts.officialClasses.toLocaleString()}`);
console.log(`  wasm RTTI classes          ${inventory.counts.wasmRttiClasses.toLocaleString()}`);
console.log('');
console.log('THIRD-PARTY REIMPLEMENTATION (.ref/artemis-compat)');
console.log(`  files                      ${compat.files}`);
console.log(`  lines                      ${compat.lines.toLocaleString()}`);
console.log(`  classes                    ${compat.classes.length}`);
console.log(`  namespaces                 ${compat.namespaces.join(', ')}`);

if (report) {
  console.log('');
  console.log('OFFICIAL CLASSES BY MEMBER COUNT (top 30)');
  const ranked = Object.entries(byClass).sort((a, b) => b[1].length - a[1].length);
  for (const [cls, members] of ranked.slice(0, 30)) {
    console.log(`  ${cls.padEnd(22)} ${String(members.length).padStart(4)} members`);
  }
  console.log('');
  console.log('OFFICIAL CLASSES WITH NO LIKELY COUNTERPART IN artemis-compat');
  const compatUpper = new Set(compat.classes.map((c) => c.toUpperCase()));
  const missing = [];
  for (const cls of Object.keys(byClass)) {
    // artc uses its own names (Compositor for CStage, LayerSet for CLayerSet),
    // so this is a lead, not a verdict: strip the C prefix and compare loosely.
    const stem = cls.replace(/^[CI]/, '').toUpperCase();
    const hit = compatUpper.has(cls.toUpperCase()) || compatUpper.has(stem);
    if (!hit) missing.push(cls);
  }
  console.log(`  ${missing.length} of ${Object.keys(byClass).length} classes have no name match`);
  console.log('  ' + missing.slice(0, 60).join(', '));
}
