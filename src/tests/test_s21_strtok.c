#include "../s21_string.h"
#include "test_s21_strtok.h"

// Тест 1: Нормальный случай - разделение строки по одному разделителю
START_TEST(test_s21_strtok_normal_single_delim) {
    char str1[] = "Hello,World,How,Are,You";
    char str2[] = "Hello,World,How,Are,You";
    const char *delim = ",";
    
    char *s21_result = s21_strtok(str1, delim);
    char *orig_result = strtok(str2, delim);
    ck_assert_str_eq(s21_result, orig_result);   
}
END_TEST

// Тест 2: Несколько разделителей подряд
START_TEST(test_s21_strtok_multiple_delimiters) {
    char str1[] = "Hello,,,World,,How,,Are,,You";
    char str2[] = "Hello,,,World,,How,,Are,,You";
    const char *delim = ",";
    
    char *s21_result = s21_strtok(str1, delim);
    char *orig_result = strtok(str2, delim);
    ck_assert_str_eq(s21_result, orig_result);
}
END_TEST

// Тест 3: Разные наборы разделителей
START_TEST(test_s21_strtok_different_delim_sets) {
    char str1[] = "Hello, World! How are you?";
    char str2[] = "Hello, World! How are you?";
    const char *delim = " ,!?";
    
    char *s21_result = s21_strtok(str1, delim);
    char *orig_result = strtok(str2, delim);
    ck_assert_str_eq(s21_result, orig_result);
}
END_TEST

// Тест 4: Строка начинается с разделителей
START_TEST(test_s21_strtok_starts_with_delim) {
    char str1[] = ",,,Hello,World";
    char str2[] = ",,,Hello,World";
    const char *delim = ",";
    
    char *s21_result = s21_strtok(str1, delim);
    char *orig_result = strtok(str2, delim);
    ck_assert_str_eq(s21_result, orig_result); 
}
END_TEST

// Тест 5: Строка заканчивается разделителем
START_TEST(test_s21_strtok_ends_with_delim) {
    char str1[] = "Hello,World,";
    char str2[] = "Hello,World,";
    const char *delim = ",";
    
    char *s21_result = s21_strtok(str1, delim);
    char *orig_result = strtok(str2, delim);
    ck_assert_str_eq(s21_result, orig_result);
}
END_TEST

// Тест 6: Разделители - спецсимволы
START_TEST(test_s21_strtok_special_chars_delim) {
    char str1[] = "Hello\nWorld\tHow\rAre";
    char str2[] = "Hello\nWorld\tHow\rAre";
    const char *delim = "\n\t\r";
    
    char *s21_result = s21_strtok(str1, delim);
    char *orig_result = strtok(str2, delim);
    ck_assert_str_eq(s21_result, orig_result);
}
END_TEST

/* ============= ГРАНИЧНЫЕ ТЕСТЫ ============= */

// Тест 7: Пустая строка 
START_TEST(test_s21_strtok_empty_string) {
    // Пустая строка
    char str1[] = "";
    const char *delim = ",";
    
    char *s21_result = s21_strtok(str1, delim);
    char *orig_result = strtok(str1, delim);
    ck_assert_ptr_eq(s21_result, orig_result);
}
END_TEST

// Тест 8: Пустой набор разделителей
START_TEST(test_s21_strtok_empty_delim) {
    char str1[] = "Hello World";
    const char *delim = "";
    
    char *s21_result = s21_strtok(str1, delim);
    char *orig_result = strtok(str1, delim);
    ck_assert_str_eq(s21_result, orig_result);
}
END_TEST


// Создание набора тестов
Suite *strtok_suite(void) {
    Suite *s = suite_create("s21_strtok");
    TCase *tc_core = tcase_create("Core");
    TCase *tc_boundary = tcase_create("Boundary");
    
    // Основные тесты
    tcase_add_test(tc_core, test_s21_strtok_normal_single_delim);
    tcase_add_test(tc_core, test_s21_strtok_multiple_delimiters);
    tcase_add_test(tc_core, test_s21_strtok_different_delim_sets);
    tcase_add_test(tc_core, test_s21_strtok_starts_with_delim);
    tcase_add_test(tc_core, test_s21_strtok_ends_with_delim);
    tcase_add_test(tc_core, test_s21_strtok_special_chars_delim);
    suite_add_tcase(s, tc_core);
    
    // Граничные случаи
    tcase_add_test(tc_boundary, test_s21_strtok_empty_string);
    tcase_add_test(tc_boundary, test_s21_strtok_empty_delim);
    suite_add_tcase(s, tc_boundary);
    
    return s;
}

