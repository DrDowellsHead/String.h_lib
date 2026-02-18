#include "../s21_string.h"
#include "test_s21_strpbrk.h"

// Тест 1: Нормальный случай - поиск символов из набора
START_TEST(test_s21_strpbrk_normal) {
    const char *str1 = "Hello, World!";
    const char *str2 = " ,!";
    
    char *s21_result = s21_strpbrk(str1, str2);
    char *orig_result = strpbrk(str1, str2);
    
    ck_assert_ptr_eq(s21_result, orig_result);
}
END_TEST

// Тест 2: Нет совпадений - возвращается NULL
START_TEST(test_s21_strpbrk_no_match) {
    const char *str1 = "Hello";
    const char *str2 = "xyz";
    
    char *s21_result = s21_strpbrk(str1, str2);
    char *orig_result = strpbrk(str1, str2);
    
    ck_assert_ptr_eq(s21_result, orig_result);

}
END_TEST

// Тест 3: Совпадение в начале строки
START_TEST(test_s21_strpbrk_match_at_start) {
    const char *str1 = "Hello";
    const char *str2 = "H";
    
    char *s21_result = s21_strpbrk(str1, str2);
    char *orig_result = strpbrk(str1, str2);
    
    ck_assert_ptr_eq(s21_result, orig_result);
}
END_TEST

// Тест 4: Совпадение в конце строки
START_TEST(test_s21_strpbrk_match_at_end) {
    const char *str1 = "abcdef123456";
    const char *str2 = "6";
    
    char *s21_result = s21_strpbrk(str1, str2);
    char *orig_result = strpbrk(str1, str2);
    
    ck_assert_ptr_eq(s21_result, orig_result);
}
END_TEST

// Тест 5: Совпадение в середине строки
START_TEST(test_s21_strpbrk_match_at_middle) {
    const char *str1 = "abcdef123456";
    const char *str2 = "f1";
    
    char *s21_result = s21_strpbrk(str1, str2);
    char *orig_result = strpbrk(str1, str2);
    
    ck_assert_ptr_eq(s21_result, orig_result);
}
END_TEST

// Тест 6: Регистрозависимость
START_TEST(test_s21_strpbrk_case_sensitive) {
    const char *str1 = "Hello World";
    const char *str2 = "hw";
    
    char *s21_result = s21_strpbrk(str1, str2);
    char *orig_result = strpbrk(str1, str2);
    
    ck_assert_ptr_eq(s21_result, orig_result);
}
END_TEST

/* ============= ГРАНИЧНЫЕ ТЕСТЫ  ============= */

// Тест 7: Пустые строки
START_TEST(test_s21_strpbrk_empty_strings) {
    // Пустой str2
    const char *str1 = "Hello";
    const char *str2_empty = "";
    char *s21_result1 = s21_strpbrk(str1, str2_empty);
    char *orig_result1 = strpbrk(str1, str2_empty);
    ck_assert_ptr_eq(s21_result1, orig_result1);
    
    // Пустой str1
    const char *str1_empty = "";
    const char *str2 = "abc";
    char *s21_result2 = s21_strpbrk(str1_empty, str2);
    char *orig_result2 = strpbrk(str1_empty, str2);
    ck_assert_ptr_eq(s21_result2, orig_result2);
    
    // Обе пустые
    char *s21_result3 = s21_strpbrk(str1_empty, str2_empty);
    char *orig_result3 = strpbrk(str1_empty, str2_empty);
    ck_assert_ptr_eq(s21_result3, orig_result3);
}
END_TEST

// Тест 9: Длинные строки и специальные символы
START_TEST(test_s21_strpbrk_long_and_special) {
    // Длинная строка с совпадением в конце
    char long_str[1000];
    for (int i = 0; i < 998; i++) {
        long_str[i] = 'a' + (i % 26);
    }
    long_str[998] = 'Z';
    long_str[999] = '\0';
    
    char *s21_result1 = s21_strpbrk(long_str, "Z");
    char *orig_result1 = strpbrk(long_str, "Z");
    ck_assert_ptr_eq(s21_result1, orig_result1);
    
    // Специальные символы
    const char *str_special = "Hello\n\tWorld\r";
    const char *str2_special = "\n\t\r";
    char *s21_result2 = s21_strpbrk(str_special, str2_special);
    char *orig_result2 = strpbrk(str_special, str2_special);
    ck_assert_ptr_eq(s21_result2, orig_result2);
}
END_TEST

// Тест 10: Крайние случаи с символами
START_TEST(test_s21_strpbrk_edge_chars) {
    // Первый символ из нескольких возможных
    const char *str1_multiple = "abcdef";
    const char *str2_multiple = "cf";
    char *s21_result1 = s21_strpbrk(str1_multiple, str2_multiple);
    char *orig_result1 = strpbrk(str1_multiple, str2_multiple);
    ck_assert_ptr_eq(s21_result1, orig_result1);
    
    // Нулевой символ в середине
    char str1_null[] = "abc\0def";
    const char *str2_null = "d";
    char *s21_result2 = s21_strpbrk(str1_null, str2_null);
    char *orig_result2 = strpbrk(str1_null, str2_null);
    ck_assert_ptr_eq(s21_result2, orig_result2);
        
    // Кириллица
    const char *str1_cyr = "Привет, Мир!";
    const char *str2_cyr = "М,";
    char *s21_result3 = s21_strpbrk(str1_cyr, str2_cyr);
    char *orig_result3 = strpbrk(str1_cyr, str2_cyr);
    ck_assert_ptr_eq(s21_result3, orig_result3);
}
END_TEST

Suite *strpbrk_suite(void) {
    Suite *s = suite_create("s21_strpbrk");
    TCase *tc_core = tcase_create("Core");
    TCase *tc_boundary = tcase_create("Boundary");
    
    // Основные тесты
    tcase_add_test(tc_core, test_s21_strpbrk_normal);
    tcase_add_test(tc_core, test_s21_strpbrk_no_match);
    tcase_add_test(tc_core, test_s21_strpbrk_match_at_start);
    tcase_add_test(tc_core, test_s21_strpbrk_match_at_end);
    tcase_add_test(tc_core, test_s21_strpbrk_match_at_middle);
    tcase_add_test(tc_core, test_s21_strpbrk_case_sensitive);
    suite_add_tcase(s, tc_core);
    
    // Граничные случаи
    tcase_add_test(tc_boundary, test_s21_strpbrk_empty_strings);
    tcase_add_test(tc_boundary, test_s21_strpbrk_long_and_special);
    tcase_add_test(tc_boundary, test_s21_strpbrk_edge_chars);
    suite_add_tcase(s, tc_boundary);
    
    return s;
}
