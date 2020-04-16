#include <QtTest>

#include <cstring>

#include "../../LinkedListsLib/linkedlist.h"
#include "../../Utils/codeutils.h"

class LinkedListTests : public QObject
{
    Q_OBJECT

public:
    LinkedListTests();

    List* createLinkedList(std::initializer_list<size_t> priorities);
    ~LinkedListTests();

private slots:
    void testListIsCorrectlyCreatedAndCleared();
    void testBasicListOperations();
    void testReverseList();
    void testSortAscendingByPriority();
    void testIterators();
    void testAssignRemoveObject();
    void testIsElementContained();

private:
    void _freeSimpleListObjects(List* list); // to be used only with objects with simple payloads that don't contain pointers to other data structures
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
        QVERIFY2(getElementAtIndex(list, 0)->priority == 6 &&
                 getElementAtIndex(list, 4)->priority == 3 &&
                 getElementAtIndex(list, 8)->priority == 7, "The element is not correctly retrieved based on index");

        clearList(list);
        QVERIFY2(getListSize(list) == 0, "The list has not been correctly emptied");

        deleteList(list);
        list = nullptr;
    }
}

void LinkedListTests::testBasicListOperations()
{
    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4, 3, 1, 2, 9, 7});
        createAndAppendToList(list, 8);

        QVERIFY2(getListSize(list) == 10 && getElementAtIndex(list, 0)->priority == 6 && getElementAtIndex(list, 9)->priority == 8, "Element has not been correctly created and appended");

        deleteList(list);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4, 3, 1, 2, 9, 7});
        createAndPrependToList(list, 8);

        QVERIFY2(getListSize(list) == 10 && getElementAtIndex(list, 0)->priority == 8 && getElementAtIndex(list, 9)->priority == 7, "Element has not been correctly created and prepended");

        deleteList(list);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4, 3, 1, 2, 9, 8});
        ListElement* firstElement = removeFirstListElement(list);

        QVERIFY2(getListSize(list) == 8 && getElementAtIndex(list, 0)->priority == 2 && getElementAtIndex(list, 7)->priority == 8, "Element has not been correctly created and appended");
        QVERIFY(firstElement->next == nullptr && firstElement->priority == 6);

        free(firstElement);
        deleteList(list);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4, 3, 1, 2, 9, 8});
        ListElement* lastElement = removeLastListElement(list);

        QVERIFY2(getListSize(list) == 8 && getElementAtIndex(list, 0)->priority == 6 && getElementAtIndex(list, 7)->priority == 9, "Element has not been correctly created and appended");
        QVERIFY(lastElement->next == nullptr && lastElement->priority == 8);

        free(lastElement);
        deleteList(list);
        list = nullptr;
    }
}

void LinkedListTests::testReverseList()
{
    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4, 3, 1, 2, 9, 7});
        reverseList(list);

        QVERIFY2(getListSize(list) == 9 &&
                 getElementAtIndex(list, 0)->priority == 7 &&
                 getElementAtIndex(list, 1)->priority == 9 &&
                 getElementAtIndex(list, 2)->priority == 2 &&
                 getElementAtIndex(list, 3)->priority == 1 &&
                 getElementAtIndex(list, 4)->priority == 3 &&
                 getElementAtIndex(list, 5)->priority == 4 &&
                 getElementAtIndex(list, 6)->priority == 5 &&
                 getElementAtIndex(list, 7)->priority == 2 &&
                 getElementAtIndex(list, 8)->priority == 6,   "The list has not been correctly reversed");

        deleteList(list);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2});
        reverseList(list);

        QVERIFY2(getListSize(list) == 2 &&
                 getElementAtIndex(list, 0)->priority == 2 &&
                 getElementAtIndex(list, 1)->priority == 6,   "The list has not been correctly reversed");

        deleteList(list);
        list = nullptr;
    }

    {
        List* list = createList();
        createAndAppendToList(list, 2);
        reverseList(list);

        QVERIFY2(getListSize(list) == 1 && getElementAtIndex(list, 0)->priority == 2, "The list has not been correctly reversed");

        deleteList(list);
        list = nullptr;
    }

    {
        List* list = createList();
        reverseList(list);

        QVERIFY2(getListSize(list) == 0, "The list has not been correctly reversed");

        deleteList(list);
        list = nullptr;
    }
}

void LinkedListTests::testSortAscendingByPriority()
{
    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 1, 2, 3}); // highest prio item first
        sortAscendingByPriority(list);

        QVERIFY2(getElementAtIndex(list, 0)->priority == 1 &&
                 getElementAtIndex(list, 1)->priority == 2 &&
                 getElementAtIndex(list, 2)->priority == 2 &&
                 getElementAtIndex(list, 3)->priority == 3 &&
                 getElementAtIndex(list, 4)->priority == 5 &&
                 getElementAtIndex(list, 5)->priority == 6, "The list hasn't been correctly sorted");

        deleteList(list);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{3, 2, 1, 5, 2, 6}); // highest prio item last
        sortAscendingByPriority(list);

        QVERIFY2(getElementAtIndex(list, 0)->priority == 1 &&
                 getElementAtIndex(list, 1)->priority == 2 &&
                 getElementAtIndex(list, 2)->priority == 2 &&
                 getElementAtIndex(list, 3)->priority == 3 &&
                 getElementAtIndex(list, 4)->priority == 5 &&
                 getElementAtIndex(list, 5)->priority == 6, "The list hasn't been correctly sorted");

        deleteList(list);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{1, 6, 2, 5, 3, 2}); // lowest prio item first
        sortAscendingByPriority(list);

        QVERIFY2(getElementAtIndex(list, 0)->priority == 1 &&
                 getElementAtIndex(list, 1)->priority == 2 &&
                 getElementAtIndex(list, 2)->priority == 2 &&
                 getElementAtIndex(list, 3)->priority == 3 &&
                 getElementAtIndex(list, 4)->priority == 5 &&
                 getElementAtIndex(list, 5)->priority == 6, "The list hasn't been correctly sorted");

        deleteList(list);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{2, 3, 5, 2, 6, 1}); // lowest prio item last
        sortAscendingByPriority(list);

        QVERIFY2(getElementAtIndex(list, 0)->priority == 1 &&
                 getElementAtIndex(list, 1)->priority == 2 &&
                 getElementAtIndex(list, 2)->priority == 2 &&
                 getElementAtIndex(list, 3)->priority == 3 &&
                 getElementAtIndex(list, 4)->priority == 5 &&
                 getElementAtIndex(list, 5)->priority == 6, "The list hasn't been correctly sorted");

        deleteList(list);
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

        deleteList(list);
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

        deleteList(list);
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

        deleteList(list);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 5});
        sortAscendingByPriority(list);

        QVERIFY2(getElementAtIndex(list, 0)->priority == 5 && getElementAtIndex(list, 1)->priority == 6, "The list hasn't been correctly sorted ");

        deleteList(list);
        list = nullptr;
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{1});
        sortAscendingByPriority(list);

        QVERIFY2(getElementAtIndex(list, 0)->priority == 1, "The list hasn't been correctly sorted (ascending) by priority");

        deleteList(list);
        list = nullptr;
    }
}

void LinkedListTests::testIterators()
{
    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 9, 8});
        ListIterator it = lbegin(list);
        QVERIFY2(!areIteratorsEqual(it, lend(list)) && it.current->priority == 6, "The begin iterator is not correctly generated");
        lnext(&it);
        QVERIFY2(!areIteratorsEqual(it, lend(list)) && it.current->priority == 2, "The iterator is not correctly incremented");
        lnext(&it);
        QVERIFY2(!areIteratorsEqual(it, lend(list)) && it.current->priority == 5, "The iterator is not correctly incremented");
        lnext(&it);
        QVERIFY2(!areIteratorsEqual(it, lend(list)) && it.current->priority == 9, "The iterator is not correctly incremented");
        lnext(&it);
        QVERIFY2(!areIteratorsEqual(it, lend(list)) && it.current->priority == 8, "The iterator is not correctly incremented");
        lnext(&it);
        QVERIFY2(areIteratorsEqual(it, lend(list)), "The iterator is not correctly incremented");
        lnext(&it);
        QVERIFY2(areIteratorsEqual(it, lend(list)), "The iterator is not correctly incremented");

        deleteList(list);
        list = nullptr;
    }

    {
        List* list = createList();
        ListIterator it = lbegin(list);
        QVERIFY2(areIteratorsEqual(it, lend(list)), "The list is empty but the begin and end iterators are not equal");
        lnext(&it);
        QVERIFY2(areIteratorsEqual(it, lend(list)), "The iterator is not correctly incremented");

        deleteList(list);
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
    _freeSimpleListObjects(list); // for remaining objects that should be de-allocated
    deleteList(list);
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

    deleteList(list);
    free(secondElement);
}

void LinkedListTests::_freeSimpleListObjects(List *list)
{
    if (list != nullptr)
    {
        for (ListIterator it = lbegin(list); !areIteratorsEqual(it, lend(list)); lnext(&it))
        {
            if (it.current->object != nullptr)
            {
                Object* object = it.current->object;
                it.current->object = nullptr;
                free(object->type);
                object->type = nullptr;
                free(object->payload);
                object->payload = nullptr;
                free(object);
                object = nullptr;
            }
        }
    }
}

QTEST_APPLESS_MAIN(LinkedListTests)

#include "tst_linkedlisttests.moc"
