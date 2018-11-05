#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの型を表す値
enum {
  TK_NUM = 256, // 整数
  TK_EOF, // 入力の終わりを表すトークン
};

// トークンの型
typedef struct {
  int ty; // トークンの型
  int val; // tyがTK_NUMの時、その数値
  char *input; // トークン文字列(エラーメッセージ用)
} Token;

enum {
  ND_NUM = 256,
};

typedef struct Node {
  int ty;
  struct Node *lhs;
  struct Node *rhs;
  int val; // tyがND_NUMの場合のみ使う
} Node;

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

// トークナイズした結果のトークン列はこの配列に保存する
Token tokens[100];
void tokenize(char *p) {
  int i = 0;
  while(*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/') {
      tokens[i].ty = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }
    if (isdigit(*p)) {
      tokens[i].ty = TK_NUM;
      tokens[i].input = p;
      tokens[i].val = strtol(p, &p, 10);
      i++;
      continue;
    }
    fprintf(stderr, "トークナイズできません: %s\n", p);
    exit(1);
  }
  tokens[i].ty = TK_EOF;
  tokens[i].input = p;
}
// エラーを報告するための関数
__attribute__((noreturn)) static void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

Node *expr();
Node *mul();
Node *term();
int pos = 0;

Node *expr() {
  Node *lhs = mul();
  if (tokens[pos].ty == TK_EOF)
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
  if (tokens[pos].ty == TK_EOF || tokens[pos].ty == '+' || tokens[pos].ty == '-')
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
  error("数値でも開きカッコでもないトークンです: %s",

    tokens[pos].input);
}

void gen(Node *node) {
  if (node->ty == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->ty) {
  case '+':
    printf("  add rax, rdi\n");
    break;
  case '-':
    printf("  sub rax, rdi\n");
    break;
  case '*':
    printf("  mul rdi\n");
    break;
  case '/':
    printf("  mov rdx, 0\n");
    printf("  div rdi\n");
    break;
  }

  printf("  push rax\n");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }
  tokenize(argv[1]);
  Node* node = expr();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");
  gen(node);

  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
