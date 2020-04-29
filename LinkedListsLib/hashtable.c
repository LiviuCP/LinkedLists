#include <string.h>

#include "hashtable.h"
#include "../Utils/error.h"

// "private" (supporting) functions
boolean _retrieveHashIndex(const char* key, size_t* hashIndex, const size_t hashSize);
HashEntry* _createHashEntry(const char* key, const char* value);
boolean _updateHashEntry(HashEntry* hashEntry, const char* value);
HashEntry* _getMatchingKeyHashEntry(const List *currentBucket, const char* key);

HashTable* createHashTable(const size_t hashSize)
{
    HashTable* result = NULL;

    if (hashSize > 0)
    {
        List** hashBuckets = (List**)calloc(hashSize, sizeof(List*));

        if (hashBuckets != NULL)
        {
            boolean allListsCreated = TRUE;

            for (size_t index = 0; index < hashSize; ++index)
            {
                hashBuckets[index] = createList();
                if (hashBuckets[index] == NULL)
                {
                    allListsCreated = FALSE;
                    break;
                }
            }

            if (allListsCreated)
            {
                HashTable* hashTable = (HashTable*)malloc(sizeof(HashTable));

                if (hashTable != NULL)
                {
                    hashTable->hashBuckets = hashBuckets;
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
                        deleteList(hashBuckets[index], deleteObject);
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
            deleteList(hashTable->hashBuckets[hashIndex], deleteHashEntry);
        }
    }

    free(hashTable->hashBuckets);
    hashTable->hashBuckets = NULL;
    free(hashTable);
    hashTable = NULL;
}

boolean insertHashEntry(const char* key, const char* value, HashTable* hashTable)
{
    boolean success = FALSE;

    if (key != NULL && value != NULL && hashTable != NULL && strlen(key) > 0 && strlen(value) > 0 && hashTable->hashSize > 0)
    {
        size_t hashIndex;

        if (_retrieveHashIndex(key, &hashIndex, hashTable->hashSize))
        {
//            fprintf(stderr, "key %s, hash index: %d\n", key, (int)hashIndex);
            List* currentBucket = hashTable->hashBuckets[hashIndex];
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
                        assignObjectToListElement(newElement, "HashEntry", newHashEntry);
                        success = TRUE;
                    }
                }
            }
        }
    }

    return success;
}

// custom deleter for hash table
void deleteHashEntry(Object* object)
{
    if (object != NULL)
    {
        if (object->payload != NULL)
        {
            ASSERT_CONDITION(object->type != NULL, "Null object type when payload is not null"); // object should have both type and payload
            ASSERT_CONDITION(strcmp(object->type,"HashEntry") == 0, "The object type is not HashEntry");

            HashEntry* entry = (HashEntry*)object->payload;
            free(entry->key);
            entry->key = NULL;
            free(entry->value);
            entry->value = NULL;
        }

        free(object->payload);
        object->payload = NULL;
        free(object->type);
        object->type = NULL;
        free(object);
        object = NULL;
    }
}

const char* getHashEntryValue(const HashTable* hashTable, const char* key)
{
    char* result = NULL;

    if (hashTable != NULL && hashTable->hashSize > 0 && key != NULL && strlen(key) != 0)
    {
        size_t hashIndex;

        if (_retrieveHashIndex(key, &hashIndex, hashTable->hashSize))
        {
            List* currentBucket = hashTable->hashBuckets[hashIndex];
            ASSERT_CONDITION(currentBucket != NULL, "NULL hash table bucket");

            HashEntry* matchingHashEntry = _getMatchingKeyHashEntry(currentBucket, key);

            if (matchingHashEntry != NULL)
            {
                result = matchingHashEntry->value;
            }
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
            hashTableEntriesCount += getListSize((hashTable->hashBuckets)[hashIndex]);
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

boolean _retrieveHashIndex(const char* key, size_t* hashIndex, const size_t hashSize)
{
    boolean success = FALSE;

    if (key != NULL && hashIndex != NULL && strlen(key) > 0 && hashSize > 0)
    {
        size_t sum = 0;

        for (size_t index = 0; index < strlen(key); ++index)
        {
            sum = sum + (size_t)key[index];
        }

        *hashIndex = sum % hashSize;
        success = TRUE;
    }

    return success;
}

HashEntry* _createHashEntry(const char* key, const char* value)
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

boolean _updateHashEntry(HashEntry* hashEntry, const char* value)
{
    boolean success = FALSE;

    if (hashEntry != NULL && value != NULL)
    {
        char* newValue = (char*)malloc(strlen(value) + 1);

        if (newValue != NULL)
        {
            strcpy(newValue, value);
            free(hashEntry->value);
            hashEntry->value = newValue;
            success = TRUE;
        }
    }

    return  success;
}

HashEntry* _getMatchingKeyHashEntry(const List* currentBucket, const char* key)
{
    HashEntry* matchingKeyHashEntry = NULL;

    if (currentBucket != NULL && key != NULL && strlen(key) != 0)
    {
        ListElement* currentBucketEntry = currentBucket->first;

        while (currentBucketEntry != NULL)
        {
            ASSERT_CONDITION(currentBucketEntry->object != NULL && strcmp(currentBucketEntry->object->type, "HashEntry") == 0 && currentBucketEntry->object->payload != NULL,
                             "The current bucket entry data is incorrect")

            HashEntry* currentHashEntry = (HashEntry*)currentBucketEntry->object->payload;

            ASSERT_CONDITION(currentHashEntry->key != NULL & currentHashEntry->value != NULL && strlen(currentHashEntry->key) > 0 && strlen(currentHashEntry->value) > 0, "Invalid key-value pair");

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
