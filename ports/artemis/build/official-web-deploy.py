#!/usr/bin/env python3
"""
Deploy the real game to the *official* Artemis WebAssembly build, in asyncify mode.

## Why this exists

There are two ways to run Artemis in a browser, and they are not variations of one
thing:

- `ports/artemis/` builds a WebAssembly port of the **third-party** `artemis-compat`
  engine (the reference clone under `.ref/artemis-compat`) with a host layer
  transcribed from its Android `NativeActivity`. That port is where all the input
  work lives, and it is where the remaining defects are.

- The **official** Artemis Engine SDK ships its own WebAssembly build under
  `.ref/artemis/WebAssembly/`. It is not open source and it is not ours to
  redistribute, but it is the reference implementation, so when a rendering or
  input question cannot be settled from `artemis-compat` alone, the official
  binary is the oracle.

This script deploys the real game to the official build. It is deliberately
separate from `web-deploy.sh`, which deploys *our* build.

## The two official modes, and why asyncify is the only usable one

`WebAssembly/` (plain) downloads the whole `artemis.data` package and reads it
with `FS_createDataFile` — the readme calls this "オンメモリモードが基本". That is
fine for the 15.8 MB SDK sample and impossible here: `pack_file.txt` caps a single
`.pfs` at 2 GB, so a real title ships as `root.pfs` (2.69 GB) plus `root.pfs.000`
(3.36 GB). The browser would have to hold ~6 GB in one `WebAssembly.Memory`.

`WebAssembly/asyncify/` reads loose files over HTTP as the engine asks for them.
The engine builds `<ASYNCIFY_URL><path>` for each request, so a game only needs to
exist as loose files on the server. Producing that tree is what
`ports/artemis/tools/extract-pfs.mjs` does; this script takes it from there.

## Why the pack is written here instead of by `GeneratePackFile2.exe`

The SDK's own packer is a Windows-only proprietary binary. In asyncify mode the
pack needs to contain `system.ini` and *nothing else* — the readme is explicit that
everything else comes from the server — so writing it is a 30-line pf8 job, and
doing it here is what makes this recipe portable and keeps the SDK binary out of
the toolchain. The format is the one `packages/runtime-artemis/src/pfs.ts` reads
and `make-min-pack.py` documents.

## Licensing

The official SDK is not GPL and not redistributable: its `Copyright.txt` grants
use only to parties who obtained permission from the author, with a licence fee
for commercial use. `.ref/` is gitignored and this script only *reads* from there
to assemble a local test deployment; nothing from the SDK may be committed.
"""

import argparse
import hashlib
import os
import re
import shutil
import struct
import sys
import uuid

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))


def write_pf8(path, files, version="6"):
    """
    Write a `pf8`-family package, byte-compatible with the SDK's own packer.

    Layout, matching what `packages/runtime-artemis/src/pfs.ts` parses:

        0   "pf"         2 bytes
        2   version      one ASCII digit; 6 is unkeyed, 8 and 9 are XOR-keyed
        3   index_size   u32 LE, byte length of the index blob *including* its count
        7   index        index_size bytes

        record: [u32 name_length][name][u32 pad][u32 offset][u32 size]

    The default version is **6**, matching what the SDK's `GeneratePackFile2.exe`
    emits for the real game (`root.pfs` starts `pf6`). That is not cosmetic: in the
    keyed variants (4/5/8/9) payloads are XOR-masked with `SHA-1(<index blob>)`, so
    declaring 8 while writing plaintext does not fail cleanly -- the engine decodes
    the plaintext into garbage and asserts deep inside boost's `unordered_map`
    while parsing it as an INI.

    ## The index trailer

    After the records the official packer writes a directory block that neither
    `pfs.ts` nor GARbro's `ArcPFS.cs` reads -- both stop after `count` records.
    The official engine does read it: a pack with correct records and no trailer
    makes the engine abort while opening, with

        Assertion failed: mlf_ >= minimum_max_load_factor, at: .../unordered/
        detail/implementation.hpp,3027,min_buckets_for_size

    followed by `babel.cpp:1513,translate`. Decoded by running the SDK packer over
    generated fixtures and varying names, the block is:

        u32  record_count + 1
        repeat record_count times:  u32 (index offset just past that record's name), u32 0
        u32 0, u32 0
        u32 (index offset just past the last record)

    which is `record_count + 1` directory entries of 8 bytes each plus the count
    and the terminator. Emitting it makes this writer's output byte-identical to
    the SDK packer's for the same input tree.

    ## Name encoding and order

    Names are stored as **CP932 (Shift_JIS)**, not UTF-8 -- verified by packing a
    file with a Japanese name and reading the bytes back. `pfs.ts` copes with both
    (it tries UTF-8 first and falls back to `shift_jis`), and the real game's
    `root.pfs` happens to hold only ASCII names, so this only matters once a pack
    contains non-ASCII paths. Matching the reference keeps our output byte-identical.

    Entries are sorted by their encoded name bytes, ascending. The SDK packer sorts
    and a reader that does a binary search over the table would depend on it, so the
    order is part of the format even though `pfs.ts` tolerates any order.

    ## Known difference for packs with more than one entry

    With a single entry -- the only case this script needs -- the output is
    byte-identical to the SDK packer's, verified across ASCII, CP932, nested, empty
    and 4 KB payloads. With several entries the *index* still matches byte for byte,
    but the SDK packer lays the payloads out in a different physical order (roughly
    case-insensitive alphabetical, so `aaa, MMM, zzz, _under` where the index sorts
    `MMM, _under, aaa, zzz`). That is invisible to any reader, because payload
    locations come from the index offsets rather than from order, so it is recorded
    here rather than chased. It would only matter if something downstream hashed the
    whole file.
    """
    encoded = [(name.encode("cp932"), payload) for name, payload in files]
    encoded.sort(key=lambda item: item[0])
    files = [(raw, payload) for raw, payload in encoded]

    # Two passes: payload offsets are absolute, so the index (and therefore the
    # header size) has to be sized before any offset can be written.
    def build_index(offsets):
        blob = bytearray(struct.pack("<i", len(files)))
        name_ends = []
        cursor = 4
        for (raw, payload), offset in zip(files, offsets):
            blob += struct.pack("<I", len(raw))
            blob += raw
            cursor += 4 + len(raw)
            name_ends.append(cursor)
            blob += struct.pack("<I", 0)  # the field both reference readers ignore
            blob += struct.pack("<I", offset)
            blob += struct.pack("<I", len(payload))
            cursor += 12

        # The directory trailer. Which offset each entry carries matters: it is the
        # index offset just *past that record's name*, not past the record. The
        # engine builds `count + 1` entries, then a terminating pair, then the
        # offset past the last record.
        trailer = bytearray(struct.pack("<I", len(files) + 1))
        for name_end in name_ends:
            trailer += struct.pack("<II", name_end, 0)
        trailer += struct.pack("<II", 0, 0)
        trailer += struct.pack("<I", cursor)
        return bytes(blob + trailer)

    index = build_index([0] * len(files))
    cursor = 7 + len(index)
    offsets = []
    for _, payload in files:
        offsets.append(cursor)
        cursor += len(payload)
    index = build_index(offsets)

    out = bytearray()
    out += b"pf" + version.encode("ascii")
    out += struct.pack("<I", len(index))
    out += index
    for _, payload in files:
        out += payload
    with open(path, "wb") as handle:
        handle.write(out)

    keyed = version in ("4", "5", "8", "9")
    print(f"  wrote {os.path.basename(path)}: {len(out):,} bytes, "
          f"{len(files)} entr{'y' if len(files) == 1 else 'ies'}, version={version}, "
          f"index {len(index):,} B{' (payloads XOR-keyed)' if keyed else ' (unkeyed)'}")
    return keyed


def patch_system_ini(path, extra):
    """
    Add or overwrite keys in the `[WASM]` section.

    The real game's `system.ini` already has a `[WASM]` block (WIDTH/HEIGHT/
    CHARSET/BOOT) because the publisher built one for a web release, but it is
    missing `SAVEPATH`, which the readme makes mandatory. Existing keys are kept
    and only the ones we set are replaced, so re-running does not stack duplicates.
    Line endings are preserved because the file is CRLF and the engine's INI
    reader is happy either way, but rewriting them would show up as a whole-file
    diff for no reason.
    """
    raw = open(path, "rb").read()
    text = raw.decode("utf-8-sig" if raw[:3] == b"\xef\xbb\xbf" else "utf-8")
    newline = "\r\n" if "\r\n" in text else "\n"
    head, sep, tail = text.partition("[WASM]")
    if not sep:
        raise SystemExit(f"{path}: no [WASM] section; asyncify mode needs one")

    keep = []
    for line in tail.split(newline):
        key = line.split("=", 1)[0].strip().upper() if "=" in line else ""
        if key in extra:
            continue
        keep.append(line)

    body = head + sep + newline + "".join(f"{k} = {v}{newline}" for k, v in extra.items())
    body += newline.join(keep).lstrip(newline)
    open(path, "wb").write(body.encode("utf-8"))
    print(f"  patched {path}")
    for key, value in extra.items():
        print(f"    [WASM] {key} = {value}")


def detect_savrpath(ini_text):
    """
    Pick a `SAVEPATH`.

    The readme requires a value unique per game *per origin*, because saves land in
    IndexedDB keyed by hostname and two games on one host would otherwise share a
    slot. The real `system.ini` has a Windows `SAVEPATH` naming the title, and its
    last path component is already a good unique string.

    Most Artemis titles have a Japanese `SAVEPATH` (this one is
    `まどそふと\\ハミダシクリエイティブ`), and the key may only contain lowercase
    ASCII letters, digits and underscores. Stripping the non-ASCII characters
    therefore leaves nothing at all, so a stable hash of the original is appended
    instead of silently collapsing every Japanese title onto the same slot --
    which would be the one case the uniqueness rule exists for.
    """
    match = re.search(r"^\s*SAVEPATH\s*=\s*(.+)$", ini_text, re.MULTILINE)
    if not match:
        return "artemis"
    original = match.group(1).strip()
    tail = re.split(r"[\\/]", original)[-1]
    cleaned = re.sub(r"[^a-z0-9_]", "", tail.lower())
    if cleaned:
        # Keep it short: the engine stores this as a directory name.
        return cleaned[:32]
    return "artemis_" + hashlib.sha1(original.encode("utf-8")).hexdigest()[:10]


def copy_sdk_build(sdk, web, flavour):
    src = os.path.join(sdk, "WebAssembly", flavour) if flavour == "asyncify" else os.path.join(sdk, "WebAssembly")
    if not os.path.isdir(src):
        raise SystemExit(f"no such official build: {src}")
    os.makedirs(web, exist_ok=True)
    copied = []
    for name in ("artemis.js", "artemis.wasm", "artemis.html"):
        target = os.path.join(web, name)
        shutil.copy2(os.path.join(src, name), target)
        copied.append((name, os.path.getsize(target)))
    for name, size in copied:
        print(f"  {name}: {size:,} bytes")
    return src


def embed_package_size(web):
    """
    Re-embed `artemis.data` into `artemis.js`'s metadata, replacing
    `ApplyWasmData.exe`.

    The metadata is the single `loadPackage({...})` argument near the top of
    `artemis.js`, and it carries the pack's size in **two** places:

        loadPackage({"files":[{"filename":"/root.pfs","start":0,"end":236,"audio":0}],
                     "remote_package_size":3752,"package_uuid":"..."})

    `remote_package_size` drives the download; each file entry's `end` is the
    byte range yielded to `FS_createDataFile`. Patching only the former leaves
    the DataRequest reading a truncated buffer, and the failure is silent -- the
    canvas stays 1x1, no console message is printed, and the engine never issues
    its first `<ASYNCIFY_URL>` request. That is exactly what the first version of
    this function did, and it is why the deployment appeared to break merely by
    moving to a subdirectory.

    The uuid is replaced too (with a random one, as the Windows tool does): it
    keys the asyncify file cache, so leaving a stale uuid next to a changed pack
    can serve stale bytes.
    """
    data = os.path.join(web, "artemis.data")
    size = os.path.getsize(data)
    js = os.path.join(web, "artemis.js")
    text = open(js, "rb").read().decode("utf-8", "replace")

    match = re.search(r'"remote_package_size":(\d+)', text)
    if not match:
        raise SystemExit(f"{js}: no remote_package_size field to update")
    old_size = match.group(1)

    # The file entries sit in the same blob, immediately before
    # remote_package_size. They are byte ranges of the downloaded pack, and in
    # asyncify mode the whole pack is one `system.ini`, so the single entry must
    # span 0..size. Anything else means the metadata no longer matches what this
    # script writes, and guessing would reintroduce the silent truncation.
    head_start = text.rfind("loadPackage(", 0, match.start())
    if head_start < 0:
        raise SystemExit(f"{js}: found remote_package_size but no loadPackage call")
    blob = text[head_start:match.start()]
    entries = list(re.finditer(r'\{"filename":"([^"]*)","start":(\d+),"end":(\d+),"audio":(\d+)\}', blob))
    if len(entries) != 1:
        raise SystemExit(
            f"{js}: expected the single-file metadata this script writes, "
            f"found {len(entries)} file entries")
    old = entries[0]
    rebuilt_entry = (f'{{"filename":"{old.group(1)}","start":0,'
                     f'"end":{size},"audio":{old.group(4)}}}')
    blob = blob[:old.start()] + rebuilt_entry + blob[old.end():]

    # Rebuild as: [everything up to the key] + patched blob + key + new number +
    # [everything after the old number]. The key literal must be re-emitted --
    # `match` spans all of `"remote_package_size":NNN`, not just the digits.
    text = (text[:head_start] + blob + '"remote_package_size":' + str(size)
            + text[match.end(1):])
    # The uuid keys the asyncify file cache; leaving a stale one beside a changed
    # pack lets the engine serve previously cached bytes.
    text = re.sub(r'"package_uuid":"[^"]*"', f'"package_uuid":"{uuid.uuid4()}"', text)
    open(js, "wb").write(text.encode("utf-8"))
    print(f"  remote_package_size = {size:,} (was {old_size}), "
          f"file range {old.group(2)}..{old.group(3)} -> 0..{size}")
    return size


def build_hashlist(game, out):
    """
    Write the optional `ASYNCIFY_HASH_LIST_URL` document.

    Format rules that abort the whole load if broken -- all three are from the
    readme and each one was hit during the first attempt:

    - one `path,sha1` per line, **LF** separated;
    - the path is relative to ASYNCIFY_URL with **no leading `./`** (unlike the
      WASM SYNC list, which requires `./`);
    - **no trailing newline**. The engine splits the body on LF, and an empty final
      element is reported as `Detected invalid hash entry. ` with an empty
      argument, which fails the load rather than being skipped.

    Hashes must be lowercase 40-character hex.
    """
    lines = []
    for dirpath, _dirs, names in os.walk(game):
        for name in names:
            full = os.path.join(dirpath, name)
            rel = os.path.relpath(full, game).replace(os.sep, "/")
            digest = hashlib.sha1()
            with open(full, "rb") as handle:
                for chunk in iter(lambda: handle.read(1 << 20), b""):
                    digest.update(chunk)
            lines.append(f"{rel},{digest.hexdigest()}")
    lines.sort()
    with open(out, "w", newline="") as handle:
        handle.write("\n".join(lines))
    tail = open(out, "rb").read()[-1:]
    print(f"  hashlist.txt: {len(lines):,} entries, trailing newline = {tail == b'\n'}")


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--game", required=True,
                        help="the loose tree extract-pfs.mjs produced (its system.ini is read from here)")
    parser.add_argument("--out", required=True, help="deployment directory")
    parser.add_argument("--port", type=int, default=4700, help="port the deployment will be served on")
    parser.add_argument("--savepath", default=None, help="override the derived [WASM] SAVEPATH")
    parser.add_argument("--asyncify-url", default=None,
                        help="explicit ASYNCIFY_URL; default is http://127.0.0.1:<port>/game/")
    parser.add_argument("--sdk", default=os.path.join(REPO, ".ref", "artemis"),
                        help="path to the official Artemis SDK checkout")
    parser.add_argument("--flavour", choices=("asyncify", "plain"), default="asyncify",
                        help="official build to deploy; asyncify reads loose files, plain loads the whole pack")
    parser.add_argument("--hashlist", action="store_true",
                        help="also write hashlist.txt (needs ASYNCIFY_HASH_LIST_URL to be honoured)")
    args = parser.parse_args()

    game = os.path.abspath(args.game)
    out = os.path.abspath(args.out)
    ini = os.path.join(game, "system.ini")
    if not os.path.isfile(ini):
        raise SystemExit(f"{ini} not found; run ports/artemis/tools/extract-pfs.mjs first")

    sdk = os.path.abspath(args.sdk)
    if not os.path.isdir(os.path.join(sdk, "WebAssembly")):
        raise SystemExit(
            f"{sdk}/WebAssembly not found.\n"
            "The official Artemis SDK is not redistributable and is not committed; place your "
            "licensed copy under .ref/artemis or pass --sdk."
        )

    print(f"== deploying the official {args.flavour} build to {out} ==")

    # Order matters: the pack must be built from the *patched* system.ini, because
    # the engine reads system.ini out of artemis.data, not off the server.
    savepath = args.savepath
    if savepath is None:
        savepath = detect_savrpath(open(ini, "rb").read().decode("utf-8", "replace"))
    asyncify_url = args.asyncify_url or f"http://127.0.0.1:{args.port}/game/"
    extra = {"SAVEPATH": savepath}
    if args.flavour == "asyncify":
        extra["ASYNCIFY_URL"] = asyncify_url
        if args.hashlist:
            extra["ASYNCIFY_HASH_LIST_URL"] = f"http://127.0.0.1:{args.port}/hashlist.txt"

    print("== system.ini [WASM] ==")
    patch_system_ini(ini, extra)

    web = os.path.join(out, "web")
    print("== official build ==")
    copy_sdk_build(sdk, web, args.flavour)

    print("== artemis.data (system.ini only) ==")
    payload = open(ini, "rb").read()
    # A `system.ini` that does not start with its own first byte is a sign the
    # caller fed in a captured stdout rather than the file: `pfs-grep --dump`
    # writes progress lines to stdout *before* the payload, so `> system.ini`
    # silently produces a corrupted config. Failing loudly here is worth it,
    # because the symptom otherwise surfaces much later as an unrelated crash
    # inside the engine's INI parser.
    if b"\n" in payload[:200] and not payload.lstrip()[:1] in (b";", b"[", b"#"):
        raise SystemExit(f"{ini}: does not look like a system.ini (first bytes: {payload[:60]!r})")
    write_pf8(os.path.join(web, "artemis.data"), [("system.ini", payload)])
    embed_package_size(web)

    if args.hashlist:
        print("== hash list ==")
        build_hashlist(game, os.path.join(web, "hashlist.txt"))

    print(f"""
== serve it ==
    # from the deployment directory, so that /web/ and /game/ are both reachable
    cd {out} && python -m http.server {args.port} --bind 127.0.0.1

    # then open
    http://127.0.0.1:{args.port}/web/artemis.html

The page must be served over HTTP: the engine cannot read local files, and asyncify
mode fetches every asset from the server as the script asks for it.
""")


if __name__ == "__main__":
    main()
