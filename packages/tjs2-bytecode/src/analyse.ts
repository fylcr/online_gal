import type { GameSource, RuntimeHost } from '@online-gal/engine-api';
import {
  disassemble,
  formatDisassembly,
  opcodeHistogram,
  type DisassemblyResult,
  type OpcodeHistogram,
} from './disassemble.js';
import { isTjs2Bytecode, parseTjs2Bytecode, Tjs2FormatError } from './container.js';
import { loadScriptBytes } from './script-bytes.js';

/**
 * What a compiled script tells us about whether this platform can run it.
 *
 * This is the actionable answer to "will my KiriKiri game work here?", and it
 * replaces a guess with a measurement. The runtime currently interprets TJS2
 * *source*; a compiled `.ks` is bytecode, so the honest verdict is that the
 * script parsed but cannot be executed. What this analysis adds beyond that is
 * *how much* of the script is understood — the opcode histogram proves the
 * container and width table agree with the real compiler, which is the
 * prerequisite for ever executing it.
 */
export interface BytecodeVerdict {
  /** Path that was analysed. */
  readonly path: string;
  readonly kind: 'bytecode' | 'source' | 'unknown';
  /** Byte length of the payload. */
  readonly byteLength: number;
  /** Contexts found, when it parsed as bytecode. */
  readonly objectCount: number;
  /** Instruction count across every context. */
  readonly instructionCount: number;
  /** Contexts whose instruction walk consumed the array exactly. */
  readonly completeWalks: number;
  /** Opcodes outside the width table; non-zero means the walk is unreliable. */
  readonly unknownOpcodes: readonly number[];
  /** Most frequent opcodes, for a quick impression of the script. */
  readonly topOpcodes: readonly { readonly name: string; readonly count: number }[];
  /** String constants, which for a KAG scenario include the dialogue. */
  readonly stringCount: number;
  /** Whether every context walked cleanly. */
  readonly structurallySound: boolean;
  /** Human-readable explanation of what this means for running the game. */
  readonly summary: string;
  /** Full listing, present only when requested. */
  readonly disassembly?: string;
}

export interface AnalyseOptions {
  /** Include the full instruction listing in the result. */
  readonly includeDisassembly?: boolean;
  /** How many opcodes to report in `topOpcodes`. */
  readonly topOpcodeCount?: number;
}

/**
 * Analyses one script file.
 *
 * Never throws for a malformed file: a game directory routinely contains
 * partially-copied or unrelated files, and the caller wants a verdict for each
 * rather than an exception that aborts the scan.
 */
export async function analyseScriptBytes(
  path: string,
  bytes: Uint8Array,
  options: AnalyseOptions = {},
): Promise<BytecodeVerdict> {
  const base = {
    path,
    byteLength: bytes.byteLength,
  };

  // A compiled script may be wrapped: `.ks` files are frequently zlib-compressed
  // behind a small header, so the payload is unwrapped first.
  let payload = bytes;
  let kind: BytecodeVerdict['kind'] = 'unknown';
  if (isTjs2Bytecode(bytes)) {
    kind = 'bytecode';
  } else {
    try {
      const decoded = await loadScriptBytes(bytes, path);
      const unwrapped = decoded.bytes;
      if (decoded.compiled && unwrapped !== undefined && isTjs2Bytecode(unwrapped)) {
        payload = unwrapped;
        kind = 'bytecode';
      } else {
        kind = 'source';
      }
    } catch {
      kind = 'unknown';
    }
  }

  if (kind !== 'bytecode') {
    return {
      ...base,
      kind,
      objectCount: 0,
      instructionCount: 0,
      completeWalks: 0,
      unknownOpcodes: [],
      topOpcodes: [],
      stringCount: 0,
      structurallySound: false,
      summary:
        kind === 'source'
          ? '明文脚本，可直接由本平台的 TJS2 源码解释器处理。'
          : '无法识别为 TJS2 字节码或明文脚本。',
    };
  }

  let program;
  try {
    program = parseTjs2Bytecode(payload);
  } catch (error) {
    const detail = error instanceof Tjs2FormatError ? error.message : String(error);
    return {
      ...base,
      kind: 'bytecode',
      objectCount: 0,
      instructionCount: 0,
      completeWalks: 0,
      unknownOpcodes: [],
      topOpcodes: [],
      stringCount: 0,
      structurallySound: false,
      summary: `字节码容器解析失败：${detail}`,
    };
  }

  const result = disassemble(program);
  const histogram = opcodeHistogram(result);
  return {
    ...base,
    kind: 'bytecode',
    objectCount: program.objects.length,
    instructionCount: histogram.total,
    completeWalks: result.objects.filter((object) => object.complete).length,
    unknownOpcodes: histogram.unknown,
    topOpcodes: topOpcodes(histogram, options.topOpcodeCount ?? 8),
    stringCount: program.constants.strings.length,
    structurallySound:
      result.objects.every((object) => object.complete) && histogram.unknown.length === 0,
    summary: summarise(result, histogram),
    ...(options.includeDisassembly ? { disassembly: formatDisassembly(result) } : {}),
  };
}

/** Reads and analyses a script from a game source. */
export async function analyseScript(
  source: GameSource,
  path: string,
  options: AnalyseOptions = {},
): Promise<BytecodeVerdict> {
  const bytes = await source.readFile(path);
  return analyseScriptBytes(path, bytes, options);
}

function topOpcodes(
  histogram: OpcodeHistogram,
  count: number,
): { name: string; count: number }[] {
  return [...histogram.byName.entries()]
    .sort((a, b) => b[1] - a[1])
    .slice(0, count)
    .map(([name, value]) => ({ name, count: value }));
}

/** Builds the human-readable verdict from the walk results. */
function summarise(result: DisassemblyResult, histogram: OpcodeHistogram): string {
  const contexts = result.objects.length;
  const incomplete = result.objects.filter((object) => !object.complete).length;

  if (incomplete > 0 || histogram.unknown.length > 0) {
    return (
      `解析出 ${contexts} 个上下文、${histogram.total} 条指令，但` +
      (incomplete > 0 ? `${incomplete} 个上下文的指令流未能对齐` : '') +
      (incomplete > 0 && histogram.unknown.length > 0 ? '，且' : '') +
      (histogram.unknown.length > 0
        ? `出现 ${histogram.unknown.length} 个未知操作码`
        : '') +
      '。操作码宽度表与真实编译器不一致，说明这一版字节码需要进一步逆向。'
    );
  }

  return (
    `解析出 ${contexts} 个上下文、${histogram.total} 条指令，指令流全部对齐、无未知操作码。` +
    '容器与操作码宽度表已得到验证，但本平台目前只解释 TJS2 源码、没有字节码 VM，' +
    '因此这个脚本仍然**无法执行**——它的价值是证明移植 VM 的格式前提已经成立。'
  );
}

/** Aggregated verdict for a whole game directory. */
export interface GameBytecodeReport {
  readonly scripts: readonly BytecodeVerdict[];
  readonly bytecodeCount: number;
  readonly sourceCount: number;
  readonly failing: readonly BytecodeVerdict[];
  /** True when at least one script was bytecode and structurally sound. */
  readonly anyRunnable: boolean;
}

/**
 * Analyses every `.ks`/`.tjs` in a directory.
 *
 * Bounded by `limit` because a large game ships thousands of scripts and a
 * compatibility answer does not need all of them.
 */
export async function analyseGame(
  host: Pick<RuntimeHost, 'source' | 'log'>,
  options: AnalyseOptions & { readonly limit?: number } = {},
): Promise<GameBytecodeReport> {
  const limit = options.limit ?? 200;
  const paths = await collectScriptPaths(host.source, limit);
  const scripts: BytecodeVerdict[] = [];

  for (const path of paths) {
    try {
      scripts.push(await analyseScript(host.source, path, options));
    } catch (error) {
      host.log('debug', `cannot analyse ${path}: ${String(error)}`);
    }
  }

  return {
    scripts,
    bytecodeCount: scripts.filter((script) => script.kind === 'bytecode').length,
    sourceCount: scripts.filter((script) => script.kind === 'source').length,
    failing: scripts.filter(
      (script) => script.kind === 'bytecode' && !script.structurallySound,
    ),
    // Until a bytecode VM exists, nothing here is runnable; the flag records
    // whether the *format* was understood, which is what gates that work.
    anyRunnable: false,
  };
}

/** Walks the tree collecting script paths, breadth-first and bounded. */
async function collectScriptPaths(
  source: GameSource,
  limit: number,
): Promise<readonly string[]> {
  const found: string[] = [];
  const queue: string[] = [''];
  const seen = new Set<string>();

  while (queue.length > 0 && found.length < limit) {
    const dir = queue.shift()!;
    if (seen.has(dir)) continue;
    seen.add(dir);

    let entries: readonly string[];
    try {
      entries = await source.list(dir);
    } catch {
      continue;
    }

    for (const entry of entries) {
      if (found.length >= limit) break;
      const path = dir ? `${dir}/${entry}` : entry;
      if (entry.endsWith('/')) {
        queue.push(path.slice(0, -1));
        continue;
      }
      const lower = entry.toLowerCase();
      if (lower.endsWith('.ks') || lower.endsWith('.tjs')) found.push(path);
    }
  }

  return found;
}
