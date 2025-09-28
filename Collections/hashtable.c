#include <stdio.h>
#include <string.h>

#include "hashtable.h"

#include "../LinkedListsLib/linkedlist.h"
#include "../Utils/error.h"

#define HASH_OFFSET 4

static const int hashEntryType = 'h' + 'a' + 's' + 'h' + 'E' + 'n' + 't' + 'r' + 'y';

// "private" (supporting) functions
static bool _retrieveHashIndex(const char* key, size_t* hashIndex, const size_t hashSize);
static HashEntry* _createHashEntry(const char* key, const char* value);
static bool _updateHashEntry(HashEntry* hashEntry, const char* value);
static HashEntry* _getMatchingKeyHashEntry(const List* currentBucket, const char* key);
static void _deleteHashEntry(Object* object); // custom deleter for hash table
static ListElement* _removeElementFromBucket(const char* key, List* bucket);
static List* _getCurrentBucket(const char* key, HashTable* hashTable);

HashTable* createHashTable(const size_t hashSize, ListElementsPool* elementsPool)
{
    HashTable* hashTable = NULL;

    // the offset bytes are required in order to prevent de-allocating data by deleting pointer to the first category (hash table object)
    void* data = hashSize > 0 ? malloc(HASH_OFFSET + sizeof(HashTable) + hashSize * sizeof(List)) : NULL;

    if (data != NULL)
    {
        hashTable = (HashTable*)(data + HASH_OFFSET);
        List* hashBuckets = (List*)(hashTable + 1);

        for (size_t index = 0; index < hashSize; ++index)
        {
            List* hashBucket = hashBuckets + index;

            hashBucket->first = NULL;
            hashBucket->last = NULL;
            hashBucket->elementsPool = elementsPool;
        }

        hashTable->hashBuckets = hashBuckets;
        hashTable->hashSize = hashSize;
        hashTable->data = data;
    }

    return hashTable;
}

void deleteHashTable(HashTable* hashTable)
{
    void* data = hashTable != NULL ? hashTable->data : NULL;
    List* hashBuckets = hashTable != NULL ? (List*)hashTable->hashBuckets : NULL;

    ASSERT(hashTable == NULL || data != NULL && hashBuckets != NULL, "Invalid hash table!");

    if (hashBuckets != NULL)
    {
        for (size_t hashIndex = 0; hashIndex < hashTable->hashSize; ++hashIndex)
        {
            clearList(hashBuckets + hashIndex, _deleteHashEntry);
        }

        hashBuckets = NULL;
    }

    FREE(data);
}

bool insertHashEntry(const char* key, const char* value, HashTable* hashTable)
{
    bool success = false;
    HashEntry* hashEntry = NULL;
    List* currentBucket = NULL;

    if (key != NULL && value != NULL && hashTable != NULL && strlen(key) > 0 && strlen(value) > 0)
    {
        currentBucket = _getCurrentBucket(key, hashTable);
        hashEntry = _getMatchingKeyHashEntry(currentBucket, key);
    }

    if (hashEntry != NULL)
    {
        if (strcmp(hashEntry->value, value) != 0)
        {
            success = _updateHashEntry(hashEntry, value);
        }
    }
    else
    {
        hashEntry = _createHashEntry(key, value);

        if (hashEntry != NULL)
        {
            ListElement* newElement = createAndAppendToList(currentBucket, 0); // all hash elements have priority 0 by default

            if (newElement != NULL)
            {
                assignObjectContentToListElement(newElement, hashEntryType, hashEntry);
                success = true;
            }
            else
            {
                ASSERT(hashEntry->key && hashEntry->value, "Invalid hash entry!");

                FREE(hashEntry->key);
                FREE(hashEntry->value);

                free(hashEntry);
                hashEntry = NULL;
            }
        }
    }

    return success;
}

void eraseHashEntry(const char* key, HashTable* hashTable)
{
    List* currentBucket = _getCurrentBucket(key, hashTable);
    ListElement* removedElement = NULL;

    ASSERT(currentBucket != NULL, "NULL bucket detected in hash table");

    if (currentBucket != NULL)
    {
        removedElement = _removeElementFromBucket(key, currentBucket);
    }

    if (removedElement != NULL)
    {
        _deleteHashEntry(&(removedElement->object));

        if (currentBucket->elementsPool != NULL)
        {
            releaseElement(removedElement, currentBucket->elementsPool);
        }
        else
        {
            free(removedElement);
        }

        removedElement = NULL;
    }
}

const char* getHashEntryValue(const char* key, const HashTable* hashTable)
{
    char* result = NULL;

    if (key != NULL && strlen(key) != 0 && hashTable != NULL)
    {
        List* currentBucket = NULL;

        if (hashTable->hashSize > 0 && hashTable->hashBuckets != NULL)
        {
            size_t hashIndex;
            _retrieveHashIndex(key, &hashIndex, hashTable->hashSize);
            List* hashBuckets = (List*)hashTable->hashBuckets;
            currentBucket = &hashBuckets[hashIndex];
        }
        else
        {
            ASSERT(false, "Invalid hash table");
        }

        HashEntry* matchingHashEntry = _getMatchingKeyHashEntry(currentBucket, key);

        if (matchingHashEntry != NULL)
        {
            result = matchingHashEntry->value;
        }
    }

    return result;
}

size_t getHashTableEntriesCount(const HashTable* hashTable)
{
    size_t hashTableEntriesCount = 0;

    if (hashTable != NULL)
    {
        if (hashTable->hashBuckets != NULL)
        {
            List* hashBuckets = (List*)hashTable->hashBuckets;

            for (size_t hashIndex = 0; hashIndex < hashTable->hashSize; ++hashIndex)
            {
                hashTableEntriesCount += getListSize(&hashBuckets[hashIndex]);
            }
        }
        else
        {
            ASSERT(false, "Invalid hash buckets detected!");
        }
    }

    return hashTableEntriesCount;
}

size_t getHashIndexesCount(const HashTable* hashTable)
{
    size_t keysCount = 0;

    if (hashTable != NULL)
    {
        keysCount = hashTable->hashSize;
    }

    return keysCount;
}

size_t getHashIndexForKey(const char* key, size_t hashSize)
{
    size_t hashIndex;
    ASSERT(_retrieveHashIndex(key, &hashIndex, hashSize), "Invalid key or hash size");

    return hashIndex;
}

static bool _retrieveHashIndex(const char* key, size_t* hashIndex, const size_t hashSize)
{
    bool success = false;

    if (key != NULL && hashIndex != NULL && strlen(key) > 0 && hashSize > 0)
    {
        size_t sum = 0;

        for (size_t index = 0; index < strlen(key); ++index)
        {
            sum = sum + (size_t)key[index];
        }

        *hashIndex = sum % hashSize;
        success = true;
    }

    return success;
}

static HashEntry* _createHashEntry(const char* key, const char* value)
{
    HashEntry* entry = NULL;

    if (key != NULL && value != NULL && strlen(key) > 0 && strlen(value) > 0)
    {
        entry = (HashEntry*)malloc(sizeof (HashEntry));

        if (entry != NULL)
        {
            char* entryKey = createStringCopy(key);
            char* entryValue = NULL;

            if (entryKey != NULL)
            {
                entryValue = createStringCopy(value);

                if (entryValue == NULL)
                {
                    free(entryKey);
                    entryKey = NULL;
                }
            }

            if (entryValue != NULL)
            {
                ASSERT(entryKey != NULL, "Invalid key!");

                entry->key = entryKey;
                entry->value = entryValue;
                entryKey = NULL;
                entryValue = NULL;
            }
            else
            {
                free(entry);
                entry = NULL;
            }
        }
    }

    return entry;
}

static bool _updateHashEntry(HashEntry* hashEntry, const char* value)
{
    bool success = false;

    if (hashEntry != NULL && value != NULL)
    {
        char* newValue = createStringCopy(value);

        if (newValue != NULL)
        {
            free(hashEntry->value);
            hashEntry->value = newValue;
            newValue = NULL;
            success = true;
        }
    }

    return  success;
}

static HashEntry* _getMatchingKeyHashEntry(const List* currentBucket, const char* key)
{
    HashEntry* matchingKeyHashEntry = NULL;

    if (currentBucket != NULL && key != NULL && strlen(key) != 0)
    {
        ListElement* currentBucketEntry = getFirstListElement(currentBucket);

        while (currentBucketEntry != NULL)
        {
            if (currentBucketEntry->object.type != hashEntryType || currentBucketEntry->object.payload == NULL)
            {
                ASSERT(false, "The current bucket entry data is incorrect");
                break;
            }

            HashEntry* currentHashEntry = (HashEntry*)(currentBucketEntry->object.payload);

            if (currentHashEntry->key == NULL || currentHashEntry->value == NULL || strlen(currentHashEntry->key) == 0 || strlen(currentHashEntry->value) == 0)
            {
                ASSERT(false, "Invalid key-value pair");
                break;
            }

            if (strcmp(currentHashEntry->key, key) == 0)
            {
                matchingKeyHashEntry = currentHashEntry;
                break;
            }

            currentBucketEntry = currentBucketEntry->next;
        }
    }

    return matchingKeyHashEntry;
}

static void _deleteHashEntry(Object* object)
{
    if (object != NULL)
    {
        if (object->payload != NULL && object->type == hashEntryType)
        {
            HashEntry* entry = (HashEntry*)object->payload;
            free(entry->key);
            entry->key = NULL;
            free(entry->value);
            entry->value = NULL;
            object->type = -1;
            free(object->payload);
            object->payload = NULL;
        }
        else
        {
            ASSERT(false, "Invalid hash entry, deleter cannot be applied");
        }
    }
}

static ListElement* _removeElementFromBucket(const char* key, List* bucket)
{
    ListElement* removedElement = NULL;

    if (bucket != NULL)
    {
        ListIterator it = lbegin(bucket);

        while (!areIteratorsEqual(it, lend(bucket)))
        {
            if (!it.current)
            {
                ASSERT(false, "Iterator points to empty list");
                break;
            }

            ASSERT(it.current->object.type == hashEntryType && it.current->object.payload != NULL,
                   "Invalid list element object detected in hash table");

            HashEntry* currentHashEntry = (HashEntry*)(it.current->object.payload);

            if (strcmp(currentHashEntry->key, key) == 0)
            {
                removedElement = removeCurrentListElement(it);
                break;
            }

            lnext(&it);
        }
    }

    return removedElement;
}

static List* _getCurrentBucket(const char* key, HashTable* hashTable)
{
    List* currentBucket = NULL;

    if (key!= NULL && strlen(key) > 0 && hashTable != NULL)
    {
        if (hashTable->hashSize > 0 && hashTable->hashBuckets != NULL)
        {
            size_t hashIndex;
            _retrieveHashIndex(key, &hashIndex, hashTable->hashSize);
            List* hashBuckets = (List*)hashTable->hashBuckets;
            currentBucket = &hashBuckets[hashIndex];
        }
        else
        {
            ASSERT(false, "Invalid hash table");
        }
    }

    return currentBucket;
}

