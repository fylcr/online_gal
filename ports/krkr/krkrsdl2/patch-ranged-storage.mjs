/**
 * Ranged-read storage for the KiriKiri engine, with the JavaScript kept out of C++ entirely.
 *
 * ## Why this is the only place that needs changing
 *
 * The engine opens files in exactly one place: `tTVPLocalFileStream`'s constructor calls
 * `SDL_RWFromFile` (`src/core/base/sdl2/StorageImpl.cpp`). Everything else — the XP3 reader, the
 * TJS VM, the visual system — reaches the filesystem through `TVPCreateStream` in
 * `base/StorageIntf.cpp`, which is platform-independent and funnels every content path. The XP3
 * reader already treats the archive as a **seekable stream** (`XP3Archive.cpp:270, 340, 554`) and
 * never assumes it is resident, so a ranged implementation is a drop-in rather than a rewrite.
 *
 * ## Why the JavaScript is not embedded with EM_JS
 *
 * `EM_JS` puts JavaScript inside a C string literal, and this patcher's own text is a JavaScript
 * template literal — so a stray apostrophe or backtick **in a comment** terminates one layer or the
 * other. The compiler then reports a syntax error at the `EM_JS` line with the macro apparently
 * undefined, pointing at the preprocessor rather than at the comment responsible. That cost several
 * rounds of chasing prose punctuation, each fix relocating the error to the next comment.
 *
 * So the JavaScript is written to **`krkr-range.js`** and linked in with **`--pre-js`**, and C++
 * only declares an `extern "C"` function. Two layers of string quoting are removed rather than
 * navigated, and the JS becomes editable as JavaScript.
 *
 * ## Why synchronous
 *
 * `tTJSBinaryStream::Read`/`Seek` are blocking by contract and are called from inside the script
 * interpreter, where nothing can be awaited. So the ranged read blocks on synchronous XHR — which
 * is **forbidden on the main thread** for an `arraybuffer` response and legal in a worker. Hence a
 * threaded build is mandatory (`USE_PTHREADS` + `PROXY_TO_PTHREAD`), which the engine already
 * supports: `SDLApplication.cpp` branches on `__EMSCRIPTEN_PTHREADS__` in eight places.
 */
import { readFileSync, writeFileSync } from 'node:fs';
import { requireRef } from '../../lib/ref.mjs';

const repo = requireRef('krkrsdl2', 'KRKRSDL2_DIR');
const headerPath = `${repo}/src/core/base/sdl2/StorageImpl.h`;
const sourcePath = `${repo}/src/core/base/sdl2/StorageImpl.cpp`;
const preJsPath = `${repo}/krkr-range.js`;

/* ------------------------------------------------------------------ the JS */

/*
  Written as a plain file so it can be edited as JavaScript.

  No apostrophes or backticks appear in the comments below either — the file is embedded into the
  link line, and there is no longer any C++ string for them to terminate, but keeping the habit
  avoids the mistake reappearing if this ever moves back inline.
*/
/*
  A **js-library**, not a pre-js.

  This distinction cost a round: a symbol referenced by wasm must be declared in a file the linker
  treats as a JS library, entered through `mergeInto(LibraryManager.library, ...)`. A `--pre-js`
  file runs in a different scope and `wasm-ld` still reports the symbol undefined —
  `undefined symbol: krkr_js_range_read (referenced by root reference)`.

  The library file therefore carries the bridge function, and the page-facing helpers are installed
  onto `Module` from the same file, which also runs in the module scope.
*/
const libraryJs = `// Ranged HTTP reads for the KiriKiri web port. Linked with --js-library.
//
// Synchronous by necessity. The engine blocking stream Read cannot await, so the request has to
// block, and a synchronous XMLHttpRequest with an arraybuffer response is only permitted in a
// worker. The build uses PROXY_TO_PTHREAD so main runs on one.
//
// Declared in C++ as
//
//   extern "C" int krkr_js_range_read(const char *url, double start, double length, void *dest)
//
// Returns 0 on success. The page supplies Module.__krkrRangeRead, so the transport stays swappable
// without rebuilding the engine.

mergeInto(LibraryManager.library, {
  krkr_js_range_read: function (urlPointer, start, length, destination) {
    // The engine asks for its archive by the path it was started with, which is \`/data.xp3\`. The
    // page supplies the URL that path actually lives at.
    var requested = UTF8ToString(urlPointer);
    var url = Module.__krkrArchiveUrl || self.__krkrArchiveUrl || requested;
    var read = Module.__krkrRangeRead;
    if (typeof read !== "function") return -1;
    var bytes = read(url, start, length);
    if (!bytes || bytes.length !== length) return -2;
    HEAPU8.set(bytes, destination);
    return 0;
  },

  // Reports the archive URL and size, or -1 when there is no HTTP-served archive.
  //
  // The URL is read from **Module**, not from the worker global.
  //
  // This is the reason range requests were zero. PROXY_TO_PTHREAD runs the engine in a worker,
  // where the global object is the WorkerGlobalScope — so a global the page set as
  // self.__krkrArchiveUrl simply does not exist on the thread the engine runs on. Module is shared
  // between the page and the worker, so the URL is published there by the launcher and read here.
  //
  // The worker global is still consulted as a fallback for the unthreaded case, where the module
  // does run on the page and the two are the same object.
  // Reports to the server, because the page cannot be inspected while the engine runs.
  //
  // The engine occupies its thread, so reading the page's log or the console times out. A
  // synchronous request from inside the bridge is the one diagnostic channel that works: it appears
  // in the server log whatever the page is doing, and it runs on the engine's thread because that is
  // where the bridge is called.
  krkr_js_diag: function (eventPointer) {
    try {
      var event = UTF8ToString(eventPointer);
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/__diag?event=" + encodeURIComponent(event), false);
      xhr.send(null);
    } catch (ignored) {
      // Diagnostics must never break the engine.
    }
  },

  krkr_js_archive_info: function (urlBuffer, urlCapacity) {
    function report(event, detail) {
      try {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/__diag?event=" + encodeURIComponent(event) + "&detail=" + encodeURIComponent(detail), false);
        xhr.send(null);
      } catch (ignored) {
        // Diagnostics must never break the engine.
      }
    }

    var url = Module.__krkrArchiveUrl || self.__krkrArchiveUrl;
    report("archive-info-called", url ? url : "(no url)");
    if (!url) return -1;
    var size = Module.__krkrArchiveSize(url);
    report("archive-size", String(size));
    if (!(size > 0)) return -1;
    // Written byte by byte: stringToUTF8 would need more room than the URL to append a terminator
    // safely, and the buffer length is known here.
    var length = Math.min(url.length, urlCapacity - 1);
    for (var i = 0; i < length; i++) {
      HEAPU8[urlBuffer + i] = url.charCodeAt(i) & 0xff;
    }
    HEAPU8[urlBuffer + length] = 0;
    return size;
  },

  // Installed inside the library object, not at file top level.
  //
  // A bare \`Module.__x = ...\` at the top of a js-library is evaluated outside the module scope and
  // the JS compiler aborts with "Internal compiler error JS compiler" — which gives no clue that the
  // placement of an assignment is the problem. Library entries run during module initialisation, so
  // the assignments belong here.
  $krkrInstallHelpers: function () {
    // Total size of the archive, asked once and cached.
    //
    // The number matters: \`tTVPHttpRangeStream\` rejects a zero size, and the XP3 reader needs it to
    // resolve a seek from the end. A \`HEAD\` request is enough and transfers nothing.
    var cachedSize = -1;
    Module.__krkrArchiveSize = function (url) {
      if (cachedSize >= 0) return cachedSize;
      var request = new XMLHttpRequest();
      request.open("HEAD", url, false);
      request.send(null);
      if (request.status === 200) {
        var header = request.getResponseHeader("Content-Length");
        if (header) cachedSize = parseInt(header, 10);
      }
      if (cachedSize < 0) {
        console.warn("krkr: could not determine the archive size for " + url);
      }
      return cachedSize;
    };

    Module.__krkrRangeRead = function (url, start, length) {
      var request = new XMLHttpRequest();
      request.open("GET", url, false);
      request.responseType = "arraybuffer";
      request.setRequestHeader("Range", "bytes=" + start + "-" + (start + length - 1));
      request.send(null);
      if (request.status !== 206 && request.status !== 200) return null;
      var bytes = new Uint8Array(request.response);
      if (bytes.length === length) return bytes;
      // A server that ignores Range answers 200 with the whole file. Handing back a slice keeps
      // the port working against such a server instead of failing, at the cost of the transfer.
      if (request.status === 200 && bytes.length >= start + length) {
        return bytes.subarray(start, start + length);
      }
      return null;
    };
  },
});
`;

writeFileSync(preJsPath, libraryJs, 'utf8');
console.log('wrote krkr-range.js (linked with --js-library)');

/* ------------------------------------------------------------------ header */

let header = readFileSync(headerPath, 'utf8');
if (header.includes('tTVPHttpRangeStream')) {
  console.log('header already patched');
} else {
  const anchor = 'class tTVPLocalFileStream : public tTJSBinaryStream';
  if (!header.includes(anchor)) {
    console.error('header anchor not found');
    process.exit(1);
  }
  const declaration = `#if defined(__EMSCRIPTEN__) && defined(__EMSCRIPTEN_PTHREADS__)
//---------------------------------------------------------------------------
// Ranged-read stream, for game files served over HTTP.
//
// Mirrors tTVPLocalFileStream deliberately: the engine treats every storage stream the same way,
// so the smaller the difference in shape the less there is to get wrong. Only Seek and Read differ
// in what they do, not in what they promise.
//
// Guarded by the same condition the build uses to decide whether main runs on a pthread, because a
// blocking HTTP read is only legal there.
//---------------------------------------------------------------------------
class tTVPHttpRangeStream : public tTJSBinaryStream
{
private:
	std::string url;
	tjs_uint64 size;
	tjs_uint64 position;

public:
	tTVPHttpRangeStream(const ttstr &origname, const std::string &source_url, tjs_uint64 total_size);
	~tTVPHttpRangeStream();

	tjs_uint64 TJS_INTF_METHOD Seek(tjs_int64 offset, tjs_int whence);
	tjs_uint TJS_INTF_METHOD Read(void *buffer, tjs_uint read_size);
	tjs_uint TJS_INTF_METHOD Write(const void *buffer, tjs_uint write_size);

	void TJS_INTF_METHOD SetEndOfStorage();
	tjs_uint64 TJS_INTF_METHOD GetSize();
};

// Chooses a ranged stream for an HTTP-served archive, or returns null to fall through to the
// ordinary file path.
//
// Declared here, in the header StorageIntf.cpp already includes, because the decision has to be
// reachable from the platform-independent dispatch. An earlier attempt declared it in
// StorageIntf.cpp itself and the compiler still reported it undeclared, since the declaration
// landed after the only use.
tTJSBinaryStream *krkr_try_ranged_stream(const ttstr &name, tjs_uint32 flags);
#endif
//---------------------------------------------------------------------------

${anchor}`;
  writeFileSync(headerPath, header.replace(anchor, declaration), 'utf8');
  console.log('header: tTVPHttpRangeStream declared');
}

/* ------------------------------------------------------------------ source */

let source = readFileSync(sourcePath, 'utf8');

if (!source.includes('#include <emscripten.h>')) {
  const includeAnchor = '#include <psp2/io/dirent.h>';
  if (!source.includes(includeAnchor)) {
    console.error('include anchor not found');
    process.exit(1);
  }
  // No header is needed for the JavaScript now: the bridge is an ordinary extern "C" declaration.
  // <emscripten.h> is still included because the implementation calls emscripten_sleep while
  // waiting, which is what keeps the pthread responsive during a long ranged read.
  source = source.replace(
    includeAnchor,
    ['#if defined(__EMSCRIPTEN__)', '#include <emscripten.h>', '#endif', includeAnchor].join('\n'),
  );
  writeFileSync(sourcePath, source, 'utf8');
  console.log('source: included <emscripten.h>');
}

if (source.includes('tTVPHttpRangeStream::Read')) {
  console.log('source already patched');
} else {
  const implementation = [
    '#if defined(__EMSCRIPTEN__) && defined(__EMSCRIPTEN_PTHREADS__)',
    '//---------------------------------------------------------------------------',
    '// Ranged-read stream implementation',
    '//',
    '// The JavaScript lives in krkr-range.js and is linked with --pre-js, so nothing here has to',
    '// embed script inside a C string. C++ only declares what it calls.',
    '//---------------------------------------------------------------------------',
    'extern "C" int krkr_js_range_read(const char *url, double start, double length, void *dest);',
    'extern "C" int krkr_js_archive_info(char *urlBuffer, int urlCapacity);',
    'extern "C" void krkr_js_diag(const char *event);',
    '',
    '// Chooses a ranged stream for the archive, or returns null so the caller falls through.',
    '//',
    '// Kept separate from the stream itself: this is the decision, and the decision has to be',
    '// reachable from the platform-independent dispatch before it does its filesystem lookup.',
    'tTJSBinaryStream *krkr_try_ranged_stream(const ttstr &name, tjs_uint32 flags)',
    '{',
    '\t(void)flags;',
    '',
    '\t// Reports on entry, before any condition, so that "the function never runs" and "it runs and',
    '\t// returns null" are distinguishable. Both look identical from the outside otherwise — the',
    '\t// same trap as the server logging only range requests.',
    '\tkrkr_js_diag("selection-called");',
    '',
    '\tchar url[1024];',
    '\turl[0] = 0;',
    '\tconst int size = krkr_js_archive_info(url, (int)sizeof(url));',
    '\tif (size <= 0 || url[0] == 0)',
    '\t{',
    '\t\treturn NULL;',
    '\t}',
    '',
    '\t// Only the archive itself is ranged. Everything else — fonts, config, anything a game keeps',
    '\t// beside the archive — is small enough to load whole, and leaving those on the normal path',
    '\t// keeps the change narrow.',
    '\tstd::string requested;',
    '\tTVPUtf16ToUtf8(requested, name.AsStdString());',
    '\tif (requested.find("data.xp3") == std::string::npos)',
    '\t{',
    '\t\treturn NULL;',
    '\t}',
    '',
    '\treturn new tTVPHttpRangeStream(name, std::string(url), (tjs_uint64)size);',
    '}',
    '',
    'tTVPHttpRangeStream::tTVPHttpRangeStream(const ttstr &origname, const std::string &source_url,',
    '\ttjs_uint64 total_size)',
    '{',
    '\turl = source_url;',
    '\tsize = total_size;',
    '\tposition = 0;',
    '',
    '\tif (size == 0)',
    '\t{',
    '\t\tTVPThrowExceptionMessage(TVPCannotOpenStorage, origname);',
    '\t}',
    '',
    '\ttjs_uint32 tick = TVPGetRoughTickCount32();',
    '\tTVPPushEnvironNoise(&tick, sizeof(tick));',
    '}',
    '//---------------------------------------------------------------------------',
    'tTVPHttpRangeStream::~tTVPHttpRangeStream()',
    '{',
    '\ttjs_uint32 tick = TVPGetRoughTickCount32();',
    '\tTVPPushEnvironNoise(&tick, sizeof(tick));',
    '}',
    '//---------------------------------------------------------------------------',
    'tjs_uint64 TJS_INTF_METHOD tTVPHttpRangeStream::Seek(tjs_int64 offset, tjs_int whence)',
    '{',
    '\t// Resolved here rather than by the transport: the transport is stateless, so there is no',
    '\t// remote file pointer to move, only ranges to request.',
    '\tswitch(whence)',
    '\t{',
    '\tcase TJS_BS_SEEK_SET:\tposition = (tjs_uint64)offset;\t\t\t\t\tbreak;',
    '\tcase TJS_BS_SEEK_CUR:\tposition = (tjs_uint64)((tjs_int64)position + offset);\tbreak;',
    '\tcase TJS_BS_SEEK_END:\tposition = (tjs_uint64)((tjs_int64)size + offset);\t\tbreak;',
    '\tdefault:\t\t\t\tposition = (tjs_uint64)offset;\t\t\t\t\tbreak;',
    '\t}',
    '',
    '\tif ((tjs_int64)position < 0)',
    '\t{',
    '\t\tTVPThrowExceptionMessage(TVPSeekError);',
    '\t}',
    '\t// Seeking past the end is legal: the following read returns zero bytes, which is how the',
    '\t// callers detect end-of-file.',
    '\treturn position;',
    '}',
    '//---------------------------------------------------------------------------',
    'tjs_uint TJS_INTF_METHOD tTVPHttpRangeStream::Read(void *buffer, tjs_uint read_size)',
    '{',
    '\tif (read_size == 0) return 0;',
    '\tif (position >= size) return 0;',
    '',
    '\t// Clamped rather than treated as an error: a request running past the end is how the XP3',
    '\t// reader discovers the end of a region, so it has to succeed with the bytes that exist.',
    '\ttjs_uint64 available = size - position;',
    '\ttjs_uint wanted = read_size;',
    '\tif ((tjs_uint64)wanted > available) wanted = (tjs_uint)available;',
    '',
    '\tint result = krkr_js_range_read(url.c_str(), (double)position, (double)wanted, buffer);',
    '\tif (result != 0)',
    '\t{',
    '\t\tTVPThrowExceptionMessage(TVPCannotOpenStorage, ttstr(url.c_str()));',
    '\t}',
    '',
    '\tposition += wanted;',
    '\treturn wanted;',
    '}',
    '//---------------------------------------------------------------------------',
    'tjs_uint TJS_INTF_METHOD tTVPHttpRangeStream::Write(const void *buffer, tjs_uint write_size)',
    '{',
    '\t// The source is read-only; zero is what the engine treats as a failed write.',
    '\t(void)buffer;',
    '\t(void)write_size;',
    '\treturn 0;',
    '}',
    '//---------------------------------------------------------------------------',
    'void TJS_INTF_METHOD tTVPHttpRangeStream::SetEndOfStorage()',
    '{',
    '\tposition = size;',
    '}',
    '//---------------------------------------------------------------------------',
    'tjs_uint64 TJS_INTF_METHOD tTVPHttpRangeStream::GetSize()',
    '{',
    '\treturn size;',
    '}',
    '//---------------------------------------------------------------------------',
    '#endif',
    '',
    '',
  ].join('\n');

  const marker = 'tTVPLocalFileStream::tTVPLocalFileStream(';
  const at = source.indexOf(marker);
  if (at === -1) {
    console.error('source anchor not found');
    process.exit(1);
  }
  // Inserted at the start of the constructor's own line.
  //
  // An earlier version walked back over the preceding comment banner to place the code above it,
  // and that walk was off by one: it split the banner, producing
  //   // tTVPLocalFileStream#if defined(__EMSCRIPTEN_PTHREADS__)
  // which swallowed the #if into a comment and left the guarded code unguarded. Not navigating the
  // banner is both simpler and correct.
  const lineStart = source.lastIndexOf('\n', at);
  const insertAt = lineStart === -1 ? at : lineStart + 1;
  source = source.slice(0, insertAt) + implementation + source.slice(insertAt);
  writeFileSync(sourcePath, source, 'utf8');
  console.log('source: ranged stream implemented');
}

/* ------------------------------------------------- the selection in StorageIntf */

/*
  The ranged stream has to be chosen in `_TVPCreateStream`, not in the backend.

  `_TVPCreateStream` is platform-independent and begins by resolving the name through
  `TVPGetPlacedPath`, which performs a **real filesystem lookup**. The archive is never placed in
  the filesystem — that is the entire point of ranged loading — so `TVPGetPlacedPath` would fail and
  the call would throw `TVPCannotOpenStorage` before any backend was consulted. Interception
  therefore has to happen before that resolution.

  A first version of this patch added the class and never referenced it, so the build succeeded,
  linked, and produced **zero range requests**. That is the failure this interception fixes, and it
  is why the check below runs before the path lookup rather than after it.
*/
const intfPath = `${repo}/external/krkrz/base/StorageIntf.cpp`;
let intf = readFileSync(intfPath, 'utf8');

if (intf.includes('krkr_try_ranged_stream')) {
  console.log('StorageIntf already patched');
} else {
  // The declaration is placed at the top of StorageIntf.cpp itself.
  //
  // Putting it in `StorageImpl.h` did not work: `StorageIntf.cpp` does not include that header, so
  // the declaration never reached the file that uses it and the compiler still reported
  // "use of undeclared identifier". Declaring it in the translation unit that calls it removes the
  // dependency on include order entirely.
  const declAnchor = '#include "StorageIntf.h"';
  if (!intf.includes(declAnchor)) {
    console.error('StorageIntf anchor not found (#include "StorageIntf.h")');
    process.exit(1);
  }
  const decl = `${declAnchor}
#if defined(__EMSCRIPTEN__) && defined(__EMSCRIPTEN_PTHREADS__)
// Ranged-read selection, defined in the platform storage backend. Declared here because this file
// is the one that calls it, and depending on a header that is not included is how the previous
// attempt failed.
tTJSBinaryStream *krkr_try_ranged_stream(const ttstr &name, tjs_uint32 flags);
#endif`;
  intf = intf.replace(declAnchor, decl);

  // The interception itself, inserted immediately before `TVPGetPlacedPath` is called.
  //
  // Anchored on a **single line**, not a multi-line block: this file uses CRLF endings, so a block
  // written with `\n` never matches. That is what made two earlier attempts at this anchor report
  // "not found" while the text was visibly present.
  //
  // The line must also be the call rather than its surroundings, because insertion before the
  // lookup is the whole point: `TVPGetPlacedPath` performs a real filesystem lookup and would throw
  // for the archive, which is deliberately not in the filesystem.
  const bodyAnchor = 'name = TVPGetPlacedPath(_name); // file must exist';
  if (!intf.includes(bodyAnchor)) {
    console.error('StorageIntf anchor not found (TVPGetPlacedPath call)');
    process.exit(1);
  }
  const body = `#if defined(__EMSCRIPTEN__) && defined(__EMSCRIPTEN_PTHREADS__)
	// Ranged reads are tried before the path lookup below.
	//
	// TVPGetPlacedPath performs a real filesystem lookup and the archive is deliberately not in the
	// filesystem, so reaching it would throw TVPCannotOpenStorage before any backend was consulted.
	// Asking the ranged provider first is what makes an HTTP-served archive reachable at all.
	if(access != TJS_BS_WRITE)
	{
		tTJSBinaryStream *ranged = krkr_try_ranged_stream(_name, flags);
		if(ranged) return ranged;
	}
#endif
	${bodyAnchor}`;
  intf = intf.replace(bodyAnchor, body);
  writeFileSync(intfPath, intf, 'utf8');
  console.log('StorageIntf: ranged streams intercepted before the path lookup');
}

console.log('done');
