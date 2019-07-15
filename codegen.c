#include "9cc.h"
#include <stdio.h>

void gen();
int label_num = 0;

void gen_lval(Node *node) {
  if (node->ty != ND_LVAR) {
    error("代入の左辺値が変数ではありません");
  }
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
  return;
}

void gen_func(Function *function) {
  printf("%s:\n", function->name);
  // プロローグ
  // 変数26個分の領域を確保する
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, %d\n", locals->offset + 8);
  for (int i = 0; i < function->statements->len; i++) {
    gen((Node *)function->statements->data[i]);
    // 式の評価結果としてスタックに一つの値が残っている
    // はずなので、スタックが溢れないようにポップしておく
    printf("  pop rax\n");
  }
  // エピローグ
  // 最後の式の結果がRAXに残っているのでそれが返り値になる
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}
void gen(Node *node) {
  if (node->ty == ND_BLOCK) {
    Vector *statements = node->statements;
    for (int i = 0; i < statements->len; i++) {
      Node *node = (Node *)statements->data[i];
      gen(node);
      printf("  pop rax\n");
    }
    return;
  }
  if (node->ty == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }
  if (node->ty == ND_RETURN) {
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  }

  if (node->ty == ND_WHILE) {
    int _label_num = label_num;
    label_num += 1;
    printf("  .Lbegin%d:\n", _label_num);
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .Lend%d\n", _label_num);
    gen(node->then);
    printf("  jmp  .Lbegin%d\n", _label_num);
    printf("  .Lend%d:\n", _label_num);
    return;
  }

  if (node->ty == ND_FOR) {
    int _label_num = label_num;
    label_num += 1;
    if (node->init) {
      gen(node->init);
    }
    printf("  .Lforbegin%d:\n", _label_num);
    if (node->cond) {
      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je  .Lforend%d\n", _label_num);
    }
    gen(node->then);
    if (node->iter_expr) {
      gen(node->iter_expr);
    }
    printf("  jmp  .Lforbegin%d\n", _label_num);
    if (node->cond) {
      printf("  .Lforend%d:\n", _label_num);
    }
    return;
  }

  if (node->ty == ND_IF) {
    int _label_num = label_num;
    label_num += 1;
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .Lend%d\n", _label_num);
    gen(node->then);
    printf("  .Lend%d:\n", _label_num);
    return;
  }

  if (node->ty == ND_IF_ELSE) {
    int _label_num = label_num;
    label_num += 1;
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .Lelse%d\n", _label_num);
    gen(node->then);
    printf("  jmp .Lend%d\n", _label_num);
    printf("  .Lelse%d:\n", _label_num);
    gen(node->els);
    printf("  .Lend%d:\n", _label_num);
    return;
  }

  if (node->ty == ND_FUNC_CALL) {
    Vector *arguments = node->arguments;
    // 第1引数～第6引数まで RDI, RSI, RDX, RCX, R8, R9 を順に使用
    char *registers[6] = {"RDI", "RSI", "RDX", "RCX", "R8", "R9"};
    for (int i = 0; i < arguments->len; i++) {
      Node *node = (Node *)arguments->data[i];
      gen(node);
      printf("  pop %s\n", registers[i]);
    }
    printf("  call %s\n", node->name);
    printf("  push rax\n");
    return;
  }
  if (node->ty == ND_LVAR) {
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
    printf("  imul rdi\n");
    break;
  case '/':
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_EQ:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NE:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LE:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LT:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  }

  printf("  push rax\n");
}
