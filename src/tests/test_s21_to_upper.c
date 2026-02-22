#include "test_s21_to_upper.h"

#include "../s21_string.h"

START_TEST(test_to_upper_null) {
  ck_assert_ptr_eq(s21_to_upper(S21_NULL), S21_NULL);
}
END_TEST

START_TEST(test_to_upper_basic) {
  const char *in = "aBz 123!?";
  char *out = (char *)s21_to_upper(in);
  ck_assert_ptr_ne(out, S21_NULL);
  ck_assert_str_eq(out, "ABZ 123!?");
  free(out);
}
END_TEST

START_TEST(test_to_upper_empty) {
  char *out = (char *)s21_to_upper("");
  ck_assert_ptr_ne(out, S21_NULL);
  ck_assert_str_eq(out, "");
  free(out);
}
END_TEST

Suite *to_upper_suite() {
  Suite *s = suite_create("s21_to_upper");
  TCase *tc = tcase_create("core");

  tcase_add_test(tc, test_to_upper_null);
  tcase_add_test(tc, test_to_upper_basic);
  tcase_add_test(tc, test_to_upper_empty);

  suite_add_tcase(s, tc);
  return s;
}