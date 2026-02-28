#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>

#include "test_s21_strerror.h"
#include "../s21_string.h"


/* Тест: Проверка всех стандартных кодов ошибок */
START_TEST(test_s21_strerror_known_codes) {
    for (int i = 0; i <= S21_MAX_ERROR_COUNT; i++) {
        char *result = s21_strerror(i);
        
        /* Проверяем, что результат не NULL */
        ck_assert_ptr_nonnull(result);
        
        /* Проверяем, что результат не пустой */
        ck_assert_int_gt(strlen(result), 0);
        
        /* Проверяем, что строка заканчивается нуль-терминатором */
        ck_assert_int_eq(result[strlen(result)], '\0');
        
        /* Для некоторых известных ошибок проверяем содержание */
        if (i == 0) {
            /* Для errno 0 обычно "Success" или "Undefined error" */
            ck_assert(strlen(result) > 0);
        }
    }
}
END_TEST

/* Тест: Проверка граничных значений в диапазоне */
START_TEST(test_s21_strerror_boundary_values) {
    /* Первый код ошибки */
    char *result_first = s21_strerror(0);
    ck_assert(result_first != NULL);
    ck_assert(result_first[0] != '\0');
    
    /* Последний код в диапазоне */
    char *result_last = s21_strerror(S21_MAX_ERROR_COUNT);
    ck_assert(result_last != NULL);
    ck_assert(result_last[0] != '\0');
    
    /* Проверка, что буфер статический (возвращается тот же адрес) */
    ck_assert_ptr_eq(result_first, s21_strerror(0));
}
END_TEST

/* Тест: Проверка неизвестных кодов ошибок (отрицательные значения) */
START_TEST(test_s21_strerror_unknown_negative) {
    int test_values[] = {-1, -100, -1000, INT_MIN};
    int num_tests = sizeof(test_values) / sizeof(test_values[0]);
    
    for (int i = 0; i < num_tests; i++) {
        int errnum = test_values[i];
        char *result = s21_strerror(errnum);
        
        ck_assert(result != NULL);
        
        /* Проверяем формат сообщения об ошибке */
        char expected[256];
        sprintf(expected, "Unknown error %d", errnum);
        
        /* Сравниваем строки */
        ck_assert_str_eq(result, expected);
        
        /* Проверяем, что сообщение не пустое */
        ck_assert(strlen(result) > strlen("Unknown error "));
    }
}
END_TEST

/* Тест: Проверка неизвестных кодов ошибок (положительные вне диапазона) */
START_TEST(test_s21_strerror_unknown_positive) {
    int test_values[] = {S21_MAX_ERROR_COUNT + 1, 
                         S21_MAX_ERROR_COUNT + 100, 
                         1000, 
                         INT_MAX};
    int num_tests = sizeof(test_values) / sizeof(test_values[0]);
    
    for (int i = 0; i < num_tests; i++) {
        int errnum = test_values[i];
        char *result = s21_strerror(errnum);
        
        ck_assert(result != NULL);
        
        /* Проверяем формат сообщения об ошибке */
        char expected[256];
        sprintf(expected, "Unknown error %d", errnum);
        
        /* Сравниваем строки */
        ck_assert_str_eq(result, expected);
        
        /* Проверяем, что число в сообщении совпадает с переданным */
        char *num_part = result + strlen("Unknown error ");
        int extracted_num = atoi(num_part);
        ck_assert_int_eq(extracted_num, errnum);
    }
}
END_TEST

/* Тест: Проверка переполнения буфера (edge case) */
START_TEST(test_s21_strerror_buffer_overflow) {
    /* Максимально длинное число для проверки буфера */
    int very_large_num = 1234567890;
    char *result = s21_strerror(very_large_num);
    
    ck_assert(result != NULL);
    
    /* Проверяем, что строка не превышает размер буфера */
    ck_assert(strlen(result) < 256);
    
    /* Проверяем корректность формирования */
    char expected[256];
    sprintf(expected, "Unknown error %d", very_large_num);
    ck_assert_str_eq(result, expected);
}
END_TEST

/* Тест: Проверка последовательных вызовов */
START_TEST(test_s21_strerror_consecutive_calls) {
    char *first = s21_strerror(2);  /* Например, ENOENT */
    char *second = s21_strerror(999); /* Неизвестная ошибка */
    
    ck_assert(first != NULL);
    ck_assert(second != NULL);
    
    /* Проверяем, что второй вызов изменил содержимое буфера */
    ck_assert(strstr(second, "Unknown error") != NULL);
    
    /* Старое сообщение должно быть потеряно (статический буфер) */
    char *third = s21_strerror(0);
    ck_assert(strlen(third) > 0);
}
END_TEST

/* Тест: Проверка всех известных ошибок с системной реализацией */
START_TEST(test_s21_strerror_vs_system) {
    for (int i = 0; i <= S21_MAX_ERROR_COUNT; i++) {
        char *s21_result = s21_strerror(i);
        char *sys_result = strerror(i);
        
        /* Разные системы могут давать разные сообщения,
           но они не должны быть пустыми */
        ck_assert(s21_result != NULL);
        ck_assert(sys_result != NULL);
        ck_assert(s21_result[0] != '\0');
        
        /* Дополнительная проверка: сообщения должны содержать 
           некоторые общие ключевые слова для известных ошибок */
        /* Например, для EPERM (1) - "Operation not permitted" */
        if (i == 1) {
            ck_assert(strstr(s21_result, "permitted") != NULL ||
                     strstr(s21_result, "Permitted") != NULL);
        }
    }
}
END_TEST

/* Тест: Проверка на NULL-безопасность */
START_TEST(test_s21_strerror_null_safety) {
    /* Функция не должна падать при любых входных данных */
    int dangerous_values[] = {INT_MIN, INT_MAX, -0, 0, 1, -1};
    
    for (int i = 0; i < 6; i++) {
        char *result = s21_strerror(dangerous_values[i]);
        ck_assert(result != NULL);
    }
}
END_TEST

/* Тест: Проверка формата для неизвестной ошибки */
START_TEST(test_s21_strerror_unknown_format) {
    char *result = s21_strerror(999);
    
    ck_assert(result != NULL);
    
    /* Проверяем начало сообщения */
    ck_assert(strncmp(result, "Unknown error ", 14) == 0);
    
    /* Проверяем, что после текста идет число */
    const char *num_start = result + 14;
    ck_assert(num_start[0] >= '0' && num_start[0] <= '9');
}
END_TEST

/* Тест: Проверка граничных значений int */
START_TEST(test_s21_strerror_int_boundaries) {
    int boundaries[] = {INT_MIN, INT_MIN + 1, -2147483647, -1, 0, 
                        1, S21_MAX_ERROR_COUNT - 1, S21_MAX_ERROR_COUNT,
                        S21_MAX_ERROR_COUNT + 1, INT_MAX - 1, INT_MAX};
    
    for (int i = 0; i < 11; i++) {
        char *result = s21_strerror(boundaries[i]);
        ck_assert(result != NULL);
        ck_assert(result[0] != '\0');
        
        /* Проверяем, что строка заканчивается нуль-терминатором */
        int len = strlen(result);
        ck_assert(result[len] == '\0');
    }
}
END_TEST

Suite *strerror_suite(void) {
    Suite *s = suite_create("s21_strerror");
    TCase *tc_core = tcase_create("Core");
    TCase *tc_boundary = tcase_create("Boundary");
    TCase *tc_unknown = tcase_create("Unknown");
    TCase *tc_edge = tcase_create("Edge");

    tcase_add_test(tc_core, test_s21_strerror_known_codes);
    tcase_add_test(tc_core, test_s21_strerror_vs_system);
    tcase_add_test(tc_core, test_s21_strerror_consecutive_calls);
    tcase_add_test(tc_core, test_s21_strerror_null_safety);
    suite_add_tcase(s, tc_core);

    tcase_add_test(tc_boundary, test_s21_strerror_boundary_values);
    tcase_add_test(tc_boundary, test_s21_strerror_int_boundaries);
    suite_add_tcase(s, tc_boundary);

    tcase_add_test(tc_unknown, test_s21_strerror_unknown_negative);
    tcase_add_test(tc_unknown, test_s21_strerror_unknown_positive);
    tcase_add_test(tc_unknown, test_s21_strerror_unknown_format);
    suite_add_tcase(s, tc_unknown);

    tc_edge = tcase_create("Edge");
    tcase_add_test(tc_edge, test_s21_strerror_buffer_overflow);
    suite_add_tcase(s, tc_edge);

    return s;
}