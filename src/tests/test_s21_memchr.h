#ifndef TEST_S21_MEMCHR_H
#define TEST_S21_MEMCHR_H
#include "check.h"

Suite *memchr_suite(void);
//вспомогательные функции
void compare_memchr_results(const void *str, int c, size_t n, const char *test_name);
void assert_both_null(const void *str, int c, size_t n, const char *test_name);
#endif // TEST_S21_MEMCHR_H