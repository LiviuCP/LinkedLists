#include <QtTest>

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
};

LinkedListTests::LinkedListTests()
{
}

List* LinkedListTests::createLinkedList(std::initializer_list<size_t> priorities)
{
    List* list = static_cast<List*>(malloc(sizeof(List)));
    list->first = nullptr;

    for (auto priority : priorities)
    {
        createAndAppendToList(list, priority);
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

        free(list);
    }
}

void LinkedListTests::testBasicListOperations()
{
    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4, 3, 1, 2, 9, 7});
        createAndAppendToList(list, 8);

        QVERIFY2(getListSize(list) == 10 && getElementAtIndex(list, 0)->priority == 6 && getElementAtIndex(list, 9)->priority == 8, "Element has not been correctly created and appended");

        free(list);
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4, 3, 1, 2, 9, 7});
        createAndPrependToList(list, 8);

        QVERIFY2(getListSize(list) == 10 && getElementAtIndex(list, 0)->priority == 8 && getElementAtIndex(list, 9)->priority == 7, "Element has not been correctly created and prepended");

        free(list);
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4, 3, 1, 2, 9, 8});
        ListElement* firstElement = removeFirstListElement(list);

        QVERIFY2(getListSize(list) == 8 && getElementAtIndex(list, 0)->priority == 2 && getElementAtIndex(list, 7)->priority == 8, "Element has not been correctly created and appended");
        QVERIFY(firstElement->next == nullptr && firstElement->priority == 6);

        free(firstElement);
        free(list);
    }

    {
        List* list = createLinkedList(std::initializer_list<size_t>{6, 2, 5, 4, 3, 1, 2, 9, 8});
        ListElement* lastElement = removeLastListElement(list);

        QVERIFY2(getListSize(list) == 8 && getElementAtIndex(list, 0)->priority == 6 && getElementAtIndex(list, 7)->priority == 9, "Element has not been correctly created and appended");
        QVERIFY(lastElement->next == nullptr && lastElement->priority == 8);

        free(lastElement);
        free(list);
    }
}

QTEST_APPLESS_MAIN(LinkedListTests)

#include "tst_linkedlisttests.moc"
