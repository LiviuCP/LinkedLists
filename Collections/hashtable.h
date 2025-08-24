#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdlib.h>
#include <stdbool.h>

#include "../LinkedListsLib/linkedlist.h"

typedef struct
{
    char* key;
    char* value;
} HashEntry;

typedef struct
{
    void* hashBuckets;
    size_t hashSize;
} HashTable;

#ifdef __cplusplus
extern "C" {
#endif

HashTable* createHashTable(const size_t hashSize, ListElementsPool* elementsPool);
void deleteHashTable(HashTable* hashTable);
bool insertHashEntry(const char* key, const char* value, HashTable* hashTable);
void eraseHashEntry(const char* key, HashTable* hashTable);

const char* getHashEntryValue(const char* key, const HashTable* hashTable);
size_t getHashTableEntriesCount(const HashTable* hashTable);
size_t getHashIndexesCount(const HashTable* hashTable);

// for testing purposes only
size_t getHashIndexForKey(const char* key, size_t hashSize);

#ifdef __cplusplus
}
#endif

#endif // HASHTABLE_H
