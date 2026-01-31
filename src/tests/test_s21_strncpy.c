#include "../s21_string.h"
#include "test_s21_strncpy.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

START_TEST(test_strncpy_basic) {
    char dest1[20], dest2[20];

    s21_strncpy(dest1, "Hello", 5);
    strncpy(dest2, "Hello", 5);

    ck_assert_mem_eq(dest1, dest2,5);
}
END_TEST

START_TEST(test_strncpy_greater) {
    char dest1[20], dest2[20];

    s21_strncpy(dest1, "Hello World", 5);
    strncpy(dest2, "Hello World", 5);

    ck_assert_mem_eq(dest1, dest2, 5);
}
END_TEST

START_TEST(test_strncpy_smaller) {
    char dest1[20], dest2[20];

    s21_strncpy(dest1, "Hello World", 5);
    strncpy(dest2, "Hellow World", 5);

    ck_assert_mem_eq(dest1, dest2, 5);
}
END_TEST

/* Автоматический тест для любой функции сравнения */
START_TEST(test_strlen_random) {
    srand(time(NULL));

    for (int i = 0; i < 50; i++) {
        int len = rand() % 100 + 1;
        char* str = random_string(len);

        size_t expected = strlen(str);
        __size_internal actual = s21_strlen(str);

        ck_assert_uint_eq(expected, actual);

        free(str);
    }
}
END_TEST

/* ========== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ========== */
char* random_string(int length) {
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

/* ========== СОЗДАНИЕ ТЕСТОВОГО НАБОРА ========== */
Suite *strncpy_suite(void){
    Suite *s = suite_create("s21_strncpy");
    TCase *tc = tcase_create("Core");

    tcase_add_test(tc, test_strncpy_basic);
    tcase_add_test(tc, test_strncpy_greater);
    tcase_add_test(tc, test_strncpy_smaller);
    tcase_add_test(tc, test_strlen_random);

    suite_add_tcase(s, tc);
    return s;
}