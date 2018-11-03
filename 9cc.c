#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  char *p = argv[1];

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");
  printf("  mov rax, %1d\n", strtol(p, &p, 10));

  while (*p) {
    if (*p == '+') {
      p++;
      printf("  add rax, %1d\n", strtol(p, &p, 10));
      continue;
    }
    if (*p == '-') {
      p++;
      printf("  sub rax, %1d\n", strtol(p, &p, 10));
      continue;
    }

    fprintf(stderr, "予期せぬ文字です, '%c'\n", *p);
  }


  printf("  ret\n");
  return 0;
}
