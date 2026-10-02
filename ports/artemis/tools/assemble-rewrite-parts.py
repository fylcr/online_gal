#!/usr/bin/env python3
"""Assemble hand-rewritten part files into one product and audit the result.

    python ports/artemis/tools/assemble-rewrite-parts.py <staging-rel> <src-rel>
    python ports/artemis/tools/assemble-rewrite-parts.py --check-only <staging-rel>

`<staging-rel>` is a path under ports/artemis/rewrite-staging without its `.cpp`
extension, e.g. `script/lua_tags_builtin`.  Every `<staging-rel>.partNN.cpp` and
`<staging-rel>.partNN<suffix>.cpp` is discovered, ordered by numeric `NN` and
then by lowercase suffix (`''` < `a` < `b` < ...), and concatenated in that
order.  The result is audited; `ports/artemis/src/<src-rel>` is written only
when the audit is clean, and only when `--check-only` was not requested.

Parts are cut mid-block, so no single part balances braces or parents; only the
concatenation has to.  C++ comments and string literals are therefore stripped
before the structural audits, which keeps `{` in a comment or `:` in a string
from producing a false positive.

Audits: every part non-empty and free of CR / tab / trailing whitespace, max
line width, forbidden generated-layer tokens (a raw `w2c_artemis_fNNN(` call is
the house form for a callee with no rename row, so it alone is allowed), brace
and paren balance, exactly one EnterFrame and one LeaveFrame, duplicate label
definitions, undefined `goto` targets, duplicate resume-point assignments with
the single exception of the zero initialization rule, the segue guard, and bare
`iN` generated locals that the product never declared.

Exit status: 0 clean, 1 audit failure or bad input, 2 usage error.
"""

import os
import re
import sys

REPO = "D:/online_gal"
STAGING = os.path.join(REPO, "ports", "artemis", "rewrite-staging")
SRC = os.path.join(REPO, "ports", "artemis", "src")

PART_RE = re.compile(r"^(.+)\.part(\d{2,})([a-z]*)\.cpp$")
GUARD = "#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE"
MAX_WIDTH = 100

# Generated-layer vocabulary.  Each entry is a regex; a `w2c_` match that is
# immediately followed by `artemis_f<digits>(` is the allowed raw-call form.
FORBIDDEN = [
    (r"\bw2c_\w+", "w2c_ generated symbol"),
    (r"\bvar_(?:[ifjl]\d+|p\d+)\b", "var_* generated local"),
    (r"\bi32_load\b", "i32_load generated memory access"),
    (r"\bi32_store\b", "i32_store generated memory access"),
    (r"\bf32_load\b", "f32_load generated memory access"),
    (r"\bf64_load\b", "f64_load generated memory access"),
    (r"\bUNREACHABLE\b", "UNREACHABLE trap macro"),
    (r"\bFUNC_PROLOGUE\b", "FUNC_PROLOGUE macro"),
    (r"\bFUNC_EPILOGUE\b", "FUNC_EPILOGUE macro"),
    (r"\bmemory_base\b", "memory_base generated global"),
    (r"\bblock_\d+\b", "block_N generated label"),
    (r"\bloop_\d+\b", "loop_N generated label"),
    (r"\bfunc_exit\b", "func_exit generated label"),
    (r"\bspill_frame\b", "spill_frame generated local"),
]

# A bare `iN` local is only a leak in products that never declare one.  Four
# staged products legitimately declare `uint32_t i0 = 0u, i1 = 0u, ...` in their
# part01 and use those names throughout, so an unconditional ban would fire on
# every one of them.  The declaration is therefore the exemption, scoped to the
# product: declaring `i0` in part01 licenses `i0` family-wide, and a product that
# declares nothing (lua_tags_builtin) may not mention `iN` at all.
#
# The declaration must be read per statement, not per line: three products write
# one `uint32_t i0 = 0u, i1 = 0u, ...;` line, so a pattern anchored to the single
# name after `uint32_t` would leave `i1` onwards undeclared and fail them.
DECL_STMT = re.compile(r"\buint32_t\b([^;]*);", re.S)
DECL_NAME = re.compile(r"\b(i[0-9]+)\b")
BARE_IN = re.compile(r"\b(i[0-9]+)\b")


def declared_locals(text):
    """Return the set of `iN` names a `uint32_t ...;` statement declares.

    Every `iN` in the declarator list is taken.  That is deliberately looser
    than real C: an initializer mentioning an `iN` also counts as declaring it.
    Erring that way can only suppress a leak report, never invent one, which is
    the safe direction for a rule whose failure mode is false positives.
    """
    names = set()
    for statement in DECL_STMT.findall(text):
        names.update(DECL_NAME.findall(statement))
    return names

RAW_CALL = r"^w2c_artemis_f\d+\("
ASSIGN_RESUME = re.compile(r"\bresume_point\s*=\s*(\d+)\s*u\b")
# Only the zero-initializing declaration is exempt: `uint32_t resume_point = 0u;`
# A declaration that initializes to anything else is an ordinary assignment and
# still has to be unique, so the exemption keys on the value, not on the `;`.
DECL_RESUME = re.compile(r"\bresume_point\s*=\s*0\s*u\s*;")
LABEL_RE = re.compile(r"^[ \t]*([A-Za-z_]\w*)\s*:;", re.M)
GOTO_RE = re.compile(r"\bgoto\s+([A-Za-z_]\w*)\s*;")


def fail(message):
    """Print an audit failure and terminate with status 1."""
    print("FAIL: %s" % message)
    sys.exit(1)


# --------------------------------------------------------------------------
# discovery
# --------------------------------------------------------------------------
def discover(rel):
    """Return [(number, suffix, path)] for `rel`'s parts, in assembly order."""
    directory = os.path.join(STAGING, os.path.dirname(rel))
    stem = os.path.basename(rel)
    if not os.path.isdir(directory):
        fail("staging directory not found: %s" % directory)
    found = []
    for name in sorted(os.listdir(directory)):
        match = PART_RE.match(name)
        if not match:
            if name.startswith(stem + ".part") and name.endswith(".cpp"):
                fail("invalid part name (want .partNN[/suffix].cpp): %s" % name)
            continue
        if match.group(1) != stem:
            continue
        number, suffix = int(match.group(2)), match.group(3)
        if number < 1:
            fail("part numbers start at 01: %s" % name)
        found.append((number, suffix, os.path.join(directory, name)))
    if not found:
        fail("no %s.partNN.cpp parts under %s" % (stem, directory))
    found.sort(key=lambda item: (item[0], item[1]))
    seen = {}
    for number, suffix, path in found:
        key = (number, suffix)
        if key in seen:
            fail("ambiguous duplicate part %02d%s: %s and %s"
                 % (number, suffix, seen[key], path))
        seen[key] = path
    numbers = sorted({number for number, _, _ in found})
    if numbers[0] != 1:
        fail("part01 is missing (found %s)" % ", ".join("%02d" % n for n in numbers))
    expected = list(range(1, numbers[-1] + 1))
    if numbers != expected:
        missing = [n for n in expected if n not in numbers]
        fail("part numbers are not contiguous through %02d, missing: %s"
             % (numbers[-1], ", ".join("%02d" % n for n in missing)))
    return found


# --------------------------------------------------------------------------
# auditing
# --------------------------------------------------------------------------
def strip_comments(text):
    """Remove //, /* */ comments and string/char literals, keeping line count.

    Replaced regions become spaces, so byte offsets and line numbers survive
    while the structural counts below cannot be fooled by comment text.
    """
    out = list(text)
    i, n = 0, len(text)
    while i < n:
        char = text[i]
        if char == "/" and i + 1 < n and text[i + 1] == "/":
            while i < n and text[i] != "\n":
                out[i] = " "
                i += 1
        elif char == "/" and i + 1 < n and text[i + 1] == "*":
            out[i] = out[i + 1] = " "
            i += 2
            while i < n and not (text[i] == "*" and i + 1 < n and text[i + 1] == "/"):
                if text[i] != "\n":
                    out[i] = " "
                i += 1
            if i < n:
                out[i] = out[i + 1] = " "
                i += 2
        elif char in "\"'":
            quote = char
            out[i] = " "
            i += 1
            while i < n:
                if text[i] == "\\" and i + 1 < n:
                    if text[i] != "\n":
                        out[i] = " "
                    out[i + 1] = " " if text[i + 1] != "\n" else "\n"
                    i += 2
                    continue
                if text[i] == quote:
                    out[i] = " "
                    i += 1
                    break
                if text[i] != "\n":
                    out[i] = " "
                i += 1
        else:
            i += 1
    return "".join(out)


def audit_lines(label, text, problems):
    """Per-file line hygiene shared by every part and by the product."""
    if not text.strip():
        problems.append("%s: empty" % label)
        return 0
    if "\r" in text:
        problems.append("%s: CR characters present" % label)
    if "\t" in text:
        problems.append("%s: tab characters present x%d" % (label, text.count("\t")))
    trailing = [i for i, line in enumerate(text.split("\n"), 1)
                if line != line.rstrip()]
    if trailing:
        problems.append("%s: trailing whitespace on %d line(s), first %d"
                        % (label, len(trailing), trailing[0]))
    return max((len(line) for line in text.split("\n")), default=0)


def audit_forbidden(label, text, problems):
    """Generated-layer tokens, allowing only the raw w2c_artemis_fNNN( call."""
    scrubbed = re.sub(r"w2c_artemis_f\d+\(", "(", text)
    for pattern, description in FORBIDDEN:
        hits = len(re.findall(pattern, scrubbed))
        if hits:
            problems.append("%s: forbidden %s x%d" % (label, description, hits))


def audit_generated_locals(label, text, problems):
    """Reject bare `iN` locals that this product never declared.

    `iN` is the wasm local numbering, so a surviving `i0`/`i1` is a rewrite that
    kept a generated spelling instead of a named local.  The names are only
    forbidden when the product does not declare them: four staged products
    legitimately declare `uint32_t i0 = 0u, ...` and use them heavily, and the
    audit would otherwise reject those products wholesale.

    The declaration scan is deliberately permissive about the initializer (it
    accepts `uint32_t i0 = 0;` as well as `= 0u;`) because it exists to decide
    whether the name is this product's own local, not to grade the declaration.
    """
    declared = declared_locals(text)
    leaked = {}
    for line_number, line in enumerate(text.split("\n"), 1):
        for name in BARE_IN.findall(line):
            if name not in declared:
                leaked.setdefault(name, []).append(line_number)
    for name in sorted(leaked, key=lambda item: int(item[1:])):
        lines = leaked[name]
        problems.append("%s: undeclared generated local %s x%d (first at line %d)"
                        % (label, name, len(lines), lines[0]))


def audit_guards(label, text, problems):
    """The segue guard must pair up, and every SyncMemoryBase sits under one."""
    lines = text.split("\n")
    depth = 0
    for number, line in enumerate(lines, 1):
        stripped = line.strip()
        if stripped == GUARD:
            depth += 1
        elif stripped == "#endif":
            depth -= 1
            if depth < 0:
                problems.append("%s:%d: #endif without a matching guard"
                                % (label, number))
                depth = 0
        elif stripped.startswith("#if"):
            # Any other conditional is out of scope for this audit.
            continue
        if "SyncMemoryBase(" in stripped and depth == 0:
            problems.append("%s:%d: SyncMemoryBase outside %s"
                            % (label, number, GUARD))
    if depth:
        problems.append("%s: %d unclosed %s" % (label, depth, GUARD))


def unclosed_lines(text, opener, closer):
    """Return (line_no, source_line) for openers never matched by a closer.

    A plain count tells the author only that something is off; naming the
    surviving openers points straight at the missing closer.
    """
    stack = []
    for number, line in enumerate(text.split("\n"), 1):
        for char in line:
            if char == opener:
                stack.append((number, line))
            elif char == closer and stack:
                stack.pop()
    return stack


def audit_structure(label, text, problems):
    """Brace/paren balance, frame counts, labels, gotos and resume points."""
    balance = text.count("{") - text.count("}")
    if balance:
        problems.append("%s: brace balance %+d" % (label, balance))
        for number, line in unclosed_lines(text, "{", "}")[:5]:
            problems.append("%s:%d: unclosed '{' in %s"
                            % (label, number, line.strip()[:60]))
    parens = text.count("(") - text.count(")")
    if parens:
        problems.append("%s: paren balance %+d" % (label, parens))
        for number, line in unclosed_lines(text, "(", ")")[:5]:
            problems.append("%s:%d: unclosed '(' in %s"
                            % (label, number, line.strip()[:60]))

    enter = len(re.findall(r"\bEnterFrame\(\)", text))
    leave = len(re.findall(r"\bLeaveFrame\(\)", text))
    if enter != 1:
        problems.append("%s: EnterFrame count %d, want exactly 1" % (label, enter))
    if leave != 1:
        problems.append("%s: LeaveFrame count %d, want exactly 1" % (label, leave))

    labels = {}
    for match in LABEL_RE.finditer(text):
        name = match.group(1)
        line = text.count("\n", 0, match.start()) + 1
        if name in labels:
            problems.append("%s:%d: duplicate label definition %s (first at %d)"
                            % (label, line, name, labels[name]))
        else:
            labels[name] = line
    gotos = {match.group(1) for match in GOTO_RE.finditer(text)}
    undefined = sorted(gotos - set(labels))
    if undefined:
        problems.append("%s: undefined goto target(s): %s"
                        % (label, ", ".join(undefined)))

    counts = {}
    for line in text.split("\n"):
        if "resume_point" not in line or "//" in line:
            continue
        match = ASSIGN_RESUME.search(line)
        if match:
            value = int(match.group(1))
            if DECL_RESUME.search(line):
                continue  # declaration initializes to zero
            counts[value] = counts.get(value, 0) + 1
    duplicate = sorted(value for value, hits in counts.items()
                       if hits > 1 and value != 0)
    if duplicate:
        problems.append("%s: duplicate resume_point assignment(s): %s"
                        % (label, ", ".join(str(value) for value in duplicate)))
    return len(labels), len(gotos)


def read_part(path):
    # newline="" keeps CR visible: universal-newline translation would fold
    # "\r\n" into "\n" and the CR audit could never fire.
    try:
        with open(path, "r", encoding="utf-8", newline="") as handle:
            return handle.read()
    except OSError as error:
        fail("cannot read %s: %s" % (path, error))
    except UnicodeDecodeError as error:
        fail("not valid UTF-8: %s: %s" % (path, error))


def write_atomic(path, text):
    """Write through a temp file in the target directory, then os.replace."""
    directory = os.path.dirname(path)
    os.makedirs(directory, exist_ok=True)
    temp = os.path.join(directory, ".%s.tmp" % os.path.basename(path))
    try:
        with open(temp, "w", encoding="utf-8", newline="\n") as handle:
            handle.write(text)
        os.replace(temp, path)
    except OSError as error:
        if os.path.exists(temp):
            os.remove(temp)
        fail("cannot write %s: %s" % (path, error))


def selftest():
    """Check that audit_generated_locals separates leaks from real locals.

    The whole risk of this rule is a false positive on a product that declares
    its own `iN` locals, so the check covers both sides: a declared product
    passes, an undeclared one fails, and near-miss identifiers stay clean.
    """
    cases = [
        # (text, want_problems, why)
        ("  uint32_t i0 = 0u, i1 = 0u;\n  i0 = l8 + 1u;\n", 0,
         "declared iN locals are legitimate"),
        ("  uint32_t i0 = 0, i1 = 0;\n  i1 = 255u;\n", 0,
         "braceless zero-initialized declaration still declares"),
        ("      p2 = i0;\n", 1,
         "undeclared bare i0 is a generated-local leak"),
        ("      p2 = i0;\n      p3 = i1;\n", 2,
         "each leaked name is reported once"),
        ("  uint32_t index = 0u;\n  index = i0 < 4u;\n", 1,
         "declaring an unrelated local does not license iN"),
        # Near-misses: identifiers and constants that merely contain or embed
        # an `i` plus digits must never trip the rule.
        ("  uint32_t i = 0u;\n  uint32_t item0 = 0u;\n  i = item0;\n", 0,
         "`i` and `item0` are not bare iN"),
        ("  value = 10u << i0_ = 0;\n", 0,
         "a non-identifier continuation hides no bare iN"),
        ("  x = si0;\n  y = pi1;\n  z = i0x;\n", 0,
         "embedded neighbours are not bare iN"),
        ("  a = 3i0;\n  b = _i0;\n  c = i0_;\n", 0,
         "digit/underscore adjacencies are not bare iN"),
    ]
    failures = []
    for text, want, why in cases:
        problems = []
        audit_generated_locals("selftest", text, problems)
        if len(problems) != want:
            failures.append("%s: wanted %d problem(s), got %d (%s)"
                            % (why, want, len(problems), problems or "none"))
    if failures:
        for failure in failures:
            print("SELFTEST FAIL: %s" % failure)
        return 1
    print("selftest: OK (%d case(s))" % len(cases))
    return 0


# --------------------------------------------------------------------------
def main(argv):
    if "--selftest" in argv[1:]:
        return selftest()
    check_only = False
    args = [arg for arg in argv[1:] if arg != "--check-only"]
    if len(args) < len(argv[1:]):
        check_only = True
    if len(args) == 1:
        rel, target = args[0], args[0] + ".cpp"
    elif len(args) == 2:
        rel, target = args
    else:
        print(__doc__)
        return 2

    parts = discover(rel)
    print("%-13s: %s" % ("staging", rel))
    print("%-13s: %d part(s) -> %s" % ("parts", len(parts), target))
    if check_only:
        print("%-13s: yes (audit only, nothing is written)" % "check-only")

    problems = []
    ordered = []
    chunks = []
    for number, suffix, path in parts:
        label = "%02d%s" % (number, suffix)
        text = read_part(path)
        ordered.append((label, path, len(text.split("\n"))))
        audit_lines("part%s" % label, text, problems)
        chunks.append(text)

    for label, path, lines in ordered:
        print("  %-6s %-52s %5d lines" % (label, os.path.relpath(path, STAGING),
                                          lines))

    body = "".join(chunks)
    code = strip_comments(body)
    width = audit_lines("product", body, problems)
    if width > MAX_WIDTH:
        problems.append("product: max line width %d > %d" % (width, MAX_WIDTH))
    audit_forbidden("product", code, problems)
    audit_generated_locals("product", code, problems)
    audit_guards("product", body, problems)
    labels, gotos = audit_structure("product", code, problems)

    raw_calls = len(re.findall(r"\bw2c_artemis_f\d+\(", code))
    enter = len(re.findall(r"\bEnterFrame\(\)", code))
    leave = len(re.findall(r"\bLeaveFrame\(\)", code))
    guards = body.count(GUARD)
    synced = len(re.findall(r"\bSyncMemoryBase\(", code))

    total = len(body.split("\n"))
    print()
    print("%-13s: %d" % ("product lines", total))
    print("%-13s: %d" % ("max width", width))
    print("%-13s: %d" % ("bytes", len(body.encode("utf-8"))))
    print("%-13s: %d   LeaveFrame: %d" % ("EnterFrame", enter, leave))
    print("%-13s: %d   gotos: %d" % ("labels", labels, gotos))
    print("%-13s: %d   SyncMemoryBase: %d" % ("guards", guards, synced))
    print("%-13s: %d (callees with no rename row)" % ("raw calls", raw_calls))
    print()
    if problems:
        print("AUDIT FAILED (%d problem(s)):" % len(problems))
        for problem in problems:
            print("  - %s" % problem)
        return 1
    print("AUDIT CLEAN")
    if check_only:
        print("check-only: nothing written")
        return 0
    out = os.path.join(SRC, target)
    write_atomic(out, body)
    print("written      : %s (%d bytes)" % (out, len(body.encode("utf-8"))))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
