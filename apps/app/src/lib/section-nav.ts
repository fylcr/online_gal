export const SECTION_NAV = [
  { href: '/', label: '游戏库' },
  { href: '/settings', label: '设置' },
] as const;

function normalizePath(path: string): string {
  return path.replace(/\/+$/, '') || '/';
}

export function matchesSection(href: string, pathname: string): boolean {
  const target = normalizePath(href);
  const current = normalizePath(pathname);
  return target === '/' ? current === '/' : current === target || current.startsWith(`${target}/`);
}
