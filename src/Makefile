CC = gcc
CFLAGS = -Wextra -Werror -std=gnu11
TARGET = s21_string

all: $(TARGET)

$(TARGET): s21_string.o main.o
	$(CC) $(CFLAGS) $^ -o $@

s21_string.o: s21_string.c s21_string.h s21_errors.h
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c s21_string.h s21_errors.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f *.o $(TARGET)

.PHONY: all run clean