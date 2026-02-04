#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "test_s21_memchr.h"
#include "../s21_string.h"

void compare_memchr_results(const void *str, int c, size_t n, const char *test_name) {
    void *std_result = memchr(str, c, n);
    void *our_result = s21_memchr(str, c, n);
    
    if (std_result == NULL) {
        ck_assert_msg(our_result == NULL, 
                     "%s: Standard memchr returned NULL but s21_memchr returned %p", 
                     test_name, our_result);
    } else {
        ck_assert_msg(our_result != NULL, 
                     "%s: Standard memchr returned %p but s21_memchr returned NULL", 
                     test_name, std_result);
        ck_assert_msg(std_result == our_result, 
                     "%s: Pointers differ. Standard: %p, Our: %p", 
                     test_name, std_result, our_result);
        ck_assert_msg(*(char *)std_result == *(char *)our_result,
                     "%s: Values differ. Standard: %d, Our: %d",
                     test_name, *(char *)std_result, *(char *)our_result);
    }
}

void assert_both_null(const void *str, int c, size_t n, const char *test_name) {
    void *std_result = memchr(str, c, n);
    void *our_result = s21_memchr(str, c, n);
    
    ck_assert_msg(std_result == NULL, 
                 "%s: Standard memchr should return NULL but returned %p", 
                 test_name, std_result);
    ck_assert_msg(our_result == NULL, 
                 "%s: s21_memchr should return NULL but returned %p", 
                 test_name, our_result);
}

// Тест 1: Пустой поиск (n = 0)
START_TEST(test_zero_length_standard) {
    const char *str = "Hello";
    
    compare_memchr_results(str, 'H', 0, "zero_length_1");
    compare_memchr_results(str, 'e', 0, "zero_length_2");
    compare_memchr_results(str, 'o', 0, "zero_length_3");
}
END_TEST

// Тест 2: Нахождение символа в начале
START_TEST(test_find_at_beginning_standard) {
    const char *str = "Hello World";
    
    compare_memchr_results(str, 'H', strlen(str), "find_beginning_1");
    compare_memchr_results(str, 'H', 1, "find_beginning_2");
    compare_memchr_results(str, 'H', 5, "find_beginning_3");
}
END_TEST

// Тест 3: Нахождение символа в середине
START_TEST(test_find_in_middle_standard) {
    const char *str = "Hello World";
    
    compare_memchr_results(str, 'W', strlen(str), "find_middle_1");
    compare_memchr_results(str, 'o', strlen(str), "find_middle_2");
    compare_memchr_results(str, 'l', strlen(str), "find_middle_3");
}
END_TEST

// Тест 4: Нахождение символа в конце
START_TEST(test_find_at_end_standard) {
    const char *str = "Hello";
    
    compare_memchr_results(str, 'o', 5, "find_end_1");
    compare_memchr_results(str, 'o', 10, "find_end_2");
}
END_TEST

// Тест 5: Символ не найден
START_TEST(test_char_not_found_standard) {
    const char *str = "Hello World";
    
    assert_both_null(str, 'x', strlen(str), "not_found_1");
    assert_both_null(str, 'X', strlen(str), "not_found_2");
    assert_both_null(str, 'z', 5, "not_found_3");
}
END_TEST

// Тест 6: Поиск нулевого символа '\0'
START_TEST(test_find_null_terminator_standard) {
    char str[] = "Hello\0World";
    compare_memchr_results(str, '\0', sizeof(str), "null_terminator");
    compare_memchr_results(str, '\0', 6, "null_terminator_2");

    void *std_result = memchr(str, '\0', 5);
    void *our_result = s21_memchr(str, '\0', 5);
    ck_assert_ptr_eq(std_result, NULL);
    ck_assert_ptr_eq(our_result, NULL);
}
END_TEST

// Тест 7: Поиск с ограничением n (символ за пределами n)
START_TEST(test_find_beyond_n_standard) {
    const char *str = "Hello World";
    
    compare_memchr_results(str, 'W', 5, "beyond_n_1");
    compare_memchr_results(str, 'd', 10, "beyond_n_2");
    compare_memchr_results(str, 'd', 11, "beyond_n_3");
}
END_TEST

// Тест 8: Поиск в бинарных данных
START_TEST(test_binary_data_standard) {
    unsigned char data[] = {0x00, 0x01, 0x02, 0xFF, 0x03, 0x00};
    
    compare_memchr_results(data, 0xFF, sizeof(data), "binary_1");
    compare_memchr_results(data, 0x00, sizeof(data), "binary_2");
    
    // Не должен найти второй 0x00
    void *std_result = memchr(data, 0x00, 3);
    void *our_result = s21_memchr(data, 0x00, 3);
    ck_assert_ptr_eq(std_result, data); // Должен найти первый байт
    ck_assert_ptr_eq(our_result, data);
}
END_TEST

// Тест 9: Отрицательные значения int c
START_TEST(test_negative_char_standard) {
    char str[] = "Hello\xFFWorld";
    
    compare_memchr_results(str, -1, sizeof(str), "negative_char_1");
    compare_memchr_results(str, (int)(unsigned char)0xFF, sizeof(str), "negative_char_2");
    
    // Проверка преобразования отрицательных значений
    void *std_result = memchr(str, (int)(char)0xFF, sizeof(str));
    void *our_result = s21_memchr(str, (int)(char)0xFF, sizeof(str));
    ck_assert_ptr_eq(std_result, our_result);
}
END_TEST

// Тест 10: Большие значения int c (за пределами char)
START_TEST(test_large_int_char_standard) {
    const char *str = "Hello World";
    
    // 0x101 = 257, после приведения к unsigned char -> 1
    compare_memchr_results(str, 0x101, strlen(str), "large_int_1");
    
    // 'e' = 101, 101 + 256 = 357, после приведения -> 101
    compare_memchr_results(str, 357, strlen(str), "large_int_2");
    
    // Несуществующий символ
    assert_both_null(str, 1000, strlen(str), "large_int_3");
}
END_TEST

// Тест 11: Пограничные значения n
START_TEST(test_boundary_n_standard) {
    const char *str = "ABCD";
    
    compare_memchr_results(str, 'A', 1, "boundary_n_1");
    compare_memchr_results(str, 'B', 2, "boundary_n_2");
    compare_memchr_results(str, 'C', 3, "boundary_n_3");
    compare_memchr_results(str, 'D', 4, "boundary_n_4");
    
    // Не должен найти
    assert_both_null(str, 'D', 3, "boundary_n_5");
}
END_TEST

// Тест 12: Поиск в массиве с повторяющимися символами
START_TEST(test_repeated_chars_standard) {
    const char *str = "AAAABBBBCCCC";
    
    compare_memchr_results(str, 'A', strlen(str), "repeated_1");
    compare_memchr_results(str, 'B', strlen(str), "repeated_2");
    compare_memchr_results(str, 'C', strlen(str), "repeated_3");
    compare_memchr_results(str, 'B', 5, "repeated_4");
}
END_TEST

Suite *memchr_suite(void){
    Suite *s = suite_create("s21_memchr");
    TCase *tc_core = tcase_create("Core");
    TCase *tc_boundary = tcase_create("Boundary");

    // Основные тесты
    tc_core = tcase_create("Core Comparison");
    tcase_add_test(tc_core, test_zero_length_standard);
    tcase_add_test(tc_core, test_find_at_beginning_standard);
    tcase_add_test(tc_core, test_find_in_middle_standard);
    tcase_add_test(tc_core, test_find_at_end_standard);
    tcase_add_test(tc_core, test_char_not_found_standard);
    tcase_add_test(tc_core, test_find_null_terminator_standard);
    tcase_add_test(tc_core, test_find_beyond_n_standard);
    suite_add_tcase(s, tc_core);
    
    // Граничные случаи
    tc_boundary = tcase_create("Boundary Cases");
    tcase_add_test(tc_boundary, test_binary_data_standard);
    tcase_add_test(tc_boundary, test_negative_char_standard);
    tcase_add_test(tc_boundary, test_large_int_char_standard);
    tcase_add_test(tc_boundary, test_boundary_n_standard);
    tcase_add_test(tc_boundary, test_repeated_chars_standard);
    suite_add_tcase(s, tc_boundary);

    return s;
}