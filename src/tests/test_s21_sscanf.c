#include "test_s21_sscanf.h"

#include <math.h>
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
  x1 = 777;
  x2 = 777;
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
  ck_assert_msg(close_double(d1, d2, 1e-9),
                "Несоответствие числа float: %f vs %f", d1, d2);
}
END_TEST

START_TEST(test_s21_sscanf_eof_empty_input) {
  int a1 = 0, a2 = 0;
  int r1 = s21_sscanf("", "%d", &a1);
  int r2 = sscanf("", "%d", &a2);
  ck_assert_int_eq(r1, r2);
}
END_TEST

static int close_long_double(long double x, long double y, long double eps) {
  long double d = x - y;
  if (d < 0) d = -d;
  return d <= eps;
}

// Добиваем все пробельные символы: ' ', \t, \n, \r, \v, \f
START_TEST(test_s21_sscanf_all_space_kinds) {
  int a1 = 0, a2 = 0;
  const char *in = "\v\f\t\r\n   42";
  int r1 = s21_sscanf(in, "%d", &a1);
  int r2 = sscanf(in, "%d", &a2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);
}
END_TEST

// Плюс-значение для signed int: покрывает ветку '+'
START_TEST(test_s21_sscanf_d_plus_sign) {
  int a1 = 0, a2 = 0;
  int r1 = s21_sscanf("+12", "%d", &a1);
  int r2 = sscanf("+12", "%d", &a2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);
}
END_TEST

// Кейс "есть знак, но нет цифр" (скан должен провалиться, но НЕ EOF)
START_TEST(test_s21_sscanf_only_sign_not_eof) {
  int a1 = 777, a2 = 777;
  int r1 = s21_sscanf("-", "%d", &a1);
  int r2 = sscanf("-", "%d", &a2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);
}
END_TEST

// %hd / %hu (ветки length == 'h')
START_TEST(test_s21_sscanf_length_h_integers) {
  short a1 = 0, a2 = 0;
  unsigned short u1 = 0, u2 = 0;

  int r1 = s21_sscanf("-7 65000", "%hd %hu", &a1, &u1);
  int r2 = sscanf("-7 65000", "%hd %hu", &a2, &u2);

  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);
  ck_assert_uint_eq(u1, u2);
}
END_TEST

//  0X (uppercase) — добиваем ветку (s[1] == 'X') в 0x-префиксе
START_TEST(test_s21_sscanf_hex_prefix_0X) {
  unsigned int x1 = 0, x2 = 0;

  int r1 = s21_sscanf("0X1A", "%x", &x1);
  int r2 = sscanf("0X1A", "%x", &x2);

  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(x1, x2);
}
END_TEST

//  'G' (uppercase) как не-hex — добиваем ветки сравнения в s21_hex_val_char
START_TEST(test_s21_sscanf_hex_invalid_upper_G) {
  unsigned int x1 = 777, x2 = 777;

  int r1 = s21_sscanf("G1", "%x", &x1);
  int r2 = sscanf("G1", "%x", &x2);

  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(x1, x2);
}
END_TEST

// Добиваем ветки float-спеков: %f %e %E %g %G и length L
START_TEST(test_s21_sscanf_float_specs_f_e_E_g_G_and_L) {
  float f1 = 0.0f, f2 = 0.0f;
  double e1 = 0.0, e2 = 0.0;
  double E1 = 0.0, E2 = 0.0;
  double g1 = 0.0, g2 = 0.0;
  double G1 = 0.0, G2 = 0.0;
  long double L1 = 0.0L, L2 = 0.0L;

  const char *in = " +3.14  1e+2  1E2  0.0001234  123.45  2.5";
  int r1 =
      s21_sscanf(in, "%f %le %lE %lg %lG %Lf", &f1, &e1, &E1, &g1, &G1, &L1);
  int r2 = sscanf(in, "%f %le %lE %lg %lG %Lf", &f2, &e2, &E2, &g2, &G2, &L2);

  ck_assert_int_eq(r1, r2);
  ck_assert_msg(close_double(f1, f2, 1e-9), "Несоответствие float: %f vs %f",
                f1, f2);
  ck_assert_msg(close_double(e1, e2, 1e-9),
                "Несоответствие double (e): %f vs %f", e1, e2);
  ck_assert_msg(close_double(E1, E2, 1e-9),
                "Несоответствие double (E): %f vs %f", E1, E2);
  ck_assert_msg(close_double(g1, g2, 1e-9),
                "Несоответствие double (g): %f vs %f", g1, g2);
  ck_assert_msg(close_double(G1, G2, 1e-9),
                "Несоответствие double (G): %f vs %f", G1, G2);
  ck_assert_msg(close_long_double(L1, L2, 1e-9L),
                "Несоответствие long double: %Lf vs %Lf", L1, L2);
}
END_TEST

//  ".5" (нет целой части) и "1." (нет дробных цифр) — добиваем ветки точки
START_TEST(test_s21_sscanf_float_dot_corner_cases) {
  double a1 = 0.0, a2 = 0.0;
  double b1 = 0.0, b2 = 0.0;

  int r1 = s21_sscanf(".5 1.", "%lf %lf", &a1, &b1);
  int r2 = sscanf(".5 1.", "%lf %lf", &a2, &b2);

  ck_assert_int_eq(r1, r2);
  ck_assert_msg(close_double(a1, a2, 1e-12),
                "Несоответствие double (a): %f vs %f", a1, a2);
  ck_assert_msg(close_double(b1, b2, 1e-12),
                "Несоответствие double (b): %f vs %f", b1, b2);
}
END_TEST

//  "." — точка без цифр: конверсия должна провалиться (но ввод не EOF)
START_TEST(test_s21_sscanf_float_dot_only_fails) {
  double a1 = 777.0, a2 = 777.0;

  int r1 = s21_sscanf(".", "%lf", &a1);
  int r2 = sscanf(".", "%lf", &a2);

  ck_assert_int_eq(r1, r2);
  ck_assert_msg(close_double(a1, a2, 1e-12),
                "Несоответствие double при ошибке: %f vs %f", a1, a2);
}
END_TEST

//  %n с h/l: добиваем ветки записи short/long
START_TEST(test_s21_sscanf_n_h_and_l) {
  int a1 = 0, a2 = 0;
  short n1s = 0, n2s = 0;
  long n1l = 0, n2l = 0;
  int b1 = 0, b2 = 0;

  int r1 = s21_sscanf("12 34", "%d %hn%ln%d", &a1, &n1s, &n1l, &b1);
  int r2 = sscanf("12 34", "%d %hn%ln%d", &a2, &n2s, &n2l, &b2);

  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);
  ck_assert_int_eq(b1, b2);
  ck_assert_int_eq(n1s, n2s);
  ck_assert_int_eq((long)n1l, (long)n2l);
}
END_TEST

//  Конверсия пыталась, но ввод НЕ EOF: "abc" под %d => return 0, а не -1
START_TEST(test_s21_sscanf_fail_not_eof) {
  int a1 = 123, a2 = 123;

  int r1 = s21_sscanf("abc", "%d", &a1);
  int r2 = sscanf("abc", "%d", &a2);

  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);
}
END_TEST

//  Формат без конверсий (any_conversion_attempted = 0)
START_TEST(test_s21_sscanf_format_only_spaces) {
  int r1 = s21_sscanf("", "   \t\n\r\v\f");
  int r2 = sscanf("", "   \t\n\r\v\f");

  ck_assert_int_eq(r1, r2);
}
END_TEST

//  Ввод только из пробелов + %s => EOF (-1)
START_TEST(test_s21_sscanf_s_whitespace_only_eof) {
  char b1[8] = {0}, b2[8] = {0};

  int r1 = s21_sscanf(" \t\n\r\v\f", "%s", b1);
  int r2 = sscanf(" \t\n\r\v\f", "%s", b2);

  ck_assert_int_eq(r1, r2);
  assert_cstr_eq(b1, b2);
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

  tcase_add_test(tc, test_s21_sscanf_all_space_kinds);
  tcase_add_test(tc, test_s21_sscanf_d_plus_sign);
  tcase_add_test(tc, test_s21_sscanf_only_sign_not_eof);
  tcase_add_test(tc, test_s21_sscanf_length_h_integers);
  tcase_add_test(tc, test_s21_sscanf_hex_prefix_0X);
  tcase_add_test(tc, test_s21_sscanf_hex_invalid_upper_G);
  tcase_add_test(tc, test_s21_sscanf_float_specs_f_e_E_g_G_and_L);
  tcase_add_test(tc, test_s21_sscanf_float_dot_corner_cases);
  tcase_add_test(tc, test_s21_sscanf_float_dot_only_fails);
  tcase_add_test(tc, test_s21_sscanf_n_h_and_l);
  tcase_add_test(tc, test_s21_sscanf_fail_not_eof);
  tcase_add_test(tc, test_s21_sscanf_format_only_spaces);
  tcase_add_test(tc, test_s21_sscanf_s_whitespace_only_eof);

  suite_add_tcase(s, tc);
  return s;
}