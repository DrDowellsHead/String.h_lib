#include "test_s21_insert.h"

#include "../s21_string.h"

START_TEST(test_insert_nulls) {
    ck_assert_ptr_eq(s21_insert(S21_NULL, "x", s21_size_make(0)), S21_NULL);
    ck_assert_ptr_eq(s21_insert("x", S21_NULL, s21_size_make(0)), S21_NULL);
}
END_TEST

START_TEST(test_insert_begin) {
    char *out = (char *)s21_insert("world", "hello", s21_size_make(0));
    ck_assert_ptr_ne(out, S21_NULL);
    ck_assert_str_eq(out, "helloworld");
    free(out);
}
END_TEST

START_TEST(test_insert_middle) {
    char *out = (char *)s21_insert("heo", "ll", s21_size_make(2));
    ck_assert_ptr_ne(out, S21_NULL);
    ck_assert_str_eq(out, "hello");
    free(out);
}
END_TEST

START_TEST(test_insert_end) {
    char *out = (char *)s21_insert("hi", "!", s21_size_make(2));
    ck_assert_ptr_ne(out, S21_NULL);
    ck_assert_str_eq(out, "hi!");
    free(out);
}
END_TEST

START_TEST(test_insert_bad_index) {
    ck_assert_ptr_eq(s21_insert("hi", "x", s21_size_make(3)), S21_NULL);
}
END_TEST

Suite *insert_suite() {
    Suite *s = suite_create("s21_insert");
    TCase *tc = tcase_create("core");

    tcase_add_test(tc, test_insert_nulls);
    tcase_add_test(tc, test_insert_begin);
    tcase_add_test(tc, test_insert_middle);
    tcase_add_test(tc, test_insert_end);
    tcase_add_test(tc, test_insert_bad_index);

    suite_add_tcase(s, tc);
    return s;
}