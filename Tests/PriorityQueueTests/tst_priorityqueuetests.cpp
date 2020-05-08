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

     // the remove

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



QTEST_APPLESS_MAIN(PriorityQueueTests)

#include "tst_priorityqueuetests.moc"
