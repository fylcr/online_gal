/**
 * Lists the tools this machine's `chrome-devtools-mcp` exposes.
 *
 * Chrome is not started by this (the launcher only starts it on the first `tools/call`), so
 * it is a cheap way to discover the exact tool names for the installed version instead of
 * guessing them from documentation for a different release.
 */
import { spawn } from 'node:child_process';

const LAUNCHER = 'C:\\Users\\ASUS\\.dsh\\chrome-devtools-mcp\\launch.mjs';

const child = spawn(process.execPath, [LAUNCHER], { stdio: ['pipe', 'pipe', 'pipe'] });
let stderr = '';
child.stderr.on('data', (d) => {
  stderr += String(d);
});

let buffer = '';
const pending = new Map();
child.stdout.on('data', (chunk) => {
  buffer += chunk.toString();
  let index;
  while ((index = buffer.indexOf('\n')) >= 0) {
    const line = buffer.slice(0, index).trim();
    buffer = buffer.slice(index + 1);
    if (!line) continue;
    try {
      const message = JSON.parse(line);
      const resolve = pending.get(message.id);
      if (resolve) {
        pending.delete(message.id);
        resolve(message);
      }
    } catch {
      /* not a JSON-RPC line */
    }
  }
});

let nextId = 1;
const send = (method, params) =>
  new Promise((resolve, reject) => {
    const id = nextId++;
    pending.set(id, resolve);
    child.stdin.write(`${JSON.stringify({ jsonrpc: '2.0', id, method, params })}\n`);
    setTimeout(() => reject(new Error(`timeout: ${method}`)), 60000);
  });

try {
  const init = await send('initialize', {
    protocolVersion: '2024-11-05',
    capabilities: {},
    clientInfo: { name: 'tool-list', version: '1.0' },
  });
  console.log('serverInfo:', JSON.stringify(init.result?.serverInfo));
  child.stdin.write(`${JSON.stringify({ jsonrpc: '2.0', method: 'notifications/initialized' })}\n`);

  const list = await send('tools/list', {});
  const tools = list.result?.tools ?? [];
  console.log(`tools: ${tools.length}`);
  for (const tool of tools) {
    const properties = Object.keys(tool.inputSchema?.properties ?? {});
    console.log(`  ${tool.name}(${properties.join(', ')})`);
  }
} catch (error) {
  console.error('failed:', String(error));
  if (stderr) console.error('stderr:', stderr.slice(-600));
} finally {
  child.kill();
  process.exit(0);
}
