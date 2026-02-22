#ifndef TEST_STRNCPY_H
#define TEST_STRNCPY_H

#include <check.h>

Suite* strncpy_suite(void);

/* Вспомогательные функции */
char* random_string(int length);

#endif /* TEST_STRNCPY_H */