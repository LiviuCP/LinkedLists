#include <QTest>

#include "listtestfixture.h"
#include "linkedlist.h"
#include "listelementspool.h"
#include "sort.h"

class ListSortingTests : public QObject
{
    Q_OBJECT

private slots:
    void testSortAscendingByPriority();
    void testSortDescendingByPriority();
    void testSortAscendingByPriorityUsingRandomAccess();
    void testSortDescendingByPriorityUsingRandomAccess();
    void testIsSortedAscendingByPriority();
    void testIsSortedDescendingByPriority();

    void testMoveListToArray();
    void testMoveArrayToList();

    void initTestCase_data();
    void cleanupTestCase();
    void init();
    void cleanup();

private:
    size_t _getSumOfPriorities(List* list);

    ListTestFixture m_Fixture;
};

void ListSortingTests::testSortAscendingByPriority()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Fixture.m_Pool);

    {
        const size_t prioritiesArray[6]{6, 2, 5, 1, 2, 3};
        m_Fixture.m_List1 = createListFromPrioritiesArray(prioritiesArray, 6, pool); // highest prio item first

        sortAscendingByPriority(m_Fixture.m_List1);

        QVERIFY(getListSize(m_Fixture.m_List1) == 6);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List1, 0)->priority == 1 && _getSumOfPriorities(m_Fixture.m_List1) == 19 && isSortedAscendingByPriority(m_Fixture.m_List1), "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List1)->priority == 1 && getLastListElement(m_Fixture.m_List1)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    {
        const size_t prioritiesArray[6]{3, 2, 1, 5, 2, 6};
        m_Fixture.m_List2 = createListFromPrioritiesArray(prioritiesArray, 6, pool); // highest prio item last

        sortAscendingByPriority(m_Fixture.m_List2);

        QVERIFY(getListSize(m_Fixture.m_List2) == 6);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List2, 0)->priority == 1 && _getSumOfPriorities(m_Fixture.m_List2) == 19 && isSortedAscendingByPriority(m_Fixture.m_List2), "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List2)->priority == 1 && getLastListElement(m_Fixture.m_List2)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    {
        const size_t prioritiesArray[6]{1, 6, 2, 5, 3, 2};
        m_Fixture.m_List3 = createListFromPrioritiesArray(prioritiesArray, 6, pool); // lowest prio item first

        sortAscendingByPriority(m_Fixture.m_List3);

        QVERIFY(getListSize(m_Fixture.m_List3) == 6);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List3, 0)->priority == 1 && _getSumOfPriorities(m_Fixture.m_List3) == 19 && isSortedAscendingByPriority(m_Fixture.m_List3), "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List3)->priority == 1 && getLastListElement(m_Fixture.m_List3)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    {
        const size_t prioritiesArray[6]{2, 3, 5, 2, 6, 1};
        m_Fixture.m_List4 = createListFromPrioritiesArray(prioritiesArray, 6, pool); // lowest prio item last

        sortAscendingByPriority(m_Fixture.m_List4);

        QVERIFY(getListSize(m_Fixture.m_List4) == 6);
        QVERIFY2(getFirstListElement(m_Fixture.m_List4)->priority == 1 && getLastListElement(m_Fixture.m_List4)->priority == 6, "First and last element of the list are not correctly referenced");
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List4, 0)->priority == 1 && _getSumOfPriorities(m_Fixture.m_List4) == 19 && isSortedAscendingByPriority(m_Fixture.m_List4), "The list hasn't been correctly sorted");
    }

    {
        const size_t prioritiesArray[6]{2, 3, 1, 2, 6, 5};
        m_Fixture.m_List5 = createListFromPrioritiesArray(prioritiesArray, 6, pool); // random

        sortAscendingByPriority(m_Fixture.m_List5);

        QVERIFY(getListSize(m_Fixture.m_List5) == 6);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List5, 0)->priority == 1 && _getSumOfPriorities(m_Fixture.m_List5) == 19 && isSortedAscendingByPriority(m_Fixture.m_List5), "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List5)->priority == 1 && getLastListElement(m_Fixture.m_List5)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    {
        const size_t prioritiesArray[2]{6, 5};
        m_Fixture.m_List6 = createListFromPrioritiesArray(prioritiesArray, 2, pool);
        sortAscendingByPriority(m_Fixture.m_List6);

        QVERIFY(getListSize(m_Fixture.m_List6) == 2);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List6, 0)->priority == 5 && getListElementAtIndex(m_Fixture.m_List6, 1)->priority == 6, "The list hasn't been correctly sorted ");
        QVERIFY2(getFirstListElement(m_Fixture.m_List6)->priority == 5 && getLastListElement(m_Fixture.m_List6)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    {
        m_Fixture.m_List7 = createEmptyList(pool);
        createAndAppendToList(m_Fixture.m_List7, 1);

        sortAscendingByPriority(m_Fixture.m_List7);

        QVERIFY(getListSize(m_Fixture.m_List7) == 1);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List7, 0)->priority == 1, "The list hasn't been correctly sorted (ascending) by priority");
        QVERIFY2(getFirstListElement(m_Fixture.m_List7)->priority == 1 && getLastListElement(m_Fixture.m_List7)->priority == 1, "First and last element of the list are not correctly referenced");
    }
}

void ListSortingTests::testSortDescendingByPriority()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Fixture.m_Pool);

    {
        const size_t prioritiesArray[6]{6, 2, 5, 1, 2, 3};
        m_Fixture.m_List1 = createListFromPrioritiesArray(prioritiesArray, 6, pool); // highest prio item first
        sortDescendingByPriority(m_Fixture.m_List1);

        QVERIFY(getListSize(m_Fixture.m_List1) == 6);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List1, 0)->priority == 6 && _getSumOfPriorities(m_Fixture.m_List1) == 19 && isSortedDescendingByPriority(m_Fixture.m_List1), "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List1)->priority == 6 && getLastListElement(m_Fixture.m_List1)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    {
        const size_t prioritiesArray[6]{3, 2, 1, 5, 2, 6};
        m_Fixture.m_List2 = createListFromPrioritiesArray(prioritiesArray, 6, pool); // highest prio item last
        sortDescendingByPriority(m_Fixture.m_List2);

        QVERIFY(getListSize(m_Fixture.m_List2) == 6);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List2, 0)->priority == 6 && _getSumOfPriorities(m_Fixture.m_List2) == 19 && isSortedDescendingByPriority(m_Fixture.m_List2), "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List2)->priority == 6 && getLastListElement(m_Fixture.m_List2)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    {
        const size_t prioritiesArray[6]{1, 6, 2, 5, 3, 2};
        m_Fixture.m_List3 = createListFromPrioritiesArray(prioritiesArray, 6, pool); // lowest prio item first
        sortDescendingByPriority(m_Fixture.m_List3);

        QVERIFY(getListSize(m_Fixture.m_List3) == 6);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List3, 0)->priority == 6 && _getSumOfPriorities(m_Fixture.m_List3) == 19 && isSortedDescendingByPriority(m_Fixture.m_List3), "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List3)->priority == 6 && getLastListElement(m_Fixture.m_List3)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    {
        const size_t prioritiesArray[6]{2, 3, 5, 2, 6, 1};
        m_Fixture.m_List4 = createListFromPrioritiesArray(prioritiesArray, 6, pool); // lowest prio item last
        sortDescendingByPriority(m_Fixture.m_List4);

        QVERIFY(getListSize(m_Fixture.m_List4) == 6);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List4, 0)->priority == 6 && _getSumOfPriorities(m_Fixture.m_List4) == 19 && isSortedDescendingByPriority(m_Fixture.m_List4), "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List4)->priority == 6 && getLastListElement(m_Fixture.m_List4)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    {
        const size_t prioritiesArray[6]{2, 3, 1, 2, 6, 5};
        m_Fixture.m_List5 = createListFromPrioritiesArray(prioritiesArray, 6, pool); // random
        sortDescendingByPriority(m_Fixture.m_List5);

        QVERIFY(getListSize(m_Fixture.m_List5) == 6);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List5, 0)->priority == 6 && _getSumOfPriorities(m_Fixture.m_List5) == 19 && isSortedDescendingByPriority(m_Fixture.m_List5), "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List5)->priority == 6 && getLastListElement(m_Fixture.m_List5)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    {
        const size_t prioritiesArray[2]{5, 6};
        m_Fixture.m_List6 = createListFromPrioritiesArray(prioritiesArray, 2, pool);
        sortDescendingByPriority(m_Fixture.m_List6);

        QVERIFY(getListSize(m_Fixture.m_List6) == 2);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List6, 0)->priority == 6 && getListElementAtIndex(m_Fixture.m_List6, 1)->priority == 5, "The list hasn't been correctly sorted ");
        QVERIFY2(getFirstListElement(m_Fixture.m_List6)->priority == 6 && getLastListElement(m_Fixture.m_List6)->priority == 5, "First and last element of the list are not correctly referenced");
    }

    {
        m_Fixture.m_List7 = createEmptyList(pool);
        createAndAppendToList(m_Fixture.m_List7, 1);

        sortDescendingByPriority(m_Fixture.m_List7);

        QVERIFY(getListSize(m_Fixture.m_List7) == 1);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List7, 0)->priority == 1, "The list hasn't been correctly sorted (ascending) by priority");
        QVERIFY2(getFirstListElement(m_Fixture.m_List7)->priority == 1 && getLastListElement(m_Fixture.m_List7)->priority == 1, "First and last element of the list are not correctly referenced");
    }
}

void ListSortingTests::testSortAscendingByPriorityUsingRandomAccess()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Fixture.m_Pool);

    const size_t firstPrioritiesArray[6]{2, 3, 5, 2, 6, 1};
    const size_t secondPrioritiesArray[8]{2, 3, 5, 2, 6, 1, 8, 7};
    const size_t thirdPrioritiesArray[12]{2, 3, 5, 2, 9, 6, 1, 8, 7, 5, 4, 6};

    // insertion sort
    {
        m_Fixture.m_List1 = createListFromPrioritiesArray(firstPrioritiesArray, 6, pool);
        sortByRandomAccess(m_Fixture.m_List1, insertionSortAscendingByPriority);

        QVERIFY(getListSize(m_Fixture.m_List1) == 6);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List1, 0)->priority == 1 && _getSumOfPriorities(m_Fixture.m_List1) == 19 && isSortedAscendingByPriority(m_Fixture.m_List1),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List1)->priority == 1 && getLastListElement(m_Fixture.m_List1)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    // heap sort
    {
        m_Fixture.m_List2 = createListFromPrioritiesArray(firstPrioritiesArray, 6, pool);
        sortByRandomAccess(m_Fixture.m_List2, heapSortAscendingByPriority);

        QVERIFY(getListSize(m_Fixture.m_List2) == 6);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List2, 0)->priority == 1 && _getSumOfPriorities(m_Fixture.m_List2) == 19 && isSortedAscendingByPriority(m_Fixture.m_List2),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List2)->priority == 1 && getLastListElement(m_Fixture.m_List2)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    // merge sort
    {
        m_Fixture.m_List3 = createListFromPrioritiesArray(secondPrioritiesArray, 8, pool);
        sortByRandomAccess(m_Fixture.m_List3, mergeSortAscendingByPriority);

        QVERIFY(getListSize(m_Fixture.m_List3) == 8);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List3, 0)->priority == 1 && _getSumOfPriorities(m_Fixture.m_List3) == 34 && isSortedAscendingByPriority(m_Fixture.m_List3),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List3)->priority == 1 && getLastListElement(m_Fixture.m_List3)->priority == 8, "First and last element of the list are not correctly referenced");
    }

    // quick sort
    {
        m_Fixture.m_List4 = createListFromPrioritiesArray(secondPrioritiesArray, 8, pool);
        sortByRandomAccess(m_Fixture.m_List4, quickSortAscendingByPriority);

        QVERIFY(getListSize(m_Fixture.m_List4) == 8);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List4, 0)->priority == 1 && _getSumOfPriorities(m_Fixture.m_List4) == 34 && isSortedAscendingByPriority(m_Fixture.m_List4),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List4)->priority == 1 && getLastListElement(m_Fixture.m_List4)->priority == 8, "First and last element of the list are not correctly referenced");
    }

#ifdef UNIX_OS
    // enhanced merge sort
    {
        m_Fixture.m_List5 = createListFromPrioritiesArray(thirdPrioritiesArray, 12, pool);
        sortByRandomAccess(m_Fixture.m_List5, enhancedMergeSortAscendingByPriority);

        QVERIFY(getListSize(m_Fixture.m_List5) == 12);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List5, 0)->priority == 1 && _getSumOfPriorities(m_Fixture.m_List5) == 58 && isSortedAscendingByPriority(m_Fixture.m_List5),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List5)->priority == 1 && getLastListElement(m_Fixture.m_List5)->priority == 9, "First and last element of the list are not correctly referenced");
    }

    // quick merge sort
    {
        m_Fixture.m_List6 = createListFromPrioritiesArray(thirdPrioritiesArray, 12, pool);
        sortByRandomAccess(m_Fixture.m_List6, quickMergeSortAscendingByPriority);

        QVERIFY(getListSize(m_Fixture.m_List6) == 12);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List6, 0)->priority == 1 && _getSumOfPriorities(m_Fixture.m_List6) == 58 && isSortedAscendingByPriority(m_Fixture.m_List6),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List6)->priority == 1 && getLastListElement(m_Fixture.m_List6)->priority == 9, "First and last element of the list are not correctly referenced");
    }

    // enhanced quick sort
    {
        m_Fixture.m_List7 = createListFromPrioritiesArray(thirdPrioritiesArray, 12, pool);
        sortByRandomAccess(m_Fixture.m_List7, enhancedQuickSortAscendingByPriority);

        QVERIFY(getListSize(m_Fixture.m_List7) == 12);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List7, 0)->priority == 1 && _getSumOfPriorities(m_Fixture.m_List7) == 58 && isSortedAscendingByPriority(m_Fixture.m_List7),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List7)->priority == 1 && getLastListElement(m_Fixture.m_List7)->priority == 9, "First and last element of the list are not correctly referenced");
    }
#endif
}

void ListSortingTests::testSortDescendingByPriorityUsingRandomAccess()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Fixture.m_Pool);

    const size_t firstPrioritiesArray[6]{2, 3, 5, 2, 6, 1};
    const size_t secondPrioritiesArray[8]{2, 3, 5, 2, 6, 1, 8, 7};
    const size_t thirdPrioritiesArray[12]{2, 3, 5, 2, 9, 6, 1, 8, 7, 5, 4, 6};

    // insertion sort
    {
        m_Fixture.m_List1 = createListFromPrioritiesArray(firstPrioritiesArray, 6, pool);
        sortByRandomAccess(m_Fixture.m_List1, insertionSortDescendingByPriority);

        QVERIFY(getListSize(m_Fixture.m_List1) == 6);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List1, 0)->priority == 6 && _getSumOfPriorities(m_Fixture.m_List1) == 19 && isSortedDescendingByPriority(m_Fixture.m_List1),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List1)->priority == 6 && getLastListElement(m_Fixture.m_List1)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    // heap sort
    {
        m_Fixture.m_List2 = createListFromPrioritiesArray(firstPrioritiesArray, 6, pool);
        sortByRandomAccess(m_Fixture.m_List2, insertionSortDescendingByPriority);

        QVERIFY(getListSize(m_Fixture.m_List2) == 6);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List2, 0)->priority == 6 && _getSumOfPriorities(m_Fixture.m_List2) == 19 && isSortedDescendingByPriority(m_Fixture.m_List2),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List2)->priority == 6 && getLastListElement(m_Fixture.m_List2)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    // merge sort
    {
        m_Fixture.m_List3 = createListFromPrioritiesArray(secondPrioritiesArray, 8, pool);
        sortByRandomAccess(m_Fixture.m_List3, insertionSortDescendingByPriority);

        QVERIFY(getListSize(m_Fixture.m_List3) == 8);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List3, 0)->priority == 8 && _getSumOfPriorities(m_Fixture.m_List3) == 34 && isSortedDescendingByPriority(m_Fixture.m_List3),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List3)->priority == 8 && getLastListElement(m_Fixture.m_List3)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    // quick sort
    {
        m_Fixture.m_List4 = createListFromPrioritiesArray(secondPrioritiesArray, 8, pool);
        sortByRandomAccess(m_Fixture.m_List4, insertionSortDescendingByPriority);

        QVERIFY(getListSize(m_Fixture.m_List4) == 8);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List4, 0)->priority == 8 && _getSumOfPriorities(m_Fixture.m_List4) == 34 && isSortedDescendingByPriority(m_Fixture.m_List4),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List4)->priority == 8 && getLastListElement(m_Fixture.m_List4)->priority == 1, "First and last element of the list are not correctly referenced");
    }

#ifdef UNIX_OS
    // enhanced merge sort
    {
        m_Fixture.m_List5 = createListFromPrioritiesArray(thirdPrioritiesArray, 12, pool);
        sortByRandomAccess(m_Fixture.m_List5, enhancedMergeSortDescendingByPriority);

        QVERIFY(getListSize(m_Fixture.m_List5) == 12);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List5, 0)->priority == 9 && _getSumOfPriorities(m_Fixture.m_List5) == 58 && isSortedDescendingByPriority(m_Fixture.m_List5),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List5)->priority == 9 && getLastListElement(m_Fixture.m_List5)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    // quick merge sort
    {
        m_Fixture.m_List6 = createListFromPrioritiesArray(thirdPrioritiesArray, 12, pool);
        sortByRandomAccess(m_Fixture.m_List6, quickMergeSortDescendingByPriority);

        QVERIFY(getListSize(m_Fixture.m_List6) == 12);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List6, 0)->priority == 9 && _getSumOfPriorities(m_Fixture.m_List6) == 58 && isSortedDescendingByPriority(m_Fixture.m_List6),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List6)->priority == 9 && getLastListElement(m_Fixture.m_List6)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    // enhanced quick sort
    {
        m_Fixture.m_List7 = createListFromPrioritiesArray(thirdPrioritiesArray, 12, pool);
        sortByRandomAccess(m_Fixture.m_List7, enhancedQuickSortDescendingByPriority);

        QVERIFY(getListSize(m_Fixture.m_List7) == 12);
        QVERIFY2(getListElementAtIndex(m_Fixture.m_List7, 0)->priority == 9 && _getSumOfPriorities(m_Fixture.m_List7) == 58 && isSortedDescendingByPriority(m_Fixture.m_List7),
                 "The list hasn't been correctly sorted");
        QVERIFY2(getFirstListElement(m_Fixture.m_List7)->priority == 9 && getLastListElement(m_Fixture.m_List7)->priority == 1, "First and last element of the list are not correctly referenced");
    }
#endif
}

void ListSortingTests::testIsSortedAscendingByPriority()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Fixture.m_Pool);

    {
        const size_t prioritiesArray[6]{6, 2, 5, 1, 2, 3};
        m_Fixture.m_List1 = createListFromPrioritiesArray(prioritiesArray, 6, pool);

        QVERIFY(getListSize(m_Fixture.m_List1) == 6);
        QVERIFY2(!isSortedAscendingByPriority(m_Fixture.m_List1), "The list is incorrectly marked as sorted ascending by priority");
    }

    {
        const size_t prioritiesArray[6]{1, 2, 2, 3, 5, 6};
        m_Fixture.m_List2 = createListFromPrioritiesArray(prioritiesArray, 6, pool);

        QVERIFY(getListSize(m_Fixture.m_List2) == 6);
        QVERIFY2(isSortedAscendingByPriority(m_Fixture.m_List2), "The list is incorrectly marked as sorted ascending by priority");
    }

    {
        const size_t prioritiesArray[6]{5, 5, 5, 5, 5, 5};
        m_Fixture.m_List3 = createListFromPrioritiesArray(prioritiesArray, 6, pool);

        QVERIFY(getListSize(m_Fixture.m_List3) == 6);
        QVERIFY2(isSortedAscendingByPriority(m_Fixture.m_List3), "The list is incorrectly marked as sorted ascending by priority");
    }
}

void ListSortingTests::testIsSortedDescendingByPriority()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Fixture.m_Pool);

    {
        const size_t prioritiesArray[6]{6, 2, 5, 1, 2, 3};
        m_Fixture.m_List1 = createListFromPrioritiesArray(prioritiesArray, 6, pool);

        QVERIFY(getListSize(m_Fixture.m_List1) == 6);
        QVERIFY2(!isSortedDescendingByPriority(m_Fixture.m_List1), "The list is incorrectly marked as sorted descending by priority");
    }

    {
        const size_t prioritiesArray[6]{6, 5, 4, 2, 2, 1};
        m_Fixture.m_List2 = createListFromPrioritiesArray(prioritiesArray, 6, pool);

        QVERIFY(getListSize(m_Fixture.m_List2) == 6);
        QVERIFY2(isSortedDescendingByPriority(m_Fixture.m_List2), "The list is incorrectly marked as sorted descending by priority");
    }

    {
        const size_t prioritiesArray[6]{5, 5, 5, 5, 5, 5};
        m_Fixture.m_List3 = createListFromPrioritiesArray(prioritiesArray, 6, pool);

        QVERIFY(getListSize(m_Fixture.m_List3) == 6);
        QVERIFY2(isSortedDescendingByPriority(m_Fixture.m_List3), "The list is incorrectly marked as sorted descending by priority");
    }
}

void ListSortingTests::testMoveListToArray()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Fixture.m_Pool);

    const size_t prioritiesArray[4]{6, 2, 5, 9};

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

void ListSortingTests::initTestCase_data()
{
    m_Fixture.init();
    QVERIFY(m_Fixture.hasInitialState());

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
    QVERIFY(m_Fixture.hasInitialState());
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
