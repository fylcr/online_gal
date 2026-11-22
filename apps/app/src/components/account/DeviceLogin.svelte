<script lang="ts">
  /**
   * Shared Cloudflare login for account settings and site setup.
   *
   * Signs in to Cloudflare with cf's device flow through the kernel. The page shows the code and
   * the link and drives the polling at the interval Cloudflare asks for; the kernel keeps no timer
   * of its own because the Service Worker may be stopped between polls. No token reaches this page.
   */
  import { onMount, untrack, type Snippet } from 'svelte';
  import {
    account,
    AccountError,
    type CloudflareStatus,
    type PendingLogin,
    type Unusable,
  } from '../../lib/account';

  type SignedIn = Extract<CloudflareStatus, { signedIn: true }>;
  let { signedIn, onStatus }: {
    signedIn: Snippet<[SignedIn, () => Promise<void>, () => Promise<void>, boolean]>;
    onStatus?: (status: CloudflareStatus) => void;
  } = $props();

  type View =
    | { kind: 'loading' }
    | { kind: 'unusable'; reason: Unusable; message: string }
    | { kind: 'error'; message: string }
    | { kind: 'signed-out'; notice?: string }
    | { kind: 'pending'; login: PendingLogin; warning?: string }
    | { kind: 'signed-in'; status: SignedIn };

  let view = $state<View>({ kind: 'loading' });
  let busy = $state(false);
  let now = $state(Date.now());
  let copied = $state(false);

  const UNUSABLE: Record<Unusable, { title: string; detail: string }> = {
    unready: {
      title: '插件系统未就绪',
      detail: '账号由插件系统的内核保管。请从已部署的站点打开本页；强制刷新后需要再普通刷新一次。本地开发服务器上没有内核。',
    },
    forbidden: {
      title: '当前管理器不能使用账号',
      detail: '只有官方管理器可以使用 Cloudflare 账号。切换回官方管理器后再试。',
    },
    unavailable: {
      title: '此浏览器不能保管凭据',
      detail: '凭据加密后存放在 IndexedDB 中，当前浏览器环境不可用（例如隐私模式限制）。',
    },
  };

  function fail(error: unknown): void {
    if (error instanceof AccountError && error.unusable !== undefined) {
      view = { kind: 'unusable', reason: error.unusable, message: error.message };
    } else {
      view = { kind: 'error', message: error instanceof Error ? error.message : String(error) };
    }
  }

  function show(status: CloudflareStatus, notice?: string): void {
    onStatus?.(status);
    if (status.signedIn) view = { kind: 'signed-in', status };
    else if (status.pending !== null) view = { kind: 'pending', login: status.pending };
    else view = notice === undefined ? { kind: 'signed-out' } : { kind: 'signed-out', notice };
  }

  async function refresh(): Promise<void> {
    view = { kind: 'loading' };
    try {
      show(await account.status());
    } catch (error) {
      fail(error);
    }
  }

  async function login(): Promise<void> {
    busy = true;
    copied = false;
    try {
      view = { kind: 'pending', login: await account.login() };
    } catch (error) {
      fail(error);
    } finally {
      busy = false;
    }
  }

  async function logout(): Promise<void> {
    busy = true;
    try {
      show(await account.logout());
    } catch (error) {
      fail(error);
    } finally {
      busy = false;
    }
  }

  onMount(() => {
    void refresh();
  });

  /** The login being polled; the loop restarts only when this changes, not on every view update. */
  const pendingCode = $derived(view.kind === 'pending' ? view.login.userCode : null);

  // Polling and the countdown live only while a login is pending, and stop when the panel leaves.
  $effect(() => {
    if (pendingCode === null) return;
    let stopped = false;
    let timer: ReturnType<typeof setTimeout> | undefined;
    const tick = setInterval(() => (now = Date.now()), 1000);

    const schedule = (seconds: number) => {
      if (!stopped) timer = setTimeout(poll, seconds * 1000);
    };
    const poll = async () => {
      const current = view;
      if (stopped || current.kind !== 'pending') return;
      try {
        const result = await account.poll();
        if (stopped) return;
        switch (result.state) {
          case 'pending':
          case 'slow-down':
            if (current.warning !== undefined) view = { kind: 'pending', login: current.login };
            schedule(result.interval);
            return;
          case 'signed-in':
            show(result.status);
            return;
          case 'expired':
            view = { kind: 'signed-out', notice: '验证码已过期，请重新登录。' };
            return;
          case 'denied':
            view = { kind: 'signed-out', notice: '已在 Cloudflare 拒绝授权。' };
            return;
          case 'none':
            await refresh();
            return;
        }
      } catch (error) {
        if (stopped) return;
        if (error instanceof AccountError && error.unusable === undefined && error.code !== 'account-failed') {
          // Offline for a moment: the login is still pending, keep trying.
          view = { kind: 'pending', login: current.login, warning: `暂时无法连接 Cloudflare：${error.message}` };
          schedule(current.login.interval);
          return;
        }
        fail(error);
      }
    };

    schedule(untrack(() => (view.kind === 'pending' ? view.login.interval : 5)));
    return () => {
      stopped = true;
      clearTimeout(timer);
      clearInterval(tick);
    };
  });

  async function copy(code: string): Promise<void> {
    try {
      await navigator.clipboard.writeText(code);
      copied = true;
    } catch {
      copied = false;
    }
  }

  function remaining(expiresAt: number): string {
    const seconds = Math.max(0, Math.round((expiresAt - now) / 1000));
    return `${Math.floor(seconds / 60)}:${String(seconds % 60).padStart(2, '0')}`;
  }

</script>

<div class="space-y-4">
  {#if view.kind === 'loading'}
    <div class="flex items-center gap-3 rounded-2xl bg-base-200 px-6 py-5 text-sm text-base-content/60">
      <span class="loading loading-spinner loading-sm"></span>
      正在读取账号状态
    </div>
  {:else if view.kind === 'unusable'}
    <div class="rounded-2xl bg-base-200 px-6 py-5">
      <p class="font-bold">{UNUSABLE[view.reason].title}</p>
      <p class="mt-2 text-sm text-base-content/60">{UNUSABLE[view.reason].detail}</p>
      <button type="button" class="btn btn-sm mt-4" onclick={refresh}>重试</button>
    </div>
  {:else if view.kind === 'error'}
    <div class="rounded-2xl bg-base-200 px-6 py-5">
      <p class="font-bold text-error">读取账号状态失败</p>
      <p class="mt-2 text-sm break-all text-base-content/60">{view.message}</p>
      <button type="button" class="btn btn-sm mt-4" onclick={refresh}>重试</button>
    </div>
  {:else if view.kind === 'signed-out'}
    <div class="rounded-2xl bg-base-200 px-6 py-5">
      <p class="font-bold">Cloudflare</p>
      <p class="mt-2 text-sm text-base-content/60">
        使用内置的 cf CLI 身份登录，申请与 <code>cf auth login</code> 相同的全部权限。凭据加密后只由插件系统的内核保管，页面拿不到 token。
      </p>
      {#if view.notice}
        <p class="mt-3 text-sm text-warning">{view.notice}</p>
      {/if}
      <button type="button" class="btn btn-primary btn-sm mt-4" disabled={busy} onclick={login}>
        {#if busy}<span class="loading loading-spinner loading-xs"></span>{/if}
        登录 Cloudflare
      </button>
    </div>
  {:else if view.kind === 'pending'}
    {@const pending = view.login}
    <div class="rounded-2xl bg-base-200 px-6 py-5">
      <p class="font-bold">在 Cloudflare 确认登录</p>
      <p class="mt-2 text-sm text-base-content/60">打开下面的链接，核对验证码后授权。授权完成后本页会自动更新。</p>
      <div class="mt-4 flex flex-wrap items-center gap-3">
        <span class="font-mono text-3xl font-bold tracking-widest select-all">{pending.userCode}</span>
        <button type="button" class="btn btn-ghost btn-xs" onclick={() => copy(pending.userCode)}>
          {copied ? '已复制' : '复制'}
        </button>
      </div>
      <a
        class="btn btn-primary btn-sm mt-4"
        href={pending.verificationUriComplete ?? pending.verificationUri}
        target="_blank"
        rel="noopener noreferrer"
      >
        打开 Cloudflare 授权页
      </a>
      <p class="mt-3 text-xs break-all text-base-content/50">{pending.verificationUri}</p>
      <div class="mt-4 flex items-center gap-3 text-sm text-base-content/60">
        <span class="loading loading-dots loading-sm"></span>
        等待授权，{remaining(pending.expiresAt)} 后过期
      </div>
      {#if view.warning}
        <p class="mt-2 text-sm text-warning">{view.warning}</p>
      {/if}
      <button type="button" class="btn btn-ghost btn-sm mt-4" disabled={busy} onclick={logout}>取消</button>
    </div>
  {:else if view.kind === 'signed-in'}
    {@render signedIn(view.status, refresh, logout, busy)}
  {/if}
</div>
