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
    Stack* stack = createStack();

    Object* segment = createObject("Segment", createSegmentPayload(2, 5, 4, 11));
    Object* localConditions = createObject("LocalConditions", createLocalConditionsPayload(7, -5, 10, 12.8));

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
