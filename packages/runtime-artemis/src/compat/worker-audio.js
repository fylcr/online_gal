/*
 * Classic-worker PCM pull for the web audio CABI. `self.createArtcAudioBridge(Module)` is loadable
 * with importScripts and returns one bridge per engine Module. No timer, no frame, no decoder.
 *
 * Counts, rates, and read lengths stay the exact JS numbers the native side returned. A missing
 * export is `available: false`, never an empty voice list. Request rate/key are ignored.
 */
(function (self) {
  'use strict';

  var SPEC = {
    _artc_web_audio_pump: { ret: 'number', args: [] },
    _artc_web_audio_voice_count: { ret: 'number', args: [] },
    _artc_web_audio_voice_handle: { ret: 'number', args: ['number'] },
    _artc_web_audio_voice_rate: { ret: 'number', args: ['number'] },
    _artc_web_audio_voice_key: { ret: 'string', args: ['number'] },
    _artc_web_audio_read_stereo: { ret: 'number', args: ['number', 'number', 'number', 'number', 'number'] },
    _artc_web_audio_ack: { ret: 'number', args: ['number', 'number'] },
    _artc_web_audio_played_frames: { ret: 'number', args: ['string'] }
  };
  var NEED_CENSUS = [
    '_artc_web_audio_pump', '_artc_web_audio_voice_count', '_artc_web_audio_voice_handle',
    '_artc_web_audio_voice_rate', '_artc_web_audio_voice_key'
  ];
  var NEED_PULL = NEED_CENSUS.concat(['_artc_web_audio_read_stereo']);
  var MAX_FRAMES = 4096;
  var MAX_VOICES = 256;
  var MAX_EXACT = 9007199254740991;
  var MAX_RATE = 384000;
  var PCM_BYTES = MAX_FRAMES * 4;
  var BLOCK = PCM_BYTES + 8;

  function isExact(n) {
    return typeof n === 'number' && Number.isFinite(n) && Math.trunc(n) === n;
  }

  function inRange(n, lo, hi) {
    return isExact(n) && n >= lo && n <= hi;
  }

  function clamp(v, lo, hi) {
    return v < lo ? lo : v > hi ? hi : v;
  }

  function create(Module) {
    if (!Module || typeof Module.cwrap !== 'function') throw new Error('createArtcAudioBridge needs a Module with cwrap');
    var cabi = {};
    var missing = [];
    for (var name in SPEC) {
      if (typeof Module[name] === 'function') cabi[name] = Module.cwrap(name.slice(1), SPEC[name].ret, SPEC[name].args, { async: false });
      else missing.push(name.slice(1));
    }
    var disposed = false;
    var block = 0;

    function live() {
      if (disposed) throw new Error('createArtcAudioBridge: disposed');
    }

    function absent(names) {
      for (var i = 0; i < names.length; i++) {
        if (typeof cabi[names[i]] !== 'function') return names[i].slice(1);
      }
      return '';
    }

    function heap() {
      var view = Module.HEAPU8;
      if (!view || !view.buffer || typeof view.byteLength !== 'number') return null;
      return view;
    }

    /* addr+bytes must sit inside the live heap. Subtract so a wrapping end cannot pass. */
    function span(addr, bytes, align) {
      var view = heap();
      if (!view) return false;
      var at = view.byteOffset + addr;
      if (align && at % align !== 0) return false;
      if (addr > view.byteLength || bytes > view.byteLength - addr) return false;
      if (addr === 0 && bytes !== 0) return false;
      return true;
    }

    function ensure() {
      if (block) return;
      if (!heap()) throw new Error('createArtcAudioBridge: heap unavailable');
      if (typeof Module._malloc !== 'function') throw new Error('createArtcAudioBridge: _malloc is unavailable');
      var p = Module._malloc(BLOCK);
      if (!p) throw new Error('createArtcAudioBridge: malloc refused ' + BLOCK + ' bytes');
      if (!span(p, BLOCK, 4)) {
        if (typeof Module._free === 'function') Module._free(p);
        throw new Error('createArtcAudioBridge: malloc span');
      }
      block = p;
    }

    function census() {
      live();
      var gone = absent(NEED_CENSUS);
      if (gone) return { available: false, unavailable: missing.slice() };
      var pumped = cabi._artc_web_audio_pump();
      if (pumped === 0) return { available: true, voices: [], unavailable: missing.slice(), pumped: false };
      if (pumped !== 1) throw new Error('artc_web_audio_pump answered ' + pumped);
      var count = cabi._artc_web_audio_voice_count();
      if (!inRange(count, 0, MAX_VOICES)) throw new Error('artc_web_audio_voice_count answered ' + count);
      var voices = [];
      for (var index = 0; index < count; index++) {
        var handle = cabi._artc_web_audio_voice_handle(index);
        if (!inRange(handle, 1, MAX_EXACT)) throw new Error('artc_web_audio_voice_handle(' + index + ') answered ' + handle);
        var rate = cabi._artc_web_audio_voice_rate(handle);
        if (rate === 0) continue;
        if (!inRange(rate, 1, MAX_RATE)) throw new Error('artc_web_audio_voice_rate(' + handle + ') answered ' + rate);
        var key = cabi._artc_web_audio_voice_key(handle);
        if (typeof key !== 'string') throw new Error('artc_web_audio_voice_key(' + handle + ') answered ' + key);
        voices.push({ handle: handle, key: key, rate: rate });
      }
      return { available: true, voices: voices, unavailable: missing.slice(), pumped: true };
    }

    function pull(requests) {
      live();
      if (!Array.isArray(requests)) throw new Error('createArtcAudioBridge: pull wants an array');
      if (requests.length > MAX_VOICES) throw new Error('createArtcAudioBridge: pull wants at most ' + MAX_VOICES + ' requests');
      var seen = Object.create(null);
      var pending = [];
      for (var i = 0; i < requests.length; i++) {
        var request = requests[i];
        var handle = request && request.handle;
        var frames = request && request.frames;
        if (!inRange(handle, 1, MAX_EXACT)) throw new Error('createArtcAudioBridge: bad handle ' + handle);
        if (!inRange(frames, 0, MAX_FRAMES)) throw new Error('createArtcAudioBridge: bad credit ' + frames);
        if (seen[handle]) throw new Error('createArtcAudioBridge: handle ' + handle + ' is listed twice in one pull');
        seen[handle] = true;
        pending.push({ handle: handle, frames: frames });
      }
      var gone = absent(NEED_PULL);
      if (gone) throw new Error('createArtcAudioBridge: ' + gone + ' is unavailable');
      if (pending.length && !heap()) throw new Error('createArtcAudioBridge: heap unavailable');
      for (var j = 0; j < pending.length; j++) {
        var rate = cabi._artc_web_audio_voice_rate(pending[j].handle);
        if (!inRange(rate, 1, MAX_RATE)) throw new Error('artc_web_audio_voice_rate(' + pending[j].handle + ') answered ' + rate);
        var key = cabi._artc_web_audio_voice_key(pending[j].handle);
        if (typeof key !== 'string') throw new Error('artc_web_audio_voice_key(' + pending[j].handle + ') answered ' + key);
        pending[j].rate = rate;
        pending[j].key = key;
      }
      var read = cabi._artc_web_audio_read_stereo;
      var blocks = [];
      for (var k = 0; k < pending.length; k++) {
        var item = pending[k];
        if (!item.frames) continue;
        ensure();
        var pcm = block;
        var slot = block + PCM_BYTES;
        if (!span(pcm, item.frames * 4, 2) || !span(slot, 4, 4) || !span(slot + 4, 4, 4)) {
          throw new Error('createArtcAudioBridge: span');
        }
        var got = read(item.handle, pcm, item.frames, slot, slot + 4);
        if (!isExact(got)) throw new Error('artc_web_audio_read_stereo answered ' + got);
        if (got < 0) throw new Error('artc_web_audio_read_stereo refused handle ' + item.handle + ' with ' + got);
        if (got > item.frames) throw new Error('artc_web_audio_read_stereo wrote ' + got + ' frames of ' + item.frames);
        if (!got) continue;
        var view = heap();
        if (!view || !span(pcm, got * 4, 2)) throw new Error('createArtcAudioBridge: span');
        var samples = new Int16Array(view.buffer, view.byteOffset + pcm, got * 2).slice();
        var gainView = new DataView(view.buffer, view.byteOffset + slot, 8);
        blocks.push({
          handle: item.handle,
          rate: item.rate,
          key: item.key,
          samples: samples,
          gain1000: clamp(gainView.getInt32(0, true), 0, 1000),
          pan1000: clamp(gainView.getInt32(4, true), -1000, 1000)
        });
      }
      return blocks;
    }

    function ack(handle, cumulativeFrames) {
      live();
      if (!inRange(handle, 1, MAX_EXACT)) throw new Error('createArtcAudioBridge: bad handle ' + handle);
      if (!inRange(cumulativeFrames, 0, MAX_EXACT)) throw new Error('createArtcAudioBridge: bad cumulative frames ' + cumulativeFrames);
      if (typeof cabi._artc_web_audio_ack !== 'function') throw new Error('createArtcAudioBridge: artc_web_audio_ack is unavailable');
      var code = cabi._artc_web_audio_ack(handle, cumulativeFrames);
      if (code !== 1) throw new Error('artc_web_audio_ack refused handle ' + handle + ' at ' + cumulativeFrames + ' with ' + code);
      return { handle: handle, frames: cumulativeFrames };
    }

    function playedFrames(key) {
      live();
      if (typeof cabi._artc_web_audio_played_frames !== 'function') {
        return { available: false, unavailable: 'artc_web_audio_played_frames' };
      }
      if (typeof key !== 'string') throw new Error('createArtcAudioBridge: bad key ' + key);
      var frames = cabi._artc_web_audio_played_frames(key);
      if (!inRange(frames, 0, MAX_EXACT)) throw new Error('artc_web_audio_played_frames answered ' + frames);
      return { available: true, frames: frames };
    }

    function dispose() {
      if (disposed) return;
      disposed = true;
      if (!block) return;
      var p = block;
      block = 0;
      Module._free(p);
    }

    return {
      census: census,
      pull: pull,
      ack: ack,
      dispose: dispose,
      playedFrames: playedFrames,
      unavailable: missing.slice()
    };
  }

  self.createArtcAudioBridge = create;
})(self);
