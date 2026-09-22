#!/usr/bin/env node
/**
 * The official engine's tag surface, read from the binary itself.
 *
 * ## Why this replaces the documentation-based list
 *
 * `tag-coverage.mjs` compared artc's dispatch table against `Docs/tag/*.txt`.
 * That was the best oracle available before the binaries were reversed, but it
 * is wrong in both directions:
 *
 *   - It **overstates** the surface. `Docs/tag/` holds 187 files, but 24 of
 *     those names never appear in the engine at all, and several are one doc
 *     spread over a family. The real dispatch surface is 101 names.
 *   - It is **blind to renames**. The docs file `font_close.txt` is registered
 *     in the binary as the literal `"/font"`, so a doc-based diff reports a
 *     missing tag where the engine actually has one under another spelling.
 *
 * The authority is `artemis::CArtemis::Command<Name>`, one non-virtual method
 * per tag, exported in `libartemis.so`'s dynamic symbol table. The method name
 * *is* the tag name (lowercased), with a handful of documented exceptions.
 *
 * ## Where the tag name actually comes from
 *
 * `CArtemis::CommandTag` is the single entry point every `e:tag{...}` reaches;
 * it switches on the tag name onto these `Command*` methods. So the symbol set
 * is not an approximation of the surface — it is the surface.
 *
 * Three facts only the binary shows, and each changes a conclusion:
 *
 *   1. **Closing tags are registered with a leading slash.** `CommandFontClose`
 *      is reached as `"/font"`, not `"font_close"`. Same for `/ruby`, `/link`,
 *      `/chgmsg`, `/tweenset`, `/voice`.
 *   2. **`seton*`/`delon*` are not individual methods.** There is exactly one
 *      `CommandSeton` and one `CommandDelon`, which switch on the suffix. So a
 *      missing `setonX` in artc's explicit table is not automatically a gap —
 *      artc's own prefix registry may cover it. This tool therefore reports the
 *      prefix families separately instead of flooding the gap list.
 *   3. **Method size measures real behaviour.** `CommandTrophy`, `CommandIme`,
 *      `CommandLua` and `CommandVibrate` are 4 bytes — they are empty stubs, so
 *      "implementing" them means doing nothing, deliberately.
 *
 * ## Usage
 *
 *   node tag-oracle.mjs                 # surface + artc gap
 *   node tag-oracle.mjs --json out.json # machine-readable
 *   node tag-oracle.mjs --sizes         # per-command size, stubs marked
 */
import { readFileSync, writeFileSync, existsSync } from 'node:fs';
import { join, resolve } from 'node:path';

const REPO = resolve(import.meta.dirname, '..', '..', '..');
const SO = join(REPO, '.ref', 'artemis', 'Android', 'app', 'src', 'main',
                'jniLibs', 'arm64-v8a', 'libartemis.so');
const DISPATCH = join(REPO, '.ref', 'artemis-compat', 'src', 'script',
                      'lua_tag_dispatch.cpp');

/**
 * Methods whose tag name is not `lowercase(methodName - "Command")`.
 * Every entry is load-bearing: without it the diff invents gaps.
 */
const RENAMES = {
  CommandAtMark: '@',            // the pre-'@' click-wait spelling lives in CommandAtMark
  CommandFontClose: '/font',
  CommandRubyClose: '/ruby',
  CommandLinkClose: '/link',
  CommandChgmsgClose: '/chgmsg',
  CommandTweensetClose: '/tweenset',
  CommandVoiceClose: '/voice',
  CommandHttp: 'httpget',        // 'http' would miss the real name
  CommandTag: 'tag',             // dispatcher itself; still a legal tag name
};

/** Families handled by one method that switches on the suffix. */
const PREFIX_FAMILIES = new Set(['CommandSeton', 'CommandDelon']);

/** An empty handler: nothing to port, and implementing it means doing nothing. */
const STUB_SIZE = 8;

/** Read the ELF64 dynamic symbol table. Returns [{name, value, size}]. */
function readDynsym(buf) {
  const shoff = buf.readBigUInt64LE(0x28);
  const shentsize = buf.readUInt16LE(0x3a);
  const shnum = buf.readUInt16LE(0x3c);
  const shstrndx = buf.readUInt16LE(0x3e);

  const sections = [];
  for (let i = 0; i < shnum; i += 1) {
    const o = Number(shoff) + i * shentsize;
    sections.push({
      nameOff: buf.readUInt32LE(o),
      type: buf.readUInt32LE(o + 4),
      addr: buf.readBigUInt64LE(o + 16),
      off: buf.readBigUInt64LE(o + 24),
      size: buf.readBigUInt64LE(o + 32),
      link: buf.readUInt32LE(o + 40),
      entsize: buf.readBigUInt64LE(o + 56),
    });
  }
  const strOff = Number(sections[shstrndx].off);
  const shname = (n) => buf.toString('utf8', strOff + n, buf.indexOf(0, strOff + n));

  const syms = [];
  for (const sec of sections) {
    if (shname(sec.nameOff) !== '.dynsym') continue;
    const strBase = Number(sections[sec.link].off);
    const count = Number(sec.size) / (Number(sec.entsize) || 24);
    for (let i = 0; i < count; i += 1) {
      const o = Number(sec.off) + i * Number(sec.entsize);
      const nameOff = buf.readUInt32LE(o);
      if (nameOff === 0) continue;
      syms.push({
        name: buf.toString('latin1', strBase + nameOff, buf.indexOf(0, strBase + nameOff)),
        value: Number(buf.readBigUInt64LE(o + 8)),
        size: Number(buf.readBigUInt64LE(o + 16)),
      });
    }
  }
  return syms;
}

/** Map every `CArtemis::Command*` symbol to a tag name. */
function officialTags(syms) {
  const out = new Map();
  const prefixes = new Set();
  for (const s of syms) {
    // _ZN7artemis8CArtemis<N>Command<Name>ERNS_12CScriptBlockEb
    const m = /^_ZN7artemis8CArtemis\d+(Command[A-Za-z0-9]+)ERNS_12CScriptBlockEb$/.exec(s.name);
    if (!m) continue;
    const fn = m[1];
    if (PREFIX_FAMILIES.has(fn)) { prefixes.add(fn); continue; }
    const tag = RENAMES[fn] ?? fn.slice('Command'.length).toLowerCase();
    // Keep the smallest address if the linker folded duplicates.
    const prev = out.get(tag);
    if (!prev || s.value < prev.addr) out.set(tag, { addr: s.value, size: s.size, fn });
  }
  return { tags: out, prefixes };
}

/**
 * Tag names artc registers explicitly, plus whether it has a prefix registry.
 *
 * The registry lives in a different translation unit from the table
 * (`lua_tags_input.cpp`, `TagGenericSetOn`), so both files are read. Checking
 * only the dispatch table reported `prefix registry: NO` and would have
 * overstated the gap by the whole seton/delon family.
 */
function artcSurface() {
  if (!existsSync(DISPATCH)) return null;
  const src = readFileSync(DISPATCH, 'utf8');
  const input = join(REPO, '.ref', 'artemis-compat', 'src', 'script', 'lua_tags_input.cpp');
  const inputSrc = existsSync(input) ? readFileSync(input, 'utf8') : '';
  const names = new Set([...src.matchAll(/\{"([^"]+)",\s*&LuaEngine::/g)].map((m) => m[1]));
  return {
    names,
    prefixRegistry: /rfind\("seton",\s*0\)/.test(inputSrc) && /rfind\("delon",\s*0\)/.test(inputSrc),
  };
}

const argv = process.argv.slice(2);
const asJson = argv.includes('--json') ? argv[argv.indexOf('--json') + 1] : null;
const showSizes = argv.includes('--sizes');

if (!existsSync(SO)) {
  console.error(`missing ${SO}`);
  console.error('the official SDK is not redistributable and is not committed;');
  console.error('place it under .ref/artemis/ to run this tool.');
  process.exit(1);
}

const { tags, prefixes } = officialTags(readDynsym(readFileSync(SO)));
const artc = artcSurface();

const officialNames = [...tags.keys()].sort();
const missing = artc ? officialNames.filter((t) => !artc.names.has(t)) : [];
const extra = artc ? [...artc.names].filter((t) => !tags.has(t)).sort() : [];

/**
 * True when artc's generic `seton*`/`delon*` registry handles this name even
 * though it is absent from the explicit table. Reporting these as gaps would
 * be wrong: the official engine handles them the same way, through one
 * `CommandSeton`/`CommandDelon` that switches on the suffix.
 */
const prefixCovered = (t) =>
  Boolean(artc?.prefixRegistry) && (t.startsWith('seton') || t.startsWith('delon'));

const missingReal = missing.filter((t) => !prefixCovered(t));

if (asJson) {
  writeFileSync(asJson, JSON.stringify({
    official: Object.fromEntries(officialNames.map((t) => [t, tags.get(t)])),
    prefixFamilies: [...prefixes].sort(),
    artc: artc ? [...artc.names].sort() : null,
    artcPrefixRegistry: artc?.prefixRegistry ?? null,
    missing,
    missingReal,
    extra,
  }, null, 1));
  console.log(`wrote ${asJson}`);
}

console.log(`official tag surface: ${officialNames.length} names`);
console.log(`  prefix families    : ${[...prefixes].sort().join(', ')}`);
if (artc) {
  console.log(`artc registered     : ${artc.names.size} names ` +
              `(prefix registry: ${artc.prefixRegistry ? 'yes' : 'NO'})`);
  console.log();
  console.log(`official has, artc LACKS (${missingReal.length}, ` +
              `${missing.length - missingReal.length} covered by the prefix registry):`);
  for (const t of missingReal) {
    const { addr, size, fn } = tags.get(t);
    const stub = size <= STUB_SIZE ? '  <- empty stub' : '';
    console.log(`  ${t.padEnd(20)} ${addr.toString(16).padStart(9, '0')} ` +
                `size=${size.toString(16).padStart(6, '0')}  ${fn}${stub}`);
  }
  const covered = missing.filter(prefixCovered);
  if (covered.length) {
    console.log(`  (prefix-covered: ${covered.join(', ')})`);
  }
  console.log();
  console.log(`artc has, official has no same-name Command (${extra.length}):`);
  console.log('  ' + extra.join(', '));
  console.log();
  console.log('note: an artc-only name is not automatically wrong — artc also');
  console.log('      implements engine behaviour the official Command* set does');
  console.log('      not name (host plumbing, tag aliases, test hooks).');
}

if (showSizes) {
  console.log();
  console.log('every Command*, smallest first (size measures real behaviour):');
  const rows = [...tags.entries()].sort((a, b) => a[1].size - b[1].size);
  for (const [t, { size, fn }] of rows) {
    console.log(`  ${size.toString(16).padStart(6, '0')}  ${t.padEnd(20)} ${fn}`);
  }
}
