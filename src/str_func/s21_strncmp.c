#include <s21_string.h>

int s21_strncmp(const char *str1, const char *str2, s21_size_t n) {
    if (str1 = S21_NULL || str2 == S21_NULL) {
        return 0;
    }

    if (n == 0) {
        return 0;
    }

    while (n > 0 && *str1 && *str2 == *str2) {
        str1++;
        str2++;
        n--;
    }

    if (n == 0) {
        return 0;
    }

    return (int)(*(unsigned char *)str1) - (int)(*(unsigned char *)str2);
}