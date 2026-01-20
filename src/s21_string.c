#include <stdio.h>
#include <stdbool.h>
#include "s21_string.h"
#include "s21_errors.h"

void *s21_memchr(const void *str, int c, s21_size_t n) {
    if( str == S21_NULL ) return S21_NULL;

    const unsigned char *byte_ptr = (const unsigned char*)str;
    unsigned char target = (unsigned char)c;
    void *found_ptr = S21_NULL;

    s21_size_t i = S21_SIZE_ZERO;
    bool found = 0;

    while(s21_size_lt(i,n) && !found){
        if(byte_ptr[s21_size_to_size_t(i)] == target ){
            found_ptr = (void *)(byte_ptr+s21_size_to_size_t(i));
            found = true;
        }
        i = s21_size_inc(i);
    }

    return found_ptr;
}

//вернёт 0, если есть равенство, <0, если str1<str2(побайтно), >0 - иначе
int s21_memcmp(const void* str1, const void*str2, s21_size_t n) {
    if( str1 == S21_NULL && str2 == S21_NULL ) return 0;
    //NULL обычно меньше любого другого указателя
    if( str1 == S21_NULL || str2 == S21_NULL ) return (str1==S21_NULL) ? -1 : 1;

    const unsigned char *p1 = (const unsigned char*)str1;
    const unsigned char *p2 = (const unsigned char*)str2;

    s21_size_t i = S21_SIZE_ZERO;
    bool is_equal = true;
    int result = 0;

    while(s21_size_lt(i, n) && is_equal ){
        unsigned char byte1 = p1[s21_size_to_size_t(i)];
        unsigned char byte2 = p2[s21_size_to_size_t(i)];
        if( byte1 != byte2 ){
            result = (int)byte1 - (int)byte2;
            is_equal = false;
        }
        i = s21_size_inc(i);
    }

    if(result)
        result = result<0 ? -1 : 1;
    
    return result;
}

void *s21_memcpy(void* dest, const void *src, s21_size_t n) {
    if (dest == S21_NULL || src == S21_NULL) return dest;

    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;

   
    //перекрытие областей памяти - dest внутри src
    if( d > s && d < s + s21_size_to_size_t(n)){
        //копируем с конца
        s21_size_t i = n;
        while(s21_size_ne(i, S21_SIZE_ZERO)){
            i = s21_size_dec(i);
             d[s21_size_to_size_t(i)] = s[s21_size_to_size_t(i)];
        }
    } else {
        //копируем с начала
        s21_size_t i = S21_SIZE_ZERO;
        while(s21_size_lt(i, n)){
            //копируем байты без каких-либо проверок
            d[s21_size_to_size_t(i)] = s[s21_size_to_size_t(i)];
            i = s21_size_inc(i);
        }
    }
    return dest;
}

void *s21_memset(void* str, int c, s21_size_t n) {
    if(str==S21_NULL) return S21_NULL;
    if(s21_size_eq(n, S21_SIZE_ZERO)) return str;

    unsigned char *ptr = (unsigned char *)str;
    unsigned char value = (unsigned char)c;

    s21_size_t i = S21_SIZE_ZERO;
    while(s21_size_lt(i,n)){
        ptr[s21_size_to_size_t(i)] = value;
        i = s21_size_inc(i);
    }

    return str;
}

char *s21_strncpy(char *dest, const char *src, s21_size_t n) {
    if (dest == S21_NULL || src == S21_NULL) return dest;  
    
    char *original_dest = dest;
    s21_size_t i = S21_SIZE_ZERO;
    
    //копируем до n символов или до '\0'
    while (s21_size_lt(i, n)) {
        dest[s21_size_to_size_t(i)] = src[s21_size_to_size_t(i)];
        
        if (src[s21_size_to_size_t(i)] == '\0') break;
        
        i = s21_size_inc(i);
    }
    
    //если не скопировали n символов (например, встретили '\0'), 
    // дополняем оставшиеся позиции нулями
    while (s21_size_lt(i, n)) {
        dest[s21_size_to_size_t(i)] = '\0';
        i = s21_size_inc(i);
    }
    
    return original_dest;
}

char *s21_strncat(char* dest, const char *src, s21_size_t n){
    if(dest==S21_NULL) return S21_NULL;
    if(src==S21_NULL) return dest;

    char *dest_end = dest;
    while(*dest_end != '\0') dest_end++;

    s21_size_t i = S21_SIZE_ZERO;

    while( s21_size_lt(i,n) && src[s21_size_to_size_t(i)] != '\0'){
        *dest_end++ = src[s21_size_to_size_t(i)];
        i = s21_size_inc(i);
    }
    *dest_end = '\0';

    return dest;
}

char *s21_strerror(int errnum){
    static char error_buffer[256];
    char* result;
    if( errnum>=0 && errnum<= S21_MAX_ERROR_COUNT){
        s21_strncpy(error_buffer, s21_error_messages[errnum], 
                       s21_size_make(255));
        error_buffer[255] = '\0';                       
        result = error_buffer;
    } else{
        result = S21_NULL;
    }
    return result;
}

char *s21_strstr(const char* haystack, const char *needle) {
    if(haystack==S21_NULL || needle==S21_NULL) return S21_NULL;
    s21_size_t n_len = s21_strlen(needle);
    s21_size_t h_len = s21_strlen(haystack);
    if(s21_size_eq(n_len, S21_SIZE_ZERO)) return (char*)haystack;
    if(s21_size_eq(h_len, S21_SIZE_ZERO)) return S21_NULL;
    
    char* result = NULL;

    for (s21_size_t i=S21_SIZE_ZERO; haystack[s21_size_to_size_t(i)]; i=s21_size_inc(i)){
        if(s21_strncmp(haystack+s21_size_to_size_t(i), needle, n_len)==0){
            return (char*)(haystack + s21_size_to_size_t(i));
        }    
    }

    return result;
}

//============================================================================================
//============================================================================================

int s21_strncmp(const char *str1, const char *str2, s21_size_t n) {
    if (str1 == S21_NULL || str2 == S21_NULL) {
        return 0;
    }

    if (s21_size_eq(n, S21_SIZE_ZERO)) {
        return 0;
    }

    while (s21_size_gt(n, S21_SIZE_ZERO) && *str1 && *str2 && *str1 == *str2) {
        str1++;
        str2++;
        n = s21_size_dec(n);
    }

    if (s21_size_eq(n, S21_SIZE_ZERO)) {
        return 0;
    }

    return (int)(*(unsigned char *)str1) - (int)(*(unsigned char *)str2);
}

s21_size_t s21_strlen(const char *str) {
    if (str == S21_NULL) {
        return S21_SIZE_ZERO;
    }

    s21_size_t i = S21_SIZE_ZERO;

    while (str[s21_size_to_size_t(i)] != '\0') {
        i=s21_size_inc(i);
    }

    return i;
}

s21_size_t s21_strcspn(const char *str1, const char *str2) {
    if (str1 == S21_NULL || str2 == S21_NULL) {
        return S21_SIZE_ZERO;
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

        i=s21_size_inc(i);
    }

    return i;
}

char* s21_strpbrk(const char *str1, const char *str2) {
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

char* s21_strrchr(const char *str, int c) {
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

char* s21_strtok(char *str, const char *delim) {
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
