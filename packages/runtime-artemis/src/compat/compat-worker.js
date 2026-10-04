/* Classic worker. No import, no rAF, no setInterval. Protocol constants are duplicated; the test pins them.
 * worker-audio.js is concatenated above this line by the copy script. One file, so importScripts stays one URL. */
var COMPAT_WINDOW_BYTES = 4 * 1024 * 1024;
var COMPAT_CONTROL_BYTES = 24;
var CONTROL_SEQUENCE = 0;
var CONTROL_COUNT = 1;
var CONTROL_FILE = 2;
var CONTROL_OFFSET_LO = 3;
var CONTROL_OFFSET_HI = 4;
var CONTROL_EPOCH = 5;
var READ_MISSING = -1;
var READ_FATAL = -2;
var READ_TIMEOUT_MS = 30000;
var OPEN_PACK = '/game/root.pfs';
var OPEN_MODE = 'windows';
/* The pack's parent. `save_dir_` is `fs::path(pack).parent_path()` unless a host overrides it. */
var GAME_DIR = '/game';
/* The slot name this transport asks the engine for. Fixed, not a user path: the host keys the slot. */
var SAVE_SLOT = 'platform-slot.dat';
/* Mirrors `save_storage.cpp`'s `max_bytes`, plus the set-level bounds a per-file ceiling cannot state. */
var MAX_FILE_BYTES = 64 * 1024 * 1024;
var MAX_FILES = 4096;
var MAX_TOTAL_BYTES = 256 * 1024 * 1024;
/* The engine's own checkpoint format, from `EncodeVariableBank(_, true, _)` in save_storage.cpp. */
var SLOT_HEADER_BYTES = 16;
var SLOT_MAX_BANK_BYTES = 64 * 1024 * 1024;
var SLOT_COUNT_LIMIT = 65536;
var SLOT_KEY_LIMIT = 1024 * 1024;
var SLOT_VALUE_LIMIT = 16 * 1024 * 1024;

/*
 * Every native call goes through Module.cwrap.
 *
 * Keys are the Module property names (leading underscore included). Emscripten's
 * `getCFunc(ident)` reads `Module['_' + ident]`, so the ident handed to cwrap is the
 * name with that one underscore removed — otherwise it looks up `Module['__artc_web_*']`.
 *
 * Signatures are the native ones verbatim: `artc_web_key(int key, int down)` takes two
 * numbers, not a C string. `artc_web_pointer(x, y, down)` takes two floats; the host
 * passes CSS pixels through untouched, so they are never bitwise-truncated.
 */
var CABI = {
  _artc_web_open: { ret: 'number', args: ['string', 'string'] },
  _artc_web_frame: { ret: 'number', args: [] },
  _artc_web_pause: { ret: 'number', args: ['number'] },
  _artc_web_dispose: { ret: null, args: [] },
  _artc_web_save: { ret: 'number', args: ['string'] },
  _artc_web_load: { ret: 'number', args: ['string'] },
  _artc_web_now_ms: { ret: 'number', args: [] },
  _artc_web_pointer: { ret: null, args: ['number', 'number', 'number'] },
  _artc_web_pointer_move: { ret: null, args: ['number', 'number'] },
  _artc_web_key: { ret: null, args: ['number', 'number'] },
  _artc_web_resize: { ret: null, args: ['number', 'number'] },
  _artc_web_last_error: { ret: 'number', args: [] },
  /* Read-only reporting. These are how the page learns what the engine actually did; none of
   * them change state, so an absent one degrades to "unverified", never to an invented number. */
  _artc_web_started: { ret: 'number', args: [] },
  _artc_web_presented: { ret: 'number', args: [] },
  _artc_web_revision: { ret: 'number', args: [] },
  _artc_web_stage_width: { ret: 'number', args: [] },
  _artc_web_stage_height: { ret: 'number', args: [] },
};

var moduleRef = null;
var cabi = {};
var audioBridge = null;
var ready = false;
var importing = false;
var opened = false;
var disposed = false;
var fatal = null;
var block = null;
var windowBytes = COMPAT_WINDOW_BYTES;
var descriptors = [];
var wasmUrl = '';
var readTimeoutMs = READ_TIMEOUT_MS;
var pendingInit = null;
/* Saved files the page read from its own store, installed before `artc_web_open` so a cold boot sees
 * the bank the previous session left. Validated in `handleInit`, before the glue is even loaded. */
var initialFiles = [];

self.Module = {
  canvas: null,
  noInitialRun: true,
  locateFile: function (path) {
    return /\.wasm$/.test(path) ? wasmUrl : path;
  },
  /* Game output reaches the page instead of the worker's console, which nobody reads. */
  print: function (line) {
    post({ type: 'log', level: 'info', line: line });
  },
  printErr: function (line) {
    post({ type: 'log', level: 'error', line: line });
  },
  onRuntimeInitialized: function () {
    ready = true;
    /* A synchronous onRuntimeInitialized fires inside importScripts; handleInit settles after it. */
    if (!importing) settle();
  },
};

function post(message) {
  self.postMessage(message);
}

function text(error) {
  return error && error.message ? error.message : String(error);
}

function refuse(id, reason) {
  post({ type: 'error', id: id, reason: reason });
}

/** Terminal for this worker. The page owns the teardown; the engine just stops being driven. */
function die(reason) {
  if (!fatal) {
    fatal = reason;
    post({ type: 'fatal', id: 0, reason: reason });
  }
}

function installStubs() {
  /* Only a prototype holder so glue can read WebGLRenderingContext.prototype. No renderer is faked. */
  if (typeof WebGLRenderingContext === 'undefined') {
    self.WebGLRenderingContext = function WebGLRenderingContext() {};
  }
  var canvas = self.Module.canvas;
  self.document = {
    querySelector: function (selector) {
      return selector === '#canvas' ? canvas : null;
    },
  };
}

function bindCabi() {
  if (!moduleRef || typeof moduleRef.cwrap !== 'function') return false;
  for (var name in CABI) {
    if (typeof moduleRef[name] !== 'function') continue;
    /* The Module property keeps its underscore; the C ident cwrap wants is that name minus one. */
    cabi[name] = moduleRef.cwrap(name.slice(1), CABI[name].ret, CABI[name].args, { async: false });
  }
  return true;
}

function lastError() {
  try {
    var read = cabi._artc_web_last_error;
    if (!read || typeof moduleRef.UTF8ToString !== 'function') return '';
    return moduleRef.UTF8ToString(read()) || '';
  } catch (error) {
    return '';
  }
}

/**
 * One native counter, or `undefined` when the build does not export it.
 *
 * `undefined` is deliberate and load-bearing: it serializes to an absent field, which the page
 * reports as unverified. Returning 0 would be indistinguishable from an engine that genuinely
 * presented nothing — the two are different findings and must not collapse.
 */
function readCounter(name) {
  var read = cabi[name];
  if (typeof read !== 'function') return undefined;
  try {
    return read() | 0;
  } catch (error) {
    return undefined;
  }
}

/** `artc_web_now_ms` is a double and is NaN before the engine starts. Absent, not NaN. */
function readNowMs() {
  var read = cabi._artc_web_now_ms;
  if (typeof read !== 'function') return undefined;
  try {
    var value = read();
    return typeof value === 'number' && isFinite(value) ? value : undefined;
  } catch (error) {
    return undefined;
  }
}

/** One blocking window exchange. SEQUENCE is published before the post, so a fast answer cannot be lost. */
function exchange(fileId, offset, length) {
  var sequence = (Atomics.load(block.control, CONTROL_SEQUENCE) + 1) | 0;
  var epoch = (Atomics.add(block.control, CONTROL_EPOCH, 1) + 1) | 0;
  Atomics.store(block.control, CONTROL_FILE, fileId | 0);
  Atomics.store(block.control, CONTROL_OFFSET_LO, offset >>> 0);
  Atomics.store(block.control, CONTROL_OFFSET_HI, Math.floor(offset / 0x100000000) >>> 0);
  Atomics.store(block.control, CONTROL_COUNT, 0);
  Atomics.store(block.control, CONTROL_SEQUENCE, sequence);
  post({ type: 'read', fileId: fileId, offset: offset, length: length, sequence: sequence, epoch: epoch });
  var waited = Atomics.wait(block.control, CONTROL_SEQUENCE, sequence, readTimeoutMs);
  if (waited === 'timed-out') {
    /* Bump the epoch so the late answer cannot land in the window, then refuse to continue. */
    Atomics.add(block.control, CONTROL_EPOCH, 1);
    die('read timed out after ' + readTimeoutMs + 'ms');
    return READ_FATAL;
  }
  if (disposed || fatal) return READ_FATAL;
  return Atomics.load(block.control, CONTROL_COUNT) | 0;
}

function fileIdFor(path) {
  for (var i = 0; i < descriptors.length; i++) {
    if (descriptors[i].virtualPath === path) return descriptors[i].id;
  }
  return -1;
}

function installRead() {
  moduleRef.artcReadRange = function (path, offset, dest, length) {
    try {
      if (!block || disposed || fatal) return -1;
      var id = fileIdFor(path);
      if (id < 0 || !(length > 0) || !Number.isSafeInteger(offset) || offset < 0) return -1;
      /* A request larger than the window is split; it is never refused for being large. */
      var done = 0;
      while (done < length) {
        var piece = length - done > windowBytes ? windowBytes : length - done;
        var status = exchange(id, offset + done, piece);
        if (status !== piece) return -1;
        moduleRef.HEAPU8.set(block.payload.subarray(0, piece), (dest + done) >>> 0);
        done += piece;
      }
      return done;
    } catch (error) {
      die(text(error));
      return -1;
    }
  };
}

/**
 * One input event to native. Returns how many native calls it reached — 0 means
 * nothing was called, never "called and ignored".
 *
 * Key values are the engine's own normalised integers (`kKeyTap = 1`,
 * `web_main.cpp:75`), not DOM strings: the native signature is
 * `artc_web_key(int key, int down)`, so a string key has no place on this wire.
 * Pointer coordinates are CSS pixels and stay float; the native side takes
 * `float x, float y`, so truncating them here would move the click.
 */
function deliver(event) {
  if (!opened || disposed || fatal) return 0;
  var count = 0;
  try {
    if (event.kind === 'key') {
      if (cabi._artc_web_key) {
        cabi._artc_web_key(event.code | 0, event.down ? 1 : 0);
        count++;
      }
    } else if (event.kind === 'pointer-move') {
      if (cabi._artc_web_pointer_move) {
        cabi._artc_web_pointer_move(event.x, event.y);
        count++;
      }
    } else if (event.kind === 'resize') {
      if (cabi._artc_web_resize) {
        cabi._artc_web_resize(event.width | 0, event.height | 0);
        count++;
      }
    } else if (event.kind === 'pointer' || event.kind === 'input') {
      /* `input` is the pre-freeze name for the same press/release event. */
      if (cabi._artc_web_pointer) {
        cabi._artc_web_pointer(event.x, event.y, event.down ? 1 : 0);
        count++;
      }
    }
  } catch (error) {
    die(text(error));
  }
  return count;
}

/**
 * The engine writes its save slot beside the pack, i.e. under `/game/`.
 *
 * Nothing in the engine creates that directory: `ReadSaveFile`/`WriteSaveFile` are plain
 * `ifstream`/`open` on a path, and Emscripten's FS has no `/game` unless someone makes it — the
 * pack never needed one, because `artcReadRange` supplies pack bytes and no filesystem open
 * happens. So the first save against a fresh MEMFS fails with ENOENT and reads as "cannot save"
 * rather than "no directory". One `mkdirTree` before open is the whole fix.
 *
 * The directory is created through `Module.FS`, i.e. after the glue has run; MEMFS warns about
 * links when `-sWASM_BIGINT`-style flags are absent, which is cosmetic and not a failure.
 */
function ensureGameDirectory() {
  try {
    var FS = moduleRef && moduleRef.FS;
    if (FS && typeof FS.mkdirTree === 'function') {
      FS.mkdirTree(GAME_DIR);
      return true;
    }
    if (FS && typeof FS.mkdir === 'function' && !FS.analyzePath(GAME_DIR).exists) {
      FS.mkdir(GAME_DIR);
      return true;
    }
    return false;
  } catch (error) {
    /* A directory that could not be made is reported, not swallowed: the load proceeds and the
     * first save will fail loudly, which is better than refusing a session that may not save. */
    post({ type: 'log', level: 'error', line: 'could not create ' + GAME_DIR + ': ' + text(error) });
    return false;
  }
}

function settle() {
  if (!pendingInit || !ready || opened) return;
  var request = pendingInit;
  pendingInit = null;
  if (!bindCabi()) {
    post({ type: 'init-failed', id: request.id, reason: 'generated glue has no Module.cwrap' });
    return;
  }
  installRead();
  if (typeof self.createArtcAudioBridge === 'function') audioBridge = self.createArtcAudioBridge(moduleRef);
  ensureGameDirectory();
  if (!cabi._artc_web_open) {
    post({ type: 'init-failed', id: request.id, reason: 'missing _artc_web_open' });
    return;
  }
  /* The saved files go in **before** the engine opens, not after its Lua framework has booted. A
   * `system.dat` installed later is a file the engine has already decided it does not have, so the
   * cold start would read globals from nothing. `handleInit` validated the set before the glue was even
   * loaded, so this loop writes and cannot refuse on a payload. */
  try {
    if (initialFiles.length > 0) installFiles(initialFiles);
  } catch (error) {
    post({ type: 'init-failed', id: request.id, reason: 'cannot install the saved files: ' + text(error) });
    return;
  }
  var code = 0;
  try {
    code = cabi._artc_web_open(OPEN_PACK, OPEN_MODE) | 0;
  } catch (error) {
    post({ type: 'init-failed', id: request.id, reason: text(error) });
    return;
  }
  if (!code) {
    post({ type: 'init-failed', id: request.id, reason: 'open returned 0: ' + lastError() });
    return;
  }
  if (fatal) {
    /* A read died inside open. Never announce a session that cannot read. */
    post({ type: 'init-failed', id: request.id, reason: fatal });
    return;
  }
  opened = true;
  post({ type: 'ready', id: request.id });
}

function handleInit(request) {
  wasmUrl = request.wasmUrl;
  descriptors = request.descriptors || [];
  /* Validated here, before the glue is imported and therefore before any filesystem this worker owns
   * exists: a bad saved set must fail the load, not install half of itself into `/game`. No slot is
   * required — a banks-only set is a legitimate cold start. */
  try {
    var saved = request.initialFiles;
    if (saved === undefined || saved === null) saved = [];
    if (!Array.isArray(saved)) throw new Error('the saved file set is not an array');
    /* An empty set is the honest "this game has never been saved" answer, so it is not a refusal. */
    initialFiles = saved.length === 0 ? [] : planFiles(saved, 0);
  } catch (error) {
    post({ type: 'init-failed', id: request.id, reason: text(error) });
    return;
  }
  if (typeof request.readTimeoutMs === 'number' && request.readTimeoutMs > 0) readTimeoutMs = request.readTimeoutMs;
  block = {
    control: new Int32Array(request.block, 0, COMPAT_CONTROL_BYTES / 4),
    payload: new Uint8Array(request.block, COMPAT_CONTROL_BYTES),
  };
  /* The page allocates the window, so the page decides its size. Never assume the default here. */
  windowBytes = block.payload.length;
  if (!(windowBytes > 0)) {
    post({ type: 'init-failed', id: request.id, reason: 'shared window is empty' });
    return;
  }
  /* Canvas, ready hook, document#canvas and the WebGL prototype must all exist before the glue runs. */
  self.Module.canvas = request.canvas || null;
  /* The copy script concatenates the helper, so this is a no-op there. A raw source load needs the URL. */
  if (typeof self.createArtcAudioBridge !== 'function' && request.audioHelperUrl) importScripts(request.audioHelperUrl);
  installStubs();
  moduleRef = self.Module;
  pendingInit = request;
  importing = true;
  try {
    importScripts(request.engineJsUrl);
  } catch (error) {
    importing = false;
    pendingInit = null;
    post({ type: 'init-failed', id: request.id, reason: text(error) });
    return;
  }
  importing = false;
  if (ready) settle();
}

/**
 * A restore path, or null when it is not one this worker will write.
 *
 * The page validates the same shape before it calls, and this repeats it deliberately: a path from the
 * wire is what `FS.writeFile` is handed, so the worker must not depend on a caller having checked. An
 * absolute path, a drive letter, a backslash or a `..` segment is a write outside the save directory.
 */
function restorePath(path) {
  if (typeof path !== 'string' || !path || path.indexOf('\0') >= 0) return null;
  if (path.charAt(0) === '/' || path.charAt(0) === '\\') return null;
  if (/^[A-Za-z]:/.test(path) || path.indexOf(':') >= 0) return null;
  if (path.indexOf('\\') >= 0 || path.indexOf('//') >= 0) return null;
  if (/\.pfs(\.\d{1,9})?$/i.test(path)) return null;
  var parts = path.split('/');
  for (var i = 0; i < parts.length; i++) {
    if (!parts[i] || parts[i] === '.' || parts[i] === '..') return null;
  }
  return path;
}

/** One FS path as a plain relative name, or null when it is not one this worker will handle. */
function relativePath(dir, full) {
  if (typeof full !== 'string' || full.indexOf(dir + '/') !== 0) return null;
  var rest = full.slice(dir.length + 1);
  if (!rest || rest.charAt(0) === '/' || rest.indexOf('\\') >= 0 || rest.indexOf('//') >= 0) return null;
  var parts = rest.split('/');
  for (var i = 0; i < parts.length; i++) {
    if (!parts[i] || parts[i] === '.' || parts[i] === '..') return null;
  }
  return rest;
}

/**
 * CRC32, the same polynomial `save_storage.cpp` gets from zlib.
 *
 * A copy of the constants and the TS `slotPayloadError` are both pinned by the test, because the two
 * halves run in different languages over the same bytes and must agree on the answer.
 */
function crcOf(bytes, end) {
  var crc = 0xffffffff;
  for (var i = 0; i < end; i++) {
    crc ^= bytes[i];
    for (var bit = 0; bit < 8; bit++) crc = (crc >>> 1) ^ (0xedb88320 & -(crc & 1));
  }
  return (crc ^ 0xffffffff) >>> 0;
}

/**
 * The engine's own checkpoint format, read all the way to the end of the bank.
 *
 * The decode is a loop over attacker-controlled lengths, so it is **allocation-free**: every length is
 * checked against the remaining bytes before it is believed, and the walk is finished before any byte
 * is moved out of the payload. A decoder that grows as it reads is the one that can be walked past the
 * end of its own input, leaving a half-written bank behind — this one either returns a range or returns
 * nothing at all. `''` means this *is* a well-formed checkpoint; anything else is why it is not.
 */
function slotPayloadError(bytes) {
  if (!bytes || bytes.length < SLOT_HEADER_BYTES) return 'it is too short to be an ARCV checkpoint';
  if (bytes.length > MAX_FILE_BYTES) return 'it is past the 64 MiB checkpoint bound';
  if (bytes[0] !== 0x41 || bytes[1] !== 0x52 || bytes[2] !== 0x43 || bytes[3] !== 0x56) {
    return 'it does not begin with the ARCV magic';
  }
  var view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  if (view.getUint32(4, true) !== 1) return 'it is not ARCV version 1';
  var end = bytes.length - 4;
  if (view.getUint32(end, true) !== crcOf(bytes, end)) return 'its CRC32 does not match its bytes';
  var count = view.getUint32(8, true);
  if (count > SLOT_COUNT_LIMIT) return 'its bank declares more than 65536 entries';
  var keys = new Set();
  var pos = 12;
  for (var i = 0; i < count; i++) {
    if (end - pos < 4) return 'a bank length runs past the end of the checkpoint';
    var keyLength = view.getUint32(pos, true);
    pos += 4;
    if (keyLength > SLOT_KEY_LIMIT || keyLength > end - pos) return 'a bank entry runs past the end of the checkpoint';
    var key = '';
    for (var k = 0; k < keyLength; k++) key += String.fromCharCode(bytes[pos + k]);
    /* `Set`, not an object literal: a bank is attacker-controlled, and a key named `constructor` or
     * `toString` would read as already-seen through the prototype chain. The TS copy uses a Set too. */
    if (keys.has(key)) return 'the bank repeats a key';
    keys.add(key);
    pos += keyLength;
    if (end - pos < 4) return 'a bank length runs past the end of the checkpoint';
    var valueLength = view.getUint32(pos, true);
    pos += 4;
    if (valueLength > SLOT_VALUE_LIMIT || valueLength > end - pos) return 'a bank entry runs past the end of the checkpoint';
    pos += valueLength;
  }
  if (pos !== end) return 'the bank does not end where the checkpoint says it does';
  return '';
}

/**
 * Every file this worker let the engine create, as base64.
 *
 * `save_storage.cpp` writes the slot beside the pack, and the engine also keeps `system.dat` (the
 * global bank) and `saveg.dat` there — so the whole *created* tree is captured, not the one slot file.
 * The pack never lives here: pack bytes reach the engine through `artcReadRange`, and this walk
 * refuses a `.pfs` name outright, so a source file can never be mistaken for a save.
 *
 * A file the page could not read is a **refusal**, never a silent omission: a checkpoint that quietly
 * dropped one file restores a session that never existed.
 */
function captureFiles() {
  var FS = moduleRef && moduleRef.FS;
  if (!FS || typeof FS.readdir !== 'function' || typeof FS.readFile !== 'function') {
    throw new Error('the glue does not export FS; the save directory cannot be read');
  }
  var out = [];
  var total = 0;
  var stack = [GAME_DIR];
  while (stack.length > 0) {
    var dir = stack.pop();
    /* A directory that cannot be listed is a **refusal**, not an empty listing. Silently skipping it
     * is how a capture quietly drops a file: the player gets a slot that restores a different session,
     * and the only sign is a scenario that starts a chapter behind. */
    var entries = FS.readdir(dir);
    for (var i = 0; i < entries.length; i++) {
      var name = entries[i];
      if (name === '.' || name === '..') continue;
      var full = dir + '/' + name;
      var flags = 0;
      try {
        flags = FS.stat(full).mode;
      } catch (error) {
        flags = 0;
      }
      if ((flags & 16384) === 16384) {
        stack.push(full);
        continue;
      }
      var relative = relativePath(GAME_DIR, full);
      if (!relative) continue;
      /* The slot this worker moved aside for the duration of a save is its own scaffolding, not game
       * output: capturing it would persist the previous checkpoint beside this one, and every save
       * after the first would carry a growing pile of them. */
      if (relative === asideName()) continue;
      /* `/game` also hosts the pack descriptors. A `.pfs` here would be the game itself, and copying
       * it into a save slot is the one mistake that would silently turn a slot into a game dump. */
      if (/\.pfs(\.\d{1,9})?$/i.test(relative)) continue;
      var bytes;
      try {
        bytes = FS.readFile(full, { encoding: 'binary' });
      } catch (error) {
        throw new Error('cannot read ' + relative + ': ' + text(error));
      }
      if (bytes.length > MAX_FILE_BYTES) throw new Error(relative + ' is past the 64 MiB save bound');
      total += bytes.length;
      if (total > MAX_TOTAL_BYTES) throw new Error('the checkpoint is past the 256 MiB save bound');
      out.push({ path: relative, data: base64FromBytes(bytes) });
      if (out.length > MAX_FILES) throw new Error('the checkpoint is past the 4096-file save bound');
    }
  }
  return out;
}

function base64FromBytes(bytes) {
  var alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';
  var out = '';
  for (var i = 0; i < bytes.length; i += 3) {
    var b0 = bytes[i];
    var b1 = i + 1 < bytes.length ? bytes[i + 1] : 0;
    var b2 = i + 2 < bytes.length ? bytes[i + 2] : 0;
    out += alphabet.charAt(b0 >> 2);
    out += alphabet.charAt(((b0 & 3) << 4) | (b1 >> 4));
    out += i + 1 < bytes.length ? alphabet.charAt(((b1 & 15) << 2) | (b2 >> 6)) : '=';
    out += i + 2 < bytes.length ? alphabet.charAt(b2 & 63) : '=';
  }
  return out;
}

/**
 * Base64 to bytes, refusing anything that is not canonical.
 *
 * A payload that reached this worker as a string is what `FS.writeFile` is handed, so the length, the
 * padding and the unused bits all have to be exactly what the encoder would have produced. A loose
 * decoder writes *different* bytes than the checkpoint held, which is the one failure a restore cannot
 * report afterwards. The length is checked **before** the output is allocated, so a hostile string
 * costs a comparison rather than a 48 MiB allocation.
 */
function base64ToBytesSafe(data) {
  var alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';
  if (typeof data !== 'string') throw new Error('a restored file is neither bytes nor base64');
  /* Canonical or refused: a length that is not a multiple of four, more than two pad characters, or a
   * body one digit short of a full group, all mean this is not what the encoder would have written.
   * The page's own decoder refuses the same three, and the two must agree — a half of the wire that
   * is looser decodes bytes the other half would never send. */
  if (data.length % 4 !== 0) throw new Error('a restored file is not valid base64');
  var body = data.replace(/=+$/, '');
  if (data.length - body.length > 2) throw new Error('a restored file is not valid base64');
  if (body.length % 4 === 1) throw new Error('a restored file is not valid base64');
  var size = Math.floor((body.length * 3) / 4);
  if (size > MAX_FILE_BYTES) throw new Error('a restored file is past the 64 MiB save bound');
  var out = new Uint8Array(size);
  var bits = 0;
  var value = 0;
  var index = 0;
  for (var i = 0; i < body.length; i++) {
    var digit = alphabet.indexOf(body.charAt(i));
    if (digit < 0) throw new Error('a restored file is not valid base64');
    value = (value << 6) | digit;
    bits += 6;
    if (bits >= 8) {
      bits -= 8;
      out[index] = (value >>> bits) & 0xff;
      index++;
    }
  }
  /* Two or three leftover digits carry bits that no byte uses; a non-zero one is a different encoding
   * of the same bytes, and accepting it means the decoded length is a guess rather than a fact. */
  if (bits > 0 && (value & ((1 << bits) - 1)) !== 0) throw new Error('a restored file is not valid base64');
  return index === size ? out : out.subarray(0, index);
}

/**
 * Restore payload bytes, as a `Uint8Array` this worker owns.
 *
 * The page sends **binary**: `postMessage` structured-clones a `Uint8Array` without the 33% expansion
 * base64 costs. That is also why the bytes are copied — the page validates the same `SharedArrayBuffer`
 * the frame loop writes to, and `FS.writeFile` here is a deferred write. Handing MEMFS a view of the
 * caller's memory means the bytes it later reads back are whatever native wrote there in between. A
 * shared buffer is refused outright rather than copied: it is a live window, not a file.
 */
function restoreBytes(data) {
  if (data instanceof Uint8Array) {
    if (typeof SharedArrayBuffer === 'function' && data.buffer instanceof SharedArrayBuffer) {
      throw new Error('a restored file shares memory with the frame window');
    }
    /* The size is checked against the bound before the copy, not after: allocating a copy of an
     * oversized payload first would be a 64 MiB allocation bought to be thrown away. */
    if (data.byteLength > MAX_FILE_BYTES) throw new Error('a restored file is past the 64 MiB save bound');
    return new Uint8Array(data);
  }
  if (typeof data === 'string') return base64ToBytesSafe(data);
  throw new Error('a restored file is neither bytes nor base64');
}

/**
 * Every entry of a file set, checked before a single byte is written.
 *
 * One pass, one answer: the caller either gets the whole plan or an exception naming the refusal, so
 * the install loop below cannot refuse halfway and leave a half-applied tree behind a running guest.
 * A restore requires the slot — `artc_web_load` reads one fixed file by name, so a set without it
 * restores nothing — while a hydration set does not: a session closed before it ever saved one still
 * has banks worth hydrating, and those are plain files, not a checkpoint.
 */
function planFiles(incoming, requireSlot) {
  if (!Array.isArray(incoming)) throw new Error('the file set is not an array');
  if (incoming.length === 0) throw new Error('the file set carried no files');
  if (incoming.length > MAX_FILES) throw new Error('the file set carries more files than a save can: ' + incoming.length);
  var seen = new Set();
  var plan = [];
  var slot = 0;
  var total = 0;
  for (var v = 0; v < incoming.length; v++) {
    var entry = incoming[v];
    var path = restorePath(entry && entry.path);
    if (!path) throw new Error('refused a restore path outside the save directory');
    var folded = path.toLowerCase();
    if (seen.has(folded)) throw new Error('the restore lists ' + path + ' twice');
    seen.add(folded);
    var bytes;
    try {
      bytes = restoreBytes(entry.data);
    } catch (error) {
      throw new Error('refused ' + path + ': ' + text(error));
    }
    if (bytes.length > MAX_FILE_BYTES) throw new Error('refused ' + path + ': past the 64 MiB save bound');
    total += bytes.length;
    if (total > MAX_TOTAL_BYTES) throw new Error('refused the restore: past the 256 MiB save bound');
    if (path === SAVE_SLOT) {
      /* Native's `DecodeVariableBank` would refuse these bytes anyway, but it does so *after* the old
       * slot has been overwritten. This is the same check, made while the running session is intact. */
      if (requireSlot) {
        var malformed = slotPayloadError(bytes);
        if (malformed) throw new Error('refused ' + path + ': ' + malformed);
      }
      slot = 1;
    }
    plan.push({ path: path, data: bytes });
  }
  if (requireSlot && !slot) throw new Error('the restore does not carry ' + SAVE_SLOT);
  return plan;
}

/**
 * Writes a validated plan into the engine's save directory, creating the directories it needs.
 *
 * Cannot refuse: every check ran in `planFiles`, and a throw here is a filesystem fault the caller
 * reports as a failure. The names it returns are what a rollback has to remove.
 */
function installFiles(plan) {
  var FS = moduleRef.FS;
  if (FS.mkdirTree) FS.mkdirTree(GAME_DIR);
  var installed = [];
  for (var i = 0; i < plan.length; i++) {
    var target = GAME_DIR + '/' + plan[i].path;
    var slash = target.lastIndexOf('/');
    if (slash > 0 && FS.mkdirTree) FS.mkdirTree(target.slice(0, slash));
    FS.writeFile(target, plan[i].data);
    installed.push(plan[i].path);
  }
  return installed;
}

/**
 * One checkpoint, in the only order the native API allows.
 *
 * `artc_web_save` is synchronous and lives on this thread, so the page cannot call it; and it returns
 * `1` for "the engine accepted the slot", which is **not** the same as "the bytes are usable".
 *
 * It is not even the same as "bytes were written". The engine writes the slot beside the pack and then
 * `SaveSystemData()` decides the return code, a `system.dat` failure answers 0 **with the slot already
 * on disk**, and a slot from an earlier run is still there when this one fails. Reporting either of
 * those is the fabricated checkpoint this function exists to prevent, so the old slot is moved aside
 * first: what is read back afterwards can only be what this run wrote. That is also why the result is
 * never compared against the old bytes — a rewritten slot that happens to be identical is still a
 * save the engine made, and equality is not evidence of anything.
 */
function handleCheckpoint(request) {
  if (!cabi._artc_web_save) {
    refuse(request.id, 'save is not implemented');
    return;
  }
  /* Not 1 means the engine refused: a missing onSave handler, a path that escapes saveDir, or a write
   * that failed. Reporting files here would fabricate a checkpoint the engine never made. */
  var moved = 0;
  try {
    moved = moveAside(SAVE_SLOT);
  } catch (error) {
    refuse(request.id, 'the previous slot could not be moved aside: ' + text(error));
    return;
  }
  var code = 0;
  try {
    code = cabi._artc_web_save(SAVE_SLOT) | 0;
  } catch (error) {
    restoreAside(moved);
    die(text(error));
    refuse(request.id, fatal || text(error));
    return;
  }
  if (code !== 1) {
    restoreAside(moved);
    refuse(request.id, 'artc_web_save returned ' + code + ': ' + lastError());
    return;
  }
  var slot;
  try {
    slot = readSaveBytes(SAVE_SLOT);
  } catch (error) {
    restoreAside(moved);
    refuse(request.id, text(error));
    return;
  }
  if (!slot || slot.length === 0) {
    restoreAside(moved);
    refuse(request.id, 'the engine acknowledged the slot but wrote no file');
    return;
  }
  /* The engine's own format or nothing. A `1` plus a byte-identical leftover file, an empty file or a
   * file native wrote half of all look like a checkpoint here, and only the format tells them apart. */
  var malformed = slotPayloadError(slot);
  if (malformed) {
    restoreAside(moved);
    refuse(request.id, 'the engine returned 1 but the slot it wrote is not a checkpoint: ' + malformed);
    return;
  }
  var files;
  try {
    files = captureFiles();
  } catch (error) {
    restoreAside(moved);
    refuse(request.id, text(error));
    return;
  }
  var carried = false;
  for (var i = 0; i < files.length; i++) if (files[i].path === SAVE_SLOT) carried = true;
  if (!carried) {
    restoreAside(moved);
    refuse(request.id, 'the save directory no longer holds ' + SAVE_SLOT + ', so this is not a checkpoint');
    return;
  }
  dropAside();
  post({ type: 'checkpoint-ok', id: request.id, files: files });
}

/**
 * The previous slot under a name the capture will not pick up, so its absence from the read-back is
 * unambiguous. The suffix is the worker's own; anything already under it is game output, not a slot.
 */
function asideName() {
  return SAVE_SLOT + '.pre-save';
}

/**
 * Move the old slot out of the way, or answer 0 when there was none.
 *
 * A rename rather than a copy: the old bytes are the player's last checkpoint, so they are kept until
 * this one has been read back and dropped only once it has.
 */
function moveAside(name) {
  var FS = moduleRef && moduleRef.FS;
  if (!FS || typeof FS.rename !== 'function') throw new Error('the glue does not export FS.rename');
  if (!FS.analyzePath(GAME_DIR + '/' + name).exists) return 0;
  if (FS.analyzePath(GAME_DIR + '/' + asideName()).exists) FS.unlink(GAME_DIR + '/' + asideName());
  FS.rename(GAME_DIR + '/' + name, GAME_DIR + '/' + asideName());
  return 1;
}

/** Put a moved-aside slot back after a refusal, so a failed checkpoint leaves the directory as it was. */
function restoreAside(moved) {
  if (!moved) return;
  try {
    moduleRef.FS.rename(GAME_DIR + '/' + asideName(), GAME_DIR + '/' + SAVE_SLOT);
  } catch (error) {
    die('the previous slot could not be put back: ' + text(error));
  }
}

/** Drop the kept-aside slot: the checkpoint it was replaced by is on the wire and readable. */
function dropAside() {
  try {
    if (moduleRef.FS.analyzePath(GAME_DIR + '/' + asideName()).exists) moduleRef.FS.unlink(GAME_DIR + '/' + asideName());
  } catch (error) {
    /* The new checkpoint is already read back; a stale `.pre-save` is a leak, not a wrong answer. */
  }
}

/** One file out of the engine's save directory, or null when it is not there. */
function readSaveBytes(name) {
  var FS = moduleRef && moduleRef.FS;
  if (!FS || typeof FS.readFile !== 'function') throw new Error('the glue does not export FS');
  var full = GAME_DIR + '/' + name;
  if (typeof FS.analyzePath === 'function') {
    var probe = FS.analyzePath(full);
    if (probe && 'exists' in probe ? !probe.exists : !probe) return null;
  }
  var bytes;
  try {
    bytes = FS.readFile(full, { encoding: 'binary' });
  } catch (error) {
    return null;
  }
  return bytes;
}

/** The file's size, or null when the engine is not holding one. Never a read: a big slot is not decoded. */
/**
 * One restore: park the guest, validate everything, back the old files up, install, load, put the run
 * state back, and roll back on a refusal.
 *
 * Order matters in both directions. The frame loop is stopped and native is paused **before** any
 * filesystem mutation, so the engine is not running while its own save directory changes — and that
 * same pause is the session's recovery point: `artc_web_load` runs on the worker's thread inside it,
 * so nothing can drive a frame through the half-applied tree. A pause that answers 0 (or throws) is a
 * refusal before anything is touched, because the engine is then in an unknown run state and the files
 * are still the running session's. The old files are kept in memory until the load is acknowledged: a
 * `0` from `artc_web_load` leaves a half-applied filesystem, and putting the previous bytes back is the
 * only recovery available here.
 *
 * A successful load is **not** a state the host asked for. `request.paused` is the pause intent the
 * page held when it asked for this restore, so the engine goes back to exactly that: paused sessions
 * stay paused (re-asserting a pause that is already held is idempotent, and skipping it would hand a
 * player who paused the game into it), running ones are told to run, and that answer must be 1 — a
 * session left stopped is a frozen canvas. A restore that cannot be rolled back ends the worker,
 * because continuing from an unknown state is worse than stopping.
 */
function handleRestore(request) {
  if (!cabi._artc_web_load) {
    refuse(request.id, 'load is not implemented');
    return;
  }
  var FS = moduleRef && moduleRef.FS;
  if (!FS || typeof FS.writeFile !== 'function') {
    refuse(request.id, 'the glue does not export FS; the save directory cannot be written');
    return;
  }
  /* Every path and **every byte** are checked before the first write. A payload with one bad entry must
   * not leave the earlier, valid files already installed: the guest is still running and a half-applied
   * tree is a state nobody can describe. So this pass validates and keeps the bytes; the install loop
   * below cannot refuse, because a write in the middle of a refusal has nowhere to report to. */
  var plan;
  try {
    plan = planFiles(request.files, 1);
  } catch (error) {
    refuse(request.id, text(error));
    return;
  }
  /* The page's latest pause intent, not a message: a `pause` message still queued on the way here may
   * have been superseded by the time the load is done. Absent means "running", which is the state a
   * session that never paused is in. */
  var paused = request.paused ? true : false;
  /* Park the guest first. `artc_web_pause(1)` answering 0 (or throwing) means native did not park, so
   * the filesystem must not be touched: an unknown run state is exactly what the rollback cannot
   * describe. `pause` in the request type is the native ACK the page also waits for. */
  if (!applyPause(1)) {
    refuse(request.id, 'artc_web_pause(1) did not park the engine: ' + lastError());
    return;
  }
  var backup = [];
  var installed = [];
  try {
    if (FS.mkdirTree) FS.mkdirTree(GAME_DIR);
    /* The backup is a **refusal**, not a best effort: a capture that silently came back empty or
     * short would let `artc_web_load` fail with a rollback that restores half of what was there, and
     * the page would be told the load was refused while the directory holds a stranger's bytes. */
    backup = captureFiles();
    installed = installFiles(plan);
    var loaded = 0;
    try {
      loaded = cabi._artc_web_load(SAVE_SLOT) | 0;
    } catch (error) {
      loaded = 0;
    }
    if (loaded !== 1) {
      /* A refusal from native, or a throw, which is the same answer: the load did not happen and the
       * filesystem a `0` leaves behind is half-applied either way, so the previous files go back
       * before anything is reported. A successful rollback is still not a clean session — native may
       * already have run its own `onLoad` restorer before failing — so the worker stops rather than
       * driving later frames against a state nobody can describe. */
      rollBack(installed, backup);
      die('artc_web_load returned ' + loaded + ': ' + lastError());
      refuse(request.id, fatal);
      return;
    }
    /* The engine answered 1, so it read a slot. Reading it back is what keeps that answer honest: a
     * `1` from a session with no slot on disk is the same shape as one from a session that loaded this
     * checkpoint, and the page cannot tell them apart from the return code. */
    var content = readSaveBytes(SAVE_SLOT);
    if (!content || content.length === 0) {
      rollBack(installed, backup);
      die('artc_web_load returned 1 but ' + SAVE_SLOT + ' is not in the save directory');
      refuse(request.id, fatal);
      return;
    }
    /* Put the run state back to what the page last asked for and wait for native to answer 1: the page
     * drives frames again as soon as this ACKs, and a `0` here means the engine is in neither the state
     * the page thinks nor one this handler can describe. The files are already this slot's, so there is
     * nothing to roll back — but the session is over, and saying so is the only honest answer. */
    if (!applyPause(paused ? 1 : 0)) {
      die('the engine did not return to ' + (paused ? 'paused' : 'running') + ': ' + lastError());
      refuse(request.id, fatal);
      return;
    }
    post({ type: 'restore-ok', id: request.id, installed: installed.length, loaded: loaded, paused: paused, audio: pullAudio([]) });
  } catch (error) {
    die(text(error));
    refuse(request.id, fatal || text(error));
  }
}

/** Put the save directory back the way it was before a refused load. */
function rollBack(installed, backup) {
  var FS = moduleRef.FS;
  try {
    for (var i = 0; i < installed.length; i++) FS.unlink(GAME_DIR + '/' + installed[i]);
    for (var j = 0; j < backup.length; j++) {
      FS.writeFile(GAME_DIR + '/' + backup[j].path, base64ToBytesSafe(backup[j].data));
    }
  } catch (error) {
    die('the restore could not be rolled back');
  }
}

/**
 * One `artc_web_pause` call, or false when native refused it.
 *
 * A `0` and a throw are the same answer here — native did not put the engine in the requested run
 * state — so a caller must not treat either as "probably fine". A throw also ends the worker: the
 * native call failing outright is not a refused transition, it is an engine that can no longer be
 * driven, and every later frame would be run against it.
 */
/** One census, then at most the credits the page named. Native never pulls on its own. */
function pullAudio(credits) {
  if (!audioBridge) return { available: false, unavailable: 'createArtcAudioBridge' };
  var census;
  try { census = audioBridge.census(); }
  catch (error) { return { available: false, unavailable: text(error) }; }
  if (!census.available) return { available: false, unavailable: (census.unavailable || []).join(',') };
  var requests = [];
  var listed = Array.isArray(credits) ? credits : [];
  for (var i = 0; i < census.voices.length; i++) {
    var voice = census.voices[i];
    var frames = 0;
    for (var j = 0; j < listed.length; j++) {
      if (listed[j] && listed[j].handle === voice.handle) { frames = listed[j].frames | 0; break; }
    }
    if (frames > 0) requests.push({ handle: voice.handle, frames: frames });
  }
  var blocks = [];
  if (requests.length) {
    try { blocks = audioBridge.pull(requests); }
    catch (error) { return { available: false, voices: census.voices, unavailable: text(error) }; }
  }
  var out = [];
  for (var k = 0; k < blocks.length; k++) {
    var block = blocks[k];
    out.push({
      handle: block.handle, rate: block.rate, key: block.key,
      samples: block.samples, gain1000: block.gain1000, pan1000: block.pan1000,
    });
  }
  return { available: true, voices: census.voices, blocks: out };
}

function applyPause(value) {
  if (!cabi._artc_web_pause) return false;
  try {
    return (cabi._artc_web_pause(value) | 0) === 1;
  } catch (error) {
    die(text(error));
    return false;
  }
}

self.onmessage = function (event) {
  var request = event.data;
  if (!request || typeof request.type !== 'string') return;
  if (request.type === 'init') {
    handleInit(request);
    return;
  }
  if (request.type === 'dispose') {
    disposed = true;
    if (audioBridge) { try { audioBridge.dispose(); } catch (error) { /* the page terminates either way */ } }
    if (cabi._artc_web_dispose) {
      try {
        cabi._artc_web_dispose();
      } catch (error) {
        /* still acknowledge; the page terminates either way */
      }
    }
    post({ type: 'disposed-ack', id: request.id | 0 });
    return;
  }
  if (request.type === 'input' || request.type === 'pointer' || request.type === 'key'
    || request.type === 'pointer-move' || request.type === 'resize') {
    post({ type: 'input-ok', id: request.id | 0, count: deliver(request) });
    return;
  }
  if (fatal) {
    refuse(request.id, fatal);
    return;
  }
  if (!opened) {
    refuse(request.id, 'not ready');
    return;
  }
  if (request.type === 'frame') {
    if (!cabi._artc_web_frame) {
      refuse(request.id, 'missing _artc_web_frame');
      return;
    }
    var running = 0;
    try {
      running = cabi._artc_web_frame() | 0;
    } catch (error) {
      die(text(error));
      refuse(request.id, fatal || text(error));
      return;
    }
    /* The engine's own counters, read after the frame. `presented` does not move when the
     * compositor's redraw gate skipped the present, and that is exactly the number a reader
     * needs: the page must never add one per acknowledged frame. A counter the glue does not
     * export is reported as absent, not as zero. */
    post({
      type: 'frame-ok',
      id: request.id,
      running: running !== 0,
      reason: running ? '' : lastError(),
      started: readCounter('_artc_web_started'),
      presented: readCounter('_artc_web_presented'),
      revision: readCounter('_artc_web_revision'),
      stage: readCounter('_artc_web_stage_width'),
      stageHeight: readCounter('_artc_web_stage_height'),
      nowMs: readNowMs(),
      audio: pullAudio(request.credits),
    });
    return;
  }
  if (request.type === 'audio-pull') {
    post({ type: 'audio-pull-ok', id: request.id, audio: pullAudio(request.credits) });
    return;
  }
  if (request.type === 'audio-ack') {
    if (!audioBridge) { refuse(request.id, 'audio bridge is absent'); return; }
    try {
      audioBridge.ack(request.handle, request.frames);
    } catch (error) {
      refuse(request.id, text(error));
      return;
    }
    post({ type: 'audio-ack-ok', id: request.id });
    return;
  }
  if (request.type === 'pause') {
    if (!cabi._artc_web_pause) {
      refuse(request.id, 'pause is not implemented');
      return;
    }
    /* One call, through the same helper the restore uses: a 0 and a throw are both "native did not
     * park or unpark the engine", and the page waits for the ACK before it claims the state is real. */
    if (!applyPause(request.paused ? 1 : 0)) {
      refuse(request.id, 'pause returned 0: ' + lastError());
      return;
    }
    post({ type: 'ack', id: request.id });
    return;
  }
  if (request.type === 'checkpoint') {
    handleCheckpoint(request);
    return;
  }
  if (request.type === 'restore') {
    handleRestore(request);
    return;
  }
  refuse(request.id, 'unknown ' + request.type);
};