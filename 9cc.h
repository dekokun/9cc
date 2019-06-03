// トークンの型を表す値
enum {
  TK_NUM = 256, // 整数
  TK_IDENT,     // 識別子
  TK_EOF,       // 入力の終わりを表すトークン
};

// トークンの型
typedef struct {
  int ty;      // トークンの型
  int val;     // tyがTK_NUMの時、その数値
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
  int val;   // tyがND_NUMの場合のみ使う
  char name; // tyがND_IDENTの場合のみ使う
} Node;

typedef struct {
  void **data;
  int capacity;
  int len;
} Vector;

typedef struct {
  Vector *keys;
  Vector *vals;
} Map;

extern Token tokens[];
extern Node *code[];

void gen();
void program();
__attribute__((noreturn)) void error(char *fmt, ...);

void runtest();
Vector *new_vector();
void vec_push(Vector *vec, void *elem);
Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);