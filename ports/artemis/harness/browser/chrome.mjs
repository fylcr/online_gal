/**
 * Drives the checks in the user's real Chrome rather than headless Edge.
 *
 * Two reasons. A real Chrome uses the real GPU instead of SwiftShader, and it has a real window size —
 * the reported abort came from a normal browser session, and the headless harness runs at 750x485,
 * which is not the canvas a player has. A crash that depends on either is invisible to the headless
 * runs.
 *
 * The `chrome-devtools-mcp` channel is unusable in this session (every call answers "Invalid MCP
 * request or session"), so this attaches to the browser that MCP launcher would itself have started:
 * same profile, same port, same flags. Chrome is spawned **detached** so it outlives this script and
 * stays open for the user to look at — which is the point of using their browser.
 *
 * The interface deliberately matches `launchEdge`, so a check can switch browsers by changing one
 * import.
 */
import { spawn } from 'node:child_process';
import { existsSync, mkdirSync } from 'node:fs';
import { Cdp, sleep } from '../../krkr/kirikiroid2/browser-test/cdp.mjs';

// Re-exported so a check needs one import for both: the browser it drives and the timer it sleeps on.
export { sleep };

const CHROME_EXE = process.env.DSH_CHROME_EXE || 'D:\\programe\\Chrome\\chrome.exe';
const PROFILE_DIR = process.env.DSH_CHROME_PROFILE || 'C:\\Users\\ASUS\\.dsh\\chrome-mcp-profile';

async function probe(port) {
  try {
    const controller = new AbortController();
    const timer = setTimeout(() => controller.abort(), 1500);
    const res = await fetch(`http://127.0.0.1:${port}/json/version`, { signal: controller.signal });
    clearTimeout(timer);
    return res.ok ? await res.json() : null;
  } catch {
    return null;
  }
}

/** Resolve once Chrome is reachable over CDP, starting it detached if it is not already up. */
export async function ensureChrome(port = 9222) {
  const existing = await probe(port);
  if (existing) return existing;
  if (!existsSync(CHROME_EXE)) throw new Error(`Chrome executable not found: ${CHROME_EXE}`);
  mkdirSync(PROFILE_DIR, { recursive: true });

  // `D:\programe\Chrome\chrome.exe` is a portable launcher whose real browser is a grandchild, so the
  // pipe transport does not survive. An HTTP CDP endpoint does, which is why this does not use
  // `--remote-debugging-pipe`.
  const child = spawn(
    CHROME_EXE,
    [
      `--user-data-dir=${PROFILE_DIR}`,
      `--remote-debugging-port=${port}`,
      '--remote-allow-origins=*',
      '--no-first-run',
      '--no-default-browser-check',
      '--disable-features=Translate,ChromeWhatsNewUI',
      '--autoplay-policy=no-user-gesture-required',
      '--mute-audio',
    ],
    { detached: true, stdio: 'ignore', windowsHide: false },
  );
  child.unref();

  const deadline = Date.now() + 60000;
  while (Date.now() < deadline) {
    await sleep(500);
    const info = await probe(port);
    if (info) return info;
  }
  throw new Error(`Chrome did not expose CDP on ${port} within 60 s`);
}

/**
 * Connect to a page target. `stop` closes the socket and, by default, leaves Chrome running.
 */
export async function launchBrowser({ port = 9222, enable = [], keepBrowser = true } = {}) {
  await ensureChrome(port);

  let wsUrl = null;
  // Prefer the page we are about to drive over any other tab the user has open.
  for (let attempt = 0; attempt < 40 && wsUrl === null; attempt += 1) {
    try {
      const targets = await (await fetch(`http://127.0.0.1:${port}/json/list`)).json();
      wsUrl = targets.find((t) => t.type === 'page' && t.webSocketDebuggerUrl)?.webSocketDebuggerUrl ?? null;
    } catch {
      /* not up yet */
    }
    if (wsUrl === null) await sleep(250);
  }
  if (wsUrl === null) throw new Error(`no page target on ${port}`);

  const cdp = new Cdp(wsUrl);
  await cdp.connect();
  for (const domain of enable) {
    if (domain === 'Network') await cdp.send('Network.enable', { maxTotalBufferSize: 256 * 1024 * 1024 });
    else await cdp.send(`${domain}.enable`);
  }

  return {
    cdp,
    async stop() {
      cdp.close();
      if (!keepBrowser) return;
      // Deliberately no kill: the user asked for their browser, and a browser that vanishes when the
      // script ends is a worse experience than one left open on the page under test.
    },
  };
}
