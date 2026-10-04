/**
 * Source offer for the compat-web build.
 *
 * The `artemis-compat` module is a GPL-3.0 build of GPL-3.0 upstream source, so the artifact has
 * to answer for that source. Three things are needed and none of them is enough alone: the
 * upstream source the recipe compiled, the patch set plus overlays it applied while compiling it,
 * and the recipe itself.
 *
 * The recipe is not a convenience. The pinned upstream tree has no `build.sh`, no `ports/` and no
 * web CMake target at all -- `overlay/cmake/web.cmake` is *appended* to the upstream
 * `CMakeLists.txt` by `ports/artemis/compat-web/build.sh`, which also copies the host overlay in
 * and applies the two patches. Without that script the bundle is a pile of files with no
 * documented way to turn them back into the binary beside it, which is not the "preferred form
 * for making modifications" GPL-3.0 §1 asks for. So the recipe travels in the archive.
 *
 * The command line is fixed on purpose. There is no mode that extrapolates from what happens to
 * be checked out: `ARTC_PIN` from the environment overrides `--pin`, so the CI run records the
 * commit it actually built, and outside CI the pipeline's own pin is used.
 *
 *   node ports/artemis/compat-web/make-source-bundle.mjs --out <dir> [--work <dir>]
 *
 * Reads `source/` (the recipe's extraction of the pinned upstream commit) and the tracked
 * `patches/`, `overlay/` and `build.sh`. Writes `<out>/artemis-compat-source.tar.gz`.
 *
 * Reads only: nothing under `.ref/` or `source/` is modified, and the scratch work directory is
 * the only thing removed.
 */
import { spawnSync } from 'node:child_process';
import { existsSync, mkdirSync, readFileSync, readdirSync, rmSync, statSync, writeFileSync } from 'node:fs';
import { dirname, isAbsolute, join, relative, resolve, sep } from 'node:path';
import { fileURLToPath } from 'node:url';

const HERE = dirname(fileURLToPath(import.meta.url));
const ROOT = resolve(HERE, '..', '..', '..');

/** The pinned upstream commit, as recorded in `.github/upstream-pins.json`. */
export const DEFAULT_PIN = '9f622629f595db85e6f76c45c06edde29d7d9961';

/**
 * What the bundle carries. Fixed names: a caller cannot widen the source offer by editing an
 * invocation, and the destination inside the archive is a constant so a recipient can find it.
 *
 * `build.sh` and `upstream-root.diff` are the two that make the archive actionable rather than
 * descriptive; `NOTICE` says which files were added on top of the pinned upstream tree. The
 * licenses travel as files, not as text quoted into a notice: the upstream `LICENSE` (GPL-3.0
 * verbatim, the grant this build is made under) and Lua 5.1's `COPYRIGHT` (MIT) come from the
 * extracted tree, so what ships is upstream's own bytes.
 */
export const EXTRA_ENTRIES = [
  ['README.md', 'SOURCE-BUNDLE.md'],
  ['build.sh', 'build.sh'],
  // The standalone recipe. `build.sh` above is the record of what CI ran, but it cannot execute
  // inside this archive: it sources `ports/env.sh` and refuses a root that is not
  // `ports/artemis/compat-web/out`, and the archive has neither. Without a second recipe the
  // offer would document the build without making it reproducible off this repository.
  ['RUN-BUILD.sh', 'RUN-BUILD.sh'],
  ['patches/0001-pf8-readat-host-source.patch', 'patches/0001-pf8-readat-host-source.patch'],
  ['patches/0002-numeric-strto.patch', 'patches/0002-numeric-strto.patch'],
  ['overlay/host/web_main.cpp', 'overlay/host/web_main.cpp'],
  ['overlay/cmake/web.cmake', 'overlay/cmake/web.cmake'],
  // The audio backend exists only in this repository: upstream's `src/audio/audio.cpp` is the
  // non-Android stub, and the web target compiles `audio_web.cpp` in its place. The pinned tree
  // copied into `src/` therefore contains none of these three, and a bundle without them is an
  // offer that cannot rebuild the module beside it. `audio_validation.h` is the argument grammar
  // the host applies to the page's numbers; `audio_web.h` is the seam the two share.
  ['overlay/audio/audio_web.cpp', 'overlay/audio/audio_web.cpp'],
  ['overlay/audio/audio_web.h', 'overlay/audio/audio_web.h'],
  ['overlay/audio/audio_validation.h', 'overlay/audio/audio_validation.h'],
];

/** Files taken from the extracted upstream tree itself, at the path they already have there. */
export const UPSTREAM_ENTRIES = [
  ['LICENSE', 'LICENSE'],
  ['third_party/lua-5.1.5/COPYRIGHT', 'third_party/lua-5.1.5/COPYRIGHT'],
];

/** Written by this generator, not copied: they describe the archive, so nothing upstream owns them. */
export const GENERATED_ENTRIES = ['NOTICE', 'upstream-root.diff'];

export function bundlePlan({ pin = process.env.ARTC_PIN || DEFAULT_PIN } = {}) {
  if (!/^[0-9a-f]{40}$/.test(pin)) {
    throw new Error(`pin must be a full 40-character lowercase hex commit, got ${JSON.stringify(pin)}`);
  }
  return { pin, name: `artemis-compat-${pin.slice(0, 12)}` };
}

/** Every path a caller-supplied directory control can be abused through is refused here. */
function checkDir(label, value) {
  if (!value) throw new Error(`${label} must not be empty`);
  const abs = resolve(value);
  if (!isAbsolute(abs)) throw new Error(`${label} must be absolute: ${abs}`);
  return abs;
}

/**
 * Recursively copy one file or directory into `into`, skipping `.git`. Returns the count of files
 * copied, so an extraction that produced an empty tree is a failure rather than a bundle that
 * looks complete and contains no source.
 */
export function copyTree(from, into, { skip = new Set(['.git']) } = {}) {
  const info = statSync(from);
  if (info.isDirectory()) {
    mkdirSync(into, { recursive: true });
    let files = 0;
    for (const name of readdirSync(from).sort()) {
      if (skip.has(name)) continue;
      files += copyTree(join(from, name), join(into, name), { skip });
    }
    return files;
  }
  mkdirSync(dirname(into), { recursive: true });
  writeFileSync(into, readFileSync(from));
  return 1;
}

/**
 * Path spellings for the two things that disagree about them.
 *
 * Node's own filesystem calls want the native form (`D:\online_gal\...` on Windows). The bundled
 * tools do not: GNU tar reads a `D:` prefix in an absolute path as a *remote host* and fails with
 * `Cannot connect to D: resolve failed`, and git-bash's `diff` is happiest with the same `/d/...`
 * spelling the shell uses. `paths.relative` exists so tar receives a path anchored at the current
 * working directory, where no drive letter is involved at all.
 */
const paths = {
  /** `D:\a\b` -> `D:/a/b`. Forward slashes only; the drive letter stays, so use for `diff`, not tar. */
  posix: (p) => resolve(p).replace(/\\/g, '/'),
  /** Repo-relative POSIX form: what tar and the shell both read unambiguously. */
  relative: (p) => relative(REPO_ROOT, resolve(p)).replace(/\\/g, '/'),
};

/**
 * Run one external tool whose only result is its exit status.
 *
 * `stdio` is inherited rather than piped on purpose. This generator runs inside the CI sandbox,
 * where a program may not open the named pipes a piped `child_process` needs; the failure is an
 * `EPERM` on the *spawn*, which arrives as `error` with a null `status` and would be reported as
 * "tar exited null". Inheriting the parent's streams needs no pipe, and the tool's own stderr
 * reaches the log instead of a buffer nobody reads.
 *
 * The exit status is the whole contract: a non-zero one stops the bundle.
 */
function run(label, command, args, options) {
  const result = spawnSync(command, args, { stdio: ['ignore', 'inherit', 'inherit'], ...options });
  if (result.error) throw new Error(`${label} failed to start: ${result.error.message}`);
  if (result.status !== 0) {
    // No stderr is captured (see above), so the status is the only reason available here; the
    // tool's own message went to this process's stderr immediately above this line.
    throw new Error(`${label} exited ${result.status}${result.signal ? ` (signal ${result.signal})` : ''}`);
  }
}

/**
 * What the archive is and which files were added to the pinned tree. Written here rather than
 * copied, so it describes the archive it is inside rather than the recipe's intentions.
 */
function notice(pin, sourceFiles) {
  const added = [...EXTRA_ENTRIES.map(([, to]) => to), 'NOTICE', 'upstream-root.diff', 'PIN'];
  return `# artemis-compat corresponding source

Upstream: https://github.com/Weiss-UltimateSavior/artemis-compat.git
Commit:   ${pin}
Licence:  GPL-3.0 (see LICENSE, the upstream file, verbatim)

\`src/\` is the upstream tree at the commit above, as extracted by \`git archive\`
(${sourceFiles} files). Nothing below \`src/\` was removed and the only addition is
\`src/NOTICE\`; \`upstream-root.diff\` is that comparison, so a review does not have to
take this paragraph's word for it.

Everything the build does to \`src/\` is \`build.sh\`, the recipe CI ran, together with the
patch set, the overlays under \`overlay/\` and \`overlay/cmake/web.cmake\`. Since the pinned
tree ships no \`build.sh\`, no \`ports/\` and no web CMake target, the patches and overlays
are only a build input *through* a recipe that applies them and appends the CMake target to
the upstream \`CMakeLists.txt\` before configuring.

The audio backend is the one part of this build that exists nowhere upstream. The pinned
\`src/audio/audio.cpp\` is the non-Android stub; the web target compiles
\`overlay/audio/audio_web.cpp\` *instead of* it, so exactly one \`Audio::Impl\` is linked.
\`overlay/audio/audio_web.h\` is the seam the host shares with that file, and
\`overlay/audio/audio_validation.h\` is the argument grammar the host applies to the page's
numbers before any of them reaches the heap. None of the three is in the pinned \`src/\`
below, so this archive carries all three explicitly.

Added to the upstream tree for this build:
${added.map((name) => `  ${name}`).join('\n')}

Unpack, then build with the Emscripten SDK 6.0.9 (the version CI used). The recipe to use is
\`RUN-BUILD.sh\`, which reads the bundled \`src/\`, applies these patches and overlays, appends
the web CMake target and links the two modules. It needs no repository and no network:

    tar -xzf artemis-compat-source.tar.gz
    cd artemis-compat-${pin.slice(0, 12)}
    EMSDK_DIR=/path/to/emsdk bash RUN-BUILD.sh -j2
    # -> out/build/artemis-compat.js and out/build/artemis-compat.wasm

\`build.sh\` is included as well, because it is the recipe CI ran and the record of the *build
logic* — patch order, the appended CMake target, and every \`emcc\`/link flag. It cannot run
in this archive: it sources \`ports/env.sh\` and REFUSES to run unless its own path ends in
\`ports/artemis/compat-web/out\`, because that check is what stops a stray \`cd\` from becoming
a deleted directory. \`RUN-BUILD.sh\` is the same build without either assumption; if the two
ever disagree about a flag, \`build.sh\` is the one that produced the shipped modules.

Bundled third-party notices, as files rather than restatements:
  LICENSE                          GPL-3.0, the upstream project's own grant
  third_party/lua-5.1.5/COPYRIGHT  Lua 5.1, MIT

stb (MIT / public domain, dual) is vendored upstream under \`src/third_party/\` and its
terms travel with those files. zlib and the other SDK ports come from the Emscripten SDK,
not from this archive.

Not included, on purpose: no official Artemis SDK byte, no emscripten SDK, and no game
data. This archive is the source of the two modules beside it and nothing else.
`;
}

function main(argv) {
  const args = argv.slice(2);
  let out = '';
  let work = '';
  for (let i = 0; i < args.length; i += 1) {
    if (args[i] === '--out') out = args[++i] ?? '';
    else if (args[i] === '--work') work = args[++i] ?? '';
    else {
      console.error(`usage: node ports/artemis/compat-web/make-source-bundle.mjs --out <dir> [--work <dir>]`);
      process.exitCode = 2;
      return;
    }
  }
  const { pin, name } = bundlePlan();
  const outDir = checkDir('--out', out);
  const workDir = checkDir('--work', work ? work : join(outDir, '.source-bundle-work'));
  const source = join(HERE, 'out', 'source');
  const bundle = join(workDir, name);
  const tarball = join(outDir, 'artemis-compat-source.tar.gz');
  if (!existsSync(source) || !statSync(source).isDirectory()) {
    throw new Error(`no extracted source at ${source}; run ports/artemis/compat-web/build.sh first`);
  }

  // The one deletion, and it is the checked version of the recipe's own rule: `--work` is refused
  // above unless it is non-empty and absolute, and the recipe's paths are refused below. Resolved
  // before `rmSync` so an empty variable cannot become a path.
  if (workDir === ROOT || workDir === HERE || workDir === outDir) {
    throw new Error(`refusing to remove ${workDir}: it is a live checkout or output directory`);
  }
  console.log(`## pin:      ${pin}`);
  console.log(`## source:   ${source}`);
  console.log(`## work:     ${workDir}`);
  console.log(`## removing: ${bundle}`);
  rmSync(bundle, { recursive: true, force: true });
  mkdirSync(bundle, { recursive: true });

  // The upstream source the recipe compiled, verbatim, at the same relative paths the recipe
  // built from (including `src/host/`, so the host sources the patches edit are inside the
  // offer rather than described by it). The bundle is flat where upstream is flat.
  const sourceFiles = copyTree(source, join(bundle, 'src'));
  if (sourceFiles === 0) throw new Error(`${source} contains no files`);

  for (const [from, to] of EXTRA_ENTRIES) {
    const files = copyTree(join(HERE, from), join(bundle, to.replace(/\//g, sep)));
    if (files === 0) throw new Error(`tracked entry ${from} is missing or empty`);
  }

  // The licenses, out of the tree that was just copied. Both are copied, never transcribed: Lua
  // 5.1 lives at upstream `third_party/lua-5.1.5/` (not `third_party/lua51/`, which is only the
  // CMake target name), and its MIT `COPYRIGHT` is the file the recipient has to be able to read.
  for (const [from, to] of UPSTREAM_ENTRIES) {
    const files = copyTree(join(source, from), join(bundle, to.replace(/\//g, sep)));
    if (files === 0) throw new Error(`upstream entry ${from} is missing or empty in ${source}`);
  }

  writeFileSync(join(bundle, 'PIN'), `${pin}\n`);
  writeFileSync(join(bundle, 'NOTICE'), notice(pin, sourceFiles));
  // `src/NOTICE` is this generator's, not upstream's: it lands in the copied tree, so the diff is
  // the only place that can say the tree below `src/` is not byte-for-byte upstream.
  writeFileSync(join(bundle, 'src', 'NOTICE'), notice(pin, sourceFiles));

  // The upstream tree as archived, minus this generator's own additions. `diff -ruN` exits 1 when
  // the trees differ, which is the normal case here (`src/NOTICE` is this generator's), so only
  // exit 2 or worse is a failure. The paths are passed in the git-bash dialect for the same reason
  // as the tar invocation below.
  const diff = spawnSync('diff', ['-ruN', '--exclude=NOTICE', paths.posix(source), paths.posix(join(bundle, 'src'))], {
    cwd: REPO_ROOT, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'],
  });
  if (diff.error) throw new Error(`diff failed to start: ${diff.error.message}`);
  if (diff.status !== 0 && diff.status !== 1) {
    throw new Error(`diff exited ${diff.status}: ${(diff.stderr ?? '').trim()}`);
  }
  writeFileSync(join(bundle, 'upstream-root.diff'), diff.stdout ?? '');
  console.log(`## files:    ${sourceFiles} upstream + ${EXTRA_ENTRIES.length} tracked + `
    + `${UPSTREAM_ENTRIES.length} upstream licence files`);

  // Sorted, mtime- and owner-normalised, gzip without a timestamp: the same inputs have to give
  // the same archive, or the source offer changes every run and no one can diff it.
  //
  // Paths here are RELATIVE TO REPO_ROOT, deliberately. GNU tar treats a leading `C:`/`D:` in an
  // absolute Windows path as a remote host (`Cannot connect to D: resolve failed`), so passing the
  // resolved absolute form turns every run into an exit 2 no matter how correct the tree is. The
  // `-C workDir` form is safe because each of the two values is still checked before use: `-C`
  // takes the absolute path, and the member name is a single directory basename.
  run('tar', 'tar', [
    '--sort=name', '--owner=0', '--group=0', '--numeric-owner', '--mtime=@0',
    '--format=gnu', '--use-compress-program=gzip -9n',
    '-cf', paths.relative(tarball), '-C', workDir, name,
  ], { cwd: REPO_ROOT });
  if (!existsSync(tarball)) throw new Error(`tar reported success but wrote no ${tarball}`);

  console.log(`## bundle:   ${relative(ROOT, tarball).replace(/\\/g, '/')} (${statSync(tarball).size} B)`);
}

const REPO_ROOT = ROOT;

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) {
  try {
    main(process.argv);
  } catch (error) {
    console.error(`source bundle failed: ${error.message}`);
    process.exitCode = 1;
  }
}
