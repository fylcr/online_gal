// Prints the full JSON-RPC response of `take_snapshot` (and `list_pages`), because the
// snapshot's payload is not in `content[0].text` the way every other tool's is.
import { spawn } from 'node:child_process';

const child = spawn(process.execPath, ['C:\\Users\\ASUS\\.dsh\\chrome-devtools-mcp\\launch.mjs'], {
  stdio: ['pipe', 'pipe', 'pipe'],
});
child.stderr.on('data', (d) => process.stderr.write(d));

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
      /* ignore */
    }
  }
});

let nextId = 1;
const send = (method, params) =>
  new Promise((resolve, reject) => {
    const id = nextId++;
    pending.set(id, resolve);
    child.stdin.write(`${JSON.stringify({ jsonrpc: '2.0', id, method, params })}\n`);
    setTimeout(() => reject(new Error(`timeout: ${method}`)), 120_000);
  });

const target = process.argv[2] ?? 'http://127.0.0.1:4321/';

await send('initialize', {
  protocolVersion: '2024-11-05',
  capabilities: {},
  clientInfo: { name: 'snapshot-debug', version: '1.0' },
});
child.stdin.write(`${JSON.stringify({ jsonrpc: '2.0', method: 'notifications/initialized' })}\n`);

await send('tools/call', { name: 'new_page', arguments: { url: target, timeout: 60_000 } });
const pages = await send('tools/call', { name: 'list_pages', arguments: {} });
console.log('list_pages raw:', JSON.stringify(pages.result).slice(0, 300));

const selected = pages.result.content[0].text
  .split('\n')
  .find((line) => line.includes('[selected]'));
const pageId = Number(/(\d+):/.exec(selected)[1]);
console.log('pageId:', pageId);

await new Promise((r) => setTimeout(r, 4000));
const snapshot = await send('tools/call', { name: 'take_snapshot', arguments: { pageId } });
console.log('take_snapshot raw (first 1200 chars):');
console.log(JSON.stringify(snapshot.result, null, 1).slice(0, 1200));

child.kill();
process.exit(0);
