#ifndef TEST_S21_MEMCMP_H
#define TEST_S21_MEMCMP_H

#include "check.h"

Suite *memcmp_suite(void);

void compare_memcmp_results(const void *str1, const void *str2,
                            size_t n /*, const char *test_name*/);

#endif  // TEST_S21_MEMCMP_H