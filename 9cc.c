#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの型を表す値
enum {
  TK_NUM = 256, // 整数
  TK_IDENT,     // 識別子
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
  ND_IDENT,
};

typedef struct Node {
  int ty;
  struct Node *lhs;
  struct Node *rhs;
  int val; // tyがND_NUMの場合のみ使う
  char name;        // tyがND_IDENTの場合のみ使う
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

void node_debug(Node *node) {
    printf("ty: %d\n", node->ty);
    printf("lhs: %s\n", node->lhs);
    printf("rhs: %s\n", node->rhs);
}

Node *new_node_ident(int val) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_IDENT;
  node->name = val;
  return node;
}

// トークナイズした結果のトークン列はこの配列に保存する
Token tokens[100];
Node *code[100];

void tokenize(char *p) {
  int i = 0;
  while(*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }
    if ('a' <= *p && *p <= 'z') {
      tokens[i].ty = TK_IDENT;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '=' || *p == ';') {
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
void program();
void program_();
Node *assign();
Node *assign_();
void gen();
void gen_code();

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
  if (tokens[pos].ty = '=') {
    pos++;
    Node *node_expr = expr();
    Node *node_assign_ = assign_();
    if (node_assign_ == NULL)
      return node_expr;
    return new_node('=', node_expr, node_assign_);
  }
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

void gen_lval(Node *node) {
  if (node->ty == ND_IDENT) {
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", ('z' - node->name + 1) * 8);
    printf("  push rax\n");
    return;
  }
  error("代入の左辺値が変数ではありません");
}

void gen(Node *node) {
  if (node->ty == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  if (node->ty == ND_IDENT) {
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  }
  if (node->ty == '=') {
    gen_lval(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
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
  program();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");
  // プロローグ
  // 変数26個分の領域を確保する
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");
  for (int i = 0; code[i]; i++) {
    gen(code[i]);
    // 式の評価結果としてスタックに一つの値が残っている
    // はずなので、スタックが溢れないようにポップしておく
    printf("  pop rax\n");
  }

  // エピローグ
  // 最後の式の結果がRAXに残っているのでそれが返り値になる
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}
