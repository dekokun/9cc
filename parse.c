#include "9cc.h"
#include <stdio.h>
#include <stdlib.h>

Node *expr();
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
  Node *node_expr= expr();
  Node *node_assign_= assign_();
  if (tokens[pos].ty != ';')
    error(";で文が終わってない: %s", tokens[pos].input);
  pos++;
  if (node_assign_ == NULL)
    return node_expr;
  return new_node('=', node_expr, node_assign_);

}
Node *assign_() {
  if (tokens[pos].ty == TK_EOF || tokens[pos].ty == ';')
    return NULL;
  if (tokens[pos].ty == '=') {
    pos++;
    Node *node_expr = expr();
    Node *node_assign_ = assign_();
    if (node_assign_ == NULL)
      return node_expr;
    return new_node('=', node_expr, node_assign_);
  }
  error("assign_: 想定しないトークンです: %s",
    tokens[pos].input);
}

Node *expr() {
  Node *lhs = mul();
  if (tokens[pos].ty == TK_EOF || tokens[pos].ty == ')' || tokens[pos].ty == ';' || tokens[pos].ty == '=')
    return lhs;
  if (tokens[pos].ty == '+') {
    pos++;
    return new_node('+', lhs, expr());
  }
  if (tokens[pos].ty == '-') {
    pos++;
    return new_node('-', lhs, expr());
  }
  error("expr: 想定しないトークンです: %s",
    tokens[pos].input);
}

Node *mul() {
  Node *lhs = term();
  if (tokens[pos].ty == TK_EOF || tokens[pos].ty == '+' || tokens[pos].ty == '-' || tokens[pos].ty == ')' || tokens[pos].ty == ';' || tokens[pos].ty == '=')
    return lhs;
  if (tokens[pos].ty == ('*')) {
    pos++;
    return new_node('*', lhs, mul());
  }
  if (tokens[pos].ty == ('/')) {
    pos++;
    return new_node('/', lhs, mul());
  }
  error("mul: 想定しないトークンです: %s",
    tokens[pos].input);
}

Node *term() {
  if (tokens[pos].ty == TK_NUM)
    return new_node_num(tokens[pos++].val);
  if (tokens[pos].ty == TK_IDENT)
    return new_node_ident(tokens[pos++].val);
  if (tokens[pos].ty == '(') {
    pos++;
    Node *node = expr();
    if (tokens[pos].ty != ')')
      error("開き括弧と閉じ括弧の対応がついてないです: %s", tokens[pos].input);
    pos++;
    return node;
  }
  error("数値でも開きカッコでもないトークンです: %s",

    tokens[pos].input);
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