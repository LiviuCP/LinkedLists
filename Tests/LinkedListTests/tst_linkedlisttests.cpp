#include <QtTest>

#include <cstring>

#include "../../LinkedListsLib/linkedlist.h"
#include "../../LinkedListsLib/sort.h"
#include "../../Utils/codeutils.h"
#include "../../Utils/testobjects.h"

class LinkedListTests : public QObject
{
    Q_OBJECT

public:
    LinkedListTests();
    ~LinkedListTests();

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
    void testSortAscendingByPriority();
    void testSortDescendingByPriority();
    void testSortAscendingByPriorityUsingRandomAccess();
    void testSortDescendingByPriorityUsingRandomAccess();
    void testIterators();
    void testAssignRemoveObject();
    void testIsElementContained();
    void testIsSortedAscendingByPriority();
    void testIsSortedDescendingByPriority();
    void testGetPreviousElement();
    void testGetLastElement();
    void testMoveListToArray();
    void testMoveArrayToList();
    void testPrintListElementsToFile();
private:
    size_t _getSumOfPriorities(List* list);
};

LinkedListTests::LinkedListTests()
{
}

LinkedListTests::~LinkedListTests()
{
}

void LinkedListTests::testListIsCorrectlyCreatedAndCleared()
{
    {
        const size_t prioritiesArray[9]{6, 2, 5, 4, 3, 1, 2, 9, 7};
        List* list = createListFromPrioritiesArray(prioritiesArray, 9);

        QVERIFY2(getListSize(list) == 9, "List size is not correct");
        QVERIFY2(getListElementAtIndex(list, 0)->priority == 6 && getListElementAtIndex(list, 4)->priority == 3 && getListElementAtIndex(list, 8)->priority == 7,
                 "The element is not correctly retrieved based on index");

        clearList(list, deleteObjectPayload);
        QVERIFY2(getListSize(list) == 0, "The list has not been correctly emptied");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }
}

void LinkedListTests::testAppendElement()
{
    const size_t prioritiesArray[9]{6, 2, 5, 4, 3, 1, 2, 9, 7};
    List* list = createListFromPrioritiesArray(prioritiesArray, 9);
    createAndAppendToList(list, 8);

    QVERIFY2(getListSize(list) == 10 && getListElementAtIndex(list, 0)->priority == 6 && getListElementAtIndex(list, 9)->priority == 8, "Element has not been correctly created and appended");

    deleteList(list, deleteObjectPayload);
    list = nullptr;
}

void LinkedListTests::testPrependElement()
{
    const size_t prioritiesArray[9]{6, 2, 5, 4, 3, 1, 2, 9, 7};
    List* list = createListFromPrioritiesArray(prioritiesArray, 9);
    createAndPrependToList(list, 8);

    QVERIFY2(getListSize(list) == 10 && getListElementAtIndex(list, 0)->priority == 8 && getListElementAtIndex(list, 9)->priority == 7, "Element has not been correctly created and prepended");

    deleteList(list, deleteObjectPayload);
    list = nullptr;
}

void LinkedListTests::testRemoveFirstElement()
{
    const size_t prioritiesArray[9]{6, 2, 5, 4, 3, 1, 2, 9, 8};
    List* list = createListFromPrioritiesArray(prioritiesArray, 9);
    ListElement* firstElement = removeFirstListElement(list);

    QVERIFY2(getListSize(list) == 8 && getListElementAtIndex(list, 0)->priority == 2 && getListElementAtIndex(list, 7)->priority == 8, "Element has not been correctly created and appended");
    QVERIFY(firstElement->next == nullptr && firstElement->priority == 6);

    free(firstElement);
    deleteList(list, deleteObjectPayload);
    list = nullptr;
}

void LinkedListTests::testRemoveLastElement()
{
    const size_t prioritiesArray[9]{6, 2, 5, 4, 3, 1, 2, 9, 8};
    List* list = createListFromPrioritiesArray(prioritiesArray, 9);
    ListElement* lastElement = removeLastListElement(list);

    QVERIFY2(getListSize(list) == 8 && getListElementAtIndex(list, 0)->priority == 6 && getListElementAtIndex(list, 7)->priority == 9, "Element has not been correctly created and appended");
    QVERIFY(lastElement->next == nullptr && lastElement->priority == 8);

    free(lastElement);
    deleteList(list, deleteObjectPayload);
    list = nullptr;
}

void LinkedListTests::testInsertElementBefore()
{
    const size_t prioritiesArray[4]{6, 2, 5, 4};

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4);

        ListIterator it = lbegin(list);
        createAndInsertBefore(it, 10);

        QVERIFY2(getListSize(list) == 5 &&
                 getListElementAtIndex(list, 0)->priority == 10 &&
                 getListElementAtIndex(list, 1)->priority == 6 &&
                 getListElementAtIndex(list, 2)->priority == 2 &&
                 getListElementAtIndex(list, 3)->priority == 5 &&
                 getListElementAtIndex(list, 4)->priority == 4,   "The previous element has not been correctly inserted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4);

        ListIterator it = lbegin(list);
        lnext(&it);
        createAndInsertBefore(it, 10);

        QVERIFY2(getListSize(list) == 5 &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 10 &&
                 getListElementAtIndex(list, 2)->priority == 2 &&
                 getListElementAtIndex(list, 3)->priority == 5 &&
                 getListElementAtIndex(list, 4)->priority == 4,   "The previous element has not been correctly inserted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4);

        ListIterator it = lbegin(list);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        createAndInsertBefore(it, 10);

        QVERIFY2(getListSize(list) == 5 &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 2 &&
                 getListElementAtIndex(list, 2)->priority == 5 &&
                 getListElementAtIndex(list, 3)->priority == 10 &&
                 getListElementAtIndex(list, 4)->priority == 4,   "The previous element has not been correctly inserted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4);

        ListIterator it = lbegin(list);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        createAndInsertBefore(it, 10);

        QVERIFY2(getListSize(list) == 5 &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 2 &&
                 getListElementAtIndex(list, 2)->priority == 5 &&
                 getListElementAtIndex(list, 3)->priority == 4 &&
                 getListElementAtIndex(list, 4)->priority == 10,   "The previous element has not been correctly inserted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createEmptyList();
        ListIterator it = lbegin(list);
        createAndInsertBefore(it, 10);

        QVERIFY2(getListSize(list) == 1 && getListElementAtIndex(list, 0)->priority == 10, "The element has not been correctly inserted into an empty list");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }
}

void LinkedListTests::testInsertElementAfter()
{
    const size_t prioritiesArray[4]{6, 2, 5, 4};

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4);

        ListIterator it = lbegin(list);
        createAndInsertAfter(it, 10);

        QVERIFY2(getListSize(list) == 5 &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 10 &&
                 getListElementAtIndex(list, 2)->priority == 2 &&
                 getListElementAtIndex(list, 3)->priority == 5 &&
                 getListElementAtIndex(list, 4)->priority == 4,   "The next element has not been correctly inserted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4);

        ListIterator it = lbegin(list);
        lnext(&it);
        createAndInsertAfter(it, 10);

        QVERIFY2(getListSize(list) == 5 &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 2 &&
                 getListElementAtIndex(list, 2)->priority == 10 &&
                 getListElementAtIndex(list, 3)->priority == 5 &&
                 getListElementAtIndex(list, 4)->priority == 4,   "The next element has not been correctly inserted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4);

        ListIterator it = lbegin(list);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        createAndInsertAfter(it, 10);

        QVERIFY2(getListSize(list) == 5 &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 2 &&
                 getListElementAtIndex(list, 2)->priority == 5 &&
                 getListElementAtIndex(list, 3)->priority == 4 &&
                 getListElementAtIndex(list, 4)->priority == 10,   "The next element has not been correctly inserted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createEmptyList();
        ListIterator it = lbegin(list);
        createAndInsertAfter(it, 10);

        QVERIFY2(getListSize(list) == 1 && getListElementAtIndex(list, 0)->priority == 10, "The element has not been correctly inserted into an empty list");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }
}

void LinkedListTests::testMoveContentToList()
{
    const size_t firstPrioritiesArray[2]{6, 2,};
    const size_t secondPrioritiesArray[2]{7, 4};

    {
        List* source = createListFromPrioritiesArray(firstPrioritiesArray, 2);
        List* destination = createListFromPrioritiesArray(secondPrioritiesArray, 2);

        moveContentToList(source, destination);

        QVERIFY2(getListSize(source) == 0 &&
                 getListSize(destination) == 4 &&
                 getListElementAtIndex(destination, 0)->priority == 7 &&
                 getListElementAtIndex(destination, 1)->priority == 4 &&
                 getListElementAtIndex(destination, 2)->priority == 6 &&
                 getListElementAtIndex(destination, 3)->priority == 2,   "The source list content has not been correctly moved to destination");

        deleteList(source, deleteObjectPayload);
        source = nullptr;
        deleteList(destination, deleteObjectPayload);
        destination = nullptr;
    }

    {
        List* source = createEmptyList();
        List* destination = createListFromPrioritiesArray(secondPrioritiesArray, 2);

        moveContentToList(source, destination);

        QVERIFY2(getListSize(source) == 0 &&
                 getListSize(destination) == 2 &&
                 getListElementAtIndex(destination, 0)->priority == 7 &&
                 getListElementAtIndex(destination, 1)->priority == 4,   "The source list content has not been correctly moved to destination");

        deleteList(source, deleteObjectPayload);
        source = nullptr;
        deleteList(destination, deleteObjectPayload);
        destination = nullptr;
    }

    {
        List* source = createListFromPrioritiesArray(firstPrioritiesArray, 2);
        List* destination = createEmptyList();

        moveContentToList(source, destination);

        QVERIFY2(getListSize(source) == 0 &&
                 getListSize(destination) == 2 &&
                 getListElementAtIndex(destination, 0)->priority == 6 &&
                 getListElementAtIndex(destination, 1)->priority == 2,   "The source list content has not been correctly moved to destination");

        deleteList(source, deleteObjectPayload);
        source = nullptr;
        deleteList(destination, deleteObjectPayload);
        destination = nullptr;
    }
}

void LinkedListTests::testCopyContentToList()
{
    const size_t firstPrioritiesArray[2]{6, 2,};
    const size_t secondPrioritiesArray[2]{7, 4};
    const size_t thirdPriorityArray[3]{5, 4, 3};

    {
        List* source = createListFromPrioritiesArray(firstPrioritiesArray, 2);
        List* destination = createListFromPrioritiesArray(secondPrioritiesArray, 2);

        copyContentToList(source, destination, copyObjectPlaceholder, deleteObjectPayload);

        QVERIFY2(getListSize(source) == 2 &&
                 getListSize(destination) == 4 &&
                 getListElementAtIndex(destination, 0)->priority == 7 &&
                 getListElementAtIndex(destination, 1)->priority == 4 &&
                 getListElementAtIndex(destination, 2)->priority == 6 &&
                 getListElementAtIndex(destination, 3)->priority == 2,   "The source list content has not been correctly copied to destination");

        deleteList(source, deleteObjectPayload);
        source = nullptr;
        deleteList(destination, deleteObjectPayload);
        destination = nullptr;
    }

    {
        List* source = createEmptyList();
        List* destination = createListFromPrioritiesArray(secondPrioritiesArray, 2);

        copyContentToList(source, destination, copyObjectPlaceholder, deleteObjectPayload);

        QVERIFY2(getListSize(source) == 0 &&
                 getListSize(destination) == 2 &&
                 getListElementAtIndex(destination, 0)->priority == 7 &&
                 getListElementAtIndex(destination, 1)->priority == 4,   "The source list content has not been correctly copied to destination");

        deleteList(source, deleteObjectPayload);
        source = nullptr;
        deleteList(destination, deleteObjectPayload);
        destination = nullptr;
    }

    {
        List* source = createListFromPrioritiesArray(firstPrioritiesArray, 2);
        List* destination = createEmptyList();

        copyContentToList(source, destination, copyObjectPlaceholder, deleteObjectPayload);

        QVERIFY2(getListSize(source) == 2 &&
                 getListSize(destination) == 2 &&
                 getListElementAtIndex(destination, 0)->priority == 6 &&
                 getListElementAtIndex(destination, 1)->priority == 2,   "The source list content has not been correctly copied to destination");

        deleteList(source, deleteObjectPayload);
        source = nullptr;
        deleteList(destination, deleteObjectPayload);
        destination = nullptr;
    }

    {
        List* source = createListFromPrioritiesArray(firstPrioritiesArray, 2);
        List* destination = createListFromPrioritiesArray(thirdPriorityArray, 3);

        ListIterator it = lbegin(source);
        assignObjectContentToListElement(it.current, SEGMENT, createSegmentPayload(2, 5, 4, 11));
        lnext(&it);
        assignObjectContentToListElement(it.current, LOCAL_CONDITIONS, createLocalConditionsPayload(7, -5, 10, 12.8));

        copyContentToList(source, destination, customCopyObject, emptyTestObject);

        const Segment* copiedSegment = static_cast<Segment*>(getListElementAtIndex(destination, 3)->object.payload);
        const LocalConditions* copiedConditions = static_cast<LocalConditions*>(getListElementAtIndex(destination, 4)->object.payload);

        QVERIFY2(getListSize(source) == 2 &&
                 getListSize(destination) == 5 &&
                 getListElementAtIndex(destination, 0)->priority == 5 &&
                 getListElementAtIndex(destination, 0)->object.type == -1 &&
                 getListElementAtIndex(destination, 0)->object.payload == nullptr &&
                 getListElementAtIndex(destination, 1)->priority == 4 &&
                 getListElementAtIndex(destination, 1)->object.type == -1 &&
                 getListElementAtIndex(destination, 1)->object.payload == nullptr &&
                 getListElementAtIndex(destination, 2)->priority == 3 &&
                 getListElementAtIndex(destination, 2)->object.type == -1 &&
                 getListElementAtIndex(destination, 2)->object.payload == nullptr &&
                 getListElementAtIndex(destination, 3)->priority == 6 &&
                 getListElementAtIndex(destination, 3)->object.type == SEGMENT &&
                 copiedSegment->start->x == 2 && copiedSegment->start->y == 5 && copiedSegment->stop->x == 4 && copiedSegment->stop->y == 11 &&
                 getListElementAtIndex(destination, 4)->priority == 2 &&
                 getListElementAtIndex(destination, 4)->object.type == LOCAL_CONDITIONS &&
                 copiedConditions->position->x == 7 && copiedConditions->position->y == -5 && copiedConditions->temperature == 10 &&
                 areDecimalNumbersEqual(copiedConditions->humidity, 12.8), "The source list content has not been correctly copied to destination");

        deleteList(source, emptyTestObject);
        source = nullptr;
        deleteList(destination, emptyTestObject);
        destination = nullptr;
    }
}

void LinkedListTests::testRemoveElementBefore()
{
    const size_t prioritiesArray[4]{6, 2, 5, 4};

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4);

        ListIterator it = lbegin(list);
        ListElement* removedElement = removePreviousListElement(it);

        QVERIFY2(getListSize(list) == 4 &&
                 removedElement == nullptr &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 2 &&
                 getListElementAtIndex(list, 2)->priority == 5 &&
                 getListElementAtIndex(list, 3)->priority == 4,   "The previous element removal does not work correctly");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4);

        ListIterator it = lbegin(list);
        lnext(&it);
        ListElement* removedElement = removePreviousListElement(it);

        QVERIFY2(getListSize(list) == 3 &&
                 removedElement->priority == 6 &&
                 getListElementAtIndex(list, 0)->priority == 2 &&
                 getListElementAtIndex(list, 1)->priority == 5 &&
                 getListElementAtIndex(list, 2)->priority == 4,   "The previous element has not been correctly removed");

        free(removedElement);
        removedElement = nullptr;
        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4);

        ListIterator it = lbegin(list);
        lnext(&it);
        lnext(&it);
        ListElement* removedElement = removePreviousListElement(it);

        QVERIFY2(getListSize(list) == 3 &&
                 removedElement->priority == 2 &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 5 &&
                 getListElementAtIndex(list, 2)->priority == 4,   "The previous element has not been correctly removed");

        free(removedElement);
        removedElement = nullptr;
        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4);

        ListIterator it = lbegin(list);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        ListElement* removedElement = removePreviousListElement(it);

        QVERIFY2(getListSize(list) == 3 &&
                 removedElement->priority == 5 &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 2 &&
                 getListElementAtIndex(list, 2)->priority == 4,   "The previous element has not been correctly removed");

        free(removedElement);
        removedElement = nullptr;
        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4);

        ListIterator it = lbegin(list);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        ListElement* removedElement = removePreviousListElement(it);

        QVERIFY2(getListSize(list) == 3 &&
                 removedElement->priority == 4 &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 2 &&
                 getListElementAtIndex(list, 2)->priority == 5,   "The previous element has not been correctly removed");

        free(removedElement);
        removedElement = nullptr;
        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createEmptyList();
        createAndAppendToList(list, 5);
        ListIterator it = lbegin(list);
        ListElement* removedElement = removePreviousListElement(it);

        QVERIFY2(getListSize(list) == 1 && removedElement == nullptr && getListElementAtIndex(list, 0)->priority == 5, "The previous element removal does not work correctly");

        lnext(&it);
        removedElement = removePreviousListElement(it);

        QVERIFY2(getListSize(list) == 0 && removedElement->priority == 5, "The only existing list element has not been correctly removed");

        free(removedElement);
        removedElement = nullptr;
        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }
}

void LinkedListTests::testRemoveElementAfter()
{
    const size_t prioritiesArray[4]{6, 2, 5, 4};

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4);

        ListIterator it = lbegin(list);
        ListElement* removedElement = removeNextListElement(it);

        QVERIFY2(getListSize(list) == 3 &&
                 removedElement->priority == 2 &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 5 &&
                 getListElementAtIndex(list, 2)->priority == 4,   "The next element has not been correctly removed");

        free(removedElement);
        removedElement = nullptr;
        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4);

        ListIterator it = lbegin(list);
        lnext(&it);
        ListElement* removedElement = removeNextListElement(it);

        QVERIFY2(getListSize(list) == 3 &&
                 removedElement->priority == 5 &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 2 &&
                 getListElementAtIndex(list, 2)->priority == 4,   "The next element has not been correctly removed");

        free(removedElement);
        removedElement = nullptr;
        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 4);

        ListIterator it = lbegin(list);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        ListElement* removedElement = removeNextListElement(it);

        QVERIFY2(getListSize(list) == 4 &&
                 removedElement == nullptr &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 2 &&
                 getListElementAtIndex(list, 2)->priority == 5 &&
                 getListElementAtIndex(list, 3)->priority == 4,   "The next element has not been correctly removed");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createEmptyList();
        createAndAppendToList(list, 5);
        ListIterator it = lbegin(list);
        ListElement* removedElement = removeNextListElement(it);

        QVERIFY2(getListSize(list) == 1 && removedElement == nullptr && getListElementAtIndex(list, 0)->priority == 5, "The only existing list element has not been correctly removed");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }
}

void LinkedListTests::testRemoveCurrentElement()
{
    const size_t prioritiesArray[3]{6, 2, 5};
    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 3);

        ListIterator it = lbegin(list);
        ListElement* removedElement = removeCurrentListElement(it);

        QVERIFY2(getListSize(list) == 2 &&
                 removedElement->priority == 6 &&
                 getListElementAtIndex(list, 0)->priority == 2 &&
                 getListElementAtIndex(list, 1)->priority == 5,   "The current element has not been correctly removed");

        free(removedElement);
        removedElement = nullptr;
        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 3);

        ListIterator it = lbegin(list);
        lnext(&it);
        ListElement* removedElement = removeCurrentListElement(it);

        QVERIFY2(getListSize(list) == 2 &&
                 removedElement->priority == 2 &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 5,   "The current element has not been correctly removed");

        free(removedElement);
        removedElement = nullptr;
        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 3);

        ListIterator it = lbegin(list);
        lnext(&it);
        lnext(&it);
        ListElement* removedElement = removeCurrentListElement(it);

        QVERIFY2(getListSize(list) == 2 &&
                 removedElement->priority == 5 &&
                 getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 2,   "The current element has not been correctly removed");

        free(removedElement);
        removedElement = nullptr;
        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createListFromPrioritiesArray(prioritiesArray, 3);

        ListIterator it = lbegin(list);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        ListElement* removedElement = removeCurrentListElement(it);

        QVERIFY2(getListSize(list) == 3 &&
                 removedElement == nullptr,   "The current element removal does not work correctly");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }
}

void LinkedListTests::testSwapElements()
{
    {
        const size_t prioritiesArray[5]{6, 2, 5, 4, 7};
        List* list = createListFromPrioritiesArray(prioritiesArray, 5);

        ListIterator firstIt = lbegin(list);
        ListIterator secondIt = firstIt;

        lnext(&firstIt);
        lnext(&secondIt);
        lnext(&secondIt);
        lnext(&secondIt);

        swapListElements(firstIt, secondIt);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 6 &&
                 getListElementAtIndex(list, 1)->priority == 4 &&
                 getListElementAtIndex(list, 2)->priority == 5 &&
                 getListElementAtIndex(list, 3)->priority == 2 &&
                 getListElementAtIndex(list, 4)->priority == 7,   "The elements have not been correctly swapped");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List *list = createEmptyList();
        createAndAppendToList(list, 2);

        swapListElements(lbegin(list), lbegin(list));
        QVERIFY2(getListElementAtIndex(list, 0)->priority == 2, "The swap function does not work correctly");

        swapListElements(lbegin(list), lend(list));
        QVERIFY2(getListElementAtIndex(list, 0)->priority == 2, "The swap function does not work correctly");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }
}

void LinkedListTests::testReverseList()
{
    const size_t firstPrioritiesArray[4]{6, 2, 5, 4};
    const size_t secondPrioritiesArray[2]{6, 2};

    {
        List* list = createListFromPrioritiesArray(firstPrioritiesArray, 4);
        reverseList(list);

        QVERIFY2(getListSize(list) == 4 &&
                 getListElementAtIndex(list, 0)->priority == 4 &&
                 getListElementAtIndex(list, 1)->priority == 5 &&
                 getListElementAtIndex(list, 2)->priority == 2 &&
                 getListElementAtIndex(list, 3)->priority == 6,   "The list has not been correctly reversed");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createListFromPrioritiesArray(secondPrioritiesArray, 2);
        reverseList(list);

        QVERIFY2(getListSize(list) == 2 &&
                 getListElementAtIndex(list, 0)->priority == 2 &&
                 getListElementAtIndex(list, 1)->priority == 6,   "The list has not been correctly reversed");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createEmptyList();
        createAndAppendToList(list, 2);
        reverseList(list);

        QVERIFY2(getListSize(list) == 1 && getListElementAtIndex(list, 0)->priority == 2, "The list has not been correctly reversed");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createEmptyList();
        reverseList(list);

        QVERIFY2(getListSize(list) == 0, "The list has not been correctly reversed");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }
}

void LinkedListTests::testSortAscendingByPriority()
{
    {
        const size_t prioritiesArray[6]{6, 2, 5, 1, 2, 3};
        List* list = createListFromPrioritiesArray(prioritiesArray, 6); // highest prio item first
        sortAscendingByPriority(list);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 1 && _getSumOfPriorities(list) == 19 && isSortedAscendingByPriority(list), "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        const size_t prioritiesArray[6]{3, 2, 1, 5, 2, 6};
        List* list = createListFromPrioritiesArray(prioritiesArray, 6); // highest prio item last
        sortAscendingByPriority(list);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 1 && _getSumOfPriorities(list) == 19 && isSortedAscendingByPriority(list), "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        const size_t prioritiesArray[6]{1, 6, 2, 5, 3, 2};
        List* list = createListFromPrioritiesArray(prioritiesArray, 6); // lowest prio item first
        sortAscendingByPriority(list);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 1 && _getSumOfPriorities(list) == 19 && isSortedAscendingByPriority(list), "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        const size_t prioritiesArray[6]{2, 3, 5, 2, 6, 1};
        List* list = createListFromPrioritiesArray(prioritiesArray, 6); // lowest prio item last
        sortAscendingByPriority(list);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 1 && _getSumOfPriorities(list) == 19 && isSortedAscendingByPriority(list), "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        const size_t prioritiesArray[6]{2, 3, 1, 2, 6, 5};
        List* list = createListFromPrioritiesArray(prioritiesArray, 6); // random
        sortAscendingByPriority(list);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 1 && _getSumOfPriorities(list) == 19 && isSortedAscendingByPriority(list), "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        const size_t prioritiesArray[2]{6, 5};
        List* list = createListFromPrioritiesArray(prioritiesArray, 2);
        sortAscendingByPriority(list);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 5 && getListElementAtIndex(list, 1)->priority == 6, "The list hasn't been correctly sorted ");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createEmptyList();
        createAndAppendToList(list, 1);

        sortAscendingByPriority(list);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 1, "The list hasn't been correctly sorted (ascending) by priority");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }
}

void LinkedListTests::testSortDescendingByPriority()
{
    {
        const size_t prioritiesArray[6]{6, 2, 5, 1, 2, 3};
        List* list = createListFromPrioritiesArray(prioritiesArray, 6); // highest prio item first
        sortDescendingByPriority(list);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 6 && _getSumOfPriorities(list) == 19 && isSortedDescendingByPriority(list), "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        const size_t prioritiesArray[6]{3, 2, 1, 5, 2, 6};
        List* list = createListFromPrioritiesArray(prioritiesArray, 6); // highest prio item last
        sortDescendingByPriority(list);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 6 && _getSumOfPriorities(list) == 19 && isSortedDescendingByPriority(list), "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        const size_t prioritiesArray[6]{1, 6, 2, 5, 3, 2};
        List* list = createListFromPrioritiesArray(prioritiesArray, 6); // lowest prio item first
        sortDescendingByPriority(list);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 6 && _getSumOfPriorities(list) == 19 && isSortedDescendingByPriority(list), "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        const size_t prioritiesArray[6]{2, 3, 5, 2, 6, 1};
        List* list = createListFromPrioritiesArray(prioritiesArray, 6); // lowest prio item last
        sortDescendingByPriority(list);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 6 && _getSumOfPriorities(list) == 19 && isSortedDescendingByPriority(list), "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        const size_t prioritiesArray[6]{2, 3, 1, 2, 6, 5};
        List* list = createListFromPrioritiesArray(prioritiesArray, 6); // random
        sortDescendingByPriority(list);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 6 && _getSumOfPriorities(list) == 19 && isSortedDescendingByPriority(list), "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        const size_t prioritiesArray[2]{5, 6};
        List* list = createListFromPrioritiesArray(prioritiesArray, 2);
        sortDescendingByPriority(list);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 6 && getListElementAtIndex(list, 1)->priority == 5, "The list hasn't been correctly sorted ");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createEmptyList();
        createAndAppendToList(list, 1);

        sortDescendingByPriority(list);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 1, "The list hasn't been correctly sorted (ascending) by priority");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }
}

void LinkedListTests::testSortAscendingByPriorityUsingRandomAccess()
{
    const size_t firstPrioritiesArray[6]{2, 3, 5, 2, 6, 1};
    const size_t secondPrioritiesArray[8]{2, 3, 5, 2, 6, 1, 8, 7};
    const size_t thirdPrioritiesArray[12]{2, 3, 5, 2, 9, 6, 1, 8, 7, 5, 4, 6};

    // insertion sort
    {
        List* list = createListFromPrioritiesArray(firstPrioritiesArray, 6);
        sortByRandomAccess(list, insertionSortAscendingByPriority);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 1 && _getSumOfPriorities(list) == 19 && isSortedAscendingByPriority(list),
                 "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    // heap sort
    {
        List* list = createListFromPrioritiesArray(firstPrioritiesArray, 6);
        sortByRandomAccess(list, heapSortAscendingByPriority);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 1 && _getSumOfPriorities(list) == 19 && isSortedAscendingByPriority(list),
                 "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    // merge sort
    {
        List* list = createListFromPrioritiesArray(secondPrioritiesArray, 8);
        sortByRandomAccess(list, mergeSortAscendingByPriority);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 1 && _getSumOfPriorities(list) == 34 && isSortedAscendingByPriority(list),
                 "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    // quick sort
    {
        List* list = createListFromPrioritiesArray(secondPrioritiesArray, 8);
        sortByRandomAccess(list, quickSortAscendingByPriority);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 1 && _getSumOfPriorities(list) == 34 && isSortedAscendingByPriority(list),
                 "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    // enhanced merge sort
    {
        List* list = createListFromPrioritiesArray(thirdPrioritiesArray, 12);
        sortByRandomAccess(list, enhancedMergeSortAscendingByPriority);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 1 && _getSumOfPriorities(list) == 58 && isSortedAscendingByPriority(list),
                 "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    // quick merge sort
    {
        List* list = createListFromPrioritiesArray(thirdPrioritiesArray, 12);
        sortByRandomAccess(list, quickMergeSortAscendingByPriority);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 1 && _getSumOfPriorities(list) == 58 && isSortedAscendingByPriority(list),
                 "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    // enhanced quick sort
    {
        List* list = createListFromPrioritiesArray(thirdPrioritiesArray, 12);
        sortByRandomAccess(list, enhancedQuickSortAscendingByPriority);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 1 && _getSumOfPriorities(list) == 58 && isSortedAscendingByPriority(list),
                 "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }
}

void LinkedListTests::testSortDescendingByPriorityUsingRandomAccess()
{
    const size_t firstPrioritiesArray[6]{2, 3, 5, 2, 6, 1};
    const size_t secondPrioritiesArray[8]{2, 3, 5, 2, 6, 1, 8, 7};
    const size_t thirdPrioritiesArray[12]{2, 3, 5, 2, 9, 6, 1, 8, 7, 5, 4, 6};

    // insertion sort
    {
        List* list = createListFromPrioritiesArray(firstPrioritiesArray, 6);
        sortByRandomAccess(list, insertionSortDescendingByPriority);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 6 && _getSumOfPriorities(list) == 19 && isSortedDescendingByPriority(list),
                 "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    // heap sort
    {
        List* list = createListFromPrioritiesArray(firstPrioritiesArray, 6);
        sortByRandomAccess(list, insertionSortDescendingByPriority);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 6 && _getSumOfPriorities(list) == 19 && isSortedDescendingByPriority(list),
                 "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    // merge sort
    {
        List* list = createListFromPrioritiesArray(secondPrioritiesArray, 8);
        sortByRandomAccess(list, insertionSortDescendingByPriority);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 8 && _getSumOfPriorities(list) == 34 && isSortedDescendingByPriority(list),
                 "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    // quick sort
    {
        List* list = createListFromPrioritiesArray(secondPrioritiesArray, 8);
        sortByRandomAccess(list, insertionSortDescendingByPriority);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 8 && _getSumOfPriorities(list) == 34 && isSortedDescendingByPriority(list),
                 "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    // enhanced merge sort
    {
        List* list = createListFromPrioritiesArray(thirdPrioritiesArray, 12);
        sortByRandomAccess(list, enhancedMergeSortDescendingByPriority);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 9 && _getSumOfPriorities(list) == 58 && isSortedDescendingByPriority(list),
                 "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    // quick merge sort
    {
        List* list = createListFromPrioritiesArray(thirdPrioritiesArray, 12);
        sortByRandomAccess(list, quickMergeSortDescendingByPriority);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 9 && _getSumOfPriorities(list) == 58 && isSortedDescendingByPriority(list),
                 "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    // enhanced quick sort
    {
        List* list = createListFromPrioritiesArray(thirdPrioritiesArray, 12);
        sortByRandomAccess(list, enhancedQuickSortDescendingByPriority);

        QVERIFY2(getListElementAtIndex(list, 0)->priority == 9 && _getSumOfPriorities(list) == 58 && isSortedDescendingByPriority(list),
                 "The list hasn't been correctly sorted");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }
}

void LinkedListTests::testIterators()
{
    {
        const size_t prioritiesArray[4]{6, 2, 5, 9};
        List* list = createListFromPrioritiesArray(prioritiesArray, 4);
        ListIterator it = lbegin(list);

        QVERIFY2(!areIteratorsEqual(it, lend(list)) && it.current->priority == 6, "The begin iterator is not correctly generated");
        lnext(&it);
        QVERIFY2(!areIteratorsEqual(it, lend(list)) && it.current->priority == 2, "The iterator is not correctly incremented");
        lnext(&it);
        QVERIFY2(!areIteratorsEqual(it, lend(list)) && it.current->priority == 5, "The iterator is not correctly incremented");
        lnext(&it);
        QVERIFY2(!areIteratorsEqual(it, lend(list)) && it.current->priority == 9, "The iterator is not correctly incremented");
        lnext(&it);
        QVERIFY2(areIteratorsEqual(it, lend(list)), "The iterator is not correctly incremented");
        lnext(&it);
        QVERIFY2(areIteratorsEqual(it, lend(list)), "The iterator is not correctly incremented");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createEmptyList();
        ListIterator it = lbegin(list);
        QVERIFY2(areIteratorsEqual(it, lend(list)), "The list is empty but the begin and end iterators are not equal");
        lnext(&it);
        QVERIFY2(areIteratorsEqual(it, lend(list)), "The iterator is not correctly incremented");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }
}

void LinkedListTests::testAssignRemoveObject()
{
    List* list = createEmptyList();
    // Point
    Point* point = static_cast<Point*>(malloc(sizeof(Point)));
    point->x = 3;
    point->y = 4;
    assignObjectContentToListElement(createAndAppendToList(list, 2), POINT, static_cast<void*>(point));
    point = nullptr;
    // int
    int* distance = static_cast<int*>(malloc(sizeof(int)));
    *distance = 5;
    assignObjectContentToListElement(createAndAppendToList(list, 3), INTEGER, static_cast<void*>(distance));
    distance = nullptr;
    // float
    double* angle = static_cast<double*>(malloc(sizeof(double)));
    *angle = 1.25;
    assignObjectContentToListElement(createAndAppendToList(list, 1), DECIMAL, static_cast<void*>(angle));
    angle = nullptr;
    // no object
    Q_UNUSED(createAndPrependToList(list, 10));

    ListIterator it = lbegin(list);
    QVERIFY2(it.current->object.type == -1 && it.current->object.payload == nullptr, "Default object is incorrect (should be empty)");
    lnext(&it);
    QVERIFY2(it.current->object.type == POINT && (static_cast<Point*>(it.current->object.payload))->x == 3
             && (static_cast<Point*>(it.current->object.payload))->y == 4, "Object has been incorrectly assigned");
    lnext(&it);
    QVERIFY2(it.current->object.type == INTEGER && *(static_cast<int*>(it.current->object.payload)) == 5, "Object has been incorrectly assigned");
    lnext(&it);
    QVERIFY2(it.current->object.type == DECIMAL && areDecimalNumbersEqual(*(static_cast<double*>(it.current->object.payload)), 1.25), "Object has been incorrectly assigned");

    Object* removedObject = static_cast<Object*>(detachContentFromListElement(getListElementAtIndex(list, 2)));
    QVERIFY2(getListElementAtIndex(list, 2)->object.type == -1 &&
             getListElementAtIndex(list, 2)->object.payload == nullptr &&
             removedObject->type == INTEGER &&
             (*(static_cast<int*>(removedObject->payload)) == 5), "Incorrect object removal from list element");

    deleteObjectPayload(removedObject);
    free(removedObject);
    removedObject = nullptr;
    deleteList(list, deleteObjectPayload);
    list = nullptr;
}

void LinkedListTests::testIsElementContained()
{
    ListElement* firstElement = createListElement();
    firstElement->priority = 10;

    ListElement* secondElement = createListElement();
    secondElement->priority = 10;

    List* list = createEmptyList();
    createAndAppendToList(list, 7);
    appendToList(list, firstElement);
    createAndAppendToList(list, 5);
    createAndPrependToList(list, 4);

    QVERIFY2(isListElementContained(firstElement, list), "Element is marked as not contained in the list (should be");
    QVERIFY2(!isListElementContained(secondElement, list), "Element is marked as contained in the list (should NOT be");

    deleteList(list, deleteObjectPayload);
    free(secondElement);
}

void LinkedListTests::testIsSortedAscendingByPriority()
{
    {
        const size_t prioritiesArray[6]{6, 2, 5, 1, 2, 3};
        List* list = createListFromPrioritiesArray(prioritiesArray, 6);

        QVERIFY2(!isSortedAscendingByPriority(list), "The list is incorrectly marked as sorted ascending by priority");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        const size_t prioritiesArray[6]{1, 2, 2, 3, 5, 6};
        List* list = createListFromPrioritiesArray(prioritiesArray, 6);

        QVERIFY2(isSortedAscendingByPriority(list), "The list is incorrectly marked as sorted ascending by priority");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        const size_t prioritiesArray[6]{5, 5, 5, 5, 5, 5};
        List* list = createListFromPrioritiesArray(prioritiesArray, 6);

        QVERIFY2(isSortedAscendingByPriority(list), "The list is incorrectly marked as sorted ascending by priority");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }
}

void LinkedListTests::testIsSortedDescendingByPriority()
{
    {
        const size_t prioritiesArray[6]{6, 2, 5, 1, 2, 3};
        List* list = createListFromPrioritiesArray(prioritiesArray, 6);
        QVERIFY2(!isSortedDescendingByPriority(list), "The list is incorrectly marked as sorted descending by priority");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        const size_t prioritiesArray[6]{6, 5, 4, 2, 2, 1};
        List* list = createListFromPrioritiesArray(prioritiesArray, 6);
        QVERIFY2(isSortedDescendingByPriority(list), "The list is incorrectly marked as sorted descending by priority");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        const size_t prioritiesArray[6]{5, 5, 5, 5, 5, 5};
        List* list = createListFromPrioritiesArray(prioritiesArray, 6);
        QVERIFY2(isSortedDescendingByPriority(list), "The list is incorrectly marked as sorted descending by priority");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }
}

void LinkedListTests::testGetPreviousElement()
{
    {
        const size_t prioritiesArray[3]{6, 2, 5};
        List* list = createListFromPrioritiesArray(prioritiesArray, 3);

        ListIterator it = lbegin(list);
        QVERIFY2(getPreviousListElement(it) == nullptr, "Previous list element is not correctly determined");

        lnext(&it);
        QVERIFY2(getPreviousListElement(it)->priority == 6, "Previous list element is not correctly determined");

        lnext(&it);
        QVERIFY2(getPreviousListElement(it)->priority == 2, "Previous list element is not correctly determined");

        lnext(&it);
        QVERIFY2(getPreviousListElement(it)->priority == 5, "Previous list element is not correctly determined");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createEmptyList();
        ListIterator it = lbegin(list);

        QVERIFY2(getPreviousListElement(it) == nullptr, "Previous list element is not correctly determined");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }
}

void LinkedListTests::testGetLastElement()
{
    {
        const size_t prioritiesArray[4]{6, 2, 5, 9};
        List* list = createListFromPrioritiesArray(prioritiesArray, 4);

        QVERIFY2(getLastListElement(list)->priority == 9, "The last list element is not correctly retrieved");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }

    {
        List* list = createEmptyList();

        QVERIFY2(getLastListElement(list) == nullptr, "The last list element is not correctly retrieved");

        deleteList(list, deleteObjectPayload);
        list = nullptr;
    }
}

void LinkedListTests::testMoveListToArray()
{
    const size_t prioritiesArray[4]{6, 2, 5, 9};
    List* list = createListFromPrioritiesArray(prioritiesArray, 4);
    size_t arraySize;
    ListElement** array = moveListToArray(list, &arraySize);

    QVERIFY2(getListSize(list) == 0 &&
             arraySize == 4 &&
             array[0]->next == nullptr && array[0]->priority == 6 &&
             array[1]->next == nullptr && array[1]->priority == 2 &&
             array[2]->next == nullptr && array[2]->priority == 5 &&
             array[3]->next == nullptr && array[3]->priority == 9 ,  "The list content has been incorrectly moved to array");

    deleteList(list, deleteObjectPayload);
    list = nullptr;
    for (size_t index = 0; index < arraySize; ++index)
    {
        free(array[index]);
        array[index] = nullptr;
    }
    free(array);
    array = nullptr;
}

void LinkedListTests::testMoveArrayToList()
{
    List* list = createEmptyList();
    ListElement** array = static_cast<ListElement**>(calloc(4, sizeof(ListElement*)));

    array[0] = createListElement();
    array[1] = createListElement();
    array[2] = createListElement();
    array[3] = createListElement();
    array[0]->priority = 6;
    array[1]->priority = 2;
    array[2]->priority = 5;
    array[3]->priority = 9;

    moveArrayToList(array, 4, list);

    QVERIFY2(array[0] == nullptr &&
             array[1] == nullptr &&
             array[2] == nullptr &&
             array[3] == nullptr &&
             getListSize(list) == 4 &&
             getListElementAtIndex(list, 0)->priority == 6 &&
             getListElementAtIndex(list, 1)->priority == 2 &&
             getListElementAtIndex(list, 2)->priority == 5 &&
             getListElementAtIndex(list, 3)->priority == 9, "The array content has been incorrectly moved to list");

    free(array);
    array = nullptr;
    deleteList(list, deleteObjectPayload);
    list = nullptr;
}

void LinkedListTests::testPrintListElementsToFile()
{
    const size_t prioritiesArray[5]{3, 2, 5, 9, 4};
    List* list = createListFromPrioritiesArray(prioritiesArray, 5);

    const char* testDataFile = "/tmp/test.txt";

    Point* point = static_cast<Point*>(malloc(sizeof(Point)));
    point->x = 5;
    point->y = 7;
    assignObjectContentToListElement(getListElementAtIndex(list, 0), POINT, static_cast<void*>(point));
    point = nullptr;

    int* distance = static_cast<int*>(malloc(sizeof(int)));
    *distance = 4;
    assignObjectContentToListElement(getListElementAtIndex(list, 2), INTEGER, static_cast<void*>(distance));
    distance = nullptr;

    double* angle = static_cast<double*>(malloc(sizeof(double)));
    *angle = 1.44;
    assignObjectContentToListElement(getListElementAtIndex(list, 3), DECIMAL, static_cast<void*>(angle));
    angle = nullptr;

    printListContentToFile(list, testDataFile, "");

    char* buffer = static_cast<char*>(malloc(100));
    FILE* readTestData = fopen(testDataFile, "r");

    fgets(buffer, 100, readTestData);
    QVERIFY2(strcmp(buffer, "Element: 0\tPriority: 3\tHas empty Object: no\tObject type: point\n") == 0, "First element is not printed correctly");
    fgets(buffer, 100, readTestData);
    QVERIFY2(strcmp(buffer, "Element: 1\tPriority: 2\tHas empty Object: yes\n") == 0, "Second element is not printed correctly");
    fgets(buffer, 100, readTestData);
    QVERIFY2(strcmp(buffer, "Element: 2\tPriority: 5\tHas empty Object: no\tObject type: integer\n") == 0, "Third element is not printed correctly");
    fgets(buffer, 100, readTestData);
    QVERIFY2(strcmp(buffer, "Element: 3\tPriority: 9\tHas empty Object: no\tObject type: decimal\n") == 0, "Fourth element is not printed correctly");
    fgets(buffer, 100, readTestData);
    QVERIFY2(strcmp(buffer, "Element: 4\tPriority: 4\tHas empty Object: yes"), "Fifth element is not printed correctly");

    fclose(readTestData);
    readTestData = nullptr;
    free(buffer);
    buffer = nullptr;
    deleteList(list, deleteObjectPayload);
    list = nullptr;
}

size_t LinkedListTests::_getSumOfPriorities(List *list)
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

QTEST_APPLESS_MAIN(LinkedListTests)

#include "tst_linkedlisttests.moc"
