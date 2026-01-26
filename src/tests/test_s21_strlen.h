#ifndef TEST_STRLEN_H
#define TEST_STRLEN_H

#include <check.h>

Suite *strlen_suite(void);

/* Вспомогательные функции */
char *create_long_string(size_t length);
void free_long_string(char *str);

#endif /* TEST_STRLEN_H */