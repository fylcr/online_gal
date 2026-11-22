import { spawnSync } from 'node:child_process';
import { appendFile, readFile, readdir, rm, stat } from 'node:fs/promises';
import { join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const root = fileURLToPath(new URL('../', import.meta.url));
const setupEntries = ['index.html', '_astro', 'sw.js', '_worker.js', '_routes.json', '_headers'];

export async function choosePhase(args, env, fetchProject = fetch) {
  if (args.length) {
    const match = args.length === 1 && /^--phase=(setup|site)$/.exec(args[0]);
    if (!match) throw new Error('请传入 --phase=setup 或 --phase=site。');
    return { phase: match[1], rule: 1 };
  }
  if (env.ONLINE_GAL_SITE) return { phase: 'site', rule: 2, record: env.ONLINE_GAL_SITE };
  if (env.CF_PAGES === '1') return { phase: 'setup', rule: 3 };
  const { CLOUDFLARE_ACCOUNT_ID: account, CLOUDFLARE_API_TOKEN: token, ONLINE_GAL_PAGES_PROJECT: project } = env;
  if (account && token && project) {
    let body;
    try {
      const response = await fetchProject(
        `https://api.cloudflare.com/client/v4/accounts/${encodeURIComponent(account)}/pages/projects/${encodeURIComponent(project)}`,
        { headers: { authorization: `Bearer ${token}` }, redirect: 'error', signal: AbortSignal.timeout(15_000) },
      );
      body = await response.json();
      if (!response.ok || body?.success !== true || !body.result) throw new Error();
    } catch {
      throw new Error('无法读取 Cloudflare Pages 项目配置。');
    }
    const variable = body.result.deployment_configs?.production?.env_vars?.ONLINE_GAL_SITE;
    if (variable && variable.value !== '') return { phase: 'site', rule: 4, record: variable.value };
    return { phase: 'setup', rule: 4 };
  }
  throw new Error('无法判断构建阶段，请传 --phase=setup 或 --phase=site。');
}

async function exists(path) {
  try {
    await stat(path);
    return true;
  } catch (error) {
    if (error.code === 'ENOENT') return false;
    throw error;
  }
}

async function checkContents(dir, markers, rejectRegistry = false) {
  for (const entry of await readdir(dir, { withFileTypes: true })) {
    const path = join(dir, entry.name);
    if (entry.isDirectory()) {
      if (rejectRegistry && entry.name === 'registry') throw new Error(`初始化构建包含注册表：${path}`);
      await checkContents(path, markers, rejectRegistry);
    }
    if (!entry.isFile()) continue;
    const data = await readFile(path);
    if (markers.some((marker) => entry.name.includes(marker) || data.includes(marker))) {
      throw new Error(`构建产物包含其他阶段的代码：${path}`);
    }
  }
}

export async function verifyOutput(phase, out, repo = root) {
  if (phase === 'site') {
    if (await exists(join(repo, 'functions/__setup')) || await exists(join(out, '_worker.js'))) {
      throw new Error('站点构建仍包含初始化 Function。');
    }
    await checkContents(out, ['__setup', 'ONLINE_GAL_SETUP']);
    return;
  }
  const entries = await readdir(out);
  for (const entry of entries) {
    if (entry !== 'favicon.svg' && !setupEntries.includes(entry)) throw new Error(`初始化构建包含额外文件：${entry}`);
  }
  for (const entry of setupEntries) {
    if (!entries.includes(entry)) throw new Error(`初始化构建缺少：${entry}`);
  }
  const markers = ['registry/index.json', '__kernel'];
  const worker = await readFile(join(out, '_worker.js'));
  if (markers.some((marker) => worker.includes(marker))) throw new Error('初始化 Worker 包含插件内核代码。');
  await checkContents(join(out, '_astro'), markers, true);
}

async function main() {
  const out = join(root, 'dist/site');
  await rm(out, { recursive: true, force: true });
  const { phase, rule, record } = await choosePhase(process.argv.slice(2), process.env);
  console.log(`[build:pages] ${phase} (规则 ${rule})`);
  if (record !== undefined) {
    try {
      if (JSON.parse(record)?.v !== 1) throw new Error();
    } catch {
      console.warn('[build:pages] ONLINE_GAL_SITE 记录格式无效，继续构建站点。');
    }
  }
  if (phase === 'site') {
    const args = ['run', 'build:site'];
    const child = process.env.npm_execpath
      ? spawnSync(process.execPath, [process.env.npm_execpath, ...args], { cwd: root, stdio: 'inherit' })
      : spawnSync('npm', args, { cwd: root, stdio: 'inherit', shell: process.platform === 'win32' });
    if (child.status !== 0) throw new Error('站点构建失败。');
  } else {
    const { buildSetup } = await import('../apps/setup/scripts/build-setup.mjs');
    await buildSetup(out);
  }
  await verifyOutput(phase, out);
  if (process.env.GITHUB_OUTPUT) await appendFile(process.env.GITHUB_OUTPUT, `phase=${phase}\n`);
}

if (process.argv[1] && import.meta.url === pathToFileURL(resolve(process.argv[1])).href) {
  main().catch((error) => {
    console.error(`[build:pages] ${error.message}`);
    process.exitCode = 1;
  });
}
