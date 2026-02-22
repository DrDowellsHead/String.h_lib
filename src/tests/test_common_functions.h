#ifndef TEST_COMMON_FUNCTIONS_H
#define TEST_COMMON_FUNCTIONS_H

#include <check.h>
#include <string.h>

/* Определяем версию Check */
#ifndef CHECK_MAJOR_VERSION
/* Для версий ниже 0.15.0 */
#define CHECK_VERSION_LT_15 1
#else
/* Для версий 0.15.0 и выше */
#if CHECK_MAJOR_VERSION > 0 || \
    (CHECK_MAJOR_VERSION == 0 && CHECK_MINOR_VERSION >= 15)
#define CHECK_VERSION_LT_15 0
#else
#define CHECK_VERSION_LT_15 1
#endif
#endif

/* Кастомная реализация для старых версий */
#if CHECK_VERSION_LT_15
#define CHECK_MEM_EQ(actual, expected, size)       \
  do {                                             \
    if (memcmp((actual), (expected), (size)) != 0) \
      ck_abort_msg("Memory comparison failed");    \
  } while (0)
/* Версия для новых версий Check */
#else
#define CHECK_MEM_EQ(actual, expected, size) \
  ck_assert_mem_eq(actual, expected, size)
#endif

/* Дополнительные макросы для удобства */
#define CHECK_MEM_NE(actual, expected, size)                              \
  do {                                                                    \
    if (memcmp((actual), (expected), (size)) == 0) {                      \
      ck_abort_msg("CHECK_MEM_NE: Memory blocks are unexpectedly equal"); \
    }                                                                     \
  } while (0)

#endif  // TEST_COMMON_FUNCTIONS_H