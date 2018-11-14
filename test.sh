#!/bin/bash
try() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  gcc -static -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" == "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$expected expected, but got $actual"
    exit 1
  fi
}

try_fail() {
  input="$1"

  ./9cc "$input" > /dev/null 2>/dev/null
  result="$?"

  if [ "$result" == "0" ]; then
    echo "not failed: $input"
    exit 1
  else
    echo "$input failed as expected."
  fi
}

try 42 '42;'
try 41 ' 12 + 34 - 5 ;'
try 52 ' 2 + 10 * 5 ;'
try 3 ' 1 + 10 /5 ;'
try 56 ' 2*(3 +(2+3) *5);'
try 4 'a = b = 2;a + b;'

try_fail '();'
try_fail '1 + (2 + );'
try_fail '1 1;'

echo OK
