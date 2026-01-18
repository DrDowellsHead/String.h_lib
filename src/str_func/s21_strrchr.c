#include <s21_string.h>

char s21_strrchr(const char *str, int c) {
    const char *last_occurrence = S21_NULL;

    if (str == S21_NULL) {
        return S21_NULL;
    }

    while (*str != '\0') {
        if (*str == (char)c) {
            last_occurrence = str;
        }

        str++;
    }

    if ((char)c == '\0') {
        return (char *)str;
    }

    return (char *)last_occurrence;
}