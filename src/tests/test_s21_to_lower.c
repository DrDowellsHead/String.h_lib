#include "test_s21_to_lower.h"

#include "../s21_string.h"

START_TEST(test_to_lower_null) {
  ck_assert_ptr_eq(s21_to_lower(S21_NULL), S21_NULL);
}
END_TEST

START_TEST(test_to_lower_basic) {
  const char *in = "AbZ 123!?";
  char *out = (char *)s21_to_lower(in);
  ck_assert_ptr_ne(out, S21_NULL);
  ck_assert_str_eq(out, "abz 123!?");
  free(out);
}
END_TEST

START_TEST(test_to_lower_empty) {
  char *out = (char *)s21_to_lower("");
  ck_assert_ptr_ne(out, S21_NULL);
  ck_assert_str_eq(out, "");
  free(out);
}
END_TEST

Suite *to_lower_suite() {
  Suite *s = suite_create("s21_to_lower");
  TCase *tc = tcase_create("core");

  tcase_add_test(tc, test_to_lower_null);
  tcase_add_test(tc, test_to_lower_basic);
  tcase_add_test(tc, test_to_lower_empty);

  suite_add_tcase(s, tc);
  return s;
}