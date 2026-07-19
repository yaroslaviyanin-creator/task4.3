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
}

// Функция выполняется после каждого теста (требование Unity)
// void - аргументы отсутствуют
void tearDown(void) {
}

// Функция тестирует успешное создание и удаление таблицы
// void - аргументы отсутствуют
void test_ht_create_success(void) {
    HashTable* ht = ht_create(100);

    TEST_ASSERT_NOT_NULL(ht);
    TEST_ASSERT_NOT_NULL(ht->buckets);
    TEST_ASSERT_EQUAL_size_t(100, ht->size);
    TEST_ASSERT_EQUAL_size_t(0, ht->count);

    ht_destroy(ht);
}

// Функция тестирует обработку ошибки при нулевом размере
// void - аргументы отсутствуют
void test_ht_create_zero_size(void) {
    HashTable* ht = ht_create(0);
    TEST_ASSERT_NULL(ht);
}

// Функция тестирует вставку новых элементов и их поиск
// void - аргументы отсутствуют
void test_ht_insert_and_search(void) {
    HashTable* ht = ht_create(10);

    // Вставляем два разных слова
    TEST_ASSERT_EQUAL_INT(1, ht_insert(ht, "apple"));
    TEST_ASSERT_EQUAL_INT(1, ht_insert(ht, "banana"));

    // Проверяем, что общее количество элементов увеличилось
    TEST_ASSERT_EQUAL_size_t(2, ht->count);

    // Ищем добавленное слово
    HashNode* node = ht_search(ht, "apple");
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_EQUAL_STRING("apple", node->key);
    TEST_ASSERT_EQUAL_INT(1, node->count);

    // Ищем слово, которого нет в таблице
    node = ht_search(ht, "cherry");
    TEST_ASSERT_NULL(node);

    ht_destroy(ht);
}

// Функция тестирует вставку дубликатов ключей
// void - аргументы отсутствуют
void test_ht_insert_duplicate(void) {
    HashTable* ht = ht_create(10);

    // Добавляем одно и то же слово трижды
    ht_insert(ht, "apple");
    ht_insert(ht, "apple");
    ht_insert(ht, "apple");

    // Количество уникальных узлов должно остаться 1
    TEST_ASSERT_EQUAL_size_t(1, ht->count);

    // Но счетчик внутри узла должен стать 3
    HashNode* node = ht_search(ht, "apple");
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_EQUAL_INT(3, node->count);

    ht_destroy(ht);
}

// Функция является точкой входа для запуска тестов Unity
// void - аргументы отсутствуют
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_ht_create_success);
    RUN_TEST(test_ht_create_zero_size);
    RUN_TEST(test_ht_insert_and_search);
    RUN_TEST(test_ht_insert_duplicate);

    return UNITY_END();
}