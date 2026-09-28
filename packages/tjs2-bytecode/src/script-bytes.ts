/**
 * Script payload unwrapping, re-exported from the KiriKiri runtime.
 *
 * A compiled `.ks` sits behind a 16-byte header with a fixed-length XOR key and
 * optional zlib, and the same header wraps both original source and bytecode.
 * The decoder already exists for the runtime that interprets source, so it is
 * re-exported here rather than duplicated: two copies would be free to disagree
 * about the container format, and the analysis would then be measuring a
 * different thing from the runtime.
 */
export { loadScriptBytes, decodeCompiledScript, sniffScriptFormat } from '@online-gal/runtime-kag';
export type { DecodedScript } from '@online-gal/runtime-kag';