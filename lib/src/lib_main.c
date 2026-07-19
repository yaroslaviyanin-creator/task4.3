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

    // Выделяем память под строку и копируем её (чтобы не зависеть от внешнего массива)
    new_node->key = (char*)malloc(strlen(key) + 1);
    if (!new_node->key) {
        fprintf(stderr, "Error: memory allocation failed for node key\n");
        free(new_node);
        return 0;
    }
    strcpy(new_node->key, key);
    new_node->count = 1;

    // Вставляем новый узел в начало цепочки (так быстрее всего)
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;

    table->count++; // Увеличиваем общее количество элементов

    return 1;
}