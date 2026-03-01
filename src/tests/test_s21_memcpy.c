#include "test_s21_memcpy.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../s21_string.h"
#include "test_common_functions.h"
#include "test_s21_strncpy.h"

void compare_memcpy_results(const void *src_data, size_t n, int has_overlap,
                            int dest_offset) {
  // 1. Базовый расчёт размера буфера
  size_t buffer_size = 4096;

  // 3. Выделение памяти
  char *buffer1 = malloc(buffer_size);
  char *buffer2 = malloc(buffer_size);

  // 4. Инициализация буферов
  memset(buffer1, 0xAA, buffer_size);
  memset(buffer2, 0xAA, buffer_size);

  // 5. Копирование исходных данных
  if (n > 0 && n < 1024 && src_data) {
    memcpy(buffer1, src_data, n);
    memcpy(buffer2, src_data, n);
  }

  // 6. Определение указателей
  char *dest1, *dest2;
  const char *src1 = buffer1;
  const char *src2 = buffer2;

  if (has_overlap) {
    dest1 = buffer1 + dest_offset;
    dest2 = buffer2 + dest_offset;
  } else {
    dest1 = buffer1 + n + 10;
    dest2 = buffer2 + n + 10;
  }

  // 7. Вызов обеих реализаций
  s21_memcpy(dest1, src1, n);
  memcpy(dest2, src2, n);

  // 8. Сравнение результатов через универсальный макрос
  if (n > 0) {
    CHECK_MEM_EQ(dest1, dest2, n);
  }

  // 9. Освобождение памяти
  free(buffer1);
  free(buffer2);
}

void test_simple_memcpy(const void *src_data, size_t n) {
  compare_memcpy_results(src_data, n, 0, 0);
}

void test_overlap_memcpy(const void *src_data, size_t n, int dest_offset) {
  compare_memcpy_results(src_data, n, 1, dest_offset);
}

/* Функция для генерации случайных данных произвольного типа */
void *random_data(size_t size) {
  unsigned char *data = malloc(size);
  if (!data) return NULL;

  for (size_t i = 0; i < size; i++) {
    data[i] = rand() % 256;
  }

  return data;
}

START_TEST(test_memcpy_random_strings) {
  // Тест 1: Случайные строки разной длины

  int lengths[] = {0, 1, 5, 10, 32, 100, 256, 512};
  int num_lengths = sizeof(lengths) / sizeof(lengths[0]);

  for (int i = 0; i < num_lengths; i++) {
    char *str = random_string(lengths[i]);

    // Тестируем с нуль-терминатором
    test_simple_memcpy(str, lengths[i] + 1);

    // Тестируем без нуль-терминатора
    test_simple_memcpy(str, lengths[i]);

    free(str);
  }
}
END_TEST

START_TEST(test_memcpy_random_data_various_sizes) {
  // Тест 2: Случайные данные разных размеров

  size_t sizes[] = {0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
  int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

  for (int i = 0; i < num_sizes; i++) {
    void *data = random_data(sizes[i]);

    test_simple_memcpy(data, sizes[i]);

    free(data);
  }
}
END_TEST

START_TEST(test_memcpy_random_overlap_cases) {
  // Тест 3: Случайные случаи перекрытия

  for (int iteration = 0; iteration < 50; iteration++) {
    size_t base_size = rand() % 500 + 10;
    char *base_data = random_string(base_size);

    size_t copy_size = rand() % (base_size - 5) + 1;
    int offset = rand() % (base_size - copy_size - 1) + 1;

    test_overlap_memcpy(base_data, copy_size, offset);

    free(base_data);
  }
}
END_TEST

Suite *memcpy_suite(void) {
  Suite *s = suite_create("s21_memcpy");

  TCase *tc_core = tcase_create("Core Tests");
  tcase_add_test(tc_core, test_memcpy_random_strings);
  tcase_add_test(tc_core, test_memcpy_random_data_various_sizes);
  suite_add_tcase(s, tc_core);

  TCase *tc_boundary = tcase_create("Boundary Cases");
  tcase_add_test(tc_boundary, test_memcpy_random_overlap_cases);
  suite_add_tcase(s, tc_boundary);

  return s;
}