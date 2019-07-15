#include "9cc.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Function *function();
Node *stmt();
Node *expr();
Node *equality();
Node *add();
Node *relational();
Node *unary();
Node *mul();
Node *term();
Vector *non_empty_arguments();
Vector *arguments();
Node *assign();
Node *new_node_ident(Token *tok);
Node *new_node_num(int val);
Node *new_node(int op, Node *lhs, Node *rhs);

Token *consume(int ty) {
  if (token->ty != ty)
    return NULL;
  Token *before = token;
  token = token->next;
  return before;
}

Token *consume_number() {
  if (token->ty != TK_NUM)
    return NULL;
  Token *before = token;
  token = token->next;
  return before;
}

Token *consume_ident() {
  if (token->ty != TK_IDENT)
    return 0;
  Token *before = token;
  token = token->next;
  return before;
}

void expect(char ty) {
  if (token->ty != ty)
    error_at(token->input, "'%c'ではありません", ty);
  token = token->next;
}

char *expect_ident() {
  if (token->ty != TK_IDENT)
    error_at(token->input, "TK_IDENTではありません");
  char *name = token->name;
  token = token->next;
  return name;
}

bool at_eof() { return token->ty == TK_EOF; }

void program() {
  int i = 0;
  while (!at_eof()) {
    code[i++] = function();
  }
  code[i] = NULL;
}

Function *function() {
  Function *function;
  function = malloc(sizeof(Function));
  function->name = expect_ident();

  expect('(');
  // まずは引数なし関数定義のみ
  expect(')');
  expect('{');
  Vector *stmts = new_vector();
  while (!consume('}')) {
    vec_push(stmts, (void *)stmt());
  }
  function->statements = stmts;
  return function;
}

Node *stmt() {
  Node *node;
  if (consume('{')) {
    Vector *vec = new_vector();
    while (!consume('}')) {
      vec_push(vec, (void *)stmt());
    }
    node = malloc(sizeof(Node));
    node->ty = ND_BLOCK;
    node->statements = vec;
    return node;
  }
  if (consume(TK_FOR)) {
    node = malloc(sizeof(Node));
    node->ty = ND_FOR;
    expect('(');
    char first_colon = ';';
    char second_colon = ';';
    if (!consume(first_colon)) {
      node->init = expr();
      expect(first_colon);
    } else {
      node->init = NULL;
    }
    if (!consume(second_colon)) {
      node->cond = expr();
      expect(second_colon);
    } else {
      node->cond = NULL;
    }
    if (!consume(')')) {
      node->iter_expr = expr();
      expect(')');
    } else {
      node->iter_expr = NULL;
    }
    node->then = stmt();
    return node;
  }
  if (consume(TK_WHILE)) {
    node = malloc(sizeof(Node));
    node->ty = ND_WHILE;
    expect('(');
    node->cond = expr();
    expect(')');
    node->then = stmt();
    return node;
  }
  if (consume(TK_IF)) {
    expect('(');
    Node *if_expr = expr();
    expect(')');
    Node *if_stmt = stmt();
    node = malloc(sizeof(Node));
    node->cond = if_expr;
    node->then = if_stmt;
    if (consume(TK_ELSE)) {
      node->ty = ND_IF_ELSE;
      node->els = stmt();
    } else {
      node->ty = ND_IF;
    }
    return node;
  }
  if (consume(TK_RETURN)) {
    node = malloc(sizeof(Node));
    node->ty = ND_RETURN;
    node->lhs = expr();
  } else {
    node = expr();
  }
  expect(';');
  return node;
}
Node *expr() { return assign(); }

Node *assign() {
  Node *lhs = equality();
  if (consume('=')) {
    return new_node('=', lhs, assign());
  }
  return lhs;
}

Node *equality() {
  Node *lhs = relational();
  if (consume(TK_EQ)) {
    return new_node(ND_EQ, lhs, relational());
  }
  if (consume(TK_NE)) {
    return new_node(ND_NE, lhs, relational());
  }
  return lhs;
}
Node *relational() {
  Node *lhs = add();
  if (consume(TK_LT)) {
    return new_node(ND_LT, lhs, add());
  }
  if (consume(TK_GT)) {
    return new_node(ND_LT, add(), lhs);
  }
  if (consume(TK_LE)) {
    return new_node(ND_LE, lhs, add());
  }
  if (consume(TK_GE)) {
    return new_node(ND_LE, add(), lhs);
  }
  return lhs;
}
Node *add() {
  Node *lhs = mul();
  if (consume('+')) {
    return new_node('+', lhs, expr());
  }
  if (consume('-')) {
    return new_node('-', lhs, expr());
  }
  return lhs;
}

Node *mul() {
  Node *lhs = unary();
  if (consume('*')) {
    return new_node('*', lhs, mul());
  }
  if (consume('/')) {
    return new_node('/', lhs, mul());
  }
  return lhs;
}

Node *unary() {
  if (consume('+')) {
    return term();
  }
  if (consume('-')) {
    return new_node('-', new_node_num(0), term());
  }
  return term();
}

Node *term() {
  Token *tok = consume_number();
  if (tok) {
    return new_node_num(tok->val);
  }
  Token *tok_ident = consume_ident();
  if (tok_ident) {
    if (!consume('(')) {
      // 変数
      return new_node_ident(tok_ident);
    }
    // 関数呼び出し
    Vector *args = arguments();
    Node *node = malloc(sizeof(Node));
    node->ty = ND_FUNC_CALL;
    node->name = tok_ident->name;
    node->arguments = args;
    return node;
  }
  expect('(');
  Node *node = expr();
  expect(')');
  return node;
}

Vector *arguments() {
  if (consume(')')) {
    return new_vector();
  }
  Vector *result = non_empty_arguments();
  expect(')');
  return result;
}
Vector *non_empty_arguments() {
  Vector *args = new_vector();
  vec_push(args, (void *)term());
  if (consume(',')) {
    Vector *after_args = non_empty_arguments();
    for (int i = 0; i < after_args->len; i++) {
      vec_push(args, after_args->data[i]);
    }
  }
  return args;
}

Node *new_node(int op, Node *lhs, Node *rhs) {
  Node *node = malloc(sizeof(Node));
  node->ty = op;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_NUM;
  node->val = val;
  return node;
}

void node_debug(Node *node) {
  printf("ty: %d\n", node->ty);
  printf("lhs: %s\n", node->lhs == NULL ? "NULL" : "lhs node");
  printf("rhs: %s\n", node->rhs == NULL ? "NULL" : "rhs node");
  printf("val: %d\n", node->val);
  // printf("name: %c\n", node->name);
}

// 変数を名前で検索する。見つからなかった場合はNULLを返す
LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next) {
    if (strcmp(var->name, tok->name) != 0) {
      return var;
    }
  }
  return NULL;
}
Node *new_node_ident(Token *tok) {
  LVar *lvar = find_lvar(tok);
  Node *node = malloc(sizeof(Node));
  node->ty = ND_LVAR;
  node->name = tok->name;
  if (!lvar) {
    int new_offset;
    if (locals) {
      new_offset = locals->offset + 8;
    } else {
      new_offset = 0;
    }

    lvar = malloc(sizeof(LVar) * 8);
    lvar->next = locals;
    lvar->name = tok->name;
    lvar->offset = new_offset;
    locals = lvar;
  }
  node->offset = lvar->offset;
  return node;
}
