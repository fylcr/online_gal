/**
 * Builds the index the worker reads, including each file's size.
 *
 * Sizes matter here in a way they did not for the shipped loader: a lazy file
 * has to answer `stat` before it has fetched anything, so the size has to be
 * known up front rather than discovered from the response.
 */
import { statSync } from 'node:fs';
import { writeFileSync } from 'node:fs';
import { resolve } from 'node:path';

const GAME_DIR = resolve('.ref/onsyuri-web/onsyuri_web/game');
const OUT = resolve('ports/ons/harness/onsyuri_index.json');

const FILES = [
  '0.txt',
  '1.txt',
  '2.txt',
  'arc.nsa',
  'arc1.nsa',
  'default.ttf',
  'ONScripter.exe',
  'Readme.txt',
];

const files = FILES.map((name) => ({
  path: name,
  url: `/game/${name}`,
  size: statSync(resolve(GAME_DIR, name)).size,
}));

const index = {
  title: 'onscript',
  gamedir: '/onsyuri/onscript',
  savedir: '/onsyuri_save/onscript',
  files,
  args: [],
};

writeFileSync(OUT, JSON.stringify(index, null, 2), 'utf8');

const total = files.reduce((sum, file) => sum + file.size, 0);
console.log(`wrote ${OUT}`);
for (const file of files) {
  console.log(`  ${String(file.size).padStart(13)} B  ${file.path}`);
}
console.log(`  ${String(total).padStart(13)} B  total (${(total / 1024 / 1024 / 1024).toFixed(2)} GB)`);
