#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <utility.h>
#include <list.h>
#include <dir_traversal.h>

int main(int argc, char *argv[]) 
{
    /* false - путь к каталогу не передан, true - путь к каталогу передан */
    bool directory_path_passed = false;

    /* Список слов, которые нужно исключить из обработки */
    list excluded_words;
    list_init(&excluded_words,sizeof(char *));

    /* Обработка аргументов командной строки */
    size_t i;
    for (i = 1; i < argc; ++i)
    {
        /* Получаем следующий аргумент командной строки */
        int flag = get_flag(argv[i]);
        switch (flag)
        {
            case INVALID_FLAG: case NOT_A_FLAG:
                if (i == 1)
                /* Первый аргумент - не флаг или невалидный флаг */
                /* Может быть такое, что путь к каталогу начинается с символа '-' (-some_dir/) */
                {   
                    directory_path_passed = true;
                    break;
                }
                else
                {
                    fprintf(stderr, "mangen: Invalid command line argument %s\n",argv[i]);
                    return 1;
                }
            case HELP_FLAG:
                display_instructions_for_use();
                return EXIT_SUCCESS;
            case VERSE_FLAG:
                display_version_information();
                return EXIT_SUCCESS;
            case EXCLUDE_FLAG:
                i++;
                if (i == argc)
                {
                    fprintf(stderr, "mangen: Incorrect use of flag -e\n");
                    return 1;
                }
                flag = get_flag(argv[i]);
                switch (flag)
                {
                    case INVALID_FLAG: case NOT_A_FLAG:
                        lpush_back(&excluded_words,argv[i]);
                        break;
                    default:
                        fprintf(stderr, "mangen: Incorrect use of flag -e\n");
                        return 1;
                }
        }
    }

    /* Вызов функции для генерации манифеста каталога */
    /* Если путь к каталогу передан, вызываем функцию с argv[1] */
    /* В противном случае передаём нулевой указатель */
    dir_traversal(directory_path_passed ? argv[1] : NULL, &excluded_words);

    list_destroy(&excluded_words);
    return EXIT_SUCCESS;
}
