#include "9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークナイズした結果のトークン列はこの配列に保存する
Token tokens[1000];
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

void tokenize(char *p) {
  int i = 0;
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }
    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
      tokens[i].ty = TK_RETURN;
      tokens[i].input = p;
      i++;
      p += 6;
      continue;
    }
    if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4])) {
      tokens[i].ty = TK_ELSE;
      tokens[i].input = p;
      i++;
      p += 4;
      continue;
    }
    if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5])) {
      tokens[i].ty = TK_WHILE;
      tokens[i].input = p;
      i++;
      p += 5;
      continue;
    }
    if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3])) {
      tokens[i].ty = TK_FOR;
      tokens[i].input = p;
      i++;
      p += 3;
      continue;
    }
    if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2])) {
      tokens[i].ty = TK_IF;
      tokens[i].input = p;
      i++;
      p += 2;
      continue;
    }
    if (('a' <= *p && *p <= 'z') || ('A' <= *p && *p <= 'Z') || *p == '_') {
      int j = 0;
      // 変数が何文字続くか判定
      do {
        j++;
      } while (is_alnum(p[j]));
      tokens[i].ty = TK_IDENT;
      tokens[i].input = p;
      tokens[i].name = strndup(p, j);
      i++;
      p += j;
      continue;
    }
    if (strncmp(p, "==", 2) == 0) {
      tokens[i].ty = TK_EQ;
      tokens[i].input = p;
      i++;
      p += 2;
      continue;
    }
    if (strncmp(p, "!=", 2) == 0) {
      tokens[i].ty = TK_NE;
      tokens[i].input = p;
      i++;
      p += 2;
      continue;
    }
    if (strncmp(p, "<=", 2) == 0) {
      tokens[i].ty = TK_LE;
      tokens[i].input = p;
      i++;
      p += 2;
      continue;
    }
    if (strncmp(p, ">=", 2) == 0) {
      tokens[i].ty = TK_GE;
      tokens[i].input = p;
      i++;
      p += 2;
      continue;
    }
    if (*p == '{') {
      tokens[i].ty = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }
    if (*p == '}') {
      tokens[i].ty = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }
    if (*p == '<') {
      tokens[i].ty = TK_LT;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }
    if (*p == '>') {
      tokens[i].ty = TK_GT;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
        *p == ')' || *p == '=' || *p == ';' || *p == ',') {
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
    error_at(p, "トークナイズできません");
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
  tokenize(user_input);
  program();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  for (int i = 0; code[i]; i++) {
    gen_func(code[i]);
  }

  return 0;
}
