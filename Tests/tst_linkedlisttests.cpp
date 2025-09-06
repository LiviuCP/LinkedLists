#include <QTest>

#include <cstring>

#include "linkedlist.h"
#include "codeutils.h"
#include "testobjects.h"

class LinkedListTests : public QObject
{
    Q_OBJECT

public:
    explicit LinkedListTests();

private slots:
    void testListIsCorrectlyCreatedAndCleared();
    void testAppendElement();
    void testPrependElement();
    void testRemoveFirstElement();
    void testRemoveLastElement();
    void testInsertElementBefore();
    void testInsertElementAfter();
    void testMoveContentToList();
    void testCopyContentToList();
    void testRemoveElementBefore();
    void testRemoveElementAfter();
    void testRemoveCurrentElement();
    void testSwapElements();
    void testReverseList();
    void testBatchReverseList();
    void testIterators();
    void testAssignRemoveObject();
    void testIsElementContained();
    void testGetPreviousElement();
    void testGetFirstAndLastElement();
    void testMoveListToArray();
    void testMoveArrayToList();
    void testPrintListElementsToFile();

    void initTestCase_data();
    void cleanupTestCase();
    void init();
    void cleanup();

private:
    void _markListForDeletion(List* list);
    void _markListElementForCleanup(ListElement* element, bool shouldRelease);
    void _clearListElementsMarkedForCleanup();

    ListElementsPool* m_Pool;
    size_t m_TotalAvailablePoolElementsCount;

    ListElement** m_ListElementRefs; // used for storing multiple element addreses, to be cleaned up after each test run (elements to be cleaned up separately)

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
    , m_ListElementRefs{nullptr}
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
        List list{nullptr, nullptr};

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

void LinkedListTests::testAppendElement()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    const size_t prioritiesArray[9]{6, 2, 5, 4, 3, 1, 2, 9, 7};
    m_List1 = createListFromPrioritiesArray(prioritiesArray, 9, pool);
    createAndAppendToList(m_List1, 8);

    QVERIFY2(getListSize(m_List1) == 10 && getListElementAtIndex(m_List1, 0)->priority == 6 && getListElementAtIndex(m_List1, 9)->priority == 8, "Element has not been correctly created and appended");
    QVERIFY2(getFirstListElement(m_List1)->priority == 6 && getLastListElement(m_List1)->priority == 8, "First and last element are not correctly referenced");
    QVERIFY(getListElementAtIndex(m_List1, 9)->object.type == -1 && getListElementAtIndex(m_List1, 9)->object.payload == nullptr);
}

void LinkedListTests::testPrependElement()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    const size_t prioritiesArray[9]{6, 2, 5, 4, 3, 1, 2, 9, 7};
    m_List1 = createListFromPrioritiesArray(prioritiesArray, 9, pool);
    createAndPrependToList(m_List1, 8);

    QVERIFY2(getListSize(m_List1) == 10 && getListElementAtIndex(m_List1, 0)->priority == 8 && getListElementAtIndex(m_List1, 9)->priority == 7, "Element has not been correctly created and prepended");
    QVERIFY2(getFirstListElement(m_List1)->priority == 8 && getLastListElement(m_List1)->priority == 7, "First and last element are not correctly referenced");
    QVERIFY(getListElementAtIndex(m_List1, 0)->object.type == -1 && getListElementAtIndex(m_List1, 0)->object.payload == nullptr);
}

void LinkedListTests::testRemoveFirstElement()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    const size_t prioritiesArray[9]{6, 2, 5, 4, 3, 1, 2, 9, 8};

    m_List1 = createListFromPrioritiesArray(prioritiesArray, 9, pool);
    ListElement* firstElement = removeFirstListElement(m_List1);

    QVERIFY2(getListSize(m_List1) == 8 && getListElementAtIndex(m_List1, 0)->priority == 2 && getListElementAtIndex(m_List1, 7)->priority == 8, "Element has not been correctly created and appended");
    QVERIFY2(getFirstListElement(m_List1)->priority == 2 && getLastListElement(m_List1)->priority == 8, "First and last element are not correctly referenced");
    QVERIFY(firstElement->next == nullptr && firstElement->priority == 6 && firstElement->object.type == -1 && firstElement->object.payload == nullptr);

    _markListElementForCleanup(firstElement, pool != nullptr);
}

void LinkedListTests::testRemoveLastElement()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    const size_t prioritiesArray[9]{6, 2, 5, 4, 3, 1, 2, 9, 8};

    m_List1 = createListFromPrioritiesArray(prioritiesArray, 9, pool);
    ListElement* lastElement = removeLastListElement(m_List1);

    QVERIFY2(getListSize(m_List1) == 8 && getListElementAtIndex(m_List1, 0)->priority == 6 && getListElementAtIndex(m_List1, 7)->priority == 9, "Element has not been correctly created and appended");
    QVERIFY2(getFirstListElement(m_List1)->priority == 6 && getLastListElement(m_List1)->priority == 9, "First and last element are not correctly referenced");
    QVERIFY(lastElement->next == nullptr && lastElement->priority == 8 && lastElement->object.type == -1 && lastElement->object.payload == nullptr);

    _markListElementForCleanup(lastElement, pool != nullptr);
}

void LinkedListTests::testInsertElementBefore()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    const size_t prioritiesArray[4]{6, 2, 5, 4};

    {
        m_List1 = createListFromPrioritiesArray(prioritiesArray, 4, pool);

        ListIterator it = lbegin(m_List1);
        createAndInsertBefore(it, 10);

        QVERIFY2(getListSize(m_List1) == 5 &&
                 getListElementAtIndex(m_List1, 0)->priority == 10 &&
                 getListElementAtIndex(m_List1, 1)->priority == 6 &&
                 getListElementAtIndex(m_List1, 2)->priority == 2 &&
                 getListElementAtIndex(m_List1, 3)->priority == 5 &&
                 getListElementAtIndex(m_List1, 4)->priority == 4,   "The previous element has not been correctly inserted");
        QVERIFY2(getFirstListElement(m_List1)->priority == 10 && getLastListElement(m_List1)->priority == 4, "First and last element are not correctly referenced");
        QVERIFY(getListElementAtIndex(m_List1, 0)->object.type == -1 && getListElementAtIndex(m_List1, 0)->object.payload == nullptr);
    }

    {
        m_List2 = createListFromPrioritiesArray(prioritiesArray, 4, pool);

        ListIterator it = lbegin(m_List2);
        lnext(&it);
        createAndInsertBefore(it, 10);

        QVERIFY2(getListSize(m_List2) == 5 &&
                 getListElementAtIndex(m_List2, 0)->priority == 6 &&
                 getListElementAtIndex(m_List2, 1)->priority == 10 &&
                 getListElementAtIndex(m_List2, 2)->priority == 2 &&
                 getListElementAtIndex(m_List2, 3)->priority == 5 &&
                 getListElementAtIndex(m_List2, 4)->priority == 4,   "The previous element has not been correctly inserted");
        QVERIFY2(getFirstListElement(m_List2)->priority == 6 && getLastListElement(m_List2)->priority == 4, "First and last element are not correctly referenced");
        QVERIFY(getListElementAtIndex(m_List2, 1)->object.type == -1 && getListElementAtIndex(m_List2, 1)->object.payload == nullptr);
    }

    {
        m_List3 = createListFromPrioritiesArray(prioritiesArray, 4, pool);

        ListIterator it = lbegin(m_List3);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        createAndInsertBefore(it, 10);

        QVERIFY2(getListSize(m_List3) == 5 &&
                 getListElementAtIndex(m_List3, 0)->priority == 6 &&
                 getListElementAtIndex(m_List3, 1)->priority == 2 &&
                 getListElementAtIndex(m_List3, 2)->priority == 5 &&
                 getListElementAtIndex(m_List3, 3)->priority == 10 &&
                 getListElementAtIndex(m_List3, 4)->priority == 4,   "The previous element has not been correctly inserted");
        QVERIFY2(getFirstListElement(m_List3)->priority == 6 && getLastListElement(m_List3)->priority == 4, "First and last element are not correctly referenced");
        QVERIFY(getListElementAtIndex(m_List3, 3)->object.type == -1 && getListElementAtIndex(m_List3, 3)->object.payload == nullptr);
    }

    {
        m_List4 = createListFromPrioritiesArray(prioritiesArray, 4, pool);

        ListIterator it = lbegin(m_List4);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        createAndInsertBefore(it, 10);

        QVERIFY2(getListSize(m_List4) == 5 &&
                 getListElementAtIndex(m_List4, 0)->priority == 6 &&
                 getListElementAtIndex(m_List4, 1)->priority == 2 &&
                 getListElementAtIndex(m_List4, 2)->priority == 5 &&
                 getListElementAtIndex(m_List4, 3)->priority == 4 &&
                 getListElementAtIndex(m_List4, 4)->priority == 10,   "The previous element has not been correctly inserted");
        QVERIFY2(getFirstListElement(m_List4)->priority == 6 && getLastListElement(m_List4)->priority == 10, "First and last element are not correctly referenced");
        QVERIFY(getListElementAtIndex(m_List4, 4)->object.type == -1 && getListElementAtIndex(m_List4, 4)->object.payload == nullptr);
    }

    {
        m_List5 = createEmptyList(pool);
        ListIterator it = lbegin(m_List5);
        createAndInsertBefore(it, 10);

        QVERIFY2(getListSize(m_List5) == 1 && getListElementAtIndex(m_List5, 0)->priority == 10, "The element has not been correctly inserted into an empty list");
        QVERIFY2(getFirstListElement(m_List5)->priority == 10 && getLastListElement(m_List5)->priority == 10, "First and last element are not correctly referenced");
        QVERIFY(getListElementAtIndex(m_List5, 0)->object.type == -1 && getListElementAtIndex(m_List5, 0)->object.payload == nullptr);
    }
}

void LinkedListTests::testInsertElementAfter()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    const size_t prioritiesArray[4]{6, 2, 5, 4};

    {
        m_List1 = createListFromPrioritiesArray(prioritiesArray, 4, pool);

        ListIterator it = lbegin(m_List1);
        createAndInsertAfter(it, 10);

        QVERIFY2(getListSize(m_List1) == 5 &&
                 getListElementAtIndex(m_List1, 0)->priority == 6 &&
                 getListElementAtIndex(m_List1, 1)->priority == 10 &&
                 getListElementAtIndex(m_List1, 2)->priority == 2 &&
                 getListElementAtIndex(m_List1, 3)->priority == 5 &&
                 getListElementAtIndex(m_List1, 4)->priority == 4,   "The next element has not been correctly inserted");
        QVERIFY2(getFirstListElement(m_List1)->priority == 6 && getLastListElement(m_List1)->priority == 4, "First and last element are not correctly referenced");
        QVERIFY(getListElementAtIndex(m_List1, 1)->object.type == -1 && getListElementAtIndex(m_List1, 1)->object.payload == nullptr);
    }

    {
        m_List2 = createListFromPrioritiesArray(prioritiesArray, 4, pool);

        ListIterator it = lbegin(m_List2);
        lnext(&it);
        createAndInsertAfter(it, 10);

        QVERIFY2(getListSize(m_List2) == 5 &&
                 getListElementAtIndex(m_List2, 0)->priority == 6 &&
                 getListElementAtIndex(m_List2, 1)->priority == 2 &&
                 getListElementAtIndex(m_List2, 2)->priority == 10 &&
                 getListElementAtIndex(m_List2, 3)->priority == 5 &&
                 getListElementAtIndex(m_List2, 4)->priority == 4,   "The next element has not been correctly inserted");
        QVERIFY2(getFirstListElement(m_List2)->priority == 6 && getLastListElement(m_List2)->priority == 4, "First and last element are not correctly referenced");
        QVERIFY(getListElementAtIndex(m_List2, 2)->object.type == -1 && getListElementAtIndex(m_List2, 2)->object.payload == nullptr);
    }

    {
        m_List3 = createListFromPrioritiesArray(prioritiesArray, 4, pool);

        ListIterator it = lbegin(m_List3);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        createAndInsertAfter(it, 10);

        QVERIFY2(getListSize(m_List3) == 5 &&
                 getListElementAtIndex(m_List3, 0)->priority == 6 &&
                 getListElementAtIndex(m_List3, 1)->priority == 2 &&
                 getListElementAtIndex(m_List3, 2)->priority == 5 &&
                 getListElementAtIndex(m_List3, 3)->priority == 4 &&
                 getListElementAtIndex(m_List3, 4)->priority == 10,   "The next element has not been correctly inserted");
        QVERIFY2(getFirstListElement(m_List3)->priority == 6 && getLastListElement(m_List3)->priority == 10, "First and last element are not correctly referenced");
        QVERIFY(getListElementAtIndex(m_List3, 4)->object.type == -1 && getListElementAtIndex(m_List3, 4)->object.payload == nullptr);
    }

    {
        m_List4 = createEmptyList(pool);
        ListIterator it = lbegin(m_List4);
        createAndInsertAfter(it, 10);

        QVERIFY2(getListSize(m_List4) == 1 && getListElementAtIndex(m_List4, 0)->priority == 10, "The element has not been correctly inserted into an empty list");
        QVERIFY2(getFirstListElement(m_List4)->priority == 10 && getLastListElement(m_List4)->priority == 10, "First and last element are not correctly referenced");
        QVERIFY(getListElementAtIndex(m_List4, 0)->object.type == -1 && getListElementAtIndex(m_List4, 0)->object.payload == nullptr);
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

void LinkedListTests::testRemoveElementBefore()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    const size_t prioritiesArray[4]{6, 2, 5, 4};

    {
        m_List1 = createListFromPrioritiesArray(prioritiesArray, 4, pool);

        ListIterator it = lbegin(m_List1);
        ListElement* removedElement = removePreviousListElement(it);

        QVERIFY2(getListSize(m_List1) == 4 &&
                 removedElement == nullptr &&
                 getListElementAtIndex(m_List1, 0)->priority == 6 &&
                 getListElementAtIndex(m_List1, 1)->priority == 2 &&
                 getListElementAtIndex(m_List1, 2)->priority == 5 &&
                 getListElementAtIndex(m_List1, 3)->priority == 4,   "The previous element removal does not work correctly");
        QVERIFY2(getFirstListElement(m_List1)->priority == 6 && getLastListElement(m_List1)->priority == 4, "First and last element of the list are not correctly referenced");
    }

    {
        m_List2 = createListFromPrioritiesArray(prioritiesArray, 4, pool);

        ListIterator it = lbegin(m_List2);
        lnext(&it);
        ListElement* removedElement = removePreviousListElement(it);

        QVERIFY2(getListSize(m_List2) == 3 &&
                 removedElement->priority == 6 &&
                 getListElementAtIndex(m_List2, 0)->priority == 2 &&
                 getListElementAtIndex(m_List2, 1)->priority == 5 &&
                 getListElementAtIndex(m_List2, 2)->priority == 4,   "The previous element has not been correctly removed");
        QVERIFY2(getFirstListElement(m_List2)->priority == 2 && getLastListElement(m_List2)->priority == 4, "First and last element of the list are not correctly referenced");
        QVERIFY(removedElement->object.type == -1 && removedElement->object.payload == nullptr);

        _markListElementForCleanup(removedElement, pool != nullptr);
    }

    {
        m_List3 = createListFromPrioritiesArray(prioritiesArray, 4, pool);

        ListIterator it = lbegin(m_List3);
        lnext(&it);
        lnext(&it);
        ListElement* removedElement = removePreviousListElement(it);

        QVERIFY2(getListSize(m_List3) == 3 &&
                 removedElement->priority == 2 &&
                 getListElementAtIndex(m_List3, 0)->priority == 6 &&
                 getListElementAtIndex(m_List3, 1)->priority == 5 &&
                 getListElementAtIndex(m_List3, 2)->priority == 4,   "The previous element has not been correctly removed");
        QVERIFY2(getFirstListElement(m_List3)->priority == 6 && getLastListElement(m_List3)->priority == 4, "First and last element of the list are not correctly referenced");
        QVERIFY(removedElement->object.type == -1 && removedElement->object.payload == nullptr);

        _markListElementForCleanup(removedElement, pool != nullptr);
    }

    {
        m_List4 = createListFromPrioritiesArray(prioritiesArray, 4, pool);

        ListIterator it = lbegin(m_List4);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        ListElement* removedElement = removePreviousListElement(it);

        QVERIFY2(getListSize(m_List4) == 3 &&
                 removedElement->priority == 5 &&
                 getListElementAtIndex(m_List4, 0)->priority == 6 &&
                 getListElementAtIndex(m_List4, 1)->priority == 2 &&
                 getListElementAtIndex(m_List4, 2)->priority == 4,   "The previous element has not been correctly removed");
        QVERIFY2(getFirstListElement(m_List4)->priority == 6 && getLastListElement(m_List4)->priority == 4, "First and last element of the list are not correctly referenced");
        QVERIFY(removedElement->object.type == -1 && removedElement->object.payload == nullptr);

        _markListElementForCleanup(removedElement, pool != nullptr);
    }

    {
        m_List5 = createListFromPrioritiesArray(prioritiesArray, 4, pool);

        ListIterator it = lbegin(m_List5);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        ListElement* removedElement = removePreviousListElement(it);

        QVERIFY2(getListSize(m_List5) == 3 &&
                 removedElement->priority == 4 &&
                 getListElementAtIndex(m_List5, 0)->priority == 6 &&
                 getListElementAtIndex(m_List5, 1)->priority == 2 &&
                 getListElementAtIndex(m_List5, 2)->priority == 5,   "The previous element has not been correctly removed");
        QVERIFY2(getFirstListElement(m_List5)->priority == 6 && getLastListElement(m_List5)->priority == 5, "First and last element of the list are not correctly referenced");
        QVERIFY(removedElement->object.type == -1 && removedElement->object.payload == nullptr);

        _markListElementForCleanup(removedElement, pool != nullptr);
    }

    {
        m_List6 = createEmptyList(pool);
        createAndAppendToList(m_List6, 5);
        ListIterator it = lbegin(m_List6);
        ListElement* removedElement = removePreviousListElement(it);

        QVERIFY2(getListSize(m_List6) == 1 && removedElement == nullptr && getListElementAtIndex(m_List6, 0)->priority == 5, "The previous element removal does not work correctly");

        lnext(&it);
        removedElement = removePreviousListElement(it);

        QVERIFY2(isEmptyList(m_List6) && removedElement->priority == 5, "The only existing list element has not been correctly removed");
        QVERIFY(removedElement->object.type == -1 && removedElement->object.payload == nullptr);

        _markListElementForCleanup(removedElement, pool != nullptr);
    }
}

void LinkedListTests::testRemoveElementAfter()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    const size_t prioritiesArray[4]{6, 2, 5, 4};

    {
        m_List1 = createListFromPrioritiesArray(prioritiesArray, 4, pool);

        ListIterator it = lbegin(m_List1);
        ListElement* removedElement = removeNextListElement(it);

        QVERIFY2(getListSize(m_List1) == 3 &&
                 removedElement->priority == 2 &&
                 getListElementAtIndex(m_List1, 0)->priority == 6 &&
                 getListElementAtIndex(m_List1, 1)->priority == 5 &&
                 getListElementAtIndex(m_List1, 2)->priority == 4,   "The next element has not been correctly removed");
        QVERIFY2(getFirstListElement(m_List1)->priority == 6 && getLastListElement(m_List1)->priority == 4, "First and last element of the list are not correctly referenced");
        QVERIFY(removedElement->object.type == -1 && removedElement->object.payload == nullptr);

        _markListElementForCleanup(removedElement, pool != nullptr);
    }

    {
        m_List2 = createListFromPrioritiesArray(prioritiesArray, 4, pool);

        ListIterator it = lbegin(m_List2);
        lnext(&it);
        ListElement* removedElement = removeNextListElement(it);

        QVERIFY2(getListSize(m_List2) == 3 &&
                 removedElement->priority == 5 &&
                 getListElementAtIndex(m_List2, 0)->priority == 6 &&
                 getListElementAtIndex(m_List2, 1)->priority == 2 &&
                 getListElementAtIndex(m_List2, 2)->priority == 4,   "The next element has not been correctly removed");
        QVERIFY2(getFirstListElement(m_List2)->priority == 6 && getLastListElement(m_List2)->priority == 4, "First and last element of the list are not correctly referenced");
        QVERIFY(removedElement->object.type == -1 && removedElement->object.payload == nullptr);

        _markListElementForCleanup(removedElement, pool != nullptr);
    }

    {
        m_List3 = createListFromPrioritiesArray(prioritiesArray, 4, pool);

        ListIterator it = lbegin(m_List3);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        ListElement* removedElement = removeNextListElement(it);

        QVERIFY2(getListSize(m_List3) == 4 &&
                 removedElement == nullptr &&
                 getListElementAtIndex(m_List3, 0)->priority == 6 &&
                 getListElementAtIndex(m_List3, 1)->priority == 2 &&
                 getListElementAtIndex(m_List3, 2)->priority == 5 &&
                 getListElementAtIndex(m_List3, 3)->priority == 4,   "The next element has not been correctly removed");
        QVERIFY2(getFirstListElement(m_List3)->priority == 6 && getLastListElement(m_List3)->priority == 4, "First and last element of the list are not correctly referenced");
    }

    {
        m_List4 = createEmptyList(pool);
        createAndAppendToList(m_List4, 5);
        ListIterator it = lbegin(m_List4);
        ListElement* removedElement = removeNextListElement(it);

        QVERIFY2(getListSize(m_List4) == 1 && removedElement == nullptr && getListElementAtIndex(m_List4, 0)->priority == 5, "The only existing list element has not been correctly removed");
        QVERIFY2(getFirstListElement(m_List4)->priority == 5 && getLastListElement(m_List4)->priority == 5, "First and last element of the list are not correctly referenced");
    }
}

void LinkedListTests::testRemoveCurrentElement()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    const size_t prioritiesArray[3]{6, 2, 5};

    {
        m_List1 = createListFromPrioritiesArray(prioritiesArray, 3, pool);

        ListIterator it = lbegin(m_List1);
        ListElement* removedElement = removeCurrentListElement(it);

        QVERIFY2(getListSize(m_List1) == 2 &&
                 removedElement->priority == 6 &&
                 getListElementAtIndex(m_List1, 0)->priority == 2 &&
                 getListElementAtIndex(m_List1, 1)->priority == 5,   "The current element has not been correctly removed");
        QVERIFY2(getFirstListElement(m_List1)->priority == 2 && getLastListElement(m_List1)->priority == 5, "First and last element of the list are not correctly referenced");
        QVERIFY(removedElement->object.type == -1 && removedElement->object.payload == nullptr);

        _markListElementForCleanup(removedElement, pool != nullptr);
    }

    {
        m_List2 = createListFromPrioritiesArray(prioritiesArray, 3, pool);

        ListIterator it = lbegin(m_List2);
        lnext(&it);
        ListElement* removedElement = removeCurrentListElement(it);

        QVERIFY2(getListSize(m_List2) == 2 &&
                 removedElement->priority == 2 &&
                 getListElementAtIndex(m_List2, 0)->priority == 6 &&
                 getListElementAtIndex(m_List2, 1)->priority == 5,   "The current element has not been correctly removed");
        QVERIFY2(getFirstListElement(m_List2)->priority == 6 && getLastListElement(m_List2)->priority == 5, "First and last element of the list are not correctly referenced");
        QVERIFY(removedElement->object.type == -1 && removedElement->object.payload == nullptr);

        _markListElementForCleanup(removedElement, pool != nullptr);
    }

    {
        m_List3 = createListFromPrioritiesArray(prioritiesArray, 3, pool);

        ListIterator it = lbegin(m_List3);
        lnext(&it);
        lnext(&it);
        ListElement* removedElement = removeCurrentListElement(it);

        QVERIFY2(getListSize(m_List3) == 2 &&
                 removedElement->priority == 5 &&
                 getListElementAtIndex(m_List3, 0)->priority == 6 &&
                 getListElementAtIndex(m_List3, 1)->priority == 2,   "The current element has not been correctly removed");
        QVERIFY2(getFirstListElement(m_List3)->priority == 6 && getLastListElement(m_List3)->priority == 2, "First and last element of the list are not correctly referenced");
        QVERIFY(removedElement->object.type == -1 && removedElement->object.payload == nullptr);

        _markListElementForCleanup(removedElement, pool != nullptr);
    }

    {
        m_List4 = createListFromPrioritiesArray(prioritiesArray, 3, pool);

        ListIterator it = lbegin(m_List4);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        ListElement* removedElement = removeCurrentListElement(it);

        QVERIFY2(getListSize(m_List4) == 3 &&
                 removedElement == nullptr,   "The current element removal does not work correctly");
        QVERIFY2(getFirstListElement(m_List4)->priority == 6 && getLastListElement(m_List4)->priority == 5, "First and last element of the list are not correctly referenced");
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

    const size_t firstPrioritiesArray[4]{6, 2, 5, 4};
    const size_t secondPrioritiesArray[2]{6, 2};

    {
        m_List1 = createListFromPrioritiesArray(firstPrioritiesArray, 4, pool);
        reverseList(m_List1);

        QVERIFY2(getListSize(m_List1) == 4 &&
                 getListElementAtIndex(m_List1, 0)->priority == 4 &&
                 getListElementAtIndex(m_List1, 1)->priority == 5 &&
                 getListElementAtIndex(m_List1, 2)->priority == 2 &&
                 getListElementAtIndex(m_List1, 3)->priority == 6,   "The list has not been correctly reversed");
        QVERIFY2(getFirstListElement(m_List1)->priority == 4 && getLastListElement(m_List1)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    {
        m_List2 = createListFromPrioritiesArray(secondPrioritiesArray, 2, pool);
        reverseList(m_List2);

        QVERIFY2(getListSize(m_List2) == 2 &&
                 getListElementAtIndex(m_List2, 0)->priority == 2 &&
                 getListElementAtIndex(m_List2, 1)->priority == 6,   "The list has not been correctly reversed");
        QVERIFY2(getFirstListElement(m_List2)->priority == 2 && getLastListElement(m_List2)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    {
        m_List3 = createEmptyList(pool);
        createAndAppendToList(m_List3, 2);
        reverseList(m_List3);

        QVERIFY2(getListSize(m_List3) == 1 && getListElementAtIndex(m_List3, 0)->priority == 2, "The list has not been correctly reversed");
        QVERIFY2(getFirstListElement(m_List3)->priority == 2 && getLastListElement(m_List3)->priority == 2, "First and last element of the list are not correctly referenced");
    }

    {
        m_List4 = createEmptyList(pool);
        reverseList(m_List4);

        QVERIFY2(isEmptyList(m_List4), "The list has not been correctly reversed");
    }
}

void LinkedListTests::testBatchReverseList()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    const size_t prioritiesArray[8]{6, 2, 5, 4, 1, 2, 7, 8};

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4, pool);
        ListElement* lastReversedElement = batchReverseList(list, 0);

        _markListForDeletion(list); // in case any QVERIFY fails no memory leak will occur (the list is deleted by cleanup() method)

        QVERIFY(!lastReversedElement);

        QVERIFY2(getListSize(list) == 4 &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 2 &&
                 getListElementAtIndex(list, 2)->priority == 5 &&
                 getListElementAtIndex(list, 3)->priority == 4,   "The list has not been correctly batch reversed");

        QVERIFY2(getFirstListElement(list)->priority == 6 && getLastListElement(list)->priority == 4, "First and last element of the list are not correctly referenced");
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4, pool);
        ListElement* lastReversedElement = batchReverseList(list, 1);

        _markListForDeletion(list);

        QVERIFY(!lastReversedElement);

        QVERIFY2(getListSize(list) == 4 &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 2 &&
                 getListElementAtIndex(list, 2)->priority == 5 &&
                 getListElementAtIndex(list, 3)->priority == 4,   "The list has not been correctly batch reversed");

        QVERIFY2(getFirstListElement(list)->priority == 6 && getLastListElement(list)->priority == 4, "First and last element of the list are not correctly referenced");
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4, pool);
        ListElement* lastReversedElement = batchReverseList(list, 2);

        _markListForDeletion(list);

        QVERIFY(lastReversedElement && lastReversedElement->priority == 5);

        QVERIFY2(getListSize(list) == 4 &&
                 getListElementAtIndex(list, 0)->priority == 2 &&
                 getListElementAtIndex(list, 1)->priority == 6 &&
                 getListElementAtIndex(list, 2)->priority == 4 &&
                 getListElementAtIndex(list, 3)->priority == 5,   "The list has not been correctly batch reversed");

        QVERIFY2(getFirstListElement(list)->priority == 2 && getLastListElement(list)->priority == 5, "First and last element of the list are not correctly referenced");
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4, pool);
        ListElement* lastReversedElement = batchReverseList(list, 3);

        _markListForDeletion(list);

        QVERIFY(lastReversedElement && lastReversedElement->priority == 6);

        QVERIFY2(getListSize(list) == 4 &&
                 getListElementAtIndex(list, 0)->priority == 5 &&
                 getListElementAtIndex(list, 1)->priority == 2 &&
                 getListElementAtIndex(list, 2)->priority == 6 &&
                 getListElementAtIndex(list, 3)->priority == 4,   "The list has not been correctly batch reversed");

        QVERIFY2(getFirstListElement(list)->priority == 5 && getLastListElement(list)->priority == 4, "First and last element of the list are not correctly referenced");
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4, pool);
        ListElement* lastReversedElement = batchReverseList(list, 4);

        _markListForDeletion(list);

        QVERIFY(lastReversedElement && lastReversedElement->priority == 6);

        QVERIFY2(getListSize(list) == 4 &&
                 getListElementAtIndex(list, 0)->priority == 4 &&
                 getListElementAtIndex(list, 1)->priority == 5 &&
                 getListElementAtIndex(list, 2)->priority == 2 &&
                 getListElementAtIndex(list, 3)->priority == 6,   "The list has not been correctly batch reversed");

        QVERIFY2(getFirstListElement(list)->priority == 4 && getLastListElement(list)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4, pool);
        ListElement* lastReversedElement = batchReverseList(list, 5);

        _markListForDeletion(list);

        QVERIFY(!lastReversedElement);

        QVERIFY2(getListSize(list) == 4 &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 2 &&
                 getListElementAtIndex(list, 2)->priority == 5 &&
                 getListElementAtIndex(list, 3)->priority == 4,   "The list has not been correctly batch reversed");

        QVERIFY2(getFirstListElement(list)->priority == 6 && getLastListElement(list)->priority == 4, "First and last element of the list are not correctly referenced");
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 5, pool);
        ListElement* lastReversedElement = batchReverseList(list, 2);

        _markListForDeletion(list);

        QVERIFY(lastReversedElement && lastReversedElement->priority == 5);

        QVERIFY2(getListSize(list) == 5 &&
                 getListElementAtIndex(list, 0)->priority == 2 &&
                 getListElementAtIndex(list, 1)->priority == 6 &&
                 getListElementAtIndex(list, 2)->priority == 4 &&
                 getListElementAtIndex(list, 3)->priority == 5 &&
                 getListElementAtIndex(list, 4)->priority == 1,   "The list has not been correctly batch reversed");

        QVERIFY2(getFirstListElement(list)->priority == 2 && getLastListElement(list)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 5, pool);
        ListElement* lastReversedElement = batchReverseList(list, 3);

        _markListForDeletion(list);

        QVERIFY(lastReversedElement && lastReversedElement->priority == 6);

        QVERIFY2(getListSize(list) == 5 &&
                 getListElementAtIndex(list, 0)->priority == 5 &&
                 getListElementAtIndex(list, 1)->priority == 2 &&
                 getListElementAtIndex(list, 2)->priority == 6 &&
                 getListElementAtIndex(list, 3)->priority == 4 &&
                 getListElementAtIndex(list, 4)->priority == 1,   "The list has not been correctly batch reversed");

        QVERIFY2(getFirstListElement(list)->priority == 5 && getLastListElement(list)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 7, pool);
        ListElement* lastReversedElement = batchReverseList(list, 2);

        _markListForDeletion(list);

        QVERIFY(lastReversedElement && lastReversedElement->priority == 1);

        QVERIFY2(getListSize(list) == 7 &&
                 getListElementAtIndex(list, 0)->priority == 2 &&
                 getListElementAtIndex(list, 1)->priority == 6 &&
                 getListElementAtIndex(list, 2)->priority == 4 &&
                 getListElementAtIndex(list, 3)->priority == 5 &&
                 getListElementAtIndex(list, 4)->priority == 2 &&
                 getListElementAtIndex(list, 5)->priority == 1 &&
                 getListElementAtIndex(list, 6)->priority == 7,   "The list has not been correctly batch reversed");

        QVERIFY2(getFirstListElement(list)->priority == 2 && getLastListElement(list)->priority == 7, "First and last element of the list are not correctly referenced");
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 7, pool);
        ListElement* lastReversedElement = batchReverseList(list, 3);

        _markListForDeletion(list);

        QVERIFY(lastReversedElement && lastReversedElement->priority == 4);

        QVERIFY2(getListSize(list) == 7 &&
                 getListElementAtIndex(list, 0)->priority == 5 &&
                 getListElementAtIndex(list, 1)->priority == 2 &&
                 getListElementAtIndex(list, 2)->priority == 6 &&
                 getListElementAtIndex(list, 3)->priority == 2 &&
                 getListElementAtIndex(list, 4)->priority == 1 &&
                 getListElementAtIndex(list, 5)->priority == 4 &&
                 getListElementAtIndex(list, 6)->priority == 7,   "The list has not been correctly batch reversed");

        QVERIFY2(getFirstListElement(list)->priority == 5 && getLastListElement(list)->priority == 7, "First and last element of the list are not correctly referenced");
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 8, pool);
        ListElement* lastReversedElement = batchReverseList(list, 2);

        _markListForDeletion(list);

        QVERIFY(lastReversedElement && lastReversedElement->priority == 7);

        QVERIFY2(getListSize(list) == 8 &&
                 getListElementAtIndex(list, 0)->priority == 2 &&
                 getListElementAtIndex(list, 1)->priority == 6 &&
                 getListElementAtIndex(list, 2)->priority == 4 &&
                 getListElementAtIndex(list, 3)->priority == 5 &&
                 getListElementAtIndex(list, 4)->priority == 2 &&
                 getListElementAtIndex(list, 5)->priority == 1 &&
                 getListElementAtIndex(list, 6)->priority == 8 &&
                 getListElementAtIndex(list, 7)->priority == 7,   "The list has not been correctly batch reversed");

        QVERIFY2(getFirstListElement(list)->priority == 2 && getLastListElement(list)->priority == 7, "First and last element of the list are not correctly referenced");
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 8, pool);
        ListElement* lastReversedElement = batchReverseList(list, 3);

        _markListForDeletion(list);

        QVERIFY(lastReversedElement && lastReversedElement->priority == 4);

        QVERIFY2(getListSize(list) == 8 &&
                 getListElementAtIndex(list, 0)->priority == 5 &&
                 getListElementAtIndex(list, 1)->priority == 2 &&
                 getListElementAtIndex(list, 2)->priority == 6 &&
                 getListElementAtIndex(list, 3)->priority == 2 &&
                 getListElementAtIndex(list, 4)->priority == 1 &&
                 getListElementAtIndex(list, 5)->priority == 4 &&
                 getListElementAtIndex(list, 6)->priority == 7 &&
                 getListElementAtIndex(list, 7)->priority == 8,   "The list has not been correctly batch reversed");

        QVERIFY2(getFirstListElement(list)->priority == 5 && getLastListElement(list)->priority == 8, "First and last element of the list are not correctly referenced");
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 8, pool);
        ListElement* lastReversedElement = batchReverseList(list, 4);

        _markListForDeletion(list);

        QVERIFY(lastReversedElement && lastReversedElement->priority == 1);

        QVERIFY2(getListSize(list) == 8 &&
                 getListElementAtIndex(list, 0)->priority == 4 &&
                 getListElementAtIndex(list, 1)->priority == 5 &&
                 getListElementAtIndex(list, 2)->priority == 2 &&
                 getListElementAtIndex(list, 3)->priority == 6 &&
                 getListElementAtIndex(list, 4)->priority == 8 &&
                 getListElementAtIndex(list, 5)->priority == 7 &&
                 getListElementAtIndex(list, 6)->priority == 2 &&
                 getListElementAtIndex(list, 7)->priority == 1,   "The list has not been correctly batch reversed");

        QVERIFY2(getFirstListElement(list)->priority == 4 && getLastListElement(list)->priority == 1, "First and last element of the list are not correctly referenced");
    }

    // some additional (corner) cases
    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 2, pool);
        ListElement* lastReversedElement = batchReverseList(list, 0);

        _markListForDeletion(list);

        QVERIFY(!lastReversedElement);

        QVERIFY2(getListSize(list) == 2 &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 2,   "The list has not been correctly batch reversed");

        QVERIFY2(getFirstListElement(list)->priority == 6 && getLastListElement(list)->priority == 2, "First and last element of the list are not correctly referenced");
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 2, pool);
        ListElement* lastReversedElement = batchReverseList(list, 1);

        _markListForDeletion(list);

        QVERIFY(!lastReversedElement);

        QVERIFY2(getListSize(list) == 2 &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 2,   "The list has not been correctly batch reversed");

        QVERIFY2(getFirstListElement(list)->priority == 6 && getLastListElement(list)->priority == 2, "First and last element of the list are not correctly referenced");
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 2, pool);
        ListElement* lastReversedElement = batchReverseList(list, 2);

        _markListForDeletion(list);

        QVERIFY(lastReversedElement && lastReversedElement->priority == 6);

        QVERIFY2(getListSize(list) == 2 &&
                 getListElementAtIndex(list, 0)->priority == 2 &&
                 getListElementAtIndex(list, 1)->priority == 6,   "The list has not been correctly batch reversed");

        QVERIFY2(getFirstListElement(list)->priority == 2 && getLastListElement(list)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 2, pool);
        ListElement* lastReversedElement = batchReverseList(list, 3);

        _markListForDeletion(list);

        QVERIFY(!lastReversedElement);

        QVERIFY2(getListSize(list) == 2 &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 2,   "The list has not been correctly batch reversed");

        QVERIFY2(getFirstListElement(list)->priority == 6 && getLastListElement(list)->priority == 2, "First and last element of the list are not correctly referenced");
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 1, pool);
        ListElement* lastReversedElement = batchReverseList(list, 0);

        _markListForDeletion(list);

        QVERIFY(!lastReversedElement);

        QVERIFY2(getListSize(list) == 1 &&
                 getListElementAtIndex(list, 0)->priority == 6, "The list has not been correctly batch reversed");

        QVERIFY2(getFirstListElement(list)->priority == 6 && getLastListElement(list)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 1, pool);
        ListElement* lastReversedElement = batchReverseList(list, 1);

        _markListForDeletion(list);

        QVERIFY(!lastReversedElement);

        QVERIFY2(getListSize(list) == 1 &&
                 getListElementAtIndex(list, 0)->priority == 6, "The list has not been correctly batch reversed");

        QVERIFY2(getFirstListElement(list)->priority == 6 && getLastListElement(list)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 1, pool);
        ListElement* lastReversedElement = batchReverseList(list, 2);

        _markListForDeletion(list);

        QVERIFY(!lastReversedElement);

        QVERIFY2(getListSize(list) == 1 &&
                 getListElementAtIndex(list, 0)->priority == 6, "The list has not been correctly batch reversed");

        QVERIFY2(getFirstListElement(list)->priority == 6 && getLastListElement(list)->priority == 6, "First and last element of the list are not correctly referenced");
    }

    {
        List* list = createEmptyList(pool);
        ListElement* lastReversedElement = batchReverseList(list, 0);

        _markListForDeletion(list);

        QVERIFY(!lastReversedElement);
        QVERIFY(isEmptyList(list));

        lastReversedElement = batchReverseList(list, 1);

        QVERIFY(!lastReversedElement);
        QVERIFY(isEmptyList(list));
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

void LinkedListTests::testAssignRemoveObject()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    m_List1 = createEmptyList(pool);

    // Point
    Point* point = static_cast<Point*>(malloc(sizeof(Point)));
    point->x = 3;
    point->y = 4;
    assignObjectContentToListElement(createAndAppendToList(m_List1, 2), POINT, static_cast<void*>(point));
    point = nullptr;
    // int
    int* distance = static_cast<int*>(malloc(sizeof(int)));
    *distance = 5;
    assignObjectContentToListElement(createAndAppendToList(m_List1, 3), INTEGER, static_cast<void*>(distance));
    distance = nullptr;
    // float
    double* angle = static_cast<double*>(malloc(sizeof(double)));
    *angle = 1.25;
    assignObjectContentToListElement(createAndAppendToList(m_List1, 1), DECIMAL, static_cast<void*>(angle));
    angle = nullptr;
    // no object
    Q_UNUSED(createAndPrependToList(m_List1, 10));

    QVERIFY(getListSize(m_List1) == 4);

    ListIterator it = lbegin(m_List1);
    QVERIFY2(it.current->object.type == -1 && it.current->object.payload == nullptr, "Default object is incorrect (should be empty)");
    lnext(&it);
    QVERIFY2(it.current->object.type == POINT && (static_cast<Point*>(it.current->object.payload))->x == 3
             && (static_cast<Point*>(it.current->object.payload))->y == 4, "Object has been incorrectly assigned");
    lnext(&it);
    QVERIFY2(it.current->object.type == INTEGER && *(static_cast<int*>(it.current->object.payload)) == 5, "Object has been incorrectly assigned");
    lnext(&it);
    QVERIFY2(it.current->object.type == DECIMAL && areDecimalNumbersEqual(*(static_cast<double*>(it.current->object.payload)), 1.25), "Object has been incorrectly assigned");

    Object* removedObject = static_cast<Object*>(detachContentFromListElement(getListElementAtIndex(m_List1, 2)));
    QVERIFY2(getListElementAtIndex(m_List1, 2)->object.type == -1 &&
             getListElementAtIndex(m_List1, 2)->object.payload == nullptr &&
             removedObject->type == INTEGER &&
             (*(static_cast<int*>(removedObject->payload)) == 5), "Incorrect object removal from list element");

    deleteObjectPayload(removedObject);
    free(removedObject);
    removedObject = nullptr;

    QVERIFY(getListSize(m_List1) == 4);
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

void LinkedListTests::testGetFirstAndLastElement()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    {
        const size_t prioritiesArray[4]{6, 2, 5, 9};
        m_List1 = createListFromPrioritiesArray(prioritiesArray, 4, pool);

        QVERIFY(getListSize(m_List1) == 4);
        QVERIFY2(getFirstListElement(m_List1)->priority == 6, "The first list element is not correctly retrieved");
        QVERIFY2(getLastListElement(m_List1)->priority == 9, "The last list element is not correctly retrieved");
    }

    {
        m_List2 = createEmptyList(pool);

        QVERIFY(isEmptyList(m_List2));
        QVERIFY2(getFirstListElement(m_List2) == nullptr, "The first list element is not correctly retrieved");
        QVERIFY2(getLastListElement(m_List2) == nullptr, "The last list element is not correctly retrieved");
    }
}

void LinkedListTests::testMoveListToArray()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    const size_t prioritiesArray[4]{6, 2, 5, 9};

    m_List1 = createListFromPrioritiesArray(prioritiesArray, 4, pool);

    QVERIFY(getListSize(m_List1) == 4);

    size_t arraySize;
    ListElement** array = moveListToArray(m_List1, &arraySize);

    QVERIFY2(isEmptyList(m_List1) &&
             arraySize == 4 &&
             array[0]->next == nullptr && array[0]->priority == 6 &&
             array[1]->next == nullptr && array[1]->priority == 2 &&
             array[2]->next == nullptr && array[2]->priority == 5 &&
             array[3]->next == nullptr && array[3]->priority == 9 ,  "The list content has been incorrectly moved to array");

    for (size_t index = 0; index < arraySize; ++index)
    {
        _markListElementForCleanup(array[index], pool != nullptr);
    }

    free(array);
    array = nullptr;
}

void LinkedListTests::testMoveArrayToList()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    m_List1 = createEmptyList(pool);
    m_ListElementRefs = static_cast<ListElement**>(calloc(4, sizeof(ListElement*)));

    m_ListElementRefs[0] = pool ? aquireElement(pool) : createListElement();
    _markListElementForCleanup(m_ListElementRefs[0], pool != nullptr);

    QVERIFY(m_ListElementRefs[0]);

    m_ListElementRefs[1] = pool ? aquireElement(pool) : createListElement();
    _markListElementForCleanup(m_ListElementRefs[1], pool != nullptr);

    QVERIFY(m_ListElementRefs[1]);

    m_ListElementRefs[2] = pool ? aquireElement(pool) : createListElement();
    _markListElementForCleanup(m_ListElementRefs[2], pool != nullptr);

    QVERIFY(m_ListElementRefs[2]);

    m_ListElementRefs[3] = pool ? aquireElement(pool) : createListElement();
    _markListElementForCleanup(m_ListElementRefs[3], pool != nullptr);

    QVERIFY(m_ListElementRefs[3]);

    m_ListElementRefs[0]->priority = 6;
    m_ListElementRefs[1]->priority = 2;
    m_ListElementRefs[2]->priority = 5;
    m_ListElementRefs[3]->priority = 9;

    QVERIFY(isEmptyList(m_List1));

    // previous cleanup markings were performed to prevent memory leaks; upon reaching this point the elements should be unmarked as the they would get transferred to the list (no longer "free")
    _clearListElementsMarkedForCleanup();

    moveArrayToList(m_ListElementRefs, 4, m_List1);

    QVERIFY2(m_ListElementRefs[0] == nullptr &&
             m_ListElementRefs[1] == nullptr &&
             m_ListElementRefs[2] == nullptr &&
             m_ListElementRefs[3] == nullptr &&
             getListSize(m_List1) == 4 &&
             getListElementAtIndex(m_List1, 0)->priority == 6 &&
             getListElementAtIndex(m_List1, 1)->priority == 2 &&
             getListElementAtIndex(m_List1, 2)->priority == 5 &&
             getListElementAtIndex(m_List1, 3)->priority == 9, "The array content has been incorrectly moved to list");
    QVERIFY2(getFirstListElement(m_List1)->priority == 6 && getLastListElement(m_List1)->priority == 9, "First and last element of the list are not correctly referenced");
    QVERIFY(getListElementAtIndex(m_List1, 1)->object.type == -1 && getListElementAtIndex(m_List1, 1)->object.payload == nullptr);
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

    QVERIFY(!m_ListElementRefs);

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

    QVERIFY(!m_ListElementRefs);

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
    m_ListsMarkedForDeletion.clear();

    QVERIFY(getAvailableElementsCount(m_Pool) == m_TotalAvailablePoolElementsCount);

    // it is assumed that the actual elements have been previously cleaned up (deleted or released) during test run
    FREE(m_ListElementRefs);
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

void LinkedListTests::_clearListElementsMarkedForCleanup()
{
    m_ListElementsMarkedForRelease.clear();
    m_ListElementsMarkedForDeletion.clear();
}

QTEST_APPLESS_MAIN(LinkedListTests)

#include "tst_linkedlisttests.moc"
