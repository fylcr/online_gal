// Ad-hoc verification of the @online-gal/vfs host byte-source seam module.
// Named *.mjs on purpose so it is NOT picked up by `node --test ".test/*.test.ts"`.
import {
  HOST_BYTE_SOURCE_KEY,
  createHostByteSource,
  defaultUrlKey,
  hostByteSourceFromGameSource,
  installHostByteSource,
} from '../../packages/vfs/dist/host-source.js';
import { FileListSource } from '../../packages/vfs/dist/sources.js';

let failures = 0;
const check = (name, condition, detail = '') => {
  if (condition) console.log(`  ok   ${name}`);
  else { failures += 1; console.log(`  FAIL ${name} ${detail}`); }
};

console.log('== defaultUrlKey ==');
const cases = [
  ['/data.xp3', 'data.xp3'],
  ['data.xp3?v=2#x', 'data.xp3'],
  ['http://host/a/b%20c.xp3?x=1', 'a/b c.xp3'],
  ['', ''],
  ['C:\\games\\a\\b.xp3', 'C:/games/a/b.xp3'],
  ['/a/../b.xp3', 'b.xp3'],
];
for (const [input, expected] of cases) {
  const actual = defaultUrlKey(input);
  check(`defaultUrlKey(${JSON.stringify(input)}) -> ${JSON.stringify(expected)}`, actual === expected, `got ${JSON.stringify(actual)}`);
}
check('malformed percent escape does not throw', (() => { try { defaultUrlKey('/a%zz%b.xp3'); return true; } catch { return false; } })());

console.log('== readRange: windowing, EOF, errors ==');
const bytes = new Uint8Array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9]);
let opens = 0;
const source = createHostByteSource((key) => {
  opens += 1;
  if (key !== 'data.xp3') return null;
  return {
    size: async () => bytes.byteLength,
    readRange: async (offset, length) => bytes.subarray(offset, offset + length),
  };
});
const mid = await source.readRange('/data.xp3', 3, 4);
check('mid window', mid.length === 4 && mid[0] === 3 && mid[3] === 6, `got ${Array.from(mid)}`);
const tail = await source.readRange('/data.xp3', 8, 5);
check('short read at EOF (no throw)', tail.length === 2 && tail[0] === 8, `got ${Array.from(tail)}`);
await source.readRange('/data.xp3?v=1', 0, 2);
check('open called once per key across distinct urls', opens === 1, `opens=${opens}`);
let missing = '';
try { await source.readRange('/nope.dat', 0, 1); } catch (error) { missing = String(error.message); }
check('missing file message', missing.includes('no such file') && missing.includes('/nope.dat') && missing.includes('nope.dat'), missing);

console.log('== a rejecting open does not poison the cache ==');
let attempt = 0;
const flaky = createHostByteSource(async (key) => {
  attempt += 1;
  if (attempt === 1) throw new Error('transient');
  return { size: async () => 1, readRange: async () => new Uint8Array([7]) };
});
let firstFailed = false;
try { await flaky.readRange('/f', 0, 1); } catch { firstFailed = true; }
const second = await flaky.readRange('/f', 0, 1);
check('retry after transient rejection', firstFailed && second[0] === 7);

console.log('== install ==');
const target = {};
installHostByteSource(source, target);
check('installs under the seam key', target[HOST_BYTE_SOURCE_KEY] === source);
installHostByteSource(source);
check('installs on globalThis by default', globalThis[HOST_BYTE_SOURCE_KEY] === source);
delete globalThis[HOST_BYTE_SOURCE_KEY];

console.log('== hostByteSourceFromGameSource over FileListSource ==');
const gameSource = new FileListSource('g', [
  { path: 'data.xp3', file: new Blob([bytes]) },
]);
const adapted = hostByteSourceFromGameSource(gameSource);
const adaptedBytes = await adapted.readRange('/data.xp3', 2, 3);
check('reads through the GameSource', adaptedBytes.length === 3 && adaptedBytes[0] === 2, `got ${Array.from(adaptedBytes)}`);
let adaptedMissing = '';
try { await adapted.readRange('/missing.xp3', 0, 1); } catch (error) { adaptedMissing = String(error.message); }
check('missing path yields no such file', adaptedMissing.includes('no such file') && adaptedMissing.includes('missing.xp3'), adaptedMissing);

console.log(failures === 0 ? '\nALL CHECKS PASSED' : `\n${failures} CHECK(S) FAILED`);
process.exit(failures === 0 ? 0 : 1);
