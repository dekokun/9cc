#include "9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークナイズした結果のトークン列はこの配列に保存する
Function *code[100];
char *user_input;
Map *ident_map;

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, ""); // pos個の空白を出力
  fprintf(stderr, "^");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void log_at(char *loc, char *msg) {
  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, ""); // pos個の空白を出力
  fprintf(stderr, "^ %s\n", msg);
}

int is_alnum(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') || (c == '_');
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(int ty, Token *cur, char *input) {
  Token *tok = calloc(1, sizeof(Token));
  tok->ty = ty;
  tok->input = input;
  cur->next = tok;
  return tok;
}

Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }
    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
      cur = new_token(TK_RETURN, cur, p);
      p += 6;
      continue;
    }
    if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4])) {
      cur = new_token(TK_ELSE, cur, p);
      p += 4;
      continue;
    }
    if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5])) {
      cur = new_token(TK_WHILE, cur, p);
      p += 5;
      continue;
    }
    if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3])) {
      cur = new_token(TK_FOR, cur, p);
      p += 3;
      continue;
    }
    if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2])) {
      cur = new_token(TK_IF, cur, p);
      p += 2;
      continue;
    }
    if (('a' <= *p && *p <= 'z') || ('A' <= *p && *p <= 'Z') || *p == '_') {
      int j = 0;
      // 変数が何文字続くか判定
      do {
        j++;
      } while (is_alnum(p[j]));
      cur = new_token(TK_IDENT, cur, p);
      cur->name = strndup(p, j);
      p += j;
      continue;
    }
    if (strncmp(p, "==", 2) == 0) {
      cur = new_token(TK_EQ, cur, p);
      p += 2;
      continue;
    }
    if (strncmp(p, "!=", 2) == 0) {
      cur = new_token(TK_NE, cur, p);
      p += 2;
      continue;
    }
    if (strncmp(p, "<=", 2) == 0) {
      cur = new_token(TK_LE, cur, p);
      p += 2;
      continue;
    }
    if (strncmp(p, ">=", 2) == 0) {
      cur = new_token(TK_GE, cur, p);
      p += 2;
      continue;
    }
    if (*p == '{') {
      cur = new_token(*p, cur, p);
      p++;
      continue;
    }
    if (*p == '}') {
      cur = new_token(*p, cur, p);
      p++;
      continue;
    }
    if (*p == '<') {
      cur = new_token(TK_LT, cur, p);
      p++;
      continue;
    }
    if (*p == '>') {
      cur = new_token(TK_GT, cur, p);
      p++;
      continue;
    }
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
        *p == ')' || *p == '=' || *p == ';' || *p == ',') {
      cur = new_token(*p, cur, p);
      p++;
      continue;
    }
    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }
    error_at(p, "トークナイズできません");
  }
  cur = new_token(TK_EOF, cur, p);
  return head.next;
}
// エラーを報告するための関数
__attribute__((noreturn)) void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void logging(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }
  // テスト実行
  if (strcmp(argv[1], "-test") == 0) {
    runtest();
    return 0;
  }
  user_input = argv[1];
  ident_map = new_map();
  token = tokenize(user_input);

  program();
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  for (int i = 0; code[i]; i++) {
    gen_func(code[i]);
  }

  return 0;
}
