#include <QTest>

#include "stack.h"
#include "listelementspool.h"
#include "codeutils.h"
#include "testobjects.h"

#define DELETE_STACK(stack, deleter) \
    if (stack) \
    { \
        deleteStack(stack, deleter); \
        stack = nullptr; \
    }

class StackTests : public QObject
{
    Q_OBJECT

public:
    explicit StackTests();

private slots:
    void testElementsAreCorrectlyPushedAndPopped();
    void testClearStack();

    void initTestCase_data();
    void cleanupTestCase();
    void init();
    void cleanup();

private:
    ListElementsPool* m_Pool;
    size_t m_TotalAvailablePoolElementsCount;

    Stack* m_Stack1;
};

StackTests::StackTests()
    : m_Pool{nullptr}
    , m_TotalAvailablePoolElementsCount{0}
    , m_Stack1{nullptr}
{
}

void StackTests::testElementsAreCorrectlyPushedAndPopped()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    // first push
    m_Stack1 = createStack(pool);

    QVERIFY(isEmptyStack(m_Stack1));

    pushToStack(m_Stack1, SEGMENT, createSegmentPayload(2, 5, 4, 11));

    QVERIFY2(!isEmptyStack(m_Stack1), "The stack is empty, no element has been pushed");

    pushToStack(m_Stack1, LOCAL_CONDITIONS, createLocalConditionsPayload(7, -5, 10, 12.8));

    // then pop
    Object* firstPoppedObject = popFromStack(m_Stack1);
    LocalConditions* firstPoppedObjectPayload = static_cast<LocalConditions*>(firstPoppedObject->payload);

    QVERIFY2(firstPoppedObject->type == LOCAL_CONDITIONS &&
             firstPoppedObjectPayload->position->x == 7 &&
             firstPoppedObjectPayload->position->y == -5 &&
             firstPoppedObjectPayload->temperature == 10 &&
             areDecimalNumbersEqual(firstPoppedObjectPayload->humidity, 12.8), "Object has not been correctly popped from list");
    QVERIFY(!isEmptyStack(m_Stack1));

    Object* secondPoppedObject = popFromStack(m_Stack1);
    Segment* secondPoppedObjectPayload = static_cast<Segment*>(secondPoppedObject->payload);

    QVERIFY2(secondPoppedObject->type == SEGMENT &&
             secondPoppedObjectPayload->start->x == 2 &&
             secondPoppedObjectPayload->start->y == 5 &&
             secondPoppedObjectPayload->stop->x == 4 &&
             secondPoppedObjectPayload->stop->y == 11,   "Object has not been correctly popped from list");
    QVERIFY(isEmptyStack(m_Stack1));

    deleteObject(firstPoppedObject, emptyTestObject);
    firstPoppedObject = nullptr;
    deleteObject(secondPoppedObject, emptyTestObject);
    secondPoppedObject = nullptr;
}

void StackTests::testClearStack()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Pool);

    m_Stack1 = createStack(pool);

    pushToStack(m_Stack1, SEGMENT, createSegmentPayload(2, 5, 4, 11));
    pushToStack(m_Stack1, LOCAL_CONDITIONS, createLocalConditionsPayload(7, -5, 10, 12.8));

    clearStack(m_Stack1, emptyTestObject);

    QVERIFY2(isEmptyStack(m_Stack1), "The stack has not been correctly emptied");

    pushToStack(m_Stack1, SEGMENT, createSegmentPayload(4, 2, 5, 10));

    QVERIFY(!isEmptyStack(m_Stack1));
}

void StackTests::initTestCase_data()
{
    m_Pool = createListElementsPool();
    QVERIFY(m_Pool);

    m_TotalAvailablePoolElementsCount = getAvailableElementsCount(m_Pool);

    ListElementsPool* p_NullPool{nullptr};

    QTest::addColumn<ListElementsPool*>("pool");

    QTest::newRow("allocation from pool") << m_Pool;
    QTest::newRow("no pool allocation") << p_NullPool;
}

void StackTests::cleanupTestCase()
{
    QVERIFY(m_Pool);

    deleteListElementsPool(m_Pool);
    m_Pool = nullptr;
    m_TotalAvailablePoolElementsCount = 0;
}

void StackTests::init()
{
    QVERIFY(m_Pool);
    QVERIFY(getAvailableElementsCount(m_Pool) == m_TotalAvailablePoolElementsCount);

    QVERIFY(!m_Stack1);
}

void StackTests::cleanup()
{
    DELETE_STACK(m_Stack1, emptyTestObject);

    QVERIFY(getAvailableElementsCount(m_Pool) == m_TotalAvailablePoolElementsCount);
}

QTEST_APPLESS_MAIN(StackTests)

#include "tst_stacktests.moc"
