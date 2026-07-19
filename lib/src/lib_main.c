/*
lib_main.c - Реализация библиотеки хеш-таблицы и работы с файлами.
Янин Ярослав Иванович
Группа МК-101
*/
#include "lib_main.h"

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