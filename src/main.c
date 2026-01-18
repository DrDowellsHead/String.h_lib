#include "s21_string.h"
#include "s21_errors.h"

#include <stdio.h>
//исключительно для тестов!
#include <errno.h> 
extern char *strerror(int errnum);

void safe_print(const char* str){
    if( str != S21_NULL )
        printf("%s\n", str);
    else
        printf("Got null string\n");
}

int main() {
    safe_print(S21_NULL); //пример использования собственного нулевого указателя
    s21_size_t int_size = S21_SIZE_OF(int);
    s21_size_t ptr_size = S21_SIZE_OF(void*);
    s21_size_t struct_size = S21_SIZE_OF(struct { char a; int b; });
    
    s21_size_t total = s21_size_add(int_size, ptr_size);
    s21_size_t array_size = s21_size_mul(S21_SIZE_OF(int), s21_size_make(100));
    
    for (s21_size_t i = S21_SIZE_ZERO; 
         s21_size_lt(i, s21_size_make(10)); 
         i = s21_size_inc(i)) {
        //printf("Iteration: %zu\n", s21_size_to_size_t(i));
    }
    
    size_t std_size = s21_size_to_size_t(ptr_size);
    printf("void* size: %zu (стандартный), %zu (наш)\n", 
           sizeof(void*), std_size);
    
    
    printf("Testing s21_strerror():\n");
    printf("OS: %s\n", S21_MACOS ? "macOS" : (S21_LINUX ? "Linux" : "Unknown"));
    
    printf("0: %s\n", s21_strerror(0));
    printf("1: %s\n", s21_strerror(1));  // EPERM
    printf("2: %s\n", s21_strerror(2));  // ENOENT
    printf("5: %s\n", s21_strerror(5));  // EIO
    printf("13: %s\n", s21_strerror(13)); // EACCES
    
    //граничные случаи
    printf("-1: %s\n", s21_strerror(-1));
    printf("999: %s\n", s21_strerror(999));  //несуществующая ошибка
    
    //сравнение со стандартным strerror
    errno = ENOENT;
    printf("Standard strerror: %s\n", strerror(errno));
    printf("Our s21_strerror: %s\n", s21_strerror(errno));
    return 0;
}
