#include "test_s21_sscanf.h"

#include <stdint.h>
#include <stdio.h>

#include "../s21_string.h"

static void assert_cstr_eq(const char *a, const char *b) {
  int i = 0;
  while (a[i] != '\0' || b[i] != '\0') {
    ck_assert_msg(a[i] == b[i], "Строки различаются в %d: '%c' и '%c'", i, a[i],
                  b[i]);
    i++;
  }
}

static int close_double(double x, double y, double eps) {
  double d = x - y;
  if (d < 0) d = -d;
  return d <= eps;
}

START_TEST(test_s21_sscanf_d_basic) {
  int a1 = 0, a2 = 0;
  int r1 = s21_sscanf("  -12  ", "%d", &a1);
  int r2 = sscanf("  -12  ", "%d", &a2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);
}
END_TEST

START_TEST(test_s21_sscanf_i_autobase) {
  int a1 = 0, a2 = 0;
  int b1 = 0, b2 = 0;
  int c1 = 0, c2 = 0;

  int r1 = s21_sscanf("010 0x10 10", "%i %i %i", &a1, &b1, &c1);
  int r2 = sscanf("010 0x10 10", "%i %i %i", &a2, &b2, &c2);

  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);
  ck_assert_int_eq(b1, b2);
  ck_assert_int_eq(c1, c2);
}
END_TEST

START_TEST(test_s21_sscanf_uox) {
  unsigned int u1 = 0, u2 = 0;
  unsigned int o1 = 0, o2 = 0;
  unsigned int x1 = 0, x2 = 0;

  int r1 = s21_sscanf("255 377 ff", "%u %o %x", &u1, &o1, &x1);
  int r2 = sscanf("255 377 ff", "%u %o %x", &u2, &o2, &x2);

  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(u1, u2);
  ck_assert_int_eq(o1, o2);
  ck_assert_int_eq(x1, x2);
}
END_TEST

START_TEST(test_s21_sscanf_percent_literal) {
  int a1 = 0, a2 = 0;
  int r1 = s21_sscanf("100%", "%d%%", &a1);
  int r2 = sscanf("100%", "%d%%", &a2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);
}
END_TEST

START_TEST(test_s21_sscanf_s_basic_width) {
  char s1[32] = {0};
  char s2[32] = {0};
  char w1[32] = {0};
  char w2[32] = {0};

  int r1 = s21_sscanf("hello world", "%s", s1);
  int r2 = sscanf("hello world", "%s", s2);
  ck_assert_int_eq(r1, r2);
  assert_cstr_eq(s1, s2);

  int r3 = s21_sscanf("abcdef", "%3s", w1);
  int r4 = sscanf("abcdef", "%3s", w2);
  ck_assert_int_eq(r3, r4);
  assert_cstr_eq(w1, w2);
}
END_TEST

START_TEST(test_s21_sscanf_c_no_skip_space) {
  char c1 = 0, c2 = 0;
  int r1 = s21_sscanf("  A", "%c", &c1);
  int r2 = sscanf("  A", "%c", &c2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq((unsigned char)c1, (unsigned char)c2);
}
END_TEST

START_TEST(test_s21_sscanf_suppression_star) {
  int x1 = 0, x2 = 0;
  int r1 = s21_sscanf("12 34", "%*d %d", &x1);
  int r2 = sscanf("12 34", "%*d %d", &x2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(x1, x2);
}
END_TEST

START_TEST(test_s21_sscanf_n_counts_and_no_assign_inc) {
  int a1 = 0, a2 = 0;
  int n1 = 0, n2 = 0;
  char word1[16] = {0};
  char word2[16] = {0};

  int r1 = s21_sscanf("123 abc", "%d %n%s", &a1, &n1, word1);
  int r2 = sscanf("123 abc", "%d %n%s", &a2, &n2, word2);

  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);
  ck_assert_int_eq(n1, n2);
  assert_cstr_eq(word1, word2);
}
END_TEST

START_TEST(test_s21_sscanf_p_pointer) {
  void *p1 = 0;
  void *p2 = 0;

  int r1 = s21_sscanf("0x10", "%p", &p1);
  int r2 = sscanf("0x10", "%p", &p2);

  ck_assert_int_eq(r1, r2);
  ck_assert_uint_eq((uintptr_t)p1, (uintptr_t)p2);
}
END_TEST

START_TEST(test_s21_sscanf_float_basic) {
  float f1 = 0.0f, f2 = 0.0f;
  double d1 = 0.0, d2 = 0.0;

  int r1 = s21_sscanf("3.14 -1.2e3", "%f %lf", &f1, &d1);
  int r2 = sscanf("3.14 -1.2e3", "%f %lf", &f2, &d2);

  ck_assert_int_eq(r1, r2);
  ck_assert_msg(close_double((double)f1, (double)f2, 1e-6),
                "Несоответствие числа float");
  ck_assert_msg(close_double(d1, d2, 1e-9), "Несоответствие числа double");
}
END_TEST

START_TEST(test_s21_sscanf_eof_returns_minus1) {
  int x = 123;
  int r1 = s21_sscanf("", "%d", &x);
  int r2 = sscanf("", "%d", &x);
  ck_assert_int_eq(r1, r2);
}
END_TEST

START_TEST(test21_sscanf_c_width) {
  char buf1[4] = {0}, buf2[4] = {0};
  int r1 = s21_sscanf("ABCD", "%3c", buf1);
  int r2 = sscanf("ABCD", "%3c", buf2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(memcmp(buf1, buf2, 3), 0);
}
END_TEST

START_TEST(test_s21_sscanf_float_bad_exp_rollback) {
  double a1 = 0, a2 = 0;
  int r1 = s21_sscanf("1e+", "%lf", &a1);
  int r2 = sscanf("1e+", "%lf", &a2);
  ck_assert_int_eq(r1, r2);
  ck_assert_msg(fabs(a1 - a2) < 1e-12,
                "Несоответствие числа float при ошибке в экспоненте");
}
END_TEST

START_TEST(test_s21_sscanf_width_integer_and_n) {
  int a1 = 0, a2 = 0;
  int n1 = -1, n2 = -1;

  int r1 = s21_sscanf("12345", "%2d%n", &a1, &n1);
  int r2 = sscanf("12345", "%2d%n", &a2, &n2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);
  ck_assert_int_eq(n1, n2);
}
END_TEST

START_TEST(test_s21_sscanf_i_width_blocks_0x_prefix) {
  int a1 = 0, a2 = 0;

  int r1 = s21_sscanf("0x10", "%1i", &a1);
  int r2 = sscanf("0x10", "%1i", &a2);

  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);
}
END_TEST

START_TEST(test_s21_sscanf_hex_uppercase_and_invalid) {
  int x1 = 0, x2 = 0;

  int r1 = s21_sscanf("FF", "%x", &x1);
  int r2 = sscanf("FF", "%x", &x2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(x1, x2);

  // не hex вообще -> обе версии должны одинаково фейлиться
  x1 = 777; x2 = 777;
  r1 = s21_sscanf("g1", "%x", &x1);
  r2 = sscanf("g1", "%x", &x2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(x1, x2);
}
END_TEST

START_TEST(test_s21_sscanf_float_negative_exp) {
  double d1 = 0.0, d2 = 0.0;

  int r1 = s21_sscanf("1e-2", "%lf", &d1);
  int r2 = sscanf("1e-2", "%lf", &d2);

  ck_assert_int_eq(r1, r2);
  ck_assert_msg(close_double(d1, d2, 1e-9), "Несоответствие числа float: %f vs %f", d1, d2);
}
END_TEST

START_TEST(test_s21_sscanf_eof_empty_input) {
  int a1 = 0, a2 = 0;
  int r1 = s21_sscanf("", "%d", &a1);
  int r2 = sscanf("", "%d", &a2);
  ck_assert_int_eq(r1, r2);
}
END_TEST

Suite *sscanf_suite() {
  Suite *s = suite_create("s21_sscanf");
  TCase *tc = tcase_create("core");

  tcase_add_test(tc, test_s21_sscanf_d_basic);
  tcase_add_test(tc, test_s21_sscanf_i_autobase);
  tcase_add_test(tc, test_s21_sscanf_uox);
  tcase_add_test(tc, test_s21_sscanf_percent_literal);

  tcase_add_test(tc, test_s21_sscanf_s_basic_width);
  tcase_add_test(tc, test_s21_sscanf_c_no_skip_space);
  tcase_add_test(tc, test_s21_sscanf_suppression_star);

  tcase_add_test(tc, test_s21_sscanf_n_counts_and_no_assign_inc);
  tcase_add_test(tc, test_s21_sscanf_p_pointer);

  tcase_add_test(tc, test_s21_sscanf_float_basic);

  tcase_add_test(tc, test_s21_sscanf_eof_returns_minus1);
  tcase_add_test(tc, test21_sscanf_c_width);
  tcase_add_test(tc, test_s21_sscanf_float_bad_exp_rollback);
  tcase_add_test(tc, test_s21_sscanf_width_integer_and_n);
  tcase_add_test(tc, test_s21_sscanf_i_width_blocks_0x_prefix);
  tcase_add_test(tc, test_s21_sscanf_hex_uppercase_and_invalid);
  tcase_add_test(tc, test_s21_sscanf_float_negative_exp);
  tcase_add_test(tc, test_s21_sscanf_eof_empty_input);

  suite_add_tcase(s, tc);
  return s;
}