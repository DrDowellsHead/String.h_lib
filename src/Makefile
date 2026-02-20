# Makefile для сборки проекта s21_string с отчетами по unit-тестам
CC = gcc
#-Wno-stringop-truncation отключает проверку на потенциальное усечение строк
#нужно для тестирования функций копирования строк
CFLAGS = -Wall -Wextra -Werror -std=gnu11 -g -Wno-stringop-truncation -Wno-error=comment
CFLAGS_COV = -fprofile-arcs -ftest-coverage
LDFLAGS_COV = -fprofile-arcs -ftest-coverage
TEST_CFLAGS = $(CFLAGS) -include string.h -include stdlib.h

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
LIB_SRC = s21_string.c s21_sscanf.c
LIB_HEADER = s21_string.h
MAIN_SRC = main.c
TEST_SRC_ALL = $(wildcard $(TESTS_DIR)/test_*.c)
TEST_SRC = $(filter-out $(TESTS_DIR)/test_s21_string.c,$(TEST_SRC_ALL))

LIB_OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(LIB_SRC))

# Исполняемые файлы
TEST_EXEC = $(BUILD_DIR)/test_runner
TEST_EXEC_COV = $(BUILD_DIR)/test_runner_cov

# Создание директорий
$(BUILD_DIR) $(REPORT_DIR):
	@mkdir -p $@

.PHONY: all clean test s21_string.a gcov_report help

# Основные цели
all: test

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

gcov_report: clean_cov $(TEST_EXEC_COV) | $(REPORT_DIR)
	@echo "=== Запуск тестов с покрытием ==="
	CK_HTML=no ./$(TEST_EXEC_COV)
	#@echo "=== Удаление временных файлов ==="
	#rm -f simple_report.txt test_log.txt test_report.xml check_report.html
	@echo "=== Генерация HTML отчета ==="
	python3 -m gcovr -r . \
		--object-directory=$(BUILD_DIR) \
		--html-details \
		--exclude-unreachable-branches \
		--exclude="s21_string.h" \
		-o $(REPORT_DIR)/index.html
	@echo ""
	@echo "HTML отчет создан: $(REPORT_DIR)/index.html"
	@echo "Чтобы открыть отчет, выполните: open $(REPORT_DIR)/index.html"

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
	@echo "  make all         - Собрать и запустить тесты (аналог make test)"
	@echo "  make test        - Собрать и запустить unit-тесты"
	@echo "  make s21_string.a - Собрать статическую библиотеку"
	@echo "  make gcov_report - Создать HTML отчет о покрытии тестами"
	@echo "  make clean       - Очистить все сгенерированные файлы"
	@echo "  make help        - Показать эту справку"

