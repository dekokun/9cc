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

try 0 0
try 42 42
try 3 '1+2'
try 101 '2+2+100-3'
try 41 ' 12 + 34 - 5 '
try 50 ' 10 * 5 '
try 52 ' 2 + 10 * 5 '
try 2 ' 10 /5 '
try 56 ' 2*(3 +(2+3) *5)'

echo OK
