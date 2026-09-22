/**
 * Idempotently patches platforms/web/vlfs.js to add a "host byte source" seam.
 *
 * Why:
 *   The wasm engine reads game bytes through vlfs.js. Stock vlfs.js fetches every
 *   remote range itself. This patch lets an embedding page install
 *   `window.__onlineGalVfsSource` (e.g. backed by @online-gal/vfs) BEFORE vlfs.js
 *   is loaded; thereafter every "remote byte" read is served through that object
 *   and vlfs.js never calls fetch for those reads. When no object is installed,
 *   behaviour is identical to the stock reference implementation.
 *
 * Idempotency:
 *   `git checkout -- platforms/web/vlfs.js` restores the clean base on every run,
 *   then the four insertions are re-applied, so repeatable runs produce
 *   byte-identical output (verified by md5sum).
 *
 * Installed-object contract (all returns are Promises; failures must reject):
 *   readRange(url, offset, length) -> Uint8Array
 *       At most `length` bytes starting at `offset`; fewer means EOF.
 *   readStream?(url, offset, length) -> ReadableStream
 *       Optional, for streamed deflate paths; falls back to readRange + Blob otherwise.
 *
 * Usage:  node patch-vlfs-host-source.mjs [repo-root]
 */
import { spawnSync } from 'node:child_process';
import { readFileSync, writeFileSync, existsSync } from 'node:fs';
import { resolve } from 'node:path';
import { refPath } from '../../lib/ref.mjs';

const DEFAULT_REPO = refPath('kirikiroid2-web', 'KIRIKIROID2_WEB_DIR');
const REPO = process.argv[2] ? resolve(process.argv[2]) : DEFAULT_REPO;
const TARGET_REL = 'platforms/web/vlfs.js';
const TARGET = `${REPO}/${TARGET_REL}`;
const SHELL_REL = 'platforms/web/shell.html';
const SHELL = `${REPO}/${SHELL_REL}`;

// --- restore clean bases (the ONLY spawns of child processes) ---
function restoreCleanBase(pathRel) {
  const git = spawnSync('git', ['-C', REPO, 'checkout', '--', pathRel], {
    encoding: 'utf8',
  });
  if (git.status !== 0) {
    const detail = [git.stdout, git.stderr].filter(Boolean).join('\n').trim();
    console.error(
      `patch-vlfs-host-source: git checkout failed (exit ${git.status}) ` +
      `for ${pathRel}${detail ? ':\n' + detail : ''}`);
    process.exit(1);
  }
}

if (!existsSync(REPO)) {
  console.error(`patch-vlfs-host-source: repo root not found: ${REPO}`);
  process.exit(1);
}
restoreCleanBase(TARGET_REL);
restoreCleanBase(SHELL_REL);

// --- read ---
let raw;
try {
  raw = readFileSync(TARGET, 'utf8');
} catch (e) {
  console.error(`patch-vlfs-host-source: could not read ${TARGET}: ${e.message}`);
  process.exit(1);
}

// Defensive: strip a leading BOM and normalise CRLF -> LF so the written file
// is guaranteed LF / no-BOM regardless of input (this repo's vlfs.js is LF).
const before = raw.replace(/^\uFEFF/, '').replace(/\r\n/g, '\n');
const beforeBytes = Buffer.byteLength(before, 'utf8');
const lines = before.split('\n');

function occurrences(hay, needle) {
  let n = 0, i = 0;
  while ((i = hay.indexOf(needle, i)) !== -1) { n++; i += needle.length; }
  return n;
}

// Anchors are matched as WHOLE lines (trailing whitespace tolerated) so that a
// short anchor such as "    function makeOpfsSessionName() {" cannot also match
// a deeper-indented sibling via substring matching.
function anchorLineIndices(anchor) {
  const idxs = [];
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];
    if (line === anchor || line.trimEnd() === anchor) idxs.push(i);
  }
  return idxs;
}

const ANCHORS = [
  { name: 'makeOpfsSessionName',  line: '    function makeOpfsSessionName() {',                       where: 'before' },
  { name: 'caseRemote',           line: "                case 'remote': {",                           where: 'after'  },
  { name: '_readZipSourceBytes',  line: '        async _readZipSourceBytes(source, pos, len) {',      where: 'after'  },
  { name: '_readZipSourceStream', line: '        async _readZipSourceStream(source, pos, len) {',     where: 'after'  },
];

// Verify every anchor occurs exactly once BEFORE writing anything.
for (const a of ANCHORS) {
  const idxs = anchorLineIndices(a.line);
  if (idxs.length !== 1) {
    console.error(
      `patch-vlfs-host-source: anchor "${a.name}" (${JSON.stringify(a.line)}) ` +
      `found ${idxs.length} times, expected exactly 1. Aborting without writing.`);
    process.exit(1);
  }
}

// Insertion blocks. Indentation is preserved verbatim from the spec.
//
// Note on Insertion 1's comment: the spec's comment mentioned the literal
// `window.__onlineGalVfsSource` (the very same token the code line uses). With a
// verbatim comment that literal would appear twice in vlfs.js (comment + code),
// which would violate Required behaviour #5 — "exactly once" — and make the
// sanity assertion fail on every run. Since the code line must keep the literal
// (it is the actual seam), the comment's mention is rephrased to "宿主取数对象"
// ("host byte source object"). No functional code is touched: every insertion's
// code is byte-for-byte the spec, and only this one comment word changes.
const INSERT_1 = `    // 宿主取数通道（host byte source）。
    //
    // 嵌入页面可以在加载本脚本之前装一个宿主取数对象，此后本文件
    // 所有「远程字节」读取都改走它，而不是自己 fetch。平台侧据此把取数来源换成
    // @online-gal/vfs（按需加载 + 平台自己的缓存与索引策略）。没有装这个对象时，
    // 本文件的行为与参考实现完全一致。
    //
    // 同源父窗口也认：本页面是被嵌进 iframe 的，父窗口在创建 frame *之前*就持有
    // 宿主对象，而本脚本在 frame 的 load 事件之前就已经执行——只认自己窗口的话，
    // 脚本执行期永远看不到它（重载尤其明显）。跨源时读父窗口会抛，吞掉即可。
    //
    // 契约（全部返回 Promise，失败必须 reject）：
    //   readRange(url, offset, length) -> Uint8Array
    //       返回 offset 起的至多 length 字节；少于 length 表示读到源末尾。
    //   readStream?(url, offset, length) -> ReadableStream
    //       可选，供 deflate 流式解压路径使用；缺省时回退到 readRange + Blob.stream()。
    function hostByteSource() {
        var src = (typeof window !== 'undefined') ? window.__onlineGalVfsSource : null;
        if (!src && typeof window !== 'undefined' && window.parent && window.parent !== window) {
            try { src = window.parent.__onlineGalVfsSource; } catch (e) { src = null; }
        }
        return (src && typeof src.readRange === 'function') ? src : null;
    }
`; // trailing blank line precedes the anchor line (`makeOpfsSessionName`)

const INSERT_2 = `                    var hostSource = hostByteSource();
                    if (hostSource) {
                        // 宿主取数：不再自己发 Range 请求，也不做 200 降级缓存，
                        // 因为取数来源自己知道尺寸与缓存策略。
                        var hostBytes = await hostSource.readRange(
                            e.url, pos, len, cloneRequestHeaders(e.requestHeaders));
                        if (hostBytes.byteLength < len)
                            throw new Error('host range read ' + e.url + ': got ' +
                                hostBytes.byteLength + ' < ' + len);
                        return hostBytes;
                    }`; // no trailing blank: sits immediately after the `case 'remote': {` line

const INSERT_3 = `            // 宿主通道只对“远程”源有意义：blob 源（registerZipBlob / assets.zip）
            // 没有 URL，字节本来就在本地，问宿主只会拿到 undefined。
            var hostSrc = (source.kind !== 'blob') ? hostByteSource() : null;
            if (hostSrc) {
                // 走宿主时跳过 _validateZipRangeResponse：那是给 HTTP 响应头用的，
                // 宿主保证返回的就是请求的那段字节，长度由下面这段校验兜住。
                var hostZipBytes = await hostSrc.readRange(source.url, pos, len);
                if (hostZipBytes.byteLength < len)
                    throw new Error('host zip range read ' + source.url + ': got ' +
                        hostZipBytes.byteLength + ' < ' + len);
                return hostZipBytes.length === len ? hostZipBytes : hostZipBytes.slice(0, len);
            }`; // no trailing blank: sits immediately after the `_readZipSourceBytes` signature

const INSERT_4 = `            // 同上：blob 源不经宿主。
            var hostStreamSrc = (source.kind !== 'blob') ? hostByteSource() : null;
            if (hostStreamSrc) {
                if (typeof hostStreamSrc.readStream === 'function')
                    return await hostStreamSrc.readStream(source.url, pos, len);
                var hostStreamBytes = await hostStreamSrc.readRange(source.url, pos, len);
                if (hostStreamBytes.byteLength < len)
                    throw new Error('host zip range read ' + source.url + ': got ' +
                        hostStreamBytes.byteLength + ' < ' + len);
                // 宿主没提供流：按块转成流。范围长度本身有界，不会整包入内存。
                return new Blob([hostStreamBytes]).stream();
            }`; // no trailing blank: sits immediately after the `_readZipSourceStream` signature

const INSERTS = {
  makeOpfsSessionName:  INSERT_1.split('\n'),
  caseRemote:           INSERT_2.split('\n'),
  _readZipSourceBytes:  INSERT_3.split('\n'),
  _readZipSourceStream: INSERT_4.split('\n'),
};

// Build the patched line array. Apply insertions in DESCENDING anchor-line index
// order so earlier splices never shift the indices of lower anchors.
const result = lines.slice();
const ops = ANCHORS.map(a => ({ name: a.name, where: a.where, at: anchorLineIndices(a.line)[0] }));
ops.sort((x, y) => y.at - x.at);
for (const op of ops) {
  const block = INSERTS[op.name];
  if (op.where === 'before') {
    result.splice(op.at, 0, ...block);
  } else {
    result.splice(op.at + 1, 0, ...block);
  }
}

const after = result.join('\n');

// Write UTF-8, LF, no BOM.
writeFileSync(TARGET, after, 'utf8');

// Assert the written (on-disk) content exposes the seam marker exactly once.
const written = readFileSync(TARGET, 'utf8');
const seam = occurrences(written, 'window.__onlineGalVfsSource');
const defn = occurrences(written, 'function hostByteSource() {');
if (seam !== 1 || defn !== 1) {
  console.error(
    `patch-vlfs-host-source: assertion failed — ` +
    `window.__onlineGalVfsSource=${seam} (want 1), ` +
    `"function hostByteSource() {"=${defn} (want 1). File left in patched state.`);
  process.exit(1);
}

const afterBytes = Buffer.byteLength(written, 'utf8');

// ===================== shell.html =====================
//
// The seam above only covers byte reads. shell.html discovers a remote file's
// SIZE and range support solely with `fetch(url, {method:'HEAD'})`, and a
// client-side source (OPFS, FileSystemAccess, a file inside an archive) has no
// URL to HEAD — so without this the stock flow would fall back to downloading
// the whole archive. `hostSourceStat` asks the installed host object first and
// only falls back to HEAD when it answers nothing, which keeps the reference
// behaviour identical when no host object is installed.

const SHELL_ANCHOR = '        async function fetchXp3FromUrl(url) {';

const SHELL_HELPER = `        // 宿主取数通道：宿主对象由嵌入页面安装，并允许从同源父窗口读取——本页面在
        // frame 里运行，父窗口在创建 frame 之前就已持有它，而本脚本在 frame 的 load
        // 之前就执行，只认自己窗口会永远看不到（重载时尤其明显）。
        function hostSeam() {
            var src = (typeof window !== 'undefined') ? window.__onlineGalVfsSource : null;
            if (!src && typeof window !== 'undefined' && window.parent && window.parent !== window) {
                try { src = window.parent.__onlineGalVfsSource; } catch (e) { src = null; }
            }
            return src || null;
        }

        // 当宿主提供 stat 时，远程文件的尺寸与范围支持由平台回答——平台来源可能根本
        // 没有 URL 可 HEAD（OPFS / FileSystemAccess / 压缩包内的文件）。没有宿主对象
        // 时返回 null，后续流程与参考实现完全一致。
        async function hostSourceStat(url) {
            var src = hostSeam();
            if (!src || typeof src.stat !== 'function') return null;
            try {
                var info = await src.stat(url);
                if (!info || !(info.size > 0)) return null;
                return {
                    size: info.size,
                    etag: typeof info.etag === 'string' ? info.etag : ''
                };
            } catch (e) {
                return null;
            }
        }

`;

const SHELL_OLD_XP3 = `                var ranges = false, size = -1;
                try {
                    var head = await fetch(url, { method: 'HEAD' });
                    if (head.ok) {
                        ranges = (head.headers.get('Accept-Ranges') || '').toLowerCase() === 'bytes';
                        size = parseInt(head.headers.get('Content-Length') || '-1', 10);
                    }
                } catch (e) {}`;

const SHELL_NEW_XP3 = `                var ranges = false, size = -1;
                var hostProbeXp3 = await hostSourceStat(url);
                if (hostProbeXp3) {
                    ranges = true;
                    size = hostProbeXp3.size;
                    console.log('[vlfs] host byte-source xp3: ' + url + ', ' + size + ' bytes');
                } else {
                    try {
                        var head = await fetch(url, { method: 'HEAD' });
                        if (head.ok) {
                            ranges = (head.headers.get('Accept-Ranges') || '').toLowerCase() === 'bytes';
                            size = parseInt(head.headers.get('Content-Length') || '-1', 10);
                        }
                    } catch (e) {}
                }`;

const SHELL_OLD_ZIP = `                    var ranges = false, size = -1, archiveETag = '';
                    try {
                        var head = await fetch(url, {
                            method: 'HEAD', cache: 'no-store'
                        });
                        if (head.ok) {
                            ranges = (head.headers.get('Accept-Ranges') || '')
                                .toLowerCase() === 'bytes';
                            size = parseInt(
                                head.headers.get('Content-Length') || '-1', 10);
                            archiveETag = strongArchiveETag(
                                head.headers.get('ETag'));
                        }
                    } catch (e) {}`;

const SHELL_NEW_ZIP = `                    var ranges = false, size = -1, archiveETag = '';
                    var hostProbeZip = await hostSourceStat(url);
                    if (hostProbeZip) {
                        ranges = true;
                        size = hostProbeZip.size;
                        archiveETag = hostProbeZip.etag;
                        console.log('[vlfs] host byte-source zip: ' + url + ', ' + size + ' bytes');
                    } else {
                        try {
                            var head = await fetch(url, {
                                method: 'HEAD', cache: 'no-store'
                            });
                            if (head.ok) {
                                ranges = (head.headers.get('Accept-Ranges') || '')
                                    .toLowerCase() === 'bytes';
                                size = parseInt(
                                    head.headers.get('Content-Length') || '-1', 10);
                                archiveETag = strongArchiveETag(
                                    head.headers.get('ETag'));
                            }
                        } catch (e) {}
                    }`;

// With a host byte source installed the archive's bytes are already local to the
// platform (OPFS handle, File System Access handle, a file inside an archive), so
// the default `loadMode=eager` — which downloads the whole archive into OPFS —
// is both pointless and impossible: there is no URL to GET. Defaulting to lazy in
// that case routes the mount through the `hostSourceStat` probe below. An explicit
// `loadMode=eager` from the caller is still honoured.
const SHELL_OLD_LOADMODE =
  `        var loadMode = normalizeLoadModeUrlParam(urlParams.get('loadMode'));`;

const SHELL_NEW_LOADMODE = `        var loadMode = normalizeLoadModeUrlParam(urlParams.get('loadMode'));
        // 装了宿主取数对象、且调用方没有显式指定 loadMode 时，默认按需加载：
        // 归档字节由平台本地提供（OPFS / FileSystemAccess / 压缩包内的文件），
        // 既没有可下载的 URL，再整包拉进 OPFS 也纯属浪费。显式 loadMode=eager 仍生效。
        if (urlParams.get('loadMode') === null && hostSeam()) {
            loadMode = 'lazy';
            console.log('[vlfs] host byte-source present; defaulting to loadMode=lazy');
        }`;

// Where the eager/lazy decision actually has to be made.
//
// `loadMode` is parsed while the page's scripts run, which is *before* the frame fires
// `load` — and the seam is installed on `load`, because navigation replaces the window
// and an earlier install would be discarded. So a seam-presence test at parse time
// always sees nothing, the eager branch wins, and the shell downloads the archive from
// a URL that a client-side source does not have (a real 404, found by the app-level
// browser check). Checking again at fetch time is what makes the seam authoritative.
const SHELL_OLD_EAGER = `                var eagerDeflate = loadMode === 'eager';
                var resourceKey = archiveResourceKey(url);`;

const SHELL_NEW_EAGER = `                var eagerDeflate = loadMode === 'eager';
                var resourceKey = archiveResourceKey(url);
                // 宿主取数通道优先：宿主答得出尺寸时，归档就在平台本地
                // （IDB / OPFS / FSA），既没有可下载的 URL，也不该再整包拉一遍。
                // 这个判断必须在**取数时**做，不能在 loadMode 解析处做：接缝要等
                // frame 的 load 事件才装上，而 loadMode 在脚本执行期就已经定下了。
                if (eagerDeflate && (await hostSourceStat(url))) {
                    eagerDeflate = false;
                }`;

let shellRaw;
try {
  shellRaw = readFileSync(SHELL, 'utf8');
} catch (e) {
  console.error(`patch-vlfs-host-source: could not read ${SHELL}: ${e.message}`);
  process.exit(1);
}
const shellBefore = shellRaw.replace(/^\uFEFF/, '').replace(/\r\n/g, '\n');
const shellBeforeBytes = Buffer.byteLength(shellBefore, 'utf8');

function replaceExactly(text, oldText, newText, name) {
  const n = occurrences(text, oldText);
  if (n !== 1) {
    console.error(
      `patch-vlfs-host-source: shell anchor "${name}" found ${n} times, ` +
      `expected exactly 1. Aborting without writing ${SHELL_REL}.`);
    process.exit(1);
  }
  // Function replacement, so `$&`-style sequences inside newText are never interpreted.
  return text.replace(oldText, () => newText);
}

let shellAfter = replaceExactly(
  shellBefore, SHELL_ANCHOR, SHELL_HELPER + SHELL_ANCHOR, 'fetchXp3FromUrl');
shellAfter = replaceExactly(shellAfter, SHELL_OLD_XP3, SHELL_NEW_XP3, 'xp3-HEAD-probe');
shellAfter = replaceExactly(shellAfter, SHELL_OLD_ZIP, SHELL_NEW_ZIP, 'zip-HEAD-probe');
shellAfter = replaceExactly(
  shellAfter, SHELL_OLD_LOADMODE, SHELL_NEW_LOADMODE, 'loadMode-default');
shellAfter = replaceExactly(shellAfter, SHELL_OLD_EAGER, SHELL_NEW_EAGER, 'eager-prefers-host');

writeFileSync(SHELL, shellAfter, 'utf8');

const shellWritten = readFileSync(SHELL, 'utf8');
const shellCalls = occurrences(shellWritten, 'hostSourceStat(');
// Four occurrences: the helper's definition plus its three call sites (the `?xp3=`
// probe, the ZIP probe, and the fetch-time eager check).
const shellSeam = occurrences(shellWritten, 'window.__onlineGalVfsSource');
// One occurrence: the line `hostSeam` reads. Its prose explains the seam without naming
// the literal, and the parent fallback spells its path differently (`window.parent.…`),
// so any other count would mean an install site that bypasses `hostSeam()`.
const shellSeamHelper = occurrences(shellWritten, 'hostSeam(');
// Three occurrences: `hostSeam`'s definition, the stat helper, and the loadMode default.
if (shellCalls !== 4 || shellSeam !== 1 || shellSeamHelper !== 3) {
  console.error(
    `patch-vlfs-host-source: assertion failed for ${SHELL_REL} — ` +
    `hostSourceStat(=${shellCalls} (want 4), window.__onlineGalVfsSource=${shellSeam} (want 1), ` +
    `hostSeam(=${shellSeamHelper} (want 3). File left in patched state.`);
  process.exit(1);
}
const shellAfterBytes = Buffer.byteLength(shellWritten, 'utf8');

console.log(
  `patched ${TARGET_REL}: ${beforeBytes} -> ${afterBytes} bytes; ` +
  `anchors: makeOpfsSessionName, caseRemote, _readZipSourceBytes, _readZipSourceStream`);
console.log(
  `patched ${SHELL_REL}: ${shellBeforeBytes} -> ${shellAfterBytes} bytes; ` +
  `anchors: fetchXp3FromUrl, xp3-HEAD-probe, zip-HEAD-probe, loadMode-default, eager-prefers-host`);
process.exit(0);
