import type {
  BlockStatement,
  Expression,
  FunctionDeclaration,
  Program,
  Statement,
} from './ast.js';
import {
  BreakSignal,
  ContinueSignal,
  Environment,
  ReturnSignal,
  TjsThrow,
  addValues,
  getProperty,
  isTjsFunction,
  setProperty,
  toTjsNumber,
  toTjsString,
  truthy,
  type Reference,
  type TjsFunction,
  type TjsValue,
} from './values.js';
import { parseTjs } from './parser.js';

/** Native (host-provided) functions and objects. */
export interface TjsHost {
  /** Extra globals available to every script. */
  readonly globals: Readonly<Record<string, TjsValue>>;
  /** Called for `debugger` and for trace output from scripts. */
  readonly trace?: (message: string) => void;
}

/** A parsed script plus its cached AST, so re-running a label is cheap. */
interface ScriptUnit {
  readonly source: string;
  readonly program: Program | null;
  readonly error: string | null;
}

const STATEMENT_KINDS = new Set([
  'Block',
  'Var',
  'FunctionDeclaration',
  'ClassDeclaration',
  'If',
  'While',
  'DoWhile',
  'For',
  'ForIn',
  'Return',
  'Break',
  'Continue',
  'Throw',
  'Try',
  'Switch',
  'ExpressionStatement',
  'Empty',
]);

/** Discriminates statement nodes from expression nodes in the shared union. */
export function isStatementNode(node: unknown): node is Statement {
  if (!node || typeof node !== 'object') return false;
  const kind = (node as { kind?: unknown }).kind;
  return typeof kind === 'string' && STATEMENT_KINDS.has(kind);
}

/**
 * Tree-walking interpreter for the TJS2 subset.
 *
 * The engine is single-threaded and cooperatively scheduled: a script runs to
 * completion or to a `yield` point, and the KAG command layer drives it. That
 * matches how KiriKiri executes KAG — the script sets state, the engine owns
 * the frame loop.
 */
export class TjsInterpreter {
  readonly global: Environment;

  constructor(private readonly host: TjsHost) {
    this.global = Environment.root();
    for (const [name, value] of Object.entries(host.globals)) {
      this.global.declare(name, value);
    }
    this.installBuiltins();
  }

  /**
   * The script-visible global object.
   *
   * KAG3 scripts reach for the TJS global namespace *by name* — `snow.ks` opens
   * with `typeof(global.snow_object) == 'undefined'` to decide whether it has
   * already registered itself. That is a different thing from the interpreter's
   * frame root: the script expects a dictionary it can read and write properties
   * on, backed by the same bindings the interpreter resolves unqualified names
   * against.
   *
   * Reads and writes are forwarded to `this.global`, so `global.foo = 1` is
   * visible as a bare `foo`, which is exactly how KAG scripts use it.
   */
  globalObject(): Record<string, TjsValue> {
    const environment = this.global;
    return new Proxy({} as Record<string, TjsValue>, {
      get: (_target, key) =>
        typeof key === 'string' ? environment.get(key) : undefined,
      set: (_target, key, value) => {
        if (typeof key === 'string') environment.declare(key, value as TjsValue);
        return true;
      },
      has: (_target, key) => typeof key === 'string' && environment.has(key),
      ownKeys: () => environment.names(),
      getOwnPropertyDescriptor: () => ({ enumerable: true, configurable: true }),
    });
  }

  private installBuiltins(): void {
    const define = (name: string, value: TjsValue): void => {
      this.global.declare(name, value);
    };

    define('int', (value: TjsValue) => Math.trunc(toTjsNumber(value)));
    define('real', (value: TjsValue) => toTjsNumber(value));
    define('string', (value: TjsValue) => toTjsString(value));
    define('parseInt', (value: TjsValue, radix?: TjsValue) =>
      Number.parseInt(toTjsString(value), radix === undefined ? 10 : toTjsNumber(radix)),
    );
    define('parseFloat', (value: TjsValue) => Number.parseFloat(toTjsString(value)));
    define('isNaN', (value: TjsValue) => Number.isNaN(toTjsNumber(value)));
    define('isFinite', (value: TjsValue) => Number.isFinite(toTjsNumber(value)));
    define('Boolean', (value: TjsValue) => truthy(value));
    define('Number', (value: TjsValue) => toTjsNumber(value));
    define('String', (value: TjsValue) => toTjsString(value));
    define('Math', Math);
    define('JSON', JSON);
    define('Date', Date);

    // `Array` behaves like the JavaScript constructor but returns plain arrays,
    // which is what TJS2 scripts expect from `.length` and `[]` indexing.
    const arrayConstructor = (...args: TjsValue[]): TjsValue[] => {
      if (args.length === 1 && typeof args[0] === 'number') {
        return new Array(Math.max(0, Math.trunc(args[0]))).fill(undefined);
      }
      return [...args];
    };
    define('Array', arrayConstructor);

    define('trace', (value: TjsValue) => {
      this.host.trace?.(toTjsString(value));
    });
    define('alert', (value: TjsValue) => {
      this.host.trace?.(toTjsString(value));
    });
  }

  /** Parses (and caches) a script unit. */
  private units = new Map<string, ScriptUnit>();

  private compile(source: string, key: string): Program {
    const cached = this.units.get(key);
    if (cached) {
      if (cached.error) throw new TjsThrow(cached.error);
      return cached.program!;
    }
    try {
      const program = parseTjs(source);
      this.units.set(key, { source, program, error: null });
      return program;
    } catch (error) {
      const message = error instanceof Error ? error.message : String(error);
      this.units.set(key, { source, program: null, error: message });
      throw new TjsThrow(message);
    }
  }

  /** Runs a whole script, returning the value of a trailing `return` if any. */
  run(source: string, key = `<inline:${source.length}>`): TjsValue {
    const program = this.compile(source, key);
    try {
      this.execBlock(program.body, this.global);
    } catch (error) {
      if (error instanceof ReturnSignal) return error.value;
      throw error;
    }
    return undefined;
  }

  /**
   * Evaluates the argument of a TJS expression embedded in a KAG attribute
   * (the `%[...]` form). Returns void when the expression is empty.
   */
  evaluateExpression(source: string, scope: Environment = this.global): TjsValue {
    const trimmed = source.trim();
    if (!trimmed) return undefined;
    const program = this.compile(`return (${trimmed});`, `<expr:${trimmed}>`);
    const local = scope.child();
    try {
      this.execBlock(program.body, local);
    } catch (error) {
      if (error instanceof ReturnSignal) return error.value;
      throw error;
    }
    return undefined;
  }

  /** Creates a callable TJS function value from source, e.g. an event handler. */
  createFunction(
    params: readonly string[],
    bodySource: string,
    name = 'anonymous',
  ): TjsFunction {
    const parameterList = params.join(', ');
    const program = this.compile(
      `function ${name}(${parameterList}) { ${bodySource} }`,
      `<fn:${name}:${bodySource.length}>`,
    );
    const scope = this.global.child();
    this.execBlock(program.body, scope);
    const created = scope.get(name);
    if (!isTjsFunction(created)) {
      throw new TjsThrow(`failed to compile function "${name}"`);
    }
    return created;
  }

  /* ---------------------------------------------------------------- */
  /* Statement execution                                              */
  /* ---------------------------------------------------------------- */

  private execBlock(statements: readonly Statement[], scope: Environment): void {
    for (const statement of statements) {
      this.execStatement(statement, scope);
    }
  }

  /** Executes a statement, returning a value for expression statements. */
  private execStatement(statement: Statement, scope: Environment): TjsValue {
    switch (statement.kind) {
      case 'Empty':
        return undefined;

      case 'ExpressionStatement':
        return this.evaluate(statement.expression, scope);

      case 'Block': {
        // A bare block gets its own scope, but `var` still hoists to the frame.
        const inner = scope.child();
        this.execBlock(statement.body, inner);
        return undefined;
      }

      case 'Var': {
        for (const declaration of statement.declarations) {
          const value =
            declaration.init === null
              ? undefined
              : this.evaluate(declaration.init, scope);
          scope.declare(declaration.name, value);
        }
        return undefined;
      }

      case 'FunctionDeclaration': {
        scope.declare(statement.name, this.makeFunction(statement, scope));
        return undefined;
      }

      case 'ClassDeclaration': {
        scope.declare(statement.name, this.makeClass(statement, scope));
        return undefined;
      }

      case 'If': {
        if (truthy(this.evaluate(statement.test, scope))) {
          this.execStatement(statement.consequent, scope);
        } else if (statement.alternate) {
          this.execStatement(statement.alternate, scope);
        }
        return undefined;
      }

      case 'While': {
        let guard = 0;
        while (truthy(this.evaluate(statement.test, scope))) {
          if ((guard += 1) > 10_000_000) {
            throw new TjsThrow('while loop exceeded the iteration guard');
          }
          try {
            this.execStatement(statement.body, scope);
          } catch (error) {
            if (error instanceof BreakSignal) break;
            if (error instanceof ContinueSignal) continue;
            throw error;
          }
        }
        return undefined;
      }

      case 'DoWhile': {
        let guard = 0;
        do {
          if ((guard += 1) > 10_000_000) {
            throw new TjsThrow('do-while loop exceeded the iteration guard');
          }
          try {
            this.execStatement(statement.body, scope);
          } catch (error) {
            if (error instanceof BreakSignal) break;
            if (error instanceof ContinueSignal) continue;
            throw error;
          }
        } while (truthy(this.evaluate(statement.test, scope)));
        return undefined;
      }

      case 'For': {
        const loopScope = scope.child();
        if (statement.init) {
          if (isStatementNode(statement.init)) {
            this.execStatement(statement.init, loopScope);
          } else {
            this.evaluate(statement.init, loopScope);
          }
        }
        let guard = 0;
        for (;;) {
          if ((guard += 1) > 10_000_000) {
            throw new TjsThrow('for loop exceeded the iteration guard');
          }
          if (statement.test && !truthy(this.evaluate(statement.test, loopScope))) break;
          try {
            this.execStatement(statement.body, loopScope);
          } catch (error) {
            if (error instanceof BreakSignal) break;
            if (error instanceof ContinueSignal) {
              // Fall through to the update expression.
            } else {
              throw error;
            }
          }
          if (statement.update) this.evaluate(statement.update, loopScope);
        }
        return undefined;
      }

      case 'ForIn': {
        const iterable = this.evaluate(statement.iterable, scope);
        const keys = this.enumerableKeys(iterable);
        for (const key of keys) {
          scope.set(statement.variable, key);
          try {
            this.execStatement(statement.body, scope);
          } catch (error) {
            if (error instanceof BreakSignal) break;
            if (error instanceof ContinueSignal) continue;
            throw error;
          }
        }
        return undefined;
      }

      case 'Return': {
        const value =
          statement.argument === null ? undefined : this.evaluate(statement.argument, scope);
        throw new ReturnSignal(value);
      }

      case 'Break':
        throw new BreakSignal();

      case 'Continue':
        throw new ContinueSignal();

      case 'Throw':
        throw new TjsThrow(this.evaluate(statement.argument, scope));

      case 'Try': {
        try {
          this.execBlock(statement.block.body, scope.child());
        } catch (error) {
          if (error instanceof ReturnSignal || error instanceof BreakSignal || error instanceof ContinueSignal) {
            this.runFinalizer(statement.finalizer, scope);
            throw error;
          }
          if (!statement.handler) {
            this.runFinalizer(statement.finalizer, scope);
            throw error;
          }
          const catchScope = scope.child();
          const thrown =
            error instanceof TjsThrow
              ? error.thrownValue
              : error instanceof Error
                ? error.message
                : error;
          if (statement.handlerParam) catchScope.declare(statement.handlerParam, thrown);
          try {
            this.execBlock(statement.handler.body, catchScope);
          } catch (nested) {
            this.runFinalizer(statement.finalizer, scope);
            throw nested;
          }
        }
        this.runFinalizer(statement.finalizer, scope);
        return undefined;
      }

      case 'Switch': {
        const discriminant = this.evaluate(statement.discriminant, scope);
        let matched = false;
        try {
          for (const switchCase of statement.cases) {
            if (!matched) {
              if (switchCase.test === null) {
                matched = true;
              } else if (
                this.strictEquals(discriminant, this.evaluate(switchCase.test, scope))
              ) {
                matched = true;
              }
            }
            if (matched) this.execBlock(switchCase.body, scope.child());
          }
        } catch (error) {
          if (error instanceof BreakSignal) return undefined;
          throw error;
        }
        return undefined;
      }

      default:
        throw new TjsThrow(`unsupported statement: ${(statement as Statement).kind}`);
    }
  }

  private runFinalizer(finalizer: BlockStatement | null, scope: Environment): void {
    if (finalizer) this.execBlock(finalizer.body, scope.child());
  }

  private enumerableKeys(value: TjsValue): readonly string[] {
    if (value === null || value === undefined) return [];
    if (Array.isArray(value)) return value.map((_, index) => String(index));
    if (value instanceof Map) return [...value.keys()].map(String);
    if (typeof value === 'object') return Object.keys(value as object);
    return [];
  }

  private makeFunction(
    declaration: Pick<FunctionDeclaration, 'name' | 'params' | 'body'>,
    scope: Environment,
  ): TjsFunction {
    return {
      __tjsFunction: true,
      name: declaration.name,
      params: declaration.params.map((param) => ({
        name: param.name,
        defaultValue: param.defaultValue,
      })),
      body: declaration.body,
      closure: scope,
    };
  }

  /**
   * Builds a class as a constructor function carrying its prototype.
   *
   * TJS2 classes are prototype-based underneath, so this maps directly onto
   * JavaScript's own prototype chain, which keeps `this` semantics identical.
   */
  private makeClass(
    declaration: Extract<Statement, { kind: 'ClassDeclaration' }>,
    scope: Environment,
  ): TjsValue {
    const classScope = scope.child();
    if (declaration.base) {
      classScope.declare('super', scope.get(declaration.base));
    }

    const methods = new Map<string, TjsFunction>();
    const statics = new Map<string, TjsValue>();
    let constructor: TjsFunction | null = null;

    for (const member of declaration.body) {
      if (member.kind === 'FunctionDeclaration') {
        const fn = this.makeFunction(member, classScope);
        if (member.name === 'constructor') constructor = fn;
        else methods.set(member.name, fn);
        continue;
      }
      if (member.kind === 'Var') {
        for (const variable of member.declarations) {
          statics.set(
            variable.name,
            variable.init === null ? undefined : this.evaluate(variable.init, classScope),
          );
        }
        continue;
      }
      if (member.kind === 'ExpressionStatement') {
        this.evaluate(member.expression, classScope);
      }
    }

    const basePrototype =
      declaration.base && typeof scope.get(declaration.base) === 'function'
        ? ((scope.get(declaration.base) as { prototype?: object }).prototype ?? null)
        : null;

    const prototype = Object.create(basePrototype);
    for (const [name, method] of methods) {
      prototype[name] = method;
    }

    const self = this;
    function ClassConstructor(this: unknown, ...args: TjsValue[]): TjsValue {
      const instance = Object.create(prototype) as Record<string, TjsValue>;
      if (constructor) {
        self.callFunction(constructor, args, instance);
      } else if (basePrototype && typeof basePrototype === 'object') {
        // Delegate to the base constructor when there is no explicit one.
        const base = basePrototype as { constructor?: TjsFunction };
        if (base.constructor) self.callFunction(base.constructor, args, instance);
      }
      return instance;
    }

    Object.defineProperty(ClassConstructor, 'name', { value: declaration.name });
    (ClassConstructor as unknown as { prototype: object }).prototype = prototype;
    for (const [name, value] of statics) {
      (ClassConstructor as unknown as Record<string, TjsValue>)[name] = value;
    }
    // Expose the class in its own scope so methods can reference static members.
    classScope.declare(declaration.name, ClassConstructor as unknown as TjsValue);

    return ClassConstructor as unknown as TjsValue;
  }

  /* ---------------------------------------------------------------- */
  /* Expression evaluation                                            */
  /* ---------------------------------------------------------------- */

  evaluate(expression: Expression, scope: Environment): TjsValue {
    switch (expression.kind) {
      case 'Literal':
        return expression.value;

      case 'Identifier':
        return scope.get(expression.name);

      case 'This':
        return scope.get('this');

      case 'Array':
        return expression.elements.map((element) =>
          element === null ? undefined : this.evaluate(element, scope),
        );

      case 'Object': {
        const target: Record<string, TjsValue> = {};
        for (const property of expression.properties) {
          const key = property.computed
            ? toTjsString(this.evaluate(property.value, scope))
            : property.key;
          if (property.computed) {
            // A computed key evaluates to the key itself, with no value.
            target[key] = undefined;
            continue;
          }
          target[key] = this.evaluate(property.value, scope);
        }
        return target;
      }

      case 'Function':
        return this.makeFunction(
          {
            name: expression.name ?? 'anonymous',
            params: expression.params,
            body: expression.body,
          },
          scope,
        );

      case 'Unary': {
        switch (expression.operator) {
          case '!':
            return !truthy(this.evaluate(expression.argument, scope));
          case '-':
            return -toTjsNumber(this.evaluate(expression.argument, scope));
          case '+':
            return toTjsNumber(this.evaluate(expression.argument, scope));
          case '~':
            return ~Math.trunc(toTjsNumber(this.evaluate(expression.argument, scope)));
          case 'typeof': {
            const value = this.evaluate(expression.argument, scope);
            if (value === null || value === undefined) return 'void';
            if (isTjsFunction(value)) return 'function';
            if (Array.isArray(value)) return 'array';
            return typeof value;
          }
          case 'void':
            this.evaluate(expression.argument, scope);
            return undefined;
          case 'delete': {
            const reference = this.resolve(expression.argument, scope);
            if (reference.kind === 'property' && reference.name !== undefined) {
              const target = this.evaluate(
                (expression.argument as Extract<Expression, { kind: 'Member' }>).object,
                scope,
              );
              if (target && typeof target === 'object') {
                delete (target as Record<string, TjsValue>)[reference.name];
              }
            }
            return true;
          }
          default:
            throw new TjsThrow(`unsupported unary operator "${expression.operator}"`);
        }
      }

      case 'Update': {
        const reference = this.resolve(expression.argument, scope);
        const previous = toTjsNumber(reference.get());
        const next = expression.operator === '++' ? previous + 1 : previous - 1;
        reference.set(next);
        return expression.prefix ? next : previous;
      }

      case 'Binary': {
        // Short-circuit operators never reach here; they are Logical nodes.
        const left = this.evaluate(expression.left, scope);
        const right = this.evaluate(expression.right, scope);
        return this.applyBinary(expression.operator, left, right);
      }

      case 'Logical': {
        const left = this.evaluate(expression.left, scope);
        if (expression.operator === '&&') {
          return truthy(left) ? this.evaluate(expression.right, scope) : left;
        }
        return truthy(left) ? left : this.evaluate(expression.right, scope);
      }

      case 'Conditional':
        return truthy(this.evaluate(expression.test, scope))
          ? this.evaluate(expression.consequent, scope)
          : this.evaluate(expression.alternate, scope);

      case 'Assignment': {
        const reference = this.resolve(expression.target, scope);
        const right = this.evaluate(expression.value, scope);
        if (expression.operator === '=') {
          reference.set(right);
          return right;
        }
        const current = reference.get();
        let result: TjsValue;
        switch (expression.operator) {
          case '+=':
            result = addValues(current, right);
            break;
          case '-=':
            result = toTjsNumber(current) - toTjsNumber(right);
            break;
          case '*=':
            result = toTjsNumber(current) * toTjsNumber(right);
            break;
          case '/=': {
            const divisor = toTjsNumber(right);
            if (divisor === 0) throw new TjsThrow('division by zero');
            result = toTjsNumber(current) / divisor;
            break;
          }
          case '%=':
            result = toTjsNumber(current) % toTjsNumber(right);
            break;
          case '&=':
            result = Math.trunc(toTjsNumber(current)) & Math.trunc(toTjsNumber(right));
            break;
          case '|=':
            result = Math.trunc(toTjsNumber(current)) | Math.trunc(toTjsNumber(right));
            break;
          case '^=':
            result = Math.trunc(toTjsNumber(current)) ^ Math.trunc(toTjsNumber(right));
            break;
          case '<<=':
            result = Math.trunc(toTjsNumber(current)) << Math.trunc(toTjsNumber(right));
            break;
          case '>>=':
            result = Math.trunc(toTjsNumber(current)) >> Math.trunc(toTjsNumber(right));
            break;
          case '>>>=':
            result = Math.trunc(toTjsNumber(current)) >>> Math.trunc(toTjsNumber(right));
            break;
          case '**=':
            result = toTjsNumber(current) ** toTjsNumber(right);
            break;
          case '&&=':
            result = truthy(current) ? right : current;
            break;
          case '||=':
            result = truthy(current) ? current : right;
            break;
          default:
            throw new TjsThrow(`unsupported assignment operator "${expression.operator}"`);
        }
        reference.set(result);
        return result;
      }

      case 'Member': {
        const target = this.evaluate(expression.object, scope);
        if (target === null || target === undefined) {
          if (expression.optional) return undefined;
          throw new TjsThrow(
            `cannot read property of ${target === null ? 'null' : 'void'}`,
          );
        }
        const key = expression.computed
          ? toTjsString(this.evaluate(expression.property, scope))
          : String((expression.property as Extract<Expression, { kind: 'Literal' }>).value);
        return getProperty(target, key);
      }

      case 'Call': {
        const callee = this.evaluate(expression.callee, scope);
        if (callee === null || callee === undefined) {
          if (expression.optional) return undefined;
          throw new TjsThrow('attempted to call a void value');
        }
        // `this` is the object a method was read from, not the surrounding scope.
        const thisArg =
          expression.callee.kind === 'Member'
            ? this.evaluate(expression.callee.object, scope)
            : scope.get('this');
        const args = expression.args.map((argument) => this.evaluate(argument, scope));
        return this.callValue(callee, args, thisArg, expression.callee.kind === 'Member');
      }

      case 'New': {
        const callee = this.evaluate(expression.callee, scope);
        const args = expression.args.map((argument) => this.evaluate(argument, scope));
        if (isTjsFunction(callee)) {
          const instance = this.callFunction(callee, args);
          return instance;
        }
        if (typeof callee === 'function') {
          return new (callee as new (...values: TjsValue[]) => TjsValue)(...args);
        }
        throw new TjsThrow('`new` requires a constructor');
      }

      case 'Sequence': {
        let result: TjsValue = undefined;
        for (const item of expression.expressions) {
          result = this.evaluate(item, scope);
        }
        return result;
      }

      default:
        throw new TjsThrow(`unsupported expression: ${(expression as Expression).kind}`);
    }
  }

  private applyBinary(operator: string, left: TjsValue, right: TjsValue): TjsValue {
    switch (operator) {
      case '+':
        return addValues(left, right);
      case '-':
        return toTjsNumber(left) - toTjsNumber(right);
      case '*':
        return toTjsNumber(left) * toTjsNumber(right);
      case '/': {
        const divisor = toTjsNumber(right);
        if (divisor === 0) throw new TjsThrow('division by zero');
        return toTjsNumber(left) / divisor;
      }
      case '%':
        return toTjsNumber(left) % toTjsNumber(right);
      case '**':
        return toTjsNumber(left) ** toTjsNumber(right);
      case '==':
        return this.looseEquals(left, right);
      case '!=':
        return !this.looseEquals(left, right);
      case '===':
        return this.strictEquals(left, right);
      case '!==':
        return !this.strictEquals(left, right);
      case '<':
        return this.compare(left, right) < 0;
      case '>':
        return this.compare(left, right) > 0;
      case '<=':
        return this.compare(left, right) <= 0;
      case '>=':
        return this.compare(left, right) >= 0;
      case '&':
        return Math.trunc(toTjsNumber(left)) & Math.trunc(toTjsNumber(right));
      case '|':
        return Math.trunc(toTjsNumber(left)) | Math.trunc(toTjsNumber(right));
      case '^':
        return Math.trunc(toTjsNumber(left)) ^ Math.trunc(toTjsNumber(right));
      case '<<':
        return Math.trunc(toTjsNumber(left)) << Math.trunc(toTjsNumber(right));
      case '>>':
        return Math.trunc(toTjsNumber(left)) >> Math.trunc(toTjsNumber(right));
      case '>>>':
        return Math.trunc(toTjsNumber(left)) >>> Math.trunc(toTjsNumber(right));
      case 'in':
        return this.hasMember(right, left);
      case 'instanceof':
        return this.isInstanceOf(left, right);
      default:
        throw new TjsThrow(`unsupported binary operator "${operator}"`);
    }
  }

  private compare(left: TjsValue, right: TjsValue): number {
    if (typeof left === 'string' || typeof right === 'string') {
      const a = toTjsString(left);
      const b = toTjsString(right);
      return a < b ? -1 : a > b ? 1 : 0;
    }
    const a = toTjsNumber(left);
    const b = toTjsNumber(right);
    return a < b ? -1 : a > b ? 1 : 0;
  }

  private looseEquals(left: TjsValue, right: TjsValue): boolean {
    if (left === right) return true;
    if (left === null || left === undefined) return right === null || right === undefined;
    if (right === null || right === undefined) return false;
    if (typeof left === 'object' || typeof right === 'object') return left === right;
    return toTjsNumber(left) === toTjsNumber(right) || toTjsString(left) === toTjsString(right);
  }

  private strictEquals(left: TjsValue, right: TjsValue): boolean {
    if (left === right) return true;
    if (left === null || left === undefined || right === null || right === undefined) {
      return (left === null || left === undefined) && (right === null || right === undefined);
    }
    return false;
  }

  private hasMember(target: TjsValue, key: TjsValue): boolean {
    if (target === null || target === undefined) return false;
    const name = toTjsString(key);
    if (Array.isArray(target)) {
      if (name === 'length') return true;
      const index = Number(name);
      return Number.isInteger(index) && index >= 0 && index < target.length;
    }
    if (target instanceof Map) return target.has(name);
    if (typeof target === 'object') return name in (target as object);
    return false;
  }

  private isInstanceOf(value: TjsValue, constructor: TjsValue): boolean {
    if (typeof constructor !== 'function') return false;
    if (constructor === Array) return Array.isArray(value);
    if (value === null || typeof value !== 'object') return false;
    const prototype = (constructor as { prototype?: unknown }).prototype;
    if (!prototype || typeof prototype !== 'object') return false;
    return prototype.isPrototypeOf(value);
  }

  /* ---------------------------------------------------------------- */
  /* References and calls                                             */
  /* ---------------------------------------------------------------- */

  private resolve(expression: Expression, scope: Environment): Reference {
    if (expression.kind === 'Identifier') {
      const name = expression.name;
      return {
        kind: scope.has(name) ? 'variable' : 'unresolvable',
        name,
        get: () => scope.get(name),
        set: (value) => scope.set(name, value),
      };
    }

    if (expression.kind === 'Member') {
      const target = this.evaluate(expression.object, scope);
      const key = expression.computed
        ? toTjsString(this.evaluate(expression.property, scope))
        : String((expression.property as Extract<Expression, { kind: 'Literal' }>).value);
      return {
        kind: 'property',
        name: key,
        get: () => getProperty(target, key),
        set: (value) => setProperty(target, key, value),
      };
    }

    throw new TjsThrow('invalid assignment target');
  }

  /** Calls a TJS function or a native JavaScript function. */
  callValue(
    callee: TjsValue,
    args: readonly TjsValue[],
    thisArg: TjsValue,
    hasThis: boolean,
  ): TjsValue {
    if (isTjsFunction(callee)) {
      return this.callFunction(callee, args, hasThis ? thisArg : undefined);
    }
    if (typeof callee === 'function') {
      const fn = callee as (...values: TjsValue[]) => TjsValue;
      // Native methods need their receiver, plain functions must not get one.
      return hasThis ? fn.apply(thisArg, args as TjsValue[]) : fn(...args);
    }
    throw new TjsThrow('attempted to call a non-function value');
  }

  /** Invokes a TJS function value with a fresh scope. */
  callFunction(
    fn: TjsFunction,
    args: readonly TjsValue[],
    thisArg: TjsValue = fn.boundThis,
  ): TjsValue {
    const scope = fn.closure.child();
    scope.declare('this', thisArg);
    scope.declare(fn.name, fn);
    for (let i = 0; i < fn.params.length; i += 1) {
      const param = fn.params[i]!;
      const provided = args[i];
      if (provided === undefined && param.defaultValue !== null && param.defaultValue !== undefined) {
        // Defaults are stored as AST in the parser output; re-evaluate them lazily.
        scope.declare(param.name, this.evaluateDefault(param.defaultValue, scope));
      } else {
        scope.declare(param.name, provided);
      }
    }
    // `arguments` is an array in TJS2, not an Arguments object.
    scope.declare('arguments', [...args]);
    try {
      this.execBlock((fn.body as BlockStatement).body, scope);
    } catch (error) {
      if (error instanceof ReturnSignal) return error.value;
      throw error;
    }
    return undefined;
  }

  /** Evaluates a parameter default, which is captured as source text. */
  private evaluateDefault(defaultValue: unknown, scope: Environment): TjsValue {
    if (
      defaultValue &&
      typeof defaultValue === 'object' &&
      'kind' in (defaultValue as object)
    ) {
      return this.evaluate(defaultValue as Expression, scope);
    }
    return undefined;
  }

  /** Registers a native global (used to expose `kag`, `System`, `Storages`). */
  define(name: string, value: TjsValue): void {
    this.global.declare(name, value);
  }

  /** For diagnostics: the number of cached script units. */
  get cachedUnitCount(): number {
    return this.units.size;
  }
}
