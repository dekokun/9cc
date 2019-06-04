#include "9cc.h"
#include <stdio.h>
#include <stdlib.h>

Node *expr();
Node *equality();
Node *add();
Node *relational();
Node *unary();
Node *mul();
Node *term();
void program_();
Node *assign();
Node *assign_();
Node *new_node_ident(int val);
Node *new_node_num(int val);
Node *new_node(int op, Node *lhs, Node *rhs);

int pos = 0;
int code_pos = 0;

int consume(int ty) {
  if (tokens[pos].ty != ty)
    return 0;
  pos++;
  return 1;
}

void program() {
  Node *node_assign = assign();
  code[code_pos] = node_assign;
  code_pos++;
  program_();
  code[code_pos] = NULL;
}

void program_() {
  if (tokens[pos].ty == TK_EOF)
    return;
  Node *node_assign = assign();
  code[code_pos] = node_assign;
  code_pos++;
  program_();
}

Node *assign() {
  Node *node_expr = expr();
  Node *node_assign_ = assign_();
  if (!consume(';'))
    error_at(tokens[pos].input, ";で文が終わってない");
  if (node_assign_ == NULL)
    return node_expr;
  return new_node('=', node_expr, node_assign_);
}
Node *assign_() {
  if (tokens[pos].ty == TK_EOF || tokens[pos].ty == ';')
    return NULL;
  if (consume('=')) {
    Node *node_expr = expr();
    Node *node_assign_ = assign_();
    if (node_assign_ == NULL)
      return node_expr;
    return new_node('=', node_expr, node_assign_);
  }
  error_at(tokens[pos].input, "assign_: 想定しないトークンです");
}

Node *expr() { return equality(); }

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
  if (tokens[pos].ty == TK_NUM)
    return new_node_num(tokens[pos++].val);
  if (tokens[pos].ty == TK_IDENT)
    return new_node_ident(tokens[pos++].val);
  if (consume('(')) {
    Node *node = expr();
    if (!consume(')'))
      error_at(tokens[pos].input, "開き括弧と閉じ括弧の対応がついてないです");
    return node;
  }
  error_at(tokens[pos].input, "数値でも開きカッコでもないトークンです");
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
  printf("name: %c\n", node->name);
}

Node *new_node_ident(int val) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_IDENT;
  node->name = val;
  return node;
}
