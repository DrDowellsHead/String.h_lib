#include "s21_string.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "s21_errors.h"

void *s21_memchr(const void *str, int c, __size_internal n) {
    if( str == S21_NULL ) return S21_NULL;

    const unsigned char *byte_ptr = (const unsigned char *)str;
    unsigned char target = (unsigned char)c;
    void *found_ptr = S21_NULL;

    s21_size_t i = S21_SIZE_ZERO;
    s21_size_t j = s21_size_make(n);
    bool found = 0;

    while(s21_size_lt(i,j) && !found){
        if(byte_ptr[s21_size_to_size_t(i)] == target ){
            found_ptr = (void *)(byte_ptr+s21_size_to_size_t(i));
            found = true;
        }
        i = s21_size_inc(i);
    }

    return found_ptr;
}

//вернёт 0, если есть равенство, <0, если str1<str2(побайтно), >0 - иначе
int s21_memcmp(const void* str1, const void* str2, __size_internal n) {
    // Проверки на NULL
    if (str1 == S21_NULL && str2 == S21_NULL) return 0;
    if (str1 == S21_NULL || str2 == S21_NULL) return (str1 == S21_NULL) ? -1 : 1;
    
    // Быстрая проверка
    if (n == 0) return 0;
    
    const unsigned char *p1 = str1;
    const unsigned char *p2 = str2;
    
    // Простой цикл for
    for (size_t i = 0; i < (size_t)n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] - p2[i];  // Приведение к int происходит автоматически
        }
    }
    
    return 0;
}

void *s21_memcpy(void *dest, const void *src, __size_internal n) {
    if (dest == S21_NULL || src == S21_NULL) return dest;

    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;

    // перекрытие областей памяти - dest внутри src
    if (d > s && d < s + n) {
        // копируем с конца
        unsigned i = n;
        while (i!=0) {
            i--;
            d[i] = s[i];
        }
    } else {
        // копируем с начала
        unsigned i = 0;
        while (i<n) {
            // копируем байты без каких-либо проверок
            d[i] = s[i];
            i++;
        }
    }
    return dest;
}

void *s21_memset(void *str, int c, __size_internal n) {
    if (str == S21_NULL) return S21_NULL;
    if (n==0) return str;

    unsigned char *ptr = (unsigned char *)str;
    unsigned char value = (unsigned char)c;

    __size_internal i = 0;
    while (i<n) {
        ptr[i] = value;
        i++;
    }

    return str;
}

char *s21_strncpy(char *dest, const char *src, __size_internal n) {
    if (dest == S21_NULL || src == S21_NULL) return dest;

    char *original_dest = dest;
    s21_size_t i = S21_SIZE_ZERO;
    s21_size_t j = s21_size_make(n);

    // копируем до n символов или до '\0'
    while (s21_size_lt(i, j)) {
        dest[s21_size_to_size_t(i)] = src[s21_size_to_size_t(i)];

        if (src[s21_size_to_size_t(i)] == '\0') break;

        i = s21_size_inc(i);
    }

    // если не скопировали n символов (например, встретили '\0'),
    //  дополняем оставшиеся позиции нулями
    while (s21_size_lt(i, j)) {
        dest[s21_size_to_size_t(i)] = '\0';
        i = s21_size_inc(i);
    }

    return original_dest;
}

char *s21_strncat(char *dest, const char *src, __size_internal n) {
    if (dest == S21_NULL) return S21_NULL;
    if (src == S21_NULL) return dest;

    char *dest_end = dest;
    while (*dest_end != '\0') dest_end++;

    s21_size_t i = S21_SIZE_ZERO;
    s21_size_t j = s21_size_make(n);

    while (s21_size_lt(i, j) && src[s21_size_to_size_t(i)] != '\0') {
        *dest_end++ = src[s21_size_to_size_t(i)];
        i = s21_size_inc(i);
    }
    *dest_end = '\0';

    return dest;
}

char *s21_strerror(int errnum) {
    static char error_buffer[256];
    char *result;
    if (errnum >= 0 && errnum <= S21_MAX_ERROR_COUNT) {
        s21_strncpy(error_buffer, s21_error_messages[errnum], 255);
        error_buffer[255] = '\0';
        result = error_buffer;
    } else {
        result = S21_NULL;
    }
    return result;
}

char *s21_strstr(const char *haystack, const char *needle) {
    if (haystack == S21_NULL || needle == S21_NULL) return S21_NULL;
    __size_internal n_len = s21_strlen(needle);
    __size_internal h_len = s21_strlen(haystack);
    if (n_len == 0) return (char *)haystack;
    if (h_len == 0) return S21_NULL;

    char *result = NULL;

    for (s21_size_t i = S21_SIZE_ZERO; haystack[s21_size_to_size_t(i)];
         i = s21_size_inc(i)) {
        if (s21_strncmp(haystack + s21_size_to_size_t(i), needle, n_len) == 0) {
            return (char *)(haystack + s21_size_to_size_t(i));
        }
    }

    return result;
}

//============================================================================================
//============================================================================================

int s21_strncmp(const char *str1, const char *str2, __size_internal n) {
    if (str1 == S21_NULL || str2 == S21_NULL) {
        return 0;
    }

    if (n == 0) {
        return 0;
    }

    while (n > 0 && *str1 && *str2 && *str1 == *str2) {
        str1++;
        str2++;
        n--;
    }

    if (n == 0) {
        return 0;
    }

    return (int)(*(unsigned char *)str1) - (int)(*(unsigned char *)str2);
}

__size_internal s21_strlen(const char *str) {
    // volatile запрещает оптимизацию порядка операций
    const char *volatile safe_str = str;

    if (safe_str == NULL) {
        return 0;
    }

    s21_size_t i = S21_SIZE_ZERO;

    while (safe_str[s21_size_to_size_t(i)] != '\0') {
        i = s21_size_inc(i);
    }

    return i.__value;
}

__size_internal s21_strcspn(const char *str1, const char *str2) {
    if (str1 == S21_NULL || str2 == S21_NULL) {
        return 0;
    }

    s21_size_t i = S21_SIZE_ZERO;

    while (str1[s21_size_to_size_t(i)] != '\0') {
        const char *ptr = str2;
        int found = 0;

        while (*ptr != '\0') {
            if (str1[s21_size_to_size_t(i)] == *ptr) {
                found = 1;
                break;
            }
            ptr++;
        }

        if (found) {
            break;
        }

        i = s21_size_inc(i);
    }

    return i.__value;
}

char *s21_strpbrk(const char *str1, const char *str2) {
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

char *s21_strchr(const char *str, int c) {
    if (str == S21_NULL) return S21_NULL;

    unsigned char target = (unsigned char)c;

    while (*str != '\0') {
        if ((unsigned char)*str == target) {
            return (char *)str;
        }

        str++;
    }

    if (target == (unsigned char)'\0') {
        return (char *)str;
    }

    return S21_NULL;
}

char *s21_strrchr(const char *str, int c) {
    if (str == S21_NULL) return S21_NULL;

    const char *last_occurrence = S21_NULL;
    unsigned char target = (unsigned char)c;

    while (*str != '\0') {
        if ((unsigned char)*str == target) {
            last_occurrence = str;
        }
        str++;
    }

    if (target == (unsigned char)'\0') {
        return (char *)str;
    }

    return (char *)last_occurrence;
}

char *s21_strtok(char *str, const char *delim) {
    // Статическая переменная нужна для сохранения состояния между вызовами
    // функции. Её состоняие сохраняется в Data-область памяти. В этой
    // переменной будет храниться значение строки ДО нахождения очередного
    // разделителя.
    static char *token = S21_NULL;

    if (str != S21_NULL) {
        token = str;
    }

    if (token == S21_NULL || *token == '\0') {
        return S21_NULL;
    }

    while (*token != '\0') {
        // Флаг: является ли текущий символ разделителем?
        int is_delim = 0;

        for (const char *d = delim; *d != '\0'; d++) {
            // Сравниваем текущий символ строки с текущим символом разделителей
            if (*token == *d) {
                is_delim = 1;
                break;
            }
        }

        if (!is_delim) {
            break;
        }

        token++;
    }

    // Проверка после пропуска разделителей
    if (*token == '\0') {
        // Достигли конца строки, не найдя НИ ОДНОГО не-разделителя
        token = S21_NULL;
        return S21_NULL;
    }

    // Находим начало токена
    // После цикла пропуска, token указывает на первый символ токена
    char *token_start = token;

    // Ищем конец токена
    // Теперь идём до первого разделителя ИЛИ до конца строки
    while (*token != '\0') {
        // Снова проверяем, является ли символ разделителем
        int is_delim = 0;

        // Проверяем текущий символ против ВСЕХ разделителей
        for (const char *d = delim; *d != '\0'; d++) {
            if (*token == *d) {
                is_delim = 1;
                break;
            }
        }

        // Если нашли разделитель
        if (is_delim) {
            // Заменяем разделитель на '\0' (терминатор строки)
            // Это "разрезает" исходную строку на части
            *token = '\0';

            // Перемещаем token на следующий символ
            // Это будет началом для СЛЕДУЮЩЕГО токена
            token++;

            // Возвращаем найденный токен
            return token_start;
        }

        // Если не разделитель, просто идём дальше
        token++;
    }

    // Достигли конца строки (последний токен)
    char *last_token = token_start;

    // Устанавливаем next_token в NULL
    // Потому что после этого токена больше ничего нет

    token = S21_NULL;

    return last_token;
}

// =========================== Special String Processing Functions
// ===========================

void *s21_to_upper(const char *str) {
    if (str == S21_NULL) return S21_NULL;

    // Длина строки без \0
    s21_size_t len = s21_size_make((__size_internal)s21_strlen(str));

    // Определение сколько байт нужно для выделения
    s21_size_t bytes = s21_size_add(len, S21_SIZE_ONE);

    // Выделение памяти
    char *out = (char *)malloc(s21_size_to_size_t(bytes));
    if (out == S21_NULL) return S21_NULL;

    for (s21_size_t i = S21_SIZE_ZERO; s21_size_lt(i, len);
         i = s21_size_inc(i)) {
        unsigned char c = (unsigned char)str[s21_size_to_size_t(i)];

        // Если "a"-"z", то перовод в верхний регистр по ASCII
        if (c >= (unsigned char)'a' && c <= (unsigned char)'z') {
            c = (unsigned char)(c - (unsigned char)('a' -
                                                    'A'));  // Минус 32 символа
                                                            // по ASCII
        }

        out[s21_size_to_size_t(i)] = (char)c;
    }

    out[s21_size_to_size_t(len)] = '\0';

    return (void *)out;
}

void *s21_to_lower(const char *str) {
    if (str == S21_NULL) return S21_NULL;

    s21_size_t len = s21_size_make((__size_internal)s21_strlen(str));
    s21_size_t bytes = s21_size_add(len, S21_SIZE_ONE);

    char *out = (char *)malloc(s21_size_to_size_t(bytes));
    if (out == S21_NULL) return S21_NULL;

    for (s21_size_t i = S21_SIZE_ZERO; s21_size_lt(i, len);
         i = s21_size_inc(i)) {
        unsigned char c = (unsigned char)str[s21_size_to_size_t(i)];

        if (c >= (unsigned char)'A' && c <= (unsigned char)'Z') {
            c = (unsigned char)(c + (unsigned char)('a' - 'A'));
        }

        out[s21_size_to_size_t(i)] = (char)c;
    }

    out[s21_size_to_size_t(len)] = '\0';
    return (void *)out;
}

void *s21_insert(const char *src, const char *str, s21_size_t start_index) {
    if (src == S21_NULL || str == S21_NULL) return S21_NULL;

    // Это для упрощения записи. Теперь idx = start_index
    s21_size_t idx = start_index;

    // Получение длин строк
    s21_size_t src_len = s21_size_make(s21_strlen(src));
    s21_size_t str_len = s21_size_make(s21_strlen(str));

    if (s21_size_gt(idx, src_len)) return S21_NULL;

    if (s21_size_would_overflow_add(src_len, str_len)) return S21_NULL;
    s21_size_t out_len = s21_size_add(src_len, str_len);

    // Добавление '\0'
    if (s21_size_would_overflow_add(out_len, S21_SIZE_ONE)) return S21_NULL;
    s21_size_t bytes = s21_size_add(out_len, S21_SIZE_ONE);

    char *out = (char *)malloc(s21_size_to_size_t(bytes));
    if (out == S21_NULL) return S21_NULL;

    for (s21_size_t i = S21_SIZE_ZERO; s21_size_lt(i, idx);
         i = s21_size_inc(i)) {
        out[s21_size_to_size_t(i)] = src[s21_size_to_size_t(i)];
    }

    for (s21_size_t j = S21_SIZE_ZERO; s21_size_lt(j, idx);
         j = s21_size_inc(j)) {
        s21_size_t pos = s21_size_add(idx, j);
        out[s21_size_to_size_t(pos)] = str[s21_size_to_size_t(j)];
    }

    for (s21_size_t i = idx; s21_size_lt(i, src_len); i = s21_size_inc(i)) {
        s21_size_t dest_pos = s21_size_add(i, str_len);
        out[s21_size_to_size_t(dest_pos)] = src[s21_size_to_size_t(i)];
    }

    out[s21_size_to_size_t(out_len)] = '\0';

    return (void *)out;
}

void *trim(const char *src, const char *trim_chars) {
    if (src == S21_NULL || trim_chars == S21_NULL) return S21_NULL;

    // end = длина строки, то есть индекс после последнего символа
    s21_size_t start = S21_SIZE_ZERO;
    s21_size_t end = s21_size_make((__size_internal)s21_strlen(src));

    // Если trim_chars пустая строка - то возвращается пустая строка
    if (*trim_chars == '\0') {
        s21_size_t bytes = s21_size_add(end, S21_SIZE_ONE);
        char *copy = (char *)malloc(s21_size_to_size_t(bytes));
        if (copy == S21_NULL) return S21_NULL;
        s21_memcpy(copy, src, end.__value);
        copy[s21_size_to_size_t(end)] = '\0';
        return (void *)copy;
    }

    // Трим слева: пока start < end и src[start] входит в trim_chars
    while (s21_size_lt(start, end)) {
        char ch = src[s21_size_to_size_t(start)];
        if (s21_strchr(trim_chars, (unsigned char)ch) == S21_NULL) break;
        start = s21_size_inc(start);
    }

    // Трим справа
    while (s21_size_gt(end, start)) {
        s21_size_t last = s21_size_dec(end);
        char ch = src[s21_size_to_size_t(last)];
        if (s21_strchr(trim_chars, (unsigned char)ch) == S21_NULL) break;
        end = last;
    }

    s21_size_t len = s21_size_sub(end, start);

    s21_size_t bytes = s21_size_add(len, S21_SIZE_ONE);
    char *dest = (char *)malloc(s21_size_to_size_t(bytes));
    if (dest == S21_NULL) return S21_NULL;

    // Копирование подстроки src[start .. start+len-1] в dest
    const char *src_ptr = src + s21_size_to_size_t(start);
    s21_memcpy(dest, src_ptr, len.__value);

    dest[s21_size_to_size_t(len)] = '\0';

    return (void *)dest;
}