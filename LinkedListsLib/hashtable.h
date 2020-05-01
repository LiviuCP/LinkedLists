#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "linkedlist.h"

typedef struct
{
    char* key;
    char* value;
} HashEntry;

typedef struct
{
    List** hashBuckets;
    size_t hashSize;
} HashTable;

#ifdef __cplusplus
extern "C" {
#endif

HashTable* createHashTable(const size_t hashSize);
void deleteHashTable(HashTable* hashTable);
boolean insertHashEntry(const char* key, const char* value, HashTable* hashTable);
void eraseHashEntry(const char* key, HashTable* hashTable);
void deleteHashEntry(Object* object);

const char* getHashEntryValue(const HashTable* hashTable, const char* key);
size_t getHashTableEntriesCount(const HashTable* hashTable);
size_t getHashIndexesCount(const HashTable* hashTable);

// for testing purposes only
size_t getHashIndexForKey(const char* key, size_t hashSize);

#ifdef __cplusplus
}
#endif

#endif // HASHTABLE_H
