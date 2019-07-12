#define _XOPEN_SOURCE 700
// トークンの型を表す値
enum {
  TK_NUM = 256, // 整数
  TK_IDENT,     // 識別子
  TK_RETURN,    // return
  TK_EOF,       // 入力の終わりを表すトークン
  TK_EQ,        // equal
  TK_NE,        // not equal
  TK_LE,        // less than or equal
  TK_GE,        // greater than or equal
  TK_LT,        // less than
  TK_GT,        // greater than
  TK_IF,        // if
  TK_ELSE,      // else
  TK_WHILE,     // while
  TK_FOR,       // for
};

typedef struct Token Token;
// トークンの型
struct Token {
  int ty; // トークンの型
  Token *next;
  int val;     // tyがTK_NUMの時、その数値
  char *name;  // tyがTK_IDENTの場合、その名前
  char *input; // トークン文字列(エラーメッセージ用)
};

// current token
Token *token;

enum {
  ND_NUM = 256,
  ND_LVAR,
  ND_EQ,        // equal
  ND_NE,        // not equal
  ND_LE,        // less than or equal
  ND_LT,        // less than
  ND_RETURN,    // return
  ND_IF,        // if
  ND_IF_ELSE,   // if - else
  ND_WHILE,     // while
  ND_FOR,       // for
  ND_BLOCK,     // compound statement(block)
  ND_FUNC_CALL, // function call
};

typedef struct {
  void **data;
  int capacity;
  int len;
} Vector;

typedef struct {
  Vector *keys;
  Vector *vals;
} Map;

typedef struct Node { // 宣言の中でNodeを使ってるのでタグ名Nodeが必要
  int ty;
  struct Node *lhs;
  struct Node *rhs;
  int val;           // tyがND_NUMの場合のみ使う
  char *name;        // tyがND_LVAR, ND_FUNC_CALLの場合のみ使う
  struct Node *cond; // tyがND_IF/ND_IF_ELSE/ND_WHILE/ND_IFの場合のみ使う
  struct Node *then; // tyがND_IF/ND_IF_ELSE/ND_WHILE/ND_FORの場合のみ使う
  struct Node *els;       // tyがND_IF_ELSEの場合のみ使う
  struct Node *init;      // tyがND_FORの場合のみ使う
  struct Node *iter_expr; // tyがND_FORの場合のみ使う
  Vector *statements;     // tyがND_BLOCKの場合
  Vector *arguments;      // tyがND_FUNC_CALLの場合
  int offset;
} Node;

typedef struct {
  char *name;
  Map *var_map;
  Vector *statements;
} Function;

typedef struct LVar LVar;
struct LVar {
  LVar *next; // 次の変数かNULL
  char *name; // 変数の名前
  int offset; // RBPからのオフセット
};

LVar *locals;

// 名前をfunctionsに
extern Function *code[];
extern Map *ident_map;

void gen_func();
void program();
__attribute__((noreturn)) void error(char *fmt, ...);
__attribute__((noreturn)) void error_at(char *loc, char *fmt, ...);
void logging(char *fmt, ...);

void runtest();
Vector *new_vector();
void vec_push(Vector *vec, void *elem);
Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);
int map_len(Map *map);