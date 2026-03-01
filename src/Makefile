# Makefile для сборки проекта s21_string с отчетами по unit-тестам
CC = gcc
#-Wno-stringop-truncation отключает проверку на потенциальное усечение строк
#нужно для тестирования функций копирования строк
CFLAGS = -Wall -Wextra -Werror -std=gnu11 -g -Wno-stringop-truncation -Wno-error=comment
CFLAGS_COV = -fprofile-arcs -ftest-coverage
LDFLAGS_COV = -fprofile-arcs -ftest-coverage
TEST_CFLAGS = $(CFLAGS)

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
    TEST_FLAGS = -lcheck -lm -lpthread
else
    TEST_FLAGS = -lcheck -lm -lrt -lpthread -lsubunit
endif

# Пути
BUILD_DIR = build
REPORT_DIR = reports
TESTS_DIR = tests

# Файлы проекта
LIB_SRC = s21_string.c s21_sscanf.c s21_sprintf.c s21_errors.c
LIB_HEADER = s21_string.h s21_errors.h
MAIN_SRC = main.c
TEST_SRC = $(wildcard $(TESTS_DIR)/test_*.c)

LIB_OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(LIB_SRC))

# Исполняемые файлы
TEST_EXEC = $(BUILD_DIR)/test_runner
TEST_EXEC_COV = $(BUILD_DIR)/test_runner_cov

.DEFAULT_GOAL := all

# Создание директорий
$(BUILD_DIR) $(REPORT_DIR):
	@mkdir -p $@

.PHONY: all clean test s21_string.a gcov_report help clean_cov

all: $(TEST_EXEC)

# ============================================
# СБОРКА БЕЗ ПОКРЫТИЯ
# ============================================

# Компиляция библиотеки
$(BUILD_DIR)/%.o: %.c $(LIB_HEADER) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Сборка статической библиотеки
s21_string.a: $(LIB_OBJ)
	ar rcs $@ $^
	@echo "Библиотека создана: $@"

# Сборка тестового раннера
$(TEST_EXEC): $(MAIN_SRC) $(TEST_SRC) s21_string.a | $(BUILD_DIR)
	$(CC) $(TEST_CFLAGS) -o $@ $(MAIN_SRC) $(TEST_SRC) s21_string.a $(TEST_FLAGS) -I. -I$(TESTS_DIR)

# ============================================
# СБОРКА С ПОКРЫТИЯ
# ============================================

# Сборка тестового раннера с покрытием
$(TEST_EXEC_COV): $(MAIN_SRC) $(TEST_SRC) $(LIB_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CFLAGS_COV) -o $@ $(MAIN_SRC) $(TEST_SRC) $(LIB_SRC) \
		$(TEST_FLAGS) -I. -I$(TESTS_DIR) $(LDFLAGS_COV)

# ============================================
# ЦЕЛИ ДЛЯ ПОЛЬЗОВАТЕЛЯ
# ============================================

# Запуск обычных тестов
test: $(TEST_EXEC)
	./$(TEST_EXEC)


# Генерация HTML отчета с помощью gcovr
gcov_report: clean_cov $(TEST_EXEC_COV) | $(REPORT_DIR)
	@echo "=== Запуск тестов с покрытием ==="
	CK_HTML=no ./$(TEST_EXEC_COV)
	
	@echo "=== Проверка наличия gcovr ==="
	@if command -v gcovr >/dev/null 2>&1; then \
		echo "✓ gcovr установлен, генерируем HTML отчет..."; \
		gcovr -r . \
			--object-directory=$(BUILD_DIR) \
			--html --html-details \
			--exclude-unreachable-branches \
			--exclude-throw-branches \
			--exclude=".*_test\.c" \
			--exclude="tests/.*" \
			--exclude="main\.c" \
			--exclude="s21_errors\.[ch]" \
			--exclude="s21_string\.h" \
			--output $(REPORT_DIR)/coverage.html; \
		echo "✓ HTML отчет создан: $(REPORT_DIR)/coverage.html"; \
		echo "✓ Детальные отчеты по функциям: $(REPORT_DIR)/coverage.functions.html"; \
		echo "✓ Детальные отчеты по файлам: $(REPORT_DIR)/coverage.*.html"; \
	else \
		echo "❌ gcovr не установлен!"; \
		echo "   Установите gcovr: pip install gcovr"; \
		exit 1; \
	fi


# Очистка всех файлов
clean:
	@echo "Очистка проекта..."
	rm -rf $(BUILD_DIR) $(REPORT_DIR)
	rm -f *.a *.o
	rm -f *.gcda *.gcno *.gcov
	rm -f simple_report.txt test_log.txt test_report.xml check_report.html
	find . -name "*.o" -delete
	rm -f $(TEST_EXEC) $(TEST_EXEC_COV)

# Очистка только файлов покрытия
clean_cov:
	@echo "Очистка файлов покрытия..."
	rm -rf $(REPORT_DIR)
	rm -f $(BUILD_DIR)/*.gcda $(BUILD_DIR)/*.gcno $(BUILD_DIR)/*.gcov
	rm -f *.gcda *.gcno *.gcov
	rm -f simple_report.txt test_log.txt test_report.xml check_report.html
	rm -f $(TEST_EXEC_COV)

# Справка
help:
	@echo "=== Makefile для проекта s21_string ==="
	@echo ""
	@echo "Доступные цели:"
	@echo "  make test                    - Собрать и запустить unit-тесты"
	@echo "  make s21_string.a            - Собрать статическую библиотеку"
	@echo "  make gcov_report              - HTML отчет через gcovr"
	@echo "  make clean                    - Очистить все"
	@echo "  make clean_cov                 - Очистить файлы покрытия"
	@echo ""
