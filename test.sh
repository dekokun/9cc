#!/bin/bash

verbose=

case "$1" in
-v|--v|--ve|--ver|--verb|--verbo|--verbos|--verbose)
    verbose=1
    shift ;;
esac

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'
try() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  gcc -static -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" == "$expected" ]; then
    echo -e $GREEN"$input => $actual"$NC
  else
    echo -e $RED"$expected expected, but got $actual"$NC
    exit 1
  fi
}

try_fail() {
  input="$1"

  output=$(./9cc "$input" 2>&1)
  result="$?"

  if [ "$result" == "0" ]; then
    echo -e $RED"not failed: $input."$NC
    if [ "$verbose" = 1 ]; then
        echo output: $output
    fi
    exit 1
  else
    echo -e $GREEN"$input failed as expected."$NC
    if [ "$verbose" = 1 ]; then
        echo output: $output
    fi
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
