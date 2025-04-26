#include <stdint.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <regex.h>

#include <utility.h>
#include <dir_traversal.h>

list get_compiled_regexes(const list *excluded_words)
/* Принимает в качестве аргумента указатель на список, содержащий имена файлов/каталогов, */
/* которые мы исключаем из обработки */
/* Возвращает список со скомпилированными регулярными выражениями */
{
    list compiled_regexes;
    list_init(&compiled_regexes, sizeof(regex_t *));

    size_t i;
    list_iterator it;
    it = lbegin(excluded_words);
    for (i = 0; i < lsize(excluded_words); ++i)
    {
        char *exclude_word = lderef(it, char *);
        /* +3 для символов '^', '$' и '\0' */
        char *regex = (char *)malloc(strlen(exclude_word) * 2 + 3); 
        if (regex == NULL) 
        {
            perror("malloc");
            exit(EXIT_FAILURE); 
        }
        /* Начало строки */
        regex[0] = '^'; 
        size_t j, ind;
        ind = 1;
        for (j = 0; exclude_word[j] != '\0'; ++j)
        {
            switch (exclude_word[j]) 
            {
                case '.':
                    regex[ind++] = '.';  
                    break;
                case '*':
                    /* В регулярных выражениях символ '*' используется для указания, что */
                    /* предшествующий ему символ или группа символов может повторяться ноль */
                    /* или более раз. Это не то, что нам нужно, поэтому символ '*' заменяем */
                    /* на сочетание ".*", что соответствует любому набору символов, включая */
                    /* пустую строку */
                    regex[ind++] = '.'; 
                    regex[ind++] = '*';
                    break;
                case '\\': 
                    regex[ind++] = '\\';
                    regex[ind++] = '\\';
                    break;
                case '^':
                    regex[ind++] = '\\';
                    regex[ind++] = '^';
                    break;
                case '$': 
                    regex[ind++] = '\\';
                    regex[ind++] = '$';
                    break;
                case '[': 
                    regex[ind++] = '\\';
                    regex[ind++] = '[';
                    break;
                case ']': 
                    regex[ind++] = '\\';
                    regex[ind++] = ']';
                    break;
                case '(':
                    regex[ind++] = '\\';
                    regex[ind++] = '(';
                    break;
                case ')': 
                    regex[ind++] = '\\';
                    regex[ind++] = ')';
                    break;
                case '{': 
                    regex[ind++] = '\\';
                    regex[ind++] = '{';
                    break;
                case '}': 
                    regex[ind++] = '\\';    
                    regex[ind++] = '}';
                    break;
                case '+': 
                    regex[ind++] = '\\';
                    regex[ind++] = '+';
                    break;
                case '?': 
                    regex[ind++] = '\\';
                    regex[ind++] = '?';
                    break;
                case '|': 
                    regex[ind++] = '\\';
                    regex[ind++] = '|';
                    break;
                default:
                    regex[ind++] = exclude_word[j];
            }
        }
        /* Конец строки */
        regex[ind++] = '$'; 
        regex[ind] = '\0';

        regex_t *compiled_regex = (regex_t *)malloc(sizeof(regex_t));

        int ret = regcomp(compiled_regex, regex, REG_EXTENDED | REG_NOSUB);
        if (ret != 0)
        {
            char error_buffer[1024];
            regerror(ret, compiled_regex, error_buffer, sizeof(error_buffer));
            fprintf(stderr, "regcomp error for pattern '%s': %s\n", regex, error_buffer);
            free(regex);
            regfree(compiled_regex);
            free(compiled_regex);
            continue;
        }

        free(regex);

        lpush_back(&compiled_regexes, compiled_regex);
        ladvance(&it,1);
    }
    return compiled_regexes;
}

void dir_traversal(const char *path_to_dir, const list *excluded_words)
{ 
    /* Список скомпилированных регулярных выражений */
    list compiled_regexes = get_compiled_regexes(excluded_words);

    /* Список абсолютных путей, необходимый для открытия каталогов и файлов */
    list absolutePaths;
    /* Список относительных путей, необходимых для вывода в консоль */
    list relativePaths;
    list_init(&absolutePaths,sizeof(char *));
    list_init(&relativePaths,sizeof(char *));

    char *abs_path;
    if (path_to_dir)
    {
        abs_path = (char *)malloc(strlen(path_to_dir)+1);
        if (abs_path == NULL)
        {
            perror("malloc");
            exit(EXIT_FAILURE); 
        }   
        strcpy(abs_path, path_to_dir);
    }
    else
    {
        abs_path = (char *)malloc(1);
        if (abs_path == NULL)
        {
            perror("malloc");
            exit(EXIT_FAILURE); 
        }
        abs_path[0] = '\0';
    }
    lpush_back(&absolutePaths, abs_path);

    char *rel_path = (char *)malloc(1);
    if (rel_path == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE); 
    }
    rel_path[0] = '\0';
    lpush_back(&relativePaths, rel_path);

    /* Обходим каталог, пока список с абсолютными путями не станет пустым */
    while (!lempty(&absolutePaths))
    {
        char *curr_abs_path = lfront(&absolutePaths, char *);
        lpop_front(&absolutePaths);

        char *curr_rel_path = lfront(&relativePaths, char *);
        lpop_front(&relativePaths);

        DIR *curr_dir = opendir(strcmp(curr_abs_path,"\0") == 0 ? "." : curr_abs_path);
        if (curr_dir == NULL) 
        {
            perror("opendir");
            free(curr_abs_path);
            free(curr_rel_path);
            continue;
        }

        struct dirent *entry;
        while ((entry = readdir(curr_dir)) != NULL) 
        {
            /* Каталоги "." и ".." мы не обрабатываем */
            if (strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0) {
                continue;
            }

            char *d_name = (char *)malloc(strlen(entry->d_name) + 1);
            if (d_name == NULL)
            {
                perror("malloc");
                exit(EXIT_FAILURE); 
            }
            strcpy(d_name, entry->d_name);

            /* Проходимся по списку со скомпилированными регулярными выражениями и сравниваем их */
            /* с текущим именем файла */
            list_iterator it;
            for (it = lbegin(&compiled_regexes); it != lend(&compiled_regexes); ladvance(&it,1))
            {
                if (regexec(lderef(it,regex_t *), d_name, 0, NULL, 0) == 0) 
                /* Если функция regexec возвращает 0, это значит, что шаблон совпал с entry->d_name */
                /* и файл/директорию с таким названием обрабатывать не требуется */
                {
                    break;
                }
            }
            
            if (it != lend(&compiled_regexes)) 
            /* Если it == lend(excluded_words), это означает, что элемент в списке не найден */
            /* В противном случае, элемент найден и обрабатывать его не требуется */
            {
                free(d_name);
                continue;
            }
            free(d_name);

            if (entry->d_type == DT_DIR)
            /* Директория */
            {
                /* +1 для символа '/' и +1 для завершающего нуля */
                char *new_abs_path = (char *)malloc(strlen(curr_abs_path) +
                    strlen(entry->d_name) + 2);
                if (new_abs_path == NULL)
                {
                    perror("malloc");
                    exit(EXIT_FAILURE); 
                }
                strcpy(new_abs_path,curr_abs_path);
                if (curr_abs_path[0] == '\0')
                /* если строка curr_abs_path пустая */
                {
                    strcat(new_abs_path,entry->d_name);
                    strcat(new_abs_path,"/");
                }
                else
                {
                    strcat(new_abs_path,"/");
                    strcat(new_abs_path,entry->d_name);
                }
                
                /* Добавляем в список absolute_paths путь к обнаруженному каталогу */
                /* для того, чтобы обработать его в дальнейшем */
                lpush_back(&absolutePaths,new_abs_path);

                /* +1 для символа '/' и +1 для завершающего нуля */
                char *new_rel_path = (char *)malloc(strlen(curr_rel_path) + 
                    strlen(entry->d_name) + 2);
                if (new_rel_path == NULL)
                {
                    perror("malloc");
                    exit(EXIT_FAILURE); 
                }
                strcpy(new_rel_path,curr_rel_path);
                strcat(new_rel_path,entry->d_name);
                strcat(new_rel_path,"/");

                /* Добавляем в список relative_paths путь к обнаруженному каталогу */
                lpush_back(&relativePaths,new_rel_path);
            }
            else  
            /* Всё остальное */
            {
                /* +1 для символа '/' и +1 для завершающего нуля */
                char *path_to_file = (char *)malloc(strlen(curr_abs_path) + 
                    strlen(entry->d_name) + 2);
                if (path_to_file == NULL)
                {
                    perror("malloc");
                    exit(EXIT_FAILURE); 
                }

                strcpy(path_to_file, curr_abs_path);
                if (curr_abs_path[0] == '\0')
                /* если строка curr_abs_path пустая */
                {
                    strcat(path_to_file, entry->d_name);
                }
                else
                {
                    strcat(path_to_file, "/");
                    strcat(path_to_file, entry->d_name);
                }

                FILE *file = fopen(path_to_file, "rb");
                if (file == NULL) 
                {
                    perror("fopen");
                    free(path_to_file);
                    continue; 
                }

                struct stat file_stats;
                if (stat(path_to_file, &file_stats) != 0) 
                {
                    perror("stat");
                    fclose(file);
                    free(path_to_file);
                    continue;
                }

                off_t file_size = file_stats.st_size;   
                char *buffer = (char *)malloc(file_size+1);
                if (buffer == NULL)
                {
                    perror("malloc");
                    exit(EXIT_FAILURE);
                }

                size_t bytes_read = fread(buffer, 1, file_size, file);
                if (bytes_read != (size_t)file_size)
                {
                    if (ferror(file)) {
                        perror("fread");
                    }
                    free(buffer);
                    free(path_to_file);
                    fclose(file);
                    continue;
                }
                
                uint32_t hash = adler32(buffer,file_size);

                printf("%s%s : %X\n",curr_rel_path,entry->d_name,hash);

                free(buffer);
                free(path_to_file);
                fclose(file);
            }
        }

        free(curr_abs_path);
        free(curr_rel_path);
        closedir(curr_dir);
    } 
    
    list_destroy(&relativePaths);
    list_destroy(&absolutePaths);   

    /* Освобождаем ресурсы, выделенные для хранения скомпилированных регулярных выражений */
    list_iterator it;
    for (it = lbegin(&compiled_regexes); it != lend(&compiled_regexes); ladvance(&it,1))
    {
        regex_t *comp_regex = lderef(it, regex_t *);
        regfree(comp_regex);
        free(comp_regex);
    }
    list_destroy(&compiled_regexes);     
}
