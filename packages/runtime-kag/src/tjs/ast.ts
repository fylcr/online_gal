/**
 * AST for the TJS2 subset the platform interprets.
 *
 * The tree is intentionally close to the source grammar so that the evaluator
 * stays a straight tree-walk: KAG scripts written in TJS2 are small control
 * snippets (menu handlers, save hooks, config readers), not hot loops.
 */

export interface Position {
  readonly line: number;
  readonly column: number;
}

export type Node = Statement | Expression;

/* ------------------------------------------------------------------ */
/* Statements                                                          */
/* ------------------------------------------------------------------ */

export interface Program {
  readonly kind: 'Program';
  readonly body: readonly Statement[];
}

export type Statement =
  | BlockStatement
  | VarStatement
  | FunctionDeclaration
  | ClassDeclaration
  | IfStatement
  | WhileStatement
  | DoWhileStatement
  | ForStatement
  | ForInStatement
  | ReturnStatement
  | BreakStatement
  | ContinueStatement
  | ThrowStatement
  | TryStatement
  | SwitchStatement
  | ExpressionStatement
  | EmptyStatement;

export interface BlockStatement {
  readonly kind: 'Block';
  readonly body: readonly Statement[];
  readonly pos: Position;
}

export interface VarStatement {
  readonly kind: 'Var';
  readonly declarations: readonly {
    readonly name: string;
    readonly init: Expression | null;
  }[];
  readonly pos: Position;
}

export interface FunctionDeclaration {
  readonly kind: 'FunctionDeclaration';
  readonly name: string;
  readonly params: readonly { readonly name: string; readonly defaultValue: Expression | null }[];
  readonly body: BlockStatement;
  readonly pos: Position;
}

export interface ClassDeclaration {
  readonly kind: 'ClassDeclaration';
  readonly name: string;
  readonly base: string | null;
  readonly body: readonly Statement[];
  readonly pos: Position;
}

export interface IfStatement {
  readonly kind: 'If';
  readonly test: Expression;
  readonly consequent: Statement;
  readonly alternate: Statement | null;
  readonly pos: Position;
}

export interface WhileStatement {
  readonly kind: 'While';
  readonly test: Expression;
  readonly body: Statement;
  readonly pos: Position;
}

export interface DoWhileStatement {
  readonly kind: 'DoWhile';
  readonly body: Statement;
  readonly test: Expression;
  readonly pos: Position;
}

export interface ForStatement {
  readonly kind: 'For';
  readonly init: Statement | Expression | null;
  readonly test: Expression | null;
  readonly update: Expression | null;
  readonly body: Statement;
  readonly pos: Position;
}

export interface ForInStatement {
  readonly kind: 'ForIn';
  readonly variable: string;
  readonly iterable: Expression;
  readonly body: Statement;
  readonly pos: Position;
}

export interface ReturnStatement {
  readonly kind: 'Return';
  readonly argument: Expression | null;
  readonly pos: Position;
}

export interface BreakStatement {
  readonly kind: 'Break';
  readonly pos: Position;
}

export interface ContinueStatement {
  readonly kind: 'Continue';
  readonly pos: Position;
}

export interface ThrowStatement {
  readonly kind: 'Throw';
  readonly argument: Expression;
  readonly pos: Position;
}

export interface TryStatement {
  readonly kind: 'Try';
  readonly block: BlockStatement;
  readonly handlerParam: string | null;
  readonly handler: BlockStatement | null;
  readonly finalizer: BlockStatement | null;
  readonly pos: Position;
}

export interface SwitchStatement {
  readonly kind: 'Switch';
  readonly discriminant: Expression;
  readonly cases: readonly {
    readonly test: Expression | null;
    readonly body: readonly Statement[];
  }[];
  readonly pos: Position;
}

export interface ExpressionStatement {
  readonly kind: 'ExpressionStatement';
  readonly expression: Expression;
  readonly pos: Position;
}

export interface EmptyStatement {
  readonly kind: 'Empty';
  readonly pos: Position;
}

/* ------------------------------------------------------------------ */
/* Expressions                                                         */
/* ------------------------------------------------------------------ */

export type Expression =
  | LiteralExpression
  | IdentifierExpression
  | ThisExpression
  | ArrayExpression
  | ObjectExpression
  | FunctionExpression
  | UnaryExpression
  | UpdateExpression
  | BinaryExpression
  | LogicalExpression
  | ConditionalExpression
  | AssignmentExpression
  | MemberExpression
  | CallExpression
  | NewExpression
  | SequenceExpression;

export interface LiteralExpression {
  readonly kind: 'Literal';
  readonly value: string | number | boolean | null;
  readonly pos: Position;
}

export interface IdentifierExpression {
  readonly kind: 'Identifier';
  readonly name: string;
  readonly pos: Position;
}

export interface ThisExpression {
  readonly kind: 'This';
  readonly pos: Position;
}

export interface ArrayExpression {
  readonly kind: 'Array';
  readonly elements: readonly (Expression | null)[];
  readonly pos: Position;
}

export interface ObjectExpression {
  readonly kind: 'Object';
  readonly properties: readonly {
    readonly key: string;
    readonly value: Expression;
    readonly computed: boolean;
    readonly isProperty: boolean;
  }[];
  readonly pos: Position;
}

export interface FunctionExpression {
  readonly kind: 'Function';
  readonly name: string | null;
  readonly params: readonly { readonly name: string; readonly defaultValue: Expression | null }[];
  readonly body: BlockStatement;
  readonly pos: Position;
}

export interface UnaryExpression {
  readonly kind: 'Unary';
  readonly operator: string;
  readonly argument: Expression;
  readonly pos: Position;
}

export interface UpdateExpression {
  readonly kind: 'Update';
  readonly operator: '++' | '--';
  readonly prefix: boolean;
  readonly argument: Expression;
  readonly pos: Position;
}

export interface BinaryExpression {
  readonly kind: 'Binary';
  readonly operator: string;
  readonly left: Expression;
  readonly right: Expression;
  readonly pos: Position;
}

export interface LogicalExpression {
  readonly kind: 'Logical';
  readonly operator: '&&' | '||';
  readonly left: Expression;
  readonly right: Expression;
  readonly pos: Position;
}

export interface ConditionalExpression {
  readonly kind: 'Conditional';
  readonly test: Expression;
  readonly consequent: Expression;
  readonly alternate: Expression;
  readonly pos: Position;
}

export interface AssignmentExpression {
  readonly kind: 'Assignment';
  readonly operator: string;
  readonly target: Expression;
  readonly value: Expression;
  readonly pos: Position;
}

export interface MemberExpression {
  readonly kind: 'Member';
  readonly object: Expression;
  readonly property: Expression;
  readonly computed: boolean;
  /** True for `?.` optional access. */
  readonly optional: boolean;
  readonly pos: Position;
}

export interface CallExpression {
  readonly kind: 'Call';
  readonly callee: Expression;
  readonly args: readonly Expression[];
  /** True for `?.()` optional call. */
  readonly optional: boolean;
  readonly pos: Position;
}

export interface NewExpression {
  readonly kind: 'New';
  readonly callee: Expression;
  readonly args: readonly Expression[];
  readonly pos: Position;
}

export interface SequenceExpression {
  readonly kind: 'Sequence';
  readonly expressions: readonly Expression[];
  readonly pos: Position;
}
