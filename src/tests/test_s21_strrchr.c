#include <stdlib.h>
#include <string.h> 

#include "../s21_string.h"
#include "test_s21_strrchr.h"

START_TEST(test_s21_strrchr_basic) {
    const char *str = "Hello, world!";
    int c = 'o';
    char *result = s21_strrchr(str, c);
    char *expected = strrchr(str, c);
    ck_assert_ptr_eq(result, expected);
    // Дополнительно проверим позицию
    ck_assert_int_eq(result - str, 8); // последнее 'o' на позиции 8
}
END_TEST

START_TEST(test_s21_strrchr_multiple) {
    const char *str = "abacaba";
    int c = 'a';
    char *result = s21_strrchr(str, c);
    ck_assert_int_eq(result - str, 6); // последняя 'a' на позиции 6
}
END_TEST

START_TEST(test_s21_strrchr_not_found) {
    const char *str = "Hello";
    int c = 'x';
    char *result = s21_strrchr(str, c);
    ck_assert_ptr_null(result);
}
END_TEST

START_TEST(test_s21_strrchr_null_char) {
    const char *str = "Hello";
    int c = '\0';
    char *result = s21_strrchr(str, c);
    char *expected = strrchr(str, c);
    ck_assert_ptr_eq(result, expected);
    ck_assert_int_eq(result - str, strlen(str)); // указатель на терминирующий ноль
}
END_TEST

START_TEST(test_s21_strrchr_empty_string) {
    const char *str = "";
    int c = 'a';
    char *result = s21_strrchr(str, c);
    ck_assert_ptr_null(result);

    c = '\0';
    result = s21_strrchr(str, c);
    ck_assert_ptr_eq(result, str); // в пустой строке нуль на позиции 0
}
END_TEST

START_TEST(test_s21_strrchr_null_pointer) {
    const char *str = NULL;
    int c = 'a';
    char *result = s21_strrchr(str, c);
    ck_assert_ptr_null(result);
}
END_TEST

START_TEST(test_s21_strrchr_negative_char) {
    // Символ с кодом -1 (255) – если в строке есть символ с кодом 255, то найдём
    // Создадим строку, содержащую символ 255
    char str[] = {255, 'a', 'b', 255, 'c', 0};
    int c = -1; // должен интерпретироваться как 255
    char *result = s21_strrchr(str, c);
    ck_assert_int_eq(result - str, 3); // последнее вхождение на позиции 3
}
END_TEST

START_TEST(test_s21_strrchr_unsigned_char) {
    char str[] = {200, 150, 200, 0};
    int c = 200;
    char *result = s21_strrchr(str, c);
    ck_assert_int_eq(result - str, 2); // последнее на позиции 2
}
END_TEST

START_TEST(test_s21_strrchr_first_position) {
    const char *str = "abc";
    int c = 'a';
    char *result = s21_strrchr(str, c);
    ck_assert_int_eq(result - str, 0);
}
END_TEST

START_TEST(test_s21_strrchr_last_position) {
    const char *str = "abca";
    int c = 'a';
    char *result = s21_strrchr(str, c);
    ck_assert_int_eq(result - str, 3);
}
END_TEST

Suite *strrchr_suite(void) {
    Suite *s = suite_create("s21_strrchr");
    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_s21_strrchr_basic);
    tcase_add_test(tc_core, test_s21_strrchr_multiple);
    tcase_add_test(tc_core, test_s21_strrchr_not_found);
    tcase_add_test(tc_core, test_s21_strrchr_null_char);
    tcase_add_test(tc_core, test_s21_strrchr_empty_string);
    tcase_add_test(tc_core, test_s21_strrchr_null_pointer);
    tcase_add_test(tc_core, test_s21_strrchr_negative_char);
    tcase_add_test(tc_core, test_s21_strrchr_unsigned_char);
    tcase_add_test(tc_core, test_s21_strrchr_first_position);
    tcase_add_test(tc_core, test_s21_strrchr_last_position);

    suite_add_tcase(s, tc_core);
    return s;
}