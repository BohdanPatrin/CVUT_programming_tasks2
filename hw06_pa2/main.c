#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Проверка, совпадают ли две строки (замок и ключ)
extern int matchStrings(const char *lock, const char *key);

// Освобождение массива строк
extern void freeStrings(char **strings);

#define LETTERS "ABCDE"

// Рекурсивная функция генерации ключей
static void generateKeys(const char *keyTpl, char *currentKey, int position,
                         char **locks, int lockCount, int minLocks,
                         int *keyCount, char ***resultKeys, int *resultCapacity) {
    if (keyTpl[position] == '\0') {
        // Достигли конца шаблона, проверяем текущий ключ
        int openLocks = 0;
        for (int i = 0; i < lockCount; ++i) {
            if (matchStrings(locks[i], currentKey)) {
                openLocks++;
                if (openLocks >= minLocks) break;
            }
        }
        if (openLocks >= minLocks) {
            (*keyCount)++;
            if (resultKeys) {
                // Добавляем ключ в результат
                if (*keyCount > *resultCapacity) {
                    *resultCapacity *= 2;
                    *resultKeys = realloc(*resultKeys, (*resultCapacity) * sizeof(char *));
                }
                (*resultKeys)[*keyCount - 1] = strdup(currentKey);
                (*resultKeys)[*keyCount] = NULL; // Завершаем массив
            }
        }
        return;
    }

    // Рекурсивно заменяем символ '-' на каждую из букв A-E или копируем текущий символ
    if (keyTpl[position] == '-') {
        for (int i = 0; i < strlen(LETTERS); ++i) {
            currentKey[position] = LETTERS[i];
            generateKeys(keyTpl, currentKey, position + 1, locks, lockCount, minLocks, keyCount, resultKeys, resultCapacity);
        }
    } else {
        currentKey[position] = keyTpl[position];
        generateKeys(keyTpl, currentKey, position + 1, locks, lockCount, minLocks, keyCount, resultKeys, resultCapacity);
    }
}

// Первая функция: подсчет количества подходящих ключей
int countKeys(int minLocks, const char **locks, const char *keyTpl) {
    int lockCount = 0;
    while (locks[lockCount]) lockCount++;

    int keyCount = 0;
    char *currentKey = malloc(strlen(keyTpl) + 1);
    generateKeys(keyTpl, currentKey, 0, (char **)locks, lockCount, minLocks, &keyCount, NULL, NULL);
    free(currentKey);
    return keyCount;
}

// Вторая функция: нахождение всех подходящих ключей
char **findKeys(int minLocks, const char **locks, const char *keyTpl) {
    int lockCount = 0;
    while (locks[lockCount]) lockCount++;

    int keyCount = 0;
    int resultCapacity = 10;
    char **resultKeys = malloc(resultCapacity * sizeof(char *));
    resultKeys[0] = NULL; // Завершаем массив

    char *currentKey = malloc(strlen(keyTpl) + 1);
    generateKeys(keyTpl, currentKey, 0, (char **)locks, lockCount, minLocks, &keyCount, &resultKeys, &resultCapacity);
    free(currentKey);

    return resultKeys;
}

