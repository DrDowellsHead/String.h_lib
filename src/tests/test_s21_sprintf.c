#include "test_s21_sprintf.h"

#include <stdarg.h>  // va_list/va_start/va_end
#include <stdio.h>   // sprintf
#include <stdlib.h>  // malloc/free

#include "../s21_string.h"

/* ------------------ BASIC ------------------ */

START_TEST(test_s21_sprintf_plain_text) {
  char s1[256], s2[256];

  int r1 = s21_sprintf(s1, "Hello, world!");
  int r2 = sprintf(s2, "Hello, world!");

  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_s21_sprintf_percent_simple) {
  char s1[256], s2[256];

  int r1 = s21_sprintf(s1, "100%% done");
  int r2 = sprintf(s2, "100%% done");

  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_s21_sprintf_percent_with_width) {
  char s1[256], s2[256];

  int r1 = s21_sprintf(s1, "[%5s]", "%");
  int r2 = sprintf(s2, "[%5s]", "%");

  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

/* ------------------ CHAR / STRING ------------------ */

START_TEST(test_s21_sprintf_char_width_minus) {
  char s1[256], s2[256];

  int r1 = s21_sprintf(s1, "[%5c][%-5c]", 'A', 'B');
  int r2 = sprintf(s2, "[%5c][%-5c]", 'A', 'B');

  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_s21_sprintf_string_width_precision) {
  char s1[256], s2[256];

  int r1 = s21_sprintf(s1, "[%10.3s][%-10.3s]", "abcdef", "abcdef");
  int r2 = sprintf(s2, "[%10.3s][%-10.3s]", "abcdef", "abcdef");

  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_s21_sprintf_string_star) {
  char s1[256], s2[256];

  int r1 = s21_sprintf(s1, "[%*.*s]", 8, 3, "abcdef");
  int r2 = sprintf(s2, "[%*.*s]", 8, 3, "abcdef");

  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

/* ------------------ INTS ------------------ */

START_TEST(test_s21_sprintf_d_flags_width_precision) {
  char s1[256], s2[256];

  int r1 = s21_sprintf(s1, "[%d][%+d][% d][%08d][%-8d][%.5d][%8.5d]", -12, 12,
                       12, 12, 12, 12, 12);
  int r2 = sprintf(s2, "[%d][%+d][% d][%08d][%-8d][%.5d][%8.5d]", -12, 12, 12,
                   12, 12, 12, 12);

  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_s21_sprintf_d_star_negative_width) {
  char s1[256], s2[256];

  // отрицательная width через '*' должна включать '-' и взять abs(width)
  // отрицательная width через '*' должна включать '-' и взять abs(width)
  int r1 = s21_sprintf(s1, "[%*d]", -6, 42);
  int r2 = sprintf(s2, "[%*d]", -6, 42);

  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_s21_sprintf_zero_precision_zero_value) {
  char s1[256], s2[256];

  // %.0d при 0 печатает пусто (только ширина, если есть)
  int r1 = s21_sprintf(s1, "[%.0d][%5.0d]", 0, 0);
  int r2 = sprintf(s2, "[%.0d][%5.0d]", 0, 0);

  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_s21_sprintf_length_h_l) {
  char s1[256], s2[256];

  short hs = -7;
  long ls = 1234567L;
  unsigned short hu = 65000;
  unsigned long lu = 4000000000UL;

  int r1 = s21_sprintf(s1, "[%hd][%ld][%hu][%lu]", hs, ls, hu, lu);
  int r2 = sprintf(s2, "[%hd][%ld][%hu][%lu]", hs, ls, hu, lu);

  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

/* ------------------ UNSIGNED / BASES ------------------ */

START_TEST(test_s21_sprintf_uox_hash) {
  char s1[256], s2[256];

  unsigned int v = 255;

  int r1 =
      s21_sprintf(s1, "[%u][%o][%#o][%x][%#x][%X][%#X]", v, v, v, v, v, v, v);
  int r2 = sprintf(s2, "[%u][%o][%#o][%x][%#x][%X][%#X]", v, v, v, v, v, v, v);

  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

/* ------------------ POINTER / N ------------------ */

START_TEST(test_s21_sprintf_pointer_p) {
  char s1[256], s2[256];
  int x = 123;

  int r1 = s21_sprintf(s1, "ptr=%p", (void *)&x);
  int r2 = sprintf(s2, "ptr=%p", (void *)&x);

  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_s21_sprintf_n) {
  char s1[256], s2[256];
  int n1 = -1, n2 = -1;

  int r1 = s21_sprintf(s1, "abc%nDEF", &n1);
  int r2 = sprintf(s2, "abc%nDEF", &n2);

  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(n1, n2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

/* ------------------ FLOATS ------------------ */

START_TEST(test_s21_sprintf_f_basic) {
  char s1[256], s2[256];

  int r1 = s21_sprintf(s1, "[%f][%.2f][%8.2f][%+8.2f][%#0.0f]", 3.14, 3.14,
                       3.14, 3.14, 3.0);
  int r2 = sprintf(s2, "[%f][%.2f][%8.2f][%+8.2f][%#0.0f]", 3.14, 3.14, 3.14,
                   3.14, 3.0);

  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_s21_sprintf_e_E_basic) {
  char s1[256], s2[256];

  double v = 1234.5;

  int r1 = s21_sprintf(s1, "[%e][%.3e][%E][%.2E]", v, v, v, v);
  int r2 = sprintf(s2, "[%e][%.3e][%E][%.2E]", v, v, v, v);

  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_s21_sprintf_g_G_basic) {
  char s1[256], s2[256];

  double a = 123.45;
  double b = 0.0001234;
  double c = 1234567.0;

  int r1 = s21_sprintf(s1, "[%g][%.4g][%G][%.3G][%g]", a, a, b, b, c);
  int r2 = sprintf(s2, "[%g][%.4g][%G][%.3G][%g]", a, a, b, b, c);

  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_s21_sprintf_Lf_long_double) {
  char s1[256], s2[256];

  long double v = (long double)3.1415926535L;

  int r1 = s21_sprintf(s1, "[%Lf][%.3Lf]", v, v);
  int r2 = sprintf(s2, "[%Lf][%.3Lf]", v, v);

  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

/* ------------------ NAN/INF ------------------ */

START_TEST(test_s21_sprintf_nan_inf) {
  char s1[256], s2[256];

  double nanv = 0.0 / 0.0;
  double infv = 1.0 / 0.0;
  double ninfv = -1.0 / 0.0;

  int r1 =
      s21_sprintf(s1, "[%f][%e][%G][%+f][% f]", nanv, infv, ninfv, infv, infv);
  int r2 = sprintf(s2, "[%f][%e][%G][%+f][% f]", nanv, infv, ninfv, infv, infv);

  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_s21_sprintf_null_string) {
  char s1[256], s2[256];

  volatile char *p = (char *)0;
  char *arg = (char *)p;

  int r1 = s21_sprintf(s1, "[%s]", arg);
  int r2 = sprintf(s2, "[%s]", arg);
  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_s21_sprintf_hash_o_zero_perc0) {
  char s1[256], s2[256];
  int r1 = s21_sprintf(s1, "[%#.0o]", 0);
  int r2 = sprintf(s2, "[%#.0o]", 0);
  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

Suite *sprintf_suite() {
  Suite *s = suite_create("s21_sprintf");
  TCase *tc = tcase_create("core");

  tcase_add_test(tc, test_s21_sprintf_plain_text);
  tcase_add_test(tc, test_s21_sprintf_percent_simple);
  tcase_add_test(tc, test_s21_sprintf_percent_with_width);

  tcase_add_test(tc, test_s21_sprintf_char_width_minus);
  tcase_add_test(tc, test_s21_sprintf_string_width_precision);
  tcase_add_test(tc, test_s21_sprintf_string_star);

  tcase_add_test(tc, test_s21_sprintf_d_flags_width_precision);
  tcase_add_test(tc, test_s21_sprintf_d_star_negative_width);
  tcase_add_test(tc, test_s21_sprintf_zero_precision_zero_value);
  tcase_add_test(tc, test_s21_sprintf_length_h_l);

  tcase_add_test(tc, test_s21_sprintf_uox_hash);

  tcase_add_test(tc, test_s21_sprintf_pointer_p);
  tcase_add_test(tc, test_s21_sprintf_n);

  tcase_add_test(tc, test_s21_sprintf_f_basic);
  tcase_add_test(tc, test_s21_sprintf_e_E_basic);
  tcase_add_test(tc, test_s21_sprintf_g_G_basic);
  tcase_add_test(tc, test_s21_sprintf_Lf_long_double);

  tcase_add_test(tc, test_s21_sprintf_nan_inf);
  tcase_add_test(tc, test_s21_sprintf_null_string);
  tcase_add_test(tc, test_s21_sprintf_hash_o_zero_perc0);

  suite_add_tcase(s, tc);
  return s;
}