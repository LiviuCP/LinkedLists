#include <QtTest>

#include "../../LinkedListsLib/stack.h"
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
    void testClearStack();
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
    Stack* stack = createStack();

    QVERIFY(isEmptyStack(stack));

    pushToStack(stack, createObject("Segment", createSegmentPayload(2, 5, 4, 11)));

    QVERIFY2(!isEmptyStack(stack), "The stack is empty, no element has been pushed");

    pushToStack(stack, createObject("LocalConditions", createLocalConditionsPayload(7, -5, 10, 12.8)));

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

void StackTests::testClearStack()
{
    Stack* stack = createStack();

    pushToStack(stack, createObject("Segment", createSegmentPayload(2, 5, 4, 11)));
    pushToStack(stack, createObject("LocalConditions", createLocalConditionsPayload(7, -5, 10, 12.8)));

    clearStack(stack, deleteTestObject);

    QVERIFY2(isEmptyStack(stack), "The stack has not been correctly emptied");

    pushToStack(stack, createObject("Segment", createSegmentPayload(4, 2, 5, 10)));

    QVERIFY(!isEmptyStack(stack));

    deleteStack(stack, deleteTestObject);
    stack = nullptr;
}

QTEST_APPLESS_MAIN(StackTests)

#include "tst_stacktests.moc"
