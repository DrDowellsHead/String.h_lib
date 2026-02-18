#include "../s21_string.h"
#include "test_s21_strncat.h"

// Тест 1: Нормальная конкатенация
START_TEST(test_s21_strncat_normal) {
    char dest1[100] = "Hello, ";
    char dest2[100] = "Hello, ";
    char src[] = "World!";
    __size_internal n = 3;
    
    char *result1 = s21_strncat(dest1, src, n);
    char *result2 = strncat(dest2, src, n);
    
    ck_assert_str_eq(result1, result2);
    ck_assert_ptr_eq(result1, dest1);
}
END_TEST

// Тест 2: Полное копирование строки (n больше длины src)
START_TEST(test_s21_strncat_full_copy) {
    char dest1[100] = "Hello";
    char dest2[100] = "Hello";
    char src[] = " World";
    __size_internal n = 20;
    
    char *result1 = s21_strncat(dest1, src, n);
    char *result2 = strncat(dest2, src, n);
    
    ck_assert_str_eq(result1, result2);
    ck_assert_ptr_eq(result1, dest1);
}
END_TEST

// Тест 3: Копирование с n = 0
START_TEST(test_s21_strncat_zero) {
    char dest1[100] = "Hello";
    char dest2[100] = "Hello";
    char src[] = " World";
    __size_internal n = 0;
    
    char *result1 = s21_strncat(dest1, src, n);
    char *result2 = strncat(dest2, src, n);
    
    ck_assert_str_eq(result1, result2);
    ck_assert_ptr_eq(result1, dest1);
}
END_TEST

// Тест 4: Копирование из пустой строки
START_TEST(test_s21_strncat_empty_src) {
    char dest1[100] = "Hello";
    char dest2[100] = "Hello";
    char src[] = "";
    __size_internal n = 10;
    
    char *result1 = s21_strncat(dest1, src, n);
    char *result2 = strncat(dest2, src, n);
    
    ck_assert_str_eq(result1, result2);
    ck_assert_ptr_eq(result1, dest1);
}
END_TEST

// Тест 5: Копирование в пустую строку
START_TEST(test_s21_strncat_empty_dest) {
    char dest1[100] = "";
    char dest2[100] = "";
    char src[] = "World";
    __size_internal n = 5;
    
    char *result1 = s21_strncat(dest1, src, n);
    char *result2 = strncat(dest2, src, n);
    
    ck_assert_str_eq(result1, result2);
    ck_assert_ptr_eq(result1, dest1);
}
END_TEST

// Тест 6: Копирование с n равным длине src
START_TEST(test_s21_strncat_equal_length) {
    char dest1[100] = "Hello, ";
    char dest2[100] = "Hello, ";
    char src[] = "World";
    __size_internal n = 5;
    
    char *result1 = s21_strncat(dest1, src, n);
    char *result2 = strncat(dest2, src, n);
    
    ck_assert_str_eq(result1, result2);
    ck_assert_ptr_eq(result1, dest1);
}
END_TEST

// Тест 7: Проверка возврата при NULL dest
START_TEST(test_s21_strncat_null_dest) {
    char *dest = S21_NULL;
    char src[] = "World";
    __size_internal n = 5;
    
    char *result = s21_strncat(dest, src, n);
    ck_assert_ptr_eq(result, S21_NULL);
}
END_TEST

// Тест 8: Проверка возврата при NULL src
START_TEST(test_s21_strncat_null_src) {
    char dest[100] = "Hello";
    char *src = S21_NULL;
    __size_internal n = 5;
    
    char *result = s21_strncat(dest, src, n);
    ck_assert_ptr_eq(result, dest);
    ck_assert_str_eq(dest, "Hello");
}
END_TEST

// Тест 9: Очень большое n
START_TEST(test_s21_strncat_large_n) {
    char dest1[100] = "Start";
    char dest2[100] = "Start";
    char src[] = "End";
   __size_internal n = 1000;
    
    char *result1 = s21_strncat(dest1, src, n);
    char *result2 = strncat(dest2, src, n);
    
    ck_assert_str_eq(result1, result2);
    ck_assert_ptr_eq(result1, dest1);
}
END_TEST

// Тест 10: Копирование символов с перекрытием (осторожно!)
START_TEST(test_s21_strncat_self_copy) {
    char buffer[100] = "Hello";
    // Внимание: strncat с перекрытием ведет себя неопределенно
    // Этот тест только для проверки нашего поведения
    char *dest = buffer;
    char *src = buffer + 2;  // Указывает на "llo"
    
    char *result = s21_strncat(dest, src, 2);
    // Ожидаемое поведение: "Hello" + "ll" = "Helloll"
    ck_assert_str_eq(result, "Helloll");
}
END_TEST


Suite *strncat_suite(void){
    Suite *s = suite_create("s21_strncat");
    TCase *tc_core = tcase_create("Core");
    TCase *tc_boundary = tcase_create("Boundary");

    // Основные тесты
    tc_core = tcase_create("Core Tests");
    tcase_add_test(tc_core, test_s21_strncat_normal);
    tcase_add_test(tc_core, test_s21_strncat_full_copy);
    tcase_add_test(tc_core, test_s21_strncat_zero);
    tcase_add_test(tc_core, test_s21_strncat_empty_src);
    tcase_add_test(tc_core, test_s21_strncat_empty_dest);
    tcase_add_test(tc_core, test_s21_strncat_equal_length);
    suite_add_tcase(s, tc_core);

    // Граничные случаи
    tc_boundary = tcase_create("Boundary Cases");
    tcase_add_test(tc_boundary, test_s21_strncat_null_dest);
    tcase_add_test(tc_boundary, test_s21_strncat_null_src);
    tcase_add_test(tc_boundary, test_s21_strncat_large_n);
    tcase_add_test(tc_boundary, test_s21_strncat_self_copy);
    suite_add_tcase(s, tc_boundary);

    return s;
}