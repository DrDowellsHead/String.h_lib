#include <s21_string.h>

char *s21_strncpy(char *dest, const char *src, s21_size_t n) {
    char *original_dest = dest;

    if (dest == S21_NULL) {
        return S21_NULL;
    }

    if (n == 0) {
        return 0;
    }

    s21_size_t i;

    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }

    for (; i < n; i++) {
        dest[i] = '\0';
    }

    return original_dest;
}