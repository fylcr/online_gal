#!/usr/bin/env python3
"""Packages the repo's real KAG3 demo into the ZIP the KiriKiri web shell can mount.

The shell takes a game through `?game=<name>`, which it mounts by parsing the ZIP's
central directory and reading entry ranges on demand. The demo at
`.ref/ak/demos/aetherkiri-kag3/data` is a *loose-file* real game (startup.tjs plus
scenario/, system/ ...), so it needs packing first. Entries are written at the ZIP
root, which is the layout the shell's common-prefix stripping expects.

Deterministic on purpose: entries are sorted and mtimes pinned, so re-running
produces the same archive and the browser test's numbers stay comparable.
"""
import os
import sys
import zipfile

# Resolved from this file's own location (`<root>/ports/krkr/kirikiroid2/browser-test/`), so the
# script works from any checkout. The demo lives in the gitignored upstream clone under `.ref/`;
# the output stays beside this script, where the browser tests look for it by default.
HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..', '..'))
SRC = os.environ.get('KIRI_DEMO_SRC') or os.path.join(ROOT, '.ref', 'ak', 'demos', 'aetherkiri-kag3', 'data')
OUT = os.environ.get('KIRI_GAME_ZIP') or os.path.join(HERE, 'game.zip')

if not os.path.isdir(SRC):
    print(f'error: demo not found: {SRC}', file=sys.stderr)
    sys.exit(1)

os.makedirs(os.path.dirname(OUT), exist_ok=True)

files = []
for dirpath, _dirnames, filenames in os.walk(SRC):
    for name in filenames:
        full = os.path.join(dirpath, name)
        rel = os.path.relpath(full, SRC).replace('\\', '/')
        files.append((rel, full))
files.sort()

with zipfile.ZipFile(OUT, 'w', zipfile.ZIP_DEFLATED) as archive:
    for rel, full in files:
        info = zipfile.ZipInfo(rel, date_time=(2020, 1, 1, 0, 0, 0))
        info.compress_type = zipfile.ZIP_DEFLATED
        info.external_attr = 0o644 << 16
        with open(full, 'rb') as source:
            archive.writestr(info, source.read())

raw = sum(os.path.getsize(full) for _rel, full in files)
print(f'wrote {OUT}')
print(f'  entries      : {len(files)}')
print(f'  uncompressed : {raw} bytes')
print(f'  archive      : {os.path.getsize(OUT)} bytes')
