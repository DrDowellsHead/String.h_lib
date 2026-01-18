#include <s21_string.h>

char s21_strpbrk(const char *str1, const char *str2) {
    if (str1 == S21_NULL || str2 == S21_NULL) {
        return S21_NULL;
    }

    while (*str1 != '\0') {
        const char *check = str2;

        while (*check != '\0') {
            if (*str1 == *check) {
                return (char *)str1;
            }
            check++;
        }
        str1++;
    }

    return S21_NULL;
}