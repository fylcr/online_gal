export interface DerivedRoute { accountId: string; projectName: string; host: string; segment: string }
export interface ConfiguredRoute extends DerivedRoute { ownerUserId: string }
export function encodeBase64Url(bytes: Uint8Array): string;
export function decodeRouteKey(value: unknown): Uint8Array<ArrayBuffer>;
export function normalizeCfRouteHost(host: unknown): string;
export function validCfOwnerId(value: unknown): value is string;
export function cfRouteOwnerBinding(accountId: string, projectName: string, ownerId: string): Promise<string>;
export function deriveCfRouteConfig(input: { accountId: string; projectName: string; host: string; routeKey: string }): Promise<DerivedRoute>;
export function readCfRuntimeRouteConfig(env: Record<string, unknown>): Promise<ConfiguredRoute | null>;
export function readCfRouteConfig(env: Record<string, unknown>): Promise<ConfiguredRoute | null>;
