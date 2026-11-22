/**
 * Browser `#lib/output`: the same payload rules as upstream (list envelopes print their `result`,
 * `result_info` goes to stderr unless quiet, an empty result prints the success label), written
 * to the page console as pretty JSON.
 */
import { getResultInfo } from '#api';

import { io, run } from '../context.ts';
import { symbols } from './ui/theme.ts';

export interface OutputOptions {
  quiet?: boolean;
  highlight?: boolean;
  successLabel?: string;
}

function outputPayload(data: unknown): { payload: unknown; resultInfo: unknown } {
  if (data && typeof data === 'object' && !Array.isArray(data) && 'result' in data && Array.isArray(data.result)) {
    return { payload: data.result, resultInfo: (data as { result_info?: unknown }).result_info };
  }
  return { payload: data, resultInfo: data && typeof data === 'object' ? getResultInfo(data) : undefined };
}

export function printSuccess(message: string): void {
  if (!run().quiet) io().write('stderr', `${symbols.success} ${message}\n`);
}

export function formatOutput(data: unknown, options: OutputOptions = {}): void {
  if (options.quiet) return;
  const { payload, resultInfo } = outputPayload(data);
  if (!run().quiet && resultInfo && typeof resultInfo === 'object' && Object.keys(resultInfo).length > 0) {
    io().write('stderr', `result_info: ${JSON.stringify(resultInfo)}\n`);
  }
  if (payload === null || payload === undefined) {
    if (options.successLabel) printSuccess(options.successLabel);
    return;
  }
  io().write('stdout', `${JSON.stringify(payload, null, 2)}\n`);
}
