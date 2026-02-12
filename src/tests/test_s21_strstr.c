#include "../s21_string.h"
#include "test_s21_strstr.h"

// Тест 1: Поиск подстроки в начале строки
START_TEST(test_s21_strstr_beginning) {
    char haystack[] = "Hello, World!";
    char needle[] = "Hello";
    
    char *result1 = s21_strstr(haystack, needle);
    char *result2 = strstr(haystack, needle);
    
    ck_assert_ptr_eq(result1, result2);
    if (result1) ck_assert_str_eq(result1, result2);
}
END_TEST

// Тест 2: Поиск подстроки в середине строки
START_TEST(test_s21_strstr_middle) {
    char haystack[] = "Hello, World!";
    char needle[] = "World";
    
    char *result1 = s21_strstr(haystack, needle);
    char *result2 = strstr(haystack, needle);
    
    ck_assert_ptr_eq(result1, result2);
    if (result1) ck_assert_str_eq(result1, result2);
}
END_TEST

// Тест 3: Поиск подстроки в конце строки
START_TEST(test_s21_strstr_end) {
    char haystack[] = "Hello, World!";
    char needle[] = "World!";
    
    char *result1 = s21_strstr(haystack, needle);
    char *result2 = strstr(haystack, needle);
    
    ck_assert_ptr_eq(result1, result2);
    if (result1) ck_assert_str_eq(result1, result2);
}
END_TEST

// Тест 4: Поиск несуществующей подстроки
START_TEST(test_s21_strstr_not_found) {
    char haystack[] = "Hello, World!";
    char needle[] = "xyz";
    
    char *result1 = s21_strstr(haystack, needle);
    char *result2 = strstr(haystack, needle);
    
    ck_assert_ptr_eq(result1, result2);
}
END_TEST

// Тест 5: Пустая искомая подстрока (needle = "")
START_TEST(test_s21_strstr_empty_needle) {
    char haystack[] = "Hello, World!";
    char needle[] = "";
    
    char *result1 = s21_strstr(haystack, needle);
    char *result2 = strstr(haystack, needle);
    
    ck_assert_ptr_eq(result1, result2);
    ck_assert_str_eq(result1, result2);
}
END_TEST

// Тест 6: Пустая строка для поиска (haystack = "")
START_TEST(test_s21_strstr_empty_haystack) {
    char haystack[] = "";
    char needle[] = "Hello";
    
    char *result1 = s21_strstr(haystack, needle);
    char *result2 = strstr(haystack, needle);
    
    ck_assert_ptr_eq(result1, result2);
}
END_TEST

// Тест 7: Обе строки пустые
START_TEST(test_s21_strstr_both_empty) {
    char haystack[] = "";
    char needle[] = "";
    
    char *result1 = s21_strstr(haystack, needle);
    char *result2 = strstr(haystack, needle);
    
    ck_assert_ptr_eq(result1, result2);
    ck_assert_str_eq(result1, result2);
}
END_TEST

// Тест 8: Поиск подстроки длиннее строки
START_TEST(test_s21_strstr_needle_longer) {
    char haystack[] = "Hello";
    char needle[] = "Hello, World!";
    
    char *result1 = s21_strstr(haystack, needle);
    char *result2 = strstr(haystack, needle);
    
    ck_assert_ptr_eq(result1, result2);
}
END_TEST

// Тест 9: Поиск одиночного символа
START_TEST(test_s21_strstr_single_char) {
    char haystack[] = "Hello, World!";
    char needle[] = "o";
    
    char *result1 = s21_strstr(haystack, needle);
    char *result2 = strstr(haystack, needle);
    
    ck_assert_ptr_eq(result1, result2);
    if (result1) ck_assert_str_eq(result1, result2);
}
END_TEST

// Тест 10: Поиск в строке с повторяющимися паттернами
START_TEST(test_s21_strstr_repeated_pattern) {
    char haystack[] = "ababababc";
    char needle[] = "ababc";
    
    char *result1 = s21_strstr(haystack, needle);
    char *result2 = strstr(haystack, needle);
    
    ck_assert_ptr_eq(result1, result2);
    if (result1) ck_assert_str_eq(result1, result2);
}
END_TEST


// Тест 11: Поиск с регистрозависимостью
START_TEST(test_s21_strstr_case_sensitive) {
    char haystack[] = "Hello, World!";
    char needle[] = "world";  // с маленькой буквы
    
    char *result1 = s21_strstr(haystack, needle);
    char *result2 = strstr(haystack, needle);
    
    ck_assert_ptr_eq(result1, result2);
}
END_TEST

// Тест 12: Поиск в строке со спецсимволами
START_TEST(test_s21_strstr_special_chars) {
    char haystack[] = "Hello\n\t\r\vWorld!";
    char needle[] = "\n\t\r\v";
    
    char *result1 = s21_strstr(haystack, needle);
    char *result2 = strstr(haystack, needle);
    
    ck_assert_ptr_eq(result1, result2);
    if (result1) ck_assert_str_eq(result1, result2);
}
END_TEST

// Тест 13: Поиск всей строки целиком
START_TEST(test_s21_strstr_whole_string) {
    char haystack[] = "Hello, World!";
    char needle[] = "Hello, World!";
    
    char *result1 = s21_strstr(haystack, needle);
    char *result2 = strstr(haystack, needle);
    
    ck_assert_ptr_eq(result1, result2);
    ck_assert_str_eq(result1, result2);
}
END_TEST

Suite *strstr_suite(void){
    Suite *s = suite_create("s21_strstr");
    TCase *tc_core = tcase_create("Core");
    TCase *tc_boundary = tcase_create("Boundary");

    // Основные тесты
    tc_core = tcase_create("Core Tests");
    tcase_add_test(tc_core, test_s21_strstr_beginning);
    tcase_add_test(tc_core, test_s21_strstr_middle);
    tcase_add_test(tc_core, test_s21_strstr_end);
    tcase_add_test(tc_core, test_s21_strstr_not_found);
    tcase_add_test(tc_core, test_s21_strstr_single_char);
    tcase_add_test(tc_core, test_s21_strstr_whole_string);
    suite_add_tcase(s, tc_core);

    // Граничные случаи
    tc_boundary = tcase_create("Boundary Cases");
    tcase_add_test(tc_boundary, test_s21_strstr_empty_needle);
    tcase_add_test(tc_boundary, test_s21_strstr_empty_haystack);
    tcase_add_test(tc_boundary, test_s21_strstr_both_empty);
    tcase_add_test(tc_boundary, test_s21_strstr_needle_longer);
    tcase_add_test(tc_boundary, test_s21_strstr_repeated_pattern);
    tcase_add_test(tc_boundary, test_s21_strstr_case_sensitive);
    tcase_add_test(tc_boundary, test_s21_strstr_special_chars);
    suite_add_tcase(s, tc_boundary);

    return s;
}