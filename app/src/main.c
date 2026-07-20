/*
main.c - Точка входа в программу.
Янин Ярослав Иванович
Группа МК-101
*/
#include "lib_main.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Функция является точкой входа в программу
// argc - количество аргументов командной строки
// argv - массив аргументов командной строки
int main(int argc, char* argv[]) {
    // Проверяем наличие хотя бы одного аргумента (имя файла)
    if (argc < 2) {
        fprintf(stderr, "Error: missing arguments.\n");
        fprintf(stderr, "Usage: %s <filename> [words_to_generate]\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];

    // Если передали второй аргумент, генерируем файл
    if (argc >= 3) {
        size_t num_words = (size_t)atoll(argv[2]);
        printf("Generating file '%s' with %zu words...\n", filename, num_words);

        if (!generate_large_file(filename, num_words)) {
            return 1; // Ошибка уже выведена в stderr внутри функции
        }
        printf("Generation done.\n");
    }

    // Создаем хеш-таблицу с начальным размером 1000 (она сама расширится при необходимости)
    HashTable* ht = ht_create(1000);
    if (!ht) {
        return 1;
    }

    printf("Parsing file '%s'...\n", filename);

    // Засекаем время парсинга
    clock_t start_time = clock();

    if (!parse_file(ht, filename)) {
        ht_destroy(ht);
        return 1;
    }

    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Выводим статистику
    printf("Parsing done in %.3f seconds.\n", time_spent);
    printf("Total unique words found: %zu\n", ht->count);
    printf("Buckets array size after all rebuilds: %zu\n\n", ht->size);

    // Демонстрируем поиск нескольких слов
    const char* test_words[] = { "university", "programming", "apple", "notfoundword" };
    for (int i = 0; i < 4; i++) {
        HashNode* node = ht_search(ht, test_words[i]);
        if (node) {
            printf("Word '%s' found %d times.\n", test_words[i], node->count);
        }
        else {
            printf("Word '%s' not found.\n", test_words[i]);
        }
    }

    // Освобождаем память
    ht_destroy(ht);
    return 0;
}