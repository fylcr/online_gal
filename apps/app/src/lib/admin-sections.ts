/**
 * Site administration areas. `/admin` lists them, and each one is its own page at
 * `/admin/<id>`, the same structure as `/settings`.
 */
export interface AdminSection {
  readonly id: AdminSectionId;
  readonly title: string;
  readonly description: string;
}

export type AdminSectionId = 'project' | 'deployments' | 'domains' | 'console';

export const ADMIN_SECTIONS: readonly AdminSection[] = [
  { id: 'project', title: '项目', description: 'Pages 项目、来源仓库与构建配置' },
  { id: 'deployments', title: '部署', description: '最近的部署，重试失败的部署，回滚生产环境' },
  { id: 'domains', title: '域名', description: '自定义域名与验证状态' },
  { id: 'console', title: '控制台', description: '在浏览器中运行 cf 命令' },
];
