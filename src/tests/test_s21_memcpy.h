#ifndef TEST_S21_MEMCPY_H
#define TEST_S21_MEMCPY_H

#include "check.h"

Suite *memcpy_suite(void);

void compare_memcpy_results(const void *src_data, size_t n, int has_overlap,
                            int dest_offset);
void test_simple_memcpy(const void *src_data, size_t n);
void test_overlap_memcpy(const void *src_data, size_t n, int dest_offset);
void *random_data(size_t size);

#endif  // TEST_S21_MEMCPY_H