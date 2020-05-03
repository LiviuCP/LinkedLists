#include <QtTest>

#include "../../LinkedListsLib/stack.h"
#include "../../LinkedListsLib/linkedlist.h"
#include "../../Utils/codeutils.h"
#include "../../Utils/testobjects.h"

class StackTests : public QObject
{
    Q_OBJECT

public:
    StackTests();
    ~StackTests();

private slots:
    void testElementsAreCorrectlyPushedAndPopped();

};

StackTests::StackTests()
{

}

StackTests::~StackTests()
{

}

void StackTests::testElementsAreCorrectlyPushedAndPopped()
{
    // first push
    Object* segment = (Object*)(malloc(sizeof (Object)));
    segment->type = (char*)malloc(strlen("Segment") + 1);
    strcpy(segment->type, "Segment");
    segment->payload = createSegmentPayload(2, 5, 4, 11);

    Object* localConditions = (Object*)(malloc(sizeof (Object)));
    localConditions->type = (char*)malloc(strlen("LocalConditions") + 1);
    strcpy(localConditions->type, "LocalConditions");
    localConditions->payload = createLocalConditionsPayload(7, -5, 10, 12.8);

    Stack* stack = createStack();

    pushToStack(stack, segment);
    segment = nullptr;

    QVERIFY2(!isEmptyStack(stack), "The stack is empty, no element has been pushed");

    pushToStack(stack, localConditions);
    localConditions = nullptr;

    // then pop
    Object* firstPoppedObject = popFromStack(stack);
    LocalConditions* firstPoppedObjectPayload = static_cast<LocalConditions*>(firstPoppedObject->payload);

    QVERIFY2(strcmp(firstPoppedObject->type, "LocalConditions") == 0 &&
             firstPoppedObjectPayload->position->x == 7 &&
             firstPoppedObjectPayload->position->y == -5 &&
             firstPoppedObjectPayload->temperature == 10 &&
             firstPoppedObjectPayload->humidity == 12.8,    "Object has not been correctly popped from list");
    QVERIFY(!isEmptyStack(stack));

    Object* secondPoppedObject = popFromStack(stack);
    Segment* secondPoppedObjectPayload = static_cast<Segment*>(secondPoppedObject->payload);

    QVERIFY2(strcmp(secondPoppedObject->type, "Segment") == 0 &&
             secondPoppedObjectPayload->start->x == 2 &&
             secondPoppedObjectPayload->start->y == 5 &&
             secondPoppedObjectPayload->stop->x == 4 &&
             secondPoppedObjectPayload->stop->y == 11,   "Object has not been correctly popped from list");
    QVERIFY(isEmptyStack(stack));

    deleteTestObject(firstPoppedObject);
    firstPoppedObject = nullptr;
    deleteTestObject(secondPoppedObject);
    secondPoppedObject = nullptr;
    deleteStack(stack, deleteTestObject);
    stack = nullptr;
}

QTEST_APPLESS_MAIN(StackTests)

#include "tst_stacktests.moc"
