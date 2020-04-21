#include <QtTest>

#include <cstring>

#include "../../LinkedListsLib/linkedlist.h"
#include "../../LinkedListsLib/sort.h"
#include "../../Utils/codeutils.h"

void customDeleteObject(Object* object); // custom C-Style object deallocator
int customCopyObject(const ListElement* source, ListElement* destination); // custom C-Style copy Object function

class LinkedListTests : public QObject
{
    Q_OBJECT

public:
    LinkedListTests();

    List* createLinkedList(std::initializer_list<size_t> priorities);
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
    void testReverseList();
    void testSortAscendingByPriority();
    void testSortAscendingByPriorityUsingRandomAccess();
    void testIterators();
    void testAssignRemoveObject();
    void testIsElementContained();
    void testIsSortedAscendingByPriority();
    void testGetLastElement();
    void testMoveListToArray();
    void testMoveArrayToList();
private:
    size_t _getSumOfPriorities(List* list);
};

LinkedListTests::LinkedListTests()
{
}

List* LinkedListTests::createLinkedList(std::initializer_list<size_t> priorities)
{
    List* list = createList();

    if (list != nullptr)
    {
        for (auto priority : priorities)
        {
            createAndAppendToList(list, priority);
        }
    }
    else
    {
        qWarning("The list could not be created");
    }

    return list;
}

LinkedListTests::~LinkedListTests()
{
}

void LinkedListTests::testListIsCorrectlyCreatedAndCleared()
{
    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4, 3, 1, 2, 9, 7});

        QVERIFY2(getListSize(list) == 9, "List size is not correct");
        QVERIFY2(getElementAtIndex(list, 0)->priority == 6 && getElementAtIndex(list, 4)->priority == 3 && getElementAtIndex(list, 8)->priority == 7,
                 "The element is not correctly retrieved based on index");

        clearList(list, deleteObject);
        QVERIFY2(getListSize(list) == 0, "The list has not been correctly emptied");

        deleteList(list, deleteObject);
        list = nullptr;
    }
}

void LinkedListTests::testAppendElement()
{
    List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4, 3, 1, 2, 9, 7});
    createAndAppendToList(list, 8);

    QVERIFY2(getListSize(list) == 10 && getElementAtIndex(list, 0)->priority == 6 && getElementAtIndex(list, 9)->priority == 8, "Element has not been correctly created and appended");

    deleteList(list, deleteObject);
    list = nullptr;
}

void LinkedListTests::testPrependElement()
{
    List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4, 3, 1, 2, 9, 7});
    createAndPrependToList(list, 8);

    QVERIFY2(getListSize(list) == 10 && getElementAtIndex(list, 0)->priority == 8 && getElementAtIndex(list, 9)->priority == 7, "Element has not been correctly created and prepended");

    deleteList(list, deleteObject);
    list = nullptr;
}

void LinkedListTests::testRemoveFirstElement()
{
    List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4, 3, 1, 2, 9, 8});
    ListElement* firstElement = removeFirstListElement(list);

    QVERIFY2(getListSize(list) == 8 && getElementAtIndex(list, 0)->priority == 2 && getElementAtIndex(list, 7)->priority == 8, "Element has not been correctly created and appended");
    QVERIFY(firstElement->next == nullptr && firstElement->priority == 6);

    free(firstElement);
    deleteList(list, deleteObject);
    list = nullptr;
}

void LinkedListTests::testRemoveLastElement()
{
    List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4, 3, 1, 2, 9, 8});
    ListElement* lastElement = removeLastListElement(list);

    QVERIFY2(getListSize(list) == 8 && getElementAtIndex(list, 0)->priority == 6 && getElementAtIndex(list, 7)->priority == 9, "Element has not been correctly created and appended");
    QVERIFY(lastElement->next == nullptr && lastElement->priority == 8);

    free(lastElement);
    deleteList(list, deleteObject);
    list = nullptr;
}

void LinkedListTests::testInsertElementBefore()
{
    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4});

        ListIterator it = lbegin(list);
        createAndInsertBefore(it, 10);

        QVERIFY2(getListSize(list) == 5 &&
                 getElementAtIndex(list, 0)->priority == 10 &&
                 getElementAtIndex(list, 1)->priority == 6 &&
                 getElementAtIndex(list, 2)->priority == 2 &&
                 getElementAtIndex(list, 3)->priority == 5 &&
                 getElementAtIndex(list, 4)->priority == 4,   "The previous element has not been correctly inserted");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4});

        ListIterator it = lbegin(list);
        lnext(&it);
        createAndInsertBefore(it, 10);

        QVERIFY2(getListSize(list) == 5 &&
                 getElementAtIndex(list, 0)->priority == 6 &&
                 getElementAtIndex(list, 1)->priority == 10 &&
                 getElementAtIndex(list, 2)->priority == 2 &&
                 getElementAtIndex(list, 3)->priority == 5 &&
                 getElementAtIndex(list, 4)->priority == 4,   "The previous element has not been correctly inserted");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4});

        ListIterator it = lbegin(list);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        createAndInsertBefore(it, 10);

        QVERIFY2(getListSize(list) == 5 &&
                 getElementAtIndex(list, 0)->priority == 6 &&
                 getElementAtIndex(list, 1)->priority == 2 &&
                 getElementAtIndex(list, 2)->priority == 5 &&
                 getElementAtIndex(list, 3)->priority == 10 &&
                 getElementAtIndex(list, 4)->priority == 4,   "The previous element has not been correctly inserted");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4});

        ListIterator it = lbegin(list);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        createAndInsertBefore(it, 10);

        QVERIFY2(getListSize(list) == 5 &&
                 getElementAtIndex(list, 0)->priority == 6 &&
                 getElementAtIndex(list, 1)->priority == 2 &&
                 getElementAtIndex(list, 2)->priority == 5 &&
                 getElementAtIndex(list, 3)->priority == 4 &&
                 getElementAtIndex(list, 4)->priority == 10,   "The previous element has not been correctly inserted");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createList();
        ListIterator it = lbegin(list);
        createAndInsertBefore(it, 10);

        QVERIFY2(getListSize(list) == 1 && getElementAtIndex(list, 0)->priority == 10, "The element has not been correctly inserted into an empty list");

        deleteList(list, deleteObject);
        list = nullptr;
    }
}

void LinkedListTests::testInsertElementAfter()
{
    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4});

        ListIterator it = lbegin(list);
        createAndInsertAfter(it, 10);

        QVERIFY2(getListSize(list) == 5 &&
                 getElementAtIndex(list, 0)->priority == 6 &&
                 getElementAtIndex(list, 1)->priority == 10 &&
                 getElementAtIndex(list, 2)->priority == 2 &&
                 getElementAtIndex(list, 3)->priority == 5 &&
                 getElementAtIndex(list, 4)->priority == 4,   "The next element has not been correctly inserted");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4});

        ListIterator it = lbegin(list);
        lnext(&it);
        createAndInsertAfter(it, 10);

        QVERIFY2(getListSize(list) == 5 &&
                 getElementAtIndex(list, 0)->priority == 6 &&
                 getElementAtIndex(list, 1)->priority == 2 &&
                 getElementAtIndex(list, 2)->priority == 10 &&
                 getElementAtIndex(list, 3)->priority == 5 &&
                 getElementAtIndex(list, 4)->priority == 4,   "The next element has not been correctly inserted");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4});

        ListIterator it = lbegin(list);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        createAndInsertAfter(it, 10);

        QVERIFY2(getListSize(list) == 5 &&
                 getElementAtIndex(list, 0)->priority == 6 &&
                 getElementAtIndex(list, 1)->priority == 2 &&
                 getElementAtIndex(list, 2)->priority == 5 &&
                 getElementAtIndex(list, 3)->priority == 4 &&
                 getElementAtIndex(list, 4)->priority == 10,   "The next element has not been correctly inserted");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createList();
        ListIterator it = lbegin(list);
        createAndInsertAfter(it, 10);

        QVERIFY2(getListSize(list) == 1 && getElementAtIndex(list, 0)->priority == 10, "The element has not been correctly inserted into an empty list");

        deleteList(list, deleteObject);
        list = nullptr;
    }
}

void LinkedListTests::testMoveContentToList()
{
    {
        List* source = createLinkedList(std::initializer_list<size_t>{6, 2});
        List* destination = createLinkedList(std::initializer_list<size_t>{7, 4});

        moveContentToList(source, destination);

        QVERIFY2(getListSize(source) == 0 &&
                 getListSize(destination) == 4 &&
                 getElementAtIndex(destination, 0)->priority == 7 &&
                 getElementAtIndex(destination, 1)->priority == 4 &&
                 getElementAtIndex(destination, 2)->priority == 6 &&
                 getElementAtIndex(destination, 3)->priority == 2,   "The source list content has not been correctly moved to destination");

        deleteList(source, deleteObject);
        source = nullptr;
        deleteList(destination, deleteObject);
        destination = nullptr;
    }

    {
        List* source = createList();
        List* destination = createLinkedList(std::initializer_list<size_t>{7, 4});

        moveContentToList(source, destination);

        QVERIFY2(getListSize(source) == 0 &&
                 getListSize(destination) == 2 &&
                 getElementAtIndex(destination, 0)->priority == 7 &&
                 getElementAtIndex(destination, 1)->priority == 4,   "The source list content has not been correctly moved to destination");

        deleteList(source, deleteObject);
        source = nullptr;
        deleteList(destination, deleteObject);
        destination = nullptr;
    }

    {
        List* source = createLinkedList(std::initializer_list<size_t>{6, 2});
        List* destination = createList();

        moveContentToList(source, destination);

        QVERIFY2(getListSize(source) == 0 &&
                 getListSize(destination) == 2 &&
                 getElementAtIndex(destination, 0)->priority == 6 &&
                 getElementAtIndex(destination, 1)->priority == 2,   "The source list content has not been correctly moved to destination");

        deleteList(source, deleteObject);
        source = nullptr;
        deleteList(destination, deleteObject);
        destination = nullptr;
    }
}

void LinkedListTests::testCopyContentToList()
{
    {
        List* source = createLinkedList(std::initializer_list<size_t>{6, 2});
        List* destination = createLinkedList(std::initializer_list<size_t>{7, 4});

        copyContentToList(source, destination, copyObject, deleteObject);

        QVERIFY2(getListSize(source) == 2 &&
                 getListSize(destination) == 4 &&
                 getElementAtIndex(destination, 0)->priority == 7 &&
                 getElementAtIndex(destination, 1)->priority == 4 &&
                 getElementAtIndex(destination, 2)->priority == 6 &&
                 getElementAtIndex(destination, 3)->priority == 2,   "The source list content has not been correctly copied to destination");

        deleteList(source, deleteObject);
        source = nullptr;
        deleteList(destination, deleteObject);
        destination = nullptr;
    }

    {
        List* source = createList();
        List* destination = createLinkedList(std::initializer_list<size_t>{7, 4});

        copyContentToList(source, destination, copyObject, deleteObject);

        QVERIFY2(getListSize(source) == 0 &&
                 getListSize(destination) == 2 &&
                 getElementAtIndex(destination, 0)->priority == 7 &&
                 getElementAtIndex(destination, 1)->priority == 4,   "The source list content has not been correctly copied to destination");

        deleteList(source, deleteObject);
        source = nullptr;
        deleteList(destination, deleteObject);
        destination = nullptr;
    }

    {
        List* source = createLinkedList(std::initializer_list<size_t>{6, 2});
        List* destination = createList();

        copyContentToList(source, destination, copyObject, deleteObject);

        QVERIFY2(getListSize(source) == 2 &&
                 getListSize(destination) == 2 &&
                 getElementAtIndex(destination, 0)->priority == 6 &&
                 getElementAtIndex(destination, 1)->priority == 2,   "The source list content has not been correctly copied to destination");

        deleteList(source, deleteObject);
        source = nullptr;
        deleteList(destination, deleteObject);
        destination = nullptr;
    }

    {
        List* source = createLinkedList(std::initializer_list<size_t>{6, 2});
        List* destination = createLinkedList(std::initializer_list<size_t>{5, 4, 3});

        ListIterator it = lbegin(source);
        assignObjectToListElement(it.current, "Segment", createSegmentPayload(2, 5, 4, 11));
        lnext(&it);
        assignObjectToListElement(it.current, "LocalConditions", createLocalConditionsPayload(7, -5, 10, 12.8));

        copyContentToList(source, destination, customCopyObject, customDeleteObject);

        const Segment* copiedSegment = static_cast<Segment*>(getElementAtIndex(destination, 3)->object->payload);
        const LocalConditions* copiedConditions = static_cast<LocalConditions*>(getElementAtIndex(destination, 4)->object->payload);

        QVERIFY2(getListSize(source) == 2 &&
                 getListSize(destination) == 5 &&
                 getElementAtIndex(destination, 0)->priority == 5 &&
                 getElementAtIndex(destination, 0)->object == nullptr &&
                 getElementAtIndex(destination, 1)->priority == 4 &&
                 getElementAtIndex(destination, 1)->object == nullptr &&
                 getElementAtIndex(destination, 2)->priority == 3 &&
                 getElementAtIndex(destination, 2)->object == nullptr &&
                 getElementAtIndex(destination, 3)->priority == 6 &&
                 strcmp(getElementAtIndex(destination, 3)->object->type, "Segment") == 0 &&
                 copiedSegment->start->x == 2 && copiedSegment->start->y == 5 && copiedSegment->stop->x == 4 && copiedSegment->stop->y == 11 &&
                 getElementAtIndex(destination, 4)->priority == 2 &&
                 strcmp(getElementAtIndex(destination, 4)->object->type, "LocalConditions") == 0 &&
                 copiedConditions->position->x == 7 && copiedConditions->position->y == -5 && copiedConditions->temperature == 10 && copiedConditions->humidity == 12.8,
                 "The source list content has not been correctly copied to destination");

        deleteList(source, customDeleteObject);
        source = nullptr;
        deleteList(destination, customDeleteObject);
        destination = nullptr;
    }
}

void LinkedListTests::testRemoveElementBefore()
{
    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4});

        ListIterator it = lbegin(list);
        ListElement* removedElement = removePreviousListElement(it);

        QVERIFY2(getListSize(list) == 4 &&
                 removedElement == nullptr &&
                 getElementAtIndex(list, 0)->priority == 6 &&
                 getElementAtIndex(list, 1)->priority == 2 &&
                 getElementAtIndex(list, 2)->priority == 5 &&
                 getElementAtIndex(list, 3)->priority == 4,   "The previous element removal does not work correctly");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4});

        ListIterator it = lbegin(list);
        lnext(&it);
        ListElement* removedElement = removePreviousListElement(it);

        QVERIFY2(getListSize(list) == 3 &&
                 removedElement->priority == 6 &&
                 getElementAtIndex(list, 0)->priority == 2 &&
                 getElementAtIndex(list, 1)->priority == 5 &&
                 getElementAtIndex(list, 2)->priority == 4,   "The previous element has not been correctly removed");

        free(removedElement);
        removedElement = nullptr;
        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4});

        ListIterator it = lbegin(list);
        lnext(&it);
        lnext(&it);
        ListElement* removedElement = removePreviousListElement(it);

        QVERIFY2(getListSize(list) == 3 &&
                 removedElement->priority == 2 &&
                 getElementAtIndex(list, 0)->priority == 6 &&
                 getElementAtIndex(list, 1)->priority == 5 &&
                 getElementAtIndex(list, 2)->priority == 4,   "The previous element has not been correctly removed");

        free(removedElement);
        removedElement = nullptr;
        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4});

        ListIterator it = lbegin(list);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        ListElement* removedElement = removePreviousListElement(it);

        QVERIFY2(getListSize(list) == 3 &&
                 removedElement->priority == 5 &&
                 getElementAtIndex(list, 0)->priority == 6 &&
                 getElementAtIndex(list, 1)->priority == 2 &&
                 getElementAtIndex(list, 2)->priority == 4,   "The previous element has not been correctly removed");

        free(removedElement);
        removedElement = nullptr;
        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4});

        ListIterator it = lbegin(list);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        ListElement* removedElement = removePreviousListElement(it);

        QVERIFY2(getListSize(list) == 3 &&
                 removedElement->priority == 4 &&
                 getElementAtIndex(list, 0)->priority == 6 &&
                 getElementAtIndex(list, 1)->priority == 2 &&
                 getElementAtIndex(list, 2)->priority == 5,   "The previous element has not been correctly removed");

        free(removedElement);
        removedElement = nullptr;
        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createList();
        createAndAppendToList(list, 5);
        ListIterator it = lbegin(list);
        ListElement* removedElement = removePreviousListElement(it);

        QVERIFY2(getListSize(list) == 1 && removedElement == nullptr && getElementAtIndex(list, 0)->priority == 5, "The previous element removal does not work correctly");

        lnext(&it);
        removedElement = removePreviousListElement(it);

        QVERIFY2(getListSize(list) == 0 && removedElement->priority == 5, "The only existing list element has not been correctly removed");

        free(removedElement);
        removedElement = nullptr;
        deleteList(list, deleteObject);
        list = nullptr;
    }
}

void LinkedListTests::testRemoveElementAfter()
{
    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4});

        ListIterator it = lbegin(list);
        ListElement* removedElement = removeNextListElement(it);

        QVERIFY2(getListSize(list) == 3 &&
                 removedElement->priority == 2 &&
                 getElementAtIndex(list, 0)->priority == 6 &&
                 getElementAtIndex(list, 1)->priority == 5 &&
                 getElementAtIndex(list, 2)->priority == 4,   "The next element has not been correctly removed");

        free(removedElement);
        removedElement = nullptr;
        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4});

        ListIterator it = lbegin(list);
        lnext(&it);
        ListElement* removedElement = removeNextListElement(it);

        QVERIFY2(getListSize(list) == 3 &&
                 removedElement->priority == 5 &&
                 getElementAtIndex(list, 0)->priority == 6 &&
                 getElementAtIndex(list, 1)->priority == 2 &&
                 getElementAtIndex(list, 2)->priority == 4,   "The next element has not been correctly removed");

        free(removedElement);
        removedElement = nullptr;
        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4});

        ListIterator it = lbegin(list);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        ListElement* removedElement = removeNextListElement(it);

        QVERIFY2(getListSize(list) == 4 &&
                 removedElement == nullptr &&
                 getElementAtIndex(list, 0)->priority == 6 &&
                 getElementAtIndex(list, 1)->priority == 2 &&
                 getElementAtIndex(list, 2)->priority == 5 &&
                 getElementAtIndex(list, 3)->priority == 4,   "The next element has not been correctly removed");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createList();
        createAndAppendToList(list, 5);
        ListIterator it = lbegin(list);
        ListElement* removedElement = removeNextListElement(it);

        QVERIFY2(getListSize(list) == 1 && removedElement == nullptr && getElementAtIndex(list, 0)->priority == 5, "The only existing list element has not been correctly removed");

        deleteList(list, deleteObject);
        list = nullptr;
    }
}

void LinkedListTests::testRemoveCurrentElement()
{
    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5});

        ListIterator it = lbegin(list);
        ListElement* removedElement = removeCurrentListElement(it);

        QVERIFY2(getListSize(list) == 2 &&
                 removedElement->priority == 6 &&
                 getElementAtIndex(list, 0)->priority == 2 &&
                 getElementAtIndex(list, 1)->priority == 5,   "The current element has not been correctly removed");

        free(removedElement);
        removedElement = nullptr;
        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5});

        ListIterator it = lbegin(list);
        lnext(&it);
        ListElement* removedElement = removeCurrentListElement(it);

        QVERIFY2(getListSize(list) == 2 &&
                 removedElement->priority == 2 &&
                 getElementAtIndex(list, 0)->priority == 6 &&
                 getElementAtIndex(list, 1)->priority == 5,   "The current element has not been correctly removed");

        free(removedElement);
        removedElement = nullptr;
        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5});

        ListIterator it = lbegin(list);
        lnext(&it);
        lnext(&it);
        ListElement* removedElement = removeCurrentListElement(it);

        QVERIFY2(getListSize(list) == 2 &&
                 removedElement->priority == 5 &&
                 getElementAtIndex(list, 0)->priority == 6 &&
                 getElementAtIndex(list, 1)->priority == 2,   "The current element has not been correctly removed");

        free(removedElement);
        removedElement = nullptr;
        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5});

        ListIterator it = lbegin(list);
        lnext(&it);
        lnext(&it);
        lnext(&it);
        ListElement* removedElement = removeCurrentListElement(it);

        QVERIFY2(getListSize(list) == 3 &&
                 removedElement == nullptr,   "The current element removal does not work correctly");

        deleteList(list, deleteObject);
        list = nullptr;
    }
}

void LinkedListTests::testReverseList()
{
    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4});
        reverseList(list);

        QVERIFY2(getListSize(list) == 4 &&
                 getElementAtIndex(list, 0)->priority == 4 &&
                 getElementAtIndex(list, 1)->priority == 5 &&
                 getElementAtIndex(list, 2)->priority == 2 &&
                 getElementAtIndex(list, 3)->priority == 6,   "The list has not been correctly reversed");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2});
        reverseList(list);

        QVERIFY2(getListSize(list) == 2 &&
                 getElementAtIndex(list, 0)->priority == 2 &&
                 getElementAtIndex(list, 1)->priority == 6,   "The list has not been correctly reversed");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createList();
        createAndAppendToList(list, 2);
        reverseList(list);

        QVERIFY2(getListSize(list) == 1 && getElementAtIndex(list, 0)->priority == 2, "The list has not been correctly reversed");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createList();
        reverseList(list);

        QVERIFY2(getListSize(list) == 0, "The list has not been correctly reversed");

        deleteList(list, deleteObject);
        list = nullptr;
    }
}

void LinkedListTests::testSortAscendingByPriority()
{
    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 1, 2, 3}); // highest prio item first
        sortAscendingByPriority(list);

        QVERIFY2(getElementAtIndex(list, 0)->priority == 1 && _getSumOfPriorities(list) == 19 && isSortedAscendingByPriority(list),
                 "The list hasn't been correctly sorted");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{3, 2, 1, 5, 2, 6}); // highest prio item last
        sortAscendingByPriority(list);

        QVERIFY2(getElementAtIndex(list, 0)->priority == 1 && _getSumOfPriorities(list) == 19 && isSortedAscendingByPriority(list),
                 "The list hasn't been correctly sorted");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{1, 6, 2, 5, 3, 2}); // lowest prio item first
        sortAscendingByPriority(list);

        QVERIFY2(getElementAtIndex(list, 0)->priority == 1 && _getSumOfPriorities(list) == 19 && isSortedAscendingByPriority(list),
                 "The list hasn't been correctly sorted");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{2, 3, 5, 2, 6, 1}); // lowest prio item last
        sortAscendingByPriority(list);

        QVERIFY2(getElementAtIndex(list, 0)->priority == 1 && _getSumOfPriorities(list) == 19 && isSortedAscendingByPriority(list),
                 "The list hasn't been correctly sorted");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{2, 3, 1, 2, 6, 5}); // random
        sortAscendingByPriority(list);

        QVERIFY2(getElementAtIndex(list, 0)->priority == 1 &&
                 getElementAtIndex(list, 1)->priority == 2 &&
                 getElementAtIndex(list, 2)->priority == 2 &&
                 getElementAtIndex(list, 3)->priority == 3 &&
                 getElementAtIndex(list, 4)->priority == 5 &&
                 getElementAtIndex(list, 5)->priority == 6, "The list hasn't been correctly sorted");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{1, 2, 2, 3, 5, 6}); // perfectly sorted ascending
        sortAscendingByPriority(list);

        QVERIFY2(getElementAtIndex(list, 0)->priority == 1 &&
                 getElementAtIndex(list, 1)->priority == 2 &&
                 getElementAtIndex(list, 2)->priority == 2 &&
                 getElementAtIndex(list, 3)->priority == 3 &&
                 getElementAtIndex(list, 4)->priority == 5 &&
                 getElementAtIndex(list, 5)->priority == 6, "The list hasn't been correctly sorted");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 5, 3, 2, 2, 1}); // perfectly sorted descending
        sortAscendingByPriority(list);

        QVERIFY2(getElementAtIndex(list, 0)->priority == 1 &&
                 getElementAtIndex(list, 1)->priority == 2 &&
                 getElementAtIndex(list, 2)->priority == 2 &&
                 getElementAtIndex(list, 3)->priority == 3 &&
                 getElementAtIndex(list, 4)->priority == 5 &&
                 getElementAtIndex(list, 5)->priority == 6, "The list hasn't been correctly sorted ");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 5});
        sortAscendingByPriority(list);

        QVERIFY2(getElementAtIndex(list, 0)->priority == 5 && getElementAtIndex(list, 1)->priority == 6, "The list hasn't been correctly sorted ");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{1});
        sortAscendingByPriority(list);

        QVERIFY2(getElementAtIndex(list, 0)->priority == 1, "The list hasn't been correctly sorted (ascending) by priority");

        deleteList(list, deleteObject);
        list = nullptr;
    }
}

void LinkedListTests::testSortAscendingByPriorityUsingRandomAccess()
{
    // insertion sort
    {
        List* list = createLinkedList(std::initializer_list<size_t>{2, 3, 5, 2, 6, 1}); // lowest prio item last
        sortByRandomAccess(list, insertionSortAscendingByPriority);

        QVERIFY2(getElementAtIndex(list, 0)->priority == 1 && _getSumOfPriorities(list) == 19 && isSortedAscendingByPriority(list),
                 "The list hasn't been correctly sorted");

        deleteList(list, deleteObject);
        list = nullptr;
    }
}

void LinkedListTests::testIterators()
{
    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 9});
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

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createList();
        ListIterator it = lbegin(list);
        QVERIFY2(areIteratorsEqual(it, lend(list)), "The list is empty but the begin and end iterators are not equal");
        lnext(&it);
        QVERIFY2(areIteratorsEqual(it, lend(list)), "The iterator is not correctly incremented");

        deleteList(list, deleteObject);
        list = nullptr;
    }
}

void LinkedListTests::testAssignRemoveObject()
{
    List* list = createList();
    // Point
    Point* point = static_cast<Point*>(malloc(sizeof(Point)));
    point->x = 3;
    point->y = 4;
    assignObjectToListElement(createAndAppendToList(list, 2), "coordinates", static_cast<void*>(point));
    point = nullptr;
    // int
    int* distance = static_cast<int*>(malloc(sizeof(int)));
    *distance = 5;
    assignObjectToListElement(createAndAppendToList(list, 3), "distance", static_cast<void*>(distance));
    distance = nullptr;
    // float
    float* angle = static_cast<float*>(malloc(sizeof(float)));
    *angle = 1.25;
    assignObjectToListElement(createAndAppendToList(list, 1), "angle", static_cast<void*>(angle));
    angle = nullptr;
    // no object
    Q_UNUSED(createAndPrependToList(list, 10));

    ListIterator it = lbegin(list);
    QVERIFY2(it.current->object == nullptr, "Default object and object type are incorrect (should be NULL)");
    lnext(&it);
    QVERIFY2(strcmp(it.current->object->type, "coordinates") == 0 && (static_cast<Point*>(it.current->object->payload))->x == 3
             && (static_cast<Point*>(it.current->object->payload))->y == 4, "Object has been incorrectly assigned");
    lnext(&it);
    QVERIFY2(strcmp(it.current->object->type, "distance") == 0 && *(static_cast<int*>(it.current->object->payload)) == 5, "Object has been incorrectly assigned");
    lnext(&it);
    QVERIFY2(strcmp(it.current->object->type, "angle") == 0 && *(static_cast<float*>(it.current->object->payload)) == 1.25f, "Object has been incorrectly assigned");

    Object* removedObject = static_cast<Object*>(removeObjectFromListElement(getElementAtIndex(list, 2)));
    QVERIFY2(getElementAtIndex(list, 2)->object == nullptr && (strcmp(removedObject->type, "distance") == 0) && (*(static_cast<int*>(removedObject->payload)) == 5),
             "Incorrect object removal from list element");

    free(removedObject);
    removedObject = nullptr;
    deleteList(list, deleteObject);
    list = nullptr;
}

void LinkedListTests::testIsElementContained()
{
    ListElement* firstElement = createListElement();
    firstElement->priority = 10;

    ListElement* secondElement = createListElement();
    secondElement->priority = 10;

    List* list = createList();
    createAndAppendToList(list, 7);
    appendToList(list, firstElement);
    createAndAppendToList(list, 5);
    createAndPrependToList(list, 4);

    QVERIFY2(isElementContained(firstElement, list), "Element is marked as not contained in the list (should be");
    QVERIFY2(!isElementContained(secondElement, list), "Element is marked as contained in the list (should NOT be");

    deleteList(list, deleteObject);
    free(secondElement);
}

void LinkedListTests::testIsSortedAscendingByPriority()
{
    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 1, 2, 3}); // highest prio item first
        QVERIFY2(!isSortedAscendingByPriority(list), "The list is incorrectly marked as sorted ascending by priority");
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{1, 2, 2, 3, 5, 6}); // highest prio item first
        QVERIFY2(isSortedAscendingByPriority(list), "The list is incorrectly marked as sorted ascending by priority");
    }
}

void LinkedListTests::testGetLastElement()
{
    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 9});

        QVERIFY2(getLastElement(list)->priority == 9, "The last list element is not correctly retrieved");

        deleteList(list, deleteObject);
        list = nullptr;
    }

    {
        List* list = createList();

        QVERIFY2(getLastElement(list) == nullptr, "The last list element is not correctly retrieved");

        deleteList(list, deleteObject);
        list = nullptr;
    }
}

void LinkedListTests::testMoveListToArray()
{
    List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 9});
    size_t arraySize;
    ListElement** array = moveListToArray(list, &arraySize);

    QVERIFY2(getListSize(list) == 0 &&
             arraySize == 4 &&
             array[0]->next == nullptr && array[0]->priority == 6 &&
             array[1]->next == nullptr && array[1]->priority == 2 &&
             array[2]->next == nullptr && array[2]->priority == 5 &&
             array[3]->next == nullptr && array[3]->priority == 9 ,  "The list content has been incorrectly moved to array");

    deleteList(list, deleteObject);
    list = nullptr;
    free(array);
    array = nullptr;
}

void LinkedListTests::testMoveArrayToList()
{
    List* list = createList();
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
             getElementAtIndex(list, 0)->priority == 6 &&
             getElementAtIndex(list, 1)->priority == 2 &&
             getElementAtIndex(list, 2)->priority == 5 &&
             getElementAtIndex(list, 3)->priority == 9, "The array content has been incorrectly moved to list");

    free(array);
    array = nullptr;
    deleteList(list, deleteObject);
    list = nullptr;
}

size_t LinkedListTests::_getSumOfPriorities(List *list)
{
    size_t sum = 0;

    if (list != nullptr)
    {
        for (size_t index{0}; index < getListSize(list); ++index)
        {
            sum = sum + getElementAtIndex(list, index)->priority;
        }
    }

    return sum;
}

QTEST_APPLESS_MAIN(LinkedListTests)

#include "tst_linkedlisttests.moc"
