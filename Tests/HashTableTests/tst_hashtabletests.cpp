#include <QtTest>

#include <cstring>

#include "../../LinkedListsLib/hashtable.h"

class HashTableTests : public QObject
{
    Q_OBJECT

public:
    HashTableTests();
    ~HashTableTests();

private slots:
    void testHashTableIsCorrectlyCreated();
    void testHashIndexesAreCorrectlyRetrieved();
    void testEntryIsCorrectlyInserted();
    void testEntryValueIsCorrectlyUpdated();
};

HashTableTests::HashTableTests()
{

}

HashTableTests::~HashTableTests()
{

}

void HashTableTests::testHashTableIsCorrectlyCreated()
{
    HashTable* hashTable = createHashTable(5);
    QVERIFY2(getHashTableEntriesCount(hashTable) == 0 && getHashIndexesCount(hashTable) == 5, "The hash table has not been correctly created");

    deleteHashTable(hashTable);
    hashTable = nullptr;
}

void HashTableTests::testHashIndexesAreCorrectlyRetrieved()
{
    QVERIFY2(getHashIndexForKey("Liviu", 1) == 0, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Ion", 1) == 0, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Schweinsteiger", 1) == 0, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Bobita", 1) == 0, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Maria", 1) == 0, "Hash index is not correctly retrieved");

    QVERIFY2(getHashIndexForKey("Liviu", 2) == 1, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Andrei", 2) == 1, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Ronaldinho", 2) == 0, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Ion", 2) == 0, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Werner", 2) == 1, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Andreea", 2) == 0, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Schweinsteiger", 2) == 0, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Roberto", 2) == 1, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Bobita", 2) == 1, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Maria", 2) == 0, "Hash index is not correctly retrieved");

    QVERIFY2(getHashIndexForKey("Liviu", 7) == 3, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Andrei", 7) == 0, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Ronaldinho", 7) == 2, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Ion", 7) == 0, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Werner", 7) == 4, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Andreea", 7) == 2, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Schweinsteiger", 2) == 0, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Roberto", 7) == 5, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Bobita", 7) == 5, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Maria", 7) == 0, "Hash index is not correctly retrieved");

    QVERIFY2(getHashIndexForKey("Liviu", 11) == 4, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Andrei", 11) == 1, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Ronaldinho", 11) == 4, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Ion", 11) == 8, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Werner", 11) == 0, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Andreea", 11) == 6, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Schweinsteiger", 11) == 2, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Roberto", 11) == 7, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Bobita", 11) == 10, "Hash index is not correctly retrieved");
    QVERIFY2(getHashIndexForKey("Maria", 11) == 6, "Hash index is not correctly retrieved");
}

void HashTableTests::testEntryIsCorrectlyInserted()
{
    HashTable* hashTable = createHashTable(7);

    insertHashEntry("Andrei", "engineer", hashTable);
    insertHashEntry("Ion", "IT manager", hashTable);
    insertHashEntry("Schweinsteiger", "footballer", hashTable);
    insertHashEntry("Roberto", "developer", hashTable);
    insertHashEntry("Bobita", "bartender", hashTable);

    QVERIFY2(getHashTableEntriesCount(hashTable) == 5, "The total number of hash table entries is not correct");
    QVERIFY2(getHashIndexesCount(hashTable) == 7, "The total number of hash table entries is not correct");
    QVERIFY2(strcmp(getHashEntryValue(hashTable, "Andrei"), "engineer") == 0 &&
             strcmp(getHashEntryValue(hashTable, "Ion"), "IT manager") == 0 &&
             strcmp(getHashEntryValue(hashTable, "Schweinsteiger"), "footballer") == 0 &&
             strcmp(getHashEntryValue(hashTable, "Roberto"), "developer") == 0 &&
             strcmp(getHashEntryValue(hashTable, "Bobita"), "bartender") == 0,   "The entries have not been correctly inserted");

    deleteHashTable(hashTable);
    hashTable = nullptr;
}

void HashTableTests::testEntryValueIsCorrectlyUpdated()
{
    HashTable* hashTable = createHashTable(7);

    insertHashEntry("Roberto", "developer", hashTable);
    insertHashEntry("Andrei", "engineer", hashTable);
    insertHashEntry("Roberto", "project manager", hashTable);

    QVERIFY2(getHashTableEntriesCount(hashTable) == 2, "The total number of hash table entries after updated is not correct");
    QVERIFY2(getHashIndexesCount(hashTable) == 7, "The total number of hash table entries after update is not correct");
    QVERIFY2(strcmp(getHashEntryValue(hashTable, "Roberto"), "project manager") == 0 &&
             strcmp(getHashEntryValue(hashTable, "Andrei"), "engineer") == 0,            "The entry has not been correctly updated");

    deleteHashTable(hashTable);
    hashTable = nullptr;
}

QTEST_APPLESS_MAIN(HashTableTests)

#include "tst_hashtabletests.moc"