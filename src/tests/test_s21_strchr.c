#include <stdlib.h>
#include <string.h>

#include "../s21_string.h"
#include "test_s21_strchr.h"

// ------------------------------------------------------------------
// Базовые тесты: поиск символа в строке
// ------------------------------------------------------------------
START_TEST(test_s21_strchr_basic) {
    const char *str = "Hello, world!";
    int c = 'o';
    
    char *result = s21_strchr(str, c);
    char *expected = strchr(str, c);
    
    ck_assert_ptr_eq(result, expected);
    ck_assert_int_eq(result - str, 4); // первое 'o' на позиции 4
}
END_TEST

START_TEST(test_s21_strchr_multiple) {
    const char *str = "abacaba";
    int c = 'a';
    
    char *result = s21_strchr(str, c);
    
    ck_assert_int_eq(result - str, 0); // первая 'a' на позиции 0
}
END_TEST

// ------------------------------------------------------------------
// Тесты: символ не найден
// ------------------------------------------------------------------
START_TEST(test_s21_strchr_not_found) {
    const char *str = "Hello";
    int c = 'x';
    
    char *result = s21_strchr(str, c);
    
    ck_assert_ptr_null(result);
}
END_TEST

// ------------------------------------------------------------------
// Тесты: поиск нулевого символа
// ------------------------------------------------------------------
START_TEST(test_s21_strchr_null_char) {
    const char *str = "Hello";
    int c = '\0';
    
    char *result = s21_strchr(str, c);
    char *expected = strchr(str, c);
    
    ck_assert_ptr_eq(result, expected);
    ck_assert_int_eq(result - str, 5); // указатель на терминирующий ноль
}
END_TEST

// ------------------------------------------------------------------
// Тесты: пустая строка
// ------------------------------------------------------------------
START_TEST(test_s21_strchr_empty_string) {
    const char *str = "";
    
    // Поиск обычного символа
    int c1 = 'a';
    char *result1 = s21_strchr(str, c1);
    ck_assert_ptr_null(result1);
    
    // Поиск нулевого символа
    int c2 = '\0';
    char *result2 = s21_strchr(str, c2);
    ck_assert_ptr_eq(result2, str); // в пустой строке нуль на позиции 0
}
END_TEST

// ------------------------------------------------------------------
// Тесты: нулевой указатель
// ------------------------------------------------------------------
START_TEST(test_s21_strchr_null_pointer) {
    const char *str = NULL;
    int c = 'a';
    
    char *result = s21_strchr(str, c);
    
    ck_assert_ptr_null(result);
}
END_TEST

// ------------------------------------------------------------------
// Тесты: специальные символы и граничные значения
// ------------------------------------------------------------------
START_TEST(test_s21_strchr_special_chars) {
    // Строка с различными специальными символами
    char str[] = {'\t', '\n', '\r', ' ', '!', '@', '#', '$', '%', 0};
    
    // Поиск табуляции
    int c1 = '\t';
    char *result1 = s21_strchr(str, c1);
    ck_assert_int_eq(result1 - str, 0);
    
    // Поиск перевода строки
    int c2 = '\n';
    char *result2 = s21_strchr(str, c2);
    ck_assert_int_eq(result2 - str, 1);
    
    // Поиск пробела
    int c3 = ' ';
    char *result3 = s21_strchr(str, c3);
    ck_assert_int_eq(result3 - str, 3);
}
END_TEST

// ------------------------------------------------------------------
// Тесты: отрицательные значения символов
// ------------------------------------------------------------------
START_TEST(test_s21_strchr_negative_char) {
    // Создаём строку, содержащую символ с кодом 255
    unsigned char str_bytes[] = {255, 'a', 'b', 255, 'c', 0};
    char *str = (char *)str_bytes;
    
    // Ищем символ с кодом -1 (255)
    int c = -1;
    char *result = s21_strchr(str, c);
    
    ck_assert_int_eq(result - str, 0); // первое вхождение на позиции 0
}
END_TEST

// ------------------------------------------------------------------
// Тесты: беззнаковые значения (больше 127)
// ------------------------------------------------------------------
START_TEST(test_s21_strchr_unsigned_char) {
    // Создаём строку с символами >127
    unsigned char str_bytes[] = {200, 150, 200, 250, 0};
    char *str = (char *)str_bytes;
    
    // Ищем символ 200
    int c = 200;
    char *result = s21_strchr(str, c);
    
    ck_assert_int_eq(result - str, 0); // первое вхождение на позиции 0
}
END_TEST

// ------------------------------------------------------------------
// Тесты: поиск первого символа в строке
// ------------------------------------------------------------------
START_TEST(test_s21_strchr_first_char) {
    const char *str = "abcdef";
    int c = 'a';
    
    char *result = s21_strchr(str, c);
    
    ck_assert_int_eq(result - str, 0);
}
END_TEST

// ------------------------------------------------------------------
// Тесты: поиск последнего символа в строке
// ------------------------------------------------------------------
START_TEST(test_s21_strchr_last_char) {
    const char *str = "abcdef";
    int c = 'f';
    
    char *result = s21_strchr(str, c);
    
    ck_assert_int_eq(result - str, 5);
}
END_TEST

// ------------------------------------------------------------------
// Тесты: поиск в длинной строке
// ------------------------------------------------------------------
START_TEST(test_s21_strchr_long_string) {
    // Создаём длинную строку
    char *str = malloc(10001);
    for (int i = 0; i < 9999; i++) {
        str[i] = 'a' + (i % 26);
    }
    str[9999] = 'X'; // специальный символ
    str[10000] = '\0';
    
    // Ищем символ 'X' в конце
    int c = 'X';
    char *result = s21_strchr(str, c);
    
    ck_assert_int_eq(result - str, 9999);
    
    free(str);
}
END_TEST

// ------------------------------------------------------------------
// Тесты: сравнение с оригинальной strchr (системная)
// ------------------------------------------------------------------
START_TEST(test_s21_strchr_vs_system) {
    // Тестовые строки
    const char *strings[] = {
        "Hello, world!",
        "abcdefghijklmnopqrstuvwxyz",
        "0123456789",
        "!@#$%^&*()",
        "",
        "a",
        "ab"
    };
    
    // Тестовые символы
    int chars[] = {'a', 'z', '0', '9', '!', '@', ' ', '\0', 'x', 'A'};
    
    for (size_t i = 0; i < sizeof(strings)/sizeof(strings[0]); i++) {
        for (size_t j = 0; j < sizeof(chars)/sizeof(chars[0]); j++) {
            const char *str = strings[i];
            int c = chars[j];
            
            char *result = s21_strchr(str, c);
            char *expected = strchr(str, c);
            
            ck_assert_ptr_eq(result, expected);
        }
    }
}
END_TEST

// ------------------------------------------------------------------
// Сборка suite'а
// ------------------------------------------------------------------
Suite *strchr_suite(void) {
    Suite *s = suite_create("s21_strchr");
    TCase *tc_core = tcase_create("Core");

    // Базовые тесты
    tcase_add_test(tc_core, test_s21_strchr_basic);
    tcase_add_test(tc_core, test_s21_strchr_multiple);
    tcase_add_test(tc_core, test_s21_strchr_not_found);
    tcase_add_test(tc_core, test_s21_strchr_null_char);
    tcase_add_test(tc_core, test_s21_strchr_empty_string);
    tcase_add_test(tc_core, test_s21_strchr_null_pointer);
    
    // Специальные символы
    tcase_add_test(tc_core, test_s21_strchr_special_chars);
    
    // Граничные значения
    tcase_add_test(tc_core, test_s21_strchr_negative_char);
    tcase_add_test(tc_core, test_s21_strchr_unsigned_char);
    
    // Крайние случаи
    tcase_add_test(tc_core, test_s21_strchr_first_char);
    tcase_add_test(tc_core, test_s21_strchr_last_char);
    tcase_add_test(tc_core, test_s21_strchr_long_string);
    
    // Сравнение с системной реализацией
    tcase_add_test(tc_core, test_s21_strchr_vs_system);

    suite_add_tcase(s, tc_core);
    return s;
}