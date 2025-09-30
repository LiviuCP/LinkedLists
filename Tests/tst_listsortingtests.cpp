#include <QTest>

#include "linkedlist.h"
#include "listelementspool.h"
#include "sort.h"

class ListSortingTests : public QObject
{
    Q_OBJECT

public:
    explicit ListSortingTests();

private slots:
    void testSortAscendingByPriority();
    void testSortDescendingByPriority();
    void testSortAscendingByPriorityUsingRandomAccess();
    void testSortDescendingByPriorityUsingRandomAccess();
    void testIsSortedAscendingByPriority();
    void testIsSortedDescendingByPriority();

    void initTestCase_data();
    void cleanupTestCase();
    void init();
    void cleanup();

private:
    size_t _getSumOfPriorities(List* list);

    ListElementsPool* m_Pool;
    size_t m_TotalAvailablePoolElementsCount;

    List* m_List1;
    List* m_List2;
    List* m_List3;
    List* m_List4;
    List* m_List5;
    List* m_List6;
    List* m_List7;
};

ListSortingTests::ListSortingTests()
    : m_Pool{nullptr}
    , m_TotalAvailablePoolElementsCount{0}
    , m_List1{nullptr}
    , m_List2{nullptr}
    , m_List3{nullptr}
    , m_List4{nullptr}
    , m_List5{nullptr}
    , m_List6{nullptr}
    , m_List7{nullptr}
{
}

void ListSortingTests::testSortAscendingByPriority()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    {
        const size_t prioritiesArray[6]{6, 2, 5, 1, 2, 3};
        m_List1 = createListFromPrioritiesArray(prioritiesArray, 6, pool); // highest prio item first

        sortAscendingByPriority(m_List1);

        QVERIFY(getListSize(m_List1) == 6);
        QVERIFY2(getListElementAtIndex(m_List1, 0)->priority == 1 && _getSumOfPriorities(m_List1) == 19 && isSortedAscendingByPriority(m_List1), "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List1)->priority == 1 && getLastListElement(m_List1)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    {
        const size_t prioritiesArray[6]{3, 2, 1, 5, 2, 6};
        m_List2 = createListFromPrioritiesArray(prioritiesArray, 6, pool); // highest prio item last

        sortAscendingByPriority(m_List2);

        QVERIFY(getListSize(m_List2) == 6);
        QVERIFY2(getListElementAtIndex(m_List2, 0)->priority == 1 && _getSumOfPriorities(m_List2) == 19 && isSortedAscendingByPriority(m_List2), "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List2)->priority == 1 && getLastListElement(m_List2)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    {
        const size_t prioritiesArray[6]{1, 6, 2, 5, 3, 2};
        m_List3 = createListFromPrioritiesArray(prioritiesArray, 6, pool); // lowest prio item first

        sortAscendingByPriority(m_List3);

        QVERIFY(getListSize(m_List3) == 6);
        QVERIFY2(getListElementAtIndex(m_List3, 0)->priority == 1 && _getSumOfPriorities(m_List3) == 19 && isSortedAscendingByPriority(m_List3), "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List3)->priority == 1 && getLastListElement(m_List3)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    {
        const size_t prioritiesArray[6]{2, 3, 5, 2, 6, 1};
        m_List4 = createListFromPrioritiesArray(prioritiesArray, 6, pool); // lowest prio item last

        sortAscendingByPriority(m_List4);

        QVERIFY(getListSize(m_List4) == 6);
        QVERIFY2(getFirstListElement(m_List4)->priority == 1 && getLastListElement(m_List4)->priority == 6, "First and last element of the list are not correctly referenced");
        QVERIFY2(getListElementAtIndex(m_List4, 0)->priority == 1 && _getSumOfPriorities(m_List4) == 19 && isSortedAscendingByPriority(m_List4), "The list hasn't been correctly sorted");
    }

    {
        const size_t prioritiesArray[6]{2, 3, 1, 2, 6, 5};
        m_List5 = createListFromPrioritiesArray(prioritiesArray, 6, pool); // random

        sortAscendingByPriority(m_List5);

        QVERIFY(getListSize(m_List5) == 6);
        QVERIFY2(getListElementAtIndex(m_List5, 0)->priority == 1 && _getSumOfPriorities(m_List5) == 19 && isSortedAscendingByPriority(m_List5), "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List5)->priority == 1 && getLastListElement(m_List5)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    {
        const size_t prioritiesArray[2]{6, 5};
        m_List6 = createListFromPrioritiesArray(prioritiesArray, 2, pool);
        sortAscendingByPriority(m_List6);

        QVERIFY(getListSize(m_List6) == 2);
        QVERIFY2(getListElementAtIndex(m_List6, 0)->priority == 5 && getListElementAtIndex(m_List6, 1)->priority == 6, "The list hasn't been correctly sorted ");
        QVERIFY2(getFirstListElement(m_List6)->priority == 5 && getLastListElement(m_List6)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    {
        m_List7 = createEmptyList(pool);
        createAndAppendToList(m_List7, 1);

        sortAscendingByPriority(m_List7);

        QVERIFY(getListSize(m_List7) == 1);
        QVERIFY2(getListElementAtIndex(m_List7, 0)->priority == 1, "The list hasn't been correctly sorted (ascending) by priority");
        QVERIFY2(getFirstListElement(m_List7)->priority == 1 && getLastListElement(m_List7)->priority == 1, "First and last element of the list are not correctly referenced");
    }
}

void ListSortingTests::testSortDescendingByPriority()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    {
        const size_t prioritiesArray[6]{6, 2, 5, 1, 2, 3};
        m_List1 = createListFromPrioritiesArray(prioritiesArray, 6, pool); // highest prio item first
        sortDescendingByPriority(m_List1);

        QVERIFY(getListSize(m_List1) == 6);
        QVERIFY2(getListElementAtIndex(m_List1, 0)->priority == 6 && _getSumOfPriorities(m_List1) == 19 && isSortedDescendingByPriority(m_List1), "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List1)->priority == 6 && getLastListElement(m_List1)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    {
        const size_t prioritiesArray[6]{3, 2, 1, 5, 2, 6};
        m_List2 = createListFromPrioritiesArray(prioritiesArray, 6, pool); // highest prio item last
        sortDescendingByPriority(m_List2);

        QVERIFY(getListSize(m_List2) == 6);
        QVERIFY2(getListElementAtIndex(m_List2, 0)->priority == 6 && _getSumOfPriorities(m_List2) == 19 && isSortedDescendingByPriority(m_List2), "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List2)->priority == 6 && getLastListElement(m_List2)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    {
        const size_t prioritiesArray[6]{1, 6, 2, 5, 3, 2};
        m_List3 = createListFromPrioritiesArray(prioritiesArray, 6, pool); // lowest prio item first
        sortDescendingByPriority(m_List3);

        QVERIFY(getListSize(m_List3) == 6);
        QVERIFY2(getListElementAtIndex(m_List3, 0)->priority == 6 && _getSumOfPriorities(m_List3) == 19 && isSortedDescendingByPriority(m_List3), "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List3)->priority == 6 && getLastListElement(m_List3)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    {
        const size_t prioritiesArray[6]{2, 3, 5, 2, 6, 1};
        m_List4 = createListFromPrioritiesArray(prioritiesArray, 6, pool); // lowest prio item last
        sortDescendingByPriority(m_List4);

        QVERIFY(getListSize(m_List4) == 6);
        QVERIFY2(getListElementAtIndex(m_List4, 0)->priority == 6 && _getSumOfPriorities(m_List4) == 19 && isSortedDescendingByPriority(m_List4), "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List4)->priority == 6 && getLastListElement(m_List4)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    {
        const size_t prioritiesArray[6]{2, 3, 1, 2, 6, 5};
        m_List5 = createListFromPrioritiesArray(prioritiesArray, 6, pool); // random
        sortDescendingByPriority(m_List5);

        QVERIFY(getListSize(m_List5) == 6);
        QVERIFY2(getListElementAtIndex(m_List5, 0)->priority == 6 && _getSumOfPriorities(m_List5) == 19 && isSortedDescendingByPriority(m_List5), "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List5)->priority == 6 && getLastListElement(m_List5)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    {
        const size_t prioritiesArray[2]{5, 6};
        m_List6 = createListFromPrioritiesArray(prioritiesArray, 2, pool);
        sortDescendingByPriority(m_List6);

        QVERIFY(getListSize(m_List6) == 2);
        QVERIFY2(getListElementAtIndex(m_List6, 0)->priority == 6 && getListElementAtIndex(m_List6, 1)->priority == 5, "The list hasn't been correctly sorted ");
        QVERIFY2(getFirstListElement(m_List6)->priority == 6 && getLastListElement(m_List6)->priority == 5, "First and last element of the list are not correctly referenced");
    }

    {
        m_List7 = createEmptyList(pool);
        createAndAppendToList(m_List7, 1);

        sortDescendingByPriority(m_List7);

        QVERIFY(getListSize(m_List7) == 1);
        QVERIFY2(getListElementAtIndex(m_List7, 0)->priority == 1, "The list hasn't been correctly sorted (ascending) by priority");
        QVERIFY2(getFirstListElement(m_List7)->priority == 1 && getLastListElement(m_List7)->priority == 1, "First and last element of the list are not correctly referenced");
    }
}

void ListSortingTests::testSortAscendingByPriorityUsingRandomAccess()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    const size_t firstPrioritiesArray[6]{2, 3, 5, 2, 6, 1};
    const size_t secondPrioritiesArray[8]{2, 3, 5, 2, 6, 1, 8, 7};
    const size_t thirdPrioritiesArray[12]{2, 3, 5, 2, 9, 6, 1, 8, 7, 5, 4, 6};

    // insertion sort
    {
        m_List1 = createListFromPrioritiesArray(firstPrioritiesArray, 6, pool);
        sortByRandomAccess(m_List1, insertionSortAscendingByPriority);

        QVERIFY(getListSize(m_List1) == 6);
        QVERIFY2(getListElementAtIndex(m_List1, 0)->priority == 1 && _getSumOfPriorities(m_List1) == 19 && isSortedAscendingByPriority(m_List1),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List1)->priority == 1 && getLastListElement(m_List1)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    // heap sort
    {
        m_List2 = createListFromPrioritiesArray(firstPrioritiesArray, 6, pool);
        sortByRandomAccess(m_List2, heapSortAscendingByPriority);

        QVERIFY(getListSize(m_List2) == 6);
        QVERIFY2(getListElementAtIndex(m_List2, 0)->priority == 1 && _getSumOfPriorities(m_List2) == 19 && isSortedAscendingByPriority(m_List2),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List2)->priority == 1 && getLastListElement(m_List2)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    // merge sort
    {
        m_List3 = createListFromPrioritiesArray(secondPrioritiesArray, 8, pool);
        sortByRandomAccess(m_List3, mergeSortAscendingByPriority);

        QVERIFY(getListSize(m_List3) == 8);
        QVERIFY2(getListElementAtIndex(m_List3, 0)->priority == 1 && _getSumOfPriorities(m_List3) == 34 && isSortedAscendingByPriority(m_List3),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List3)->priority == 1 && getLastListElement(m_List3)->priority == 8, "First and last element of the list are not correctly referenced");
    }

    // quick sort
    {
        m_List4 = createListFromPrioritiesArray(secondPrioritiesArray, 8, pool);
        sortByRandomAccess(m_List4, quickSortAscendingByPriority);

        QVERIFY(getListSize(m_List4) == 8);
        QVERIFY2(getListElementAtIndex(m_List4, 0)->priority == 1 && _getSumOfPriorities(m_List4) == 34 && isSortedAscendingByPriority(m_List4),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List4)->priority == 1 && getLastListElement(m_List4)->priority == 8, "First and last element of the list are not correctly referenced");
    }

#ifdef UNIX_OS
    // enhanced merge sort
    {
        m_List5 = createListFromPrioritiesArray(thirdPrioritiesArray, 12, pool);
        sortByRandomAccess(m_List5, enhancedMergeSortAscendingByPriority);

        QVERIFY(getListSize(m_List5) == 12);
        QVERIFY2(getListElementAtIndex(m_List5, 0)->priority == 1 && _getSumOfPriorities(m_List5) == 58 && isSortedAscendingByPriority(m_List5),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List5)->priority == 1 && getLastListElement(m_List5)->priority == 9, "First and last element of the list are not correctly referenced");
    }

    // quick merge sort
    {
        m_List6 = createListFromPrioritiesArray(thirdPrioritiesArray, 12, pool);
        sortByRandomAccess(m_List6, quickMergeSortAscendingByPriority);

        QVERIFY(getListSize(m_List6) == 12);
        QVERIFY2(getListElementAtIndex(m_List6, 0)->priority == 1 && _getSumOfPriorities(m_List6) == 58 && isSortedAscendingByPriority(m_List6),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List6)->priority == 1 && getLastListElement(m_List6)->priority == 9, "First and last element of the list are not correctly referenced");
    }

    // enhanced quick sort
    {
        m_List7 = createListFromPrioritiesArray(thirdPrioritiesArray, 12, pool);
        sortByRandomAccess(m_List7, enhancedQuickSortAscendingByPriority);

        QVERIFY(getListSize(m_List7) == 12);
        QVERIFY2(getListElementAtIndex(m_List7, 0)->priority == 1 && _getSumOfPriorities(m_List7) == 58 && isSortedAscendingByPriority(m_List7),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List7)->priority == 1 && getLastListElement(m_List7)->priority == 9, "First and last element of the list are not correctly referenced");
    }
#endif
}

void ListSortingTests::testSortDescendingByPriorityUsingRandomAccess()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    const size_t firstPrioritiesArray[6]{2, 3, 5, 2, 6, 1};
    const size_t secondPrioritiesArray[8]{2, 3, 5, 2, 6, 1, 8, 7};
    const size_t thirdPrioritiesArray[12]{2, 3, 5, 2, 9, 6, 1, 8, 7, 5, 4, 6};

    // insertion sort
    {
        m_List1 = createListFromPrioritiesArray(firstPrioritiesArray, 6, pool);
        sortByRandomAccess(m_List1, insertionSortDescendingByPriority);

        QVERIFY(getListSize(m_List1) == 6);
        QVERIFY2(getListElementAtIndex(m_List1, 0)->priority == 6 && _getSumOfPriorities(m_List1) == 19 && isSortedDescendingByPriority(m_List1),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List1)->priority == 6 && getLastListElement(m_List1)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    // heap sort
    {
        m_List2 = createListFromPrioritiesArray(firstPrioritiesArray, 6, pool);
        sortByRandomAccess(m_List2, insertionSortDescendingByPriority);

        QVERIFY(getListSize(m_List2) == 6);
        QVERIFY2(getListElementAtIndex(m_List2, 0)->priority == 6 && _getSumOfPriorities(m_List2) == 19 && isSortedDescendingByPriority(m_List2),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List2)->priority == 6 && getLastListElement(m_List2)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    // merge sort
    {
        m_List3 = createListFromPrioritiesArray(secondPrioritiesArray, 8, pool);
        sortByRandomAccess(m_List3, insertionSortDescendingByPriority);

        QVERIFY(getListSize(m_List3) == 8);
        QVERIFY2(getListElementAtIndex(m_List3, 0)->priority == 8 && _getSumOfPriorities(m_List3) == 34 && isSortedDescendingByPriority(m_List3),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List3)->priority == 8 && getLastListElement(m_List3)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    // quick sort
    {
        m_List4 = createListFromPrioritiesArray(secondPrioritiesArray, 8, pool);
        sortByRandomAccess(m_List4, insertionSortDescendingByPriority);

        QVERIFY(getListSize(m_List4) == 8);
        QVERIFY2(getListElementAtIndex(m_List4, 0)->priority == 8 && _getSumOfPriorities(m_List4) == 34 && isSortedDescendingByPriority(m_List4),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List4)->priority == 8 && getLastListElement(m_List4)->priority == 1, "First and last element of the list are not correctly referenced");
    }

#ifdef UNIX_OS
    // enhanced merge sort
    {
        m_List5 = createListFromPrioritiesArray(thirdPrioritiesArray, 12, pool);
        sortByRandomAccess(m_List5, enhancedMergeSortDescendingByPriority);

        QVERIFY(getListSize(m_List5) == 12);
        QVERIFY2(getListElementAtIndex(m_List5, 0)->priority == 9 && _getSumOfPriorities(m_List5) == 58 && isSortedDescendingByPriority(m_List5),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List5)->priority == 9 && getLastListElement(m_List5)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    // quick merge sort
    {
        m_List6 = createListFromPrioritiesArray(thirdPrioritiesArray, 12, pool);
        sortByRandomAccess(m_List6, quickMergeSortDescendingByPriority);

        QVERIFY(getListSize(m_List6) == 12);
        QVERIFY2(getListElementAtIndex(m_List6, 0)->priority == 9 && _getSumOfPriorities(m_List6) == 58 && isSortedDescendingByPriority(m_List6),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List6)->priority == 9 && getLastListElement(m_List6)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    // enhanced quick sort
    {
        m_List7 = createListFromPrioritiesArray(thirdPrioritiesArray, 12, pool);
        sortByRandomAccess(m_List7, enhancedQuickSortDescendingByPriority);

        QVERIFY(getListSize(m_List7) == 12);
        QVERIFY2(getListElementAtIndex(m_List7, 0)->priority == 9 && _getSumOfPriorities(m_List7) == 58 && isSortedDescendingByPriority(m_List7),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_List7)->priority == 9 && getLastListElement(m_List7)->priority == 1, "First and last element of the list are not correctly referenced");
    }
#endif
}

void ListSortingTests::testIsSortedAscendingByPriority()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    {
        const size_t prioritiesArray[6]{6, 2, 5, 1, 2, 3};
        m_List1 = createListFromPrioritiesArray(prioritiesArray, 6, pool);

        QVERIFY(getListSize(m_List1) == 6);
        QVERIFY2(!isSortedAscendingByPriority(m_List1), "The list is incorrectly marked as sorted ascending by priority");
    }

    {
        const size_t prioritiesArray[6]{1, 2, 2, 3, 5, 6};
        m_List2 = createListFromPrioritiesArray(prioritiesArray, 6, pool);

        QVERIFY(getListSize(m_List2) == 6);
        QVERIFY2(isSortedAscendingByPriority(m_List2), "The list is incorrectly marked as sorted ascending by priority");
    }

    {
        const size_t prioritiesArray[6]{5, 5, 5, 5, 5, 5};
        m_List3 = createListFromPrioritiesArray(prioritiesArray, 6, pool);

        QVERIFY(getListSize(m_List3) == 6);
        QVERIFY2(isSortedAscendingByPriority(m_List3), "The list is incorrectly marked as sorted ascending by priority");
    }
}

void ListSortingTests::testIsSortedDescendingByPriority()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    {
        const size_t prioritiesArray[6]{6, 2, 5, 1, 2, 3};
        m_List1 = createListFromPrioritiesArray(prioritiesArray, 6, pool);

        QVERIFY(getListSize(m_List1) == 6);
        QVERIFY2(!isSortedDescendingByPriority(m_List1), "The list is incorrectly marked as sorted descending by priority");
    }

    {
        const size_t prioritiesArray[6]{6, 5, 4, 2, 2, 1};
        m_List2 = createListFromPrioritiesArray(prioritiesArray, 6, pool);

        QVERIFY(getListSize(m_List2) == 6);
        QVERIFY2(isSortedDescendingByPriority(m_List2), "The list is incorrectly marked as sorted descending by priority");
    }

    {
        const size_t prioritiesArray[6]{5, 5, 5, 5, 5, 5};
        m_List3 = createListFromPrioritiesArray(prioritiesArray, 6, pool);

        QVERIFY(getListSize(m_List3) == 6);
        QVERIFY2(isSortedDescendingByPriority(m_List3), "The list is incorrectly marked as sorted descending by priority");
    }
}

void ListSortingTests::initTestCase_data()
{
    m_Pool = createListElementsPool();
    QVERIFY(m_Pool);

    m_TotalAvailablePoolElementsCount = getAvailableElementsCount(m_Pool);

    ListElementsPool* p_NullPool{nullptr};

    QTest::addColumn<ListElementsPool*>("pool");

    QTest::newRow("allocation from pool") << m_Pool;
    QTest::newRow("no pool allocation") << p_NullPool;
}

void ListSortingTests::cleanupTestCase()
{
    QVERIFY(m_Pool);

    deleteListElementsPool(m_Pool);
    m_Pool = nullptr;
    m_TotalAvailablePoolElementsCount = 0;

    QVERIFY(!m_List1);
    QVERIFY(!m_List2);
    QVERIFY(!m_List3);
    QVERIFY(!m_List4);
    QVERIFY(!m_List5);
    QVERIFY(!m_List6);
    QVERIFY(!m_List7);
}

void ListSortingTests::init()
{
    QVERIFY(m_Pool);
    QVERIFY(getAvailableElementsCount(m_Pool) == m_TotalAvailablePoolElementsCount);

    QVERIFY(!m_List1);
    QVERIFY(!m_List2);
    QVERIFY(!m_List3);
    QVERIFY(!m_List4);
    QVERIFY(!m_List5);
    QVERIFY(!m_List6);
    QVERIFY(!m_List7);
}

void ListSortingTests::cleanup()
{
    DELETE_LIST(m_List1, deleteObjectPayload);
    DELETE_LIST(m_List2, deleteObjectPayload);
    DELETE_LIST(m_List3, deleteObjectPayload);
    DELETE_LIST(m_List4, deleteObjectPayload);
    DELETE_LIST(m_List5, deleteObjectPayload);
    DELETE_LIST(m_List6, deleteObjectPayload);
    DELETE_LIST(m_List7, deleteObjectPayload);

    QVERIFY(getAvailableElementsCount(m_Pool) == m_TotalAvailablePoolElementsCount);
}

size_t ListSortingTests::_getSumOfPriorities(List *list)
{
    size_t sum = 0;

    if (list != nullptr)
    {
        for (size_t index{0}; index < getListSize(list); ++index)
        {
            sum = sum + getListElementAtIndex(list, index)->priority;
        }
    }

    return sum;
}

QTEST_APPLESS_MAIN(ListSortingTests)

#include "tst_listsortingtests.moc"
