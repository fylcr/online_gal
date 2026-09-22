# ONS emulation: baseline verified, and the byte-granularity gap

## Emulation works

OnscripterYuri's WASM build runs the real game correctly. Evidence from the
engine's own log and state:

```
ONScripter Yuri v0.7.7beta, (Jh0.8.0,Ons20181218,NSC2.96)
Initialize GameController
Display: 793 x 446 (32 bpp)
Audio: 48000 Hz 16 bit stereo
```

- the page title becomes the game's own **`Tsukiniyorisou + Append for ONScripter`**,
  which is read out of the script — so `0.txt`–`2.txt` were parsed
- canvas is `1189x669`, WebGL context alive (not lost)
- both archives were mounted and opened; the missing `uoncur.bmp` / `system.lua`
  warnings are assets this title does not ship, not failures

So the engine, the game, the mount and the rendering path all work. The remaining
problem is not correctness, it is **granularity**.

## The gap, confirmed by measurement

`onsyuri.html`'s `mount_game` does defer reads — it records
`{url, loaded:false}` per file and fetches on first touch. But when a file *is*
touched, `fetch_file` performs:

```js
fetch(fnode.url).then(resp => resp.arrayBuffer())   // the entire file
_FS.writeFile(path, new Uint8Array(data));
```

Measured over the boot, by intercepting `fetch` and reading `content-length`:

| request | bytes |
| --- | --- |
| `onsyuri.wasm` | 4.4 MB |
| `1.txt` | 4.0 MB |
| `2.txt` | 1.6 MB |
| `default.ttf` | 9.3 MB |
| `arc.nsa` | **1779.9 MB** |
| `arc1.nsa` | **1151.2 MB** |
| **total** | **2950.5 MB** |

Every request is a plain `200` with no `Range` header. So "lazy" here means only
*when*, never *how much*: 3 GB is resident before the first frame, and the
`filemap` confirms it — `arc.nsa` and `arc1.nsa` both report `loaded: true`.

This is exactly the shortcoming reported: the WASM build cannot load files on
demand at byte granularity, so everything has to be in the browser first. It is
not a limitation of ONS, and not of WebAssembly — it is one `arrayBuffer()` call.

## Why the fix is not local

Replacing that call with a ranged read is not sufficient on its own: Emscripten's
in-memory FS is synchronous (`fseek`/`fread`), while a range fetch is async.
Handing the FS a partial file would make the engine read wrong bytes, not fewer.

The bridge that resolves this is the one AetherKiri's browser host already uses
and which was verified earlier in this repository:

```js
// main thread blocks; a worker performs the async range fetch
const sab = new SharedArrayBuffer(8 + length);
worker.postMessage({ kind: 'read', fileId, offset, length, sab });
Atomics.wait(header, 0, 0, 30000);
return new Uint8Array(sab, 8, count);
```

That pattern turns an async byte-range source into a synchronous one, which is
precisely what the Emscripten FS needs. `packages/vfs/src/lazy.ts` already
provides the ranged end — `RangedLazyFile` over a `Blob` or HTTP source, with
`ArchiveEntryLazyFile` serving entries inside an archive — and the missing piece
is the synchronous facade over it.

## Verification harness

- server: `.ref/onsyuri-web/serve.mjs` (port 4599, Range-capable, COOP/COEP,
  hardened against client disconnects)
- game: `.ref/onsyuri-web/onsyuri_web/game/` — the eight real files, hard-linked
  so nothing is copied
- index: `.ref/onsyuri-web/onsyuri_web/onsyuri_index.json`
