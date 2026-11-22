/** Browser `#lib/progress`: tells the console a request started, then runs it. */
import { io, run } from '../context.ts';

export async function withProgress<T>(message: string, task: () => Promise<T>): Promise<T> {
  if (!run().quiet) io().progress?.(message);
  return task();
}
