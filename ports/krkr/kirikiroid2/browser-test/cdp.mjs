/**
 * Minimal DevTools-protocol client and Edge launcher, shared by the browser checks.
 *
 * Nothing in this repository ships Playwright or Puppeteer, and the machine's Edge is
 * Chromium 153 — past the JSPI requirement the reference project states (137). Node
 * carries a global `WebSocket`, so the browser is launched with `--remote-debugging-port`
 * and spoken to directly: no installs, and one place that knows the launch flags.
 *
 * The two callers are deliberately different in what they assert:
 *
 *  - `kiri-browser-probe.mjs` drives the *build* page directly, comparing the stock
 *    fetch path against an installed host byte source. That is how the seam itself is
 *    measured (coverage of the archive actually read).
 *  - `kiri-app-flow.mjs` drives the *platform's own UI*, so the chain from import to
 *    engine is exercised end to end rather than inferred from unit tests.
 */

export const EDGE_PATH = 'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe';

/** Resolves after `ms`; the only timer this module owns. */
export const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

/**
 * One connection to a CDP target.
 *
 * Requests are matched by id; events are dispatched to the handlers registered for
 * their method. Deliberately tiny: enough to enable domains, evaluate expressions and
 * observe console/network traffic, and nothing speculative.
 */
export class Cdp {
  constructor(url) {
    this.url = url;
    this.nextId = 1;
    this.pending = new Map();
    this.handlers = new Map();
    this.socket = null;
  }

  async connect() {
    this.socket = new WebSocket(this.url);
    await new Promise((resolve, reject) => {
      this.socket.addEventListener('open', resolve, { once: true });
      this.socket.addEventListener('error', reject, { once: true });
    });
    this.socket.addEventListener('message', (event) => {
      const message = JSON.parse(event.data);
      if (message.id !== undefined) {
        const entry = this.pending.get(message.id);
        if (!entry) return;
        this.pending.delete(message.id);
        if (message.error) entry.reject(new Error(`${message.error.message} (${message.error.code})`));
        else entry.resolve(message.result);
        return;
      }
      for (const handler of this.handlers.get(message.method) ?? []) handler(message.params);
    });
  }

  send(method, params = {}) {
    const id = this.nextId++;
    return new Promise((resolve, reject) => {
      this.pending.set(id, { resolve, reject });
      this.socket.send(JSON.stringify({ id, method, params }));
    });
  }

  on(method, handler) {
    if (!this.handlers.has(method)) this.handlers.set(method, []);
    this.handlers.get(method).push(handler);
  }

  /** Evaluates an expression and returns its value, or throws with the page's error. */
  async evaluate(expression) {
    const result = await this.send('Runtime.evaluate', {
      expression,
      returnByValue: true,
      awaitPromise: true,
    });
    if (result.exceptionDetails) {
      throw new Error(
        result.exceptionDetails.exception?.description ??
          result.exceptionDetails.text ??
          'evaluate failed',
      );
    }
    return result.result?.value;
  }

  close() {
    try {
      this.socket?.close();
    } catch {
      /* already gone */
    }
  }
}

/**
 * Starts Edge headless and connects to its first page target.
 *
 * `enable` is a convenience for the domains both callers want; anything else is the
 * caller's business. The returned `stop` kills the browser and removes the throwaway
 * profile, and is safe to call twice.
 */
export async function launchEdge({ port = 9222, enable = [], extraArgs = [] } = {}) {
  const { spawn } = await import('node:child_process');
  const { mkdtempSync, rmSync } = await import('node:fs');
  const { tmpdir } = await import('node:os');
  const { join } = await import('node:path');

  const profile = mkdtempSync(join(tmpdir(), 'kiri-cdp-'));
  const edge = spawn(
    EDGE_PATH,
    [
      '--headless=new',
      '--enable-unsafe-swiftshader',
      '--mute-audio',
      '--autoplay-policy=no-user-gesture-required',
      '--no-first-run',
      '--no-default-browser-check',
      '--disable-extensions',
      '--disable-background-networking',
      `--user-data-dir=${profile}`,
      `--remote-debugging-port=${port}`,
      ...extraArgs,
      'about:blank',
    ],
    { stdio: ['ignore', 'ignore', 'pipe'] },
  );
  let stderr = '';
  edge.stderr.on('data', (chunk) => {
    stderr += String(chunk);
  });

  let cdp;
  try {
    let wsUrl = null;
    for (let attempt = 0; attempt < 80 && wsUrl === null; attempt += 1) {
      try {
        const response = await fetch(`http://127.0.0.1:${port}/json/list`);
        const targets = await response.json();
        wsUrl = targets.find((t) => t.type === 'page' && t.webSocketDebuggerUrl)?.webSocketDebuggerUrl ?? null;
      } catch {
        /* browser not up yet */
      }
      if (wsUrl === null) await sleep(250);
    }
    if (wsUrl === null) {
      throw new Error(`Edge never exposed a page target on port ${port}. stderr: ${stderr.slice(-800)}`);
    }
    cdp = new Cdp(wsUrl);
    await cdp.connect();
    for (const domain of enable) {
      if (domain === 'Network') {
        await cdp.send('Network.enable', { maxTotalBufferSize: 256 * 1024 * 1024 });
      } else {
        await cdp.send(`${domain}.enable`);
      }
    }
  } catch (error) {
    cdp?.close();
    try {
      edge.kill('SIGKILL');
    } catch {
      /* already gone */
    }
    throw error;
  }

  return {
    cdp,
    async stop() {
      cdp.close();
      try {
        edge.kill('SIGKILL');
      } catch {
        /* already gone */
      }
      await sleep(400);
      try {
        rmSync(profile, { recursive: true, force: true });
      } catch {
        /* best effort */
      }
    },
  };
}
