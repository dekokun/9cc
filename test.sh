#!/bin/bash

verbose=
debug=

case "$1" in
-v | --v | --ve | --ver | --verb | --verbo | --verbos | --verbose)
  verbose=1
  shift
  ;;
-d | --d | --debug | -debug)
  debug=1
  shift
  ;;
esac

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'
try() {
  expected="$1"
  input="$2"
    if [ "$debug" = 1 ]; then
        ./9cc -debug "$input" >tmp.s
    else
        ./9cc "$input" >tmp.s
    fi
  compile_result="$?"
  if [ $compile_result != 0 ]; then
      echo -e "${RED}compile failed{$NC}"
      exit
  fi
  gcc -static -o tmp tmp.s test/*.o
  ./tmp
  actual="$?"

  if [ "$actual" == "$expected" ]; then
    echo -e "$GREEN$input => $actual$NC"
  else
    echo -e "$RED$input => $expected expected, but got $actual$NC"
    exit 1
  fi
}

try_fail() {
  input="$1"

  output=$(./9cc "$input" 2>&1)
  result="$?"

  if [ "$result" == "0" ]; then
    echo -e "${RED}not failed: $input.$NC"
    if [ "$verbose" = 1 ]; then
      echo output:
      echo "$output"
    fi
    exit 1
  else
    echo -e "$GREEN$input failed as expected.$NC"
    if [ "$verbose" = 1 ]; then
      echo output:
      echo "$output"
    fi
  fi
}

try 6 'foo() {a = 1; b = 5; return a + b;} main() {return foo();}'
try 5 'foo() {return 2;} main() {return foo() + foo() + 1;}'
try 6 'main() {return 6;}'
try 7 'main() {a = foo_1(2); b = foo_2(2, 3); return a + b;}'
try 2 'main() {return foo_1(2);}'
try 5 'main() {return foo_2(2, 3);}'
try 8 'main() {return foo_3(2, 3, 3);}'
try 12 'main() {return foo_4(2, 3, 3, 4);}'
try 17 'main() {return foo_5(2, 3, 3, 4, 5);}'
try 18 'main() {return foo_6(2, 3, 3, 4, 5, 1);}'
try 1 'main() {return foo();}'
try 28 'main() {a1 = 1;a2 = 1;a3 = 1;a4 = 1;a5 = 1;a6 = 1;a7 = 1;a8 = 1;a9 = 1;a10 = 1;a11 = 1;a12 = 1;a13 = 1;a14 = 1;a15 = 1;a16 = 1;a17 = 1;a18 = 1;a19 = 1;a20 = 1;a21 = 1;a22 = 1;a23 = 1;a24 = 1;a25 = 1;a26 = 1;a27 = 1;a28 = 1;return a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9 + a10 + a11 + a12 + a13 + a14 + a15 + a16 + a17 + a18 + a19 + a20 + a21 + a22 + a23 + a24 + a25 + a26 + a27 + a28;}'
try 5 'main() {{return 5;}}'
try 5 'main() {{a= 5;return a;}}'
try 3 'main() {a = 1; if (a > 5) {return 1;} else {a = a + 1; return a + 1;}}'
try 5 'main() {for (i = 0; i < 5; i = i + 1) i;return i;}'
try 5 'main() {i = 0; for (;i < 5;) i = i + 1;return i;}'
try 6 'main() {for (i = 0; ;i = i + 1) if (i > 5) return i;}'
try 6 'main() {for (i = 0; ;) if ((i = i + 1) > 5) return i;}'
try 4 'main() {a = 0; while (a <= 3) a = a + 1;return a;}'
try 3 'main() {a = 2; if (1 == 1) a = 3; else a = 4; return a;}'
try 4 'main() {a = 2; if (1 != 1) a = 3; else a = 4; return a;}'
try 4 'main() {a = 2; if (1 == 1) if (1 == 1) a = 4; return a;}'
try 3 'main() {a = 2; if (1 == 1) a = 3; if (1 != 1) a = 4; return a;}'
try 3 'main() {a = 2; if (1 == 1) a = 3; return a;}'
try 2 'main() {a = 2; if (1 != 1) a = 3; return a;}'
try 0 'main() {_foo = 0; return _foo;}'
try 6 'main() {foo = 1; bar = 2 + 3; return foo + bar;}'
try 6 'main() {foo = 1; far = 2 + 3; return foo + far;}'
## try 0 'return foo + bar};'
try 2 'main() {a = 2; b = 3; return a;}'
try 1 'main() {a = -1; b = -1; return a + b + 3;}'
try 19 'main() {a = 8; b = 5 * 6 - 8; return a + b / 2;}'
try 5 'main() {return 5;return 6;}'
try 42 'main() {42;}'
try 41 'main() { 12 + 34 - 5 ;}'
try 52 'main() { 2 + 10 * 5 ;}'
try 3 'main() { 1 + 10 /5 ;}'
try 56 'main() { 2*(3 +(2+3) *5);}'
try 4 'main() {a = b = 2;a + b;}'
try 0 'main() {-42 + 42;}'
try 10 'main() {-10 * 5 + 60;}'
try 1 'main() {5 == 5;}'
try 0 'main() {5 != 5;}'
try 1 'main() {3 <= 5;}'
try 0 'main() {5 <= 3;}'
try 1 'main() {5 >= 3;}'
try 0 'main() {3 >= 5;}'
try 1 'main() {3 >= 3;}'
try 1 'main() {3 <= 3;}'
try 0 'main() {3 < 3;}'
try 1 'main() {2 < 3;}'
try 0 'main() {3 > 3;}'
try 1 'main() {3 > 2;}'

try_fail 'main() {();}'
try_fail 'main() {if  hoge;}'
try_fail 'main() {if (hoge;}'
try_fail 'main() {1 + (2 + );}'
try_fail 'main() {1 1;}'
try_fail 'main() {1 = 1;}'
try_fail 'main() {(a + 1) = 1;}'
try_fail 'main() {return foo_2(2, );}'

echo OK
