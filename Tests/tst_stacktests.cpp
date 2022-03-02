#include <QtTest>

#include "stack.h"
#include "codeutils.h"
#include "testobjects.h"

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

    pushToStack(stack, SEGMENT, createSegmentPayload(2, 5, 4, 11));

    QVERIFY2(!isEmptyStack(stack), "The stack is empty, no element has been pushed");

    pushToStack(stack, LOCAL_CONDITIONS, createLocalConditionsPayload(7, -5, 10, 12.8));

    // then pop
    Object* firstPoppedObject = popFromStack(stack);
    LocalConditions* firstPoppedObjectPayload = static_cast<LocalConditions*>(firstPoppedObject->payload);

    QVERIFY2(firstPoppedObject->type == LOCAL_CONDITIONS &&
             firstPoppedObjectPayload->position->x == 7 &&
             firstPoppedObjectPayload->position->y == -5 &&
             firstPoppedObjectPayload->temperature == 10 &&
             areDecimalNumbersEqual(firstPoppedObjectPayload->humidity, 12.8), "Object has not been correctly popped from list");
    QVERIFY(!isEmptyStack(stack));

    Object* secondPoppedObject = popFromStack(stack);
    Segment* secondPoppedObjectPayload = static_cast<Segment*>(secondPoppedObject->payload);

    QVERIFY2(secondPoppedObject->type == SEGMENT &&
             secondPoppedObjectPayload->start->x == 2 &&
             secondPoppedObjectPayload->start->y == 5 &&
             secondPoppedObjectPayload->stop->x == 4 &&
             secondPoppedObjectPayload->stop->y == 11,   "Object has not been correctly popped from list");
    QVERIFY(isEmptyStack(stack));

    deleteObject(firstPoppedObject, emptyTestObject);
    firstPoppedObject = nullptr;
    deleteObject(secondPoppedObject, emptyTestObject);
    secondPoppedObject = nullptr;
    deleteStack(stack, emptyTestObject);
    stack = nullptr;
}

void StackTests::testClearStack()
{
    Stack* stack = createStack();

    pushToStack(stack, SEGMENT, createSegmentPayload(2, 5, 4, 11));
    pushToStack(stack, LOCAL_CONDITIONS, createLocalConditionsPayload(7, -5, 10, 12.8));

    clearStack(stack, emptyTestObject);

    QVERIFY2(isEmptyStack(stack), "The stack has not been correctly emptied");

    pushToStack(stack, SEGMENT, createSegmentPayload(4, 2, 5, 10));

    QVERIFY(!isEmptyStack(stack));

    deleteStack(stack, emptyTestObject);
    stack = nullptr;
}

QTEST_APPLESS_MAIN(StackTests)

#include "tst_stacktests.moc"
