#include <QTest>

#include <cstring>

#include "linkedlist.h"
#include "listelementspool.h"
#include "codeutils.h"
#include "testobjects.h"

enum class InsertionScenario
{
    APPEND,
    PREPEND,
    INSERT_ELEMENT_BEFORE,
    INSERT_ELEMENT_AFTER
};

enum class RemovalScenario
{
    REMOVE_FIRST_ELEMENT,
    REMOVE_LAST_ELEMENT,
    REMOVE_ELEMENT_BEFORE,
    REMOVE_CURRENT_ELEMENT,
    REMOVE_ELEMENT_AFTER
};

enum class Result
{
    SUCCESS,
    FAIL
};

class LinkedListTests : public QObject
{
    Q_OBJECT

public:
    explicit LinkedListTests();

private slots:
    void testListIsCorrectlyCreatedAndCleared();
    void testAppendOrPrepend();
    void testInsertElementBeforeOrAfter();
    void testRemoveFirstOrLastElement();
    void testRemoveArbitraryElement();
    void testMoveContentToList();
    void testCopyContentToList();
    void testSwapElements();
    void testReverseList();
    void testBatchReverseList();
    void testIterators();
    void testIsElementContained();
    void testGetPreviousElement();
    void testPrintListElementsToFile();

    void testAppendOrPrepend_data();
    void testInsertElementBeforeOrAfter_data();
    void testRemoveFirstOrLastElement_data();
    void testRemoveArbitraryElement_data();
    void testReverseList_data();
    void testBatchReverseList_data();

    void initTestCase_data();
    void cleanupTestCase();
    void init();
    void cleanup();

private:
    void _markListForDeletion(List* list);
    void _markListElementForCleanup(ListElement* element, bool shouldRelease);

    ListElementsPool* m_Pool;
    size_t m_TotalAvailablePoolElementsCount;

    List* m_List1;
    List* m_List2;
    List* m_List3;
    List* m_List4;
    List* m_List5;
    List* m_List6;
    List* m_List7;
    List* m_List8;

    // lists that are not test class members marked for deletion (deleted when running cleanup()) - do not add members (e.g. m_pList1) here
    std::vector<List*> m_ListsMarkedForDeletion;

    // elements collected from test cases that need to be discarded (released to pool or freed)
    std::vector<ListElement*> m_ListElementsMarkedForRelease;
    std::vector<ListElement*> m_ListElementsMarkedForDeletion;
};

LinkedListTests::LinkedListTests()
    : m_Pool{nullptr}
    , m_TotalAvailablePoolElementsCount{0}
    , m_List1{nullptr}
    , m_List2{nullptr}
    , m_List3{nullptr}
    , m_List4{nullptr}
    , m_List5{nullptr}
    , m_List6{nullptr}
    , m_List7{nullptr}
    , m_List8{nullptr}
{
}

void LinkedListTests::testListIsCorrectlyCreatedAndCleared()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    {
        const size_t prioritiesArray[9]{6, 2, 5, 4, 3, 1, 2, 9, 7};
        m_List1 = createListFromPrioritiesArray(prioritiesArray, 9, pool);

        QVERIFY2(getListSize(m_List1) == 9, "List size is not correct");
        QVERIFY2(getFirstListElement(m_List1)->priority == 6 && getLastListElement(m_List1)->priority == 7, "First and last element are not correctly referenced");
        QVERIFY2(getListElementAtIndex(m_List1, 0)->priority == 6 && getListElementAtIndex(m_List1, 4)->priority == 3 && getListElementAtIndex(m_List1, 8)->priority == 7,
                 "The element is not correctly retrieved based on index");
        QVERIFY(getListElementAtIndex(m_List1, 4)->object.type == -1 && getListElementAtIndex(m_List1, 4)->object.payload == nullptr);

        clearList(m_List1, deleteObjectPayload);
        QVERIFY2(isEmptyList(m_List1), "The list has not been correctly emptied");
    }

    {
        List list{nullptr, nullptr, nullptr}; // no elements pool used for the list containing elements stored on stack

        ListElement first{Object{-1, nullptr}, 6, nullptr};
        ListElement second{Object{-1, nullptr}, 2, nullptr};
        ListElement third{Object{-1, nullptr}, 4, nullptr};
        ListElement fourth{Object{-1, nullptr}, 5, nullptr};
        ListElement fifth{Object{-1, nullptr}, 7, nullptr};

        appendToList(&list, &first);
        appendToList(&list, &second);
        appendToList(&list, &third);
        prependToList(&list, &fourth);
        prependToList(&list, &fifth);

        QVERIFY2(getListSize(&list) == 5, "List size is not correct");
        QVERIFY2(getFirstListElement(&list)->priority == 7 && getLastListElement(&list)->priority == 4, "First and last element are not correctly referenced");
        QVERIFY2(getListElementAtIndex(&list, 0)->priority == 7 && getListElementAtIndex(&list, 2)->priority == 6 && getListElementAtIndex(&list, 4)->priority == 4,
                 "The element is not correctly retrieved based on index");
        QVERIFY(getListElementAtIndex(&list, 3)->object.type == -1 && getListElementAtIndex(&list, 3)->object.payload == nullptr);

        detachListElements(&list); // this is how we clear list if elements are on the stack (don't user clearList()!!!)
        QVERIFY(isEmptyList(&list));
    }
}

void LinkedListTests::testAppendOrPrepend()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    QFETCH(std::vector<size_t>, priorities);
    QFETCH(InsertionScenario, insertionScenario);
    QFETCH(size_t, priorityToInsert);
    QFETCH(std::vector<size_t>, expectedPriorities);

    QVERIFY(insertionScenario == InsertionScenario::APPEND || insertionScenario == InsertionScenario::PREPEND);

    const size_t prioritiesCount = priorities.size();
    const size_t expectedPrioritiesCount = prioritiesCount + 1;

    QVERIFY(expectedPriorities.size() == expectedPrioritiesCount);

    List* list = prioritiesCount > 0 ? createListFromPrioritiesArray(priorities.data(), prioritiesCount, pool) : createEmptyList(pool);
    QVERIFY(list);

    _markListForDeletion(list);

    ListElement* insertedElement = insertionScenario == InsertionScenario::APPEND ? createAndAppendToList(list, priorityToInsert)
                                                                                  : createAndPrependToList(list, priorityToInsert);

    QVERIFY(insertedElement && insertedElement->priority == priorityToInsert && insertedElement->object.type == -1 && insertedElement->object.payload == nullptr);
    QVERIFY(getListSize(list) == prioritiesCount + 1);

    for (size_t index = 0; index < expectedPrioritiesCount; ++index)
    {
        QVERIFY(getListElementAtIndex(list, index)->priority == expectedPriorities[index]);
    }

    QVERIFY(getFirstListElement(list)->priority == expectedPriorities[0] && getLastListElement(list)->priority == expectedPriorities[expectedPrioritiesCount - 1]);
}

void LinkedListTests::testInsertElementBeforeOrAfter()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    QFETCH(std::vector<size_t>, priorities);
    QFETCH(InsertionScenario, insertionScenario);
    QFETCH(size_t, iteratorIncrementationsCount);
    QFETCH(size_t, priorityToInsert);
    QFETCH(std::vector<size_t>, expectedPriorities);

    QVERIFY(insertionScenario == InsertionScenario::INSERT_ELEMENT_BEFORE || insertionScenario == InsertionScenario::INSERT_ELEMENT_AFTER);

    const size_t prioritiesCount = priorities.size();
    List* list = prioritiesCount > 0 ? createListFromPrioritiesArray(priorities.data(), prioritiesCount, pool) : createEmptyList(pool);

    QVERIFY(list);
    _markListForDeletion(list);

    ListIterator it = lbegin(list);

    for (size_t index = 0; index < iteratorIncrementationsCount; ++index)
    {
        lnext(&it);
    }

    ListElement* insertedElement = insertionScenario == InsertionScenario::INSERT_ELEMENT_BEFORE ? createAndInsertBefore(it, priorityToInsert)
                                                                                          : createAndInsertAfter(it, priorityToInsert);

    QVERIFY(insertedElement && insertedElement->priority == priorityToInsert && insertedElement->object.type == -1 && !insertedElement->object.payload);

    for (size_t index = 0; index < prioritiesCount; ++index)
    {
        QVERIFY(getListElementAtIndex(list, index)->priority == expectedPriorities[index]);
    }

    QVERIFY(getFirstListElement(list)->priority == expectedPriorities[0] && getLastListElement(list)->priority == expectedPriorities[prioritiesCount]);
}

void LinkedListTests::testRemoveFirstOrLastElement()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    QFETCH(std::vector<size_t>, priorities);
    QFETCH(RemovalScenario, removalScenario);
    QFETCH(Result, expectedResult);
    QFETCH(size_t, removedElementPriority);
    QFETCH(std::vector<size_t>, expectedPriorities);

    QVERIFY(removalScenario == RemovalScenario::REMOVE_FIRST_ELEMENT || removalScenario == RemovalScenario::REMOVE_LAST_ELEMENT);

    const size_t prioritiesCount = priorities.size();
    const size_t expectedPrioritiesCount = expectedPriorities.size();

    QVERIFY(prioritiesCount >= expectedPrioritiesCount && prioritiesCount <= expectedPrioritiesCount + 1);

    List* list = prioritiesCount > 0 ? createListFromPrioritiesArray(priorities.data(), prioritiesCount, pool) : createEmptyList(pool);
    QVERIFY(list);

    _markListForDeletion(list);

    ListElement* removedElement = removalScenario == RemovalScenario::REMOVE_FIRST_ELEMENT ? removeFirstListElement(list)
                                                                                    : removeLastListElement(list);
    _markListElementForCleanup(removedElement, pool != nullptr);

    if (expectedResult == Result::SUCCESS)
    {
        QVERIFY(removedElement && removedElement->priority == removedElementPriority && removedElement->object.type == -1 && !removedElement->object.payload);
    }
    else
    {
        QVERIFY(!removedElement);
    }

    QVERIFY(getListSize(list) == expectedPrioritiesCount);

    for (size_t index = 0; index < expectedPrioritiesCount; ++index)
    {
        QVERIFY(getListElementAtIndex(list, index)->priority == expectedPriorities[index]);
    }

    if (!isEmptyList(list))
    {
        QVERIFY(getFirstListElement(list)->priority == expectedPriorities[0] && getLastListElement(list)->priority == expectedPriorities[expectedPrioritiesCount - 1]);
    }
}

void LinkedListTests::testRemoveArbitraryElement()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    QFETCH(std::vector<size_t>, priorities);
    QFETCH(RemovalScenario, removalScenario);
    QFETCH(size_t, iteratorIncrementationsCount);
    QFETCH(Result, expectedResult);
    QFETCH(size_t, removedElementPriority);
    QFETCH(std::vector<size_t>, expectedPriorities);

    QVERIFY(removalScenario == RemovalScenario::REMOVE_ELEMENT_BEFORE ||
            removalScenario == RemovalScenario::REMOVE_ELEMENT_AFTER ||
            removalScenario == RemovalScenario::REMOVE_CURRENT_ELEMENT);

    const size_t prioritiesCount = priorities.size();
    const size_t expectedPrioritiesCount = expectedPriorities.size();

    QVERIFY(prioritiesCount >= expectedPrioritiesCount && prioritiesCount <= expectedPrioritiesCount + 1);

    List* list = prioritiesCount > 0 ? createListFromPrioritiesArray(priorities.data(), prioritiesCount, pool) : createEmptyList(pool);
    QVERIFY(list);

    _markListForDeletion(list);

    ListIterator it = lbegin(list);

    for (size_t index = 0; index < iteratorIncrementationsCount; ++index)
    {
        lnext(&it);
    }

    ListElement* removedElement = removalScenario == RemovalScenario::REMOVE_ELEMENT_BEFORE ? removePreviousListElement(it)
                                                                            : removalScenario == RemovalScenario::REMOVE_ELEMENT_AFTER ? removeNextListElement(it)
                                                                                                                       : removeCurrentListElement(it);
    _markListElementForCleanup(removedElement, pool != nullptr);

    if (expectedResult == Result::SUCCESS)
    {
        QVERIFY(removedElement && removedElement->priority == removedElementPriority && removedElement->object.type == -1 && !removedElement->object.payload);
    }
    else
    {
        QVERIFY(!removedElement);
    }

    QVERIFY(getListSize(list) == expectedPrioritiesCount);

    for (size_t index = 0; index < expectedPrioritiesCount; ++index)
    {
        QVERIFY(getListElementAtIndex(list, index)->priority == expectedPriorities[index]);
    }

    if (!isEmptyList(list))
    {
        QVERIFY(getFirstListElement(list)->priority == expectedPriorities[0] && getLastListElement(list)->priority == expectedPriorities[expectedPrioritiesCount - 1]);
    }
}

void LinkedListTests::testMoveContentToList()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    const size_t firstPrioritiesArray[2]{6, 2,};
    const size_t secondPrioritiesArray[2]{7, 4};

    {
        m_List1 = createListFromPrioritiesArray(firstPrioritiesArray, 2, pool);
        m_List2 = createListFromPrioritiesArray(secondPrioritiesArray, 2, pool);

        moveContentToList(m_List1, m_List2);

        QVERIFY2(isEmptyList(m_List1) &&
                 getListSize(m_List2) == 4 &&
                 getListElementAtIndex(m_List2, 0)->priority == 7 &&
                 getListElementAtIndex(m_List2, 1)->priority == 4 &&
                 getListElementAtIndex(m_List2, 2)->priority == 6 &&
                 getListElementAtIndex(m_List2, 3)->priority == 2,   "The source list content has not been correctly moved to destination");
        QVERIFY2(getFirstListElement(m_List2)->priority == 7 && getLastListElement(m_List2)->priority == 2, "First and last element of the destination list are not correctly referenced");
    }

    {
        m_List3 = createEmptyList(pool);
        m_List4 = createListFromPrioritiesArray(secondPrioritiesArray, 2, pool);

        moveContentToList(m_List3, m_List4);

        QVERIFY2(isEmptyList(m_List3) &&
                 getListSize(m_List4) == 2 &&
                 getListElementAtIndex(m_List4, 0)->priority == 7 &&
                 getListElementAtIndex(m_List4, 1)->priority == 4,   "The source list content has not been correctly moved to destination");
        QVERIFY2(getFirstListElement(m_List4)->priority == 7 && getLastListElement(m_List4)->priority == 4, "First and last element of the destination list are not correctly referenced");
    }

    {
        m_List5 = createListFromPrioritiesArray(firstPrioritiesArray, 2, pool);
        m_List6 = createEmptyList(pool);

        moveContentToList(m_List5, m_List6);

        QVERIFY2(isEmptyList(m_List5) &&
                 getListSize(m_List6) == 2 &&
                 getListElementAtIndex(m_List6, 0)->priority == 6 &&
                 getListElementAtIndex(m_List6, 1)->priority == 2,   "The source list content has not been correctly moved to destination");
        QVERIFY2(getFirstListElement(m_List6)->priority == 6 && getLastListElement(m_List6)->priority == 2, "First and last element of the destination list are not correctly referenced");
    }
}

void LinkedListTests::testCopyContentToList()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    const size_t firstPrioritiesArray[2]{6, 2};
    const size_t secondPrioritiesArray[2]{7, 4};
    const size_t thirdPriorityArray[3]{5, 4, 3};

    {
        m_List1 = createListFromPrioritiesArray(firstPrioritiesArray, 2, pool);
        m_List2 = createListFromPrioritiesArray(secondPrioritiesArray, 2, pool);

        copyContentToList(m_List1, m_List2, copyObjectPlaceholder, deleteObjectPayload);

        QVERIFY2(getListSize(m_List1) == 2 &&
                 getListSize(m_List2) == 4 &&
                 getListElementAtIndex(m_List2, 0)->priority == 7 &&
                 getListElementAtIndex(m_List2, 1)->priority == 4 &&
                 getListElementAtIndex(m_List2, 2)->priority == 6 &&
                 getListElementAtIndex(m_List2, 3)->priority == 2,   "The source list content has not been correctly copied to destination");
        QVERIFY(getListElementAtIndex(m_List2, 2)->object.type == -1 && getListElementAtIndex(m_List2, 2)->object.payload == nullptr);
        QVERIFY2(getFirstListElement(m_List2)->priority == 7 && getLastListElement(m_List2)->priority == 2, "First and last element of the destination list are not correctly referenced");
        QVERIFY2(getFirstListElement(m_List1)->priority == 6 && getLastListElement(m_List1)->priority == 2, "First and last element of the source list are not correctly referenced");
    }

    {
        m_List3 = createEmptyList(pool);
        m_List4 = createListFromPrioritiesArray(secondPrioritiesArray, 2, pool);

        copyContentToList(m_List3, m_List4, copyObjectPlaceholder, deleteObjectPayload);

        QVERIFY2(isEmptyList(m_List3) &&
                 getListSize(m_List4) == 2 &&
                 getListElementAtIndex(m_List4, 0)->priority == 7 &&
                 getListElementAtIndex(m_List4, 1)->priority == 4,   "The source list content has not been correctly copied to destination");
        QVERIFY2(getFirstListElement(m_List4)->priority == 7 && getLastListElement(m_List4)->priority == 4, "First and last element of the destination list are not correctly referenced");
    }

    {
        m_List5 = createListFromPrioritiesArray(firstPrioritiesArray, 2, pool);
        m_List6 = createEmptyList(pool);

        copyContentToList(m_List5, m_List6, copyObjectPlaceholder, deleteObjectPayload);

        QVERIFY2(getListSize(m_List5) == 2 &&
                 getListSize(m_List6) == 2 &&
                 getListElementAtIndex(m_List6, 0)->priority == 6 &&
                 getListElementAtIndex(m_List6, 1)->priority == 2,   "The source list content has not been correctly copied to destination");
        QVERIFY(getListElementAtIndex(m_List6, 1)->object.type == -1 && getListElementAtIndex(m_List6, 1)->object.payload == nullptr);
        QVERIFY2(getFirstListElement(m_List6)->priority == 6 && getLastListElement(m_List6)->priority == 2, "First and last element of the destination list are not correctly referenced");
        QVERIFY2(getFirstListElement(m_List5)->priority == 6 && getLastListElement(m_List5)->priority == 2, "First and last element of the source list are not correctly referenced");
    }

    {
        m_List7 = createListFromPrioritiesArray(firstPrioritiesArray, 2, pool);
        m_List8 = createListFromPrioritiesArray(thirdPriorityArray, 3, pool);

        ListIterator it = lbegin(m_List7);
        assignObjectContentToListElement(it.current, SEGMENT, createSegmentPayload(2, 5, 4, 11));
        lnext(&it);
        assignObjectContentToListElement(it.current, LOCAL_CONDITIONS, createLocalConditionsPayload(7, -5, 10, 12.8));

        copyContentToList(m_List7, m_List8, customCopyObject, emptyTestObject);

        const Segment* copiedSegment = static_cast<Segment*>(getListElementAtIndex(m_List8, 3)->object.payload);
        const LocalConditions* copiedConditions = static_cast<LocalConditions*>(getListElementAtIndex(m_List8, 4)->object.payload);

        QVERIFY2(getListSize(m_List7) == 2 &&
                 getListSize(m_List8) == 5 &&
                 getListElementAtIndex(m_List8, 0)->priority == 5 &&
                 getListElementAtIndex(m_List8, 0)->object.type == -1 &&
                 getListElementAtIndex(m_List8, 0)->object.payload == nullptr &&
                 getListElementAtIndex(m_List8, 1)->priority == 4 &&
                 getListElementAtIndex(m_List8, 1)->object.type == -1 &&
                 getListElementAtIndex(m_List8, 1)->object.payload == nullptr &&
                 getListElementAtIndex(m_List8, 2)->priority == 3 &&
                 getListElementAtIndex(m_List8, 2)->object.type == -1 &&
                 getListElementAtIndex(m_List8, 2)->object.payload == nullptr &&
                 getListElementAtIndex(m_List8, 3)->priority == 6 &&
                 getListElementAtIndex(m_List8, 3)->object.type == SEGMENT &&
                 copiedSegment->start->x == 2 && copiedSegment->start->y == 5 && copiedSegment->stop->x == 4 && copiedSegment->stop->y == 11 &&
                 getListElementAtIndex(m_List8, 4)->priority == 2 &&
                 getListElementAtIndex(m_List8, 4)->object.type == LOCAL_CONDITIONS &&
                 copiedConditions->position->x == 7 && copiedConditions->position->y == -5 && copiedConditions->temperature == 10 &&
                 areDecimalNumbersEqual(copiedConditions->humidity, 12.8), "The source list content has not been correctly copied to destination");
        QVERIFY2(getFirstListElement(m_List8)->priority == 5 && getLastListElement(m_List8)->priority == 2, "First and last element of the destination list are not correctly referenced");
        QVERIFY2(getFirstListElement(m_List7)->priority == 6 && getLastListElement(m_List7)->priority == 2, "First and last element of the source list are not correctly referenced");

        for (ListIterator it = lbegin(m_List7); !areIteratorsEqual(it, lend(m_List7)); lnext(&it))
        {
            emptyTestObject(&it.current->object);
        }

        for (ListIterator it = lbegin(m_List8); !areIteratorsEqual(it, lend(m_List8)); lnext(&it))
        {
            emptyTestObject(&it.current->object);
        }
    }
}

void LinkedListTests::testSwapElements()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    {
        const size_t prioritiesArray[5]{6, 2, 5, 4, 7};
        m_List1 = createListFromPrioritiesArray(prioritiesArray, 5, pool);

        ListIterator firstIt = lbegin(m_List1);
        ListIterator secondIt = firstIt;

        lnext(&firstIt);
        lnext(&secondIt);
        lnext(&secondIt);
        lnext(&secondIt);

        swapListElements(firstIt, secondIt);

        QVERIFY2(getListSize(m_List1) == 5 &&
                 getListElementAtIndex(m_List1, 0)->priority == 6 &&
                 getListElementAtIndex(m_List1, 1)->priority == 4 &&
                 getListElementAtIndex(m_List1, 2)->priority == 5 &&
                 getListElementAtIndex(m_List1, 3)->priority == 2 &&
                 getListElementAtIndex(m_List1, 4)->priority == 7,   "The elements have not been correctly swapped");
        QVERIFY2(getFirstListElement(m_List1)->priority == 6 && getLastListElement(m_List1)->priority == 7, "First and last element of the list are not correctly referenced");
    }

    {
        m_List2 = createEmptyList(pool);
        createAndAppendToList(m_List2, 2);

        swapListElements(lbegin(m_List2), lbegin(m_List2));

        QVERIFY(getListSize(m_List2) == 1);
        QVERIFY2(getListElementAtIndex(m_List2, 0)->priority == 2, "The swap function does not work correctly");

        swapListElements(lbegin(m_List2), lend(m_List2));

        QVERIFY2(getListElementAtIndex(m_List2, 0)->priority == 2, "The swap function does not work correctly");
        QVERIFY2(getFirstListElement(m_List2)->priority == 2 && getLastListElement(m_List2)->priority == 2, "First and last element of the list are not correctly referenced");
    }
}

void LinkedListTests::testReverseList()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    QFETCH(std::vector<size_t>, priorities);
    QFETCH(std::vector<size_t>, expectedPriorities);

    const size_t prioritiesCount = priorities.size();
    QVERIFY(prioritiesCount == expectedPriorities.size());

    List* list = prioritiesCount > 0 ? createListFromPrioritiesArray(priorities.data(), prioritiesCount, pool) : createEmptyList(pool);
    QVERIFY(list);

    reverseList(list);
    _markListForDeletion(list);

    QVERIFY(getListSize(list) == prioritiesCount);

    for (size_t index = 0; index < prioritiesCount; ++index)
    {
        QVERIFY(getListElementAtIndex(list, index)->priority == expectedPriorities[index]);
    }

    if (prioritiesCount > 0)
    {
        QVERIFY(getFirstListElement(list)->priority == expectedPriorities[0] && getLastListElement(list)->priority == expectedPriorities[prioritiesCount - 1]);
    }
}

void LinkedListTests::testBatchReverseList()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    QFETCH(std::vector<size_t>, priorities);
    QFETCH(size_t, batchSize);
    QFETCH(Result, expectedResult);
    QFETCH(std::vector<size_t>, expectedPriorities);
    QFETCH(size_t, lastReversedElementPriority);

    const size_t prioritiesCount = priorities.size();
    QVERIFY(prioritiesCount == expectedPriorities.size());

    List* list = prioritiesCount > 0 ? createListFromPrioritiesArray(priorities.data(), prioritiesCount, pool) : createEmptyList(pool);
    QVERIFY(list);

    ListElement* lastReversedElement = batchReverseList(list, batchSize);

    _markListForDeletion(list); // in case any QVERIFY fails no memory leak will occur (the list is deleted by cleanup() method)

    QVERIFY(expectedResult == Result::SUCCESS ? lastReversedElement != nullptr && lastReversedElement->priority == lastReversedElementPriority : lastReversedElement == nullptr);
    QVERIFY(getListSize(list) == prioritiesCount);

    for (size_t index = 0; index < prioritiesCount; ++index)
    {
        QVERIFY(getListElementAtIndex(list, index)->priority == expectedPriorities[index]);
    }

    if (prioritiesCount > 0)
    {
        QVERIFY(getFirstListElement(list)->priority == expectedPriorities[0] && getLastListElement(list)->priority == expectedPriorities[prioritiesCount - 1]);
    }
}

void LinkedListTests::testIterators()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    {
        const size_t prioritiesArray[4]{6, 2, 5, 9};
        m_List1 = createListFromPrioritiesArray(prioritiesArray, 4, pool);
        ListIterator it = lbegin(m_List1);

        QVERIFY(getListSize(m_List1) == 4);

        QVERIFY2(!areIteratorsEqual(it, lend(m_List1)) && it.current->priority == 6, "The begin iterator is not correctly generated");
        lnext(&it);
        QVERIFY2(!areIteratorsEqual(it, lend(m_List1)) && it.current->priority == 2, "The iterator is not correctly incremented");
        lnext(&it);
        QVERIFY2(!areIteratorsEqual(it, lend(m_List1)) && it.current->priority == 5, "The iterator is not correctly incremented");
        lnext(&it);
        QVERIFY2(!areIteratorsEqual(it, lend(m_List1)) && it.current->priority == 9, "The iterator is not correctly incremented");
        lnext(&it);
        QVERIFY2(areIteratorsEqual(it, lend(m_List1)), "The iterator is not correctly incremented");
        lnext(&it);
        QVERIFY2(areIteratorsEqual(it, lend(m_List1)), "The iterator is not correctly incremented");
    }

    {
        m_List2 = createEmptyList(pool);
        ListIterator it = lbegin(m_List2);

        QVERIFY(isEmptyList(m_List2));

        QVERIFY2(areIteratorsEqual(it, lend(m_List2)), "The list is empty but the begin and end iterators are not equal");
        lnext(&it);
        QVERIFY2(areIteratorsEqual(it, lend(m_List2)), "The iterator is not correctly incremented");
    }
}

void LinkedListTests::testIsElementContained()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    ListElement* firstElement = pool ? aquireElement(pool) : createListElement();
    QVERIFY(firstElement);
    firstElement->priority = 10;

    ListElement* secondElement = pool ? aquireElement(pool) : createListElement();
    QVERIFY(secondElement);
    secondElement->priority = 10;

    m_List1 = createEmptyList(pool);
    createAndAppendToList(m_List1, 7);
    appendToList(m_List1, firstElement);
    createAndAppendToList(m_List1, 5);
    createAndPrependToList(m_List1, 4);

    QVERIFY(getListSize(m_List1) == 4);
    QVERIFY2(isListElementContained(firstElement, m_List1), "Element is marked as not contained in the list (should be");
    QVERIFY2(!isListElementContained(secondElement, m_List1), "Element is marked as contained in the list (should NOT be");

    _markListElementForCleanup(secondElement, pool != nullptr);
}

void LinkedListTests::testGetPreviousElement()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    {
        const size_t prioritiesArray[3]{6, 2, 5};
        m_List1 = createListFromPrioritiesArray(prioritiesArray, 3, pool);

        QVERIFY(getListSize(m_List1) == 3);

        ListIterator it = lbegin(m_List1);
        QVERIFY2(getPreviousListElement(it) == nullptr, "Previous list element is not correctly determined");

        lnext(&it);
        QVERIFY2(getPreviousListElement(it)->priority == 6, "Previous list element is not correctly determined");

        lnext(&it);
        QVERIFY2(getPreviousListElement(it)->priority == 2, "Previous list element is not correctly determined");

        lnext(&it);
        QVERIFY2(getPreviousListElement(it)->priority == 5, "Previous list element is not correctly determined");
    }

    {
        m_List2 = createEmptyList(pool);
        ListIterator it = lbegin(m_List2);

        QVERIFY(isEmptyList(m_List2));
        QVERIFY2(getPreviousListElement(it) == nullptr, "Previous list element is not correctly determined");
    }
}

void LinkedListTests::testPrintListElementsToFile()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    const size_t prioritiesArray[5]{3, 2, 5, 9, 4};
    m_List1 = createListFromPrioritiesArray(prioritiesArray, 5, pool);

    QVERIFY(getListSize(m_List1) == 5);

#ifdef _WIN32
    const char* testDataFile = "C:\\test.txt";
#else
    const char* testDataFile = "/tmp/test.txt";
#endif

    Point* point = static_cast<Point*>(malloc(sizeof(Point)));
    point->x = 5;
    point->y = 7;
    assignObjectContentToListElement(getListElementAtIndex(m_List1, 0), POINT, static_cast<void*>(point));
    point = nullptr;

    int* distance = static_cast<int*>(malloc(sizeof(int)));
    *distance = 4;
    assignObjectContentToListElement(getListElementAtIndex(m_List1, 2), INTEGER, static_cast<void*>(distance));
    distance = nullptr;

    double* angle = static_cast<double*>(malloc(sizeof(double)));
    *angle = 1.44;
    assignObjectContentToListElement(getListElementAtIndex(m_List1, 3), DECIMAL, static_cast<void*>(angle));
    angle = nullptr;

    printListContentToFile(m_List1, testDataFile, "");

    char* buffer = static_cast<char*>(malloc(100));
    FILE* readTestData = fopen(testDataFile, "r");

    if (!readTestData)
    {
        free(buffer);
        buffer = nullptr;
        QFAIL("File opening failure!");
    }

    char* result = fgets(buffer, 100, readTestData);
    const bool success1 = strcmp(buffer, "Element: 0\tPriority: 3\tHas empty Object: no\tObject type: point\n") == 0;

    if (result)
    {
        result = fgets(buffer, 100, readTestData);
    }

    const bool success2 = strcmp(buffer, "Element: 1\tPriority: 2\tHas empty Object: yes\n") == 0;

    if (result)
    {
        result = fgets(buffer, 100, readTestData);
    }

    const bool success3 = strcmp(buffer, "Element: 2\tPriority: 5\tHas empty Object: no\tObject type: integer\n") == 0;

    if (result)
    {
        result = fgets(buffer, 100, readTestData);
    }

    const bool success4 = strcmp(buffer, "Element: 3\tPriority: 9\tHas empty Object: no\tObject type: decimal\n") == 0;

    if (result)
    {
        fgets(buffer, 100, readTestData);
    }

    const bool success5 = strcmp(buffer, "Element: 4\tPriority: 4\tHas empty Object: yes");

    fclose(readTestData);
    readTestData = nullptr;
    free(buffer);
    buffer = nullptr;

    // checks performed at the end to avoid memory leaks
    QVERIFY2(success1, "First element is not printed correctly");
    QVERIFY2(success2, "Second element is not printed correctly");
    QVERIFY2(success3, "Third element is not printed correctly");
    QVERIFY2(success4, "Fourth element is not printed correctly");
    QVERIFY2(success5, "Fifth element is not printed correctly");
}

void LinkedListTests::testAppendOrPrepend_data()
{
    QTest::addColumn<std::vector<size_t>>("priorities");
    QTest::addColumn<InsertionScenario>("insertionScenario");
    QTest::addColumn<size_t>("priorityToInsert");
    QTest::addColumn<std::vector<size_t>>("expectedPriorities");

    QTest::newRow("Append: 1") << std::vector<size_t>{6, 2, 5, 4, 3, 1, 2, 9, 7} << InsertionScenario::APPEND << size_t{10} << std::vector<size_t>{6, 2, 5, 4, 3, 1, 2, 9, 7, 10};
    QTest::newRow("Append: 2") << std::vector<size_t>{6, 2} << InsertionScenario::APPEND << size_t{10} << std::vector<size_t>{6, 2, 10};
    QTest::newRow("Append: 3") << std::vector<size_t>{6} << InsertionScenario::APPEND << size_t{10} << std::vector<size_t>{6, 10};
    QTest::newRow("Append: 4") << std::vector<size_t>{} << InsertionScenario::APPEND << size_t{10} << std::vector<size_t>{10};
    QTest::newRow("Prepend: 1") << std::vector<size_t>{6, 2, 5, 4, 3, 1, 2, 9, 7} << InsertionScenario::PREPEND << size_t{10} << std::vector<size_t>{10, 6, 2, 5, 4, 3, 1, 2, 9, 7};
    QTest::newRow("Prepend: 2") << std::vector<size_t>{6, 2} << InsertionScenario::PREPEND << size_t{10} << std::vector<size_t>{10, 6, 2};
    QTest::newRow("Prepend: 3") << std::vector<size_t>{6} << InsertionScenario::PREPEND << size_t{10} << std::vector<size_t>{10, 6};
    QTest::newRow("Prepend: 4") << std::vector<size_t>{} << InsertionScenario::PREPEND << size_t{10} << std::vector<size_t>{10};
}

void LinkedListTests::testInsertElementBeforeOrAfter_data()
{
    QTest::addColumn<std::vector<size_t>>("priorities");
    QTest::addColumn<InsertionScenario>("insertionScenario");
    QTest::addColumn<size_t>("iteratorIncrementationsCount");
    QTest::addColumn<size_t>("priorityToInsert");
    QTest::addColumn<std::vector<size_t>>("expectedPriorities");

    QTest::newRow("Insert before: 1") << std::vector<size_t>{6, 2, 5, 4} << InsertionScenario::INSERT_ELEMENT_BEFORE << size_t{0} << size_t{10} << std::vector<size_t>{10, 6, 2, 5, 4};
    QTest::newRow("Insert before: 2") << std::vector<size_t>{6, 2, 5, 4} << InsertionScenario::INSERT_ELEMENT_BEFORE << size_t{1} << size_t{10} << std::vector<size_t>{6, 10, 2, 5, 4};
    QTest::newRow("Insert before: 3") << std::vector<size_t>{6, 2, 5, 4} << InsertionScenario::INSERT_ELEMENT_BEFORE << size_t{3} << size_t{10} << std::vector<size_t>{6, 2, 5, 10, 4};
    QTest::newRow("Insert before: 4") << std::vector<size_t>{6, 2, 5, 4} << InsertionScenario::INSERT_ELEMENT_BEFORE << size_t{4} << size_t{10} << std::vector<size_t>{6, 2, 5, 4, 10};
    QTest::newRow("Insert before: 5") << std::vector<size_t>{} << InsertionScenario::INSERT_ELEMENT_BEFORE << size_t{0} << size_t{10} << std::vector<size_t>{10};
    QTest::newRow("Insert after: 1") << std::vector<size_t>{6, 2, 5, 4} << InsertionScenario::INSERT_ELEMENT_AFTER << size_t{0} << size_t{10} << std::vector<size_t>{6, 10, 2, 5, 4};
    QTest::newRow("Insert after: 2") << std::vector<size_t>{6, 2, 5, 4} << InsertionScenario::INSERT_ELEMENT_AFTER << size_t{1} << size_t{10} << std::vector<size_t>{6, 2, 10, 5, 4};
    QTest::newRow("Insert after: 3") << std::vector<size_t>{6, 2, 5, 4} << InsertionScenario::INSERT_ELEMENT_AFTER << size_t{3} << size_t{10} << std::vector<size_t>{6, 2, 5, 4, 10};
    QTest::newRow("Insert after: 4") << std::vector<size_t>{} << InsertionScenario::INSERT_ELEMENT_AFTER << size_t{0} << size_t{10} << std::vector<size_t>{10};
}

void LinkedListTests::testRemoveFirstOrLastElement_data()
{
    QTest::addColumn<std::vector<size_t>>("priorities");
    QTest::addColumn<RemovalScenario>("removalScenario");
    QTest::addColumn<Result>("expectedResult");
    QTest::addColumn<size_t>("removedElementPriority");
    QTest::addColumn<std::vector<size_t>>("expectedPriorities");

    QTest::newRow("Remove first: 1") << std::vector<size_t>{6, 2, 5, 4, 3, 1, 2, 9, 8} << RemovalScenario::REMOVE_FIRST_ELEMENT << Result::SUCCESS << size_t{6} << std::vector<size_t>{2, 5, 4, 3, 1, 2, 9, 8};
    QTest::newRow("Remove first: 2") << std::vector<size_t>{6, 2} << RemovalScenario::REMOVE_FIRST_ELEMENT << Result::SUCCESS << size_t{6} << std::vector<size_t>{2};
    QTest::newRow("Remove first: 3") << std::vector<size_t>{6} << RemovalScenario::REMOVE_FIRST_ELEMENT << Result::SUCCESS << size_t{6} << std::vector<size_t>{};
    QTest::newRow("Remove first: 4") << std::vector<size_t>{} << RemovalScenario::REMOVE_FIRST_ELEMENT << Result::FAIL << size_t{0} << std::vector<size_t>{};
    QTest::newRow("Remove last: 1") << std::vector<size_t>{6, 2, 5, 4, 3, 1, 2, 9, 8} << RemovalScenario::REMOVE_LAST_ELEMENT << Result::SUCCESS << size_t{8} << std::vector<size_t>{6, 2, 5, 4, 3, 1, 2, 9};
    QTest::newRow("Remove last: 2") << std::vector<size_t>{6, 2} << RemovalScenario::REMOVE_LAST_ELEMENT << Result::SUCCESS << size_t{2} << std::vector<size_t>{6};
    QTest::newRow("Remove last: 3") << std::vector<size_t>{6} << RemovalScenario::REMOVE_LAST_ELEMENT << Result::SUCCESS << size_t{6} << std::vector<size_t>{};
    QTest::newRow("Remove last: 4") << std::vector<size_t>{} << RemovalScenario::REMOVE_LAST_ELEMENT << Result::FAIL << size_t{0} << std::vector<size_t>{};
}

void LinkedListTests::testRemoveArbitraryElement_data()
{
    QTest::addColumn<std::vector<size_t>>("priorities");
    QTest::addColumn<RemovalScenario>("removalScenario");
    QTest::addColumn<size_t>("iteratorIncrementationsCount");
    QTest::addColumn<Result>("expectedResult");
    QTest::addColumn<size_t>("removedElementPriority");
    QTest::addColumn<std::vector<size_t>>("expectedPriorities");

    QTest::newRow("Remove previous: 1") << std::vector<size_t>{6, 2, 5, 4} << RemovalScenario::REMOVE_ELEMENT_BEFORE << size_t{0} << Result::FAIL << size_t{0} << std::vector<size_t>{6, 2, 5, 4};
    QTest::newRow("Remove previous: 2") << std::vector<size_t>{6, 2, 5, 4} << RemovalScenario::REMOVE_ELEMENT_BEFORE << size_t{1} << Result::SUCCESS << size_t{6} << std::vector<size_t>{2, 5, 4};
    QTest::newRow("Remove previous: 3") << std::vector<size_t>{6, 2, 5, 4} << RemovalScenario::REMOVE_ELEMENT_BEFORE << size_t{2} << Result::SUCCESS << size_t{2} << std::vector<size_t>{6, 5, 4};
    QTest::newRow("Remove previous: 4") << std::vector<size_t>{6, 2, 5, 4} << RemovalScenario::REMOVE_ELEMENT_BEFORE << size_t{3} << Result::SUCCESS << size_t{5} << std::vector<size_t>{6, 2, 4};
    QTest::newRow("Remove previous: 5") << std::vector<size_t>{6, 2, 5, 4} << RemovalScenario::REMOVE_ELEMENT_BEFORE << size_t{4} << Result::SUCCESS << size_t{4} << std::vector<size_t>{6, 2, 5};
    QTest::newRow("Remove previous: 6") << std::vector<size_t>{5} << RemovalScenario::REMOVE_ELEMENT_BEFORE << size_t{0} << Result::FAIL << size_t{0} << std::vector<size_t>{5};
    QTest::newRow("Remove previous: 7") << std::vector<size_t>{5} << RemovalScenario::REMOVE_ELEMENT_BEFORE << size_t{1} << Result::SUCCESS << size_t{5} << std::vector<size_t>{};
    QTest::newRow("Remove previous: 8") << std::vector<size_t>{} << RemovalScenario::REMOVE_ELEMENT_BEFORE << size_t{0} << Result::FAIL << size_t{0} << std::vector<size_t>{};
    QTest::newRow("Remove next: 1") << std::vector<size_t>{6, 2, 5, 4} << RemovalScenario::REMOVE_ELEMENT_AFTER << size_t{0} << Result::SUCCESS << size_t{2} << std::vector<size_t>{6, 5, 4};
    QTest::newRow("Remove next: 2") << std::vector<size_t>{6, 2, 5, 4} << RemovalScenario::REMOVE_ELEMENT_AFTER << size_t{1} << Result::SUCCESS << size_t{5} << std::vector<size_t>{6, 2, 4};
    QTest::newRow("Remove next: 3") << std::vector<size_t>{6, 2, 5, 4} << RemovalScenario::REMOVE_ELEMENT_AFTER << size_t{2} << Result::SUCCESS << size_t{4} << std::vector<size_t>{6, 2, 5};
    QTest::newRow("Remove next: 4") << std::vector<size_t>{6, 2, 5, 4} << RemovalScenario::REMOVE_ELEMENT_AFTER << size_t{3} << Result::FAIL << size_t{0} << std::vector<size_t>{6, 2, 5, 4};
    QTest::newRow("Remove next: 5") << std::vector<size_t>{5} << RemovalScenario::REMOVE_ELEMENT_AFTER << size_t{0} << Result::FAIL << size_t{0} << std::vector<size_t>{5};
    QTest::newRow("Remove next: 6") << std::vector<size_t>{} << RemovalScenario::REMOVE_ELEMENT_AFTER << size_t{0} << Result::FAIL << size_t{0} << std::vector<size_t>{};
    QTest::newRow("Remove current: 1") << std::vector<size_t>{6, 2, 5} << RemovalScenario::REMOVE_CURRENT_ELEMENT << size_t{0} << Result::SUCCESS << size_t{6} << std::vector<size_t>{2, 5};
    QTest::newRow("Remove current: 2") << std::vector<size_t>{6, 2, 5} << RemovalScenario::REMOVE_CURRENT_ELEMENT << size_t{1} << Result::SUCCESS << size_t{2} << std::vector<size_t>{6, 5};
    QTest::newRow("Remove current: 3") << std::vector<size_t>{6, 2, 5} << RemovalScenario::REMOVE_CURRENT_ELEMENT << size_t{2} << Result::SUCCESS << size_t{5} << std::vector<size_t>{6, 2};
    QTest::newRow("Remove current: 4") << std::vector<size_t>{6, 2, 5} << RemovalScenario::REMOVE_CURRENT_ELEMENT << size_t{3} << Result::FAIL << size_t{0} << std::vector<size_t>{6, 2, 5};
    QTest::newRow("Remove current: 5") << std::vector<size_t>{5} << RemovalScenario::REMOVE_CURRENT_ELEMENT << size_t{0} << Result::SUCCESS << size_t{5} << std::vector<size_t>{};
    QTest::newRow("Remove current: 6") << std::vector<size_t>{5} << RemovalScenario::REMOVE_CURRENT_ELEMENT << size_t{1} << Result::FAIL << size_t{0} << std::vector<size_t>{5};
    QTest::newRow("Remove current: 7") << std::vector<size_t>{} << RemovalScenario::REMOVE_CURRENT_ELEMENT << size_t{0} << Result::FAIL << size_t{0} << std::vector<size_t>{};
}

void LinkedListTests::testReverseList_data()
{
    QTest::addColumn<std::vector<size_t>>("priorities");
    QTest::addColumn<std::vector<size_t>>("expectedPriorities");

    QTest::newRow("1") << std::vector<size_t>{6, 2, 5, 4} << std::vector<size_t>{4, 5, 2, 6};
    QTest::newRow("2") << std::vector<size_t>{6, 2} << std::vector<size_t>{2, 6};
    QTest::newRow("3") << std::vector<size_t>{2} << std::vector<size_t>{2};
    QTest::newRow("4") << std::vector<size_t>{} << std::vector<size_t>{};
}

void LinkedListTests::testBatchReverseList_data()
{
    QTest::addColumn<std::vector<size_t>>("priorities");
    QTest::addColumn<size_t>("batchSize");
    QTest::addColumn<Result>("expectedResult");
    QTest::addColumn<std::vector<size_t>>("expectedPriorities");
    QTest::addColumn<size_t>("lastReversedElementPriority");

    QTest::newRow("1") << std::vector<size_t>{6, 2, 5, 4} << size_t{0} << Result::FAIL << std::vector<size_t>{6, 2, 5, 4} << size_t{0};
    QTest::newRow("2") << std::vector<size_t>{6, 2, 5, 4} << size_t{1} << Result::FAIL << std::vector<size_t>{6, 2, 5, 4} << size_t{0};
    QTest::newRow("3") << std::vector<size_t>{6, 2, 5, 4} << size_t{2} << Result::SUCCESS << std::vector<size_t>{2, 6, 4, 5} << size_t{5};
    QTest::newRow("4") << std::vector<size_t>{6, 2, 5, 4} << size_t{3} << Result::SUCCESS << std::vector<size_t>{5, 2, 6, 4} << size_t{6};
    QTest::newRow("5") << std::vector<size_t>{6, 2, 5, 4} << size_t{4} << Result::SUCCESS << std::vector<size_t>{4, 5, 2, 6} << size_t{6};
    QTest::newRow("6") << std::vector<size_t>{6, 2, 5, 4} << size_t{5} << Result::FAIL << std::vector<size_t>{6, 2, 5, 4} << size_t{0};
    QTest::newRow("7") << std::vector<size_t>{6, 2, 5, 4, 1} << size_t{2} << Result::SUCCESS << std::vector<size_t>{2, 6, 4, 5, 1} << size_t{5};
    QTest::newRow("8") << std::vector<size_t>{6, 2, 5, 4, 1} << size_t{3} << Result::SUCCESS << std::vector<size_t>{5, 2, 6, 4, 1} << size_t{6};
    QTest::newRow("9") << std::vector<size_t>{6, 2, 5, 4, 1, 2, 7} << size_t{2} << Result::SUCCESS << std::vector<size_t>{2, 6, 4, 5, 2, 1, 7} << size_t{1};
    QTest::newRow("10") << std::vector<size_t>{6, 2, 5, 4, 1, 2, 7} << size_t{3} << Result::SUCCESS << std::vector<size_t>{5, 2, 6, 2, 1, 4, 7} << size_t{4};
    QTest::newRow("11") << std::vector<size_t>{6, 2, 5, 4, 1, 2, 7, 8} << size_t{2} << Result::SUCCESS << std::vector<size_t>{2, 6, 4, 5, 2, 1, 8, 7} << size_t{7};
    QTest::newRow("12") << std::vector<size_t>{6, 2, 5, 4, 1, 2, 7, 8} << size_t{3} << Result::SUCCESS << std::vector<size_t>{5, 2, 6, 2, 1, 4, 7, 8} << size_t{4};
    QTest::newRow("13") << std::vector<size_t>{6, 2, 5, 4, 1, 2, 7, 8} << size_t{4} << Result::SUCCESS << std::vector<size_t>{4, 5, 2, 6, 8, 7, 2, 1} << size_t{1};
    QTest::newRow("14") << std::vector<size_t>{6, 2} << size_t{0} << Result::FAIL << std::vector<size_t>{6, 2} << size_t{0};
    QTest::newRow("15") << std::vector<size_t>{6, 2} << size_t{1} << Result::FAIL << std::vector<size_t>{6, 2} << size_t{0};
    QTest::newRow("16") << std::vector<size_t>{6, 2} << size_t{2} << Result::SUCCESS << std::vector<size_t>{2, 6} << size_t{6};
    QTest::newRow("17") << std::vector<size_t>{6, 2} << size_t{3} << Result::FAIL << std::vector<size_t>{6, 2} << size_t{0};
    QTest::newRow("18") << std::vector<size_t>{6} << size_t{0} << Result::FAIL << std::vector<size_t>{6} << size_t{0};
    QTest::newRow("19") << std::vector<size_t>{6} << size_t{1} << Result::FAIL << std::vector<size_t>{6} << size_t{0};
    QTest::newRow("20") << std::vector<size_t>{6} << size_t{2} << Result::FAIL << std::vector<size_t>{6} << size_t{0};
    QTest::newRow("21") << std::vector<size_t>{} << size_t{0} << Result::FAIL << std::vector<size_t>{} << size_t{0};
    QTest::newRow("22") << std::vector<size_t>{} << size_t{1} << Result::FAIL << std::vector<size_t>{} << size_t{0};
}

void LinkedListTests::initTestCase_data()
{
    m_Pool = createListElementsPool();
    QVERIFY(m_Pool);

    m_TotalAvailablePoolElementsCount = getAvailableElementsCount(m_Pool);

    ListElementsPool* p_NullPool{nullptr};

    QTest::addColumn<ListElementsPool*>("pool");

    QTest::newRow("allocation from pool") << m_Pool;
    QTest::newRow("no pool allocation") << p_NullPool;
}

void LinkedListTests::cleanupTestCase()
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
    QVERIFY(!m_List8);

    QVERIFY(m_ListsMarkedForDeletion.empty());
    QVERIFY(m_ListElementsMarkedForRelease.empty());
    QVERIFY(m_ListElementsMarkedForDeletion.empty());
}

void LinkedListTests::init()
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
    QVERIFY(!m_List8);

    QVERIFY(m_ListsMarkedForDeletion.empty());
    QVERIFY(m_ListElementsMarkedForRelease.empty());
    QVERIFY(m_ListElementsMarkedForDeletion.empty());
}

void LinkedListTests::cleanup()
{
    DELETE_LIST(m_List1, deleteObjectPayload);
    DELETE_LIST(m_List2, deleteObjectPayload);
    DELETE_LIST(m_List3, deleteObjectPayload);
    DELETE_LIST(m_List4, deleteObjectPayload);
    DELETE_LIST(m_List5, deleteObjectPayload);
    DELETE_LIST(m_List6, deleteObjectPayload);
    DELETE_LIST(m_List7, deleteObjectPayload);
    DELETE_LIST(m_List8, deleteObjectPayload);

    for (auto list : m_ListsMarkedForDeletion)
    {
        DELETE_LIST(list, deleteObjectPayload);
    }

    m_ListsMarkedForDeletion.clear();

    for (auto& element : m_ListElementsMarkedForRelease)
    {
        if (element)
        {
            releaseElement(element, m_Pool);
            element = nullptr;
        }
    }

    for (auto& element : m_ListElementsMarkedForDeletion)
    {
        FREE(element);
    }

    m_ListElementsMarkedForRelease.clear();
    m_ListElementsMarkedForDeletion.clear();

    QVERIFY(getAvailableElementsCount(m_Pool) == m_TotalAvailablePoolElementsCount);
}

void LinkedListTests::_markListForDeletion(List* list)
{
    if (list)
    {
        m_ListsMarkedForDeletion.push_back(list);
    }
}

void LinkedListTests::_markListElementForCleanup(ListElement* element, bool shouldRelease)
{
    if (element)
    {
        auto& freeElementsCollector = shouldRelease ? m_ListElementsMarkedForRelease : m_ListElementsMarkedForDeletion;
        freeElementsCollector.push_back(element);
    }
}

QTEST_APPLESS_MAIN(LinkedListTests)

#include "tst_linkedlisttests.moc"
