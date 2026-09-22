/**
 * Lists the ONS build's exported functions, focusing on the input surface.
 *
 * A prebuilt binary cannot have its source rewritten the way AetherKiri's build
 * does — it patches `ONScripter.cpp` at configure time to call a host-supplied
 * `aetherkiri_onscripter_wait_event(SDL_Event *)`. For a prebuilt binary the only
 * way in is through what it *exports*, so the export table is the thing to read.
 *
 * Emscripten exports every `EMSCRIPTEN_KEEPALIVE` and every undead-stripped C
 * symbol with a leading underscore, so scanning names for event-related terms
 * identifies the injectable entry points without guessing.
 */
import { readFileSync } from 'node:fs';

const wasm = readFileSync('.ref/onsyuri-web/onsyuri_web/onsyuri.wasm');

/** Reads the LEB128 length-prefixed strings out of a section. */
function readSection(bytes, id) {
  let at = 8;
  while (at < bytes.byteLength) {
    const sectionId = bytes[at];
    at += 1;
    let size = 0;
    let shift = 0;
    for (;;) {
      const byte = bytes[at];
      at += 1;
      size |= (byte & 0x7f) << shift;
      if ((byte & 0x80) === 0) break;
      shift += 7;
    }
    if (sectionId === id) return { start: at, size };
    at += size;
  }
  return null;
}

function readLeb(bytes, cursor) {
  let value = 0;
  let shift = 0;
  let at = cursor;
  for (;;) {
    const byte = bytes[at];
    at += 1;
    value |= (byte & 0x7f) << shift;
    if ((byte & 0x80) === 0) break;
    shift += 7;
  }
  return { value, at };
}

function readName(bytes, cursor) {
  const { value: length, at } = readLeb(bytes, cursor);
  return { name: Buffer.from(bytes.subarray(at, at + length)).toString('utf8'), at: at + length };
}

const section = readSection(wasm, 7); // export section
if (!section) {
  console.log('no export section found');
  process.exit(0);
}

let at = section.start;
const { value: count, at: afterCount } = readLeb(wasm, at);
at = afterCount;

const KIND = ['function', 'table', 'memory', 'global'];
const exports = [];
for (let index = 0; index < count; index += 1) {
  const name = readName(wasm, at);
  at = name.at;
  const kind = wasm[at];
  at += 1;
  const { at: afterIndex } = readLeb(wasm, at);
  at = afterIndex;
  exports.push({ name: name.name, kind: KIND[kind] ?? String(kind) });
}

console.log(`total exports: ${exports.length}`);
for (const entry of exports) console.log(`  ${entry.kind.padEnd(8)} ${entry.name}`);

const functions = exports.filter((entry) => entry.kind === 'function');
console.log(`\nfunctions: ${functions.length}\n`);

const groups = {
  'event / input': /event|Event|mouse|Mouse|key|Key|touch|Touch|button|Button/,
  SDL: /^_?SDL_/,
  'engine commands': /^_?(ONS|Ons|ons)/,
  'main / init': /main|init|Init|start|Start|run|Run|exec|Exec/,
};

for (const [label, pattern] of Object.entries(groups)) {
  const hits = functions.filter((entry) => pattern.test(entry.name));
  console.log(`=== ${label}: ${hits.length} ===`);
  for (const hit of hits.slice(0, 30)) console.log('  ' + hit.name);
  console.log();
}
