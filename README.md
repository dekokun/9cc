# 9cc

[![Build Status](https://travis-ci.org/dekokun/9cc.svg?branch=master)](https://travis-ci.org/dekokun/9cc)

C言語のコンパイラです。
[低レイヤを知りたい人のための Cコンパイラ作成入門](https://www.sigbus.info/compilerbook/) を参考に作成しています

## EBNF

```ebnf
program    = function*
function   = ident "(" ")" "{" stmt "}"
stmt       = expr ";"
           | "return" expr ";"
           | "if" "(" expr ")" stmt ("else" stmt)?
           | "while" "(" expr ")" stmt
           | "for" "(" expr? ";" expr? ";" expr? ")" stmt
           | "{" stmt* "}"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? term
term       = num
           | ident
           | ident("(" arguments ")")?
           | "(" expr ")"
arguments  = ε | non_empty_arguments
non_empty_arguments = term | non_empty_arguments
```
