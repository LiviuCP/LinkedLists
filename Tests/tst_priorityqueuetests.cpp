#include <QTest>

#include "priorityqueue.h"
#include "codeutils.h"
#include "testobjects.h"

class PriorityQueueTests : public QObject
{
    Q_OBJECT

private slots:
    void testElementsInsertionDeletion();
    void testClearPriorityQueue();
    void testIterators();
    void testModifyObject();
};

void PriorityQueueTests::testElementsInsertionDeletion()
{
    PriorityQueue* queue = createPriorityQueue();

    QVERIFY(isEmptyQueue(queue));

    // first insert
    insertIntoPriorityQueue(queue, 4, POINT, createPointPayload(4, 5));

    QVERIFY2(!isEmptyQueue(queue), "The queue is empty, no element has been inserted");

    insertIntoPriorityQueue(queue, 2, INTEGER, createIntegerPayload(4));
    insertIntoPriorityQueue(queue, 5, POINT, createPointPayload(-4, 8));
    insertIntoPriorityQueue(queue, 8, LOCAL_CONDITIONS, createLocalConditionsPayload(7, -5, 10, 12.8));
    insertIntoPriorityQueue(queue, 2, INTEGER, createIntegerPayload(-4));
    insertIntoPriorityQueue(queue, 5, DECIMAL, createDecimalPayload(4.5567));

    // then remove

    Object* firstRemovedObject = removeFromPriorityQueue(queue);
    LocalConditions* firstRemovedObjectPayload = static_cast<LocalConditions*>(firstRemovedObject->payload);
    QVERIFY2(firstRemovedObject->type == LOCAL_CONDITIONS &&
             firstRemovedObjectPayload->position->x == 7 &&
             firstRemovedObjectPayload->position->y == -5 &&
             firstRemovedObjectPayload->temperature == 10 &&
             areDecimalNumbersEqual(firstRemovedObjectPayload->humidity, 12.8), "Object has not been correctly removed from queue");

    Object* secondRemovedObject = removeFromPriorityQueue(queue);
    Point* secondRemovedObjectPayload = static_cast<Point*>(secondRemovedObject->payload);
    QVERIFY2(secondRemovedObject->type == POINT &&
             secondRemovedObjectPayload->x == -4 &&
             secondRemovedObjectPayload->y == 8 , "Object has not been correctly removed from queue");

    Object* thirdRemovedObject = removeFromPriorityQueue(queue);
    double* thirdRemovedObjectPayload = static_cast<double*>(thirdRemovedObject->payload);
    QVERIFY2(thirdRemovedObject->type == DECIMAL &&
             areDecimalNumbersEqual(*thirdRemovedObjectPayload, 4.5567), "Object has not been correctly removed from queue");

    Object* fourthRemovedObject = removeFromPriorityQueue(queue);
    Point* fourthRemovedObjectPayload = static_cast<Point*>(fourthRemovedObject->payload);
    QVERIFY2(secondRemovedObject->type == POINT &&
             fourthRemovedObjectPayload->x == 4 &&
             fourthRemovedObjectPayload->y == 5 , "Object has not been correctly removed from queue");

    Object* fifthRemovedObject = removeFromPriorityQueue(queue);
    int* fifthRemovedObjectPayload = static_cast<int*>(fifthRemovedObject->payload);
    QVERIFY2(fifthRemovedObject->type == INTEGER && *fifthRemovedObjectPayload == 4, "Object has not been correctly removed from queue");
    QVERIFY(!isEmptyQueue(queue));

    Object* sixthRemovedObject = removeFromPriorityQueue(queue);
    int* sixthRemovedObjectPayload = static_cast<int*>(sixthRemovedObject->payload);
    QVERIFY2(sixthRemovedObject->type == INTEGER && *sixthRemovedObjectPayload == -4, "Object has not been correctly removed from queue");
    QVERIFY(isEmptyQueue(queue));

    deleteObject(firstRemovedObject, emptyTestObject);
    firstRemovedObject = nullptr;
    deleteObject(secondRemovedObject, emptyTestObject);
    secondRemovedObject = nullptr;
    deleteObject(thirdRemovedObject, emptyTestObject);
    thirdRemovedObject = nullptr;
    deleteObject(fourthRemovedObject, emptyTestObject);
    fourthRemovedObject = nullptr;
    deleteObject(fifthRemovedObject, emptyTestObject);
    fifthRemovedObject = nullptr;
    deleteObject(sixthRemovedObject, emptyTestObject);
    sixthRemovedObject = nullptr;
    deletePriorityQueue(queue, emptyTestObject);
    queue = nullptr;
}

void PriorityQueueTests::testClearPriorityQueue()
{
    PriorityQueue* queue = createPriorityQueue();

    insertIntoPriorityQueue(queue, 5, SEGMENT, createSegmentPayload(2, 5, 4, 11));
    insertIntoPriorityQueue(queue, 8, LOCAL_CONDITIONS, createLocalConditionsPayload(7, -5, 10, 12.8));

    clearPriorityQueue(queue, emptyTestObject);

    QVERIFY2(isEmptyQueue(queue), "The queue has not been correctly emptied");

    insertIntoPriorityQueue(queue, 7, SEGMENT, createSegmentPayload(4, 2, 5, 10));

    QVERIFY(!isEmptyQueue(queue));

    deletePriorityQueue(queue, emptyTestObject);
    queue = nullptr;
}

void PriorityQueueTests::testIterators()
{
    {
        PriorityQueue* queue = createPriorityQueue();

        insertIntoPriorityQueue(queue, 4, POINT, createPointPayload(4, 5));
        insertIntoPriorityQueue(queue, 2, INTEGER, createIntegerPayload(4));
        insertIntoPriorityQueue(queue, 5, POINT, createPointPayload(-4, 8));
        insertIntoPriorityQueue(queue, 8, LOCAL_CONDITIONS, createLocalConditionsPayload(7, -5, 10, 12.8));
        insertIntoPriorityQueue(queue, 2, INTEGER, createIntegerPayload(-4));
        insertIntoPriorityQueue(queue, 5, DECIMAL, createDecimalPayload(4.5567));

        PriorityQueueIterator it = pqbegin(queue);

        Object* firstObject = getPriorityQueueObject(it);
        LocalConditions* firstObjectPayload = static_cast<LocalConditions*>(firstObject->payload);
        QVERIFY2(getObjectPriority(it) == 8 &&
                 firstObject->type == LOCAL_CONDITIONS &&
                 firstObjectPayload->position->x == 7 &&
                 firstObjectPayload->position->y == -5 &&
                 firstObjectPayload->temperature == 10 &&
                 areDecimalNumbersEqual(firstObjectPayload->humidity, 12.8), "The iterator does not point to the right queue object");

        pqnext(&it);

        Object* secondObject = getPriorityQueueObject(it);
        Point* secondObjectPayload = static_cast<Point*>(secondObject->payload);
        QVERIFY2(getObjectPriority(it) == 5 &&
                 secondObject->type == POINT &&
                 secondObjectPayload->x == -4 &&
                 secondObjectPayload->y == 8 ,            "The iterator does not point to the right queue object");

        pqnext(&it);

        Object* thirdObject = getPriorityQueueObject(it);
        double* thirdObjectPayload = static_cast<double*>(thirdObject->payload);
        QVERIFY2(getObjectPriority(it) == 5 &&
                 thirdObject->type == DECIMAL &&
                 areDecimalNumbersEqual(*thirdObjectPayload, 4.5567), "The iterator does not point to the right queue object");

        pqnext(&it);

        Object* fourthObject = getPriorityQueueObject(it);
        Point* fourthObjectPayload = static_cast<Point*>(fourthObject->payload);
        QVERIFY2(getObjectPriority(it) == 4 &&
                 secondObject->type == POINT &&
                 fourthObjectPayload->x == 4 &&
                 fourthObjectPayload->y == 5 ,            "The iterator does not point to the right queue object");

        pqnext(&it);

        Object* fifthObject = getPriorityQueueObject(it);
        int* fifthObjectPayload = static_cast<int*>(fifthObject->payload);
        QVERIFY2(getObjectPriority(it) == 2 &&
                 fifthObject->type == INTEGER &&
                 *fifthObjectPayload == 4,                "The iterator does not point to the right queue object");

        pqnext(&it);

        Object* sixthObject = getPriorityQueueObject(it);
        int* sixthObjectPayload = static_cast<int*>(sixthObject->payload);
        QVERIFY2(getObjectPriority(it) == 2 &&
                 sixthObject->type == INTEGER &&
                 *sixthObjectPayload == -4,               "The iterator does not point to the right queue object");

        pqnext(&it);

        QVERIFY2(it.queueItem == nullptr, "The iterator doesn't correctly reach the end position");

        deletePriorityQueue(queue, emptyTestObject);
        queue = nullptr;
    }

    {
        PriorityQueue* queue = createPriorityQueue();
        PriorityQueueIterator it = pqbegin(queue);

        QVERIFY2(it.queueItem == nullptr, "The begin iterator of an empty priority queue is not correctly created");

        deletePriorityQueue(queue, emptyTestObject);
        queue = nullptr;
    }
}

void PriorityQueueTests::testModifyObject()
{
    PriorityQueue* queue = createPriorityQueue();

    insertIntoPriorityQueue(queue, 8, POINT, createPointPayload(4, 5));
    insertIntoPriorityQueue(queue, 2, INTEGER, createIntegerPayload(4));
    insertIntoPriorityQueue(queue, 5, POINT, createPointPayload(-4, 8));
    insertIntoPriorityQueue(queue, 4, LOCAL_CONDITIONS, createLocalConditionsPayload(7, -5, 10, 12.8));
    insertIntoPriorityQueue(queue, 2, INTEGER, createIntegerPayload(-4));
    insertIntoPriorityQueue(queue, 5, DECIMAL, createDecimalPayload(4.5567));

    PriorityQueueIterator it = pqbegin(queue);
    pqnext(&it);
    pqnext(&it);
    pqnext(&it);

    Object* objectToEdit = getPriorityQueueObject(it);

    QVERIFY(getObjectPriority(it) == 4);

    LocalConditions* payloadToEdit = static_cast<LocalConditions*>(objectToEdit->payload);
    payloadToEdit->position->y = 2;
    payloadToEdit->temperature = 11;
    objectToEdit = nullptr;

    deleteObject(removeFromPriorityQueue(queue), emptyTestObject);
    deleteObject(removeFromPriorityQueue(queue), emptyTestObject);
    deleteObject(removeFromPriorityQueue(queue), emptyTestObject);

    Object* removedObject = removeFromPriorityQueue(queue);
    LocalConditions* removedObjectPayload = static_cast<LocalConditions*>(removedObject->payload);

    QVERIFY2(removedObject->type == LOCAL_CONDITIONS &&
             removedObjectPayload->position->x == 7 &&
             removedObjectPayload->position->y == 2 &&
             removedObjectPayload->temperature == 11 &&
             areDecimalNumbersEqual(removedObjectPayload->humidity, 12.8), "The object has not been correctly modified");

    deleteObject(removedObject, emptyTestObject);
    removedObject = nullptr;

    deletePriorityQueue(queue, emptyTestObject);
    queue = nullptr;
}



QTEST_APPLESS_MAIN(PriorityQueueTests)

#include "tst_priorityqueuetests.moc"
