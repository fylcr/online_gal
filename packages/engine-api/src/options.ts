/**
 * Cross-engine option keys.
 *
 * Ported from AetherKiri's `bridge/engine_api/include/engine_options.h`, which
 * unifies the option strings its C++ providers and bootstrap agree on. Porting
 * the key names (rather than inventing new ones) means a per-game tuning note
 * written against the native players still means the same thing here, and it
 * keeps the per-engine option tables comparable across the two codebases.
 *
 * Options that only make sense for a native host are deliberately **not**
 * ported, because in a browser there is nothing for them to select:
 *
 * - `renderer` / `render_backend` / `frame_output` — AetherKiri chooses between
 *   Godot-owned GPU resources, an external GPU render target and a CPU
 *   readback fallback. Here the runtime draws straight into the host canvas, so
 *   there is exactly one path and no option to express.
 * - GPU/PSB cache budgets — the browser's own image and decode caches are
 *   managed by `AssetManager`, not by the engine core.
 * - `text_translation.*` — supplied by AetherKiri's private provider.
 */

/** Frame rate limit in frames per second; 0 follows the display refresh. */
export const OPTION_FPS_LIMIT = 'fps_limit';

/** Memory profile: `balanced` or `aggressive`. */
export const OPTION_MEMORY_PROFILE = 'memory_profile';

/** Archive cache entry limit, mirroring AetherKiri's `archive_cache_count`. */
export const OPTION_ARCHIVE_CACHE_COUNT = 'archive_cache_count';

/**
 * Auto-path cache entry limit.
 *
 * KiriKiri resolves a bare filename by searching the archive tree and caches
 * the result; a browser pays the same cost per lookup, so the same tuning knob
 * applies.
 */
export const OPTION_AUTOPATH_CACHE_COUNT = 'autopath_cache_count';

/**
 * Surface unrecognised script commands as errors instead of absorbing them.
 *
 * Direct analogue of AetherKiri's `mock_enabled`/`ENGINE_OPTION_MOCK_ENABLED`:
 * the native engine silently substitutes mock objects for missing plugins so a
 * game keeps running; turning that off surfaces real errors for debugging.
 * The browser runtime has no plugins to mock, so the failure mode is an
 * unsupported KAG tag or NScripter command — this option decides whether it is
 * logged at trace level (default) or reported through `RuntimeInstance.report`.
 */
export const OPTION_STRICT_MODE = 'strict_mode';

/** Emit a per-command compatibility report at the end of a session. */
export const OPTION_COMPATIBILITY_REPORT = 'compatibility_report';

/** Decode an `.xp3` whose entries are XOR-protected, using this key. */
export const OPTION_XP3_KEY = 'xp3_key';

/** Script text encoding override, e.g. `shift_jis` or `gbk`. */
export const OPTION_SCRIPT_ENCODING = 'script_encoding';

/** Text speed in characters per second; 0 reveals text instantly. */
export const OPTION_TEXT_SPEED = 'text_speed';

/** Values for `OPTION_MEMORY_PROFILE`. */
export const MEMORY_PROFILE_BALANCED = 'balanced';
export const MEMORY_PROFILE_AGGRESSIVE = 'aggressive';

/** Every option key the platform understands. */
export const KNOWN_OPTION_KEYS = [
  OPTION_FPS_LIMIT,
  OPTION_MEMORY_PROFILE,
  OPTION_ARCHIVE_CACHE_COUNT,
  OPTION_AUTOPATH_CACHE_COUNT,
  OPTION_STRICT_MODE,
  OPTION_COMPATIBILITY_REPORT,
  OPTION_XP3_KEY,
  OPTION_SCRIPT_ENCODING,
  OPTION_TEXT_SPEED,
] as const;

export type OptionKey = (typeof KNOWN_OPTION_KEYS)[number];

/** How an option value should be presented and validated in the UI. */
export type OptionValueKind = 'boolean' | 'integer' | 'string' | 'enum';

export interface OptionDescriptor {
  readonly key: OptionKey;
  readonly label: string;
  readonly kind: OptionValueKind;
  readonly defaultValue: string | number | boolean;
  /** Allowed values, for `enum`. */
  readonly values?: readonly string[];
  readonly min?: number;
  readonly max?: number;
  readonly description: string;
}

/**
 * The option surface a runtime advertises.
 *
 * AetherKiri's providers accept a flat `engine_option_t` key/value pair and
 * ignore what they do not understand. Declaring the table per provider instead
 * lets the launcher show only the knobs that do something, and lets a probe
 * report which tuning a given game is expected to need.
 */
export interface OptionSchema {
  readonly key: OptionKey;
  readonly descriptor: OptionDescriptor;
}

const DESCRIPTORS: Readonly<Record<OptionKey, OptionDescriptor>> = {
  [OPTION_FPS_LIMIT]: {
    key: OPTION_FPS_LIMIT,
    label: '帧率上限',
    kind: 'integer',
    defaultValue: 60,
    min: 0,
    max: 240,
    description: '0 表示跟随显示器刷新率。',
  },
  [OPTION_MEMORY_PROFILE]: {
    key: OPTION_MEMORY_PROFILE,
    label: '内存策略',
    kind: 'enum',
    defaultValue: MEMORY_PROFILE_BALANCED,
    values: [MEMORY_PROFILE_BALANCED, MEMORY_PROFILE_AGGRESSIVE],
    description: '激进策略会更快释放已解码图像，适合资源量大的游戏。',
  },
  [OPTION_ARCHIVE_CACHE_COUNT]: {
    key: OPTION_ARCHIVE_CACHE_COUNT,
    label: '归档缓存条目数',
    kind: 'integer',
    defaultValue: 512,
    min: 0,
    max: 65536,
    description: '归档读取器缓存的解压文件数量上限。',
  },
  [OPTION_AUTOPATH_CACHE_COUNT]: {
    key: OPTION_AUTOPATH_CACHE_COUNT,
    label: '路径查找缓存条目数',
    kind: 'integer',
    defaultValue: 2048,
    min: 0,
    max: 65536,
    description: '引擎按文件名搜索归档时的结果缓存上限。',
  },
  [OPTION_STRICT_MODE]: {
    key: OPTION_STRICT_MODE,
    label: '严格模式',
    kind: 'boolean',
    defaultValue: false,
    description: '不支持的脚本命令将作为错误上报，而不是仅记录日志后跳过。',
  },
  [OPTION_COMPATIBILITY_REPORT]: {
    key: OPTION_COMPATIBILITY_REPORT,
    label: '兼容性报告',
    kind: 'boolean',
    defaultValue: true,
    description: '会话结束时汇总未支持的命令，用于判断兼容性。',
  },
  [OPTION_XP3_KEY]: {
    key: OPTION_XP3_KEY,
    label: 'XP3 解密密钥',
    kind: 'string',
    defaultValue: '',
    description: '部分 XP3 条目的 XOR 密钥，通常可从 startup.tjs 得到。',
  },
  [OPTION_SCRIPT_ENCODING]: {
    key: OPTION_SCRIPT_ENCODING,
    label: '脚本编码',
    kind: 'string',
    defaultValue: '',
    description: '留空表示自动探测；可指定 shift_jis 或 gbk。',
  },
  [OPTION_TEXT_SPEED]: {
    key: OPTION_TEXT_SPEED,
    label: '文字速度',
    kind: 'integer',
    defaultValue: 45,
    min: 0,
    max: 2000,
    description: '每秒显示字数；0 表示瞬间显示。',
  },
};

/** Returns the descriptor for an option key. */
export function optionDescriptor(key: OptionKey): OptionDescriptor {
  return DESCRIPTORS[key];
}

/** Validates and coerces a raw option value from the launcher or a game config. */
export function coerceOption(
  key: OptionKey,
  raw: unknown,
): string | number | boolean | null {
  const descriptor = DESCRIPTORS[key];
  switch (descriptor.kind) {
    case 'boolean':
      if (typeof raw === 'boolean') return raw;
      if (typeof raw === 'string') {
        if (['1', 'true', 'yes', 'on'].includes(raw.toLowerCase())) return true;
        if (['0', 'false', 'no', 'off'].includes(raw.toLowerCase())) return false;
      }
      return null;
    case 'integer': {
      const value = typeof raw === 'number' ? raw : Number(raw);
      if (!Number.isFinite(value)) return null;
      const rounded = Math.trunc(value);
      const min = descriptor.min ?? Number.NEGATIVE_INFINITY;
      const max = descriptor.max ?? Number.POSITIVE_INFINITY;
      if (rounded < min || rounded > max) return null;
      return rounded;
    }
    case 'enum': {
      const value = String(raw);
      return descriptor.values?.includes(value) ? value : null;
    }
    default:
      return typeof raw === 'string' ? raw : raw === null || raw === undefined ? null : String(raw);
  }
}

/** Builds a full option table for the keys a provider advertises. */
export function schemaFor(keys: readonly OptionKey[]): readonly OptionSchema[] {
  return keys.map((key) => ({ key, descriptor: DESCRIPTORS[key] }));
}
