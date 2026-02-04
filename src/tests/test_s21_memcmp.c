#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "test_s21_memcmp.h"
#include "../s21_string.h"

void compare_memcmp_results(const void *str1, const void *str2, size_t n) {
    int std_result = memcmp(str1, str2, n);
    int our_result = s21_memcmp(str1, str2, n);

    //здесь сверяем только знаки, потому что иначе не проходит тест с бинарными данными
    int std_sign = (std_result > 0) ? 1 : ((std_result < 0) ? -1 : 0);
    int our_sign = (our_result > 0) ? 1 : ((our_result < 0) ? -1 : 0);
    
    ck_assert_int_eq(std_sign, our_sign);
}

/* ========== ТЕСТЫ ========== */

// Тест 1: Пустые строки (n = 0)
START_TEST(test_zero_length) {
    const char *str1 = "abc";
    const char *str2 = "def";
    
    compare_memcmp_results(str1, str2, 0);
}
END_TEST

// Тест 2: Равные строки
START_TEST(test_equal_strings) {
    const char *str1 = "Hello World";
    const char *str2 = "Hello World";
    
    compare_memcmp_results(str1, str2, strlen(str1));
}
END_TEST

// Тест 3: Строка 1 меньше строки 2 (первый байт)
START_TEST(test_str1_less_first_byte) {
    const char *str1 = "Apple";
    const char *str2 = "Banana";
    
    compare_memcmp_results(str1, str2, 5);
}
END_TEST

// Тест 4: Строка 1 больше строки 2 (первый байт)
START_TEST(test_str1_greater_first_byte) {
    const char *str1 = "Banana";
    const char *str2 = "Apple";
    
    compare_memcmp_results(str1, str2, 5);
}
END_TEST

// Тест 5: Строка 1 меньше строки 2 (последующие байты)
START_TEST(test_str1_less_later_byte) {
    const char *str1 = "Hello";
    const char *str2 = "Helmo";
    
    compare_memcmp_results(str1, str2, 5);
}
END_TEST

// Тест 6: Строка 1 больше строки 2 (последующие байты)
START_TEST(test_str1_greater_later_byte) {
    const char *str1 = "Helmo";
    const char *str2 = "Hello";
    
    compare_memcmp_results(str1, str2, 5);
}
END_TEST

// Тест 7: Разные длины строк
START_TEST(test_different_lengths) {
    const char *str1 = "Hello";
    const char *str2 = "Hello World";
    
    compare_memcmp_results(str1, str2, 5); 
}
END_TEST

// Тест 8: Бинарные данные
START_TEST(test_binary_data) {
    unsigned char data1[] = {0x00, 0x01, 0x02, 0x03};
    unsigned char data2[] = {0x00, 0x01, 0x02, 0x04};
    unsigned char data3[] = {0x00, 0x01, 0x02, 0x02};
    compare_memcmp_results(data1, data2, 4);
    compare_memcmp_results(data1, data3, 4);
    compare_memcmp_results(data2, data3, 4);
}
END_TEST

// Тест 9: Строки с нулевыми байтами
START_TEST(test_strings_with_null_bytes) {
    char str1[] = "Hello\0World";
    char str2[] = "Hello\0World";
    char str3[] = "Hello\0Xorld";
    
    compare_memcmp_results(str1, str2, sizeof(str1));
    compare_memcmp_results(str1, str3, sizeof(str1));
    compare_memcmp_results(str1, str2, 5); // Только "Hello"
    
    // Сравнение с разными позициями нулевого байта
    char str4[] = "AB\0CD";
    char str5[] = "AB\0CE";
    compare_memcmp_results(str4, str5, sizeof(str4));
}
END_TEST

// Тест 10: Пограничные значения n
START_TEST(test_boundary_n_values) {
    const char *str1 = "ABCDEF";
    const char *str2 = "ABCDeF";
    
    compare_memcmp_results(str1, str2, 0);
}
END_TEST


// Тест 11: Тест с пограничными значениями байтов
START_TEST(test_edge_byte_values) {
    // Минимальное и максимальное значения unsigned char
    unsigned char min_max1[] = {0x00, 0x7F, 0x80, 0xFF};
    unsigned char min_max2[] = {0x00, 0x7F, 0x80, 0xFE};
    unsigned char min_max3[] = {0x01, 0x7F, 0x80, 0xFF};
    
    compare_memcmp_results(min_max1, min_max2, sizeof(min_max1));
    compare_memcmp_results(min_max1, min_max3, sizeof(min_max1));
    compare_memcmp_results(min_max2, min_max3, sizeof(min_max1));
}
END_TEST

// Тест 12: Частичное сравнение
START_TEST(test_partial_comparison) {
    const char *long_str = "This is a very long string for testing partial comparison";
    
    // Сравнение разных частей строки
    for (size_t i = 0; i < 10; i++) {
        for (size_t len = 1; len < 10; len++) {
            if (i + len <= strlen(long_str)) {
                compare_memcmp_results(long_str + i, long_str + i, len);
            }
        }
    }
    
    // Сравнение с небольшой разницей
    const char *str1 = "partial test";
    const char *str2 = "partial tast";
    compare_memcmp_results(str1, str2, 12);
    compare_memcmp_results(str1, str2, 7);
}
END_TEST

Suite *memcmp_suite(void){
    Suite *s = suite_create("s21_memcmp");
    TCase *tc_core = tcase_create("Core");
    TCase *tc_boundary = tcase_create("Boundary");

    // Основные тесты
    tc_core = tcase_create("Core Tests");
    tcase_add_test(tc_core, test_zero_length);
    tcase_add_test(tc_core, test_equal_strings);
    tcase_add_test(tc_core, test_str1_less_first_byte);
    tcase_add_test(tc_core, test_str1_greater_first_byte);
    tcase_add_test(tc_core, test_str1_less_later_byte);
    tcase_add_test(tc_core, test_str1_greater_later_byte);
    suite_add_tcase(s, tc_core);
    
    // Граничные случаи
    tc_boundary = tcase_create("Boundary Cases");
    tcase_add_test(tc_boundary, test_binary_data);
    tcase_add_test(tc_boundary, test_strings_with_null_bytes);
    tcase_add_test(tc_boundary, test_boundary_n_values);
    tcase_add_test(tc_boundary, test_edge_byte_values);
    tcase_add_test(tc_boundary, test_partial_comparison);
    tcase_add_test(tc_boundary, test_different_lengths);
    suite_add_tcase(s, tc_boundary);

    return s;
}