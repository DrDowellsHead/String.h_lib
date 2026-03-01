#ifndef S21_ERRORS_H
#define S21_ERRORS_H

// макросы для определения операционной системы
#if defined(__APPLE__) && defined(__MACH__)
#define S21_MACOS 1
#define S21_LINUX 0
#define S21_MAX_ERROR_COUNT 107
#elif defined(__linux__)
#define S21_MACOS 0
#define S21_LINUX 1
#define S21_MAX_ERROR_COUNT 131
#endif

extern const char *s21_error_messages[];
// для других систем массив ошибок не определяем

#endif