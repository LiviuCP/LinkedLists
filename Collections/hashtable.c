#include <string.h>

#include "hashtable.h"
#include "../LinkedListsLib/linkedlist.h"

#include "../Utils/error.h"

static const int hashEntryType = 'h' + 'a' + 's' + 'h' + 'E' + 'n' + 't' + 'r' + 'y';

// "private" (supporting) functions
static bool _retrieveHashIndex(const char* key, size_t* hashIndex, const size_t hashSize);
static HashEntry* _createHashEntry(const char* key, const char* value);
static bool _updateHashEntry(HashEntry* hashEntry, const char* value);
static HashEntry* _getMatchingKeyHashEntry(const List *currentBucket, const char* key);
static void _deleteHashEntry(Object* object); // custom deleter for hash table

HashTable* createHashTable(const size_t hashSize)
{
    HashTable* result = NULL;

    if (hashSize > 0)
    {
        List** hashBuckets = (List**)calloc(hashSize, sizeof(List*));

        if (hashBuckets != NULL)
        {
            bool allListsCreated = true;

            for (size_t index = 0; index < hashSize; ++index)
            {
                hashBuckets[index] = createEmptyList();
                if (hashBuckets[index] == NULL)
                {
                    allListsCreated = false;
                    break;
                }
            }

            if (allListsCreated)
            {
                HashTable* hashTable = (HashTable*)malloc(sizeof(HashTable));

                if (hashTable != NULL)
                {
                    hashTable->hashBuckets = (void*)hashBuckets;
                    hashTable->hashSize = hashSize;
                    result = hashTable;
                }
            }
        }

        if (result == NULL)
        {
            if (hashBuckets != NULL)
            {
                for (size_t index = 0; index < hashSize; ++index)
                {
                    if (hashBuckets[index] != NULL)
                    {
                        deleteList(hashBuckets[index], deleteObjectPayload);
                        hashBuckets[index] = NULL;
                    }
                }
            }
            free(hashBuckets);
            hashBuckets = NULL;
        }
    }

    return result;
}

void deleteHashTable(HashTable* hashTable)
{
    if (hashTable != NULL)
    {
        for (size_t hashIndex = 0; hashIndex < hashTable->hashSize; ++hashIndex)
        {
            deleteList((List*)(hashTable->hashBuckets)[hashIndex], _deleteHashEntry);
        }
    }

    free(hashTable->hashBuckets);
    hashTable->hashBuckets = NULL;
    free(hashTable);
    hashTable = NULL;
}

bool insertHashEntry(const char* key, const char* value, HashTable* hashTable)
{
    bool success = false;

    if (key != NULL && value != NULL && hashTable != NULL && strlen(key) > 0 && strlen(value) > 0)
    {
        ASSERT_CONDITION(hashTable->hashSize > 0 && hashTable->hashBuckets != NULL, "Invalid hash table")

        size_t hashIndex;
        _retrieveHashIndex(key, &hashIndex, hashTable->hashSize);
//        fprintf(stderr, "key %s, hash index: %d\n", key, (int)hashIndex);

        List* currentBucket = (List*)(hashTable->hashBuckets)[hashIndex];
        ASSERT_CONDITION(currentBucket != NULL, "NULL hash table bucket");

        HashEntry* matchingKeyHashEntry = _getMatchingKeyHashEntry(currentBucket, key);

        if (matchingKeyHashEntry != NULL)
        {
            if (strcmp(matchingKeyHashEntry->value, value) != 0)
            {
                success = _updateHashEntry(matchingKeyHashEntry, value);
            }
        }
        else
        {
            HashEntry* newHashEntry = _createHashEntry(key, value);

            if (newHashEntry != NULL)
            {
                ListElement* newElement = createAndAppendToList(currentBucket, 0); // all hash elements have priority 0 by default
                if (newElement != NULL)
                {
                    assignObjectContentToListElement(newElement, hashEntryType, newHashEntry);
                    success = true;
                }
            }
        }
    }

    return success;
}

void eraseHashEntry(const char* key, HashTable* hashTable)
{
    if (key!= NULL && strlen(key) > 0 && hashTable != NULL)
    {
        ASSERT_CONDITION(hashTable->hashSize > 0 && hashTable->hashBuckets != NULL, "Invalid hash table")

        size_t hashIndex;
        _retrieveHashIndex(key, &hashIndex, hashTable->hashSize);

        List* currentBucket = (List*)(hashTable->hashBuckets)[hashIndex];
        ASSERT_CONDITION(currentBucket != NULL, "NULL bucket detected in hash table")

        ListElement* removedElement = NULL;
        ListIterator it = lbegin(currentBucket);

        while (!areIteratorsEqual(it, lend(currentBucket)))
        {
            ASSERT_CONDITION(it.current->object.type == hashEntryType && it.current->object.payload != NULL,
                             "Invalid list element object detected in hash table")

            HashEntry* currentHashEntry = (HashEntry*)(it.current->object.payload);

            if (strcmp(currentHashEntry->key, key) == 0)
            {
                removedElement = removeCurrentListElement(it);
                break;
            }

            lnext(&it);
        }

        if (removedElement != NULL)
        {
            _deleteHashEntry(&(removedElement->object));
            free(removedElement);
            removedElement = NULL;
        }
    }
}

const char* getHashEntryValue(const char* key, const HashTable* hashTable)
{
    char* result = NULL;

    if (key != NULL && strlen(key) != 0 && hashTable != NULL)
    {
        ASSERT_CONDITION(hashTable->hashSize > 0 && hashTable->hashBuckets != NULL, "Invalid hash table")

        size_t hashIndex;
        _retrieveHashIndex(key, &hashIndex, hashTable->hashSize);

        List* currentBucket = (List*)(hashTable->hashBuckets)[hashIndex];
        ASSERT_CONDITION(currentBucket != NULL, "NULL hash table bucket");

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
        for (size_t hashIndex = 0; hashIndex < hashTable->hashSize; ++hashIndex)
        {
            hashTableEntriesCount += getListSize((List*)(hashTable->hashBuckets)[hashIndex]);
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
    ASSERT_CONDITION(_retrieveHashIndex(key, &hashIndex, hashSize), "Invalid key or hash size")

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
    HashEntry* result = NULL;

    if (key != NULL && value != NULL && strlen(key) != 0 && strlen(value) != 0)
    {
        HashEntry* entry = NULL;
        char* entryKey = (char*)malloc(strlen(key) + 1);
        char* entryValue = (char*)malloc(strlen(value) + 1);

        if (entryKey != NULL && entryValue != NULL)
        {
            entry = (HashEntry*)malloc(sizeof (HashEntry));

            if (entry != NULL)
            {
                strcpy(entryKey, key);
                strcpy(entryValue, value);
                entry->key = entryKey;
                entry->value = entryValue;
                entryKey = NULL;
                entryValue = NULL;
                result = entry;
            }
        }

        if (result == NULL)
        {
            free(entry);
            entry = NULL;
            free(entryKey);
            entryKey = NULL;
            free(entryValue);
            entryValue = NULL;
        }
    }

    return result;
}

static bool _updateHashEntry(HashEntry* hashEntry, const char* value)
{
    bool success = false;

    if (hashEntry != NULL && value != NULL)
    {
        char* newValue = (char*)malloc(strlen(value) + 1);

        if (newValue != NULL)
        {
            strcpy(newValue, value);
            free(hashEntry->value);
            hashEntry->value = newValue;
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
        ListElement* currentBucketEntry = currentBucket->first;

        while (currentBucketEntry != NULL)
        {
            ASSERT_CONDITION(currentBucketEntry->object.type == hashEntryType && currentBucketEntry->object.payload != NULL,
                             "The current bucket entry data is incorrect")

            HashEntry* currentHashEntry = (HashEntry*)(currentBucketEntry->object.payload);

            ASSERT_CONDITION(currentHashEntry->key != NULL && currentHashEntry->value != NULL && strlen(currentHashEntry->key) > 0 && strlen(currentHashEntry->value) > 0, "Invalid key-value pair");

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
        ASSERT_CONDITION(object->payload != NULL && object->type == hashEntryType, "Invalid hash entry, deleter cannot be applied")

        HashEntry* entry = (HashEntry*)object->payload;
        free(entry->key);
        entry->key = NULL;
        free(entry->value);
        entry->value = NULL;
        object->type = -1;
        free(object->payload);
        object->payload = NULL;
    }
}

