#include "test_s21_strcspn.h"

#include "../s21_string.h"

// Тест 1: Нормальный случай - поиск символов из набора
START_TEST(test_s21_strcspn_normal) {
  const char *str1 = "Hello, World!";
  const char *str2 = " ,!";

  __size_internal s21_result = s21_strcspn(str1, str2);
  size_t orig_result = strcspn(str1, str2);

  ck_assert_uint_eq(s21_result, orig_result);
}
END_TEST

// Тест 2: Нет совпадений - возвращается длина всей строки
START_TEST(test_s21_strcspn_no_match) {
  const char *str1 = "Hello";
  const char *str2 = "xyz";

  __size_internal s21_result = s21_strcspn(str1, str2);
  size_t orig_result = strcspn(str1, str2);

  ck_assert_uint_eq(s21_result, orig_result);
}
END_TEST

// Тест 3: Совпадение в начале строки - возвращается 0
START_TEST(test_s21_strcspn_match_at_start) {
  const char *str1 = "Hello";
  const char *str2 = "H";

  __size_internal s21_result = s21_strcspn(str1, str2);
  size_t orig_result = strcspn(str1, str2);

  ck_assert_uint_eq(s21_result, orig_result);
}
END_TEST

// Тест 4: Совпадение в конце строки
START_TEST(test_s21_strcspn_match_at_end) {
  const char *str1 = "abcdef123456";
  const char *str2 = "6";

  __size_internal s21_result = s21_strcspn(str1, str2);
  size_t orig_result = strcspn(str1, str2);

  ck_assert_uint_eq(s21_result, orig_result);
}
END_TEST

// Тест 5: Совпадение в середине строки
START_TEST(test_s21_strcspn_match_at_middle) {
  const char *str1 = "abcdef123456";
  const char *str2 = "f1";

  __size_internal s21_result = s21_strcspn(str1, str2);
  size_t orig_result = strcspn(str1, str2);

  ck_assert_uint_eq(s21_result, orig_result);
}
END_TEST

// Тест 6: Регистрозависимость
START_TEST(test_s21_strcspn_case_sensitive) {
  const char *str1 = "Hello World";
  const char *str2 = "hw";

  __size_internal s21_result = s21_strcspn(str1, str2);
  size_t orig_result = strcspn(str1, str2);

  ck_assert_uint_eq(s21_result, orig_result);
}
END_TEST

/* ============= ГРАНИЧНЫЕ ТЕСТЫ ============= */

// Тест 7: Пустые строки
START_TEST(test_s21_strcspn_empty_strings) {
  // Пустой str2
  const char *str1 = "Hello";
  const char *str2_empty = "";
  __size_internal s21_result1 = s21_strcspn(str1, str2_empty);
  size_t orig_result1 = strcspn(str1, str2_empty);
  ck_assert_uint_eq(s21_result1, orig_result1);

  // Пустой str1
  const char *str1_empty = "";
  const char *str2 = "abc";
  __size_internal s21_result2 = s21_strcspn(str1_empty, str2);
  size_t orig_result2 = strcspn(str1_empty, str2);
  ck_assert_uint_eq(s21_result2, orig_result2);

  // Обе пустые
  __size_internal s21_result3 = s21_strcspn(str1_empty, str2_empty);
  size_t orig_result3 = strcspn(str1_empty, str2_empty);
  ck_assert_uint_eq(s21_result3, orig_result3);
}
END_TEST

// Тест 8: Длинные строки и специальные символы
START_TEST(test_s21_strcspn_long_and_special) {
  // Длинная строка без совпадений
  char long_str[1000];
  for (int i = 0; i < 999; i++) {
    long_str[i] = 'a' + (i % 26);
  }
  long_str[999] = '\0';

  __size_internal s21_result1 = s21_strcspn(long_str, "Zz");
  size_t orig_result1 = strcspn(long_str, "Zz");
  ck_assert_uint_eq(s21_result1, orig_result1);

  // Специальные символы
  const char *str_special = "Hello\n\tWorld\r";
  const char *str2_special = "\n\t\r";
  __size_internal s21_result2 = s21_strcspn(str_special, str2_special);
  size_t orig_result2 = strcspn(str_special, str2_special);
  ck_assert_uint_eq(s21_result2, orig_result2);
}
END_TEST

// Тест 9: Крайние случаи с символами
START_TEST(test_s21_strcspn_edge_chars) {
  // Все символы совпадают
  const char *str1_all = "abcde";
  const char *str2_all = "edcba";
  __size_internal s21_result1 = s21_strcspn(str1_all, str2_all);
  size_t orig_result1 = strcspn(str1_all, str2_all);
  ck_assert_uint_eq(s21_result1, orig_result1);

  // Нулевой символ в середине
  char str1_null[] = "abc\0def";
  const char *str2_null = "d";
  __size_internal s21_result2 = s21_strcspn(str1_null, str2_null);
  size_t orig_result2 = strcspn(str1_null, str2_null);
  ck_assert_uint_eq(s21_result2, orig_result2);

  // Кириллица
  const char *str1_cyr = "Привет, Мир!";
  const char *str2_cyr = "М,";
  __size_internal s21_result3 = s21_strcspn(str1_cyr, str2_cyr);
  size_t orig_result3 = strcspn(str1_cyr, str2_cyr);
  ck_assert_uint_eq(s21_result3, orig_result3);
}
END_TEST

Suite *strcspn_suite(void) {
  Suite *s = suite_create("s21_strcspn");
  TCase *tc_core = tcase_create("Core");
  TCase *tc_boundary = tcase_create("Boundary");

  // Основные тесты
  tcase_add_test(tc_core, test_s21_strcspn_normal);
  tcase_add_test(tc_core, test_s21_strcspn_no_match);
  tcase_add_test(tc_core, test_s21_strcspn_match_at_start);
  tcase_add_test(tc_core, test_s21_strcspn_match_at_end);
  tcase_add_test(tc_core, test_s21_strcspn_match_at_middle);
  tcase_add_test(tc_core, test_s21_strcspn_case_sensitive);
  suite_add_tcase(s, tc_core);

  // Граничные случаи
  tcase_add_test(tc_boundary, test_s21_strcspn_empty_strings);
  tcase_add_test(tc_boundary, test_s21_strcspn_long_and_special);
  tcase_add_test(tc_boundary, test_s21_strcspn_edge_chars);
  suite_add_tcase(s, tc_boundary);

  return s;
}