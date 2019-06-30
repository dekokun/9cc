#include "9cc.h"
#include <stdio.h>
#include <stdlib.h>

void expect_check(int line, int expected, int actual) {
  if (expected == actual)
    return;
  fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
  exit(1);
}

void test_vector() {
  Vector *vec = new_vector();
  expect_check(__LINE__, 0, vec->len);

  for (int i = 0; i < 100; i++)
    vec_push(vec, (void *)i);

  expect_check(__LINE__, 100, vec->len);
  expect_check(__LINE__, 0, (int)vec->data[0]);
  expect_check(__LINE__, 50, (int)vec->data[50]);
  expect_check(__LINE__, 99, (int)vec->data[99]);
}

void test_map() {
  Map *map = new_map();
  expect_check(__LINE__, 0, (int)map_get(map, "foo"));
  expect_check(__LINE__, 0, map_len(map));

  map_put(map, "foo", (void *)2);
  expect_check(__LINE__, 2, (int)map_get(map, "foo"));
  expect_check(__LINE__, 1, map_len(map));

  map_put(map, "bar", (void *)4);
  expect_check(__LINE__, 4, (int)map_get(map, "bar"));
  expect_check(__LINE__, 2, map_len(map));

  map_put(map, "foo", (void *)6);
  expect_check(__LINE__, 6, (int)map_get(map, "foo"));
  expect_check(__LINE__, 3, map_len(map));
}

void runtest() {
  test_vector();
  test_map();
  printf("OK\n");
}
