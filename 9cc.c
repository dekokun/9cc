#include "9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>



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
__attribute__((noreturn)) void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
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
