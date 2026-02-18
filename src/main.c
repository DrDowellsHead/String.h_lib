#include "s21_string.h"
#include "tests/test_s21_strlen.h"
#include "tests/test_s21_strncmp.h"
#include "tests/test_s21_strncpy.h"
#include "tests/test_s21_memchr.h"
#include "tests/test_s21_memcmp.h"
#include "tests/test_s21_memcpy.h"
#include "tests/test_s21_memset.h"
#include "tests/test_s21_strncat.h"
#include "tests/test_s21_strstr.h"
#include "tests/test_s21_strcspn.h"
#include "tests/test_s21_strpbrk.h"
#include "tests/test_s21_strtok.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("=== Запуск тестов библиотеки s21_string ===\n");
    Suite *s_strlen = strlen_suite();
    Suite *s_strncmp = strncmp_suite();
    Suite *s_strncpy = strncpy_suite();
    Suite *s_memchr = memchr_suite();
    Suite *s_memcmp = memcmp_suite();
    Suite *s_memcpy = memcpy_suite();
    Suite *s_memset = memset_suite();
    Suite *s_strncat = strncat_suite();
    Suite *s_strstr = strstr_suite();
    Suite *s_strcspn = strcspn_suite();
    Suite *s_strpbrk = strpbrk_suite();
    Suite *s_strtok = strtok_suite();

    // Создаем тестовый раннер
    SRunner *sr = srunner_create(s_strlen);
    srunner_add_suite(sr, s_strncmp);
    srunner_add_suite(sr, s_strncpy);
    srunner_add_suite(sr, s_memchr);
    srunner_add_suite(sr, s_memcmp);
    srunner_add_suite(sr, s_memcpy);
    srunner_add_suite(sr, s_memset);
    srunner_add_suite(sr, s_strncat);
    srunner_add_suite(sr, s_strstr);
    srunner_add_suite(sr, s_strcspn);
    srunner_add_suite(sr, s_strpbrk);
    srunner_add_suite(sr, s_strtok);

    // Настраиваем отчеты
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_set_log(sr, "test_log.txt");
    srunner_set_xml(sr, "test_report.xml");
    
    // Запускаем все тесты
    srunner_run_all(sr, CK_NORMAL);
    
    // Получаем результаты
    int total_tests = srunner_ntests_run(sr);
    int failed_tests = srunner_ntests_failed(sr);
    
    printf("\n=== Результаты тестирования ===\n");
    printf("Всего тестов: %d\n", total_tests);
    printf("Пройдено:     %d\n", total_tests - failed_tests);
    printf("Провалено:    %d\n", failed_tests);
    
    // Генерация HTML отчета Check (если установлен xsltproc)
    printf("\n=== Генерация отчетов ===\n");
    if (system("command -v xsltproc >/dev/null 2>&1") == 0) {
        system("xsltproc /usr/share/check/check.xsl test_report.xml > check_report.html 2>/dev/null");
        printf("✓ HTML отчет Check создан: check_report.html\n");
    } else {
        printf("ℹ Для HTML отчетов Check установите: sudo apt install xsltproc\n");
    }
    
    // Создаем простой текстовый отчет
    FILE *report = fopen("simple_report.txt", "w");
    if (report) {
        fprintf(report, "Тестирование библиотеки s21_string\n");
        fprintf(report, "Дата: %s\n", __DATE__);
        fprintf(report, "Время: %s\n", __TIME__);
        fprintf(report, "Всего тестов: %d\n", total_tests);
        fprintf(report, "Пройдено: %d\n", total_tests - failed_tests);
        fprintf(report, "Провалено: %d\n", failed_tests);
        fprintf(report, "Статус: %s\n", 
                failed_tests == 0 ? "ВСЕ ТЕСТЫ ПРОЙДЕНЫ" : "ЕСТЬ ОШИБКИ");
        fclose(report);
        printf("✓ Простой отчет создан: simple_report.txt\n");
    }
    
    srunner_free(sr);
    
    if (failed_tests == 0) {
        printf("\n🎉 ВСЕ ТЕСТЫ ПРОЙДЕНЫ УСПЕШНО!\n");
    } else {
        printf("\n❌ ЕСТЬ ПРОВАЛЕННЫЕ ТЕСТЫ!\n");
    }
    
    return 0;
}