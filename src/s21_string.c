#include <stdio.h>
#include "s21_string.h"
#include "s21_errors.h"

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