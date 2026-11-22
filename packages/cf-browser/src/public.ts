/**
 * What the page gives the browser cf (openspec/changes/cf-account-admin, cf-browser spec).
 *
 * This file has no upstream imports, so consumers type-check against it without the `.ref/cf`
 * checkout; `runCf` itself lives in the built bundle (`dist/index.js`).
 */

export type CfStream = 'stdout' | 'stderr';

/** The kernel's account status as the console needs it; never carries a token. */
export interface CfAccountStatus {
  signedIn: boolean;
  user?: { id: string; email: string };
  scopes?: readonly string[];
  expiresAt?: number;
  /** The account that owns this site's Pages project, when the signed-in user has it. */
  siteAdmin?: { accountId: string; accountName: string; projectName: string } | null;
}

/** `cf auth login|logout|whoami` go through these to the kernel account API. */
export interface CfAccountBridge {
  status(): Promise<CfAccountStatus>;
  /** Runs the device flow, writing the code and link with `write`, until signed in. */
  login(write: (text: string) => void, signal: AbortSignal): Promise<void>;
  logout(): Promise<void>;
}

export interface CfAskOptions {
  secret?: boolean;
  choices?: readonly string[];
  labels?: Readonly<Record<string, string>>;
}

export interface CfIo {
  /** Console text. `stdout` is the command's result; `stderr` is diagnostics and notices. */
  write(stream: CfStream, text: string): void;
  /** Response bytes that are not text, offered to the user as a download. */
  writeBytes(bytes: Uint8Array, name: string): void;
  /** Inline confirmation; resolves `false` when the user declines. */
  confirm(question: string): Promise<boolean>;
  /** Inline input; resolves `null` when the user cancels. */
  ask(question: string, options?: CfAskOptions): Promise<string | null>;
  /** A long request started; the console may show it until the command ends. */
  progress?(message: string): void;
  /** Aborts every request of the running command. */
  readonly signal: AbortSignal;
  /** Help text wrap width; 100 when absent. */
  readonly columns?: number;
  /** Session variables, as cf reads them from the environment (`CLOUDFLARE_ACCOUNT_ID`, `CLOUDFLARE_ZONE_ID`). */
  readonly env?: Readonly<Record<string, string>>;
  readonly account: CfAccountBridge;
}

/** Runs one cf command line (without the leading `cf`) and resolves its exit code. */
export declare function runCf(argv: readonly string[], io: CfIo): Promise<number>;
