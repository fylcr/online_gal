#!/usr/bin/env node
/**
 * Inspect a WebAssembly module: its section table, and the `name` custom
 * section when the build kept one.
 *
 * ## Why this exists
 *
 * `.ref/artemis/` ships the official Artemis Engine only as prebuilt
 * `artemis.wasm` — there is no source. Its behaviour therefore has to be
 * recovered from the binary, and the largest cheap win is the `name` section:
 * a build that kept symbols tells us its own class and method names, and those
 * can be matched against the third-party C++ reimplementation
 * (`artemis-compat`, namespace `artc`) to find what it is missing or doing
 * differently.
 *
 * Whether symbols exist also decides whether `wasm2c` (wabt) is worth running
 * at all: without a `name` section its output is a wall of
 * `w2c_artemis_f12345`; with one it is navigable and greppable. This tool
 * answers "is reverse engineering worth starting" before anything is built or
 * downloaded.
 *
 * Usage:
 *   node wasm-sections.mjs <module.wasm> [--names] [--out <file>] [--limit <n>]
 */
import { readFileSync, writeFileSync } from 'node:fs';

const SECTION_KINDS = {
  0: 'custom', 1: 'type', 2: 'import', 3: 'function', 4: 'table', 5: 'memory',
  6: 'global', 7: 'export', 8: 'start', 9: 'element', 10: 'code', 11: 'data',
  12: 'datacount', 13: 'tag',
};

/** Unsigned LEB128 at `offset`; returns `[value, nextOffset]`. */
function leb(bytes, offset) {
  let value = 0;
  let shift = 0;
  for (;;) {
    const byte = bytes[offset++];
    value |= (byte & 0x7f) << shift;
    if ((byte & 0x80) === 0) return [value >>> 0, offset];
    shift += 7;
  }
}

function parseSections(bytes) {
  if (bytes.length < 8) throw new Error('file is too small to be a wasm module');
  const magicOk =
    bytes[0] === 0x00 && bytes[1] === 0x61 && bytes[2] === 0x73 && bytes[3] === 0x6d;
  if (!magicOk) throw new Error('bad magic: this is not a wasm module');
  const version = bytes.readUInt32LE(4);

  const sections = [];
  let offset = 8;
  while (offset < bytes.length) {
    const id = bytes[offset++];
    let size;
    [size, offset] = leb(bytes, offset);
    const payloadStart = offset;
    let customName = null;
    if (id === 0) {
      let nameLength;
      [nameLength, offset] = leb(bytes, offset);
      customName = bytes.subarray(offset, offset + nameLength).toString('utf8');
    }
    sections.push({
      id,
      kind: SECTION_KINDS[id] ?? `unknown(${id})`,
      customName,
      offset: payloadStart,
      size,
    });
    offset = payloadStart + size;
  }
  return { version, sections };
}

/** Walk the subsections of the `name` custom section. */
function nameSubsections(bytes, section) {
  let offset = section.offset;
  let nameLength;
  [nameLength, offset] = leb(bytes, offset);
  const sectionName = bytes.subarray(offset, offset + nameLength).toString('utf8');
  offset += nameLength;

  const end = section.offset + section.size;
  const subs = [];
  while (offset < end) {
    const subId = bytes[offset++];
    let subSize;
    [subSize, offset] = leb(bytes, offset);
    subs.push({ subId, start: offset, end: offset + subSize });
    offset += subSize;
  }
  return { sectionName, subs };
}

/**
 * Names come in several subsections; 1 (functions) is the one that matters for
 * reading a decompilation. Each is a count followed by `index, length, bytes`.
 */
function nameEntries(bytes, sub) {
  let offset = sub.start;
  let count;
  [count, offset] = leb(bytes, offset);
  const entries = [];
  for (let n = 0; n < count && offset < sub.end; n++) {
    let index;
    let length;
    [index, offset] = leb(bytes, offset);
    [length, offset] = leb(bytes, offset);
    entries.push({ index, name: bytes.subarray(offset, offset + length).toString('utf8') });
    offset += length;
  }
  return entries;
}

function parseArgs(argv) {
  const opts = { path: null, names: false, out: null, limit: 40 };
  for (let i = 0; i < argv.length; i++) {
    const arg = argv[i];
    if (arg === '--names') opts.names = true;
    else if (arg === '--out') opts.out = argv[++i];
    else if (arg === '--limit') opts.limit = Number(argv[++i]);
    else if (arg.startsWith('-')) throw new Error(`unknown option: ${arg}`);
    else opts.path = arg;
  }
  if (!opts.path) throw new Error('usage: wasm-sections.mjs <module.wasm> [--names] [--out <file>]');
  return opts;
}

const opts = parseArgs(process.argv.slice(2));
const bytes = readFileSync(opts.path);
const { version, sections } = parseSections(bytes);

console.log(`${opts.path}`);
console.log(`  version ${version}, ${bytes.length.toLocaleString()} bytes, ${sections.length} sections\n`);
console.log(`  ${'id'.padStart(3)}  ${'kind'.padEnd(9)} ${'custom name'.padEnd(18)} ${'size'.padStart(14)}  ${'share'.padStart(7)}`);
for (const section of sections) {
  const share = ((section.size / bytes.length) * 100).toFixed(1) + '%';
  console.log(
    `  ${String(section.id).padStart(3)}  ${section.kind.padEnd(9)} ` +
      `${(section.customName ?? '').padEnd(18)} ${section.size.toLocaleString().padStart(14)}  ${share.padStart(7)}`,
  );
}

const nameSection = sections.find((s) => s.id === 0 && s.customName === 'name');
const dwarf = sections.filter((s) => s.id === 0 && s.customName?.startsWith('.debug'));

console.log('');
if (!nameSection) {
  console.log('name section: ABSENT — a decompilation will have no symbol names.');
} else {
  const { subs } = nameSubsections(bytes, nameSection);
  console.log(`name section: present, subsections ${subs.map((s) => s.subId).join(',')}`);
  if (opts.names) {
    const labels = {
      0: 'module', 1: 'functions', 2: 'locals', 3: 'labels', 4: 'types',
      5: 'tables', 6: 'memories', 7: 'globals', 8: 'elem segments', 9: 'data segments',
    };
    for (const sub of subs) {
      const entries = nameEntries(bytes, sub);
      console.log(`  ${String(sub.subId).padStart(2)} ${(labels[sub.subId] ?? '?').padEnd(14)} ${entries.length} named`);
      if (sub.subId === 1) {
        for (const entry of entries.slice(0, opts.limit)) {
          console.log(`       ${String(entry.index).padStart(7)}  ${entry.name}`);
        }
        if (entries.length > opts.limit) console.log(`       ... ${entries.length - opts.limit} more`);
        if (opts.out) {
          const lines = entries.map((e) => `${e.index}\t${e.name}`);
          writeFileSync(opts.out, lines.join('\n'));
          console.log(`  wrote ${entries.length} function names to ${opts.out}`);
        }
      }
    }
  } else {
    console.log('  (pass --names to dump them)');
  }
}

console.log('');
console.log(
  dwarf.length
    ? `DWARF: ${dwarf.length} section(s) — ${dwarf.map((s) => s.customName).join(', ')}`
    : 'DWARF: absent (no types or line numbers in the module)',
);
