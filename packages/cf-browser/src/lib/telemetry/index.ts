/** Browser `#lib/telemetry`: cf in the browser sends no telemetry (cf-browser spec). */
export interface ShortFlagAlias<Canonical extends string = string> {
  canonical: Canonical;
  type: 'boolean' | 'value';
}
export type ShortFlagAliases<Args> = Record<string, ShortFlagAlias<Extract<keyof Args, string>>>;
export interface ArgClassification<Args = Record<string, unknown>> {
  safeFlags: string[];
  shortFlagAliases?: ShortFlagAliases<Args>;
}
export type CommandTelemetryMeta<Args = Record<string, unknown>> =
  | { command: string; sendMetrics?: boolean; searchQuery?: string; recordArgs: false; classification?: never }
  | { command: string; sendMetrics?: boolean; searchQuery?: string; recordArgs?: true; classification: ArgClassification<Args> };

export async function runWithTelemetry<T>(_meta: unknown, _argv: unknown, fn: () => T | Promise<T>): Promise<T> {
  return fn();
}
