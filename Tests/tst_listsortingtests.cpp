#include <QTest>
#include <set>

#include "listtestfixture.h"
#include "sort.h"

enum class SortingStatus
{
    SORTED,
    UNSORTED
};

enum class SortingOrder
{
    ASCENDING,
    DESCENDING
};

enum class SortingAlgorithm
{
    INSERTION,
    HEAP,
    MERGE,
    QUICK,
    ENHANCED_MERGE,
    ENHANCED_QUICK,
    QUICK_MERGE
};

class ListSortingTests : public QObject
{
    Q_OBJECT

private slots:
    void testSortByPriorityNoRandomAccess();
    void testSortByPriorityUsingRandomAccess();
    void testIsSortedByPriority();
    void testMoveListToArray();
    void testMoveArrayToList();

    void testSortByPriorityNoRandomAccess_data();
    void testSortByPriorityUsingRandomAccess_data();
    void testIsSortedByPriority_data();

    void initTestCase_data();
    void cleanupTestCase();
    void init();
    void cleanup();

private:
    ListTestFixture m_Fixture;
};

void ListSortingTests::testSortByPriorityNoRandomAccess()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Fixture.m_Pool);

    QFETCH(Priorities, priorities);
    QFETCH(SortingOrder, sortingOrder);
    QFETCH(Priorities, expectedPriorities);

    const size_t prioritiesCount = priorities.size();
    QVERIFY(prioritiesCount == expectedPriorities.size());

    List* list = prioritiesCount > 0 ? createListFromPrioritiesArray(priorities.data(), prioritiesCount, pool) : createEmptyList(pool);
    QVERIFY(list);

    m_Fixture.markListForDeletion(list);

    QVERIFY(getListSize(list) == prioritiesCount);

    if (sortingOrder == SortingOrder::ASCENDING)
    {
        sortAscendingByPriority(list);
    }
    else
    {
        sortDescendingByPriority(list);
    }

    for (size_t index = 0; index < prioritiesCount; ++index)
    {
        QVERIFY(getListElementAtIndex(list, index)->priority == expectedPriorities[index]);
    }

    if (prioritiesCount > 0)
    {
        QVERIFY(getFirstListElement(list)->priority == expectedPriorities[0] && getLastListElement(list)->priority == expectedPriorities[prioritiesCount - 1]);
    }

    QVERIFY(sortingOrder == SortingOrder::ASCENDING ? isSortedAscendingByPriority(list) : isSortedDescendingByPriority(list));
}

void ListSortingTests::testSortByPriorityUsingRandomAccess()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Fixture.m_Pool);

    QFETCH(Priorities, priorities);
    QFETCH(SortingAlgorithm, algorithm);
    QFETCH(SortingOrder, sortingOrder);
    QFETCH(Priorities, expectedPriorities);

    const std::set<SortingAlgorithm> singleThreadedSortingAlgorithms{SortingAlgorithm::INSERTION, SortingAlgorithm::HEAP, SortingAlgorithm::MERGE, SortingAlgorithm::QUICK};
#ifdef UNIX_OS
    const std::set<SortingAlgorithm> multiThreadedSortingAlgorithms{SortingAlgorithm::ENHANCED_MERGE, SortingAlgorithm::ENHANCED_QUICK, SortingAlgorithm::QUICK_MERGE};
    QVERIFY(singleThreadedSortingAlgorithms.contains(algorithm) || multiThreadedSortingAlgorithms.contains(algorithm));
#else
    QVERIFY(singleThreadedSortingAlgorithms.contains(algorithm));
#endif

    const size_t prioritiesCount = priorities.size();
    QVERIFY(prioritiesCount == expectedPriorities.size());

    List* list = prioritiesCount > 0 ? createListFromPrioritiesArray(priorities.data(), prioritiesCount, pool) : createEmptyList(pool);
    QVERIFY(list);

    m_Fixture.markListForDeletion(list);

    QVERIFY(getListSize(list) == prioritiesCount);

    const auto ascendingSortingFunction = algorithm == SortingAlgorithm::INSERTION ? insertionSortAscendingByPriority
                                                                                   : algorithm == SortingAlgorithm::HEAP ? heapSortAscendingByPriority
                                                                                   : algorithm == SortingAlgorithm::MERGE ? mergeSortAscendingByPriority
                                                                                   : algorithm == SortingAlgorithm::QUICK ? quickSortAscendingByPriority
                                                                                   : algorithm == SortingAlgorithm::ENHANCED_MERGE ? enhancedMergeSortAscendingByPriority
                                                                                   : algorithm == SortingAlgorithm::ENHANCED_QUICK ? enhancedQuickSortAscendingByPriority
                                                                                                                                   : quickMergeSortAscendingByPriority;
    const auto descendingSortingFunction = algorithm == SortingAlgorithm::INSERTION ? insertionSortDescendingByPriority
                                                                                    : algorithm == SortingAlgorithm::HEAP ? heapSortDescendingByPriority
                                                                                    : algorithm == SortingAlgorithm::MERGE ? mergeSortDescendingByPriority
                                                                                    : algorithm == SortingAlgorithm::QUICK ? quickSortDescendingByPriority
                                                                                    : algorithm == SortingAlgorithm::ENHANCED_MERGE ? enhancedMergeSortDescendingByPriority
                                                                                    : algorithm == SortingAlgorithm::ENHANCED_QUICK ? enhancedQuickSortDescendingByPriority
                                                                                                                                    : quickMergeSortDescendingByPriority;

    sortByPriorityUsingRandomAccess(list, sortingOrder == SortingOrder::ASCENDING ? ascendingSortingFunction : descendingSortingFunction);

    for (size_t index = 0; index < prioritiesCount; ++index)
    {
        QVERIFY(getListElementAtIndex(list, index)->priority == expectedPriorities[index]);
    }

    if (prioritiesCount > 0)
    {
        QVERIFY(getFirstListElement(list)->priority == expectedPriorities[0] && getLastListElement(list)->priority == expectedPriorities[prioritiesCount - 1]);
    }

    QVERIFY(sortingOrder == SortingOrder::ASCENDING ? isSortedAscendingByPriority(list) : isSortedDescendingByPriority(list));
}

void ListSortingTests::testIsSortedByPriority()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Fixture.m_Pool);

    QFETCH(Priorities, priorities);
    QFETCH(SortingOrder, sortingOrderToCheck);
    QFETCH(SortingStatus, expectedSortingStatus);

    const size_t prioritiesCount = priorities.size();
    List* list = prioritiesCount > 0 ? createListFromPrioritiesArray(priorities.data(), prioritiesCount, pool) : createEmptyList(pool);

    m_Fixture.markListForDeletion(list);
    const bool shouldBeSorted = expectedSortingStatus == SortingStatus::SORTED;

    QVERIFY(sortingOrderToCheck == SortingOrder::ASCENDING ? isSortedAscendingByPriority(list) == shouldBeSorted
                                                           : isSortedDescendingByPriority(list) == shouldBeSorted);
}

void ListSortingTests::testMoveListToArray()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Fixture.m_Pool);

    const Priority prioritiesArray[4]{6, 2, 5, 9};

    m_Fixture.m_List1 = createListFromPrioritiesArray(prioritiesArray, 4, pool);

    QVERIFY(getListSize(m_Fixture.m_List1) == 4);

    size_t arraySize;
    ListElement** array = moveListToArray(m_Fixture.m_List1, &arraySize);

    QVERIFY2(isEmptyList(m_Fixture.m_List1) &&
             arraySize == 4 &&
             array[0]->next == nullptr && array[0]->priority == 6 &&
             array[1]->next == nullptr && array[1]->priority == 2 &&
             array[2]->next == nullptr && array[2]->priority == 5 &&
             array[3]->next == nullptr && array[3]->priority == 9 ,  "The list content has been incorrectly moved to array");

    for (size_t index = 0; index < arraySize; ++index)
    {
        m_Fixture.markListElementForCleanup(array[index], pool != nullptr);
    }

    free(array);
    array = nullptr;
}

void ListSortingTests::testMoveArrayToList()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Fixture.m_Pool);

    m_Fixture.m_List1 = createEmptyList(pool);
    m_Fixture.m_ListElementRefs = static_cast<ListElement**>(calloc(4, sizeof(ListElement*)));

    m_Fixture.m_ListElementRefs[0] = pool ? aquireElement(pool) : createListElement();
    m_Fixture.markListElementForCleanup(m_Fixture.m_ListElementRefs[0], pool != nullptr);

    QVERIFY(m_Fixture.m_ListElementRefs[0]);

    m_Fixture.m_ListElementRefs[1] = pool ? aquireElement(pool) : createListElement();
    m_Fixture.markListElementForCleanup(m_Fixture.m_ListElementRefs[1], pool != nullptr);

    QVERIFY(m_Fixture.m_ListElementRefs[1]);

    m_Fixture.m_ListElementRefs[2] = pool ? aquireElement(pool) : createListElement();
    m_Fixture.markListElementForCleanup(m_Fixture.m_ListElementRefs[2], pool != nullptr);

    QVERIFY(m_Fixture.m_ListElementRefs[2]);

    m_Fixture.m_ListElementRefs[3] = pool ? aquireElement(pool) : createListElement();
    m_Fixture.markListElementForCleanup(m_Fixture.m_ListElementRefs[3], pool != nullptr);

    QVERIFY(m_Fixture.m_ListElementRefs[3]);

    m_Fixture.m_ListElementRefs[0]->priority = 6;
    m_Fixture.m_ListElementRefs[1]->priority = 2;
    m_Fixture.m_ListElementRefs[2]->priority = 5;
    m_Fixture.m_ListElementRefs[3]->priority = 9;

    QVERIFY(isEmptyList(m_Fixture.m_List1));

    // previous cleanup markings were performed to prevent memory leaks; upon reaching this point the elements should be unmarked as the they would get transferred to the list (no longer "free")
    m_Fixture.clearListElementsMarkedForCleanup();

    moveArrayToList(m_Fixture.m_ListElementRefs, 4, m_Fixture.m_List1);

    QVERIFY2(m_Fixture.m_ListElementRefs[0] == nullptr &&
             m_Fixture.m_ListElementRefs[1] == nullptr &&
             m_Fixture.m_ListElementRefs[2] == nullptr &&
             m_Fixture.m_ListElementRefs[3] == nullptr &&
             getListSize(m_Fixture.m_List1) == 4 &&
             getListElementAtIndex(m_Fixture.m_List1, 0)->priority == 6 &&
             getListElementAtIndex(m_Fixture.m_List1, 1)->priority == 2 &&
             getListElementAtIndex(m_Fixture.m_List1, 2)->priority == 5 &&
             getListElementAtIndex(m_Fixture.m_List1, 3)->priority == 9, "The array content has been incorrectly moved to list");
    QVERIFY2(getFirstListElement(m_Fixture.m_List1)->priority == 6 && getLastListElement(m_Fixture.m_List1)->priority == 9, "First and last element of the list are not correctly referenced");
    QVERIFY(getListElementAtIndex(m_Fixture.m_List1, 1)->object.type == -1 && getListElementAtIndex(m_Fixture.m_List1, 1)->object.payload == nullptr);
}

void ListSortingTests::testSortByPriorityNoRandomAccess_data()
{
    QTest::addColumn<Priorities>("priorities");
    QTest::addColumn<SortingOrder>("sortingOrder");
    QTest::addColumn<Priorities>("expectedPriorities");

    QTest::newRow("Sort ascending: 1") << Priorities{6, 2, 5, 1, 2, 3} << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 5, 6};
    QTest::newRow("Sort ascending: 2") << Priorities{3, 2, 1, 5, 2, 6} << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 5, 6};
    QTest::newRow("Sort ascending: 3") << Priorities{1, 6, 2, 5, 3, 2} << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 5, 6};
    QTest::newRow("Sort ascending: 4") << Priorities{2, 3, 5, 2, 6, 1} << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 5, 6};
    QTest::newRow("Sort ascending: 5") << Priorities{2, 3, 1, 2, 6, 5} << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 5, 6};
    QTest::newRow("Sort ascending: 6") << Priorities{1, 2, 2, 3, 5, 6} << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 5, 6};
    QTest::newRow("Sort ascending: 7") << Priorities{6, 5, 3, 2, 2, 1} << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 5, 6};
    QTest::newRow("Sort ascending: 8") << Priorities{5, 6} << SortingOrder::ASCENDING << Priorities{5, 6};
    QTest::newRow("Sort ascending: 9") << Priorities{6, 5} << SortingOrder::ASCENDING << Priorities{5, 6};
    QTest::newRow("Sort ascending: 10") << Priorities{1} << SortingOrder::ASCENDING << Priorities{1};
    QTest::newRow("Sort ascending: 11") << Priorities{} << SortingOrder::ASCENDING << Priorities{};
    QTest::newRow("Sort descending: 1") << Priorities{6, 2, 5, 1, 2, 3} << SortingOrder::DESCENDING << Priorities{6, 5, 3, 2, 2, 1};
    QTest::newRow("Sort descending: 2") << Priorities{3, 2, 1, 5, 2, 6} << SortingOrder::DESCENDING << Priorities{6, 5, 3, 2, 2, 1};
    QTest::newRow("Sort descending: 3") << Priorities{1, 6, 2, 5, 3, 2} << SortingOrder::DESCENDING << Priorities{6, 5, 3, 2, 2, 1};
    QTest::newRow("Sort descending: 4") << Priorities{2, 3, 5, 2, 6, 1} << SortingOrder::DESCENDING << Priorities{6, 5, 3, 2, 2, 1};
    QTest::newRow("Sort descending: 5") << Priorities{2, 3, 1, 2, 6, 5} << SortingOrder::DESCENDING << Priorities{6, 5, 3, 2, 2, 1};
    QTest::newRow("Sort descending: 6") << Priorities{1, 2, 2, 3, 5, 6} << SortingOrder::DESCENDING << Priorities{6, 5, 3, 2, 2, 1};
    QTest::newRow("Sort descending: 7") << Priorities{6, 5, 3, 2, 2, 1} << SortingOrder::DESCENDING << Priorities{6, 5, 3, 2, 2, 1};
    QTest::newRow("Sort descending: 8") << Priorities{5, 6} << SortingOrder::DESCENDING << Priorities{6, 5};
    QTest::newRow("Sort descending: 9") << Priorities{6, 5} << SortingOrder::DESCENDING << Priorities{6, 5};
    QTest::newRow("Sort descending: 10") << Priorities{1} << SortingOrder::DESCENDING << Priorities{1};
    QTest::newRow("Sort descending: 11") << Priorities{} << SortingOrder::DESCENDING << Priorities{};
    }

void ListSortingTests::testSortByPriorityUsingRandomAccess_data()
{
    QTest::addColumn<Priorities>("priorities");
    QTest::addColumn<SortingAlgorithm>("algorithm");
    QTest::addColumn<SortingOrder>("sortingOrder");
    QTest::addColumn<Priorities>("expectedPriorities");

    QTest::newRow("Sort ascending - insertion: 1") << Priorities{2, 3, 5, 2, 9, 6, 1, 8, 7, 5, 4, 6} << SortingAlgorithm::INSERTION << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - insertion: 2") << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9} << SortingAlgorithm::INSERTION << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - insertion: 3") << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1} << SortingAlgorithm::INSERTION << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - insertion: 4") << Priorities{8, 8, 8, 8, 8, 8, 8, 8} << SortingAlgorithm::INSERTION << SortingOrder::ASCENDING << Priorities{8, 8, 8, 8, 8, 8, 8, 8};
    QTest::newRow("Sort ascending - insertion: 5") << Priorities{5, 3} << SortingAlgorithm::INSERTION << SortingOrder::ASCENDING << Priorities{3, 5};
    QTest::newRow("Sort ascending - insertion: 6") << Priorities{3, 5} << SortingAlgorithm::INSERTION << SortingOrder::ASCENDING << Priorities{3, 5};
    QTest::newRow("Sort ascending - insertion: 7") << Priorities{4} << SortingAlgorithm::INSERTION << SortingOrder::ASCENDING << Priorities{4};
    QTest::newRow("Sort ascending - insertion: 8") << Priorities{} << SortingAlgorithm::INSERTION << SortingOrder::ASCENDING << Priorities{};
    QTest::newRow("Sort ascending - heap: 1") << Priorities{2, 3, 5, 2, 9, 6, 1, 8, 7, 5, 4, 6} << SortingAlgorithm::HEAP << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - heap: 2") << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9} << SortingAlgorithm::HEAP << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - heap: 3") << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1} << SortingAlgorithm::HEAP << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - heap: 4") << Priorities{8, 8, 8, 8, 8, 8, 8, 8} << SortingAlgorithm::HEAP << SortingOrder::ASCENDING << Priorities{8, 8, 8, 8, 8, 8, 8, 8};
    QTest::newRow("Sort ascending - heap: 5") << Priorities{5, 3} << SortingAlgorithm::HEAP << SortingOrder::ASCENDING << Priorities{3, 5};
    QTest::newRow("Sort ascending - heap: 6") << Priorities{3, 5} << SortingAlgorithm::HEAP << SortingOrder::ASCENDING << Priorities{3, 5};
    QTest::newRow("Sort ascending - heap: 7") << Priorities{4} << SortingAlgorithm::HEAP << SortingOrder::ASCENDING << Priorities{4};
    QTest::newRow("Sort ascending - heap: 8") << Priorities{} << SortingAlgorithm::HEAP << SortingOrder::ASCENDING << Priorities{};
    QTest::newRow("Sort ascending - merge: 1") << Priorities{2, 3, 5, 2, 9, 6, 1, 8, 7, 5, 4, 6} << SortingAlgorithm::MERGE << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - merge: 2") << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9} << SortingAlgorithm::MERGE << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - merge: 3") << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1} << SortingAlgorithm::MERGE << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - merge: 4") << Priorities{8, 8, 8, 8, 8, 8, 8, 8} << SortingAlgorithm::MERGE << SortingOrder::ASCENDING << Priorities{8, 8, 8, 8, 8, 8, 8, 8};
    QTest::newRow("Sort ascending - merge: 5") << Priorities{5, 3} << SortingAlgorithm::MERGE << SortingOrder::ASCENDING << Priorities{3, 5};
    QTest::newRow("Sort ascending - merge: 6") << Priorities{3, 5} << SortingAlgorithm::MERGE << SortingOrder::ASCENDING << Priorities{3, 5};
    QTest::newRow("Sort ascending - merge: 7") << Priorities{4} << SortingAlgorithm::MERGE << SortingOrder::ASCENDING << Priorities{4};
    QTest::newRow("Sort ascending - merge: 8") << Priorities{} << SortingAlgorithm::MERGE << SortingOrder::ASCENDING << Priorities{};
    QTest::newRow("Sort ascending - quick: 1") << Priorities{2, 3, 5, 2, 9, 6, 1, 8, 7, 5, 4, 6} << SortingAlgorithm::QUICK << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - quick: 2") << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9} << SortingAlgorithm::QUICK << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - quick: 3") << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1} << SortingAlgorithm::QUICK << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - quick: 4") << Priorities{8, 8, 8, 8, 8, 8, 8, 8} << SortingAlgorithm::QUICK << SortingOrder::ASCENDING << Priorities{8, 8, 8, 8, 8, 8, 8, 8};
    QTest::newRow("Sort ascending - quick: 5") << Priorities{5, 3} << SortingAlgorithm::QUICK << SortingOrder::ASCENDING << Priorities{3, 5};
    QTest::newRow("Sort ascending - quick: 6") << Priorities{3, 5} << SortingAlgorithm::QUICK << SortingOrder::ASCENDING << Priorities{3, 5};
    QTest::newRow("Sort ascending - quick: 7") << Priorities{4} << SortingAlgorithm::QUICK << SortingOrder::ASCENDING << Priorities{4};
    QTest::newRow("Sort ascending - quick: 8") << Priorities{} << SortingAlgorithm::QUICK << SortingOrder::ASCENDING << Priorities{};
    QTest::newRow("Sort descending - insertion: 1") << Priorities{2, 3, 5, 2, 9, 6, 1, 8, 7, 5, 4, 6} << SortingAlgorithm::INSERTION << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - insertion: 2") << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9} << SortingAlgorithm::INSERTION << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - insertion: 3") << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1} << SortingAlgorithm::INSERTION << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - insertion: 4") << Priorities{8, 8, 8, 8, 8, 8, 8, 8} << SortingAlgorithm::INSERTION << SortingOrder::DESCENDING << Priorities{8, 8, 8, 8, 8, 8, 8, 8};
    QTest::newRow("Sort descending - insertion: 5") << Priorities{5, 3} << SortingAlgorithm::INSERTION << SortingOrder::DESCENDING << Priorities{5, 3};
    QTest::newRow("Sort descending - insertion: 6") << Priorities{3, 5} << SortingAlgorithm::INSERTION << SortingOrder::DESCENDING << Priorities{5, 3};
    QTest::newRow("Sort descending - insertion: 7") << Priorities{4} << SortingAlgorithm::INSERTION << SortingOrder::DESCENDING << Priorities{4};
    QTest::newRow("Sort descending - insertion: 8") << Priorities{} << SortingAlgorithm::INSERTION << SortingOrder::DESCENDING << Priorities{};
    QTest::newRow("Sort descending - heap: 1") << Priorities{2, 3, 5, 2, 9, 6, 1, 8, 7, 5, 4, 6} << SortingAlgorithm::HEAP << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - heap: 2") << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9} << SortingAlgorithm::HEAP << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - heap: 3") << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1} << SortingAlgorithm::HEAP << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - heap: 4") << Priorities{8, 8, 8, 8, 8, 8, 8, 8} << SortingAlgorithm::HEAP << SortingOrder::DESCENDING << Priorities{8, 8, 8, 8, 8, 8, 8, 8};
    QTest::newRow("Sort descending - heap: 5") << Priorities{5, 3} << SortingAlgorithm::HEAP << SortingOrder::DESCENDING << Priorities{5, 3};
    QTest::newRow("Sort descending - heap: 6") << Priorities{3, 5} << SortingAlgorithm::HEAP << SortingOrder::DESCENDING << Priorities{5, 3};
    QTest::newRow("Sort descending - heap: 7") << Priorities{4} << SortingAlgorithm::HEAP << SortingOrder::DESCENDING << Priorities{4};
    QTest::newRow("Sort descending - heap: 8") << Priorities{} << SortingAlgorithm::HEAP << SortingOrder::DESCENDING << Priorities{};
    QTest::newRow("Sort descending - merge: 1") << Priorities{2, 3, 5, 2, 9, 6, 1, 8, 7, 5, 4, 6} << SortingAlgorithm::MERGE << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - merge: 2") << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9} << SortingAlgorithm::MERGE << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - merge: 3") << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1} << SortingAlgorithm::MERGE << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - merge: 4") << Priorities{8, 8, 8, 8, 8, 8, 8, 8} << SortingAlgorithm::MERGE << SortingOrder::DESCENDING << Priorities{8, 8, 8, 8, 8, 8, 8, 8};
    QTest::newRow("Sort descending - merge: 5") << Priorities{5, 3} << SortingAlgorithm::MERGE << SortingOrder::DESCENDING << Priorities{5, 3};
    QTest::newRow("Sort descending - merge: 6") << Priorities{3, 5} << SortingAlgorithm::MERGE << SortingOrder::DESCENDING << Priorities{5, 3};
    QTest::newRow("Sort descending - merge: 7") << Priorities{4} << SortingAlgorithm::MERGE << SortingOrder::DESCENDING << Priorities{4};
    QTest::newRow("Sort descending - merge: 8") << Priorities{} << SortingAlgorithm::MERGE << SortingOrder::DESCENDING << Priorities{};
    QTest::newRow("Sort descending - quick: 1") << Priorities{2, 3, 5, 2, 9, 6, 1, 8, 7, 5, 4, 6} << SortingAlgorithm::QUICK << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - quick: 2") << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9} << SortingAlgorithm::QUICK << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - quick: 3") << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1} << SortingAlgorithm::QUICK << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - quick: 4") << Priorities{8, 8, 8, 8, 8, 8, 8, 8} << SortingAlgorithm::QUICK << SortingOrder::DESCENDING << Priorities{8, 8, 8, 8, 8, 8, 8, 8};
    QTest::newRow("Sort descending - quick: 5") << Priorities{5, 3} << SortingAlgorithm::QUICK << SortingOrder::DESCENDING << Priorities{5, 3};
    QTest::newRow("Sort descending - quick: 6") << Priorities{3, 5} << SortingAlgorithm::QUICK << SortingOrder::DESCENDING << Priorities{5, 3};
    QTest::newRow("Sort descending - quick: 7") << Priorities{4} << SortingAlgorithm::QUICK << SortingOrder::DESCENDING << Priorities{4};
    QTest::newRow("Sort descending - quick: 8") << Priorities{} << SortingAlgorithm::QUICK << SortingOrder::DESCENDING << Priorities{};
#ifdef UNIX_OS
    QTest::newRow("Sort ascending - enhanced merge: 1") << Priorities{2, 3, 5, 2, 9, 6, 1, 8, 7, 5, 4, 6} << SortingAlgorithm::ENHANCED_MERGE << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - enhanced merge: 2") << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9} << SortingAlgorithm::ENHANCED_MERGE << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - enhanced merge: 3") << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1} << SortingAlgorithm::ENHANCED_MERGE << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - enhanced merge: 4") << Priorities{8, 8, 8, 8, 8, 8, 8, 8} << SortingAlgorithm::ENHANCED_MERGE << SortingOrder::ASCENDING << Priorities{8, 8, 8, 8, 8, 8, 8, 8};
    QTest::newRow("Sort ascending - enhanced merge: 5") << Priorities{5, 3} << SortingAlgorithm::ENHANCED_MERGE << SortingOrder::ASCENDING << Priorities{3, 5};
    QTest::newRow("Sort ascending - enhanced merge: 6") << Priorities{3, 5} << SortingAlgorithm::ENHANCED_MERGE << SortingOrder::ASCENDING << Priorities{3, 5};
    QTest::newRow("Sort ascending - enhanced merge: 7") << Priorities{4} << SortingAlgorithm::ENHANCED_MERGE << SortingOrder::ASCENDING << Priorities{4};
    QTest::newRow("Sort ascending - enhanced merge: 8") << Priorities{} << SortingAlgorithm::ENHANCED_MERGE << SortingOrder::ASCENDING << Priorities{};
    QTest::newRow("Sort ascending - enhanced quick: 1") << Priorities{2, 3, 5, 2, 9, 6, 1, 8, 7, 5, 4, 6} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - enhanced quick: 2") << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - enhanced quick: 3") << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - enhanced quick: 4") << Priorities{8, 8, 8, 8, 8, 8, 8, 8} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::ASCENDING << Priorities{8, 8, 8, 8, 8, 8, 8, 8};
    QTest::newRow("Sort ascending - enhanced quick: 5") << Priorities{5, 3} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::ASCENDING << Priorities{3, 5};
    QTest::newRow("Sort ascending - enhanced quick: 6") << Priorities{3, 5} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::ASCENDING << Priorities{3, 5};
    QTest::newRow("Sort ascending - enhanced quick: 7") << Priorities{4} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::ASCENDING << Priorities{4};
    QTest::newRow("Sort ascending - enhanced quick: 8") << Priorities{} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::ASCENDING << Priorities{};
    QTest::newRow("Sort ascending - quick-merge: 1") << Priorities{2, 3, 5, 2, 9, 6, 1, 8, 7, 5, 4, 6} << SortingAlgorithm::QUICK_MERGE << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - quick-merge: 2") << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9} << SortingAlgorithm::QUICK_MERGE << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - quick-merge: 3") << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1} << SortingAlgorithm::QUICK_MERGE << SortingOrder::ASCENDING << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9};
    QTest::newRow("Sort ascending - quick-merge: 4") << Priorities{8, 8, 8, 8, 8, 8, 8, 8} << SortingAlgorithm::QUICK_MERGE << SortingOrder::ASCENDING << Priorities{8, 8, 8, 8, 8, 8, 8, 8};
    QTest::newRow("Sort ascending - quick-merge: 5") << Priorities{5, 3} << SortingAlgorithm::QUICK_MERGE << SortingOrder::ASCENDING << Priorities{3, 5};
    QTest::newRow("Sort ascending - quick-merge: 6") << Priorities{3, 5} << SortingAlgorithm::QUICK_MERGE << SortingOrder::ASCENDING << Priorities{3, 5};
    QTest::newRow("Sort ascending - quick-merge: 7") << Priorities{4} << SortingAlgorithm::QUICK_MERGE << SortingOrder::ASCENDING << Priorities{4};
    QTest::newRow("Sort ascending - quick-merge: 8") << Priorities{} << SortingAlgorithm::QUICK_MERGE << SortingOrder::ASCENDING << Priorities{};
    QTest::newRow("Sort descending - enhanced merge: 1") << Priorities{2, 3, 5, 2, 9, 6, 1, 8, 7, 5, 4, 6} << SortingAlgorithm::ENHANCED_MERGE << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - enhanced merge: 2") << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9} << SortingAlgorithm::ENHANCED_MERGE << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - enhanced merge: 3") << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1} << SortingAlgorithm::ENHANCED_MERGE << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - enhanced merge: 4") << Priorities{8, 8, 8, 8, 8, 8, 8, 8} << SortingAlgorithm::ENHANCED_MERGE << SortingOrder::DESCENDING << Priorities{8, 8, 8, 8, 8, 8, 8, 8};
    QTest::newRow("Sort descending - enhanced merge: 5") << Priorities{5, 3} << SortingAlgorithm::ENHANCED_MERGE << SortingOrder::DESCENDING << Priorities{5, 3};
    QTest::newRow("Sort descending - enhanced merge: 6") << Priorities{3, 5} << SortingAlgorithm::ENHANCED_MERGE << SortingOrder::DESCENDING << Priorities{5, 3};
    QTest::newRow("Sort descending - enhanced merge: 7") << Priorities{4} << SortingAlgorithm::ENHANCED_MERGE << SortingOrder::DESCENDING << Priorities{4};
    QTest::newRow("Sort descending - enhanced merge: 8") << Priorities{} << SortingAlgorithm::ENHANCED_MERGE << SortingOrder::DESCENDING << Priorities{};
    QTest::newRow("Sort descending - enhanced quick: 1") << Priorities{2, 3, 5, 2, 9, 6, 1, 8, 7, 5, 4, 6} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - enhanced quick: 2") << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - enhanced quick: 3") << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - enhanced quick: 4") << Priorities{8, 8, 8, 8, 8, 8, 8, 8} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::DESCENDING << Priorities{8, 8, 8, 8, 8, 8, 8, 8};
    QTest::newRow("Sort descending - enhanced quick: 5") << Priorities{5, 3} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::DESCENDING << Priorities{5, 3};
    QTest::newRow("Sort descending - enhanced quick: 6") << Priorities{3, 5} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::DESCENDING << Priorities{5, 3};
    QTest::newRow("Sort descending - enhanced quick: 7") << Priorities{4} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::DESCENDING << Priorities{4};
    QTest::newRow("Sort descending - enhanced quick: 8") << Priorities{} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::DESCENDING << Priorities{};
    QTest::newRow("Sort descending - quick-merge: 1") << Priorities{2, 3, 5, 2, 9, 6, 1, 8, 7, 5, 4, 6} << SortingAlgorithm::QUICK_MERGE << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - quick-merge: 2") << Priorities{1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9} << SortingAlgorithm::QUICK_MERGE << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - quick-merge: 3") << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1} << SortingAlgorithm::QUICK_MERGE << SortingOrder::DESCENDING << Priorities{9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 2, 1};
    QTest::newRow("Sort descending - quick-merge: 4") << Priorities{8, 8, 8, 8, 8, 8, 8, 8} << SortingAlgorithm::QUICK_MERGE << SortingOrder::DESCENDING << Priorities{8, 8, 8, 8, 8, 8, 8, 8};
    QTest::newRow("Sort descending - quick-merge: 5") << Priorities{5, 3} << SortingAlgorithm::QUICK_MERGE << SortingOrder::DESCENDING << Priorities{5, 3};
    QTest::newRow("Sort descending - quick-merge: 6") << Priorities{3, 5} << SortingAlgorithm::QUICK_MERGE << SortingOrder::DESCENDING << Priorities{5, 3};
    QTest::newRow("Sort descending - quick-merge: 7") << Priorities{4} << SortingAlgorithm::QUICK_MERGE << SortingOrder::DESCENDING << Priorities{4};
    QTest::newRow("Sort descending - quick-merge: 8") << Priorities{} << SortingAlgorithm::QUICK_MERGE << SortingOrder::DESCENDING << Priorities{};

    // additional enhanced quick sorting tests
    QTest::newRow("Sort ascending - enhanced quick: 9") << Priorities{2, 5, 4, 2, 9} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::ASCENDING << Priorities{2, 2, 4, 5, 9};
    QTest::newRow("Sort ascending - enhanced quick: 10") << Priorities{3, 9, 5, 7, 3, 6} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::ASCENDING << Priorities{3, 3, 5, 6, 7, 9};
    QTest::newRow("Sort ascending - enhanced quick: 11") << Priorities{4, 10, 7, 5, 9, 5, 5} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::ASCENDING << Priorities{4, 5, 5, 5, 7, 9, 10};
    QTest::newRow("Sort descending - enhanced quick: 9") << Priorities{2, 5, 4, 2, 9} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::DESCENDING << Priorities{9, 5, 4, 2, 2};
    QTest::newRow("Sort descending - enhanced quick: 10") << Priorities{3, 9, 5, 7, 3, 6} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::DESCENDING << Priorities{9, 7, 6, 5, 3, 3};
    QTest::newRow("Sort descending - enhanced quick: 11") << Priorities{4, 10, 7, 5, 9, 5, 5} << SortingAlgorithm::ENHANCED_QUICK << SortingOrder::DESCENDING << Priorities{10, 9, 7, 5, 5, 5, 4};
#endif
}

void ListSortingTests::testIsSortedByPriority_data()
{
    QTest::addColumn<Priorities>("priorities");
    QTest::addColumn<SortingOrder>("sortingOrderToCheck");
    QTest::addColumn<SortingStatus>("expectedSortingStatus");

    QTest::newRow("Check ascending sorting: 1") << Priorities{6, 2, 5, 1, 2, 3} << SortingOrder::ASCENDING << SortingStatus::UNSORTED;
    QTest::newRow("Check ascending sorting: 2") << Priorities{1, 2, 2, 3, 5, 6} << SortingOrder::ASCENDING << SortingStatus::SORTED;
    QTest::newRow("Check ascending sorting: 3") << Priorities{6, 5, 4, 2, 2, 1} << SortingOrder::ASCENDING << SortingStatus::UNSORTED;
    QTest::newRow("Check ascending sorting: 4") << Priorities{5, 5, 5, 5, 5, 5} << SortingOrder::ASCENDING << SortingStatus::SORTED;
    QTest::newRow("Check ascending sorting: 5") << Priorities{3, 5} << SortingOrder::ASCENDING << SortingStatus::SORTED;
    QTest::newRow("Check ascending sorting: 6") << Priorities{5, 3} << SortingOrder::ASCENDING << SortingStatus::UNSORTED;
    QTest::newRow("Check ascending sorting: 7") << Priorities{4} << SortingOrder::ASCENDING << SortingStatus::SORTED;
    QTest::newRow("Check ascending sorting: 8") << Priorities{} << SortingOrder::ASCENDING << SortingStatus::SORTED;
    QTest::newRow("Check descending sorting: 1") << Priorities{6, 2, 5, 1, 2, 3} << SortingOrder::DESCENDING << SortingStatus::UNSORTED;
    QTest::newRow("Check descending sorting: 2") << Priorities{6, 5, 4, 2, 2, 1} << SortingOrder::DESCENDING << SortingStatus::SORTED;
    QTest::newRow("Check descending sorting: 3") << Priorities{1, 2, 2, 3, 5, 6} << SortingOrder::DESCENDING << SortingStatus::UNSORTED;
    QTest::newRow("Check descending sorting: 4") << Priorities{5, 5, 5, 5, 5, 5} << SortingOrder::DESCENDING << SortingStatus::SORTED;
    QTest::newRow("Check descending sorting: 5") << Priorities{5, 3} << SortingOrder::DESCENDING << SortingStatus::SORTED;
    QTest::newRow("Check descending sorting: 6") << Priorities{3, 5} << SortingOrder::DESCENDING << SortingStatus::UNSORTED;
    QTest::newRow("Check descending sorting: 7") << Priorities{4} << SortingOrder::DESCENDING << SortingStatus::SORTED;
    QTest::newRow("Check descending sorting: 8") << Priorities{} << SortingOrder::DESCENDING << SortingStatus::SORTED;
}

void ListSortingTests::initTestCase_data()
{
    m_Fixture.init();

    ListElementsPool* p_NullPool{nullptr};

    QTest::addColumn<ListElementsPool*>("pool");

    QTest::newRow("allocation from pool") << m_Fixture.m_Pool;
    QTest::newRow("no pool allocation") << p_NullPool;
}

void ListSortingTests::cleanupTestCase()
{
    QVERIFY(m_Fixture.hasInitialState());
    m_Fixture.destroy();
}

void ListSortingTests::init()
{
    QVERIFY(m_Fixture.hasInitialState());
}

void ListSortingTests::cleanup()
{
    m_Fixture.resetToInitialState();
}

QTEST_APPLESS_MAIN(ListSortingTests)

#include "tst_listsortingtests.moc"
