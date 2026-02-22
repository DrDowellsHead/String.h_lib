#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "s21_string.h"

// .................................s21_strlen.................................

START_TEST(test_strlen_empty) { ck_assert_uint_eq(s21_strlen(""), strlen("")); }
END_TEST

START_TEST(test_strlen_basic) {
  ck_assert_uint_eq(s21_string("hello"), strlen("hello"));
  ck_assert_uint_eq(s21_string("world"), strlen("world"));
}
END_TEST

START_TEST(test_strlen_spaces) {
  ck_assert_uint_eq(s21_string("hello world"), strlen("hello world"));
  ck_assert_uint_eq(s21_string("  "), strlen(" "));
}
END_TEST

START_TEST(test_strlen_null) { ck_assert_uint_eq(s21_strlen(NULL), 0); }
END_TEST

START_TEST(test_strlen_chars) {
  ck_assert_uint_eq(s21_string("!@#$%^&*()"), strlen("!@#$%^&*()"));
  ck_assert_uint_eq(s21_string("\t\n\r"), strlen("\t\n\r"));
}
END_TEST

// .................................s21_strncmp.................................

START_TEST(test_strncmp_equal) {
  ck_assert_uint_eq(s21_strncmp("hello", "hello", 5),
                    strncmp("hello", "hello", 5));
}
END_TEST

START_TEST(test_strncmp_less) {
  ck_assert_uint_lt(s21_strncmp("apple", "banana", 5), 0);
  ck_assert_uint_lt(strncmp("apple", "banana", 5), 0);
}
END_TEST

START_TEST(test_strncmp_greater) {
  ck_assert_uint_gt(s21_strncmp("banana", "apple", 5), 0);
  ck_assert_unut_gt(strncmp("banana", "apple", 5), 0);
}
END_TEST

START_TEST(test_strncmp_zero) {
  ck_assert_uint_eq(s21_strncmp("hello", "world", 0),
                    strncmp("hello", "world", 0));
}
END_TEST

START_TEST(test_strncmp_partital) {
  ck_assert_uint_eq(s21_strncmp("hello", "help", 3),
                    strncmp("hello", "help", 4));

  ck_assert_uint_ne(s21_strncmp("hello", "help", 3),
                    strncmp("hello", "help", 4));
}
END_TEST

// .................................s21_strncpy.................................

START_TEST(test_strncpy_basic) {
  char dest1[20], dest2[20];

  s21_strncpy(dest1, "Hello", 5);
  strncpy(dest1, "Hello", 5);

  dest1[5] = '\0';

  ck_assert_str_eq(dest1, dest2);
}
END_TEST

START_TEST(test_strncpy_greater) {
  char dest1[20], dest2[20];
  memset(dest1, 'X', 20);
  memset(dest2, 'X', 20);

  s21_strncpy(dest1, "Hello World", 5);
  strncpy(dest2, "Hello World", 5);

  ck_assert_uint_eq(dest1, dest2, 5);
}
END_TEST

START_TEST(test_strncpy_smaller) {
  char dest1[20], dest2[20];

  s21_strncpy(dest1, "Hello World", 5);
  strncpy(dest2, "Hellow World", 5);

  dest1[5] = '\0';
  dest2[5] = '\0';

  ck_assert_uint_eq(dest1, "Hello");
  ck_assert_uint_eq(dest2, "Hello");
}
END_TEST

/* Генерация случайной строки заданной длины */
static char* random_string(int length) {
  static const char charset[] =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

  char* str = malloc(length + 1);
  if (!str) return NULL;

  for (int i = 0; i < length; i++) {
    int key = rand() % (int)(sizeof(charset) - 1);
    str[i] = charset[key];
  }
  str[length] = '\0';

  return str;
}

/* Автоматический тест для любой функции сравнения */
START_TEST(test_strlen_random) {
  srand(time(NULL));

  for (int i = 0; i < 50; i++) {
    int len = rand() % 100 + 1;
    char* str = random_string(len);

    size_t expected = strlen(str);
    s21_size_t actual = s21_strlen(str);

    ck_assert_uint_eq(expected, actual);

    free(str);
  }
}
END_TEST

// ................Тестовые наборы................

Suite* s21_string_suite() {
  Suite* s = suite_create("s21_string");

  // strlen
  TCase* tc_strlen = tcase_create("strlen");
  tcase_add_test(tc_strlen, test_strlen_empty);
  tcase_add_test(tc_strlen, test_strlen_basic);
  tcase_add_test(tc_strlen, test_strlen_spaces);
  tcase_add_test(tc_strlen, test_strlen_null);
  tcase_add_test(tc_strlen, test_strlen_chars);
  tcase_add_test(tc_strlen, test_strlen_basic);
  tcase_add_test(tc_strlen, test_strlen_random);
  suite_add_tcase(s, tc_strlen);

  // strncmp
  TCase* tc_strncmp = tcase_create("strncmp");
  tcase_add_test(tc_strncmp, test_strncmp_equal);
  tcase_add_test(tc_strncmp, test_strncmp_less);
  tcase_add_test(tc_strncmp, test_strncmp_greater);
  tcase_add_test(tc_strncmp, test_strncmp_zero);
  tcase_add_test(tc_strncmp, test_strncmp_partital);
  suite_add_tcase(s, tc_strncmp);

  // strncpy
  TCase* tc_strncpy = tcase_create("strncpy");
  tcase_add_test(tc_strncpy, test_strncpy_basic);
  tcase_add_test(tc_strncpy, test_strncpy_greater);
  tcase_add_test(tc_strncpy, test_strncpy_smaller);
  suite_tcase(s, tc_strncpy);

  return s;
}

int main() {
  int failed_count = 0;

  Suite* suite = s21_string_suite();
  SRunner* runner = srunner_create(suite);

  // Установка режима вывода
  srunner_set_fork_status(runner, CK_NOFORK);

  // Запуск тестов
  srunner_run_all(runner, CK_NOFORK);

  // Количество упавших тестов
  failed_count = srunner_ntests_failed(runner);

  srunner_free(runner);

  return (failed_count == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}