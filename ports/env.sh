#!/usr/bin/env bash
#
# Shared environment for the build recipes under `ports/`.
#
# Every script here drives an **upstream** engine checkout plus an Emscripten SDK, and both live
# outside the repository. Writing those locations into each script is what made this tree
# unportable: the original copies hardcoded `D:/online_gal/.ref/...` and `D:/emsdk`, so a clone
# anywhere else could not build. This file resolves them once, by search, and every recipe sources
# it.
#
# Resolution order for each location is: **explicit environment variable first**, then the
# conventional place on this machine, then the in-repo location a fresh clone would use. Sourcing
# is idempotent, so a script may source it without checking.
#
# Usage:
#
#     . "$(dirname "$0")/../env.sh"    # from ports/<engine>/build/
#     . "$(dirname "$0")/../env.sh"    # from ports/ons/build/, etc.
#
# On Windows the recipes run under Git Bash, where paths are POSIX while the tools want either
# form. `ports_unix` / `ports_win` convert; both are identity functions elsewhere.

if [ -n "${PORTS_ENV_SOURCED:-}" ]; then
  return 0
fi
PORTS_ENV_SOURCED=1

# `<root>/ports/env.sh` -> `<root>`
PORTS_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
export PORTS_ROOT

# Upstream checkouts. Gitignored and fetched by the user, because they are not ours to
# redistribute; see the root `.gitignore` for what each one is.
REF_DIR="${REF_DIR:-$PORTS_ROOT/.ref}"
export REF_DIR

# --- Emscripten ---------------------------------------------------------------------------
#
# `EMSDK_DIR` is honoured if already set. Otherwise the SDK is searched for by looking for the
# `emcc` driver, because a directory that merely *exists* is not proof the SDK is built out — an
# interrupted `emsdk install` leaves a tree with no compiler in it.
if [ -z "${EMSDK_DIR:-}" ]; then
  for ports_emsdk_candidate in /d/emsdk "$PORTS_ROOT/.ref/emsdk" "$HOME/emsdk" "$(cygpath -u 'C:/emsdk' 2>/dev/null)"; do
    [ -n "$ports_emsdk_candidate" ] || continue
    if [ -x "$ports_emsdk_candidate/upstream/emscripten/emcc" ]; then
      EMSDK_DIR="$ports_emsdk_candidate"
      break
    fi
  done
fi
export EMSDK_DIR="${EMSDK_DIR:-}"

# --- Path conversion ----------------------------------------------------------------------
#
# Identity on Linux/macOS: `cygpath` only exists under Git Bash / MSYS.

ports_unix() {
  if command -v cygpath >/dev/null 2>&1; then cygpath -u "$1"; else printf '%s' "$1"; fi
}

ports_win() {
  if command -v cygpath >/dev/null 2>&1; then cygpath -w "$1"; else printf '%s' "$1"; fi
}

export -f ports_unix ports_win 2>/dev/null || true

# --- Emscripten defaults ------------------------------------------------------------------
#
# `EM_CONFIG` and `EMSDK` are what the emsdk tooling reads; the tool directories go on `PATH`
# because `emsdk_env.sh` does not reliably do it from a non-interactive shell (the ONS recipe
# documents that `command -v emcc` came back empty after sourcing it).

if [ -n "$EMSDK_DIR" ]; then
  export EMSDK="$EMSDK_DIR"
  # `EM_CONFIG` is deliberately NOT set.
  #
  # Setting it to the POSIX form (`/d/emsdk/.emscripten`) makes Emscripten's cache sanity string
  # come out as `6.0.9|D:/emsdk/upstream/bin`, while not setting it (or setting the Windows form)
  # gives `6.0.9|D:\emsdk/upstream/bin`. Those two strings differ, so Emscripten reports
  # `config changed, clearing cache` and **wipes the whole cache — including the sysroot zlib that
  # CMake then cannot find** — every time the spelling alternates between runs. The observable
  # failure is a configure-time `Could NOT find ZLIB (missing: ZLIB_LIBRARY ZLIB_INCLUDE_DIR)`,
  # which reads like a missing dependency rather than a self-inflicted cache wipe.
  #
  # Emscripten finds `$EMSDK_DIR/.emscripten` on its own, so setting it buys nothing. A caller who
  # really needs a non-default config can still export `EM_CONFIG` before sourcing this file.
  EMSDK_BIN="$(ports_unix "$EMSDK_DIR")/upstream/emscripten"
  case ":$PATH:" in
    *":$EMSDK_BIN:"*) ;;
    *) PATH="$EMSDK_BIN:$PATH" ;;
  esac
  # `emcc.exe` lives in the SDK root rather than `upstream/bin` on Windows, so both directories
  # have to be present or `emcc` is unresolvable from one of the two layouts.
  EMSDK_UPSTREAM_BIN="$(ports_unix "$EMSDK_DIR")/upstream/bin"
  case ":$PATH:" in
    *":$EMSDK_UPSTREAM_BIN:"*) ;;
    *) PATH="$EMSDK_UPSTREAM_BIN:$PATH" ;;
  esac
  export PATH
fi

# --- Reporting ----------------------------------------------------------------------------

ports_report_env() {
  echo "## repo root:  $PORTS_ROOT"
  echo "## ref dir:    $REF_DIR"
  echo "## emsdk:      ${EMSDK_DIR:-MISSING}"
  echo "## emcc:       $(command -v emcc 2>/dev/null || echo MISSING)"
}
