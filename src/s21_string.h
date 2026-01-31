#ifndef S21_STRING_H
#define S21_STRING_H

#include <stddef.h>  
#include <stdint.h>


//__SIZE_TYPE__ - это макрос компилятора, который определяет какой базовый тип определен в данной ОС как size_t
// #ifdef __SIZE_TYPE__
// typedef __SIZE_TYPE__ __size_internal;
// #elif SIZE_MAX == ULLONG_MAX
// typedef unsigned long long __size_internal;
// #elif SIZE_MAX == ULONG_MAX
// typedef unsigned long __size_internal;
// #elif SIZE_MAX == UINT_MAX
// typedef unsigned int __size_internal;
// #else
// typedef unsigned long __size_internal;  
// #endif

// //наш тип - это обертка над базовым типом, который используется как size_t
// typedef struct s21_size{
//     __size_internal __value;
// } s21_size_t;
typedef uintptr_t __size_internal;

//наш тип - это обёртка над базовым типом
typedef struct s21_size {
    __size_internal __value;
} s21_size_t;

//определяем константы во избежание смешивания со стандартными int-типами
#define S21_SIZE_ZERO ((s21_size_t){0})
#define S21_SIZE_ONE ((s21_size_t){1})
#define S21_SIZE_MAX ((s21_size_t){SIZE_MAX})

//нулевой указатель определяем через наш 0
#define S21_NULL ((void*)0)

//получаем результат sizeof
#define S21_SIZE_OF(type) ((s21_size_t){sizeof(type)})
#define S21_SIZE_OF_VAR(var) ((s21_size_t){sizeof(var)})

//все функции static, чтобы можно было подключать s21_string.h без ошибки "multiple definition"
//inline - запрос к компилятору, при котором содержимое функции встраивается в код без стека вызовов функции
//для совместимости преобразуем в size_t
static inline size_t s21_size_to_size_t(s21_size_t s) {
    return (size_t)s.__value;
}

//создание своего типа из литерала
static inline s21_size_t s21_size_make(__size_internal value) {
    return (s21_size_t){value};
}

//арифметические операции над своим типом
static inline s21_size_t s21_size_add(s21_size_t a, s21_size_t b) {
    return (s21_size_t){a.__value + b.__value};
}

static inline s21_size_t s21_size_sub(s21_size_t a, s21_size_t b) {
    return (s21_size_t){a.__value - b.__value};
}

static inline s21_size_t s21_size_mul(s21_size_t a, s21_size_t b) {
    return (s21_size_t){a.__value * b.__value};
}

static inline s21_size_t s21_size_div(s21_size_t a, s21_size_t b) {
    return (s21_size_t){a.__value / b.__value};
}

//операторы сравнения
static inline int s21_size_eq(s21_size_t a, s21_size_t b) {
    return a.__value == b.__value;
}

static inline int s21_size_lt(s21_size_t a, s21_size_t b) {
    return a.__value < b.__value;
}

static inline int s21_size_gt(s21_size_t a, s21_size_t b) {
    return a.__value > b.__value;
}

static inline int s21_size_ne(s21_size_t a, s21_size_t b) {
    return a.__value != b.__value;
}

//инкремент и декремент
static inline s21_size_t s21_size_inc(s21_size_t s) {
    return (s21_size_t){s.__value + 1};
}

static inline s21_size_t s21_size_dec(s21_size_t s) {
    return (s21_size_t){s.__value - 1};
}

//проверка переполнения(выхода за область допустимых значений)
static inline int s21_size_would_overflow_add(s21_size_t a, s21_size_t b) {
    return a.__value > SIZE_MAX - b.__value;
}

static inline int s21_size_would_overflow_mul(s21_size_t a, s21_size_t b) {
    return b.__value != 0 && a.__value > SIZE_MAX / b.__value;
}

//функции для работы со строками
void *s21_memchr(const void* str, int c, s21_size_t n);
int s21_memcmp(const void* str1, const void*str2, s21_size_t n);
void *s21_memcpy(void* dest, const void *src, s21_size_t n);
void *s21_memset(void* str, int c, s21_size_t n);
char *s21_strncat(char* dest, const char *src, s21_size_t n);
char *s21_strerror(int errnum);
char *s21_strncpy(char *dest, const char *src, __size_internal n);
char *s21_strstr(const char* haystack, const char *needle);
//----------------------------------------------------------------
int s21_strncmp(const char *str1, const char *str2, __size_internal n);
__size_internal s21_strlen(const char *str);
s21_size_t s21_strcspn(const char *str1, const char *str2);
char* s21_strpbrk(const char *str1, const char *str2);
char* s21_strrchr(const char *str, int c);
char* s21_strtok(char *str, const char *delim); 
#endif /*S21_STRING_H*/