#ifndef TEST_S21_MEMSET_H
#define TEST_S21_MEMSET_H

#include "check.h"

Suite *memset_suite(void);

void compare_memset_result(const void *initial_data, size_t data_size,
                           int fill_char, size_t fill_size);
#endif  // TEST_S21_MEMSET_H