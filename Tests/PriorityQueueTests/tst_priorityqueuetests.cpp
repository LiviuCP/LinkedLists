#include <QtTest>

#include "../../LinkedListsLib/priorityqueue.h"
#include "../../Utils/codeutils.h"
#include "../../Utils/testobjects.h"

class PriorityQueueTests : public QObject
{
    Q_OBJECT

public:
    PriorityQueueTests();
    ~PriorityQueueTests();

private slots:
    void testElementsInsertionDeletion();
    void testClearPriorityQueue();
    void testIterators();
};

PriorityQueueTests::PriorityQueueTests()
{

}

PriorityQueueTests::~PriorityQueueTests()
{

}

void PriorityQueueTests::testElementsInsertionDeletion()
{
    PriorityQueue* queue = createPriorityQueue();

    QVERIFY(isEmptyQueue(queue));

    // first insert
    insertIntoPriorityQueue(queue, 4, createObject("Point", createPointPayload(4, 5)));

    QVERIFY2(!isEmptyQueue(queue), "The queue is empty, no element has been inserted");

    insertIntoPriorityQueue(queue, 2, createObject("Integer", createIntegerPayload(4)));
    insertIntoPriorityQueue(queue, 5, createObject("Point", createPointPayload(-4, 8)));
    insertIntoPriorityQueue(queue, 8, createObject("LocalConditions", createLocalConditionsPayload(7, -5, 10, 12.8)));
    insertIntoPriorityQueue(queue, 2, createObject("Integer", createIntegerPayload(-4)));
    insertIntoPriorityQueue(queue, 5, createObject("Decimal", createDecimalPayload(4.5567)));

    // then remove

    Object* firstRemovedObject = removeFromPriorityQueue(queue);
    LocalConditions* firstRemovedObjectPayload = static_cast<LocalConditions*>(firstRemovedObject->payload);
    QVERIFY2(strcmp(firstRemovedObject->type, "LocalConditions") == 0 &&
             firstRemovedObjectPayload->position->x == 7 &&
             firstRemovedObjectPayload->position->y == -5 &&
             firstRemovedObjectPayload->temperature == 10 &&
             firstRemovedObjectPayload->humidity == 12.8,    "Object has not been correctly removed from queue");

    Object* secondRemovedObject = removeFromPriorityQueue(queue);
    Point* secondRemovedObjectPayload = static_cast<Point*>(secondRemovedObject->payload);
    QVERIFY2(strcmp(secondRemovedObject->type, "Point") == 0 &&
             secondRemovedObjectPayload->x == -4 &&
             secondRemovedObjectPayload->y == 8 ,            "Object has not been correctly removed from queue");

    Object* thirdRemovedObject = removeFromPriorityQueue(queue);
    double* thirdRemovedObjectPayload = static_cast<double*>(thirdRemovedObject->payload);
    QVERIFY2(strcmp(thirdRemovedObject->type, "Decimal") == 0 &&
             *thirdRemovedObjectPayload == 4.5567,           "Object has not been correctly removed from queue");

    Object* fourthRemovedObject = removeFromPriorityQueue(queue);
    Point* fourthRemovedObjectPayload = static_cast<Point*>(fourthRemovedObject->payload);
    QVERIFY2(strcmp(secondRemovedObject->type, "Point") == 0 &&
             fourthRemovedObjectPayload->x == 4 &&
             fourthRemovedObjectPayload->y == 5 ,            "Object has not been correctly removed from queue");

    Object* fifthRemovedObject = removeFromPriorityQueue(queue);
    int* fifthRemovedObjectPayload = static_cast<int*>(fifthRemovedObject->payload);
    QVERIFY2(strcmp(fifthRemovedObject->type, "Integer") == 0 &&
             *fifthRemovedObjectPayload == 4,           "Object has not been correctly removed from queue");
    QVERIFY(!isEmptyQueue(queue));

    Object* sixthRemovedObject = removeFromPriorityQueue(queue);
    int* sixthRemovedObjectPayload = static_cast<int*>(sixthRemovedObject->payload);
    QVERIFY2(strcmp(sixthRemovedObject->type, "Integer") == 0 &&
             *sixthRemovedObjectPayload == -4,           "Object has not been correctly removed from queue");
    QVERIFY(isEmptyQueue(queue));

    deleteTestObject(firstRemovedObject);
    firstRemovedObject = nullptr;
    deleteTestObject(secondRemovedObject);
    secondRemovedObject = nullptr;
    deleteTestObject(thirdRemovedObject);
    thirdRemovedObject = nullptr;
    deleteTestObject(fourthRemovedObject);
    fourthRemovedObject = nullptr;
    deleteTestObject(fifthRemovedObject);
    fifthRemovedObject = nullptr;
    deleteTestObject(sixthRemovedObject);
    sixthRemovedObject = nullptr;
    deletePriorityQueue(queue, deleteTestObject);
    queue = nullptr;
}

void PriorityQueueTests::testClearPriorityQueue()
{
    PriorityQueue* queue = createPriorityQueue();

    insertIntoPriorityQueue(queue, 5, createObject("Segment", createSegmentPayload(2, 5, 4, 11)));
    insertIntoPriorityQueue(queue, 8, createObject("LocalConditions", createLocalConditionsPayload(7, -5, 10, 12.8)));

    clearPriorityQueue(queue, deleteTestObject);

    QVERIFY2(isEmptyQueue(queue), "The queue has not been correctly emptied");

    insertIntoPriorityQueue(queue, 7, createObject("Segment", createSegmentPayload(4, 2, 5, 10)));

    QVERIFY(!isEmptyQueue(queue));

    deletePriorityQueue(queue, deleteTestObject);
    queue = nullptr;
}

void PriorityQueueTests::testIterators()
{
    {
        PriorityQueue* queue = createPriorityQueue();

        insertIntoPriorityQueue(queue, 4, createObject("Point", createPointPayload(4, 5)));
        insertIntoPriorityQueue(queue, 2, createObject("Integer", createIntegerPayload(4)));
        insertIntoPriorityQueue(queue, 5, createObject("Point", createPointPayload(-4, 8)));
        insertIntoPriorityQueue(queue, 8, createObject("LocalConditions", createLocalConditionsPayload(7, -5, 10, 12.8)));
        insertIntoPriorityQueue(queue, 2, createObject("Integer", createIntegerPayload(-4)));
        insertIntoPriorityQueue(queue, 5, createObject("Decimal", createDecimalPayload(4.5567)));

        PriorityQueueIterator it = pqbegin(queue);

        Object* firstObject = getPriorityQueueObject(it);
        LocalConditions* firstObjectPayload = static_cast<LocalConditions*>(firstObject->payload);
        QVERIFY2(getObjectPriority(it) == 8 &&
                 strcmp(firstObject->type, "LocalConditions") == 0 &&
                 firstObjectPayload->position->x == 7 &&
                 firstObjectPayload->position->y == -5 &&
                 firstObjectPayload->temperature == 10 &&
                 firstObjectPayload->humidity == 12.8,    "The iterator does not point to the right queue object");

        pqnext(&it);

        Object* secondObject = getPriorityQueueObject(it);
        Point* secondObjectPayload = static_cast<Point*>(secondObject->payload);
        QVERIFY2(getObjectPriority(it) == 5 &&
                 strcmp(secondObject->type, "Point") == 0 &&
                 secondObjectPayload->x == -4 &&
                 secondObjectPayload->y == 8 ,            "The iterator does not point to the right queue object");

        pqnext(&it);

        Object* thirdObject = getPriorityQueueObject(it);
        double* thirdObjectPayload = static_cast<double*>(thirdObject->payload);
        QVERIFY2(getObjectPriority(it) == 5 &&
                 strcmp(thirdObject->type, "Decimal") == 0 &&
                 *thirdObjectPayload == 4.5567,           "The iterator does not point to the right queue object");

        pqnext(&it);

        Object* fourthObject = getPriorityQueueObject(it);
        Point* fourthObjectPayload = static_cast<Point*>(fourthObject->payload);
        QVERIFY2(getObjectPriority(it) == 4 &&
                 strcmp(secondObject->type, "Point") == 0 &&
                 fourthObjectPayload->x == 4 &&
                 fourthObjectPayload->y == 5 ,            "The iterator does not point to the right queue object");

        pqnext(&it);

        Object* fifthObject = getPriorityQueueObject(it);
        int* fifthObjectPayload = static_cast<int*>(fifthObject->payload);
        QVERIFY2(getObjectPriority(it) == 2 &&
                 strcmp(fifthObject->type, "Integer") == 0 &&
                 *fifthObjectPayload == 4,                "The iterator does not point to the right queue object");

        pqnext(&it);

        Object* sixthObject = getPriorityQueueObject(it);
        int* sixthObjectPayload = static_cast<int*>(sixthObject->payload);
        QVERIFY2(getObjectPriority(it) == 2 &&
                 strcmp(sixthObject->type, "Integer") == 0 &&
                 *sixthObjectPayload == -4,               "The iterator does not point to the right queue object");

        pqnext(&it);

        QVERIFY2(it.queueItem == nullptr, "The iterator doesn't correctly reach the end position");

        deletePriorityQueue(queue, deleteTestObject);
        queue = nullptr;
    }

    {
        PriorityQueue* queue = createPriorityQueue();
        PriorityQueueIterator it = pqbegin(queue);

        QVERIFY2(it.queueItem == nullptr, "The begin iterator of an empty priority queue is not correctly created");

        deletePriorityQueue(queue, deleteTestObject);
        queue = nullptr;
    }
}



QTEST_APPLESS_MAIN(PriorityQueueTests)

#include "tst_priorityqueuetests.moc"
