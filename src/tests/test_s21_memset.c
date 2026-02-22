#include "test_s21_memset.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../s21_string.h"
#include "test_common_functions.h"

void compare_memset_result_simple(const void *initial_data, size_t data_size,
                                  int fill_char, size_t fill_size) {
  // Если fill_size = 0, используем data_size (полное заполнение)
  size_t actual_fill_size = (fill_size == 0) ? data_size : fill_size;

  char *buffer1 = malloc(data_size);
  char *buffer2 = malloc(data_size);

  if (!buffer1 || !buffer2) {
    free(buffer1);
    free(buffer2);
    ck_abort_msg("Memory allocation failed");
    return;
  }

  // Инициализация буферов
  if (initial_data && data_size > 0) {
    memcpy(buffer1, initial_data, data_size);
    memcpy(buffer2, initial_data, data_size);
  }

  s21_memset(buffer1, fill_char, actual_fill_size);
  memset(buffer2, fill_char, actual_fill_size);

  CHECK_MEM_EQ(buffer1, buffer2, data_size);

  free(buffer1);
  free(buffer2);
}

/* 1. Полное заполнение нулями */
START_TEST(test_memset_basic_zero) {
  char test_data[] = "Hello, World!";
  compare_memset_result_simple(test_data, sizeof(test_data), 0, 0);
}
END_TEST

/* 2. Полное заполнение символом */
START_TEST(test_memset_basic_char) {
  char test_data[50];
  // Инициализируем тестовые данные
  memset(test_data, 'X', sizeof(test_data) - 1);
  test_data[sizeof(test_data) - 1] = '\0';

  compare_memset_result_simple(test_data, sizeof(test_data), 'A', 0);
}
END_TEST

/* 3. Частичное заполнение (первые N байт) */
START_TEST(test_memset_partial) {
  char test_data[] = "1234567890";
  // Заполняем только первые 5 байт
  compare_memset_result_simple(test_data, sizeof(test_data), '*', 5);
}
END_TEST

/* 4. Заполнение нулевого размера (n=0) */
START_TEST(test_memset_zero_size) {
  char test_data[] = "Test string";
  char original_copy[sizeof(test_data)];

  // Сохраняем оригинальные данные
  memcpy(original_copy, test_data, sizeof(test_data));

  // Вызываем обе реализации с n=0
  s21_memset(test_data, 'Z', 0);
  // Используем временную переменную, чтобы обойти предупреждение
  size_t zero_size = 0;
  memset(test_data, 'Z', zero_size);

  // Проверяем, что данные не изменились
  CHECK_MEM_EQ(test_data, original_copy, sizeof(test_data));
}
END_TEST

/* 5. Отрицательные значения символа */
START_TEST(test_memset_negative_char) {
  char test_data[] = "Test data";
  // Отрицательные значения должны приводиться к unsigned char
  compare_memset_result_simple(test_data, sizeof(test_data), -1, 0);
}
END_TEST

/* 6. Заполнение массива int */
START_TEST(test_memset_int_array) {
  int numbers[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  compare_memset_result_simple(numbers, sizeof(numbers), 0xFF, 0);
}
END_TEST

/* 7. Заполнение одного байта */
START_TEST(test_memset_single_byte) {
  char test_data[] = "X";
  compare_memset_result_simple(test_data, 1, 'Y', 0);
}
END_TEST

/* 8. Переполнение символа (значение > 255) */
START_TEST(test_memset_char_overflow) {
  char test_data[50] = {0};
  // Значения > 255 обрезаются до одного байта
  compare_memset_result_simple(test_data, sizeof(test_data), 0x1234, 0);
}
END_TEST

Suite *memset_suite(void) {
  Suite *s = suite_create("s21_memset");
  TCase *tc_core = tcase_create("Core");
  TCase *tc_boundary = tcase_create("Boundary");

  // Основные тесты
  tc_core = tcase_create("Core Tests");
  tcase_add_test(tc_core, test_memset_basic_zero);
  tcase_add_test(tc_core, test_memset_basic_char);
  tcase_add_test(tc_core, test_memset_partial);
  tcase_add_test(tc_core, test_memset_zero_size);
  tcase_add_test(tc_core, test_memset_negative_char);
  tcase_add_test(tc_core, test_memset_int_array);
  suite_add_tcase(s, tc_core);

  // Граничные случаи
  tc_boundary = tcase_create("Boundary Cases");
  tcase_add_test(tc_boundary, test_memset_single_byte);
  tcase_add_test(tc_boundary, test_memset_char_overflow);
  suite_add_tcase(s, tc_boundary);

  return s;
}