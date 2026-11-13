/**
 * TEMPORARY layout mock for the library grid. Not real data: the real list
 * comes from `platform.library.list()` once the library island is built.
 * Delete this file (and its import in `src/pages/index.astro`) at that point.
 */
export type MockGameStatus = 'playable' | 'unsupported' | 'unrecognized';

export interface MockGame {
  readonly id: string;
  readonly title: string;
  /**
   * Developer / brand. NOTE: `GameRecord` in `@online-gal/core` has no such field
   * yet; showing it for real needs a core change (separate change, see design.md).
   */
  readonly developer: string | null;
  /** Display label as `runtimeLabel` would give it; `null` when unrecognized. */
  readonly engine: string | null;
  readonly status: MockGameStatus;
  /** Formatted `playTimeMs`, `null` if never played. */
  readonly playTime: string | null;
  /** Cover placeholder hue (0–360). Real cards use the game's own image. */
  readonly hue: number;
}

export const MOCK_GAMES: readonly MockGame[] = [
  { id: 'm1', title: '星之终途', developer: '星海工作室', engine: 'KiriKiri', status: 'playable', playTime: '12 小时', hue: 210 },
  { id: 'm2', title: '月影之下的约定', developer: 'Moonlit Soft', engine: 'ONScripter', status: 'playable', playTime: '3 小时', hue: 260 },
  { id: 'm3', title: '夏日残响', developer: '蝉鸣社', engine: 'KiriKiri', status: 'playable', playTime: null, hue: 30 },
  { id: 'm4', title: '雪色回廊', developer: 'Snowfall', engine: 'Artemis', status: 'unsupported', playTime: null, hue: 190 },
  { id: 'm5', title: '樱花飘落之时', developer: '春风堂', engine: 'ONScripter', status: 'playable', playTime: '27 小时', hue: 340 },
  { id: 'm6', title: '未命名文件夹', developer: null, engine: null, status: 'unrecognized', playTime: null, hue: 0 },
  { id: 'm7', title: '机械少女与旧城的黄昏', developer: '齿轮与花', engine: 'KiriKiri', status: 'playable', playTime: '8 小时', hue: 150 },
  { id: 'm8', title: '雨夜', developer: 'Rainy Day Works', engine: 'Siglus', status: 'unsupported', playTime: null, hue: 230 },
];
