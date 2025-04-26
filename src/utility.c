#include <utility.h>

int get_flag(const char *str)
{
    if (str[0] != '-') {
        return NOT_A_FLAG;
    }

    switch (str[1]) 
    {
        case 'h':
            if (str[2] == '\0') {
                return HELP_FLAG;
            }
            break;
        case 'v':
            if (str[2] == '\0') {
                return VERSE_FLAG;
            }
            break;
        case 'e':
            if (str[2] == '\0') {
                return EXCLUDE_FLAG;
            }
            break;
        default:
            break;
    }

    return INVALID_FLAG;
}

uint32_t adler32(const char *buf, const size_t len) 
{
    uint32_t a = 1, b = 0;
    size_t index;

    for (index = 0; index < len; ++index) 
    {
        a = (a + buf[index]) % 65521;
        b = (b + a) % 65521;
    }
    return (b << 16) | a;
}

void display_instructions_for_use()
{
    printf("\e[1mNAME\e[0m\n");
    printf("\tmangen - генерирует манифест каталога\n\n");
    printf("\e[1mSYNOPSIS\e[0m\n");
    printf("\t./mangen [DIR_PATH] [OPTIONS]\n\n");
    printf("\e[1mDESCRIPTION\e[0m\n");
    printf("\tДанная утилита генерирует манифест каталога (список пар из\n");
    printf("\tимени/пути файла и значения его хэш-суммы)\n\n");
    printf("\tЗапускаться утилита должна строкой вида:\n");
    printf("\t./mangen [DIR_PATH] [OPTIONS], где DIR_PATH - путь к каталогу,\n");
    printf("\tманифест которого будет генерироваться. В случае отсутствия пути\n");
    printf("\tв аргументах командной строки необходимо генерировать манифест для\n");
    printf("\tтекущего каталога. OPTIONS является опциями командной строки,\n");
    printf("\tописанными далее.\n\n\t\e[1mThe following options are available:\e[0m\n\n");
    printf("\t\e[1m-v\e[0m\n\n\tВывести информацию о версии и об авторе и завершить");
    printf("исполнение\n\n");
    printf("\t\e[1m-e [FILE_NAME]\e[0m\n\n\tИсключает из обработки все файлы/каталоги с именем ");
    printf("FILE_NAME,\n");
    printf("\tгде FILE_NAME - регулярное выражение с обработкой '.' (точка)\n");
    printf("\tкак произвольного символа и '*' (астериск) как произвольной\n");
    printf("\tпоследовательности символов.\n");
    printf("\tОбратите внимание. Если мы запускаем утилиту таким образом:\n");
    printf("\t./mangen some_dir -e some_dir,\n");
    printf("\tто для каталога some_dir всё равно будет сгенерирован манифест.\n");
}

#define VERSION   "0.1 (pre-release)"
#define AUTHOR    "Семен Журавлев"
#define COPYRIGHT "Copyright (c) 2025 Семен Журавлев"

void display_version_information() 
{
    printf("\e[1mNAME\e[0m\n");
    printf("\tmangen - генерирует манифест каталога\n\n");
    printf("\e[1mVERSION\e[0m\n");
    printf("\t%s\n\n", VERSION);
    printf("\e[1mAUTHOR\e[0m\n");
    printf("\t%s\n\n", AUTHOR);
    printf("\e[1mCOPYRIGHT\e[0m\n");
    printf("\t%s\n", COPYRIGHT);
}