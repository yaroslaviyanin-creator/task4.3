/*
lib_main.c - Реализация библиотеки хеш-таблицы и работы с файлами.
Янин Ярослав Иванович
Группа МК-101
*/
#include "lib_main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Функция вычисляет хеш-сумму для строки по алгоритму djb2
// key - указатель на нуль-терминированную строку
size_t hash_djb2(const char* key) {
    size_t hash = 5381;
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c; // эквивалентно hash * 33 + c
    }

    return hash;
}

// Функция создает новую хеш-таблицу
// size - начальный размер массива бакетов
HashTable* ht_create(size_t size) {
    if (size == 0) {
        fprintf(stderr, "Error: hash table size cannot be zero\n");
        return NULL;
    }

    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    if (!table) {
        fprintf(stderr, "Error: memory allocation failed for hash table\n");
        return NULL;
    }

    // Используем calloc для автоматического зануления массива указателей
    table->buckets = (HashNode**)calloc(size, sizeof(HashNode*));
    if (!table->buckets) {
        fprintf(stderr, "Error: memory allocation failed for buckets\n");
        free(table);
        return NULL;
    }

    table->size = size;
    table->count = 0;

    return table;
}

// Функция удаляет хеш-таблицу и освобождает память
// table - указатель на удаляемую хеш-таблицу
void ht_destroy(HashTable* table) {
    if (!table) {
        return;
    }

    for (size_t i = 0; i < table->size; i++) {
        HashNode* current = table->buckets[i];
        while (current) {
            HashNode* next = current->next;
            free(current->key); // Очищаем строку, так как будем копировать её при вставке
            free(current);      // Очищаем сам узел
            current = next;
        }
    }

    free(table->buckets);
    free(table);
}

// Функция добавляет строку в таблицу или увеличивает счетчик, если она уже есть
// table - указатель на хеш-таблицу
// key - строка-ключ для добавления
int ht_insert(HashTable* table, const char* key) {
    if (!table || !key) {
        return 0;
    }

    // Проверяем фактор загрузки (load factor). 
    // Если количество элементов равно или больше размера массива (в среднем 1 элемент на список),
    // таблица начинает работать медленно, поэтому мы её перестраиваем.
    if (table->count >= table->size) {
        ht_rebuild(table);
    }

    size_t hash = hash_djb2(key);
    size_t index = hash % table->size; // Находим индекс бакета

    // Проверяем, есть ли уже такой ключ в цепочке
    HashNode* current = table->buckets[index];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->count++; // Если нашли, просто увеличиваем счетчик
            return 1;
        }
        current = current->next;
    }

    // Если ключа нет, создаем новый узел
    HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));
    if (!new_node) {
        fprintf(stderr, "Error: memory allocation failed for new node\n");
        return 0;
    }

    // Выделяем память под строку и копируем её
    new_node->key = (char*)malloc(strlen(key) + 1);
    if (!new_node->key) {
        fprintf(stderr, "Error: memory allocation failed for node key\n");
        free(new_node);
        return 0;
    }
    strcpy(new_node->key, key);
    new_node->count = 1;

    // Вставляем новый узел в начало цепочки
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;

    table->count++; // Увеличиваем общее количество элементов

    return 1;
}

// Функция ищет узел по ключу в хеш-таблице
// table - указатель на хеш-таблицу
// key - искомая строка-ключ
HashNode* ht_search(HashTable* table, const char* key) {
    if (!table || !key) {
        return NULL;
    }

    size_t hash = hash_djb2(key);
    size_t index = hash % table->size;

    HashNode* current = table->buckets[index];

    // Проходим по цепочке в поиске нужного ключа
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current; // Узел найден
        }
        current = current->next;
    }

    return NULL; // Узел не найден
}

// Функция удаляет узел по ключу из хеш-таблицы
// table - указатель на хеш-таблицу
// key - строка-ключ для удаления
int ht_delete(HashTable* table, const char* key) {
    if (!table || !key) {
        return 0;
    }

    size_t hash = hash_djb2(key);
    size_t index = hash % table->size;

    HashNode* current = table->buckets[index];
    HashNode* prev = NULL;

    // Идем по списку и ищем элемент, запоминая предыдущий узел
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Если удаляем не первый элемент цепочки
            if (prev) {
                prev->next = current->next;
            }
            // Если удаляем голову (первый элемент) цепочки
            else {
                table->buckets[index] = current->next;
            }

            free(current->key);
            free(current);
            table->count--; // Уменьшаем общее количество
            return 1; // Успешно удалили
        }
        prev = current;
        current = current->next;
    }

    return 0; // Ключ не найден
}

// Функция перестраивает хеш-таблицу, увеличивая её размер в 2 раза (рехеширование)
// table - указатель на хеш-таблицу
void ht_rebuild(HashTable* table) {
    if (!table) {
        return;
    }

    size_t new_size = table->size * 2;
    // Создаем новый массив бакетов увеличенного размера
    HashNode** new_buckets = (HashNode**)calloc(new_size, sizeof(HashNode*));
    if (!new_buckets) {
        fprintf(stderr, "Error: memory allocation failed during rebuild\n");
        return;
    }

    // Переносим все существующие узлы в новый массив
    for (size_t i = 0; i < table->size; i++) {
        HashNode* current = table->buckets[i];
        while (current) {
            HashNode* next = current->next; // Запоминаем следующий узел в старой цепочке

            // Вычисляем новый индекс для текущего узла
            size_t new_index = hash_djb2(current->key) % new_size;

            // Вставляем текущий узел в новую цепочку
            current->next = new_buckets[new_index];
            new_buckets[new_index] = current;

            current = next; // Переходим к следующему узлу
        }
    }

    // Освобождаем старый массив бакетов и обновляем указатели
    free(table->buckets);
    table->buckets = new_buckets;
    table->size = new_size;
}

// Функция перестраивает хеш-таблицу, увеличивая её размер в 2 раза (рехеширование)
// table - указатель на хеш-таблицу
void ht_rebuild(HashTable* table) {
    if (!table) {
        return;
    }

    size_t new_size = table->size * 2;
    // Создаем новый массив бакетов увеличенного размера
    HashNode** new_buckets = (HashNode**)calloc(new_size, sizeof(HashNode*));
    if (!new_buckets) {
        fprintf(stderr, "Error: memory allocation failed during rebuild\n");
        return;
    }

    // Переносим все существующие узлы в новый массив
    for (size_t i = 0; i < table->size; i++) {
        HashNode* current = table->buckets[i];
        while (current) {
            HashNode* next = current->next; // Запоминаем следующий узел в старой цепочке

            // Вычисляем новый индекс для текущего узла
            size_t new_index = hash_djb2(current->key) % new_size;

            // Вставляем текущий узел в новую цепочку
            current->next = new_buckets[new_index];
            new_buckets[new_index] = current;

            current = next; // Переходим к следующему узлу
        }
    }

    // Освобождаем старый массив бакетов и обновляем указатели
    free(table->buckets);
    table->buckets = new_buckets;
    table->size = new_size;
}

// Функция разбирает текстовый файл по словам и добавляет их в хеш-таблицу
// table - указатель на хеш-таблицу
// filename - путь к текстовому файлу
int parse_file(HashTable* table, const char* filename) {
    if (!table || !filename) {
        return 0;
    }

    // Открываем файл для чтения
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: cannot open file %s\n", filename);
        return 0;
    }

    char buffer[1024]; // Буфер для чтения строк
    // Набор символов, которые считаются разделителями слов (пробелы и знаки препинания)
    const char* delimiters = " \t\n\r.,;:!?()[]{}'\"";

    // Читаем файл построчно
    while (fgets(buffer, sizeof(buffer), file)) {
        // Выделяем первое слово в строке с помощью strtok
        char* word = strtok(buffer, delimiters);

        while (word) {
            // TODO: В следующем коммите добавим вызов ht_insert(table, word)

            // Получаем следующее слово
            word = strtok(NULL, delimiters);
        }
    }

    fclose(file);
    return 1;
}