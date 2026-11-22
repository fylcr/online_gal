/* Native ownership stays in this classic worker. No timers or frame loop. */
(function (self) {
  'use strict';
  var module = null, api = {}, audio = null, epoch = 0, audioEpoch = 1;
  var control, payload, descriptors = [], canvas, opened = false, dead = false, init;
  var signatures = {
    open:['number',['string','string']], frame:['number',[]], pointer:[null,['number','number','number']],
    pointer_move:[null,['number','number']], key:[null,['number','number']], resize:[null,['number','number']],
    started:['number',[]], stage_width:['number',[]], stage_height:['number',[]], revision:['number',[]],
    presented:['number',[]], exit_requested:['number',[]], last_error:['string',[]], pause:['number',['number']],
    now_ms:['number',[]], dispose:[null,[]], save:['number',['string']], load:['number',['string']],
    log_sink:[null,['number']], audio_pump:['number',[]], audio_voice_count:['number',[]],
    audio_voice_handle:['number',['number']], audio_voice_key:['string',['number']], audio_voice_rate:['number',['number']],
    audio_read_stereo:['number',['number','number','number','number','number']], audio_ended:['number',['number']],
    audio_played_frames:['number',['string']], audio_ack:['number',['number','number']]
  };
  function post(message) { self.postMessage(Object.assign({}, message, {epoch:epoch})); }
  function reply(m, data) { post(Object.assign({type:m.type+'-ok', id:m.id}, data)); }
  function errorText(e) { return String(e && e.message || e); }
  function release() {
    if (audio) { audio.dispose(); audio = null; }
    if (api.log_sink) api.log_sink(0); // No addFunction export: stderr/printErr is the supported log path.
    if (api.dispose) api.dispose();
    opened = false;
  }
  function fatal(e) {
    if (dead) return;
    dead = true;
    try { release(); } catch (_) { /* main terminates even if native teardown fails */ }
    post({type:'fatal', reason:errorText(e)});
  }
  function path(name) {
    if (typeof name !== 'string' || !name || /[\\\0:]/.test(name) || name.split('/').some(function (s) { return !s || s==='.' || s==='..'; })) throw new Error('unsafe path');
    return '/game/'+name;
  }
  function directory(target) { module.FS.mkdirTree(target.slice(0,target.lastIndexOf('/'))); }
  function write(file) {
    var target = path(file.path);
    if (!(file.data instanceof Uint8Array)) throw new Error('file bytes missing');
    if (descriptors.some(function(d) { return d.virtualPath === target; })) throw new Error('file collides with range pack');
    directory(target); module.FS.writeFile(target, file.data);
  }
  function resize(m) {
    ['width','height','backingWidth','backingHeight'].forEach(function(k) {
      if (!Number.isSafeInteger(m[k]) || m[k]<1 || m[k]>2147483647) throw new Error('invalid canvas '+k);
    });
    if (canvas.width !== m.backingWidth) canvas.width = m.backingWidth;
    if (canvas.height !== m.backingHeight) canvas.height = m.backingHeight;
    if (opened) api.resize(m.width,m.height);
  }
  function range(name, offset, destination, length) {
    if (!Number.isSafeInteger(offset) || offset<0 || !Number.isSafeInteger(length) || length<0 ||
        !Number.isSafeInteger(destination) || destination<1 || destination>module.HEAPU8.length || length>module.HEAPU8.length-destination) throw new Error('invalid native range');
    var descriptor = descriptors.find(function(d) { return d.virtualPath === name; });
    if (!descriptor) return -1;
    if (offset>descriptor.size || length>descriptor.size-offset) throw new Error('native read beyond pack');
    var copied = 0;
    while (copied<length) {
      var count = Math.min(length-copied,payload.length), at = offset+copied;
      var sequence = Atomics.load(control,0)|0, readEpoch = Atomics.load(control,5)|0;
      Atomics.store(control,1,-2); Atomics.store(control,2,descriptor.id);
      Atomics.store(control,3,at>>>0); Atomics.store(control,4,Math.floor(at/4294967296)>>>0);
      post({type:'read',fileId:descriptor.id,offset:at,length:count,sequence:sequence,readEpoch:readEpoch});
      var deadline = performance.now()+30000;
      while (Atomics.load(control,0)===sequence) {
        var left = deadline-performance.now();
        if (left<=0 || Atomics.wait(control,0,sequence,left)==='timed-out') throw new Error('PF8 range timeout');
      }
      if (Atomics.load(control,5)!==readEpoch) throw new Error('PF8 read cancelled');
      var got = Atomics.load(control,1);
      if (got === -1) return -1;
      if (got !== count) throw new Error('PF8 range failure '+got+'/'+count);
      // malloc/heap growth can replace the view: never retain HEAPU8 across an exchange.
      module.HEAPU8.set(payload.subarray(0,count),destination+copied);
      copied+=count;
    }
    return length;
  }
  function bind() {
    module = self.Module;
    ['ccall','cwrap','FS','UTF8ToString','HEAPU8','ENV','_malloc','_free'].forEach(function(n) {
      if (module[n]===undefined) throw new Error('missing module '+n);
    });
    Object.keys(signatures).forEach(function(n) {
      if (typeof module['_artc_web_'+n]!=='function') throw new Error('missing CABI '+n);
      api[n]=module.cwrap('artc_web_'+n,signatures[n][0],signatures[n][1]);
    });
    module.FS.mkdirTree('/game');
    if (typeof self.createArtcAudioBridge!=='function') throw new Error('worker-audio helper not supplied');
    audio=self.createArtcAudioBridge(module);
    api.log_sink(0);
    reply(init, {});
  }
  function initialize(m) {
    if (init) throw new Error('worker initialized twice');
    init=m; epoch=m.epoch; canvas=m.canvas; descriptors=m.descriptors;
    if (!canvas || typeof canvas.getContext!=='function' || !(m.buffer instanceof SharedArrayBuffer) || m.buffer.byteLength!==4194328) throw new Error('invalid canvas/shared window');
    control=new Int32Array(m.buffer,0,6); payload=new Uint8Array(m.buffer,24);
    var initialized=false, importing=true;
    self.Module={canvas:canvas,noInitialRun:true,
      locateFile:function(p) { if (/\.wasm$/.test(p)) return m.wasmUrl; return new URL(p,m.engineJsUrl).href; },
      artcReadRange:function(name,offset,dest,len) {
        try { return range(name,offset,dest,len); }
        catch(e) { dead=true; post({type:'fatal',reason:errorText(e)}); return -1; }
      },
      print:function(line) { post({type:'log',level:'info',line:String(line)}); },
      printErr:function(line) { post({type:'log',level:'error',line:String(line)}); },
      onAbort:function(reason) { fatal('WASM abort: '+reason); },
      onRuntimeInitialized:function() { initialized=true; if (!importing) { try { bind(); } catch(e) { fatal(e); } } }
    };
    // Emscripten's #canvas selector must resolve the real transferred target, not a fake GL object.
    self.document={querySelector:function(selector) { return selector==='#canvas' ? canvas : null; }};
    if (typeof self.WebGLRenderingContext==='undefined') self.WebGLRenderingContext=function() {};
    if (m.audioHelperUrl) importScripts(m.audioHelperUrl);
    importScripts(m.engineJsUrl);
    importing=false;
    if (initialized && !dead) bind();
  }
  function files() {
    var out=[], total=0;
    function walk(dir) {
      module.FS.readdir(dir).forEach(function(name) {
        if (name==='.' || name==='..') return;
        var full=dir+'/'+name, stat=module.FS.stat(full);
        if (module.FS.isDir(stat.mode)) { walk(full); return; }
        // Only files actually written by native operations, never the preloaded game tree.
        if (!changed.has(full)) return;
        var data=module.FS.readFile(full), relative=full.slice(6); path(relative);
        total+=data.length;
        if (data.length>67108864 || total>268435456 || out.length>=4096) throw new Error('native save exceeds file bounds');
        out.push({path:relative,data:new Uint8Array(data)});
      });
    }
    walk('/game');
    if (!out.some(function(f) { return f.path==='platform-slot.dat'; })) throw new Error('native checkpoint slot missing');
    return out;
  }
  var changed=new Set();
  function captureWrites(work) {
    // The exported legacy FS.write is also used by native write syscalls. trackingDelegate
    // would require unexported FS_DEBUG build support, so observe actual successful writes.
    var old=module.FS.write, rename=module.FS.rename;
    module.FS.write=function(stream) {
      var result=old.apply(module.FS,arguments);
      if (result>0 && stream.path.indexOf('/game/')===0) changed.add(stream.path);
      return result;
    };
    module.FS.rename=function(from,to) {
      var result=rename.apply(module.FS,arguments);
      if(changed.delete(from))changed.add(to);
      return result;
    };
    try { return work(); } finally { module.FS.write=old; module.FS.rename=rename; }
  }
  self.onmessage=function(event) {
    var m=event.data;
    if (!m || (m.type!=='init' && m.epoch!==epoch)) return;
    if(m.type==='dispose') {
      try { release(); } catch(e) { post({type:'log',level:'error',line:errorText(e)}); }
      dead=true; post({type:'disposed'}); self.close(); return;
    }
    if(dead)return;
    try {
      if (m.type==='init') { initialize(m); return; }
      if (!module) throw new Error('worker not ready');
      if (m.type==='loose') { if(opened)throw new Error('loose after open'); write(m.file); reply(m,{}); return; }
      if (m.type==='open') {
        if(opened)throw new Error('open twice');
        m.files.forEach(function(f) { write(f); changed.add(path(f.path)); });
        if(api.open('/game/root.pfs','windows')!==1)throw new Error(api.last_error()||'open refused');
        if(dead)throw new Error('open range failed');
        opened=true;
        resize({width:init.surface.width,height:init.surface.height,backingWidth:canvas.width,backingHeight:canvas.height});
        reply(m,{}); return;
      }
      if (!opened) throw new Error('engine not open');
      if (m.type==='frame') {
        var running=captureWrites(function(){return api.frame();});
        if(dead)return;
        if(running!==0 && running!==1)throw new Error('invalid native frame');
        if(!running && api.last_error())throw new Error(api.last_error());
        reply(m,{running:running===1,started:api.started(),revision:api.revision(),presented:api.presented(),
          stage:api.stage_width(),stageHeight:api.stage_height(),nowMs:api.now_ms(),voices:running ? audio.census().voices : []}); return;
      }
      if (m.type==='pause') { if(api.pause(m.paused?1:0)!==1)throw new Error(api.last_error()||'pause refused'); reply(m,{}); return; }
      if (m.type==='resize') { resize(m); return; }
      if (m.type==='input') {
        var input=m.input;
        if(input.kind==='pointer')api.pointer(input.x,input.y,input.down?1:0);
        else if(input.kind==='pointer-move')api.pointer_move(input.x,input.y);
        else if(input.kind==='key')api.key(input.code,input.down?1:0);
        else throw new Error('unsupported input');
        return;
      }
      if (m.type==='save') {
        if(captureWrites(function(){return api.save('platform-slot.dat');})!==1)throw new Error(api.last_error()||'save refused');
        reply(m,{files:files()}); return;
      }
      if (m.type==='load') {
        audioEpoch=m.audioEpoch;
        m.files.forEach(function(f) { write(f); changed.add(path(f.path)); });
        if(captureWrites(function(){return api.load('platform-slot.dat');})!==1)throw new Error(api.last_error()||'load refused');
        reply(m,{}); return;
      }
      if (m.type==='audio-pull') {
        if(m.audioEpoch!==audioEpoch)throw new Error('stale audio pull');
        reply(m,{blocks:audio.pull(m.requests)}); return;
      }
      if (m.type==='audio-ack') {
        if(m.audioEpoch===audioEpoch && api.audio_voice_rate(m.handle)>0) audio.ack(m.handle,m.frames);
        return;
      }
      throw new Error('unknown worker operation');
    } catch(e) { fatal(e); }
  };
})(self);
