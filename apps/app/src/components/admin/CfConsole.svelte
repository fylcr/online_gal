<script lang="ts">
  /**
   * The cf console on /admin (openspec/changes/cf-account-admin, tasks 5.3).
   *
   * Plain Svelte: an input line, ↑/↓ history and session variables from `lib/cf-console`, text
   * output, cancel, and inline confirm/ask prompts. The browser cf is imported on the first
   * command so the page itself does not load it, and each command group loads on first use.
   * Requests go to `/__cf/api` without a token; the kernel adds it.
   */
  import { onDestroy, tick } from 'svelte';
  import type { CfAccountBridge, CfAskOptions, CfIo, CfStream } from '@online-gal/cf-browser';
  import mitUrl from '@online-gal/cf-browser/LICENSE-MIT.txt?url&no-inline';
  import apacheUrl from '@online-gal/cf-browser/LICENSE-APACHE.txt?url&no-inline';
  import { account, type CloudflareStatus } from '../../lib/account';
  import { exportVariables, loadEnv, loadHistory, remember, splitCommandLine } from '../../lib/cf-console';

  type Entry =
    | { id: number; kind: 'command' | 'note' | CfStream; text: string }
    | { id: number; kind: 'download'; name: string; href: string };

  type Prompt =
    | { kind: 'confirm'; question: string; resolve: (yes: boolean) => void }
    | { kind: 'ask'; question: string; options: CfAskOptions; value: string; resolve: (answer: string | null) => void };

  const OUTPUT_LIMIT = 2_000_000;

  let entries = $state<Entry[]>([]);
  let line = $state('');
  let running = $state(false);
  let loading = $state(false);
  let prompt = $state<Prompt | null>(null);
  let history = $state<string[]>(loadHistory());
  let env = $state<Record<string, string>>(loadEnv());
  let cursor = -1;
  let draft = '';
  let nextId = 0;
  let size = 0;
  let loaded = false;
  let controller: AbortController | undefined;
  let output: HTMLDivElement | undefined = $state();
  let input: HTMLInputElement | undefined = $state();
  const blobs: string[] = [];

  function append(kind: 'command' | 'note' | CfStream, text: string): void {
    if (text === '') return;
    const last = entries.at(-1);
    if (last !== undefined && last.kind === kind && kind !== 'command') {
      (last as { text: string }).text += text;
    } else {
      entries.push({ id: nextId++, kind, text });
    }
    size += text.length;
    while (size > OUTPUT_LIMIT && entries.length > 1) {
      const dropped = entries.shift()!;
      if ('text' in dropped) size -= dropped.text.length;
    }
    void scroll();
  }

  async function scroll(): Promise<void> {
    await tick();
    if (output !== undefined) output.scrollTop = output.scrollHeight;
  }

  function clear(): void {
    for (const url of blobs.splice(0)) URL.revokeObjectURL(url);
    entries = [];
    size = 0;
  }

  const sleep = (ms: number, signal: AbortSignal) =>
    new Promise<void>((resolve, reject) => {
      const timer = setTimeout(resolve, ms);
      signal.addEventListener('abort', () => {
        clearTimeout(timer);
        reject(signal.reason);
      }, { once: true });
    });

  function toCf(status: CloudflareStatus) {
    if (!status.signedIn) return { signedIn: false };
    return {
      signedIn: true,
      ...(status.user === null ? {} : { user: status.user }),
      scopes: status.scopes,
      expiresAt: status.expiresAt,
      siteAdmin: status.siteAdmin,
    };
  }

  const bridge: CfAccountBridge = {
    status: async () => toCf(await account.status()),
    login: async (write, signal) => {
      const pending = await account.login();
      write(`在浏览器中打开 ${pending.verificationUriComplete ?? pending.verificationUri}\n核对验证码 ${pending.userCode} 后授权。\n`);
      let interval = pending.interval;
      for (;;) {
        await sleep(interval * 1000, signal);
        const result = await account.poll();
        switch (result.state) {
          case 'pending':
          case 'slow-down':
            interval = result.interval;
            continue;
          case 'signed-in':
            return;
          case 'expired':
            throw new Error('验证码已过期');
          case 'denied':
            throw new Error('已在 Cloudflare 拒绝授权');
          case 'none':
            throw new Error('登录已被取消');
        }
      }
    },
    logout: async () => {
      await account.logout();
    },
  };

  function settle(): void {
    if (prompt === null) return;
    if (prompt.kind === 'confirm') prompt.resolve(false);
    else prompt.resolve(null);
    prompt = null;
  }

  function makeIo(signal: AbortSignal): CfIo {
    signal.addEventListener('abort', settle, { once: true });
    return {
      write: (stream, text) => append(stream, text),
      writeBytes: (bytes, name) => {
        const href = URL.createObjectURL(new Blob([bytes as BlobPart]));
        blobs.push(href);
        entries.push({ id: nextId++, kind: 'download', name, href });
        void scroll();
      },
      confirm: (question) =>
        new Promise((resolve) => {
          prompt = { kind: 'confirm', question, resolve };
          void scroll();
        }),
      ask: (question, options = {}) =>
        new Promise((resolve) => {
          prompt = { kind: 'ask', question, options, value: options.choices?.[0] ?? '', resolve };
          void scroll();
        }),
      progress: (message) => append('note', `${message}\n`),
      signal,
      // About 7.2px per character at text-xs in the monospace stack, less the padding.
      columns: output === undefined ? 80 : Math.max(40, Math.floor((output.clientWidth - 32) / 7.2)),
      env: { ...env },
      account: bridge,
    };
  }

  function answer(value: boolean | string | null): void {
    const current = prompt;
    if (current === null) return;
    prompt = null;
    if (current.kind === 'confirm') {
      append('note', `${current.question} ${value === true ? '是' : '否'}\n`);
      current.resolve(value === true);
    } else {
      append('note', `${current.question} ${value === null ? '（取消）' : current.options.secret ? '******' : String(value)}\n`);
      current.resolve(value === null ? null : String(value));
    }
    void tick().then(() => input?.focus());
  }

  async function submit(): Promise<void> {
    if (running) return;
    const text = line.trim();
    line = '';
    cursor = -1;
    if (text === '') return;
    history = remember(history, text);
    append('command', `$ ${text}\n`);

    let argv: string[];
    try {
      argv = splitCommandLine(text);
    } catch (error) {
      append('stderr', `${error instanceof Error ? error.message : String(error)}\n`);
      return;
    }
    if (argv.length === 0) return;
    if (argv[0] === 'clear') {
      clear();
      return;
    }
    if (argv[0] === 'export') {
      const result = exportVariables(env, argv.slice(1));
      env = result.env;
      append('stdout', result.output);
      if (result.error !== undefined) append('stderr', result.error);
      return;
    }

    running = true;
    controller = new AbortController();
    try {
      if (!loaded) {
        loading = true;
        append('note', '正在加载 cf…\n');
      }
      const { runCf } = await import('@online-gal/cf-browser');
      loaded = true;
      loading = false;
      const code = await runCf(argv, makeIo(controller.signal));
      if (code !== 0) append('note', `退出码 ${code}\n`);
    } catch (error) {
      append('stderr', `${error instanceof Error ? error.message : String(error)}\n`);
    } finally {
      settle();
      loading = false;
      running = false;
      controller = undefined;
      await tick();
      input?.focus();
    }
  }

  function cancel(): void {
    controller?.abort(new DOMException('cancelled', 'AbortError'));
  }

  function keydown(event: KeyboardEvent): void {
    if (event.key === 'Enter') {
      event.preventDefault();
      void submit();
    } else if (event.key === 'c' && event.ctrlKey && running) {
      event.preventDefault();
      cancel();
    } else if (event.key === 'l' && event.ctrlKey) {
      event.preventDefault();
      clear();
    } else if (event.key === 'ArrowUp' && history.length > 0) {
      event.preventDefault();
      if (cursor === -1) {
        draft = line;
        cursor = history.length;
      }
      cursor = Math.max(0, cursor - 1);
      line = history[cursor]!;
    } else if (event.key === 'ArrowDown' && cursor !== -1) {
      event.preventDefault();
      cursor++;
      if (cursor >= history.length) {
        cursor = -1;
        line = draft;
      } else {
        line = history[cursor]!;
      }
    }
  }

  /** Splits text into plain runs and links, so the login URL and API links can be clicked. */
  function linkify(text: string): Array<{ text: string; href?: string }> {
    const parts: Array<{ text: string; href?: string }> = [];
    let last = 0;
    for (const match of text.matchAll(/https?:\/\/[^\s"'<>)\]]+/g)) {
      if (match.index > last) parts.push({ text: text.slice(last, match.index) });
      parts.push({ text: match[0], href: match[0] });
      last = match.index + match[0].length;
    }
    if (last < text.length) parts.push({ text: text.slice(last) });
    return parts;
  }

  onDestroy(() => {
    cancel();
    for (const url of blobs.splice(0)) URL.revokeObjectURL(url);
  });
</script>

<div class="rounded-2xl bg-base-200 px-6 py-5">
  <p class="text-sm text-base-content/60">
    输入 <code>cf --help</code> 查看命令，开头的 <code>cf</code> 可以省略。用 <code>export CLOUDFLARE_ACCOUNT_ID=…</code> 设置本次会话的变量，<code>clear</code> 清屏，Ctrl+C 取消。需要本地环境的命令（dev、deploy 等）在浏览器中不可用。
  </p>

  <div
    bind:this={output}
    class="mt-4 h-[28rem] overflow-y-auto bg-base-100 px-4 py-3 font-mono text-xs leading-relaxed"
    role="log"
    aria-live="polite"
  >
    {#each entries as entry (entry.id)}
      {#if entry.kind === 'download'}
        <a class="link link-primary block" href={entry.href} download={entry.name}>下载 {entry.name}</a>
      {:else}
        <pre
          class={[
            'break-all whitespace-pre-wrap',
            entry.kind === 'command' && 'font-bold text-primary',
            entry.kind === 'stderr' && 'text-base-content/60',
            entry.kind === 'note' && 'text-info',
          ]}>{#each linkify(entry.text) as part}{#if part.href}<a class="link" href={part.href} target="_blank" rel="noopener noreferrer">{part.text}</a>{:else}{part.text}{/if}{/each}</pre>
      {/if}
    {/each}

    {#if prompt?.kind === 'confirm'}
      {@const current = prompt}
      <div class="mt-2 flex flex-wrap items-center gap-2">
        <span class="whitespace-pre-wrap">{current.question}</span>
        <button type="button" class="btn btn-error btn-xs" onclick={() => answer(true)}>确认</button>
        <button type="button" class="btn btn-xs" onclick={() => answer(false)}>取消</button>
      </div>
    {:else if prompt?.kind === 'ask'}
      {@const current = prompt}
      <form
        class="mt-2 flex flex-wrap items-center gap-2"
        onsubmit={(event) => {
          event.preventDefault();
          answer(current.value);
        }}
      >
        <span class="whitespace-pre-wrap">{current.question}</span>
        {#if current.options.choices}
          <select class="select select-xs w-auto" bind:value={current.value}>
            {#each current.options.choices as choice}
              <option value={choice}>{current.options.labels?.[choice] ?? choice}</option>
            {/each}
          </select>
        {:else}
          <!-- svelte-ignore a11y_autofocus -->
          <input
            class="input input-xs w-64"
            type={current.options.secret ? 'password' : 'text'}
            bind:value={current.value}
            autofocus
          />
        {/if}
        <button type="submit" class="btn btn-primary btn-xs">确定</button>
        <button type="button" class="btn btn-xs" onclick={() => answer(null)}>取消</button>
      </form>
    {/if}
  </div>

  <div class="mt-3 flex items-center gap-2">
    <span class="font-mono text-sm text-base-content/50">$</span>
    <input
      bind:this={input}
      bind:value={line}
      onkeydown={keydown}
      class="input input-sm flex-1 font-mono"
      placeholder={running ? (loading ? '正在加载…' : '执行中…') : 'cf zones list'}
      disabled={prompt !== null}
      readonly={running}
      autocomplete="off"
      autocapitalize="off"
      spellcheck="false"
      aria-label="cf 命令"
    />
    {#if running}
      <button type="button" class="btn btn-sm" onclick={cancel}>取消</button>
    {:else}
      <button type="button" class="btn btn-primary btn-sm" onclick={submit}>执行</button>
    {/if}
  </div>

  <p class="mt-4 text-xs text-base-content/50">
    cf © Cloudflare, Inc. ·
    <a class="link" href={mitUrl} target="_blank" rel="noopener">MIT</a> /
    <a class="link" href={apacheUrl} target="_blank" rel="noopener">Apache-2.0</a>
  </p>
</div>
