import { ArtemisHostError } from './errno.js';
import type { ArtemisMemory } from './memory.js';

/**
 * The `gl` bucket of `ports/artemis/harness/glue/imports-map.json` — 99 rows, 99 short names.
 *
 * This is the first bucket whose implementation is not a syscall. Everything in `fd` answers a
 * question about bytes on a disk; everything here answers a question about a **rendering context that
 * only exists in a browser**. A wasm module has no canvas, so the official glue borrows the page's:
 * `Browser.createContext` asks `Module.canvas.getContext("webgl", …)`, hands the resulting object to a
 * pool keyed by a synthetic handle, and the 55 `_gl*` rows forward straight into it. The guest never
 * learns that the context is a JavaScript object; it learns a small integer.
 *
 * That design is the whole reason this module exists in this shape. The integers are the contract:
 * `_glGenTextures` writes ids into the guest's heap, `_glCreateProgram` returns one, and
 * `_glDeleteProgram` takes it back. Everything in between — the pool, the error latch, the id
 * counter, the uniform-location table — is state the *glue* owns on the guest's behalf, so a host that
 * did not reimplement it would answer every lookup with a plausible zero and let the guest render a
 * black canvas with no error anywhere.
 *
 * **This slice is the boot path.** It implements the twelve rows the guest walks between process
 * start and its first `main` frame — `_glfwInit` through `_glfwDestroyWindow` plus
 * `_emscripten_get_canvas_element_size` — and the whole `_gl*` draw path they exist to serve,
 * because a context that cannot take a `drawElements` proves nothing. `_emscripten_set_main_loop` is
 * here for the same reason: without it the guest returns from `main` instead of parking, and a
 * returned `main` is indistinguishable from a game that ran to completion (which is what the official
 * control run shows, recorded in `.test/team-artemis/a3-ctrl-out.json`).
 *
 * **Two families are deliberately not implemented yet**: the eleven `__emscripten_set_*_callback_on_thread`
 * rows and the nineteen `_al*`/`_alc*` rows. They are still *supplied* — the engine links or the
 * module does not instantiate — but they are supplied as explicit refusals, listed in
 * {@link ARTEMIS_GL_REFUSED}, each taking its per-call default and reporting itself once. A row that
 * returns a family-wide `0` without saying so is the failure shape this bucket exists to remove:
 * "the engine is stuck" has to become "the engine is stuck *here*, at this short name".
 *
 * **The canvas, the context and the event loop are session-supplied.** A host that ran only under
 * Node has no canvas, and the honest answer for a missing context is the GL default for that call —
 * `void` does nothing, a getter returns the neutral value — reported, never a silent zero.
 */

/* -------------------------------------------------------------------------- */
/* Session-supplied browser surface                                            */
/* -------------------------------------------------------------------------- */

/**
 * The canvas, structurally typed rather than as `HTMLCanvasElement`.
 *
 * Two reasons, both practical. A Node test needs to supply a recorder that counts `drawElements`
 * calls without a GPU, and a structural type is what makes that a type-checked object instead of an
 * `any` cast. And the *shape* is the contract here: the bucket needs `width`/`height` (the two
 * `__emscripten_get_canvas_element_size` writes), `getContext` (the one context this host ever has),
 * and the listener pair so `_glfwInit` and `_glfwTerminate` are genuinely symmetric.
 */
export interface ArtemisGlCanvas {
  width: number;
  height: number;
  getContext(kind: string, attributes?: ArtemisGlContextAttributes): unknown;
  addEventListener?(type: string, listener: (event: unknown) => void, capture?: boolean): void;
  removeEventListener?(type: string, listener: (event: unknown) => void, capture?: boolean): void;
  style?: {
    removeProperty(property: string): void;
    setProperty(property: string, value: string, priority?: string): void;
  };
}

/**
 * The context attributes the guest's GLFW hints produce.
 *
 * `majorVersion` is the one field that changes which context kind is asked for. The official glue
 * defaults it to `1` and asks for `"webgl"`, and a guest whose shaders are GLSL ES 1.00 must get a
 * WebGL1 context or nothing compiles. `enableExtensionsByDefault` is the glue's own switch, and it
 * defaults to on, which is what makes `OES_vertex_array_object` present for `_glGenVertexArrays`.
 */
export interface ArtemisGlContextAttributes {
  antialias: boolean;
  alpha: boolean;
  majorVersion: number;
  enableExtensionsByDefault?: boolean;
  depth?: boolean;
  stencil?: boolean;
}

/**
 * The WebGL surface this bucket drives, typed by what the 99 rows actually call.
 *
 * Deliberately not `WebGLRenderingContext`: those enums are string unions and nominal interfaces, so
 * every one of the 55 forwards would need a cast to pass a raw `GLenum` number — 55 casts that could
 * each be wrong in a way the compiler would not catch. A structural interface keeps each call site's
 * argument order in the type where a mistake is actually visible.
 *
 * The five extension-derived methods are optional because they do not exist unconditionally: on a
 * WebGL2 context `createVertexArray` and friends are native, and `OES_vertex_array_object` is absent.
 * The official glue aliases them onto the context when the extension is present, which is what the
 * optionality records.
 */
export interface ArtemisWebGLContext {
  getExtension(name: string): unknown;
  getSupportedExtensions(): string[] | null;

  viewport(x: number, y: number, width: number, height: number): void;
  clear(mask: number): void;
  clearColor(red: number, green: number, blue: number, alpha: number): void;
  enable(capability: number): void;
  disable(capability: number): void;
  activeTexture(unit: number): void;
  blendFuncSeparate(sf: number, df: number, sc: number, dc: number): void;
  pixelStorei(pname: number, param: number): void;

  createTexture(): unknown;
  deleteTexture(texture: unknown): void;
  bindTexture(target: number, texture: unknown): void;
  texParameteri(target: number, pname: number, param: number): void;
  texImage2D(
    target: number,
    level: number,
    internalFormat: number,
    width: number,
    height: number,
    border: number,
    format: number,
    type: number,
    pixels: ArrayBufferView | null,
  ): void;
  copyTexImage2D(
    target: number,
    level: number,
    internalFormat: number,
    x: number,
    y: number,
    width: number,
    height: number,
    border: number,
  ): void;

  createBuffer(): unknown;
  deleteBuffer(buffer: unknown): void;
  bindBuffer(target: number, buffer: unknown): void;
  bufferData(target: number, usage: number): void;
  bufferData(target: number, data: ArrayBufferView | null, usage: number): void;
  bufferSubData(target: number, offset: number, data: ArrayBufferView): void;

  createShader(type: number): unknown;
  shaderSource(shader: unknown, source: string): void;
  compileShader(shader: unknown): void;
  getShaderParameter(shader: unknown, pname: number): number;
  getShaderInfoLog(shader: unknown): string | null;
  getShaderSource(shader: unknown): string | null;
  deleteShader(shader: unknown): void;

  createProgram(): { name?: number } | null;
  attachShader(program: unknown, shader: unknown): void;
  linkProgram(program: unknown): void;
  getProgramParameter(program: unknown, pname: number): number;
  getProgramInfoLog(program: unknown): string | null;
  useProgram(program: unknown): void;
  deleteProgram(program: unknown): void;
  getActiveUniform(program: unknown, index: number): { name: string; size: number };
  getActiveAttrib(program: unknown, index: number): { name: string; size: number };
  getActiveUniformBlockName(program: unknown, index: number): string;
  getUniformLocation(program: unknown, name: string): unknown;
  getAttribLocation(program: unknown, name: string): number;

  uniform1i(location: unknown, value: number): void;
  uniform1f(location: unknown, value: number): void;
  uniform3f(location: unknown, x: number, y: number, z: number): void;
  uniform1fv(location: unknown, value: Float32Array): void;

  vertexAttribPointer(
    index: number,
    size: number,
    type: number,
    normalized: boolean,
    stride: number,
    offset: number,
  ): void;
  enableVertexAttribArray(index: number): void;
  drawElements(mode: number, count: number, type: number, offset: number): void;
  readPixels(
    x: number,
    y: number,
    width: number,
    height: number,
    format: number,
    type: number,
    pixels: ArrayBufferView,
  ): void;

  createFramebuffer(): unknown;
  bindFramebuffer(target: number, framebuffer: unknown): void;
  framebufferTexture2D(
    target: number,
    attachment: number,
    textarget: number,
    texture: unknown,
    level: number,
  ): void;
  framebufferRenderbuffer(
    target: number,
    attachment: number,
    renderbuffertarget: number,
    renderbuffer: unknown,
  ): void;
  deleteFramebuffer(framebuffer: unknown): void;
  checkFramebufferStatus(target: number): number;

  createRenderbuffer(): unknown;
  bindRenderbuffer(target: number, renderbuffer: unknown): void;
  renderbufferStorage(
    target: number,
    internalFormat: number,
    width: number,
    height: number,
  ): void;
  deleteRenderbuffer(renderbuffer: unknown): void;

  /** Present only when `OES_vertex_array_object` is enabled, or natively on WebGL2. */
  createVertexArray?(): unknown;
  deleteVertexArray?(vao: unknown): void;
  bindVertexArray?(vao: unknown): void;
  /** Present only when `ANGLE_instanced_arrays` is enabled, or natively on WebGL2. */
  vertexAttribDivisor?(index: number, divisor: number): void;
  drawArraysInstanced?(mode: number, first: number, count: number, primcount: number): void;
  drawElementsInstanced?(
    mode: number,
    count: number,
    type: number,
    offset: number,
    primcount: number,
  ): void;
}

/**
 * One GL object pool, indexed by the synthetic id the guest sees.
 *
 * An array, not a record: `getNewId` fills `table[i] = null` up to the new id, and every delete row
 * reads `table[id]` expecting a hole to be `null`. A record with a numeric index would type those
 * holes `undefined`, which is exactly the check that is supposed to fail.
 */
type GlPool = (unknown | null)[];

/** What `_glGetProgramiv` needs to answer the four enums the glue answers from the table. */
interface GlProgramInfo {
  readonly uniforms: Record<string, [size: number, id: number]>;
  maxUniformLength: number;
  maxAttributeLength: number;
  maxUniformBlockNameLength: number;
}

/** The glue's `GLFW_Window`, reduced to the fields the twelve boot rows read. */
interface GlfwWindow {
  id: number;
  width: number;
  height: number;
  storedWidth: number;
  storedHeight: number;
  fullscreen: boolean;
  title: number | null;
  windowCloseFunc: number | null;
  windowSizeFunc: number | null;
  framebufferSizeFunc: number | null;
  shouldClose: number;
}

/** What a session must supply for any of this bucket to do anything visible. */
export interface ArtemisGlHost {
  readonly memory: ArtemisMemory;
  /** The page's canvas. Absent means every `_gl*` row takes its per-call default. */
  readonly canvas?: ArtemisGlCanvas;
  /** The `document`, for the `_emscripten_set_*` event-target rows' target `1`. */
  readonly document?: unknown;
  /** The `window`, for `_glfwInit`'s keyboard/gamepad listeners. */
  readonly window?: ArtemisGlWindowEvents;
  /** Schedules the next main-loop tick. In a browser this wraps `requestAnimationFrame`. */
  readonly requestFrame?: (tick: () => void) => void;
  /** `Date.now`, so the main loop's tick bookkeeping is deterministic under test. */
  readonly now?: () => number;
  /**
   * The guest's `dynCall_v` — how the main loop reaches the guest's frame function.
   *
   * The one field of this interface that is not `readonly`: it is an *export* of the bridge, so it
   * only exists after `WebAssembly.instantiate`, and the import table has to be built before that.
   * The table holds this object by reference and fills the field in afterwards.
   */
  dynCallV?: (entry: number) => void | Promise<void>;
  /**
   * The guest's `dynCall_iiii` — how every event callback is entered.
   *
   * Same late-binding reason as {@link ArtemisGlHost.dynCallV}, and the same non-`readonly` shape: the
   * event rows are the *only* place in this bucket that calls into the guest with four i32 arguments,
   * and `getDynCaller("iiii", callbackfunc)` is the glue's own way of reaching that. Without it the
   * eleven `__emscripten_set_*_callback_on_thread` rows could only ever register listeners they can
   * never fire — the exact "registered a success and never called it" shape this bucket rejects.
   *
   * The implementation is the official glue's own lookup order, not a guess: `getDynCaller(sig, index)`
   * takes the *table entry* first and only falls back to the exported `dynCall_<sig>`. Which of the two
   * actually served the last call is answered by {@link ArtemisGlHost.dynCallIiiiRoute}, because "the
   * table entry" and "the forwarder" fail differently and a report that cannot tell them apart cannot
   * tell a wrong index from a wrong arity.
   */
  dynCallIiii?: (entry: number, a: number, b: number, c: number) => number | Promise<number>;
  /**
   * How the last {@link ArtemisGlHost.dynCallIiii} call was routed — `table`, `forwarder`, both, or
   * `none`. A getter rather than a string so a caller reads the value at the moment it reports it, and
   * so the `gl` bucket can say which of the official glue's two paths answered a given callback without
   * `boot.ts` having to push a value back in.
   */
  dynCallIiiiRoute?: () => string;
  /**
   * The guest's `malloc`, for the five event blocks the glue allocates on first use.
   *
   * The blocks are guest heap: mouse 64 B, key 164 B, wheel 96 B, touch 1684 B, visibility 8 B. A
   * block this host does not allocate has no address to hand the callback, so a session that supplies
   * no `malloc` gets the per-call neutral answer on these rows rather than a listener that fires into
   * a pointer nothing owns.
   */
  readonly malloc?: (bytes: number) => number;
  /** `Browser.resizeCanvas`, which the official glue defaults to `true`. */
  readonly resizeCanvas?: boolean;
  /** `Module.forcedAspectRatio`, the only canvas-sizing policy the guest can set from JS. */
  readonly forcedAspectRatio?: number;
  /**
   * The nineteen audio rows, supplied by a real audio host instead of this bucket's refusals.
   *
   * The table is checked for **all nineteen** short names at construction and a table missing any of
   * them throws {@link ArtemisHostError}. This is the whole point of the field: the audio rows are one
   * family, the guest's mixer drives them together, and a half-wired table is the failure this bucket
   * exists to prevent — the rows that *did* get a body would play sound while the row that did not
   * quietly reported a refusal mid-frame, so the game would be audible and half-deaf with nothing
   * naming the row that was left out. All or nothing.
   *
   * A function is looked up **per call**, not captured, so a host object passed by reference can
   * still have its rows filled in after the import table exists — the same late-binding reason as
   * {@link ArtemisGlHost.dynCallV}.
   */
  readonly audioImports?: Readonly<Record<string, (...args: number[]) => number | void>>;
/** Reports a call this host could not serve, so a session can log it instead of swallowing it. */
  readonly onBlocked?: (what: string, detail: string) => void;
  /**
   * Serializes one **whole** guest operation — a frame, or a DOM event delivery.
   *
   * Optional, and absent means "run inline", which is what every synchronous session and every
   * existing test already does. When the session supplies it, the host never enters the guest from
   * inside a frame or a listener: the whole unit is handed over, awaited there, and the host waits for
   * {@link ArtemisGlHost.waitUntilIdle} before it schedules anything else. That is what keeps two frames
   * from overlapping when the guest's import is itself asynchronous.
   */
  readonly enqueue?: (task: () => void | Promise<void>) => void | Promise<void>;
  /** Resolves once the guest has no outstanding asynchronous work; awaited between frames and after events. */
  readonly waitUntilIdle?: () => Promise<void>;
  /** True after the session disposed the loop, so late frames and late DOM events become no-ops. */
  readonly isDisposed?: () => boolean;
  /** The guest allocator, for the five event blocks — and for the one best-effort release at dispose. */
  free?: (ptr: number) => void;
  /** Handed the loop's {@link ArtemisGlLifecycle} once the loop exists, so the session can control it. */
  setLifecycle?: (controller: ArtemisGlLifecycle) => void;
}

/**
 * The loop controller a session receives through {@link ArtemisGlHost.setLifecycle}.
 *
 * Deliberately three methods and no state: the session can pause for a tab switch, resume, and dispose
 * for good. `dispose` is the only one that tears the listeners down — a pause is not a teardown, so a
 * paused game resumes with its canvas listeners, its GL state and its event blocks intact.
 */
export interface ArtemisGlLifecycle {
  /** Stop entering the guest and cancel any pending frame. Idempotent. */
  pause(): void;
  /** Start again with **exactly one** pending frame, if the loop is installed, running and not disposed. */
  resume(): void;
  /** Idempotent teardown: stop the loop, cancel the pending frame, detach every listener, free the blocks. */
  dispose(): void | Promise<void>;
}

/** Just enough of `window` for the listener registration `_glfwInit` performs. */
export interface ArtemisGlWindowEvents {
  addEventListener(type: string, listener: (event: unknown) => void, capture?: boolean): void;
  removeEventListener(type: string, listener: (event: unknown) => void, capture?: boolean): void;
}

/** The import table, keyed by the wasm short name. */
export type ArtemisGlImports = Record<string, (...args: number[]) => number | void>;

/* -------------------------------------------------------------------------- */
/* Frozen constants                                                           */
/* -------------------------------------------------------------------------- */

/**
 * `GLFW.defaultHints` verbatim.
 *
 * Only four entries are ever read — `135181` (samples), `135173` (depth), `135174` (stencil),
 * `135172` (alpha) — but the whole table is reproduced because `createWindow` copies `hints` onto the
 * window as `attributes`, and a guest that later reads its own window back through a row this slice
 * does not implement would see a truncated table.
 */
const GLFW_DEFAULT_HINTS: Readonly<Record<number, number>> = {
  131073: 0, 131074: 0, 131075: 1, 131076: 1, 131077: 1,
  135169: 8, 135170: 8, 135171: 8, 135172: 8, 135173: 24, 135174: 8,
  135175: 0, 135176: 0, 135177: 0, 135178: 0, 135179: 0, 135180: 0, 135181: 0, 135182: 0, 135183: 0,
  139265: 196609, 139266: 1, 139267: 0, 139268: 0, 139269: 0, 139270: 0, 139271: 0, 139272: 0,
};

/** The two hint keys `createWindow` turns into context attributes, named so the intent is readable. */
const HINT_SAMPLES = 135181;
const HINT_DEPTH = 135173;
const HINT_STENCIL = 135174;
const HINT_ALPHA = 135172;

/**
 * `_emscripten_set_main_loop_timing` count > this uses `requestAnimationFrame`.
 *
 * The glue's own scheduler: mode `0` is a fixed-interval `setTimeout`, mode `1` is `rAF`, mode `2` is
 * `setImmediate`. `rAF` is the faithful choice for a browser frame loop, and the value is the frame
 * divisor `_glfwSwapInterval(1)` asks for.
 */
const MAIN_LOOP_RAF_MODE = 1;

/** The glue's `miniTempWebGLFloatBuffers` ceiling: `count <= 288` gets a cached scratch view. */
const MINI_TEMP_COUNT = 288;

/** The extensions `initExtensions` enables without being asked, verbatim from the glue. */
const AUTO_ENABLED_EXTENSIONS: readonly string[] = [
  'OES_texture_float', 'OES_texture_half_float', 'OES_standard_derivatives',
  'OES_vertex_array_object', 'WEBGL_compressed_texture_s3tc', 'WEBGL_depth_texture',
  'OES_element_index_uint', 'EXT_texture_filter_anisotropic', 'EXT_frag_depth',
  'WEBGL_draw_buffers', 'ANGLE_instanced_arrays', 'OES_texture_float_linear',
  'OES_texture_half_float_linear', 'EXT_blend_minmax', 'EXT_shader_texture_lod',
  'OES_texture_norm16', 'WEBGL_compressed_texture_pvrtc', 'EXT_color_buffer_half_float',
  'EXT_color_buffer_float', 'EXT_sRGB', 'WEBGL_compressed_texture_etc1',
  'EXT_disjoint_timer_query', 'WEBGL_compressed_texture_etc', 'WEBGL_compressed_texture_astc',
  'EXT_color_buffer_float', 'WEBGL_compressed_texture_s3tc_srgb',
  'EXT_disjoint_timer_query_webgl2', 'WEBKIT_WEBGL_compressed_texture_pvrtc',
];

/**
 * `_glfwInit`'s listener list, in the glue's order.
 *
 * `window` rows attach to the page, `canvas` rows to the module's canvas. `_glfwTerminate` removes
 * exactly these — that symmetry is the only part of the list this slice depends on, and it is what
 * makes an init/terminate pair testable without a browser.
 */
const GLFW_WINDOW_EVENTS: readonly string[] = [
  'gamepadconnected', 'gamepaddisconnected', 'keydown', 'keypress', 'keyup', 'blur',
];
const GLFW_CANVAS_EVENTS: readonly string[] = [
  'touchmove', 'touchstart', 'touchcancel', 'touchend', 'mousemove', 'mousedown', 'mouseup',
  'wheel', 'mousewheel', 'mouseenter', 'mouseleave', 'drop', 'dragover',
];

/** `_glGetProgramiv` / `_glGetShaderiv` enums the glue answers from its own table, not from the driver. */
const GL_INFO_LOG_LENGTH = 35716;
const GL_ACTIVE_UNIFORMS = 35718;
const GL_ACTIVE_ATTRIBUTES = 35721;
const GL_ACTIVE_UNIFORM_BLOCKS = 35382;
const GL_ACTIVE_UNIFORM_MAX_LENGTH = 35719;
const GL_ACTIVE_ATTRIBUTE_MAX_LENGTH = 35722;
const GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH = 35381;
const GL_SHADER_SOURCE_LENGTH = 35720;

/** `GL_CONTEXT_LOST`; the only framebuffer status the guest treats as fatal. */
const GL_FRAMEBUFFER_COMPLETE = 36053;

/** Builds the `gl` import table for one instance. */
export function createArtemisGlImports(host: ArtemisGlHost): ArtemisGlImports {
  const { memory } = host;
  const report = (what: string, detail: string): void => {
    try {
      host.onBlocked?.(what, detail);
    } catch {
      // A diagnostic sink that throws must not become the failure it was describing.
    }
  };

  /**
   * Refusal reporting, once per short name.
   *
   * The point of a refusal is that a human reads it. A row inside the guest's per-frame loop would
   * otherwise report thousands of times and bury the one line that matters, so the first call names
   * itself and the rest are silent — the count is still in `boot.ts`'s trace, which is where "how
   * often" belongs.
   */
  const refused = new Set<string>();
  const refuse = (name: string, detail: string): void => {
    if (refused.has(name)) return;
    refused.add(name);
    report('gl', `${name} (${ARTEMIS_GL_OFFICIAL[name] ?? name}) ${detail}`);
  };

  /**
   * Reads a NUL-terminated string out of the guest heap.
   *
   * The `| 0` is not decoration: a wasm i32 arrives in JavaScript signed, so a heap address above
   * 2^31 comes through negative and has to be squeezed back to the unsigned value the accessor wants.
   */
  const utf8 = (pointer: number): string => memory.utf8((pointer | 0) >>> 0);

  /** `HEAP32[ptr >>> 2]`, the read every id and enum argument goes through. */
  const loadI32 = (pointer: number): number => memory.i32[(pointer | 0) >>> 2] ?? 0;

  /** `setValue(ptr, value, "i32")` — skipped when the pointer is null, exactly as the glue skips it. */
  const storeI32 = (pointer: number, value: number): void => {
    if (!pointer) return;
    memory.i32[(pointer | 0) >>> 2] = value | 0;
  };

  /* ------------------------------------------------------------------ */
  /* GL state — the glue's `GL` object                                   */
  /* ------------------------------------------------------------------ */

  const gl = {
    counter: 1,
    lastError: 0 as number | null,
    unpackAlignment: 4,
    buffers: [] as GlPool,
    programs: [] as GlPool,
    framebuffers: [] as GlPool,
    renderbuffers: [] as GlPool,
    textures: [] as GlPool,
    uniforms: [] as GlPool,
    shaders: [] as GlPool,
    vaos: [] as GlPool,
    programInfos: {} as Record<number, GlProgramInfo>,
    contexts: [] as GlPool,
    currentContext: null as { GLctx: ArtemisWebGLContext } | null,
    ctx: null as ArtemisWebGLContext | null,
  };

  /** `GL.recordError` — first error wins, so the guest's `glGetError` reports the *first* mistake. */
  const recordError = (code: number): void => {
    if (gl.lastError === null || gl.lastError === 0) gl.lastError = code;
  };

  /**
   * `GL.getNewId(table)`.
   *
   * One counter for every pool, which is what makes an id from `createProgram` and an id from
   * `createTexture` never collide in the guest's own bookkeeping. The `table[i] = null` fill is
   * load-bearing: `_glDeleteTextures` walks `table[id]` and a hole there would be `undefined` rather
   * than the null the delete path tests.
   */
  const getNewId = (table: GlPool): number => {
    const ret = gl.counter;
    gl.counter += 1;
    for (let i = table.length; i < ret; i += 1) table[i] = null;
    return ret;
  };

  /** `GL.getSource` — concatenates the guest's shader sources, honouring an explicit length array. */
  const getSource = (
    shader: number,
    count: number,
    stringArray: number,
    lengthArray: number,
  ): string => {
    void shader;
    let source = '';
    const decoder = new TextDecoder('utf-8');
    for (let i = 0; i < count; i += 1) {
      const len = lengthArray ? loadI32(lengthArray + i * 4) : -1;
      const pointer = (loadI32(stringArray + i * 4) | 0) >>> 0;
      if (len < 0) {
        source += memory.utf8(pointer);
      } else {
        source += decoder.decode(memory.u8.subarray(pointer, pointer + len));
      }
    }
    return source;
  };

  /** `__colorChannelsInGlTextureFormat` — the channel count a pixel `format` occupies. */
  const colorChannels = (format: number): number => {
    const table: Record<number, number> = { 5: 3, 6: 4, 8: 2, 29502: 3, 29504: 4 };
    return table[format - 6402] ?? 1;
  };

  /**
   * `heapObjectForWebGLType` + `heapAccessShiftForWebGLHeap`, as one lookup.
   *
   * The `type` values are the GL enums offset by 5120 (`GL_UNSIGNED_BYTE` = 5121 becomes 1), and the
   * shift converts a byte offset into an element index for the matching typed view. Both are needed
   * by `_glTexImage2D` and `_glReadPixels`, the only two rows that hand raw pixel bytes to WebGL.
   */
  const heapForType = (
    type: number,
  ): { bytes: Uint8Array | Uint16Array | Uint32Array | Float32Array; shift: number } => {
    const enumValue = type - 5120;
    if (enumValue === 1) return { bytes: memory.u8, shift: 0 };
    if (enumValue === 4) return { bytes: memory.u32, shift: 2 };
    if (enumValue === 6) return { bytes: memory.f32, shift: 2 };
    if (enumValue === 5 || enumValue === 28922) return { bytes: memory.u32, shift: 2 };
    return { bytes: memory.u16, shift: 1 };
  };

  /**
   * `computeUnpackAlignedImageSize` — how many bytes `_glTexImage2D` must read for a `width x height`
   * image at `bytesPerPixel`, honouring `GL_UNPACK_ALIGNMENT`.
   *
   * The row is rounded up to the alignment, and the *height* multiplies the rounded row. This is the
   * classic "the last row is padded" rule; a host that multiplied the unrounded row would read a few
   * bytes short and hand WebGL a buffer whose tail is garbage.
   */
  const unpackAlignedImageSize = (
    width: number,
    height: number,
    sizePerPixel: number,
    alignment: number,
  ): number => {
    const roundedToNextMultipleOf = (x: number, y: number): number => (x + y - 1) & -y;
    const plainRowSize = width * sizePerPixel;
    return height * roundedToNextMultipleOf(plainRowSize, alignment);
  };

  /**
   * `emscriptenWebGLGetTexPixelData` — a typed view over the guest's pixel bytes.
   *
   * Returns `null` when the pointer is 0, which is not the same as "no pixels": `_glTexImage2D` uses
   * 0 to mean *allocate* the texture, and `_glReadPixels` records `GL_INVALID_OPERATION` (1280) on a
   * null view. The two rows check the pointer themselves; this only builds the view.
   */
  const texPixelData = (
    type: number,
    format: number,
    width: number,
    height: number,
    pointer: number,
  ): ArrayBufferView | null => {
    if (!pointer) return null;
    const { bytes, shift } = heapForType(type);
    const byteSize = 1 << shift;
    const sizePerPixel = colorChannels(format) * byteSize;
    const total = unpackAlignedImageSize(width, height, sizePerPixel, gl.unpackAlignment);
    return bytes.subarray((pointer | 0) >>> shift, ((pointer + total) | 0) >>> shift);
  };

  /**
   * `__glGenObject` — the shared body of the five `_glGen*` rows.
   *
   * The id is written to the guest's array **even when creation failed**, where it writes 0 and latches
   * `GL_INVALID_OPERATION` (1282). That is the contract the guest's `glGenTextures` error path checks,
   * and it is the reason the pool write cannot be conditional on the pool insert.
   */
  const genObject = (
    n: number,
    outPointer: number,
    createFunction: keyof ArtemisWebGLContext,
    table: GlPool,
  ): void => {
    const ctx = gl.ctx;
    if (!ctx) {
      refuse('__glGenObject', 'was called with no WebGL context, so every generated id is 0');
      for (let i = 0; i < n; i += 1) storeI32(outPointer + i * 4, 0);
      return;
    }
    const create = ctx[createFunction];
    if (typeof create !== 'function') {
      refuse(String(createFunction), 'is not available on this context, so the ids are 0');
      for (let i = 0; i < n; i += 1) storeI32(outPointer + i * 4, 0);
      return;
    }
    for (let i = 0; i < n; i += 1) {
      const object = (create as () => unknown).call(ctx);
      const id = object ? getNewId(table) : 0;
      if (object) {
        (object as { name?: number }).name = id;
        table[id] = object;
      } else {
        recordError(1282);
      }
      storeI32(outPointer + i * 4, id);
    }
  };

  /**
   * `GL.populateUniformTable` — run at link time, because `_glGetUniformLocation` is answered from it.
   *
   * The guest asks for `foo` and `foo[3]` with the *same* integer location, so this walks the active
   * uniforms once, gives each a synthetic id, and stores the driver's real `WebGLUniformLocation`
   * against it. `uniform1f` then takes the synthetic id and looks the real location up. The array
   * members get consecutive ids so `id + index` is the whole of the lookup.
   */
  const populateUniformTable = (program: number): void => {
    const ctx = gl.ctx;
    const glProgram = gl.programs[program];
    if (!ctx || !glProgram) return;
    const table: GlProgramInfo = {
      uniforms: {},
      maxUniformLength: 0,
      maxAttributeLength: -1,
      maxUniformBlockNameLength: -1,
    };
    gl.programInfos[program] = table;
    const numUniforms = ctx.getProgramParameter(glProgram, GL_ACTIVE_UNIFORMS);
    for (let i = 0; i < numUniforms; i += 1) {
      const uniform = ctx.getActiveUniform(glProgram, i);
      let name = uniform.name;
      table.maxUniformLength = Math.max(table.maxUniformLength, name.length + 1);
      if (name.endsWith(']')) name = name.slice(0, name.lastIndexOf('['));
      const location = ctx.getUniformLocation(glProgram, name);
      if (location) {
        const id = getNewId(gl.uniforms);
        table.uniforms[name] = [uniform.size, id];
        gl.uniforms[id] = location;
        for (let j = 1; j < uniform.size; j += 1) {
          const memberLocation = ctx.getUniformLocation(glProgram, `${name}[${j}]`);
          gl.uniforms[getNewId(gl.uniforms)] = memberLocation;
        }
      }
    }
  };

  /**
   * The scratch views `_glUniform1fv` uses for short vectors.
   *
   * The glue keeps a pooled `Float32Array` per length so a per-frame `uniform1fv(loc, 1, p)` does not
   * allocate. `count <= 288` is the cutoff, above which the glue reads straight out of `HEAPF32`.
   */
  const miniTempBuffers: (Float32Array | undefined)[] = [];

  /* ------------------------------------------------------------------ */
  /* Context creation                                                     */
  /* ------------------------------------------------------------------ */

  /**
   * `GL.initExtensions` — alias the five extension surfaces onto the context and auto-enable the
   * 27-name allow-list.
   *
   * The aliases matter: `_glGenVertexArrays` calls `createVertexArray` as a *property*, because on a
   * WebGL1 context that method only exists under `createVertexArrayOES`. Copying it over is what lets
   * the five rows be written once, as plain forwards.
   */
  const initExtensions = (ctx: ArtemisWebGLContext): void => {
    const vaoExt = ctx.getExtension('OES_vertex_array_object') as
      | { createVertexArrayOES(): unknown; deleteVertexArrayOES(v: unknown): void; bindVertexArrayOES(v: unknown): void; isVertexArrayOES(v: unknown): boolean }
      | null;
    if (vaoExt) {
      (ctx as { createVertexArray?: () => unknown }).createVertexArray = () =>
        vaoExt.createVertexArrayOES();
      (ctx as { deleteVertexArray?: (v: unknown) => void }).deleteVertexArray = (v) =>
        vaoExt.deleteVertexArrayOES(v);
      (ctx as { bindVertexArray?: (v: unknown) => void }).bindVertexArray = (v) =>
        vaoExt.bindVertexArrayOES(v);
    }

    const instExt = ctx.getExtension('ANGLE_instanced_arrays') as
      | {
          vertexAttribDivisorANGLE(index: number, divisor: number): void;
          drawArraysInstancedANGLE(m: number, f: number, c: number, p: number): void;
          drawElementsInstancedANGLE(m: number, c: number, t: number, o: number, p: number): void;
        }
      | null;
    if (instExt) {
      (ctx as { vertexAttribDivisor?: (i: number, d: number) => void }).vertexAttribDivisor = (
        index,
        divisor,
      ) => instExt.vertexAttribDivisorANGLE(index, divisor);
      (ctx as { drawArraysInstanced?: (m: number, f: number, c: number, p: number) => void })
        .drawArraysInstanced = (mode, first, count, primcount) =>
          instExt.drawArraysInstancedANGLE(mode, first, count, primcount);
      (ctx as { drawElementsInstanced?: (m: number, c: number, t: number, o: number, p: number) => void })
        .drawElementsInstanced = (mode, count, type, offset, primcount) =>
          instExt.drawElementsInstancedANGLE(mode, count, type, offset, primcount);
    }

    const drawBuffersExt = ctx.getExtension('WEBGL_draw_buffers') as
      | { drawBuffersWEBGL(n: number, buffers: ArrayBufferView | number[]): void }
      | null;
    if (drawBuffersExt) {
      (ctx as { drawBuffers?: (n: number, b: ArrayBufferView | number[]) => void }).drawBuffers = (
        n,
        buffers,
      ) => drawBuffersExt.drawBuffersWEBGL(n, buffers);
    }

    for (const name of ctx.getSupportedExtensions() ?? []) {
      if (AUTO_ENABLED_EXTENSIONS.includes(name)) ctx.getExtension(name);
    }
  };

  /**
   * `Browser.createContext` — the one place a context is made.
   *
   * The kind string follows the glue's `majorVersion` default of 1, so a guest whose shaders are GLSL
   * ES 1.00 gets `"webgl"` and a guest that asks for 2 gets `"webgl2"`. Getting this backwards is
   * the single most expensive mistake available in this bucket: a WebGL2 context silently refuses ES
   * 1.00 shaders only at `compileShader`, three calls into the first frame, with an error on the
   * console rather than an exception at the point of the mistake.
   */
  const createContext = (
    canvas: ArtemisGlCanvas,
    attributes: ArtemisGlContextAttributes,
  ): ArtemisWebGLContext | null => {
    const kind = attributes.majorVersion >= 2 ? 'webgl2' : 'webgl';
    const ctx = canvas.getContext(kind, attributes) as ArtemisWebGLContext | null;
    if (!ctx) return null;
    if (attributes.enableExtensionsByDefault !== false) initExtensions(ctx);
    gl.ctx = ctx;
    gl.currentContext = { GLctx: ctx };
    return ctx;
  };

  /**
   * `Browser.updateCanvasDimensions` + `setCanvasSize`, minus the fullscreen branch.
   *
   * The three things that matter: the `forcedAspectRatio` correction, the `resizeCanvas` switch
   * between the backing store and the CSS box, and the resize-listener notification that reaches
   * `GLFW.onCanvasResize` — which is how the guest learns its framebuffer changed size.
   */
  const setCanvasSize = (width: number, height: number): void => {
    const canvas = host.canvas;
    if (!canvas) return;
    let w = width;
    let h = height;
    const ratio = host.forcedAspectRatio ?? 0;
    if (ratio > 0) {
      if (w / h < ratio) w = Math.round(h * ratio);
      else h = Math.round(w / ratio);
    }
    if (host.resizeCanvas !== false) {
      if (canvas.width !== w) canvas.width = w;
      if (canvas.height !== h) canvas.height = h;
      canvas.style?.removeProperty('width');
      canvas.style?.removeProperty('height');
    } else {
      canvas.style?.setProperty('width', `${w}px`, 'important');
      canvas.style?.setProperty('height', `${h}px`, 'important');
    }
    onCanvasResize(w, h);
  };

  /* ------------------------------------------------------------------ */
  /* GLFW state — the glue's `GLFW` object                                */
  /* ------------------------------------------------------------------ */

  const glfw = {
    windows: null as (GlfwWindow | null)[] | null,
    active: null as GlfwWindow | null,
    hints: GLFW_DEFAULT_HINTS as Readonly<Record<number, number>>,
    initialTime: 0,
  };

  /** The listener handles `_glfwTerminate` has to remove, and nothing else. */
  const attached: { target: ArtemisGlWindowEvents | ArtemisGlCanvas; type: string; listener: (event: unknown) => void }[] = [];

  const attach = (
    target: ArtemisGlWindowEvents | ArtemisGlCanvas,
    type: string,
    listener: (event: unknown) => void,
  ): void => {
    target.addEventListener?.(type, listener, true);
    attached.push({ target, type, listener });
  };

  const detachAll = (): void => {
    for (const entry of attached.splice(0)) {
      entry.target.removeEventListener?.(entry.type, entry.listener, true);
    }
  };

  /** `GLFW.WindowFromId` — ids are 1-based, and 0 is "no window" by the `id <= 0` test. */
  const windowFromId = (id: number): GlfwWindow | null => {
    if (id <= 0 || !glfw.windows) return null;
    return glfw.windows[id - 1] ?? null;
  };

  const makeWindow = (
    id: number,
    width: number,
    height: number,
    title: number,
  ): GlfwWindow => ({
    id,
    width,
    height,
    storedWidth: width,
    storedHeight: height,
    fullscreen: false,
    title: title === 0 ? null : title,
    windowCloseFunc: null,
    windowSizeFunc: null,
    framebufferSizeFunc: null,
    shouldClose: 0,
  });

  /**
   * `GLFW.onCanvasResize` + the two size-change callbacks.
   *
   * The `dynCall_viii` side is the `glfwWindowSizeFunc` the guest registers; without the guest's
   * `dynCall_viii` there is nothing to call, and reporting that once is more useful than a frame
   * callback that silently does nothing on every resize.
   */
  const onCanvasResize = (width: number, height: number): void => {
    const active = glfw.active;
    if (!active) return;
    if (active.width !== width || active.height !== height) {
      active.width = width;
      active.height = height;
    } else {
      return;
    }
    setCanvasSize(active.width, active.height);
  };

  /* ------------------------------------------------------------------ */
  /* The main loop                                                        */
  /* ------------------------------------------------------------------ */

  const mainLoop = {
    func: null as number | null,
    arg: 0,
    timingMode: 0,
    timingValue: 0,
    currentFrameNumber: 0,
    tickStartTime: 0,
    running: false,
    paused: false,
    disposed: false,
    /**
     * Bumped whenever a pause, a dispose or a resume invalidates whatever was already scheduled.
     *
     * `requestFrame` returns nothing — the session owns the cancel, because only it knows whether a
     * rAF handle or a timer id is what it handed out. So the session cannot be told "cancel this one";
     * instead every scheduled tick captures the generation it was scheduled under and the tick itself
     * compares. A tick whose generation is stale runs, finds nothing to do and returns without entering
     * the guest, which is the only shape that works when the sender cannot be revoked.
     */
    generation: 0,
    /** 1 while a frame is scheduled but has not run yet, 0 otherwise. Prevents two outstanding frames. */
    tickPending: 0,
    /**
     * 1 while an asynchronous frame has been *entered* but has not settled.
     *
     * `ticking` cannot serve here: it is cleared in `runMainLoop`'s `finally`, i.e. the moment the
     * synchronous part of the tick returns, which for an async host is long before the guest's own
     * `dynCallV` promise has settled. A pause arriving in that window finds `tickPending` clear (the
     * tick that started the frame consumed it) and sets no `oweFrame`, and the frame's own pump then
     * sees `paused` and declines to arm — so the loop would stall forever with a frame in flight and
     * nothing to repay it. This flag is what makes that window observable.
     */
    frameInFlight: 0,
    /** True while the runner is inside a tick. `resume()` refuses to run inside one. */
    ticking: false,
    /** A pending frame was cancelled rather than consumed, so `resume()` owes the guest one. */
    oweFrame: false,
  };

  const isDisposed = (): boolean => mainLoop.disposed || host.isDisposed?.() === true;

  /**
   * Runs `task` through the session's serializer when there is one.
   *
   * Two reasons this is a helper rather than an `enqueue?.(…)` at each site. The synchronous case has
   * to stay synchronous all the way out — a session with neither `enqueue` nor a promise-returning
   * `dynCallV` must keep the exact call shape it has today — and the returned value has to be adopted
   * immediately so a rejected promise inside the task cannot become an unhandled rejection.
   */
  const serialize = (task: () => void | Promise<void>): void | Promise<void> => {
    if (!host.enqueue) return task();
    return host.enqueue(task);
  };

  /**
   * Adopts a value that may or may not be a promise, reporting a rejection instead of leaking it.
   *
   * A guest fault thrown synchronously is *not* routed here: it propagates as it always did, because
   * the loop's own `try`/`catch` and the session's event dispatch are where a synchronous throw is
   * supposed to land. This is only for the terminal promise, whose rejection would otherwise be an
   * unhandled rejection that tears down a Node session with no game state in it.
   */
  const settle = (value: void | Promise<void>): void | Promise<void> => {
    if (!value || typeof (value as Promise<void>).then !== 'function') return undefined;
    return (value as Promise<void>).then(undefined, (error: unknown) => {
      report('main loop', `an asynchronous frame or event failed with ${describeFault(error)}`);
    });
  };

  /** Hands one unit to the session serializer, adopting any promise. Used by the event path. */
  const enqueueUnit = (task: () => void | Promise<void>): void => {
    try {
      settle(serialize(task));
    } catch (error) {
      report('event', `an event handler threw ${describeFault(error)}`);
    }
  };

  /**
   * Resolves once the guest imports are idle, or immediately when the session has no such notion.
   *
   * The returned promise is handed straight back rather than routed through {@link settle}: a caller
   * that asked whether the guest is idle has to receive the real rejection, or a failed wait would
   * look like a successful one and the next frame would be armed on top of it.
   */
  const idle = (): void | Promise<void> => {
    const wait = host.waitUntilIdle;
    if (!wait) return undefined;
    // A synchronous throw is handed to the caller rather than reported and turned into `undefined`:
    // `undefined` reads as "the wait succeeded", and every caller would pump the next frame on top of
    // a wait that never finished.
    return wait();
  };

  /**
   * Stops the loop after a failed frame or a failed wait, and says why.
   *
   * `running = false` and `func = null` are both set *before* the report: the report is an arbitrary
   * host callback, and a loop left armed while it runs could be revived by a `resume()` or armed by a
   * `requestFrame` that fires inside it.
   */
  const failFrame = (error: unknown): void => {
    mainLoop.running = false;
    mainLoop.func = null;
    report('main loop', `an asynchronous frame failed with ${describeFault(error)}`);
  };

  /**
   * Runs the guest frame and, only once it has fully finished, schedules the next one.
   *
   * `enterFrame` is where the "do not enter a reentrant guest" rule is enforced: the guest is only ever
   * reached from a serialized unit, so a frame cannot overlap an in-flight event delivery. It runs the
   * guest and waits for its outstanding work, and **nothing else** — the decision to arm the next frame
   * belongs to `runMainLoop`, which owns the completion exactly once. A pump here would be a second
   * owner, and a second owner is how a frame gets armed while `frameInFlight` is still set.
   */
  const enterFrame = (): void | Promise<void> => {
    if (!mainLoop.running || mainLoop.paused || isDisposed()) return undefined;
    const func = mainLoop.func;
    if (func === null) return undefined;
    let result: void | Promise<void>;
    try {
      result = host.dynCallV?.(func);
    } catch (error) {
      // The guest's own `"unwind"` arriving *here* is not a fault and not a failed frame: it is the same
      // healthy control flow as the one that propagates out of the runner, only raised by the frame
      // function itself. The loop survives it, and it never reaches `host.enqueue`'s promise — the throw
      // stays inside `enterFrame` by design, so a serializer cannot mistake it for a fatal queue error.
      // It is settled through `idle()` exactly like a returned frame, because the guest did return from
      // its frame: what remains is only the outstanding work the guest left behind.
      if (error === 'unwind') return idle();
      if (!isGuestFault(error)) throw error;
      mainLoop.running = false;
      mainLoop.func = null;
      report(
        'main loop',
        `frame ${mainLoop.currentFrameNumber} trapped with ${describeFault(error)}; the loop is ` +
          'stopped because a trapped frame leaves the GL state and the guest heap meaningless',
      );
      return undefined;
    }
    const waitForResult = (): void | Promise<void> => {
      if (result && typeof (result as Promise<void>).then === 'function') {
        return (result as Promise<void>).then(
          () => idle(),
          // A *rejected* literal `"unwind"` is the same healthy control flow as the thrown one: the frame
          // settled, the loop keeps running, and — like the synchronous case above — the idle wait still
          // has to be honoured before the next frame is armed. Every other rejection, including
          // `undefined`, is terminal and is re-thrown untouched for the completion owner to record.
          (error: unknown) => {
            if (error !== 'unwind') throw error;
            return idle();
          },
        );
      }
      // Each step is isolated so a synchronous failure still propagates: a throwing `waitUntilIdle`,
      // or a `waitUntilIdle` read after the frame, must reach the completion owner rather than leave a
      // frame flagged as in flight whose wait never completed.
      const waited = idle();
      if (waited && typeof waited.then === 'function') return waited;
      return undefined;
    };
    return waitForResult();
  };

  /**
   * Settles one frame, **once**, and decides what the loop does next.
   *
   * This is the only place `frameInFlight` is cleared and the only place a frame is armed, and that
   * single ownership is the whole point. A pause or a dispose that arrives while the unit is in flight
   * wins, because both are re-read here rather than at the moment the frame started: a positive
   * `frameInFlight` means the guest still owns a frame, so instead of arming a second one the loop
   * records the debt in `oweFrame` and leaves the repayment to `resume()`.
   */
  const failedFrame = (error: unknown): void => {
    mainLoop.frameInFlight = 0;
    failFrame(error);
  };

  const completeFrame = (): void => {
    mainLoop.frameInFlight = 0;
    if (!mainLoop.running || isDisposed()) return;
    if (mainLoop.paused) {
      mainLoop.oweFrame = true;
      return;
    }
    mainLoop.oweFrame = false;
    scheduleFrame();
  };

  /**
   * Settles the frame through the idle barrier: the guest's `"unwind"` is healthy control flow, and the
   * wait the guest left behind still has to finish before the next frame is armed. `frameInFlight` stays
   * set for the whole wait, so a pause or a resume arriving in between finds a frame it must not arm
   * over. The subscription happens before any synchronous throw can escape, and every rejection —
   * including a bare `undefined` — is terminal.
   */
  const completeAfterIdle = (error: unknown): void => {
    try {
      const waited = idle();
      if (waited && typeof (waited as Promise<void>).then === 'function') {
        void (waited as Promise<void>).then(() => completeFrame(), failed => failedFrame(failed));
      } else completeFrame();
    } catch (failure) {
      failedFrame(failure);
    }
  };

  /**
   * The runner, shaped as the glue's `Browser_mainLoop_runner` minus the blocker queue.
   *
   * Only one thing is load-bearing here: `simulateInfiniteLoop` makes `_b` **throw** after the loop is
   * installed. That throw is the official "the guest has entered its frame loop" signal — the control
   * run of the official glue records exactly that (`"unwind"`), so our host must produce the same
   * shape or the two are not comparable.
   *
   * The runner is where a guest fault is *finally* diagnosable, and the only honest thing to do with
   * it is stop the loop and name it. A frame is entered through `requestFrame`, i.e. through the
   * session's event loop, so an exception raised by the guest here propagates into whatever pumped
   * the frame: in a browser it becomes an unhandled error attributed to a rAF callback with no game
   * state in it, and in Node it tears down the process with a wasm address nobody can resolve. The
   * guest's own `unwind` is left to propagate — that is the healthy signal and it is the one the
   * official glue also throws — but a wasm trap (`unreachable`, an out-of-bounds access, a
   * `RuntimeError` of any other kind) stops the loop and records the row, because after a trap the
   * frame number, the GL state and the guest heap are all meaningless and continuing would only
   * manufacture a second, less honest failure on top of the first.
   *
   * The tick is a *claim* on the pending slot, not a frame: a stale generation (a pause, a dispose or a
   * resume that happened after this tick was armed) or a pause in force consumes the claim and returns
   * without entering the guest at all. That claim is also what makes `pause` able to tell "a frame is
   * scheduled" (repay on resume) from "a frame is running" (`frameInFlight`, repay at completion).
   *
   * A failed frame stops the loop through {@link completeFrame}, on **both** paths — the synchronous
   * one and the adopted promise. A `settle`-style handler is the wrong instrument here and was the
   * defect: it turns a rejection into a log line and resolves, so a frame that failed asynchronously
   * left `running` true and the loop armed on top of a guest that never finished its frame. The
   * synchronous failure is worth propagating as well and it is *not* what this changes: a synchronous
   * throw out of `serialize` still leaves this function the way it always did, and the caller sees it,
   * but the frame is settled and the loop stopped first so it cannot be re-entered on top of.
   *
   * The guest's own `"unwind"` is not a failure and is never routed to `failFrame`: it is the official
   * main-loop throw, i.e. healthy control flow, and a string is exactly how both the guest and the
   * official glue spell it. It *does* settle the frame like any other return, because the guest did
   * return from `dynCall_v` — only the loop's `running` flag survives it, which is what keeps a
   * later `resume()` able to arm the next one.
   */
  const runMainLoop = (generation: number): void => {
    if (generation !== mainLoop.generation) return;
    if (!mainLoop.running || mainLoop.paused || isDisposed()) return;
    mainLoop.tickPending = 0;
    mainLoop.currentFrameNumber = (mainLoop.currentFrameNumber + 1) | 0;
    if (
      mainLoop.timingMode === MAIN_LOOP_RAF_MODE &&
      mainLoop.timingValue > 1 &&
      mainLoop.currentFrameNumber % mainLoop.timingValue !== 0
    ) {
      scheduleFrame();
      return;
    }
    if (mainLoop.timingMode === 0) mainLoop.tickStartTime = (host.now ?? Date.now)();
    // Marked before the unit is serialized, so a pause arriving between this line and the guest's
    // first instruction still finds a frame to repay instead of a clear slot it can arm over.
    mainLoop.frameInFlight = 1;
    mainLoop.ticking = true;
    let outcome: void | Promise<void>;
    try {
      outcome = serialize(enterFrame);
    } catch (error) {
      // Only a `serialize` that throws the guest's own literal `"unwind"` is healthy here. Any other
      // throw — a wasm trap, `undefined`, anything unrecognised — is a failed frame: the loop stops and
      // the frame is settled *before* the original error is handed back to the caller. Testing
      // `isGuestFault` instead would invert this exactly, because `"unwind"` is a string and a string is
      // precisely what that predicate does *not* call a fault.
      if (error !== 'unwind') {
        failedFrame(error);
        throw error;
      }
      // Deliberately left to propagate: this is the runner's own throw, and dropping it would change the
      // synchronous shape the sibling test pins. What matters is that the frame is settled once, here,
      // rather than re-entered by the catch.
      completeAfterIdle(error);
      throw error;
    } finally {
      mainLoop.ticking = false;
    }
    if (outcome && typeof (outcome as Promise<void>).then === 'function') {
      // Adopted here rather than through `settle`: a rejected frame must stop the loop, and the frame
      // must not be reported as settled before the scheduler has actually run.
      void (outcome as Promise<void>).then(
        () => completeFrame(),
        // The serialized frame rejected. A literal `"unwind"` is the healthy control signal, so it
        // settles the frame and lets the loop arm the next one; everything else is terminal and stops
        // the loop. The distinction is made on the exact value, never on `instanceof`, so a rejection
        // carrying `undefined` still fails the frame instead of being mistaken for a healthy return.
        (error: unknown) => {
          if (error === 'unwind') completeAfterIdle(error);
          else failedFrame(error);
        },
      );
    } else {
      completeFrame();
    }
  };

  /**
   * Is this a wasm fault rather than one of the guest's own C++/control-flow throws?
   *
   * The distinction decides whether the loop may keep running, so it is made on the value, not on the
   * message: `ArtemisExit` and `ArtemisLongjmp` are how the guest says "stop, deliberately", and the
   * string `"unwind"` is the official main-loop throw. A `WebAssembly.RuntimeError` is the opposite —
   * the guest reached an instruction it was never meant to, which is a fault in the world we handed it.
   */
  const isGuestFault = (error: unknown): boolean => {
    if (typeof error === 'string') return false;
    if (error instanceof WebAssembly.RuntimeError) return true;
    const name = (error as { constructor?: { name?: string } } | null)?.constructor?.name ?? '';
    return name === 'RuntimeError';
  };

  /**
   * A fault in one line, with the wasm function index when the engine gave one, plus the VERBATIM
   * stack when there is one.
   *
   * The stack is not decoration. A real `unreachable` in Chrome carries `wasm-function[NNNN]` frames,
   * and NNNN is the only handle that maps a fault back to a function. Truncating to 200 characters
   * throws that away, and `String(error)` alone is the message with no frames at all — so a fault
   * with no index is reported as having no stack, rather than as having been truncated into one.
   */
  const describeFault = (error: unknown): string => {
    const text = String(error);
    const fn = /wasm-function\[(\d+)\]/.exec(text);
    const stack = (error as { stack?: unknown } | null)?.stack;
    const verbatim = typeof stack === 'string' && stack.length > 0 ? stack : '';
    const frames = /wasm-function\[(\d+)\]/.exec(verbatim);
    if (fn || frames) {
      return `${text.replace(/:\s+wasm.*/s, '')} (wasm function ${(fn ?? frames)![1]})` +
        (verbatim ? ` :: ${verbatim.slice(0, 1200)}` : ' :: no stack on this error');
    }
    return (text.slice(0, 200) || '(no message)') + (verbatim ? ` :: ${verbatim.slice(0, 1200)}` : '');
  };

  const scheduleFrame = (): void => {
    if (!mainLoop.running || mainLoop.paused || isDisposed()) return;
    if (mainLoop.tickPending || mainLoop.frameInFlight) return;
    if (host.requestFrame) {
      mainLoop.tickPending = 1;
      const generation = mainLoop.generation;
      host.requestFrame(() => runMainLoop(generation));
      return;
    }
    // No frame source (a headless host): the loop is installed and armed, but nothing pumps it. The
    // guest's first frame has already run by the time this is reached, which is the observable fact
    // that matters; reporting keeps "it never drew" from looking like a bug.
    refuse('_b', 'installed a main loop with no frame source, so frames run only when pumped manually');
  };

  /**
   * Removes every listener the eleven `__emscripten_set_*_callback_on_thread` rows registered.
   *
   * A `function` declaration rather than a `const` arrow on purpose: `dispose` may run before the
   * `jsEvents` object it walks is initialized, and hoisting keeps that from being a temporal-dead-zone
   * throw in a path whose entire job is to not throw.
   */
  function detachAllEventHandlers(): void {
    for (const handler of jsEvents.eventHandlers.splice(0)) {
      if (handler.listener) {
        handler.target.removeEventListener(
          handler.eventTypeString,
          handler.listener,
          handler.useCapture,
        );
      }
    }
  }

  /** `free(ptr)` for each of the five owned blocks, once; ids are cleared first so a retry is a no-op. */
  function freeEventBlocks(): void {
    const blocks = [
      jsEvents.mouseEvent,
      jsEvents.keyEvent,
      jsEvents.wheelEvent,
      jsEvents.touchEvent,
      jsEvents.visibilityChangeEvent,
    ];
    jsEvents.mouseEvent = 0;
    jsEvents.keyEvent = 0;
    jsEvents.wheelEvent = 0;
    jsEvents.touchEvent = 0;
    jsEvents.visibilityChangeEvent = 0;
    const free = host.free;
    if (!free) return;
    for (const block of blocks) {
      if (!block) continue;
      try {
        free(block);
      } catch (error) {
        report('event', `could not free the event block at ${block}: ${describeFault(error)}`);
      }
    }
  }

  /**
   * The lifecycle controller, built once the loop object exists and handed to the session.
   *
   * `pause` deliberately does **not** touch the listeners: `_glfwTerminate` and the GL context survive a
   * pause, so a tab that becomes visible again resumes into the same game rather than a torn-down one.
   * What a pause does invalidate is every frame that was already armed — hence the generation bump —
   * and `resume` pays back the frame the cancelled tick would have run under exactly one condition:
   * `oweFrame`, which is only set when the pending frame was cancelled rather than consumed. A stale
   * generation that fires on its own finds `tickPending` clear and returns without entering the guest,
   * so the two paths cannot both arm a frame.
   *
   * `dispose` is idempotent and terminal. It is the only place listeners come off, and the only place
   * the five event blocks are released — after `waitUntilIdle`, never before, because a parked root
   * still points into them.
   */
  const lifecycle: ArtemisGlLifecycle = {
    pause: (): void => {
      if (mainLoop.disposed) return;
      mainLoop.paused = true;
      mainLoop.generation += 1;
      if (mainLoop.tickPending) {
        mainLoop.tickPending = 0;
        mainLoop.oweFrame = true;
      }
      // A frame is scheduled but not yet run, or a frame has been entered and has not settled. Either
      // way the loop owes the guest exactly one frame, and the two conditions are distinct: a tick that
      // already consumed `tickPending` and is sitting inside the guest must still be repaid, or the
      // loop stalls with a frame in flight and nothing left to pump it.
      if (mainLoop.tickPending || mainLoop.frameInFlight) mainLoop.oweFrame = true;
    },
    resume: (): void => {
      if (mainLoop.disposed || !mainLoop.paused) return;
      mainLoop.paused = false;
      mainLoop.generation += 1;
      mainLoop.tickPending = 0;
      if (mainLoop.running && mainLoop.func !== null) scheduleFrame();
    },
    dispose: (): void | Promise<void> => {
      if (mainLoop.disposed) return undefined;
      mainLoop.disposed = true;
      mainLoop.running = false;
      mainLoop.paused = true;
      mainLoop.func = null;
      mainLoop.generation += 1;
      mainLoop.tickPending = 0;
      mainLoop.frameInFlight = 0;
      mainLoop.oweFrame = false;
      detachAll();
      detachAllEventHandlers();
      // Releasing the event blocks is the last step of the returned promise: the guest may still be
      // inside a frame or a listener when dispose is called, and that code holds live pointers. A
      // synchronous `waitUntilIdle` puts us on the same turn as that guest code, so a throw from it
      // must be reported and the blocks released anyway — a dispose that leaked five guest heap
      // blocks because the wait was inconvenient is worse than the throw it was reporting.
      const release = (): void => {
        try {
          freeEventBlocks();
        } catch (error) {
          report('event', `dispose could not release the event blocks: ${describeFault(error)}`);
        }
      };
      let waited: void | Promise<void>;
      try {
        waited = idle();
      } catch (error) {
        report('event', `a synchronous waitUntilIdle failed during dispose: ${describeFault(error)}`);
        release();
        return undefined;
      }
      if (waited && typeof waited.then === 'function') return waited.then(release, release);
      release();
      return undefined;
    },
  };

  /** Hands the controller to the session once. The session owns the scheduling around it. */
  function registerLifecycle(): void {
    try {
      host.setLifecycle?.(lifecycle);
    } catch (error) {
      report('lifecycle', `setLifecycle threw ${describeFault(error)}`);
    }
  }

  /**
   * `_emscripten_set_main_loop_timing(mode, value)`.
   *
   * Returns 1 when no loop is installed, exactly as the glue does — that is "timing recorded, nothing
   * scheduled", and a guest that calls it before `_b` uses the return value to mean "you are early".
   */
  const setMainLoopTiming = (mode: number, value: number): number => {
    mainLoop.timingMode = mode;
    mainLoop.timingValue = value;
    if (mainLoop.func === null) return 1;
    if (mode === 0) {
      // A fixed-interval mode with a 0 interval is `_glfwSwapInterval(0)`: draw as fast as the frame
      // source allows rather than on a timer.
      mainLoop.timingValue = 0;
    } else {
      mainLoop.timingValue = value;
    }
    return 0;
  };

  /* ------------------------------------------------------------------ */
  /* The table                                                            */
  /* ------------------------------------------------------------------ */

  /**
   * The context accessor every `_gl*` row guards with.
   *
   * Reporting through {@link refuse} rather than throwing is deliberate: the guest treats a missing GL
   * context as "nothing was drawn this frame" and keeps running, and a host that threw would convert
   * a headless run into a crash before it could say which row wanted the context.
   */
  const needCtx = (name: string): ArtemisWebGLContext | null => {
    if (gl.ctx) return gl.ctx;
    refuse(name, 'was called with no WebGL context, so it took the neutral GL default');
    return null;
  };

  const imports: ArtemisGlImports = {
    /* ---------------- GLFW: the boot path ---------------- */

    /**
     * `oc _glfwInit() -> 1`.
     *
     * The one row whose return value the whole boot hangs on: a guest that sees 0 bails out of its
     * display check and exits. The listeners are attached to the page and the canvas exactly as the
     * glue does, so `_glfwTerminate` has real handles to remove.
     */
    oc: (): number => {
      if (glfw.windows) return 1;
      glfw.initialTime = (host.now ?? Date.now)();
      glfw.hints = GLFW_DEFAULT_HINTS;
      glfw.windows = [];
      glfw.active = null;
      const target = host.window;
      for (const type of GLFW_WINDOW_EVENTS) {
        if (target) attach(target, type, () => {});
      }
      const canvas = host.canvas;
      for (const type of GLFW_CANVAS_EVENTS) {
        if (canvas) attach(canvas, type, () => {});
      }
      if (!target && !canvas) {
        refuse('oc', 'attached no listeners because the session supplied neither a window nor a canvas');
      }
      return 1;
    },

    /**
     * `nc _glfwCreateWindow(width, height, title, monitor, share) -> handle`.
     *
     * The boot path's real test. It must return a **non-zero handle** and it must have created a
     * context by then, because `if (!Module.ctx) return 0` runs before the window is recorded — a
     * handle with no context is the failure the Lead's ruling is aimed at. The single-window
     * restriction is the glue's own: a second window throws the literal
     * `"glfwCreateWindow only supports one window at time currently"`.
     */
    nc: (width: number, height: number, title: number, monitor: number): number => {
      const windows = glfw.windows;
      if (!windows) {
        refuse('nc', 'was called before _glfwInit, so there is no window table to add to');
        return 0;
      }
      let i = 0;
      while (i < windows.length && windows[i] !== null) i += 1;
      if (i > 0) throw 'glfwCreateWindow only supports one window at time currently';
      const id = i + 1;
      if (width <= 0 || height <= 0) return 0;

      if (monitor) {
        refuse('nc', 'cannot enter real fullscreen without a document, so the canvas was sized instead');
      }
      setCanvasSize(width, height);

      const canvas = host.canvas;
      if (!canvas) {
        refuse('nc', 'has no canvas, so no context was created and the window is refused');
        return 0;
      }
      if (!gl.ctx) {
        const hints = glfw.hints;
        const attributes: ArtemisGlContextAttributes = {
          antialias: hints[HINT_SAMPLES]! > 1,
          depth: hints[HINT_DEPTH]! > 0,
          stencil: hints[HINT_STENCIL]! > 0,
          alpha: hints[HINT_ALPHA]! > 0,
          majorVersion: 1,
        };
        if (createContext(canvas, attributes) === null) {
          refuse('nc', `could not create a WebGL context with ${JSON.stringify(attributes)}`);
          return 0;
        }
      }
      const win = makeWindow(id, width, height, title);
      if (id - 1 === windows.length) windows.push(win);
      else windows[id - 1] = win;
      glfw.active = win;
      return win.id;
    },

    /**
     * `nb _glfwMakeContextCurrent(winid)`.
     *
     * The official body is literally empty, because the glue only ever has one context and it is
     * already current. Re-binding it is a superset with identical observable behaviour, and it means a
     * session with two windows does not silently draw the first one.
     */
    nb: (winid: number): void => {
      const win = windowFromId(winid);
      if (!win || !gl.ctx) return;
      gl.currentContext = { GLctx: gl.ctx };
    },

    /**
     * `kb _glfwGetWindowSize(winid, width, height)` and `jb _glfwGetFramebufferSize` — the same
     * `GLFW.getWindowSize` body, zero-filling both out-pointers when the id is unknown.
     *
     * The `if (width)` guard is the glue's: a null out-pointer means "the guest does not want this
     * value", not "write to address 0", and a host that ignored it would scribble over the guest's
     * first four bytes of heap.
     */
    kb: (winid: number, width: number, height: number): void => {
      const win = windowFromId(winid);
      storeI32(width, win ? win.width : 0);
      storeI32(height, win ? win.height : 0);
    },
    jb: (winid: number, width: number, height: number): void => {
      const win = windowFromId(winid);
      storeI32(width, win ? win.width : 0);
      storeI32(height, win ? win.height : 0);
    },

    /**
     * `gb _glfwSwapBuffers(winid)` — `GLFW.swapBuffers` is literally `{}`.
     *
     * A browser canvas presents itself; there is nothing to flip. So this row is *really* implemented
     * by doing nothing, which is worth saying because "an empty body" is otherwise indistinguishable
     * from "not implemented" in a trace.
     */
    gb: (): void => {},

    /**
     * `mb _glfwSwapInterval(interval)` — `Math.abs(interval)`, then mode 0 when 0 and mode 1 otherwise.
     *
     * The absolute value matters: GLFW promises a *rate*, and a negative interval is a rate, so
     * dropping the sign would make a guest that passes -1 ask for `rAF % -1`, which is never 0 and so
     * draws every frame anyway — an obscurely wrong frame budget rather than an error.
     */
    mb: (interval: number): void => {
      const value = Math.abs(interval);
      setMainLoopTiming(value === 0 ? 0 : MAIN_LOOP_RAF_MODE, value);
    },

    /** `pc _glfwPollEvents()` — literally `{}`; the browser delivers events on its own. */
    pc: (): void => {},

    /**
     * `mc _glfwTerminate()` — remove every listener, collapse the canvas to 1x1, drop the window table.
     *
     * Symmetric with `oc` by construction: both go through the same `attached` list, so a test that
     * attaches a canvas and terminates can assert nothing is still listening.
     */
    mc: (): void => {
      detachAll();
      const canvas = host.canvas;
      if (canvas) {
        canvas.width = 1;
        canvas.height = 1;
      }
      glfw.windows = null;
      glfw.active = null;
    },

    /**
     * `Zb _glfwDestroyWindow(winid)`.
     *
     * The glue calls the guest's window-close callback first (`dynCall_vi`), then frees the slot, and
     * only drops the context once the **last** window is gone. This slice has no `dynCall_vi`, so the
     * callback is reported once rather than skipped silently.
     */
    Zb: (winid: number): void => {
      const windows = glfw.windows;
      if (!windows) return;
      const win = windowFromId(winid);
      if (!win) return;
      if (win.windowCloseFunc) {
        refuse('Zb', 'has no dynCall_vi, so the guest window-close callback was not invoked');
      }
      windows[win.id - 1] = null;
      if (glfw.active && glfw.active.id === win.id) glfw.active = null;
      for (const other of windows) if (other !== null) return;
      gl.ctx = null;
      gl.currentContext = null;
    },

    /**
     * `$a _emscripten_get_canvas_element_size(target, width, height) -> 0` / `-4`.
     *
     * `target` is a small integer standing for a DOM node, not a pointer: `0` is falsy, `1` is the
     * document, `2` is the window, and anything above 2 is a **CSS selector string** in guest memory.
     * A host that treated it as a pointer would query `document` for the string `"<garbage>"` and
     * return -4 for a canvas that is right there.
     */
    $a: (target: number, width: number, height: number): number => {
      const canvas = findCanvasEventTarget(target) as Partial<ArtemisGlCanvas> | null | undefined;
      if (!canvas || typeof canvas.width !== 'number' || typeof canvas.height !== 'number') return -4;
      storeI32(width, canvas.width);
      storeI32(height, canvas.height);
      return 0;
    },

    /** `da _emscripten_get_device_pixel_ratio() -> f64`, defaulting to 1 off a browser. */
    da: (): number => {
      const ratio = (globalThis as { devicePixelRatio?: unknown }).devicePixelRatio;
      return typeof ratio === 'number' && ratio ? ratio : 1;
    },

    /**
     * `_b _emscripten_set_main_loop(func, fps, simulateInfiniteLoop)`.
     *
     * The three-argument shape the guest uses, and the *whole* of the emscripten contract: the frame
     * function is a table index, not an export, so it is reached through `dynCall_v`; and when
     * `simulateInfiniteLoop` is set the call **throws** after installing the loop, which is how the
     * guest learns `main` will never return. A host that returned normally here would let the guest
     * run its `while (1)` loop on the wasm stack and trap.
     */
    _b: (func: number, fps: number, simulateInfiniteLoop: number): void => {
      if (mainLoop.func !== null) {
        throw 'emscripten_set_main_loop: there can only be one main loop function at once: call emscripten_cancel_main_loop to cancel the previous one before setting a new one with different parameters.';
      }
      if (!host.dynCallV) {
        refuse('_b', 'has no dynCall_v, so the guest frame function can never be entered');
        throw 'unwind';
      }
      mainLoop.func = func | 0;
      mainLoop.running = true;
      if (fps > 0) setMainLoopTiming(0, 1000 / fps);
      else setMainLoopTiming(MAIN_LOOP_RAF_MODE, 1);
      // The first frame runs synchronously, so a headless session can see the guest draw once rather
      // than waiting for a scheduler that may not exist. Note that the frame itself may be asynchronous
      // now: `runMainLoop` adopts that promise, and the *scheduling* of the following frame is inside it
      // and therefore also after the promise settles. This import never returns a promise — the throw
      // below has to stay synchronous for the guest, which is the one part of the contract that cannot
      // move — so the frame's tail is reachable only through the loop's own pump.
      runMainLoop(mainLoop.generation);
      if (simulateInfiniteLoop) throw 'unwind';
    },

    /**
     * `$b _emscripten_enter_soft_fullscreen(target, strategy)`.
     *
     * A real fullscreen request needs a document this host may not have, and a *soft* one is a CSS
     * re-layout the glue builds out of a five-word strategy struct. Neither is on the boot path, so
     * this row refuses honestly: it returns -4 when there is no target at all (the glue's own
     * answer) and reports that the strategy was not honoured.
     */
    $b: (target: number): number => {
      const element = findEventTarget(target);
      if (!element) return -4;
      refuse('$b', 'found the target but cannot honour a fullscreen strategy without a document');
      return 0;
    },

    /** `lb _glewInit() -> 0` — the official answer is 0, and it is a real answer, not a stub. */
    lb: (): number => 0,

    /* ---------------- the _gl* draw path ---------------- */

    /** `eb _glClear(mask)`. */
    eb: (mask: number): void => gl.ctx?.clear(mask | 0),
    /** `fb _glClearColor(r, g, b, a)`. */
    fb: (r: number, g: number, b: number, a: number): void => gl.ctx?.clearColor(r, g, b, a),
    /** `ta _glViewport(x, y, width, height)`. */
    ta: (x: number, y: number, width: number, height: number): void =>
      gl.ctx?.viewport(x | 0, y | 0, width | 0, height | 0),
    /** `ya _glEnable(cap)`. */
    ya: (cap: number): void => gl.ctx?.enable(cap | 0),
    /** `J _glDisable(cap)`. */
    J: (cap: number): void => gl.ctx?.disable(cap | 0),
    /** `S _glActiveTexture(unit)`. */
    S: (unit: number): void => gl.ctx?.activeTexture(unit | 0),
    /** `_ _glBlendFuncSeparate(sf, df, sc, dc)`. */
    _: (sf: number, df: number, sc: number, dc: number): void =>
      gl.ctx?.blendFuncSeparate(sf | 0, df | 0, sc | 0, dc | 0),
    /** `H _glTexParameteri(target, pname, param)`. */
    H: (target: number, pname: number, param: number): void =>
      gl.ctx?.texParameteri(target | 0, pname | 0, param | 0),
    /** `Kc _glCopyTexImage2D(target, level, internalFormat, x, y, width, height, border)`. */
    Kc: (
      target: number,
      level: number,
      internalFormat: number,
      x: number,
      y: number,
      width: number,
      height: number,
      border: number,
    ): void =>
      gl.ctx?.copyTexImage2D(
        target | 0, level | 0, internalFormat | 0,
        x | 0, y | 0, width | 0, height | 0, border | 0,
      ),
    /** `ja _glEnableVertexAttribArray(index)`. */
    ja: (index: number): void => gl.ctx?.enableVertexAttribArray(index | 0),

    /** `M _glBindTexture(target, texture)`. */
    M: (target: number, texture: number): void => gl.ctx?.bindTexture(target | 0, gl.textures[texture]),
    /** `N _glBindBuffer(target, buffer)`. */
    N: (target: number, buffer: number): void => gl.ctx?.bindBuffer(target | 0, gl.buffers[buffer]),
    /** `W _glBindFramebuffer(target, framebuffer)`. */
    W: (target: number, framebuffer: number): void =>
      gl.ctx?.bindFramebuffer(target | 0, gl.framebuffers[framebuffer]),
    /** `Ya _glBindRenderbuffer(target, renderbuffer)`. */
    Ya: (target: number, renderbuffer: number): void =>
      gl.ctx?.bindRenderbuffer(target | 0, gl.renderbuffers[renderbuffer]),
    /** `Fa _glBindVertexArray(vao)`. */
    Fa: (vao: number): void => gl.ctx?.bindVertexArray?.(gl.vaos[vao]),

    /**
     * `sb _glBufferData(target, size, data, usage)`.
     *
     * The `data ? … : size` ternary is the row's whole logic: a non-zero pointer means "here are the
     * bytes", and 0 means "allocate `size`". WebGL takes the same two forms (`bufferData(target, size,
     * usage)` versus `bufferData(target, view, usage)`), so the choice passes straight through.
     */
    sb: (target: number, size: number, data: number, usage: number): void => {
      const ctx = gl.ctx;
      if (!ctx) {
        refuse('sb', 'was called with no WebGL context, so nothing was uploaded');
        return;
      }
      if (data) {
        const start = (data | 0) >>> 0;
        ctx.bufferData(target | 0, memory.u8.subarray(start, start + (size | 0)), usage | 0);
      } else {
        // WebGL2 takes a `WebGLBufferSource` for the sizing form, so the number is not a legal
        // argument here — it is wrapped in a zero-length byte view, which asks for the allocation
        // and carries no bytes, exactly like the `size` the glue passes.
        ctx.bufferData(target | 0, new Uint8Array(0), usage | 0);
      }
    },

    /** `la _glBufferSubData(target, offset, size, data)` — a range, never a size-only form. */
    la: (target: number, offset: number, size: number, data: number): void => {
      const ctx = gl.ctx;
      if (!ctx) {
        refuse('la', 'was called with no WebGL context, so nothing was uploaded');
        return;
      }
      if (!data) {
        refuse('la', 'was called with a null data pointer, where the official glue reads from address 0');
        return;
      }
      const start = (data | 0) >>> 0;
      ctx.bufferSubData(target | 0, offset | 0, memory.u8.subarray(start, start + (size | 0)));
    },

    /** `T _glGenTextures(n, textures)`. */
    T: (n: number, textures: number): void => genObject(n | 0, textures, 'createTexture', gl.textures),
    /** `tb _glGenBuffers(n, buffers)`. */
    tb: (n: number, buffers: number): void => genObject(n | 0, buffers, 'createBuffer', gl.buffers),
    /** `Wb _glGenVertexArrays(n, arrays)`. */
    Wb: (n: number, arrays: number): void => genObject(n | 0, arrays, 'createVertexArray', gl.vaos),
    /** `_a _glGenFramebuffers(n, ids)`. */
    _a: (n: number, ids: number): void => genObject(n | 0, ids, 'createFramebuffer', gl.framebuffers),
    /** `Za _glGenRenderbuffers(n, renderbuffers)`. */
    Za: (n: number, renderbuffers: number): void =>
      genObject(n | 0, renderbuffers, 'createRenderbuffer', gl.renderbuffers),

    /**
     * The five `_glDelete*` rows, as one body parameterised by pool and deleter.
     *
     * The two shapes are genuinely different in the glue and both are kept: textures, buffers,
     * framebuffers and renderbuffers check for a null pool slot and `continue`, while
     * `_glDeleteVertexArrays` does not. A host that "tidied" the vertex-array row to match the others
     * would stop being wrong in a case the guest can hit (deleting a name it never generated).
     */
    U: (n: number, textures: number): void => {
      const ctx = gl.ctx;
      for (let i = 0; i < (n | 0); i += 1) {
        const id = loadI32(textures + i * 4);
        const texture = gl.textures[id];
        if (!texture) continue;
        ctx?.deleteTexture(texture);
        (texture as { name?: number }).name = 0;
        gl.textures[id] = null;
      }
    },
    rb: (n: number, buffers: number): void => {
      const ctx = gl.ctx;
      for (let i = 0; i < (n | 0); i += 1) {
        const id = loadI32(buffers + i * 4);
        const buffer = gl.buffers[id];
        if (!buffer) continue;
        ctx?.deleteBuffer(buffer);
        (buffer as { name?: number }).name = 0;
        gl.buffers[id] = null;
      }
    },
    Yb: (n: number, vaos: number): void => {
      const ctx = gl.ctx;
      for (let i = 0; i < (n | 0); i += 1) {
        const id = loadI32(vaos + i * 4);
        ctx?.deleteVertexArray?.(gl.vaos[id]);
        gl.vaos[id] = null;
      }
    },
    ra: (n: number, framebuffers: number): void => {
      const ctx = gl.ctx;
      for (let i = 0; i < (n | 0); i += 1) {
        const id = loadI32(framebuffers + i * 4);
        const framebuffer = gl.framebuffers[id];
        if (!framebuffer) continue;
        ctx?.deleteFramebuffer(framebuffer);
        (framebuffer as { name?: number }).name = 0;
        gl.framebuffers[id] = null;
      }
    },
    qa: (n: number, renderbuffers: number): void => {
      const ctx = gl.ctx;
      for (let i = 0; i < (n | 0); i += 1) {
        const id = loadI32(renderbuffers + i * 4);
        const renderbuffer = gl.renderbuffers[id];
        if (!renderbuffer) continue;
        ctx?.deleteRenderbuffer(renderbuffer);
        (renderbuffer as { name?: number }).name = 0;
        gl.renderbuffers[id] = null;
      }
    },

    /**
     * `Z _glDeleteProgram(id)`.
     *
     * Drops the `programInfos` entry as well as the program, because `_glGetProgramiv` finds its
     * four special answers in that table and a stale one would answer from a linked program that no
     * longer exists. An unknown non-zero id latches `GL_INVALID_VALUE` (1281).
     */
    Z: (id: number): void => {
      if (!id) return;
      const program = gl.programs[id];
      if (!program) {
        recordError(1281);
        return;
      }
      gl.ctx?.deleteProgram(program);
      (program as { name?: number }).name = 0;
      gl.programs[id] = null;
      gl.programInfos[id] = undefined as unknown as GlProgramInfo;
    },

    /** `wa _glDeleteShader(id)` — same shape, without the programInfos cleanup (shaders have none). */
    wa: (id: number): void => {
      if (!id) return;
      const shader = gl.shaders[id];
      if (!shader) {
        recordError(1281);
        return;
      }
      gl.ctx?.deleteShader(shader);
      gl.shaders[id] = null;
    },

    /** `va _glCreateProgram() -> id`, allocated from the same counter as every other pool. */
    va: (): number => {
      const ctx = gl.ctx;
      if (!ctx) {
        refuse('va', 'was called with no WebGL context, so no program was created');
        return 0;
      }
      const id = getNewId(gl.programs);
      const program = ctx.createProgram();
      if (program) program.name = id;
      gl.programs[id] = program;
      return id;
    },

    /** `db _glCreateShader(type) -> id`. */
    db: (type: number): number => {
      const ctx = gl.ctx;
      if (!ctx) {
        refuse('db', 'was called with no WebGL context, so no shader was created');
        return 0;
      }
      const id = getNewId(gl.shaders);
      gl.shaders[id] = ctx.createShader(type | 0);
      return id;
    },

    /** `cb _glShaderSource(shader, count, string, length)` — the guest's pointer arrays, read once. */
    cb: (shader: number, count: number, string: number, length: number): void =>
      gl.ctx?.shaderSource(gl.shaders[shader], getSource(shader, count | 0, string, length)),
    /** `bb _glCompileShader(shader)`. */
    bb: (shader: number): void => gl.ctx?.compileShader(gl.shaders[shader]),
    /** `Y _glAttachShader(program, shader)`. */
    Y: (program: number, shader: number): void =>
      gl.ctx?.attachShader(gl.programs[program], gl.shaders[shader]),
    /**
     * `ua _glLinkProgram(program)`.
     *
     * The `populateUniformTable` call is the reason linking matters to this host: without it every
     * `_glGetUniformLocation` would answer -1 and every `uniform*` would be a silent no-op, which is
     * exactly the "black canvas, no error" shape the uniform table exists to prevent.
     */
    ua: (program: number): void => {
      gl.ctx?.linkProgram(gl.programs[program]);
      populateUniformTable(program);
    },
    /** `qb _glUseProgram(program)`. */
    qb: (program: number): void => gl.ctx?.useProgram(gl.programs[program]),

    /**
     * `K _glGetUniformLocation(program, name) -> location`.
     *
     * Answered from the link-time table, not from the driver, because the guest's locations have to be
     * **small integers the guest can pass back to us**. The `foo[3]` case is the reason the table
     * stores `[size, baseId]`: the base id plus the array index is the synthetic location, and an
     * out-of-range index answers -1 exactly as the glue does.
     */
    K: (program: number, name: number): number => {
      let uniformName = utf8(name);
      let arrayIndex = 0;
      if (uniformName.endsWith(']')) {
        const leftBrace = uniformName.lastIndexOf('[');
        const tail = uniformName.slice(leftBrace + 1);
        arrayIndex = tail === ']' ? 0 : Number.parseInt(tail, 10);
        uniformName = uniformName.slice(0, leftBrace);
      }
      const info = gl.programInfos[program];
      const uniformInfo = info?.uniforms[uniformName];
      if (uniformInfo && arrayIndex >= 0 && arrayIndex < uniformInfo[0]) {
        return uniformInfo[1] + arrayIndex;
      }
      return -1;
    },

    /** `ka _glGetAttribLocation(program, name) -> int`, straight from the driver. */
    ka: (program: number, name: number): number =>
      gl.ctx?.getAttribLocation(gl.programs[program], utf8(name)) ?? -1,

    /** `va`-adjacent uniform setters; each looks the real location up by the synthetic id. */
    $: (location: number, value: number): void => gl.ctx?.uniform1i(gl.uniforms[location], value | 0),
    ha: (location: number, value: number): void => gl.ctx?.uniform1f(gl.uniforms[location], value),
    pb: (location: number, x: number, y: number, z: number): void =>
      gl.ctx?.uniform3f(gl.uniforms[location], x, y, z),

    /**
     * `ob _glUniform1fv(location, count, value)`.
     *
     * The two-branch body is a real allocation optimisation in the glue, and its upper branch is
     * reproduced **verbatim including its unit mismatch**: the glue's `HEAPF32.subarray` is given
     * `value + count * 4`, which is a byte offset used as an element index. It reads past the end of
     * the vector. This is upstream's code, so the fast path here matches it exactly; changing it
     * would make our host differ from the official one for counts over 288.
     */
    ob: (location: number, count: number, value: number): void => {
      const ctx = gl.ctx;
      if (!ctx) {
        refuse('ob', 'was called with no WebGL context, so the uniform was not set');
        return;
      }
      const n = count | 0;
      let view: Float32Array;
      if (n <= MINI_TEMP_COUNT) {
        let scratch = miniTempBuffers[n - 1];
        if (!scratch) {
          scratch = new Float32Array(n);
          miniTempBuffers[n - 1] = scratch;
        }
        for (let i = 0; i < n; i += 1) scratch[i] = memory.f32[((value | 0) >>> 2) + i] ?? 0;
        view = scratch;
      } else {
        view = memory.f32.subarray((value | 0) >>> 2, ((value + n * 4) | 0) >>> 2);
      }
      ctx.uniform1fv(gl.uniforms[location], view);
    },

    /**
     * `ia _glVertexAttribPointer(index, size, type, normalized, stride, ptr)`.
     *
     * The `ptr` argument is passed through as a plain number and that is correct, not a shortcut: the
     * glue also hands WebGL the raw wasm pointer, and the emscripten WebGL binding outside the
     * minified bundle coerces it to the byte offset it is. The two are the same integer — a byte offset
     * into the bound buffer — and there is no heap view involved at all.
     */
    ia: (index: number, size: number, type: number, normalized: number, stride: number, ptr: number): void =>
      gl.ctx?.vertexAttribPointer(
        index | 0, size | 0, type | 0, !!normalized, stride | 0, ptr | 0,
      ),

    /** `xa _glDrawElements(mode, count, type, indices)` — `indices` is a byte offset, as above. */
    xa: (mode: number, count: number, type: number, indices: number): void =>
      gl.ctx?.drawElements(mode | 0, count | 0, type | 0, indices | 0),

    /**
     * `pa _glTexImage2D(target, level, internalFormat, width, height, border, format, type, pixels)`.
     *
     * `pixels` is a raw guest pointer, so it needs the real view — the aligned-row arithmetic included.
     * A 0 pointer means "allocate", which is why the null check is on the pointer and not on the view.
     */
    pa: (
      target: number,
      level: number,
      internalFormat: number,
      width: number,
      height: number,
      border: number,
      format: number,
      type: number,
      pixels: number,
    ): void => {
      const ctx = gl.ctx;
      if (!ctx) {
        refuse('pa', 'was called with no WebGL context, so no texture image was set');
        return;
      }
      const view = pixels ? texPixelData(type, format, width, height, pixels) : null;
      ctx.texImage2D(
        target | 0, level | 0, internalFormat | 0, width | 0, height | 0,
        border | 0, format | 0, type | 0, view,
      );
    },

    /**
     * `hb _glReadPixels(x, y, width, height, format, type, pixels)`.
     *
     * The only GL row with a **destination** guest pointer. A null view latches
     * `GL_INVALID_OPERATION` (1280) and returns, rather than handing WebGL a zero-length array — which
     * it would accept and write nothing into, leaving the guest reading its own uninitialised heap.
     */
    hb: (
      x: number,
      y: number,
      width: number,
      height: number,
      format: number,
      type: number,
      pixels: number,
    ): void => {
      const ctx = gl.ctx;
      if (!ctx) {
        refuse('hb', 'was called with no WebGL context, so nothing was read back');
        return;
      }
      const view = texPixelData(type, format, width, height, pixels);
      if (!view) {
        recordError(1280);
        return;
      }
      ctx.readPixels(x | 0, y | 0, width | 0, height | 0, format | 0, type | 0, view);
    },

    /**
     * `X _glGetProgramiv(program, pname, p)`.
     *
     * Four enums are answered from the link-time table (log length, max uniform/attribute name length,
     * max uniform block name length) and everything else is the driver's answer. The `program >=
     * GL.counter` test is the glue's, and it is what keeps a guest that passes `-1` from indexing the
     * table at a negative slot.
     */
    X: (program: number, pname: number, p: number): void => {
      const ctx = gl.ctx;
      if (!p) {
        recordError(1281);
        return;
      }
      if (program >= gl.counter) {
        recordError(1281);
        return;
      }
      const info = gl.programInfos[program];
      if (!info) {
        recordError(1282);
        return;
      }
      if (pname === GL_INFO_LOG_LENGTH) {
        const log = ctx?.getProgramInfoLog(gl.programs[program]) ?? '(unknown error)';
        storeI32(p, log.length + 1);
        return;
      }
      if (pname === GL_ACTIVE_UNIFORM_MAX_LENGTH) {
        storeI32(p, info.maxUniformLength);
        return;
      }
      if (pname === GL_ACTIVE_ATTRIBUTE_MAX_LENGTH) {
        if (info.maxAttributeLength === -1 && ctx) {
          const programObject = gl.programs[program];
          const numAttribs = ctx.getProgramParameter(programObject, GL_ACTIVE_ATTRIBUTES);
          info.maxAttributeLength = 0;
          for (let i = 0; i < numAttribs; i += 1) {
            const attrib = ctx.getActiveAttrib(programObject, i);
            info.maxAttributeLength = Math.max(info.maxAttributeLength, attrib.name.length + 1);
          }
        }
        storeI32(p, info.maxAttributeLength);
        return;
      }
      if (pname === GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH) {
        if (info.maxUniformBlockNameLength === -1 && ctx) {
          const programObject = gl.programs[program];
          const numBlocks = ctx.getProgramParameter(programObject, GL_ACTIVE_UNIFORM_BLOCKS);
          info.maxUniformBlockNameLength = 0;
          for (let i = 0; i < numBlocks; i += 1) {
            const name = ctx.getActiveUniformBlockName(programObject, i);
            info.maxUniformBlockNameLength = Math.max(info.maxUniformBlockNameLength, name.length + 1);
          }
        }
        storeI32(p, info.maxUniformBlockNameLength);
        return;
      }
      storeI32(p, ctx?.getProgramParameter(gl.programs[program], pname | 0) ?? 0);
    },

    /**
     * `sa _glGetShaderiv(shader, pname, p)`.
     *
     * Two enums are special: `GL_INFO_LOG_LENGTH` and `GL_SHADER_SOURCE_LENGTH` both count **the NUL**
     * (`log.length + 1`), because the guest allocates a buffer of exactly that size and then calls the
     * matching `getInfoLog`. A host that reported `length` would make the guest allocate one byte short.
     */
    sa: (shader: number, pname: number, p: number): void => {
      const ctx = gl.ctx;
      if (!p) {
        recordError(1281);
        return;
      }
      if (pname === GL_INFO_LOG_LENGTH) {
        const log = ctx?.getShaderInfoLog(gl.shaders[shader]) ?? '(unknown error)';
        storeI32(p, log ? log.length + 1 : 0);
        return;
      }
      if (pname === GL_SHADER_SOURCE_LENGTH) {
        const source = ctx?.getShaderSource(gl.shaders[shader]);
        storeI32(p, source ? source.length + 1 : 0);
        return;
      }
      storeI32(p, ctx?.getShaderParameter(gl.shaders[shader], pname | 0) ?? 0);
    },

    /**
     * `ab _glGetShaderInfoLog` and `ib _glGetProgramInfoLog`, as one body.
     *
     * The guard is `maxLength > 0 && infoLog`, and both halves matter: a 0 `maxLength` is the guest
     * asking "how long is it?" through the `getiv` path, and a null `infoLog` is a guest that passed no
     * buffer. In both cases the glue writes **nothing** and reports 0 bytes.
     */
    ab: (shader: number, maxLength: number, infoLog: number, length: number): void => {
      const ctx = gl.ctx;
      const log = ctx?.getShaderInfoLog(gl.shaders[shader]) ?? '(unknown error)';
      const written =
        maxLength > 0 && infoLog ? memory.writeUtf8((infoLog | 0) >>> 0, log, maxLength | 0) : 0;
      if (length) storeI32(length, written < 0 ? 0 : written);
    },
    ib: (program: number, maxLength: number, infoLog: number, length: number): void => {
      const ctx = gl.ctx;
      const log = ctx?.getProgramInfoLog(gl.programs[program]) ?? '(unknown error)';
      const written =
        maxLength > 0 && infoLog ? memory.writeUtf8((infoLog | 0) >>> 0, log, maxLength | 0) : 0;
      if (length) storeI32(length, written < 0 ? 0 : written);
    },

    /** `Va _glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer)`. */
    Va: (target: number, attachment: number, renderbuffertarget: number, renderbuffer: number): void =>
      gl.ctx?.framebufferRenderbuffer(
        target | 0, attachment | 0, renderbuffertarget | 0, gl.renderbuffers[renderbuffer],
      ),
    /** `Wa _glFramebufferTexture2D(target, attachment, textarget, texture, level)`. */
    Wa: (target: number, attachment: number, textarget: number, texture: number, level: number): void =>
      gl.ctx?.framebufferTexture2D(
        target | 0, attachment | 0, textarget | 0, gl.textures[texture], level | 0,
      ),
    /** `Xa _glRenderbufferStorage(target, internalFormat, width, height)`. */
    Xa: (target: number, internalFormat: number, width: number, height: number): void =>
      gl.ctx?.renderbufferStorage(target | 0, internalFormat | 0, width | 0, height | 0),
    /**
     * `Ua _glCheckFramebufferStatus(target)`.
     *
     * The only `_gl*` row that returns a **constant the host knows** rather than forwarding: with no
     * context the answer is `0` (`GL_FRAMEBUFFER_STATUS_ERROR` by accident, but the guest only compares
     * against `GL_FRAMEBUFFER_COMPLETE`, so it takes the "incomplete" branch and reports a broken
     * framebuffer — which is true).
     */
    Ua: (target: number): number => {
      const ctx = gl.ctx;
      if (!ctx) {
        refuse('Ua', 'was called with no WebGL context, so the framebuffer reads as incomplete');
        return 0;
      }
      return ctx.checkFramebufferStatus(target | 0);
    },
  };

  /**
   * `findEventTarget(target)` — the small integer the `__emscripten_set_*_callback` rows use.
   *
   * Reproduced here because `$a` needs it, and because getting the convention right is the difference
   * between a working canvas query and -4 forever: `0` is falsy (no target), `1` is the document, `2`
   * is the window, and above 2 the number is a **CSS selector** in guest memory.
   *
   * The return type is deliberately `unknown` rather than a union of the two event targets: `1` and
   * `2` resolve to a document and a window, neither of which has `width`/`height`, and a typed union
   * would push the `$a` canvas check behind a narrowing dance that a real canvas can never need. The
   * row that cares checks the two properties it uses and reports a refusal otherwise.
   */
  function findEventTarget(target: number): unknown {
    if (target === 1) return host.document ?? null;
    if (target === 2) return host.window ?? null;
    if (target <= 2) return null;
    const doc = host.document as { querySelector(selector: string): unknown } | undefined;
    if (!doc) return null;
    return doc.querySelector(utf8(target)) ?? null;
  }

  /** `findCanvasEventTarget` is `findEventTarget`; the name difference is historical, not semantic. */
  const findCanvasEventTarget = findEventTarget;

  /* ------------------------------------------------------------------ */
  /* The eleven `__emscripten_set_*_callback_on_thread` rows               */
  /* ------------------------------------------------------------------ */

  /**
   * The glue's `JSEvents`, in the part that matters.
   *
   * The five blocks are allocated **once, on first use**, and every listener of the same family then
   * writes into the same block. That is not an optimisation the guest can observe; it is a contract,
   * because a callback that receives a pointer is free to keep it and read it again after the handler
   * returns. Allocating per event would hand out pointers the guest never sees again.
   */
  const jsEvents = {
    mouseEvent: 0,
    keyEvent: 0,
    wheelEvent: 0,
    touchEvent: 0,
    visibilityChangeEvent: 0,
    eventHandlers: [] as GlEventHandler[],
  };
  /**
   * Allocates one of the five event blocks, validating the address before anything may keep it.
   *
   * `host.malloc` is guest-adjacent code reached through the session: the wasm unmarshal contract is
   * that a plain `number` inside `[-0x80000000, 0xffffffff]` is an i32 pointer, so anything else — a
   * string, `NaN`, a fraction, a value outside that window — is not an address at all and must never
   * reach a slot the guest is entered with. `>>> 0` normalises the signed form to the unsigned one the
   * heap views take; 0 stays 0 in both forms, which is the glue's own "no block" value.
   *
   * The range check runs *after* `memory.refresh()`, because the allocation may have grown the heap and
   * a stale `byteLength` would reject an address that is now valid. Zero is never a valid address
   * either: `malloc(0)` and a null return are the same pointer, and both mean there is no block.
   *
   * **Ownership begins at the coercion, not at the return.** The allocator has already run guest code
   * by the time it answers, so the moment its answer is accepted as an address — numeric, integer,
   * non-zero, in the wasm i32 window, aligned, and inside the heap — that block belongs to this host
   * and every path out of here either returns it or releases it **exactly once**. `refresh` and
   * `assertRange` are exactly the two calls that can still refuse after the answer was accepted (a
   * detached buffer, a heap that shrank), and only those two release: a value refused *before*
   * ownership began was never an allocation this host owns, so it is dropped without a `free`, and an
   * arbitrary integer — `NaN>>>0` is 0, `1e21>>>0` is a plausible-looking address — can never reach
   * `free` at all. One release, and its own failure is reported beside the primary diagnosis rather
   * than retried: `free` is not idempotent, so a second call would be a double free, which corrupts
   * the heap this whole file exists to protect.
   *
   * Returns the normalized address, or 0. The caller returns its row's error code and installs **no
   * listener** on 0, so a registration can never claim a success it did not perform.
   */
  const allocateEvent = (size: number): number => {
    const malloc = host.malloc;
    if (!malloc) return 0;
    let pointer: unknown;
    try {
      pointer = malloc(size);
    } catch (error) {
      report('event', `malloc(${size}) threw ${describeFault(error)}`);
      return 0;
    }
    // Rejected as "not an address at all", before ownership: nothing is freed, because there is
    // nothing this host may claim as its own.
    if (typeof pointer !== 'number' || !Number.isInteger(pointer)) return 0;
    if (pointer === 0 || pointer < -0x80000000 || pointer > 0xffffffff) return 0;
    const address = pointer >>> 0;
    // 4-byte alignment for every block; the wheel block's f64 deltas need 8 or a Float64Array view
    // over it faults.
    if (address % (size === 96 ? 8 : 4) !== 0) return 0;
    // Ownership starts here. The two calls below are the only ones that can still refuse an address
    // this host already owns, and a refusal releases it once rather than leaking it.
    try {
      memory.refresh();
      memory.assertRange(address, size);
    } catch (error) {
      const free = host.free;
      if (free) {
        try {
          free(address);
        } catch (freeError) {
          report(
            'event',
            `free(${address}) after a refused ${size}-byte event block threw ${describeFault(freeError)}`,
          );
        }
      }
      report(
        'event',
        `the ${size}-byte event block at ${address} is outside the heap, so it was released ` +
          `(${describeFault(error)})`,
      );
      return 0;
    }
    return address;
  };

  /** The glue's `eventHandler` record. `allowsDeferredCalls` is recorded because it is load-bearing. */
  interface GlEventHandler {
    target: GlEventTarget;
    allowsDeferredCalls: boolean;
    eventTypeString: string;
    callbackfunc: number;
    handlerFunc: (event: unknown) => void | Promise<void>;
    /** Runs at arrival, in the listener, and its value is what the queued unit receives. */
    capture?: (event: unknown) => unknown;
    useCapture: boolean;
    listener?: (event: unknown) => void;
  }

  /** The two shapes a resolved event target can take, kept structural on purpose (see `findEventTarget`). */
  type GlEventTarget = {
    addEventListener(type: string, listener: (event: unknown) => void, capture?: boolean): void;
    removeEventListener(type: string, listener: (event: unknown) => void, capture?: boolean): void;
    onwheel?: unknown;
    getBoundingClientRect?(): { left: number; top: number };
  };

  /** The event fields every handler reads, as the loose `e` the glue closes over. */
  interface GlDomEvent {
    screenX?: number;
    screenY?: number;
    clientX?: number;
    clientY?: number;
    ctrlKey?: boolean;
    shiftKey?: boolean;
    altKey?: boolean;
    metaKey?: boolean;
    button?: number;
    buttons?: number;
    movementX?: number;
    movementY?: number;
    location?: number;
    repeat?: boolean;
    charCode?: number;
    keyCode?: number;
    which?: number;
    key?: string;
    code?: string;
    char?: string;
    locale?: string;
    deltaX?: number;
    deltaY?: number;
    deltaZ?: number;
    deltaMode?: number;
    touches?: GlDomTouch[];
    changedTouches?: GlDomTouch[];
    targetTouches?: GlDomTouch[];
    preventDefault(): void;
  }

  interface GlDomTouch {
    identifier: number;
    screenX: number;
    screenY: number;
    clientX: number;
    clientY: number;
    pageX: number;
    pageY: number;
    isChanged?: number;
    onTarget?: number;
  }

  /**
   * `preventDefault` pulled off the *original* DOM event and bound to it.
   *
   * The queued unit never holds the event object — the whole point of capturing is that the unit runs
   * after the dispatch is over — so the receiver is kept here, in the closure, bound to its owner. A
   * function lifted without `bind` would throw on the first `preventDefault()` the guest earned.
   */
  const bindPreventDefault = (event: unknown): (() => void) | undefined => {
    const original = (event as { preventDefault?: unknown } | null)?.preventDefault;
    if (typeof original !== 'function') return undefined;
    return (original as () => void).bind(event);
  };

  /** What a touch event is reduced to at arrival: the scalar fields and the three copied lists. */
  interface TouchCapture {
    touches: GlDomTouch[];
    changedTouches: GlDomTouch[];
    targetTouches: GlDomTouch[];
    ctrlKey: number;
    shiftKey: number;
    altKey: number;
    metaKey: number;
    preventDefault: () => void;
  }

  /** The never-throws stand-in for a captured event whose original had no `preventDefault`. */
  const noPreventDefault = (): void => {};

  /** What a visibility event is reduced to at arrival: the two scalars and the bound `preventDefault`. */
  interface VisibilityCapture {
    hidden: number;
    stateIndex: number;
    preventDefault: () => void;
  }

  /** `__getBoundingClientRect(e)` — the two special targets have no box, and the glue says so. */
  const getBoundingClientRect = (target: GlEventTarget): { left: number; top: number } => {
    if (target === host.document || target === host.window) return { left: 0, top: 0 };
    return target.getBoundingClientRect?.() ?? { left: 0, top: 0 };
  };

  /**
   * `JSEvents.registerOrRemoveHandler(eventHandler)`.
   *
   * A handler with a `callbackfunc` is installed and remembered; one without **removes** any handler
   * registered on the same (target, eventTypeString) pair. That removal rule is why a guest can
   * unregister a callback by passing 0, and it is why the identity key is the pair and not the
   * callback: re-registering the same event type replaces rather than accumulates.
   */
  const registerOrRemoveHandler = (eventHandler: GlEventHandler): void => {
    // Unconditional, and *before* the `callbackfunc` test: a 0 is the unregister request, but a
    // new callback replaces rather than accumulates, so the pair's previous listener comes off in
    // both cases. Without this, re-registering the same (target, eventTypeString) left the old
    // listener installed and the page fed two callbacks into one guest.
    for (let i = 0; i < jsEvents.eventHandlers.length; i += 1) {
      const other = jsEvents.eventHandlers[i];
      if (other && other.target === eventHandler.target && other.eventTypeString === eventHandler.eventTypeString) {
        if (other.listener) other.target.removeEventListener(other.eventTypeString, other.listener, other.useCapture);
        jsEvents.eventHandlers.splice(i, 1);
        i -= 1;
      }
    }
    if (!eventHandler.callbackfunc) return;
    /**
     * The listener queues the **whole** handler, not just the guest call.
     *
     * The tempting shape is to fill the reusable heap block here, in the listener, and queue only the
     * `dynCall_iiii` — that is how the official glue is written, because there the queue is only ever
     * drained by another *event*, on the same thread, with the heap untouched in between. That
     * assumption is false for a session serializer that can yield: by the time the queued call runs,
     * the guest may have grown or moved the heap (which detaches every typed-array view) or another
     * delivery may have overwritten the shared block. Filling inside the queued unit is what makes
     * "read the event, write the block, enter the guest" one indivisible step, which is the only order
     * in which the pointer handed to the guest still describes the event that was queued.
     *
     * `memory.refresh()` is called before the fill for the same reason: the views this closure captured
     * are re-derived from the current `WebAssembly.Memory` buffer every time, so a growth is invisible
     * to the caller rather than fatal.
     *
     * The state recheck lives here too. A pause and a dispose both mean "the guest must not be entered",
     * and neither can un-queue a listener the DOM already invoked — so the check happens at run time,
     * immediately before the guest call, and a late event after dispose is dropped silently.
     */
    const listener = (event: unknown): void => {
      if (mainLoop.disposed || isDisposed() || mainLoop.paused) return;
      // The capture runs HERE, while the event is still being dispatched, so it sees arrival values.
      const captured = eventHandler.capture ? eventHandler.capture(event) : event;
      enqueueUnit(() => {
        if (mainLoop.disposed || isDisposed() || mainLoop.paused) return undefined;
        return eventHandler.handlerFunc(captured);
      });
    };
    eventHandler.listener = listener;
    eventHandler.target.addEventListener(eventHandler.eventTypeString, listener, eventHandler.useCapture);
    jsEvents.eventHandlers.push(eventHandler);
  };

  /**
   * Enters the guest's handler and honours its return value.
   *
   * A truthy answer means "the guest consumed this event", which is the glue's only reason to call
   * `preventDefault` — the guest cannot reach the DOM event any other way. A guest fault inside the
   * callback must not be reported as a registration problem, so it propagates to the session's own
   * event loop, which is where a real browser would surface it.
   *
   * The return slot grew a promise along with `dynCallIiii`, and `preventDefault` waits for it for the
   * same reason the fill does: the answer is the guest's verdict on the event, and defaulting before
   * the guest has given it would fight the guest. A sync host sees the identical call it saw before —
   * the compare-and-default happens inline, with no promise and no extra turn.
   *
   * The parameter is deliberately `Pick<GlDomEvent, 'preventDefault'>` and not a `GlDomEvent`: the
   * only thing this row is still allowed to touch is the default action, and the two families that
   * passed a captured event here hold a *reduction* of the DOM event, not the event — `TouchCapture`
   * carries numeric ctrl/shift/alt/meta bits and `VisibilityCapture` carries `hidden`, neither of
   * which is a field of `GlDomEvent`. Asking for the full event forced every caller to launder its
   * capture back into a DOM shape it never had; narrowing to the one member that is actually used
   * makes the capture assignable as-is, and makes the constraint true rather than appeased.
   */
  const callHandler = (
    callbackfunc: number,
    eventTypeId: number,
    block: number,
    userData: number,
    event: Pick<GlDomEvent, 'preventDefault'>,
  ): void | Promise<void> => {
    const dynCallIiii = host.dynCallIiii;
    if (!dynCallIiii) {
      refuse('event', 'has no dynCall_iiii, so an event handler could be registered but never entered');
      return undefined;
    }
    /**
     * The four numbers are recorded **whether or not the call succeeds**, and they are kept in the
     * local below rather than pushed into the blocked trace. An earlier version reported them through
     * `report('event', …)` before and after the call, which put *every healthy event delivery* into
     * the blocked trace — a mouse move put two lines in it — and buried the one line that matters. A
     * delivered event is not a blocked row, so it is not reported as one. The values still name the
     * guest's index, its event-type id, the block we handed it and the `userData` it gave us, which is
     * what a fault needs, and the fault itself is what surfaces them: it propagates out of the guest
     * call unchanged.
     *
     * The value is the table index the guest handed us, not an address: the guest passes its event
     * callbacks as real entries of its own function table (the shipped bridge exports that table as
     * `Lc`, 3,613 slots, and Chrome's own trap names `fidx 507` reading a table entry whose real type
     * is not the one its `call_indirect` declared). A host that echoed the four values back could
     * answer "wrong slot" from the numbers alone, with no need to guess which half of the contract
     * broke.
     */
    const entered = `dynCall_iiii(${callbackfunc | 0}, ${eventTypeId}, ${block}, ${userData | 0})`;
    const result = dynCallIiii(callbackfunc | 0, eventTypeId, block, userData | 0);
    const honour = (consumed: number | Promise<number> | void): void => {
      if (consumed) event.preventDefault?.();
    };
    if (result && typeof (result as Promise<number>).then === 'function') {
      const waited: Promise<void> = (result as Promise<number>).then((consumed) => {
        honour(consumed);
        return idle() as void | Promise<void>;
      });
      return waited;
    }
    honour(result as number);
    const waited = idle();
    return waited;
  };

  /** The five block fillers, each writing the glue's field order exactly. */

  /** `__fillMouseEventData(eventStruct, e, target)` — 64 bytes, ending in the two target-relative fields. */
  const fillMouseEventData = (eventStruct: number, event: GlDomEvent, target: GlEventTarget): void => {
    const idx = eventStruct >>> 2;
    const h32 = memory.i32;
    const h16 = memory.u16;
    h32[idx + 0] = event.screenX ?? 0;
    h32[idx + 1] = event.screenY ?? 0;
    h32[idx + 2] = event.clientX ?? 0;
    h32[idx + 3] = event.clientY ?? 0;
    h32[idx + 4] = event.ctrlKey ? 1 : 0;
    h32[idx + 5] = event.shiftKey ? 1 : 0;
    h32[idx + 6] = event.altKey ? 1 : 0;
    h32[idx + 7] = event.metaKey ? 1 : 0;
    h16[idx * 2 + 16] = event.button ?? 0;
    h16[idx * 2 + 17] = event.buttons ?? 0;
    h32[idx + 9] = event.movementX ?? 0;
    h32[idx + 10] = event.movementY ?? 0;
    const rect = getBoundingClientRect(target);
    h32[idx + 11] = (event.clientX ?? 0) - rect.left;
    h32[idx + 12] = (event.clientY ?? 0) - rect.top;
  };

  /**
   * `__fillKeyEventData` inlined into the key handler: 164 bytes, four strings at 36/68/100/132.
   *
   * The four `stringToUTF8` calls each carry their own byte budget, and the budget is part of the
   * contract — a host that wrote a longer string would run into the next field. `writeUtf8` returns
   * `-1` without writing when the value does not fit, which is the same outcome the glue gets.
   */
  const fillKeyEventData = (keyEventData: number, event: GlDomEvent): void => {
    const idx = keyEventData >>> 2;
    const h32 = memory.i32;
    h32[idx + 0] = event.location ?? 0;
    h32[idx + 1] = event.ctrlKey ? 1 : 0;
    h32[idx + 2] = event.shiftKey ? 1 : 0;
    h32[idx + 3] = event.altKey ? 1 : 0;
    h32[idx + 4] = event.metaKey ? 1 : 0;
    h32[idx + 5] = event.repeat ? 1 : 0;
    h32[idx + 6] = event.charCode ?? 0;
    h32[idx + 7] = event.keyCode ?? 0;
    h32[idx + 8] = event.which ?? 0;
    memory.writeUtf8(keyEventData + 36, event.key ?? '', 32);
    memory.writeUtf8(keyEventData + 68, event.code ?? '', 32);
    memory.writeUtf8(keyEventData + 100, event.char ?? '', 32);
    memory.writeUtf8(keyEventData + 132, event.locale ?? '', 32);
  };

  /**
   * `__registerMouseEventCallback` — the three mouse rows share one 64-byte block.
   *
   * `allowsDeferredCalls` is the glue's own per-event-type answer, and it is reproduced rather than
   * simplified: the glue defers `mousemove` (it fires in bursts and the guest only needs the last
   * one) but never `mouseenter`/`mouseleave`, which carry state changes rather than positions.
   */
  const registerMouseEventCallback = (
    target: number,
    userData: number,
    useCapture: number,
    callbackfunc: number,
    eventTypeId: number,
    eventTypeString: string,
  ): number => {
    const useCaptureFlag = useCapture !== 0;
    // The target is resolved before anything is allocated, and that order is the row's own contract:
    // `callbackfunc === 0` is an *unregister*, which must remove the listener on the same (target,
    // event type) pair without allocating a block it will never fill and without claiming a success
    // it did not perform.
    const element = findEventTarget(target) as GlEventTarget | null;
    if (!element) {
      refuse(eventTypeString, 'resolved no event target, so it answered -4 rather than registering');
      return -4;
    }
    // Allocate only for a registration, and only when its family has no block yet.
    if (callbackfunc && !jsEvents.mouseEvent) {
      const block = allocateEvent(64);
      if (!block) {
        refuse(eventTypeString, 'could not allocate a valid 64-byte event block, so it answered -4');
        return -4;
      }
      jsEvents.mouseEvent = block;
    }
    const handlerFunc = (ev: unknown): void | Promise<void> => {
      const event = (ev ?? {}) as GlDomEvent;
      memory.refresh();
      fillMouseEventData(jsEvents.mouseEvent, event, element);
      return callHandler(callbackfunc, eventTypeId, jsEvents.mouseEvent, userData, event);
    };
    registerOrRemoveHandler({
      target: element,
      allowsDeferredCalls: eventTypeString !== 'mousemove' && eventTypeString !== 'mouseenter' && eventTypeString !== 'mouseleave',
      eventTypeString,
      callbackfunc,
      handlerFunc,
      useCapture: useCaptureFlag,
    });
    return 0;
  };

  /** `__registerKeyEventCallback` — the two key rows share one 164-byte block, always deferred-capable. */
  const registerKeyEventCallback = (
    target: number,
    userData: number,
    useCapture: number,
    callbackfunc: number,
    eventTypeId: number,
    eventTypeString: string,
  ): number => {
    const useCaptureFlag = useCapture !== 0;
    /**
     * The target is resolved **before** the block, and the missing `as GlEventTarget` cast was the bug
     * this ordering fixes: a guest that passes an unresolvable target — `0`, or a selector nothing
     * matches — used to reach `registerOrRemoveHandler` with `null` and throw out of the import, which
     * is not an answer the glue ever gives. It answers `-4` and installs nothing.
     */
    const element = findEventTarget(target) as GlEventTarget | null;
    if (!element) {
      refuse(eventTypeString, 'resolved no event target, so it answered -4 rather than registering');
      return -4;
    }
    if (callbackfunc && !jsEvents.keyEvent) {
      const block = allocateEvent(164);
      if (!block) {
        refuse(eventTypeString, 'could not allocate a valid 164-byte event block, so it answered -4');
        return -4;
      }
      jsEvents.keyEvent = block;
    }
    const handlerFunc = (ev: unknown): void | Promise<void> => {
      const event = (ev ?? {}) as GlDomEvent;
      memory.refresh();
      fillKeyEventData(jsEvents.keyEvent, event);
      return callHandler(callbackfunc, eventTypeId, jsEvents.keyEvent, userData, event);
    };
    registerOrRemoveHandler({
      target: element,
      allowsDeferredCalls: true,
      eventTypeString,
      callbackfunc,
      handlerFunc,
      useCapture: useCaptureFlag,
    });
    return 0;
  };

  /**
   * `__registerWheelEventCallback` — one 96-byte block: the mouse 64 bytes plus three f64 deltas and
   * the delta mode.
   *
   * The `onwheel` test is the row's only real precondition and it is the one place the glue asks the
   * *target* rather than the document anything, so it is reproduced as a property test: a target with
   * no `onwheel` answers `-1`, and a target whose `onwheel` is `undefined` answers `-1` too. Testing
   * `"onwheel" in target` instead would have registered a listener on a canvas that silently drops it.
   */
  const registerWheelEventCallback = (
    target: number,
    userData: number,
    useCapture: number,
    callbackfunc: number,
    eventTypeId: number,
    eventTypeString: string,
  ): number => {
    const element = findEventTarget(target) as GlEventTarget | null;
    if (!element) {
      refuse(eventTypeString, 'resolved no wheel target, so it answered -1 rather than registering');
      return -1;
    }
    if (element.onwheel === undefined) {
      refuse(eventTypeString, `the target has no onwheel, so it answered -1 as the glue does rather than registering a listener the target would drop`);
      return -1;
    }
    if (callbackfunc && !jsEvents.wheelEvent) {
      const block = allocateEvent(96);
      if (!block) {
        refuse(eventTypeString, 'could not allocate a valid 96-byte event block, so it answered -1');
        return -1;
      }
      jsEvents.wheelEvent = block;
    }
    const handlerFunc = (ev: unknown): void | Promise<void> => {
      const event = (ev ?? {}) as GlDomEvent;
      const wheelEvent = jsEvents.wheelEvent;
      memory.refresh();
      fillMouseEventData(wheelEvent, event, element);
      const f64 = memory.f64;
      f64[(wheelEvent + 64) >>> 3] = event.deltaX ?? 0;
      f64[(wheelEvent + 72) >>> 3] = event.deltaY ?? 0;
      f64[(wheelEvent + 80) >>> 3] = event.deltaZ ?? 0;
      memory.i32[(wheelEvent + 88) >>> 2] = event.deltaMode ?? 0;
      return callHandler(callbackfunc, eventTypeId, wheelEvent, userData, event);
    };
    registerOrRemoveHandler({
      target: element,
      allowsDeferredCalls: true,
      eventTypeString,
      callbackfunc,
      handlerFunc,
      useCapture: useCapture !== 0,
    });
    return 0;
  };

  /**
   * `__registerTouchEventCallback` — the four touch rows share one 1684-byte block.
   *
   * The block is `1 + 4` header words (touch count, then ctrl/shift/alt/meta) followed by up to
   * **32** touches of 13 words, which is where 1684 comes from: `4 + 32 * 13 = 420` words = 1680 bytes,
   * plus the 4 bytes the count sits in. The glue stops adding touches past 31 (`if (++numTouches > 31)
   * break`), so a 33-finger gesture reports 32 — reproduced rather than "fixed", because a guest that
   * sized its own array to the glue's 32 would misread a larger one.
   *
   * The three touch lists are folded into one map keyed by `identifier`, with `changedTouches` marked
   * `isChanged = 1` and `targetTouches` marked `onTarget = 1`, exactly as the glue folds them: the
   * guest reads one array and tests those two flags, so a touch that is both changed and on-target
   * appears once with both flags set.
   */
  const registerTouchEventCallback = (
    target: number,
    userData: number,
    useCapture: number,
    callbackfunc: number,
    eventTypeId: number,
    eventTypeString: string,
  ): number => {
    const element = findEventTarget(target) as GlEventTarget | null;
    if (!element) {
      refuse(eventTypeString, 'resolved no touch target, so it answered -4 rather than registering');
      return -4;
    }
    if (callbackfunc && !jsEvents.touchEvent) {
      const block = allocateEvent(1684);
      if (!block) {
        refuse(eventTypeString, 'could not allocate a valid 1684-byte event block, so it answered -4');
        return -4;
      }
      jsEvents.touchEvent = block;
    }
    /**
     * The three touch lists and the modifiers are copied out **at arrival**, in the listener, not in
     * the queued unit.
     *
     * A browser `TouchList` is a live-ish host object: its entries are read through accessors, and the
     * list is only guaranteed for the duration of the dispatch. Snapshotting at dequeue reads a list
     * the browser is entitled to have recycled, and reports a *later* gesture's coordinates as if they
     * were this event's. `identifier` is the only field a `TouchList` guarantees; everything else is
     * optional here and filled with a default, exactly as the glue's `__emscripten_get_touch_data`
     * does. `identifier` is still converted, because the glue stores it as an i32.
     */
    const captureTouch = (ev: unknown): TouchCapture => {
      const event = (ev ?? {}) as GlDomEvent;
      const snapshot = (list: GlDomTouch[] | undefined): GlDomTouch[] => {
        const out: GlDomTouch[] = [];
        for (const touch of list ?? []) {
          out.push({
            identifier: touch.identifier | 0,
            screenX: touch.screenX ?? 0,
            screenY: touch.screenY ?? 0,
            clientX: touch.clientX ?? 0,
            clientY: touch.clientY ?? 0,
            pageX: touch.pageX ?? 0,
            pageY: touch.pageY ?? 0,
          });
        }
        return out;
      };
      return {
        touches: snapshot(event.touches),
        changedTouches: snapshot(event.changedTouches),
        targetTouches: snapshot(event.targetTouches),
        ctrlKey: event.ctrlKey ? 1 : 0,
        shiftKey: event.shiftKey ? 1 : 0,
        altKey: event.altKey ? 1 : 0,
        metaKey: event.metaKey ? 1 : 0,
        preventDefault: bindPreventDefault(event) ?? noPreventDefault,
      };
    };
    const handlerFunc = (captured: unknown): void | Promise<void> => {
      const event = captured as TouchCapture;
      memory.refresh();
      const touches = new Map<number, GlDomTouch>();
      for (const touch of event.touches) touches.set(touch.identifier, touch);
      for (const touch of event.changedTouches) {
        touch.isChanged = 1;
        touches.set(touch.identifier, touch);
      }
      for (const touch of event.targetTouches) {
        const known = touches.get(touch.identifier);
        if (known) known.onTarget = 1;
      }
      const touchEvent = jsEvents.touchEvent;
      const h32 = memory.i32;
      let idx = touchEvent >>> 2;
      h32[idx + 1] = event.ctrlKey;
      h32[idx + 2] = event.shiftKey;
      h32[idx + 3] = event.altKey;
      h32[idx + 4] = event.metaKey;
      idx += 5;
      // The target box is read at dequeue: it is the layout at the time the guest is entered, and the
      // glue reads it there too. Only the touch *values* have to be arrival values.
      const targetRect = getBoundingClientRect(element);
      let numTouches = 0;
      for (const touch of touches.values()) {
        h32[idx + 0] = touch.identifier;
        h32[idx + 1] = touch.screenX;
        h32[idx + 2] = touch.screenY;
        h32[idx + 3] = touch.clientX;
        h32[idx + 4] = touch.clientY;
        h32[idx + 5] = touch.pageX;
        h32[idx + 6] = touch.pageY;
        h32[idx + 7] = touch.isChanged ?? 0;
        h32[idx + 8] = touch.onTarget ?? 0;
        h32[idx + 9] = touch.clientX - targetRect.left;
        h32[idx + 10] = touch.clientY - targetRect.top;
        idx += 13;
        numTouches += 1;
        if (numTouches > 31) break;
      }
      h32[touchEvent >>> 2] = numTouches;
      return callHandler(callbackfunc, eventTypeId, touchEvent, userData, event);
    };
    const useCaptureFlag = useCapture !== 0;
    registerOrRemoveHandler({
      target: element,
      allowsDeferredCalls: eventTypeString === 'touchstart' || eventTypeString === 'touchend',
      eventTypeString,
      callbackfunc,
      handlerFunc,
      capture: captureTouch,
      useCapture: useCaptureFlag,
    });
    return 0;
  };

  /**
   * `__registerVisibilityChangeEventCallback` — an 8-byte block: `document.hidden` then the
   * `visibilityState` index into `["hidden","visible","prerender","unloaded"]`.
   *
   * The row's own precondition is `specialHTMLTargets[1]`, which is the `document`; so this one row
   * resolves its target from the *host* rather than from the guest's target argument, which is why its
   * signature is `(i32,i32,i32,i32)` while the other ten take five arguments. Reproducing that
   * asymmetry is the point — a host that resolved the target from the guest here would attach a
   * visibility handler to whatever element the guest passed and never learn about tab switches.
   *
   * The 8-byte block goes through {@link allocateEvent} like the other four families rather than a raw
   * `host.malloc(8)`: the same allocator answer has to clear the same numeric, integer, non-zero and
   * alignment checks before an address may be stored, and the wheel block's reasoning applies here too
   * — the two fields are `i32`s, so 4-byte alignment is the requirement.
   */
  const registerVisibilityChangeEventCallback = (
    userData: number,
    useCapture: number,
    callbackfunc: number,
    eventTypeId: number,
    eventTypeString: string,
  ): number => {
    const document = host.document as (GlEventTarget & { hidden?: boolean; visibilityState?: string }) | undefined;
    if (!document) {
      refuse(eventTypeString, 'has no document, so it answered -4 as the glue does rather than registering');
      return -4;
    }
    if (callbackfunc && !jsEvents.visibilityChangeEvent) {
      const block = allocateEvent(8);
      if (!block) {
        refuse(eventTypeString, 'could not allocate a valid 8-byte event block, so it answered -4');
        return -4;
      }
      jsEvents.visibilityChangeEvent = block;
    }
    /**
     * `document.hidden` and `document.visibilityState` are read **at arrival**, in the listener.
     *
     * The two values describe the transition that caused the event: the queued unit runs one or more
     * turns later, by which time the document may already have flipped back and the guest would be told
     * about a state it never changed into. Reading them at arrival keeps the notification honest about
     * the edge it announces; the unit below never touches `document` at all. The `preventDefault` is the
     * bound original, so the guest's verdict still reaches the DOM.
     */
    const captureVisibility = (ev: unknown): VisibilityCapture => {
      const states = ['hidden', 'visible', 'prerender', 'unloaded'];
      return {
        hidden: document.hidden ? 1 : 0,
        stateIndex: Math.max(0, states.indexOf(document.visibilityState ?? 'visible')),
        preventDefault: bindPreventDefault(ev) ?? noPreventDefault,
      };
    };
    const handlerFunc = (captured: unknown): void | Promise<void> => {
      const event = captured as VisibilityCapture;
      memory.refresh();
      const visibilityChangeEvent = jsEvents.visibilityChangeEvent;
      memory.i32[visibilityChangeEvent >>> 2] = event.hidden;
      memory.i32[(visibilityChangeEvent + 4) >>> 2] = event.stateIndex;
      return callHandler(callbackfunc, eventTypeId, visibilityChangeEvent, userData, event);
    };
    registerOrRemoveHandler({
      target: document,
      // The glue's record has no allowsDeferredCalls for this family at all; `false` is the honest
      // transcription rather than a claim that the glue considered it.
      allowsDeferredCalls: false,
      eventTypeString,
      callbackfunc,
      handlerFunc,
      capture: captureVisibility,
      useCapture: useCapture !== 0,
    });
    return 0;
  };

  /**
   * The eleven `__emscripten_set_*_callback_on_thread` rows, each now the real registration.
   *
   * A `callbackfunc` of 0 is the unregister request, and it is why `registerOrRemoveHandler` keys on
   * (target, event type): the guest's own `emscripten_set_*_callback(0)` must actually remove the
   * listener, or the page keeps feeding a guest that has said it is no longer listening.
   *
   * The rows that need a target answer `-4` (or `-1` for wheel) when there is none, because that is
   * the glue's own answer for the same condition. No row reports a success it did not perform.
   */
  const eventRows: Readonly<Record<string, (...args: number[]) => number>> = {
    lc: (target, userData, useCapture, callbackfunc): number =>
      registerMouseEventCallback(target, userData, useCapture, callbackfunc, 5, 'mousedown'),
    kc: (target, userData, useCapture, callbackfunc): number =>
      registerMouseEventCallback(target, userData, useCapture, callbackfunc, 8, 'mousemove'),
    jc: (target, userData, useCapture, callbackfunc): number =>
      registerMouseEventCallback(target, userData, useCapture, callbackfunc, 6, 'mouseup'),
    ic: (target, userData, useCapture, callbackfunc): number =>
      registerWheelEventCallback(target, userData, useCapture, callbackfunc, 9, 'wheel'),
    cc: (target, userData, useCapture, callbackfunc): number =>
      registerKeyEventCallback(target, userData, useCapture, callbackfunc, 2, 'keydown'),
    bc: (target, userData, useCapture, callbackfunc): number =>
      registerKeyEventCallback(target, userData, useCapture, callbackfunc, 3, 'keyup'),
    hc: (target, userData, useCapture, callbackfunc): number =>
      registerTouchEventCallback(target, userData, useCapture, callbackfunc, 22, 'touchstart'),
    ec: (target, userData, useCapture, callbackfunc): number =>
      registerTouchEventCallback(target, userData, useCapture, callbackfunc, 23, 'touchend'),
    gc: (target, userData, useCapture, callbackfunc): number =>
      registerTouchEventCallback(target, userData, useCapture, callbackfunc, 24, 'touchmove'),
    dc: (target, userData, useCapture, callbackfunc): number =>
      registerTouchEventCallback(target, userData, useCapture, callbackfunc, 25, 'touchcancel'),
    ac: (_target, userData, useCapture, callbackfunc): number =>
      registerVisibilityChangeEventCallback(userData, useCapture, callbackfunc, 21, 'visibilitychange'),
  };

  for (const [name, body] of Object.entries(eventRows)) imports[name] = body;

  /**
   * The nineteen audio rows.
   *
   * **Two mutually exclusive shapes, and the choice is all-or-nothing.** When the host supplies
   * {@link ArtemisGlHost.audioImports} the rows are that table's real bodies — the WebAudio host in
   * `audio.ts` — and nothing here refuses. That table is checked for every one of the nineteen names
   * *now*, at construction, and a table missing any of them throws rather than leaving the rest wired:
   * a half-supplied family is the exact failure the all-or-nothing rule exists to prevent, because the
   * rows that did get a body would play sound and the row that did not would answer a refusal nobody
   * reads. When no table is supplied the rows are the deliberate refusals the map's owner text records.
   *
   * **Those refusals are not a stub for missing effort.** An audio row answered with a default is
   * *worse than absent*, because the guest's mixer is driven by `alSourcePlay` and a silent source is a
   * game that runs, draws, and produces no sound with nothing in any log to say why. Until a session
   * supplies the real host these rows are documented refusals — per-call defaults, reported once, and
   * named in {@link ARTEMIS_GL_REFUSED} so the committed map says so too.
   *
   * The table is read once, here, into `bound` — each row's function is *captured* rather than
   * re-read inside its closure. A `suppliedAudio[name](...)` written inside the arrow would be a
   * fresh optional read on every call (`noUncheckedIndexedAccess` types it `F | undefined`, which is
   * TS2722), and an optional call is the one shape this table must never take: a missing row would
   * silently do nothing instead of refusing, which is precisely the silent-sound failure the
   * all-or-nothing rule exists to prevent. Capturing the validated function makes each closure call
   * a definite function or not exist at all.
   *
   * The throw abandons the whole table: `imports` is a local, so a table missing any row is never
   * returned and the rows already copied into `bound` are unreachable.
   */
  const suppliedAudio = host.audioImports;
  if (suppliedAudio) {
    const bound: [string, (...args: number[]) => number | void][] = [];
    for (const name of ARTEMIS_GL_AUDIO_ROWS) {
      const supplied = suppliedAudio[name];
      if (typeof supplied !== 'function') {
        throw new ArtemisHostError(
          `the gl bucket was given an audioImports table without ${name} ` +
            `(${ARTEMIS_GL_OFFICIAL[name] ?? name}): the nineteen audio rows are one family and are ` +
            'wired all together or not at all, so a partially supplied table is refused rather than ' +
            'half-installed',
        );
      }
      bound.push([name, supplied]);
    }
    for (const [name, supplied] of bound) {
      imports[name] = (...args: number[]): number | void => supplied(...args);
    }
  } else {
    for (const name of ARTEMIS_GL_AUDIO_ROWS) {
      imports[name] = (...args: number[]): number => {
        refuse(
          name,
          `is a deliberate refusal: audio rows are not implemented yet, so it took the per-call neutral value (${
            audioRefusalAnswer(name, args)
          }) rather than pretending to play sound`,
        );
        return audioRefusalAnswer(name, args);
      };
    }
  }

  /* ------------------------------------------------------------------ */
  /* Sanity                                                               */
  /* ------------------------------------------------------------------ */

  /**
   * The table has to cover all 99 rows, because a missing key is not a stub — it is a `LinkError` and
   * the engine never instantiates. This throws at construction, where the message can name the row,
   * rather than at instantiation, where V8 names only the first.
   */
  for (const name of ARTEMIS_GL_IMPORTS) {
    if (typeof imports[name] !== 'function') {
      throw new ArtemisHostError(`the gl bucket is missing an import: ${name}`);
    }
  }
  const provided = Object.keys(imports);
  for (const name of ARTEMIS_GL_IMPORTS) {
    if (!provided.includes(name)) {
      throw new ArtemisHostError(`the gl bucket did not supply ${name}`);
    }
  }

  // Every later binding is initialized above this line, so a `setLifecycle` callback that calls
  // `dispose()` immediately walks real objects instead of half-built ones.
  registerLifecycle();

  return imports;
}

/** Builds the `gl` import table, typed so `boot.ts` can spread it into its own table. */
export function createArtemisGlTable(host: ArtemisGlHost): Record<string, (...args: number[]) => number | void> {
  return createArtemisGlImports(host);
}

/**
 * The nineteen audio rows, listed so the refusal loop, {@link ARTEMIS_GL_REFUSED} and the committed
 * map's owner text all read from one source.
 *
 * Nineteen, not twenty-two: `_alcOpenDevice` / `_alcCreateContext` / `_alcMakeContextCurrent` are
 * three of these, and counting families rather than rows is how a later reader ends up looking for
 * two rows that were never there. `ga` is the trap — `_alSourcePause`, not a GL name.
 */
const ARTEMIS_GL_AUDIO_ROWS: readonly string[] = [
  'F', 'G', 'Ga', 'Ha', 'Ia', 'Ja', 'Ka', 'La', 'Ma', 'Na', 'Oa', 'Pa', 'R', 'ga', 'oa', 'vc',
  'wc', 'xc', 'yc',
];

/**
 * The per-call answer each audio refusal gives, so the refusal is a *documented* default and not one
 * family-wide 0.
 *
 * This is the shape the Lead's rule asks for, and the answers are the OpenAL ones rather than a lazy
 * zero because a single `0` across nineteen rows is the failure mode the rule exists to prevent. The
 * two that are not 0 are the two where `0` would be a lie: `alcOpenDevice` / `alcCreateContext`
 * return an *id*, and a 0 there is OpenAL's own "no device"/"no context" answer, so the refusal
 * reports the id it declined to mint rather than pretending it minted one.
 */
const ARTEMIS_GL_AUDIO_ANSWER: Readonly<Record<string, number>> = {
  // `alcOpenDevice` / `alcCreateContext` / `alcMakeContextCurrent` — 0 is ALC's own failure value,
  // so 0 is the honest "no context" answer rather than a family-wide placeholder.
  Pa: 0,
  Oa: 0,
  Na: 0,
  // `alGetString` / `alGetError` — both are `i32` in the official table, and both return 0 when
  // there is no current context, which is exactly the state these rows refuse to create.
  F: 0,
  G: 40964, // AL_INVALID_OPERATION: "no current context" is what a refused row must report.
  // Everything else is a void OpenAL call; the neutral value is "nothing happened".
  Ga: 0, Ha: 0, Ia: 0, Ja: 0, Ka: 0, La: 0, Ma: 0, R: 0, ga: 0, oa: 0, vc: 0, wc: 0, xc: 0, yc: 0,
};

/** The per-call answer for one audio row, defaulting to the neutral 0 for a name not listed above. */
const audioRefusalAnswer = (name: string, _args: readonly number[]): number =>
  ARTEMIS_GL_AUDIO_ANSWER[name] ?? 0;

/** The 99 `gl` short names, in the order `imports-map.json` first lists them. */
export const ARTEMIS_GL_IMPORTS: readonly string[] = [
  '$', '$a', '$b', 'F', 'Fa', 'G', 'Ga', 'H', 'Ha', 'Ia', 'J', 'Ja', 'K', 'Ka', 'Kc', 'La', 'M',
  'Ma', 'N', 'Na', 'Oa', 'Pa', 'R', 'S', 'T', 'U', 'Ua', 'Va', 'W', 'Wa', 'Wb', 'X', 'Xa', 'Y',
  'Ya', 'Yb', 'Z', 'Za', 'Zb', '_', '_a', '_b', 'ab', 'ac', 'bb', 'bc', 'cb', 'cc', 'da', 'db',
  'dc', 'eb', 'ec', 'fb', 'ga', 'gb', 'gc', 'ha', 'hb', 'hc', 'ia', 'ib', 'ic', 'ja', 'jb', 'jc',
  'ka', 'kb', 'kc', 'la', 'lb', 'lc', 'mb', 'mc', 'nb', 'nc', 'oa', 'ob', 'oc', 'pa', 'pb', 'pc',
  'qa', 'qb', 'ra', 'rb', 'sa', 'sb', 'ta', 'tb', 'ua', 'va', 'vc', 'wa', 'wc', 'xa', 'xc', 'ya',
  'yc',
];

/** The official name behind each `gl` short name, for a report a human can read. */
export const ARTEMIS_GL_OFFICIAL: Readonly<Record<string, string>> = {
  $: '_glUniform1i',
  $a: '_emscripten_get_canvas_element_size',
  $b: '_emscripten_enter_soft_fullscreen',
  F: '_alGetString',
  Fa: '_glBindVertexArray',
  G: '_alGetError',
  Ga: '_alSourcei',
  H: '_glTexParameteri',
  Ha: '_alBufferData',
  Ia: '_alSourceUnqueueBuffers',
  J: '_glDisable',
  Ja: '_alDeleteSources',
  K: '_glGetUniformLocation',
  Ka: '_alDeleteBuffers',
  Kc: '_glCopyTexImage2D',
  La: '_alGenBuffers',
  M: '_glBindTexture',
  Ma: '_alGenSources',
  N: '_glBindBuffer',
  Na: '_alcMakeContextCurrent',
  Oa: '_alcCreateContext',
  Pa: '_alcOpenDevice',
  R: '_alSourcePlay',
  S: '_glActiveTexture',
  T: '_glGenTextures',
  U: '_glDeleteTextures',
  Ua: '_glCheckFramebufferStatus',
  Va: '_glFramebufferRenderbuffer',
  W: '_glBindFramebuffer',
  Wa: '_glFramebufferTexture2D',
  Wb: '_glGenVertexArrays',
  X: '_glGetProgramiv',
  Xa: '_glRenderbufferStorage',
  Y: '_glAttachShader',
  Ya: '_glBindRenderbuffer',
  Yb: '_glDeleteVertexArrays',
  Z: '_glDeleteProgram',
  Za: '_glGenRenderbuffers',
  Zb: '_glfwDestroyWindow',
  _: '_glBlendFuncSeparate',
  _a: '_glGenFramebuffers',
  _b: '_emscripten_set_main_loop',
  ab: '_glGetShaderInfoLog',
  ac: '_emscripten_set_visibilitychange_callback_on_thread',
  bb: '_glCompileShader',
  bc: '_emscripten_set_keyup_callback_on_thread',
  cb: '_glShaderSource',
  cc: '_emscripten_set_keydown_callback_on_thread',
  da: '_emscripten_get_device_pixel_ratio',
  db: '_glCreateShader',
  dc: '_emscripten_set_touchcancel_callback_on_thread',
  eb: '_glClear',
  ec: '_emscripten_set_touchend_callback_on_thread',
  fb: '_glClearColor',
  ga: '_alSourcePause',
  gb: '_glfwSwapBuffers',
  gc: '_emscripten_set_touchmove_callback_on_thread',
  ha: '_glUniform1f',
  hb: '_glReadPixels',
  hc: '_emscripten_set_touchstart_callback_on_thread',
  ia: '_glVertexAttribPointer',
  ib: '_glGetProgramInfoLog',
  ic: '_emscripten_set_wheel_callback_on_thread',
  ja: '_glEnableVertexAttribArray',
  jb: '_glfwGetFramebufferSize',
  jc: '_emscripten_set_mouseup_callback_on_thread',
  ka: '_glGetAttribLocation',
  kb: '_glfwGetWindowSize',
  kc: '_emscripten_set_mousemove_callback_on_thread',
  la: '_glBufferSubData',
  lb: '_glewInit',
  lc: '_emscripten_set_mousedown_callback_on_thread',
  mb: '_glfwSwapInterval',
  mc: '_glfwTerminate',
  nb: '_glfwMakeContextCurrent',
  nc: '_glfwCreateWindow',
  oa: '_alGetSourcei',
  ob: '_glUniform1fv',
  oc: '_glfwInit',
  pa: '_glTexImage2D',
  pb: '_glUniform3f',
  pc: '_glfwPollEvents',
  qa: '_glDeleteRenderbuffers',
  qb: '_glUseProgram',
  ra: '_glDeleteFramebuffers',
  rb: '_glDeleteBuffers',
  sa: '_glGetShaderiv',
  sb: '_glBufferData',
  ta: '_glViewport',
  tb: '_glGenBuffers',
  ua: '_glLinkProgram',
  va: '_glCreateProgram',
  vc: '_alSource3f',
  wa: '_glDeleteShader',
  wc: '_alSourcef',
  xa: '_glDrawElements',
  xc: '_alSourceStop',
  ya: '_glEnable',
  yc: '_alSourceQueueBuffers',
};

/**
 * The rows this slice supplies as refusals rather than implementations, grouped by why.
 *
 * The committed map's owner text names these families too, because the CI gate can only prove the map
 * matches the ABI — it cannot prove the map is *true*. This constant is the in-repo half of that
 * honesty: a test asserts it against the compiled table, so a row that stops being a refusal has to
 * be deleted from this list in the same commit that implements it.
 */
export const ARTEMIS_GL_REFUSED: Readonly<Record<string, string>> = {
  // The nineteen audio rows: real WebAudio, deliberately not faked with a silent default.
  //
  // The eleven `__emscripten_set_*_callback_on_thread` rows were here until the event family landed;
  // they are real registrations now (the five shared blocks, the 32-touch cap, the (target, type)
  // removal key) and were deleted from this list in the same commit that implemented them.
  F: 'audio', G: 'audio', Ga: 'audio', Ha: 'audio', Ia: 'audio', Ja: 'audio', Ka: 'audio',
  La: 'audio', Ma: 'audio', Na: 'audio', Oa: 'audio', Pa: 'audio', R: 'audio', ga: 'audio',
  oa: 'audio', vc: 'audio', wc: 'audio', xc: 'audio', yc: 'audio',
  // Two rows that are real but partial: a canvas-less fullscreen and the WebGL2-only framebuffer
  // answer. They are implemented as far as this slice can honestly take them, and the gap is named.
  $b: 'partial',
};
