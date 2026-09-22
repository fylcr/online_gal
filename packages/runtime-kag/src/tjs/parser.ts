import { TjsSyntaxError, tokenize, type Token } from './lexer.js';
import type {
  BlockStatement,
  Expression,
  FunctionDeclaration,
  Position,
  Program,
  Statement,
} from './ast.js';

/** Binary operator precedence, lowest binds loosest. */
const BINARY_PRECEDENCE: Readonly<Record<string, number>> = {
  '||': 1,
  '&&': 2,
  '|': 3,
  '^': 4,
  '&': 5,
  '==': 6,
  '!=': 6,
  '===': 6,
  '!==': 6,
  '<': 7,
  '>': 7,
  '<=': 7,
  '>=': 7,
  instanceof: 7,
  in: 7,
  '<<': 8,
  '>>': 8,
  '>>>': 8,
  '+': 9,
  '-': 9,
  '*': 10,
  '/': 10,
  '%': 10,
  '**': 11,
};

const ASSIGNMENT_OPERATORS = new Set([
  '=',
  '+=',
  '-=',
  '*=',
  '/=',
  '%=',
  '&=',
  '|=',
  '^=',
  '<<=',
  '>>=',
  '>>>=',
  '**=',
  '&&=',
  '||=',
]);

/**
 * Recursive-descent parser for TJS2.
 *
 * Statements that begin with a keyword are matched directly; everything else
 * falls through to the expression parser, which is how TJS2 allows a function
 * call or assignment to be a statement without a marker.
 */
export class TjsParser {
  private readonly tokens: readonly Token[];
  private index = 0;

  constructor(source: string) {
    this.tokens = tokenize(source);
  }

  parseProgram(): Program {
    const body: Statement[] = [];
    while (!this.atEnd()) {
      // Stray semicolons between top-level statements are ignored.
      if (this.matchPunct(';')) continue;
      body.push(this.parseStatement());
    }
    return { kind: 'Program', body };
  }

  /* ---------------------------------------------------------------- */
  /* Token helpers                                                    */
  /* ---------------------------------------------------------------- */

  private get current(): Token {
    return this.tokens[Math.min(this.index, this.tokens.length - 1)]!;
  }

  private atEnd(): boolean {
    return this.current.type === 'eof';
  }

  private position(token: Token = this.current): Position {
    return { line: token.line, column: token.column };
  }

  private advance(): Token {
    const token = this.current;
    if (!this.atEnd()) this.index += 1;
    return token;
  }

  private checkPunct(value: string): boolean {
    return this.current.type === 'punct' && this.current.value === value;
  }

  private checkKeyword(value: string): boolean {
    return this.current.type === 'keyword' && this.current.value === value;
  }

  private matchPunct(value: string): boolean {
    if (!this.checkPunct(value)) return false;
    this.advance();
    return true;
  }

  private matchKeyword(value: string): boolean {
    if (!this.checkKeyword(value)) return false;
    this.advance();
    return true;
  }

  private expectPunct(value: string): Token {
    if (!this.checkPunct(value)) {
      throw this.error(`expected "${value}" but found ${this.describe(this.current)}`);
    }
    return this.advance();
  }

  private expectIdentifier(): Token {
    if (this.current.type !== 'identifier' && this.current.type !== 'keyword') {
      throw this.error(`expected an identifier but found ${this.describe(this.current)}`);
    }
    // Contextual keywords double as identifiers in TJS2 (`property`, `global`).
    return this.advance();
  }

  private describe(token: Token): string {
    if (token.type === 'eof') return 'end of input';
    if (token.type === 'string') return `string ${JSON.stringify(token.literal ?? '')}`;
    return `${token.type} "${token.value}"`;
  }

  private error(message: string, token: Token = this.current): TjsSyntaxError {
    return new TjsSyntaxError(message, token.line, token.column);
  }

  /** Consumes an optional statement terminator. */
  private endStatement(): void {
    while (this.matchPunct(';')) {
      // Allow `;;` without complaining.
    }
  }

  /* ---------------------------------------------------------------- */
  /* Statements                                                       */
  /* ---------------------------------------------------------------- */

  private parseStatement(): Statement {
    const token = this.current;
    const pos = this.position(token);

    if (this.checkPunct('{')) return this.parseBlock();
    if (this.checkPunct(';')) {
      this.advance();
      return { kind: 'Empty', pos };
    }

    if (token.type === 'keyword') {
      switch (token.value) {
        case 'var':
        case 'const':
        case 'int':
        case 'real':
        case 'string':
        case 'octet':
          return this.parseVarStatement();
        case 'function':
          return this.parseFunctionDeclaration();
        case 'class':
          return this.parseClassDeclaration();
        case 'if':
          return this.parseIf();
        case 'while':
          return this.parseWhile();
        case 'do':
          return this.parseDoWhile();
        case 'for':
          return this.parseFor();
        case 'return':
          return this.parseReturn();
        case 'break':
          this.advance();
          this.endStatement();
          return { kind: 'Break', pos };
        case 'continue':
          this.advance();
          this.endStatement();
          return { kind: 'Continue', pos };
        case 'throw':
          return this.parseThrow();
        case 'try':
          return this.parseTry();
        case 'switch':
          return this.parseSwitch();
        default:
          break;
      }
    }

    const expression = this.parseExpression();
    this.endStatement();
    return { kind: 'ExpressionStatement', expression, pos };
  }

  private parseBlock(): BlockStatement {
    const pos = this.position();
    this.expectPunct('{');
    const body: Statement[] = [];
    while (!this.checkPunct('}') && !this.atEnd()) {
      if (this.matchPunct(';')) continue;
      body.push(this.parseStatement());
    }
    this.expectPunct('}');
    return { kind: 'Block', body, pos };
  }

  /** Parses a single statement used as a loop or `if` body. */
  private parseBody(): Statement {
    return this.checkPunct('{') ? this.parseBlock() : this.parseStatement();
  }

  private parseVarStatement(): Statement {
    const pos = this.position();
    this.advance(); // the type keyword
    const declarations: { name: string; init: Expression | null }[] = [];
    for (;;) {
      const nameToken = this.expectIdentifier();
      let init: Expression | null = null;
      if (this.matchPunct('=')) init = this.parseAssignment();
      declarations.push({ name: nameToken.value, init });
      if (!this.matchPunct(',')) break;
    }
    this.endStatement();
    return { kind: 'Var', declarations, pos };
  }

  private parseParameterList(): { name: string; defaultValue: Expression | null }[] {
    this.expectPunct('(');
    const params: { name: string; defaultValue: Expression | null }[] = [];
    while (!this.checkPunct(')') && !this.atEnd()) {
      // TJS2 allows a type prefix on parameters, e.g. `function f(int a)`.
      if (
        this.current.type === 'keyword' &&
        ['int', 'real', 'string', 'octet', 'var'].includes(this.current.value)
      ) {
        this.advance();
      }
      const nameToken = this.expectIdentifier();
      let defaultValue: Expression | null = null;
      if (this.matchPunct('=')) defaultValue = this.parseAssignment();
      params.push({ name: nameToken.value, defaultValue });
      if (!this.matchPunct(',')) break;
    }
    this.expectPunct(')');
    return params;
  }

  private parseFunctionDeclaration(): FunctionDeclaration {
    const pos = this.position();
    this.advance(); // `function`
    const nameToken = this.expectIdentifier();
    const params = this.parseParameterList();
    const body = this.parseBlock();
    return {
      kind: 'FunctionDeclaration',
      name: nameToken.value,
      params,
      body,
      pos,
    };
  }

  private parseClassDeclaration(): Statement {
    const pos = this.position();
    this.advance(); // `class`
    const nameToken = this.expectIdentifier();
    let base: string | null = null;
    // TJS2 spells inheritance both as `extends Base` and `class X : Base`.
    if (this.matchKeyword('extends') || this.matchPunct(':')) {
      base = this.expectIdentifier().value;
    }
    const body: Statement[] = [];
    this.expectPunct('{');
    while (!this.checkPunct('}') && !this.atEnd()) {
      if (this.matchPunct(';')) continue;
      body.push(this.parseStatement());
    }
    this.expectPunct('}');
    return { kind: 'ClassDeclaration', name: nameToken.value, base, body, pos };
  }

  private parseIf(): Statement {
    const pos = this.position();
    this.advance();
    this.expectPunct('(');
    const test = this.parseExpression();
    this.expectPunct(')');
    const consequent = this.parseBody();
    let alternate: Statement | null = null;
    if (this.matchKeyword('else')) alternate = this.parseBody();
    return { kind: 'If', test, consequent, alternate, pos };
  }

  private parseWhile(): Statement {
    const pos = this.position();
    this.advance();
    this.expectPunct('(');
    const test = this.parseExpression();
    this.expectPunct(')');
    const body = this.parseBody();
    return { kind: 'While', test, body, pos };
  }

  private parseDoWhile(): Statement {
    const pos = this.position();
    this.advance();
    const body = this.parseBody();
    if (!this.matchKeyword('while')) {
      throw this.error('expected "while" after a do-block');
    }
    this.expectPunct('(');
    const test = this.parseExpression();
    this.expectPunct(')');
    this.endStatement();
    return { kind: 'DoWhile', body, test, pos };
  }

  private parseFor(): Statement {
    const pos = this.position();
    this.advance();
    this.expectPunct('(');

    let init: Statement | Expression | null = null;
    if (!this.checkPunct(';')) {
      if (
        this.current.type === 'keyword' &&
        ['var', 'const', 'int', 'real', 'string', 'octet'].includes(this.current.value)
      ) {
        // Reuse the var parser but keep the header semicolons intact.
        const varPos = this.position();
        this.advance();
        const declarations: { name: string; init: Expression | null }[] = [];
        for (;;) {
          const nameToken = this.expectIdentifier();
          let value: Expression | null = null;
          if (this.matchPunct('=')) value = this.parseAssignment();
          declarations.push({ name: nameToken.value, init: value });

          // `for (var key in object)` is the TJS2 for-in form.
          if (this.checkKeyword('in')) {
            this.advance();
            const iterable = this.parseExpression();
            this.expectPunct(')');
            const body = this.parseBody();
            return {
              kind: 'ForIn',
              variable: nameToken.value,
              iterable,
              body,
              pos,
            };
          }
          if (!this.matchPunct(',')) break;
        }
        init = { kind: 'Var', declarations, pos: varPos };
      } else {
        const expression = this.parseExpression();
        if (this.checkKeyword('in')) {
          this.advance();
          const iterable = this.parseExpression();
          this.expectPunct(')');
          const body = this.parseBody();
          const variable =
            expression.kind === 'Identifier'
              ? expression.name
              : (() => {
                  throw this.error('for-in requires a simple variable name');
                })();
          return { kind: 'ForIn', variable, iterable, body, pos };
        }
        init = expression;
      }
    }
    this.expectPunct(';');

    let test: Expression | null = null;
    if (!this.checkPunct(';')) test = this.parseExpression();
    this.expectPunct(';');

    let update: Expression | null = null;
    if (!this.checkPunct(')')) update = this.parseExpression();
    this.expectPunct(')');

    const body = this.parseBody();
    return { kind: 'For', init, test, update, body, pos };
  }

  private parseReturn(): Statement {
    const pos = this.position();
    this.advance();
    let argument: Expression | null = null;
    // A newline after `return` ends the statement, matching TJS2.
    if (!this.checkPunct(';') && !this.checkPunct('}') && !this.atEnd() && this.current.noNewlineBefore) {
      argument = this.parseExpression();
    }
    this.endStatement();
    return { kind: 'Return', argument, pos };
  }

  private parseThrow(): Statement {
    const pos = this.position();
    this.advance();
    const argument = this.parseExpression();
    this.endStatement();
    return { kind: 'Throw', argument, pos };
  }

  private parseTry(): Statement {
    const pos = this.position();
    this.advance();
    const block = this.parseBlock();

    let handlerParam: string | null = null;
    let handler: BlockStatement | null = null;
    let finalizer: BlockStatement | null = null;

    if (this.matchKeyword('catch')) {
      if (this.matchPunct('(')) {
        // TJS2 permits `catch (e)` and `catch (e if cond)`.
        handlerParam = this.expectIdentifier().value;
        if (this.matchKeyword('if')) this.parseExpression();
        this.expectPunct(')');
      }
      handler = this.parseBlock();
    }
    if (this.matchKeyword('finally')) finalizer = this.parseBlock();

    if (!handler && !finalizer) {
      throw this.error('a try block requires catch or finally');
    }
    return { kind: 'Try', block, handlerParam, handler, finalizer, pos };
  }

  private parseSwitch(): Statement {
    const pos = this.position();
    this.advance();
    this.expectPunct('(');
    const discriminant = this.parseExpression();
    this.expectPunct(')');
    this.expectPunct('{');

    const cases: { test: Expression | null; body: Statement[] }[] = [];
    while (!this.checkPunct('}') && !this.atEnd()) {
      let test: Expression | null = null;
      if (this.matchKeyword('case')) {
        test = this.parseExpression();
      } else if (this.matchKeyword('default')) {
        test = null;
      } else {
        throw this.error('expected "case" or "default" inside a switch');
      }
      this.expectPunct(':');
      const body: Statement[] = [];
      while (
        !this.checkKeyword('case') &&
        !this.checkKeyword('default') &&
        !this.checkPunct('}') &&
        !this.atEnd()
      ) {
        body.push(this.parseStatement());
      }
      cases.push({ test, body });
    }
    this.expectPunct('}');
    return { kind: 'Switch', discriminant, cases, pos };
  }

  /* ---------------------------------------------------------------- */
  /* Expressions                                                      */
  /* ---------------------------------------------------------------- */

  /** Entry point for a full expression, including the comma operator. */
  parseExpression(): Expression {
    const first = this.parseAssignment();
    if (!this.checkPunct(',')) return first;
    const expressions: Expression[] = [first];
    const pos = this.position();
    while (this.matchPunct(',')) {
      expressions.push(this.parseAssignment());
    }
    return { kind: 'Sequence', expressions, pos };
  }

  /** Assignment is right-associative and binds loosest below `?:`. */
  parseAssignment(): Expression {
    const pos = this.position();
    const left = this.parseConditional();
    if (this.current.type === 'punct' && ASSIGNMENT_OPERATORS.has(this.current.value)) {
      const operator = this.advance().value;
      const value = this.parseAssignment();
      return { kind: 'Assignment', operator, target: left, value, pos };
    }
    return left;
  }

  private parseConditional(): Expression {
    const pos = this.position();
    const test = this.parseBinary(0);
    if (!this.matchPunct('?')) return test;
    const consequent = this.parseAssignment();
    this.expectPunct(':');
    const alternate = this.parseAssignment();
    return { kind: 'Conditional', test, consequent, alternate, pos };
  }

  /**
   * Precedence-climbing binary parser.
   *
   * `**` is right-associative; every other binary operator is left-associative,
   * matching the reference implementation.
   */
  private parseBinary(minPrecedence: number): Expression {
    let left = this.parseUnary();
    for (;;) {
      const token = this.current;
      if (token.type !== 'punct' && token.type !== 'keyword') break;
      const precedence = BINARY_PRECEDENCE[token.value];
      if (precedence === undefined || precedence < minPrecedence) break;
      const operator = token.value;

      const pos = this.position();
      this.advance();
      const nextMin = operator === '**' ? precedence : precedence + 1;
      const right = this.parseBinary(nextMin);
      left =
        operator === '&&' || operator === '||'
          ? {
              kind: 'Logical',
              operator: operator as '&&' | '||',
              left,
              right,
              pos,
            }
          : { kind: 'Binary', operator, left, right, pos };
    }
    return left;
  }

  private parseUnary(): Expression {
    const token = this.current;
    const pos = this.position(token);

    if (
      token.type === 'punct' &&
      ['!', '-', '+', '~'].includes(token.value)
    ) {
      this.advance();
      const argument = this.parseUnary();
      return { kind: 'Unary', operator: token.value, argument, pos };
    }
    if (token.type === 'keyword' && ['typeof', 'void', 'delete', 'new'].includes(token.value)) {
      if (token.value === 'new') return this.parseNew();
      this.advance();
      const argument = this.parseUnary();
      return { kind: 'Unary', operator: token.value, argument, pos };
    }
    if (token.type === 'punct' && (token.value === '++' || token.value === '--')) {
      this.advance();
      const argument = this.parseUnary();
      return {
        kind: 'Update',
        operator: token.value as '++' | '--',
        prefix: true,
        argument,
        pos,
      };
    }
    return this.parsePostfix();
  }

  private parseNew(): Expression {
    const pos = this.position();
    this.advance(); // `new`
    const callee = this.parsePostfix();
    if (callee.kind === 'Call') {
      return { kind: 'New', callee: callee.callee, args: callee.args, pos };
    }
    return { kind: 'New', callee, args: [], pos };
  }

  private parsePostfix(): Expression {
    let expression = this.parsePrimary();
    for (;;) {
      const token = this.current;
      const pos = this.position(token);

      if (token.type === 'punct' && token.value === '.') {
        this.advance();
        const propertyToken = this.expectIdentifier();
        expression = {
          kind: 'Member',
          object: expression,
          property: {
            kind: 'Literal',
            value: propertyToken.value,
            pos: this.position(propertyToken),
          },
          computed: false,
          optional: false,
          pos,
        };
        continue;
      }

      if (token.type === 'punct' && token.value === '?.') {
        this.advance();
        const propertyToken = this.expectIdentifier();
        expression = {
          kind: 'Member',
          object: expression,
          property: {
            kind: 'Literal',
            value: propertyToken.value,
            pos: this.position(propertyToken),
          },
          computed: false,
          optional: true,
          pos,
        };
        continue;
      }

      if (token.type === 'punct' && token.value === '[') {
        this.advance();
        const property = this.parseExpression();
        this.expectPunct(']');
        expression = {
          kind: 'Member',
          object: expression,
          property,
          computed: true,
          optional: false,
          pos,
        };
        continue;
      }

      if (token.type === 'punct' && (token.value === '(' || token.value === '?.')) {
        const optional = token.value === '?.';
        if (optional) {
          // `?.(` is an optional call.
          this.advance();
          if (!this.checkPunct('(')) {
            // `?.` followed by something else was handled above; rewind is not
            // possible, so treat it as an optional member access.
            const propertyToken = this.expectIdentifier();
            expression = {
              kind: 'Member',
              object: expression,
              property: {
                kind: 'Literal',
                value: propertyToken.value,
                pos: this.position(propertyToken),
              },
              computed: false,
              optional: true,
              pos,
            };
            continue;
          }
        } else {
          this.advance();
        }
        const args: Expression[] = [];
        while (!this.checkPunct(')') && !this.atEnd()) {
          args.push(this.parseAssignment());
          if (!this.matchPunct(',')) break;
        }
        this.expectPunct(')');
        expression = { kind: 'Call', callee: expression, args, optional, pos };
        continue;
      }

      if (token.type === 'punct' && (token.value === '++' || token.value === '--')) {
        this.advance();
        expression = {
          kind: 'Update',
          operator: token.value as '++' | '--',
          prefix: false,
          argument: expression,
          pos,
        };
        continue;
      }

      break;
    }
    return expression;
  }

  private parsePrimary(): Expression {
    const token = this.current;
    const pos = this.position(token);

    switch (token.type) {
      case 'number': {
        this.advance();
        const value = token.value.startsWith('0x') || token.value.startsWith('0X')
          ? Number.parseInt(token.value, 16)
          : Number(token.value);
        return { kind: 'Literal', value, pos };
      }
      case 'string': {
        this.advance();
        return { kind: 'Literal', value: token.literal ?? '', pos };
      }
      case 'identifier':
        this.advance();
        return { kind: 'Identifier', name: token.value, pos };
      case 'keyword': {
        switch (token.value) {
          case 'true':
            this.advance();
            return { kind: 'Literal', value: true, pos };
          case 'false':
            this.advance();
            return { kind: 'Literal', value: false, pos };
          case 'null':
            this.advance();
            return { kind: 'Literal', value: null, pos };
          case 'this':
            this.advance();
            return { kind: 'This', pos };
          case 'function':
            return this.parseFunctionExpression();
          case 'new':
            return this.parseNew();
          default:
            // Contextual keywords such as `property` and `global` are usable as
            // plain identifiers.
            this.advance();
            return { kind: 'Identifier', name: token.value, pos };
        }
      }
      case 'punct': {
        if (token.value === '(') {
          this.advance();
          const expression = this.parseExpression();
          this.expectPunct(')');
          return expression;
        }
        if (token.value === '[') return this.parseArrayLiteral();
        if (token.value === '{') return this.parseObjectLiteral();
        if (token.value === '#') {
          // `#` alone is the TJS "with" receiver placeholder; treat as `this`.
          this.advance();
          return { kind: 'This', pos };
        }
        break;
      }
      default:
        break;
    }

    throw this.error(`unexpected ${this.describe(token)}`);
  }

  private parseFunctionExpression(): Expression {
    const pos = this.position();
    this.advance(); // `function`
    let name: string | null = null;
    if (this.current.type === 'identifier') name = this.advance().value;
    const params = this.parseParameterList();
    const body = this.parseBlock();
    return { kind: 'Function', name, params, body, pos };
  }

  private parseArrayLiteral(): Expression {
    const pos = this.position();
    this.expectPunct('[');
    const elements: (Expression | null)[] = [];
    while (!this.checkPunct(']') && !this.atEnd()) {
      if (this.checkPunct(',')) {
        this.advance();
        elements.push(null);
        continue;
      }
      elements.push(this.parseAssignment());
      if (!this.matchPunct(',')) break;
    }
    this.expectPunct(']');
    return { kind: 'Array', elements, pos };
  }

  private parseObjectLiteral(): Expression {
    const pos = this.position();
    this.expectPunct('{');
    const properties: {
      key: string;
      value: Expression;
      computed: boolean;
      isProperty: boolean;
    }[] = [];

    while (!this.checkPunct('}') && !this.atEnd()) {
      const keyToken = this.current;
      let key: string;
      let computed = false;
      /** TJS2's `property name = defaultValue` object member. */
      let isProperty = false;

      if (keyToken.type === 'keyword' && keyToken.value === 'property') {
        this.advance();
        isProperty = true;
        key = this.expectIdentifier().value;
      } else if (keyToken.type === 'string') {
        this.advance();
        key = keyToken.literal ?? '';
      } else if (keyToken.type === 'number') {
        this.advance();
        key = keyToken.value;
      } else if (keyToken.type === 'punct' && keyToken.value === '[') {
        this.advance();
        const dynamic = this.parseExpression();
        this.expectPunct(']');
        properties.push({
          key: '',
          value: dynamic,
          computed: true,
          isProperty: false,
        });
        if (!this.matchPunct(',')) break;
        continue;
      } else {
        key = this.expectIdentifier().value;
      }

      // `{ a }` shorthand is legal in TJS2.
      if (this.checkPunct(':') || this.checkPunct('=')) {
        this.advance();
        properties.push({
          key,
          value: this.parseAssignment(),
          computed,
          isProperty,
        });
      } else {
        properties.push({
          key,
          value: { kind: 'Identifier', name: key, pos: this.position(keyToken) },
          computed,
          isProperty,
        });
      }
      if (!this.matchPunct(',')) break;
    }

    this.expectPunct('}');
    return { kind: 'Object', properties, pos };
  }
}

/** Convenience wrapper: parse TJS2 source into a program tree. */
export function parseTjs(source: string): Program {
  return new TjsParser(source).parseProgram();
}
