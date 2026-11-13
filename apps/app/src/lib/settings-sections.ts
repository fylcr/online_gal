/**
 * Top-level settings groups. `/settings` lists them; each one is its own page
 * at `/settings/<id>`. Fields come from `AppSettings` in `@online-gal/core`
 * (packages/core/src/types.ts); the items inside each group are filled in later.
 */
export interface SettingsSection {
  readonly id: string;
  readonly title: string;
  readonly description: string;
}

export const SETTINGS_SECTIONS: readonly SettingsSection[] = [
  { id: 'playback', title: '播放', description: '文字速度、画面尺寸' },
  { id: 'audio', title: '音频', description: '音乐、音效、语音音量与静音' },
  { id: 'engines', title: '引擎', description: '本构建可用的引擎与不可用原因' },
  { id: 'data', title: '数据', description: '存档导入导出、游戏库存储' },
  { id: 'advanced', title: '高级', description: '诊断信息' },
];
