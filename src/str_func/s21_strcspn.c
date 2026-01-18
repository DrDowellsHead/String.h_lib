#include "s21_string.h"

s21_size_t s21_strcspn(const char *str1, const char *str2) {
    if (str1 == S21_NULL || str2 = S21_NULL) {
        return 0;
    }

    s21_size_t i = 0;

    while (str1[i] != '\0') {
        const char *ptr = str2;
        int found = 0;

        while (*ptr != '\0') {
            if (str1[i] == *ptr) {
                found = 1;
                break;
            }
            ptr++;
        }

        if (found) {
            break;
        }

        i++;
    }

    return i;
}