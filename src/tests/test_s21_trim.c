#include "test_s21_trim.h"

#include <stdlib.h>  // free

#include "../s21_string.h"

START_TEST(test_s21_trim_null_src) {
    ck_assert_ptr_eq(s21_trim(S21_NULL, " "), S21_NULL);
}
END_TEST

START_TEST(test_s21_trim_null_trim_chars) {
    ck_assert_ptr_eq(s21_trim(" hi ", S21_NULL), S21_NULL);
}
END_TEST

START_TEST(test_s21_trim_empty_trim_chars_returns_copy) {
    char *out = (char *)s21_trim("xxHelloxx", "");
    ck_assert_ptr_ne(out, S21_NULL);
    ck_assert_str_eq(out, "xxHelloxx");
    free(out);
}
END_TEST

START_TEST(test_s21_trim_basic_one_char) {
    char *out = (char *)s21_trim("xxxHello Worldxxx", "x");
    ck_assert_ptr_ne(out, S21_NULL);
    ck_assert_str_eq(out, "Hello World");
    free(out);
}
END_TEST

START_TEST(test_s21_trim_spaces) {
    char *out = (char *)s21_trim("   Hi   ", " ");
    ck_assert_ptr_ne(out, S21_NULL);
    ck_assert_str_eq(out, "Hi");
    free(out);
}
END_TEST

START_TEST(test_s21_trim_multi_chars) {
    char *out = (char *)s21_trim("\n\t Hi \r\n", " \t\n\r");
    ck_assert_ptr_ne(out, S21_NULL);
    ck_assert_str_eq(out, "Hi");
    free(out);
}
END_TEST

START_TEST(test_s21_trim_all_removed) {
    char *out = (char *)s21_trim("xxxx", "x");
    ck_assert_ptr_ne(out, S21_NULL);
    ck_assert_str_eq(out, "");
    free(out);
}
END_TEST

START_TEST(test_s21_trim_nothing_to_trim) {
    char *out = (char *)s21_trim("Hello", "x");
    ck_assert_ptr_ne(out, S21_NULL);
    ck_assert_str_eq(out, "Hello");
    free(out);
}
END_TEST

Suite *trim_suite() {
    Suite *s = suite_create("s21_trim");
    TCase *tc = tcase_create("core");

    tcase_add_test(tc, test_s21_trim_null_src);
    tcase_add_test(tc, test_s21_trim_null_trim_chars);
    tcase_add_test(tc, test_s21_trim_empty_trim_chars_returns_copy);
    tcase_add_test(tc, test_s21_trim_basic_one_char);
    tcase_add_test(tc, test_s21_trim_spaces);
    tcase_add_test(tc, test_s21_trim_multi_chars);
    tcase_add_test(tc, test_s21_trim_all_removed);
    tcase_add_test(tc, test_s21_trim_nothing_to_trim);

    suite_add_tcase(s, tc);
    return s;
}