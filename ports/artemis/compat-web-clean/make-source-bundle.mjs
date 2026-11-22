import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
import { fileURLToPath } from 'node:url';
import { execFileSync } from 'node:child_process';

const port = fs.realpathSync(path.dirname(fileURLToPath(import.meta.url)));
const candidate = process.argv[2];
if (!candidate || !/^[A-Za-z0-9][A-Za-z0-9_-]*$/.test(candidate)) throw Error('Invalid candidate');
const run = path.join(port, 'out', candidate);
function guarded(p) {
  if (fs.realpathSync(p) !== p || !p.startsWith(path.join(port, 'out') + path.sep)) throw Error('Output escaped clean port: ' + p);
  return p;
}
guarded(run);
const source = guarded(path.join(run, 'source'));
const cache = guarded(path.join(run, 'gitrepo'));
const clock = process.env.ARTC_CLEAN_CLOCK || '';
if (clock && clock !== 'clock-ms-01') throw Error('Invalid ARTC_CLEAN_CLOCK');
const manifestPath = clock ? 'candidates/clock-ms-01/manifest.json' : 'source-manifest.json';
const manifestBytes = fs.readFileSync(path.join(port, manifestPath));
if (!manifestBytes.equals(fs.readFileSync(path.join(run, 'source-manifest.json')))) throw Error('Manifest changed since build');
const manifest = JSON.parse(manifestBytes);
const git = (...args) => execFileSync('git', ['-C', cache, ...args], { encoding: 'utf8' }).trim();
if (git('remote', 'get-url', 'origin') !== manifest.origin || git('rev-parse', 'HEAD') !== manifest.commit || git('rev-parse', `${manifest.commit}^{tree}`) !== manifest.tree || git('status', '--porcelain', '--untracked-files=all')) throw Error('Upstream identity/cleanliness mismatch');
const hash = bytes => crypto.createHash('sha256').update(bytes).digest('hex');
const expected = JSON.parse(fs.readFileSync(path.join(run, 'source-hashes.json'), 'utf8'));
const actual = {};
function inventory(dir) {
  for (const entry of fs.readdirSync(dir, { withFileTypes: true })) {
    const p = path.join(dir, entry.name);
    if (entry.isDirectory()) inventory(p);
    else if (entry.isFile()) actual[path.relative(source, p).split(path.sep).join('/')] = hash(fs.readFileSync(p));
    else throw Error('Unsupported upstream entry: ' + p);
  }
}
inventory(source);
if (Object.keys(actual).length !== Object.keys(expected).length || Object.entries(expected).some(([p, h]) => actual[p] !== h)) throw Error('Archived source changed');
for (const f of [...manifest.platformFiles, ...manifest.recipeFiles]) {
  if (!fs.lstatSync(path.join(port, f)).isFile()) throw Error('Platform/recipe missing or symlink: ' + f);
}
const portHashes = JSON.parse(fs.readFileSync(path.join(run, 'port-hashes.json'), 'utf8'));
for (const [f, h] of Object.entries(portHashes)) if (hash(fs.readFileSync(path.join(port, f))) !== h) throw Error('Port changed since build: ' + f);
// Include the entire new platform, including an optional B-owned ABI header.
const staging = path.join(run, 'source-bundle');
if (fs.existsSync(staging)) throw Error('Bundle staging exists; never overwrite');
fs.mkdirSync(staging);
guarded(staging);
fs.cpSync(source, path.join(staging, 'upstream'), { recursive: true });
const recipe = path.join(staging, 'port');
fs.mkdirSync(recipe);
for (const f of manifest.recipeFiles) {
  fs.mkdirSync(path.dirname(path.join(recipe, f)), { recursive: true });
  fs.copyFileSync(path.join(port, f), path.join(recipe, f));
}
if (manifest.clockCandidate) {
  const f = manifest.clockCandidate.replacementPath;
  if (hash(fs.readFileSync(path.join(port, f))) !== manifest.clockCandidate.replacementSha256 || actual[manifest.clockCandidate.upstreamPath] !== manifest.clockCandidate.upstreamSha256) throw Error('Clock candidate source mismatch');
  fs.copyFileSync(path.join(port, f), path.join(recipe, f));
}
function copyPlatform(dir) {
  const from = path.join(port, dir);
  for (const e of fs.readdirSync(from, { withFileTypes: true })) {
    const relative = path.join(dir, e.name);
    if (e.isDirectory()) copyPlatform(relative);
    else if (e.isFile()) {
      if (!Object.hasOwn(portHashes, relative.split(path.sep).join('/'))) throw Error('New platform entry since build: ' + relative);
      fs.mkdirSync(path.dirname(path.join(recipe, relative)), { recursive: true });
      fs.copyFileSync(path.join(port, relative), path.join(recipe, relative));
    } else throw Error('Unsupported platform entry: ' + relative);
  }
}
for (const dir of ['host', 'platform', 'audio']) copyPlatform(dir);
fs.copyFileSync(path.join(run, 'source-hashes.json'), path.join(staging, 'source-hashes.json'));
fs.copyFileSync(path.join(run, 'port-hashes.json'), path.join(staging, 'port-hashes.json'));
fs.writeFileSync(path.join(staging, 'BUILD.txt'), 'Corresponding source: upstream/ + port/.\nOffline reconstruction with an existing Emscripten SDK/zlib:\nemcmake cmake -S port -B build -DARTC_CLEAN_SOURCE=<absolute upstream directory>' + (clock ? ' -DARTC_CLEAN_CLOCK=clock-ms-01' : '') + ' -DZLIB_INCLUDE_DIR="<effective cache>/sysroot/include" -DZLIB_LIBRARY="<effective cache>/sysroot/lib/wasm32-emscripten/libz.a" -DCMAKE_BUILD_TYPE=Release\ncmake --build build\nUse the same absolute effective cache for EM_CACHE and both ZLIB paths; quote paths containing spaces. Convert Git Bash cache paths to native paths for Emscripten. Unset/empty EM_CACHE uses the installed SDK cache (unset an empty variable before emcmake); relative cache paths are not supported.\nNo official SDK or legacy patches are inputs. Runtime integration is delivered separately.\n');
const archive = path.join(run, 'artemis-compat-source.tar.gz');
if (fs.existsSync(archive)) throw Error('Bundle archive exists');
// Relative tar operands avoid interpreting a Windows drive colon as a remote host.
execFileSync('tar', ['-czf', path.basename(archive), '-C', path.basename(staging), '.'], { cwd: run, stdio: 'inherit' });
fs.writeFileSync(archive + '.sha256', `${hash(fs.readFileSync(archive))}  ${path.basename(archive)}\n`);
console.log(archive);
