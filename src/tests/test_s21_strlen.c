#include "test_s21_strlen.h"

#include <stdlib.h>
#include <string.h>

#include "../s21_string.h"

/* ========== РЕАЛИЗАЦИИ ТЕСТОВ ========== */

START_TEST(test_empty_string) { ck_assert_uint_eq(s21_strlen(""), strlen("")); }
END_TEST

START_TEST(test_basic_string) {
  ck_assert_uint_eq(s21_strlen("hello"), strlen("hello"));
  ck_assert_uint_eq(s21_strlen("world"), strlen("world"));
}
END_TEST

START_TEST(test_strlen_spaces) {
  ck_assert_uint_eq(s21_strlen("hello world"), strlen("hello world"));
  ck_assert_uint_eq(s21_strlen(" "), strlen(" "));
}
END_TEST

START_TEST(test_single_char) {
  ck_assert_uint_eq(s21_strlen("A"), strlen("A"));
}
END_TEST

START_TEST(test_null_pointer) { ck_assert_uint_eq(s21_strlen(NULL), 0); }
END_TEST

START_TEST(test_long_string) {
  char *str = create_long_string(999);
  ck_assert_uint_eq(s21_strlen(str), strlen(str));
  free_long_string(str);
}
END_TEST

START_TEST(test_special_chars) {
  ck_assert_uint_eq(s21_strlen("!@#$%^&*()"), strlen("!@#$%^&*()"));
  ck_assert_uint_eq(s21_strlen("\t\n\r"), strlen("\t\n\r"));
}
END_TEST

START_TEST(test_mixed_content) {
  ck_assert_uint_eq(s21_strlen("Hello\x00World"), strlen("Hello\x00World"));
}
END_TEST

/* ========== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ========== */

char *create_long_string(size_t length) {
  char *str = malloc(length + 1);
  if (str) {
    memset(str, 'X', length);
    str[length] = '\0';
  }
  return str;
}

void free_long_string(char *str) { free(str); }

/* ========== СОЗДАНИЕ ТЕСТОВОГО НАБОРА ========== */

Suite *strlen_suite(void) {
  Suite *s = suite_create("s21_strlen");
  TCase *tc = tcase_create("Core");

  tcase_add_test(tc, test_empty_string);
  tcase_add_test(tc, test_basic_string);
  tcase_add_test(tc, test_single_char);
  tcase_add_test(tc, test_null_pointer);
  tcase_add_test(tc, test_strlen_spaces);
  tcase_add_test(tc, test_long_string);
  tcase_add_test(tc, test_special_chars);
  tcase_add_test(tc, test_mixed_content);

  suite_add_tcase(s, tc);
  return s;
}
