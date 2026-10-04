/** Concatenate the audio helper in front of the classic worker. Byte copy, no regex. */
import { mkdirSync, readFileSync, writeFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const here = dirname(fileURLToPath(import.meta.url));
const dir = join(here, '..', 'src', 'compat');
const outputDir = join(here, '..', 'dist', 'compat');
mkdirSync(outputDir, { recursive: true });
const helper = readFileSync(join(dir, 'worker-audio.js'));
const worker = readFileSync(join(dir, 'compat-worker.js'));
writeFileSync(join(outputDir, 'compat-worker.js'), Buffer.concat([helper, Buffer.from('\n'), worker]));
