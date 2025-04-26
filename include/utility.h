#ifndef UTILITY_H
#define UTILITY_H

#include <stdint.h>
#include <stdio.h>

#define HELP_FLAG      0
#define VERSE_FLAG     1
#define EXCLUDE_FLAG   2
#define INVALID_FLAG (-1)
#define NOT_A_FLAG   (-2)

/* Принимает в качестве аргумента строку с потенциальным флагом */
/* Возвращает HELP_FLAG, если передан флаг "-h" */
/* Возвращает VERSE_FLAG, если передан флаг "-v" */
/* Возвращает EXCLUDE_FLAG, если передан флаг "-e" */
/* Если флаг невалидный, возвращает INVALID_FLAG */
/* Если переданный аргумент не является флагом, возвращает NOT_A_FLAG */
int get_flag(const char *);

/* Хэш-функция */
uint32_t adler32(const char *, const size_t);

/* Выводит в стандартный поток вывода инструкцию по использованию и описание опций */
/* и завершает исполнение программы */
void display_instructions_for_use(void);

/* Вывести в стандартный поток вывода информацию о версии и об авторе */
/* и завершает исполнение программы */
void display_version_information(void);

#endif