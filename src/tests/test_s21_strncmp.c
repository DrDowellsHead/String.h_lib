#include "test_s21_strncmp.h"

#include <stdlib.h>
#include <string.h>

#include "../s21_string.h"

/* ========== РЕАЛИЗАЦИИ ТЕСТОВ ========== */
START_TEST(test_strncmp_equal) {
  ck_assert_uint_eq(s21_strncmp("hello", "hello", 5),
                    strncmp("hello", "hello", 5));
}
END_TEST

START_TEST(test_strncmp_less) {
  ck_assert_uint_eq(s21_strncmp("apple", "banana", 5),
                    strncmp("apple", "banana", 5));
}
END_TEST

START_TEST(test_strncmp_greater) {
  ck_assert_uint_eq(s21_strncmp("banana", "apple", 5),
                    strncmp("banana", "apple", 5));
}
END_TEST

START_TEST(test_strncmp_zero) {
  ck_assert_uint_eq(s21_strncmp("hello", "world", 0),
                    strncmp("hello", "world", 0));
}
END_TEST

START_TEST(test_strncmp_partital) {
  ck_assert_uint_eq(s21_strncmp("hello", "help", 3),
                    strncmp("hello", "help", 3));

  ck_assert_uint_eq(s21_strncmp("hello", "help", 4),
                    strncmp("hello", "help", 4));
}
END_TEST

/* ========== СОЗДАНИЕ ТЕСТОВОГО НАБОРА ========== */
Suite *strncmp_suite(void) {
  Suite *s = suite_create("s21_strncmp");
  TCase *tc = tcase_create("Core");

  tcase_add_test(tc, test_strncmp_equal);
  tcase_add_test(tc, test_strncmp_less);
  tcase_add_test(tc, test_strncmp_greater);
  tcase_add_test(tc, test_strncmp_zero);
  tcase_add_test(tc, test_strncmp_partital);

  suite_add_tcase(s, tc);
  return s;
}