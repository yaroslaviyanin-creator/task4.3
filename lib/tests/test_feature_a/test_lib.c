/*
test_lib.c - Файл с тестами функционала хеш-таблицы.
Янин Ярослав Иванович
Группа МК-101
*/
#include "lib_main.h"
#include "unity.h"
#include <stdio.h>

// Функция выполняется перед каждым тестом (требование Unity)
// void - аргументы отсутствуют
void setUp(void) {
    // Пока оставляем пустой, инициализация специфична для каждого теста
}

// Функция выполняется после каждого теста (требование Unity)
// void - аргументы отсутствуют
void tearDown(void) {
    // Пока оставляем пустой
}

// Функция тестирует успешное создание и удаление таблицы
// void - аргументы отсутствуют
void test_ht_create_success(void) {
    HashTable* ht = ht_create(100);

    // Проверяем, что память выделилась
    TEST_ASSERT_NOT_NULL(ht);
    TEST_ASSERT_NOT_NULL(ht->buckets);

    // Проверяем начальные значения
    TEST_ASSERT_EQUAL_size_t(100, ht->size);
    TEST_ASSERT_EQUAL_size_t(0, ht->count);

    // Удаляем таблицу, чтобы избежать утечек памяти в тестах
    ht_destroy(ht);
}

// Функция тестирует обработку ошибки при нулевом размере
// void - аргументы отсутствуют
void test_ht_create_zero_size(void) {
    HashTable* ht = ht_create(0);

    // При размере 0 функция должна вернуть NULL
    TEST_ASSERT_NULL(ht);
}

// Функция является точкой входа для запуска тестов Unity
// void - аргументы отсутствуют
int main(void) {
    UNITY_BEGIN(); // Инициализация фреймворка

    RUN_TEST(test_ht_create_success);
    RUN_TEST(test_ht_create_zero_size);

    return UNITY_END(); // Подведение итогов тестов
}