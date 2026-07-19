/*
lib_main.c - Реализация библиотеки хеш-таблицы и работы с файлами.
Янин Ярослав Иванович
Группа МК-101
*/
#include "lib_main.h"
#include <stdio.h>
#include <stdlib.h>

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