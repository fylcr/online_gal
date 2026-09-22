/**
 * Runtime values and lexical environments for the TJS2 interpreter.
 *
 * Values use native JavaScript objects and arrays so that runtime-provided
 * native objects (the `kag` object, `Storages`, `System`) can be handed to
 * scripts directly without marshalling.
 */

export type TjsValue = unknown;

export type TjsFunction = {
  readonly __tjsFunction: true;
  readonly name: string;
  readonly params: readonly { readonly name: string; readonly defaultValue: unknown }[];
  readonly body: unknown;
  readonly closure: Environment;
  readonly boundThis?: unknown;
};

export function isTjsFunction(value: unknown): value is TjsFunction {
  return (
    typeof value === 'object' &&
    value !== null &&
    (value as { __tjsFunction?: boolean }).__tjsFunction === true
  );
}

/** A scope chain. Lookups walk outward; assignments write through. */
export class Environment {
  private readonly bindings = new Map<string, TjsValue>();

  constructor(private readonly parent: Environment | null = null) {}

  static root(): Environment {
    return new Environment(null);
  }

  child(): Environment {
    return new Environment(this);
  }

  /** The outermost scope, used for `var` hoisting to the global frame. */
  get global(): Environment {
    let scope: Environment = this;
    while (scope.parent) scope = scope.parent;
    return scope;
  }

  /** Declares (or redeclares) a binding in this scope. */
  declare(name: string, value: TjsValue = undefined): void {
    this.bindings.set(name, value);
  }

  has(name: string): boolean {
    if (this.bindings.has(name)) return true;
    return this.parent?.has(name) ?? false;
  }

  get(name: string): TjsValue {
    if (this.bindings.has(name)) return this.bindings.get(name);
    if (this.parent) return this.parent.get(name);
    return undefined;
  }

  /**
   * Names visible in this scope, innermost first.
   *
   * Used by the script-visible global object's `ownKeys`; later duplicates are
   * dropped so an inner shadowing binding is reported once.
   */
  names(): string[] {
    const seen = new Set<string>();
    const out: string[] = [];
    for (let scope: Environment | null = this; scope; scope = scope.parent) {
      for (const name of scope.bindings.keys()) {
        if (seen.has(name)) continue;
        seen.add(name);
        out.push(name);
      }
    }
    return out;
  }

  /** Assigns to an existing binding, or declares one at the global frame. */
  set(name: string, value: TjsValue): void {
    if (this.bindings.has(name)) {
      this.bindings.set(name, value);
      return;
    }
    if (this.parent) {
      this.parent.set(name, value);
      return;
    }
    this.bindings.set(name, value);
  }

  /** Declares in the owning function/global frame, matching TJS2 `var`. */
  declareVar(name: string, value: TjsValue = undefined): void {
    this.global.declare(name, value);
  }

  /** Names visible in this scope and its ancestors, for `for in` on globals. */
  visibleNames(): readonly string[] {
    const names = new Set<string>(this.bindings.keys());
    if (this.parent) for (const name of this.parent.visibleNames()) names.add(name);
    return [...names];
  }

  entries(): readonly [string, TjsValue][] {
    return [...this.bindings.entries()];
  }
}

/** Thrown by `throw` and by runtime-native errors crossing into TJS2. */
export class TjsThrow extends Error {
  constructor(readonly thrownValue: TjsValue) {
    super(
      typeof thrownValue === 'string'
        ? thrownValue
        : (() => {
            try {
              return JSON.stringify(thrownValue);
            } catch {
              return String(thrownValue);
            }
          })(),
    );
    this.name = 'TjsThrow';
  }
}

/** `break` inside a loop or switch. */
export class BreakSignal extends Error {
  constructor() {
    super('break');
    this.name = 'BreakSignal';
  }
}

/** `continue` inside a loop. */
export class ContinueSignal extends Error {
  constructor() {
    super('continue');
    this.name = 'ContinueSignal';
  }
}

/** `return` from a function body. */
export class ReturnSignal extends Error {
  constructor(readonly value: TjsValue) {
    super('return');
    this.name = 'ReturnSignal';
  }
}

/** Truthiness rules: empty string, 0, null and void are falsy. */
export function truthy(value: TjsValue): boolean {
  if (value === null || value === undefined || value === false) return false;
  if (value === 0 || value === '') return false;
  if (typeof value === 'number' && Number.isNaN(value)) return false;
  return true;
}

/** TJS2 `+` concatenates when either side is a string. */
export function addValues(left: TjsValue, right: TjsValue): TjsValue {
  if (typeof left === 'string' || typeof right === 'string') {
    return toTjsString(left) + toTjsString(right);
  }
  return toTjsNumber(left) + toTjsNumber(right);
}

export function toTjsNumber(value: TjsValue): number {
  if (typeof value === 'number') return value;
  if (typeof value === 'boolean') return value ? 1 : 0;
  if (value === null || value === undefined) return 0;
  if (typeof value === 'string') {
    const trimmed = value.trim();
    if (trimmed === '') return 0;
    const parsed = Number(trimmed);
    return Number.isFinite(parsed) ? parsed : Number.NaN;
  }
  return Number.NaN;
}

export function toTjsString(value: TjsValue): string {
  if (value === null || value === undefined) return '';
  if (typeof value === 'string') return value;
  if (typeof value === 'number') {
    return Number.isInteger(value) ? String(value) : String(value);
  }
  if (typeof value === 'boolean') return value ? 'true' : 'false';
  if (Array.isArray(value)) return value.map((entry) => toTjsString(entry)).join(',');
  if (typeof value === 'object') {
    const record = value as { toString?: () => string };
    if (typeof record.toString === 'function' && record.toString !== Object.prototype.toString) {
      try {
        return String(record.toString());
      } catch {
        // Fall through to JSON.
      }
    }
    try {
      return JSON.stringify(value);
    } catch {
      return '[object]';
    }
  }
  return String(value);
}

export function toTjsBoolean(value: TjsValue): boolean {
  return truthy(value);
}

/**
 * Reference model.
 *
 * `get`/`set` closures capture the storage location so that compound
 * assignment (`+=`), increment and destructuring all write back correctly.
 */
export type Reference = {
  readonly get: () => TjsValue;
  readonly set: (value: TjsValue) => void;
  readonly kind: 'variable' | 'property' | 'unresolvable';
  readonly name?: string;
};

/** Property access that understands arrays, strings, maps and `length`. */
export function getProperty(target: TjsValue, key: string): TjsValue {
  if (target === null || target === undefined) return undefined;
  if (Array.isArray(target)) {
    if (key === 'length') return target.length;
    const index = Number(key);
    if (Number.isInteger(index)) return target[index];
    return (target as unknown as Record<string, TjsValue>)[key];
  }
  if (typeof target === 'string') {
    if (key === 'length') return target.length;
    const index = Number(key);
    if (Number.isInteger(index)) return target[index];
    return undefined;
  }
  if (typeof target === 'number' || typeof target === 'boolean') {
    if (key === 'length') return undefined;
    return (target as unknown as Record<string, TjsValue>)[key];
  }
  if (target instanceof Map) {
    // `Map` shadows its own methods with instance state, so the prototype is
    // consulted explicitly; this lets `Storages`-style host maps expose
    // `get`/`set`/`has` to scripts exactly as a TJS2 `Dictionary` would.
    const own = (target as unknown as Record<string, TjsValue>)[key];
    if (own !== undefined) return own;
    const prototype = Object.getPrototypeOf(target) as Record<string, TjsValue>;
    const method = prototype?.[key];
    return typeof method === 'function' ? method.bind(target) : undefined;
  }
  if (typeof target === 'function') {
    // Class objects carry their members on `.prototype`, which is how `super`
    // resolves: TJS2 exposes the base class object, so a member read has to
    // look through to its prototype.
    const own = (target as unknown as Record<string, TjsValue>)[key];
    if (own !== undefined) return own;
    const prototype = (target as { prototype?: unknown }).prototype;
    if (prototype && typeof prototype === 'object') {
      return (prototype as Record<string, TjsValue>)[key];
    }
    return undefined;
  }
  return (target as Record<string, TjsValue>)[key];
}

export function setProperty(target: TjsValue, key: string, value: TjsValue): void {
  if (target === null || target === undefined) {
    throw new TjsThrow(`cannot set property "${key}" of ${target === null ? 'null' : 'void'}`);
  }
  if (Array.isArray(target)) {
    const index = Number(key);
    if (Number.isInteger(index) && index >= 0) {
      target[index] = value;
      return;
    }
    (target as unknown as Record<string, TjsValue>)[key] = value;
    return;
  }
  if (target instanceof Map) {
    target.set(key, value);
    return;
  }
  if (typeof target === 'object') {
    (target as Record<string, TjsValue>)[key] = value;
    return;
  }
  throw new TjsThrow(`cannot set property "${key}" on a primitive value`);
}
