#include <QTest>

#include <cstring>

#include "hashtable.h"
#include "listelementspool.h"

#define DELETE_HASHTABLE(hashTable) \
    if (hashTable) \
    { \
        deleteHashTable(hashTable); \
        hashTable = nullptr; \
    }

class HashTableTests : public QObject
{
    Q_OBJECT

public:
    explicit HashTableTests();

private slots:
    void testHashTableIsCorrectlyCreated();
    void testHashIndexesAreCorrectlyRetrieved();
    void testEntryIsCorrectlyInserted();
    void testEntryIsCorrectlyErased();
    void testEntryValueIsCorrectlyUpdated();

    void initTestCase_data();
    void cleanupTestCase();
    void init();
    void cleanup();

private:
    ListElementsPool* m_Pool;
    size_t m_TotalAvailablePoolElementsCount;

    HashTable* m_HashTable1;
    HashTable* m_HashTable2;
};

HashTableTests::HashTableTests()
    : m_Pool{nullptr}
    , m_TotalAvailablePoolElementsCount{0}
    , m_HashTable1{nullptr}
    , m_HashTable2{nullptr}
{
}

void HashTableTests::testHashTableIsCorrectlyCreated()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    m_HashTable1 = createHashTable(5, pool);
    QVERIFY2(getHashTableEntriesCount(m_HashTable1) == 0 && getHashIndexesCount(m_HashTable1) == 5, "The hash table has not been correctly created");
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
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    m_HashTable1 = createHashTable(7, pool);

    insertHashEntry("Andrei", "engineer", m_HashTable1);
    insertHashEntry("Ion", "IT manager", m_HashTable1);
    insertHashEntry("Schweinsteiger", "footballer", m_HashTable1);
    insertHashEntry("Roberto", "developer", m_HashTable1);
    insertHashEntry("Bobita", "bartender", m_HashTable1);

    QVERIFY2(getHashTableEntriesCount(m_HashTable1) == 5, "The total number of hash table entries is not correct");
    QVERIFY2(getHashIndexesCount(m_HashTable1) == 7, "The total number of hash table entries is not correct");
    QVERIFY2(strcmp(getHashEntryValue("Andrei", m_HashTable1), "engineer") == 0 &&
             strcmp(getHashEntryValue("Ion", m_HashTable1), "IT manager") == 0 &&
             strcmp(getHashEntryValue("Schweinsteiger", m_HashTable1), "footballer") == 0 &&
             strcmp(getHashEntryValue("Roberto", m_HashTable1), "developer") == 0 &&
             strcmp(getHashEntryValue("Bobita", m_HashTable1), "bartender") == 0,   "The entries have not been correctly inserted");
}

void HashTableTests::testEntryIsCorrectlyErased()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    {
        m_HashTable1 = createHashTable(5, pool);

        insertHashEntry("Andrei", "engineer", m_HashTable1);
        insertHashEntry("Ion", "IT manager", m_HashTable1);
        insertHashEntry("Ionica", "footballer", m_HashTable1);

        eraseHashEntry("Ion", m_HashTable1);

        QVERIFY2(getHashTableEntriesCount(m_HashTable1) == 2 &&
                 getHashIndexesCount(m_HashTable1) == 5 &&
                 strcmp(getHashEntryValue("Andrei", m_HashTable1), "engineer") == 0 &&
                 strcmp(getHashEntryValue("Ionica", m_HashTable1), "footballer") == 0 , "The entry has not been correctly erased");

        eraseHashEntry("Ion", m_HashTable1);
        QVERIFY(getHashTableEntriesCount(m_HashTable1) == 2);

        eraseHashEntry("Petre", m_HashTable1);
        QVERIFY(getHashTableEntriesCount(m_HashTable1) == 2);
    }

    {
        m_HashTable2 = createHashTable(5, pool);
        eraseHashEntry("Petre", m_HashTable2);

        QVERIFY(getHashTableEntriesCount(m_HashTable2) == 0 && getHashIndexesCount(m_HashTable2) == 5);
    }
}

void HashTableTests::testEntryValueIsCorrectlyUpdated()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    m_HashTable1 = createHashTable(7, pool);

    insertHashEntry("Roberto", "developer", m_HashTable1);
    insertHashEntry("Andrei", "engineer", m_HashTable1);
    insertHashEntry("Roberto", "project manager", m_HashTable1);

    QVERIFY2(getHashTableEntriesCount(m_HashTable1) == 2, "The total number of hash table entries after updated is not correct");
    QVERIFY2(getHashIndexesCount(m_HashTable1) == 7, "The total number of hash table entries after update is not correct");
    QVERIFY2(strcmp(getHashEntryValue("Roberto", m_HashTable1), "project manager") == 0 &&
             strcmp(getHashEntryValue("Andrei", m_HashTable1), "engineer") == 0,            "The entry has not been correctly updated");
}

void HashTableTests::initTestCase_data()
{
    m_Pool = createListElementsPool();
    QVERIFY(m_Pool);

    m_TotalAvailablePoolElementsCount = getAvailableElementsCount(m_Pool);

    ListElementsPool* p_NullPool{nullptr};

    QTest::addColumn<ListElementsPool*>("pool");

    QTest::newRow("allocation from pool") << m_Pool;
    QTest::newRow("no pool allocation") << p_NullPool;

}

void HashTableTests::cleanupTestCase()
{
    QVERIFY(m_Pool);

    deleteListElementsPool(m_Pool);
    m_Pool = nullptr;
    m_TotalAvailablePoolElementsCount = 0;
}

void HashTableTests::init()
{
    QVERIFY(m_Pool);
    QVERIFY(getAvailableElementsCount(m_Pool) == m_TotalAvailablePoolElementsCount);

    QVERIFY(!m_HashTable1);
    QVERIFY(!m_HashTable2);
}

void HashTableTests::cleanup()
{
    DELETE_HASHTABLE(m_HashTable1);
    DELETE_HASHTABLE(m_HashTable2);

    QVERIFY(getAvailableElementsCount(m_Pool) == m_TotalAvailablePoolElementsCount);
}

QTEST_APPLESS_MAIN(HashTableTests)

#include "tst_hashtabletests.moc"
