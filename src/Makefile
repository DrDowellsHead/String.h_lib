CC = gcc
CFLAGS = -Wextra -Werror -std=gnu11 -g
COV_FLAGS = --coverage
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)  # macOS
    TEST_FLAGS = -lcheck -lm -lpthread
else                     # Linux
    TEST_FLAGS = -lcheck -lm -lrt -lpthread -lsubunit
endif

# Пути
SRC_DIR = .
TEST_DIR = tests
BUILD_DIR = build
REPORT_DIR = reports

# Сборка статической библиотеки
LIB_NAME = s21_string
LIB_STATIC = $(BUILD_DIR)/$(LIB_NAME).a
HEADER = $(LIB_NAME).h

# Исходные файлы
SRC_FILES = $(LIB_NAME).c
OBJ_FILES = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Тесты
TEST_SRC = $(TEST_DIR)/test_s21_strlen.c
TEST_HEADER = $(TEST_DIR)/test_s21_strlen.h
TEST_OBJ = $(BUILD_DIR)/test_s21_strlen.o

MAIN_SRC = main.c
MAIN_EXEC = $(BUILD_DIR)/main
MAIN_COV_EXEC = $(BUILD_DIR)/main_cov

# Создание директорий
$(BUILD_DIR) $(REPORT_DIR):
	@mkdir -p $@

.PHONY: all clean test gcov_report

# Основные цели
all: $(MAIN_EXEC)

# Компиляция основной программы
$(MAIN_EXEC): $(MAIN_SRC) $(TEST_OBJ) $(LIB_STATIC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $(MAIN_SRC) $(TEST_OBJ) $(LIB_STATIC) $(TEST_FLAGS) -I. -I$(TEST_DIR)

# Компиляция тестового файла
$(TEST_OBJ): $(TEST_SRC) $(TEST_HEADER) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ -I. -I$(TEST_DIR)

# Статическая библиотека
$(LIB_STATIC): $(OBJ_FILES) | $(BUILD_DIR)
	ar rcs $@ $^

# Объектные файлы библиотеки
$(BUILD_DIR)/%.o: %.c $(HEADER) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Запуск тестов
test: $(MAIN_EXEC)
	./$(MAIN_EXEC)

# Gcov отчет 
gcov_report: clean_gcov | $(REPORT_DIR)
	@echo "=== Сборка с покрытием ==="
	@$(MAKE) $(MAIN_COV_EXEC)
	@echo "=== Запуск тестов ==="
	cd $(BUILD_DIR) && ./main_cov
	@echo "=== Генерация отчета gcov ==="
	@for gcda_file in $(BUILD_DIR)/*.gcda; do \
		if [ -f "$$gcda_file" ]; then \
			base=$$(basename "$$gcda_file" .gcda); \
			echo "Анализ $$base..."; \
			gcov -b -o $(BUILD_DIR) "$$gcda_file" 2>/dev/null || true; \
		fi; \
	done
	@echo "=== Генерация HTML отчета gcovr ==="
	python3 -m gcovr -r . --object-directory=$(BUILD_DIR) \
		--exclude=".*test.*" \
		--html --html-details -o $(REPORT_DIR)/index.html
	@echo "HTML отчет создан: $(REPORT_DIR)/index.html"

# Версия main с поддержкой gcov 
$(MAIN_COV_EXEC): $(MAIN_SRC) $(TEST_SRC) $(LIB_NAME).c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(COV_FLAGS) -o $@ $(MAIN_SRC) $(TEST_SRC) $(LIB_NAME).c \
		-I. -I$(TEST_DIR) $(TEST_FLAGS)

# Очистка
clean:
	rm -rf $(BUILD_DIR) $(REPORT_DIR)
	find . -name "*.gcda" -o -name "*.gcno" -o -name "*.gcov" -o -name "*.o" -delete

clean_gcov:
	rm -f *.gcda *.gcno *.gcov
	rm -rf $(REPORT_DIR)

# Информация
info:
	@echo "=== Информация о проекте ==="
	@echo "Библиотека: $(LIB_STATIC)"
	@echo "Тесты: $(TEST_SRC)"
	@echo "Основная программа: $(MAIN_SRC)"
	@echo "GCov версия: $(MAIN_COV_EXEC)"
	@echo ""
	@echo "Доступные цели:"
	@echo "  make all         - Собрать программу"
	@echo "  make test        - Запустить тесты"
	@echo "  make gcov_report - Создать отчет о покрытии"
	@echo "  make clean       - Очистить все"
	@echo "  make info        - Эта информация"