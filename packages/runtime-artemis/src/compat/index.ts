/** Wire constants, the page facade and the compat runtime. The worker cannot import this. */

export {
  allocateCompatBlock,
  COMPAT_CONTROL_BYTES,
  COMPAT_OPEN_MODE,
  COMPAT_OPEN_PACK,
  COMPAT_PROTOCOL_VERSION,
  COMPAT_SAVE_DIR,
  COMPAT_SAVE_KIND,
  COMPAT_SAVE_MAX_FILE_BYTES,
  COMPAT_SAVE_MAX_FILES,
  COMPAT_SAVE_MAX_TOTAL_BYTES,
  COMPAT_SAVE_SLOT,
  COMPAT_WINDOW_BYTES,
  CONTROL,
  loadOffset,
  OPTIONAL_CABI,
  READ_FATAL,
  READ_MISSING,
  READ_TIMEOUT_MS,
  storeOffset,
  u32,
} from './protocol.js';
export type { CompatFileDescriptor, CompatSharedBlock } from './protocol.js';
export { CompatReadHost } from './read-host.js';
export type { CompatReadRequest } from './read-host.js';
export {
  assertRelativePath,
  base64ToBytes,
  CompatWorkerHost,
  readCheckpointFiles,
} from './worker-host.js';
export type {
  CompatCheckpointFile,
  CompatCheckpointFiles,
  CompatWorkerHostOptions,
  CompatWorkerInit,
} from './worker-host.js';
export { CompatArtemisRuntime, describeGame, keyCode } from './runtime.js';
