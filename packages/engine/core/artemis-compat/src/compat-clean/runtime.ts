import { isSaveStateFiles, type RuntimeHost, type RuntimeOptions, type RuntimeInstance, type InputEvent, type SaveState, type SaveStateFiles, type GameSource } from '@online-gal/engine-api';
import { CleanWorkerHost, CLEAN_STORE_PREFIX, plainPath, validateFiles, type CleanFile, type CleanEngineUrls } from './worker-host.js';
import { COMPAT_SAVE_KIND, type CompatFileDescriptor } from '../compat/protocol.js';

/** Explicit independent worker/glue/WASM triple; never resolves the old configured deployment. */
export class CleanArtemisRuntime implements RuntimeInstance {
  readonly runtimeId = 'artemis';
  private page: CleanWorkerHost | null = null;
  private loading: Promise<void> | null = null;
  private loaded = false;
  private closed = false;
  private paused = false;
  private failure: string | null = null;
  private persistence: Promise<void> = Promise.resolve();
  private surface = '';
  constructor(private readonly host: RuntimeHost, private readonly options: RuntimeOptions, private readonly urls: CleanEngineUrls) {
    const triple=[urls.workerUrl,urls.engineJsUrl,urls.wasmUrl];
    if (triple.some((url)=>!url) || new Set(triple).size!==3) throw new Error('clean worker, glue and WASM must be independently specified');
  }
  load(): Promise<void> {
    if (!this.loading) { this.loading=this.start(); void this.loading.catch(()=>undefined); }
    return this.loading;
  }
  private async start(): Promise<void> {
    try {
      if (this.closed || globalThis.crossOriginIsolated!==true || typeof SharedArrayBuffer!=='function' || typeof OffscreenCanvas!=='function') throw new Error('clean runtime requires an isolated OffscreenCanvas page');
      const content=await describeSource(this.host.source,()=>this.checkAlive());
      this.checkAlive();
      const saved: CleanFile[]=[];
      for (const key of await this.host.store.list(CLEAN_STORE_PREFIX)) {
        if (!key.startsWith(CLEAN_STORE_PREFIX)) continue;
        const path=key.slice(CLEAN_STORE_PREFIX.length); plainPath(path);
        const data=await this.host.store.read(key); this.checkAlive();
        if (!(data instanceof Uint8Array)) throw new Error('persisted file unavailable: '+key);
        saved.push({path,data:new Uint8Array(data)});
      }
      validateFiles(saved,false);
      const element=this.host.canvas;
      const css=this.host.surface ?? this.options.surface;
      const backing={width:element.width,height:element.height};
      const canvas=element instanceof OffscreenCanvas ? element : element.transferControlToOffscreen();
      this.checkAlive();
      this.page=new CleanWorkerHost(this.host.source,this.urls,(reason)=>this.fatal(reason),
        ()=>this.host.notify({type:'session-end',reason:'native exit'}),(level,line)=>this.host.log(level,line),
        ()=>this.host.setting('muted',false) || this.host.setting('mute',false));
      const self=this;
      async function* loose(): AsyncIterable<CleanFile> {
        for (const path of content.loose) {
          self.checkAlive();
          const data=await self.host.source.readFile(path); self.checkAlive();
          if (!(data instanceof Uint8Array)) throw new Error('loose source returned no bytes: '+path);
          yield {path,data};
        }
      }
      // Main does not instantiate WASM. All source files precede save hydration and native open.
      await this.page.load(canvas,content.packs,css,saved,loose());
      this.checkAlive(); this.loaded=true;
      this.surface=[css.width,css.height,backing.width,backing.height].join(',');
      if (this.paused) await this.page.setPaused(true);
    } catch(error) { if(!this.closed)this.fatal(String(error)); throw error; }
  }
  private checkAlive(): void { if(this.closed)throw new Error('clean runtime disposed'); }
  private fatal(reason: string): void {
    if(this.closed)return;
    this.failure=reason; this.dispose();
    try { this.host.notify({type:'fatal',message:reason}); } catch { /* terminal teardown already owns resources */ }
  }
  tick(_delta: number): void {
    if (!this.loaded || this.closed || this.paused || !this.page) return;
    const css=this.host.surface, element=this.host.canvas;
    const shape=[css.width,css.height,element.width,element.height].join(',');
    if(shape!==this.surface) { this.page.resize(css.width,css.height,element.width,element.height); this.surface=shape; }
    const frame=this.page.frame();
    if(frame)void frame.catch((error)=>{if(!this.closed)this.fatal(String(error));});
  }
  render(): void {}
  async unlockAudio(): Promise<void> { this.checkAlive(); return this.page ? this.page.unlockAudio() : Promise.reject(new Error('no clean speaker')); }
  pause(): void { this.paused=true; if(this.loaded)void this.page?.setPaused(true).catch((e)=>this.fatal(String(e))); }
  resume(): void { this.paused=false; if(this.loaded)void this.page?.setPaused(false).catch((e)=>this.fatal(String(e))); }
  sendInput(event: InputEvent): void {
    if(this.closed)return;
    if(event.type==='pointer-down' || event.type==='key-down')void this.unlockAudio().catch((e)=>this.host.log('warn',String(e)));
    if(!this.loaded || this.closed || this.paused || this.page?.busy)return;
    if(event.type==='pointer-down' || event.type==='pointer-up' || event.type==='pointer-move') {
      if(!Number.isFinite(event.x) || !Number.isFinite(event.y))return;
      if(event.type!=='pointer-move' && event.button===2) this.page?.input({kind:'key',code:8,down:event.type==='pointer-down'});
      else if(event.type==='pointer-move' || event.button===0) this.page?.input({kind:event.type==='pointer-move'?'pointer-move':'pointer',x:event.x,y:event.y,down:event.type==='pointer-down'});
      else this.host.log('warn','unsupported pointer button');
    } else if(event.type==='key-down' || event.type==='key-up') {
      const code=keyCode(event.key,event.code);
      if(code===null)this.host.log('warn','unsupported key: '+event.key);
      else this.page?.input({kind:'key',code,down:event.type==='key-down'});
    } else this.host.log('warn','unsupported input: '+event.type);
  }
  private persist(files: readonly CleanFile[]): Promise<void> {
    const run=this.persistence.then(async()=>{
      this.checkAlive();
      for(const file of files) { await this.host.store.write(CLEAN_STORE_PREFIX+file.path,file.data); this.checkAlive(); }
    });
    this.persistence=run.catch(()=>undefined); return run;
  }
  async save(): Promise<SaveStateFiles> {
    const page=this.page;
    if(!page || !this.loaded || this.closed)throw new Error('no loaded clean session');
    try {
      return await page.operation(async()=>{
        const files=await page.checkpoint(); await this.persist(files);
        return {version:1,kind:COMPAT_SAVE_KIND,data:null,atomic:false,files:files.map((f)=>({path:f.path,data:encode(f.data)}))};
      });
    } catch(error) { if(!this.closed)this.fatal(String(error)); throw error; }
  }
  async restore(state: SaveState): Promise<void> {
    if(state.version!==1 || !isSaveStateFiles(state) || state.kind!==COMPAT_SAVE_KIND)throw new Error('foreign clean checkpoint');
    const files=validateFiles(state.files.map((f)=>({path:f.path,data:decode(f.data)})),true);
    const page=this.page;
    if(!page || !this.loaded || this.closed)throw new Error('no loaded clean session');
    try { await page.operation(async()=>{await page.restore(files); await this.persist(files);}); }
    catch(error) { if(!this.closed)this.fatal(String(error)); throw error; }
  }
  dispose(): void { if(this.closed)return; this.closed=true; this.loaded=false; this.page?.dispose(); }
  whenDisposed(): Promise<void> { return Promise.all([this.page?.whenDisposed(),this.persistence]).then(()=>undefined); }
  describe(): Readonly<Record<string,unknown>> { return {engine:'compat-clean',loaded:this.loaded,disposed:this.closed,failure:this.failure,worker:this.page?.describe(),realGameVerified:false}; }
}
export function createCleanArtemisRuntime(host: RuntimeHost, options: RuntimeOptions, urls: CleanEngineUrls): CleanArtemisRuntime {
  return new CleanArtemisRuntime(host,options,urls);
}
export function keyCode(key: string, code: string): number | null {
  if(key.length===1) { const n=key.toUpperCase().charCodeAt(0); return n>=32 && n<127?n:null; }
  const named:Record<string,number>={Enter:13,NumpadEnter:13,Space:32,Spacebar:32,Escape:8,Esc:8,Backspace:8,ArrowLeft:37,ArrowUp:38,ArrowRight:39,ArrowDown:40};
  return named[key]??named[code]??null;
}
async function describeSource(source: GameSource, checkAlive: () => void): Promise<{packs: CompatFileDescriptor[]; loose:string[]}> {
  if(!source.capabilities.list || !source.capabilities.randomAccess || !source.readRange)throw new Error('complete enumeration and range reads required');
  const entries:string[]=[], dirs=[''], seen=new Set<string>();
  while(dirs.length) {
    const dir=dirs.shift()!;
    const listing=await source.list(dir); checkAlive();
    for(const name of listing) {
      const directory=name.endsWith('/'), path=(dir?dir+'/':'')+(directory?name.slice(0,-1):name);
      plainPath(path); if(seen.has(path))throw new Error('duplicate source path: '+path); seen.add(path);
      if(directory)dirs.push(path); else entries.push(path);
    }
  }
  const roots=entries.filter((p)=>!p.includes('/') && /\.pfs$/i.test(p)).sort();
  const base=roots.find((p)=>p.toLowerCase()==='root.pfs') ?? (roots.length===1?roots[0]:undefined);
  if(!base)throw new Error('ambiguous or missing root PF8 pack');
  const packs:CompatFileDescriptor[]=[], mounted=new Set<string>();
  for(let i=-1;i<64;i++) {
    const path=i<0?base:base+'.'+String(i).padStart(3,'0');
    if(!entries.includes(path))continue;
    const size=await source.size(path); checkAlive();
    if(!Number.isSafeInteger(size) || size<11)throw new Error('pack size unavailable: '+path);
    const magic=await source.readRange(path,0,3); checkAlive();
    if(magic.length!==3 || magic[0]!==112 || magic[1]!==102 || ![50,54,56].includes(magic[2]!)) { if(i<0)throw new Error('root is not a PF container'); else continue; }
    mounted.add(path);
    packs.push({id:packs.length+1,path,virtualPath:'/game/root.pfs'+(i<0?'':'.'+String(i).padStart(3,'0')),size});
  }
  const loose=entries.filter((p)=>!mounted.has(p));
  const virtual=new Set(packs.map((p)=>p.virtualPath.slice(6)));
  if(loose.some((p)=>virtual.has(p)))throw new Error('loose conflicts with virtual pack mapping');
  return {packs,loose};
}
function encode(bytes: Uint8Array): string {
  let text=''; for(const byte of bytes)text+=String.fromCharCode(byte); return btoa(text);
}
function decode(text: string): Uint8Array {
  if(typeof text!=='string' || !/^(?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{2}==|[A-Za-z0-9+/]{3}=)?$/.test(text) || text.length>89478488)throw new Error('invalid save base64');
  const raw=atob(text), bytes=Uint8Array.from(raw,(c)=>c.charCodeAt(0));
  if(encode(bytes)!==text)throw new Error('noncanonical save base64'); return bytes;
}
