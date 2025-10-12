#include <QTest>

#include "listtestfixture.h"
#include "codeutils.h"
#include "testobjects.h"

#define ALLOC_LIST_ELEMENT_REFS(listElementRefs, count, fixture) \
    listElementRefs = (ListElement**)malloc(count * sizeof(ListElement*)); \
    fixture.markListElementRefsForDeletion(listElementRefs); \
\
    QVERIFY(listElementRefs); \
\
    for (size_t index = 0; index < count; ++index) \
    { \
        listElementRefs[index] = nullptr; \
    } \

#define CHECK_AQUIRED_ELEMENTS(listElementRefs, count) \
    QVERIFY((listElementRefs)); \
\
    for (size_t index = 0; index < count; ++index) \
    { \
        QVERIFY((listElementRefs)[index] && \
                (listElementRefs)[index]->priority == 0 && \
                (listElementRefs)[index]->object.type == -1 && \
                (listElementRefs)[index]->object.payload == nullptr); \
    }

#define CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(pool, aquiredElementsCount, availableElementsCount) \
    QVERIFY(getAquiredElementsCount(pool) == aquiredElementsCount && getAvailableElementsCount(pool) == availableElementsCount);

#define RELEASE_ELEMENTS(listElementRefs, count, elementsPool) \
    QVERIFY((listElementRefs)); \
\
    for (size_t index = 0; index < count; ++index) \
    { \
        releaseElement((listElementRefs)[index], elementsPool); \
    }

class ListElementTests : public QObject
{
    Q_OBJECT

private slots:
    void testAquiringSinglePoolElement();
    void testAquiringMultiplePoolElements();
    void testOptimizingPoolCapacity();
    void testAllPoolElementsAquired();
    void testAssignRemoveObject();
    void testCustomCopyObject();

    void initTestCase_data();
    void cleanupTestCase();
    void init();
    void cleanup();

private:
    ListTestFixture m_Fixture;
};

void ListElementTests::testAquiringSinglePoolElement()
{
    m_Fixture.m_TempPool1 = createListElementsPool(USE_DEFAULT_MAX_SLICES_COUNT);
    QVERIFY(m_Fixture.m_TempPool1);

    // in this case the temp pool is assigned to m_Fixture.m_List1 only to prevent elements deletion from list (in case of test fail); the aquiring/releasing of elements occurs externally
    m_Fixture.m_List1 = createEmptyList(m_Fixture.m_TempPool1);
    QVERIFY(m_Fixture.m_List1);

    const size_t initialPooledElementsCount = getAvailableElementsCount(m_Fixture.m_TempPool1);

    QVERIFY(initialPooledElementsCount == ELEMENTS_POOL_SLICE_SIZE);
    QVERIFY(initialPooledElementsCount >= 8); // safety net to prevent overflows, see below
    QVERIFY(getAquiredElementsCount(m_Fixture.m_TempPool1) == 0);

    ListElement* element = aquireElement(m_Fixture.m_TempPool1);

    CHECK_AQUIRED_ELEMENTS(&element, 1);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 1, ELEMENTS_POOL_SLICE_SIZE - 1);

    prependToList(m_Fixture.m_List1, element);

    element = aquireElement(m_Fixture.m_TempPool1);

    CHECK_AQUIRED_ELEMENTS(&element, 1);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 2, ELEMENTS_POOL_SLICE_SIZE - 2);

    appendToList(m_Fixture.m_List1, element);

    QVERIFY(getListSize(m_Fixture.m_List1) == 2);

    for (size_t index = 0; index < ELEMENTS_POOL_SLICE_SIZE - 3; ++index)
    {
        element = aquireElement(m_Fixture.m_TempPool1);
        QVERIFY(element);

        if (index % 2)
        {
            prependToList(m_Fixture.m_List1, element);
        }
        else
        {
            appendToList(m_Fixture.m_List1, element);
        }
    }

    QVERIFY(getListSize(m_Fixture.m_List1) == ELEMENTS_POOL_SLICE_SIZE - 1);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, ELEMENTS_POOL_SLICE_SIZE - 1, 1);

    element = aquireElement(m_Fixture.m_TempPool1);

    CHECK_AQUIRED_ELEMENTS(&element, 1);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, ELEMENTS_POOL_SLICE_SIZE, 0);

    appendToList(m_Fixture.m_List1, element);
    QVERIFY(getListSize(m_Fixture.m_List1) == ELEMENTS_POOL_SLICE_SIZE);

    element = aquireElement(m_Fixture.m_TempPool1);

    QVERIFY(element);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, ELEMENTS_POOL_SLICE_SIZE + 1, ELEMENTS_POOL_SLICE_SIZE - 1);

    bool released = false;

    released = releaseElement(element, m_Fixture.m_TempPool1);
    QVERIFY(released);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, ELEMENTS_POOL_SLICE_SIZE, ELEMENTS_POOL_SLICE_SIZE);

    element = removeLastListElement(m_Fixture.m_List1);
    QVERIFY(element);

    released = releaseElement(element, m_Fixture.m_TempPool1);
    QVERIFY(released);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, ELEMENTS_POOL_SLICE_SIZE - 1, ELEMENTS_POOL_SLICE_SIZE + 1);

    element = removeFirstListElement(m_Fixture.m_List1);
    QVERIFY(element);

    released = releaseElement(element, m_Fixture.m_TempPool1);
    QVERIFY(released);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, ELEMENTS_POOL_SLICE_SIZE - 2, ELEMENTS_POOL_SLICE_SIZE + 2);

    // releasing an element twice is not allowed
    released = releaseElement(element, m_Fixture.m_TempPool1);
    QVERIFY(!released);

    ListElement* newElement = createListElement();
    m_Fixture.markListElementForCleanup(newElement, false);

    // releasing an element that doesn't belong to pool is not allowed
    released = releaseElement(newElement, m_Fixture.m_TempPool1);
    QVERIFY(!released);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, ELEMENTS_POOL_SLICE_SIZE - 2, ELEMENTS_POOL_SLICE_SIZE + 2);

    element = aquireElement(m_Fixture.m_TempPool1);

    CHECK_AQUIRED_ELEMENTS(&element, 1);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, ELEMENTS_POOL_SLICE_SIZE - 1, ELEMENTS_POOL_SLICE_SIZE + 1);

    released = releaseElement(element, m_Fixture.m_TempPool1);
    QVERIFY(released);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, ELEMENTS_POOL_SLICE_SIZE - 2, ELEMENTS_POOL_SLICE_SIZE + 2);

    const size_t listElementsCount = getListSize(m_Fixture.m_List1);
    QVERIFY(listElementsCount == ELEMENTS_POOL_SLICE_SIZE - 2);

    for (size_t index = 0; index < listElementsCount; ++index)
    {
        element = index % 2 == 0 ? removeLastListElement(m_Fixture.m_List1) : removeFirstListElement(m_Fixture.m_List1);
        QVERIFY(element);
        released = releaseElement(element, m_Fixture.m_TempPool1);
        QVERIFY(released);
        element = nullptr;
    }

    QVERIFY(isEmptyList(m_Fixture.m_List1));
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 0, 2 * ELEMENTS_POOL_SLICE_SIZE);

    released = releaseElement(nullptr, m_Fixture.m_TempPool1);
    QVERIFY(!released);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 0, 2 * ELEMENTS_POOL_SLICE_SIZE);
}

void ListElementTests::testAquiringMultiplePoolElements()
{
    m_Fixture.m_TempPool1 = createListElementsPool(USE_DEFAULT_MAX_SLICES_COUNT);
    QVERIFY(m_Fixture.m_TempPool1);

    // batch 1
    const size_t batchSize1 = 3;
    ListElement** listElementRefs1 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs1, batchSize1, m_Fixture);

    bool multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs1, batchSize1);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs1, batchSize1);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 3, ELEMENTS_POOL_SLICE_SIZE - 3);

    // batch 2
    const size_t batchSize2 = ELEMENTS_POOL_SLICE_SIZE - 2;
    ListElement** listElementRefs2 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs2, batchSize2, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs2, batchSize2);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs2, batchSize2);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, ELEMENTS_POOL_SLICE_SIZE + 1, ELEMENTS_POOL_SLICE_SIZE - 1);

    RELEASE_ELEMENTS(listElementRefs2, batchSize2, m_Fixture.m_TempPool1);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 3, 2 * ELEMENTS_POOL_SLICE_SIZE - 3);

    RELEASE_ELEMENTS(listElementRefs1, batchSize1, m_Fixture.m_TempPool1);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 0, 2 * ELEMENTS_POOL_SLICE_SIZE);

    // batch 3
    const size_t batchSize3 = 2 * ELEMENTS_POOL_SLICE_SIZE;
    ListElement** listElementRefs3 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs3, batchSize3, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs3, batchSize3);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs3, batchSize3);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 2 * ELEMENTS_POOL_SLICE_SIZE, 0);

    RELEASE_ELEMENTS(listElementRefs3, batchSize3, m_Fixture.m_TempPool1);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 0, 2 * ELEMENTS_POOL_SLICE_SIZE);

    // batch 4
    const size_t batchSize4 = 1;
    ListElement** listElementRefs4 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs4, batchSize4, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs4, batchSize4);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs4, batchSize4);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 1, 2 * ELEMENTS_POOL_SLICE_SIZE - 1);

    // batch 5
    size_t batchSize5 = 2 * ELEMENTS_POOL_SLICE_SIZE + 1;
    ListElement** listElementRefs5 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs5, batchSize5, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs5, batchSize5);
    QVERIFY(!multipleElementsAquired);

    --batchSize5;

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs5, batchSize5);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs5, batchSize5);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 2 * ELEMENTS_POOL_SLICE_SIZE + 1, ELEMENTS_POOL_SLICE_SIZE - 1);

    RELEASE_ELEMENTS(listElementRefs4, batchSize4, m_Fixture.m_TempPool1);
    RELEASE_ELEMENTS(listElementRefs5, batchSize5, m_Fixture.m_TempPool1);

    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 0, 3 * ELEMENTS_POOL_SLICE_SIZE);

    // batch 6
    const size_t batchSize6 = ELEMENTS_POOL_SLICE_SIZE + ELEMENTS_POOL_SLICE_SIZE / 2;
    ListElement** listElementRefs6 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs6, batchSize6, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs6, batchSize6);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs6, batchSize6);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, ELEMENTS_POOL_SLICE_SIZE + ELEMENTS_POOL_SLICE_SIZE / 2, ELEMENTS_POOL_SLICE_SIZE + ELEMENTS_POOL_SLICE_SIZE / 2);

    // batch 7
    const size_t batchSize7 = ELEMENTS_POOL_SLICE_SIZE;
    ListElement** listElementRefs7 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs7, batchSize7, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs7, batchSize7);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs7, batchSize7);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 2 * ELEMENTS_POOL_SLICE_SIZE + ELEMENTS_POOL_SLICE_SIZE / 2, ELEMENTS_POOL_SLICE_SIZE / 2);

    // batch 8
    size_t batchSize8 = ELEMENTS_POOL_SLICE_SIZE + ELEMENTS_POOL_SLICE_SIZE / 2 + 1;
    ListElement** listElementRefs8 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs8, batchSize8, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs8, batchSize8);

    QVERIFY(!multipleElementsAquired);

    --batchSize8;

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs8, batchSize8);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs8, batchSize8);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 4 * ELEMENTS_POOL_SLICE_SIZE, 0);

    // batch 9
    const size_t batchSize9 = 5;
    ListElement** listElementRefs9 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs9, batchSize9, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs9, batchSize9);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs9, batchSize9);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 4 * ELEMENTS_POOL_SLICE_SIZE + 5, ELEMENTS_POOL_SLICE_SIZE - 5);

    RELEASE_ELEMENTS(listElementRefs6, batchSize6, m_Fixture.m_TempPool1);
    RELEASE_ELEMENTS(listElementRefs7, batchSize7, m_Fixture.m_TempPool1);
    RELEASE_ELEMENTS(listElementRefs8, batchSize8, m_Fixture.m_TempPool1);
    RELEASE_ELEMENTS(listElementRefs9, batchSize9, m_Fixture.m_TempPool1);

    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 0, 5 * ELEMENTS_POOL_SLICE_SIZE);

    // combined test: aquire single element and aquire multiple elements
    m_Fixture.m_TempPool2 = createListElementsPool(USE_DEFAULT_MAX_SLICES_COUNT);
    QVERIFY(m_Fixture.m_TempPool2);

    ListElement* element = aquireElement(m_Fixture.m_TempPool2);
    QVERIFY(element);
    CHECK_AQUIRED_ELEMENTS(&element, 1);

    const size_t batchSize10 = ELEMENTS_POOL_SLICE_SIZE - 1;
    ListElement** listElementRefs10 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs10, batchSize10, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs10, batchSize10);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs10, batchSize10);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, ELEMENTS_POOL_SLICE_SIZE, 0);

    RELEASE_ELEMENTS(listElementRefs10, batchSize10, m_Fixture.m_TempPool2);
    RELEASE_ELEMENTS(&element, 1, m_Fixture.m_TempPool2);

    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 0, ELEMENTS_POOL_SLICE_SIZE);

    // additional corner cases
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs10, 0);
    QVERIFY(!multipleElementsAquired);

    multipleElementsAquired = aquireElements(nullptr, listElementRefs10, 1);
    QVERIFY(!multipleElementsAquired);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, nullptr, 1);
    QVERIFY(!multipleElementsAquired);
}

void ListElementTests::testOptimizingPoolCapacity()
{
    /* first pool: simple test for optimizing capacity */
    m_Fixture.m_TempPool1 = createListElementsPool(USE_DEFAULT_MAX_SLICES_COUNT);
    QVERIFY(m_Fixture.m_TempPool1);

    // batch 1
    const size_t batchSize1 = ELEMENTS_POOL_SLICE_SIZE + 1;
    ListElement** listElementRefs1 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs1, batchSize1, m_Fixture);
    bool multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs1, batchSize1);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, ELEMENTS_POOL_SLICE_SIZE + 1, ELEMENTS_POOL_SLICE_SIZE - 1);

    // batch 2
    const size_t batchSize2 = 5;
    ListElement** listElementRefs2 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs2, batchSize2, m_Fixture);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs2, batchSize2);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, ELEMENTS_POOL_SLICE_SIZE + 6, ELEMENTS_POOL_SLICE_SIZE - 6);

    RELEASE_ELEMENTS(listElementRefs1, batchSize1, m_Fixture.m_TempPool1);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 5, 2 * ELEMENTS_POOL_SLICE_SIZE - 5);

    shrinkPoolCapacity(m_Fixture.m_TempPool1);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 5, ELEMENTS_POOL_SLICE_SIZE - 5);

    RELEASE_ELEMENTS(listElementRefs2, batchSize2, m_Fixture.m_TempPool1);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 0, ELEMENTS_POOL_SLICE_SIZE);

    shrinkPoolCapacity(m_Fixture.m_TempPool1);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 0, ELEMENTS_POOL_SLICE_SIZE);

    /* second pool: test that the integrity of the aquired elements is preserved when optimization is performed */
    m_Fixture.m_TempPool2 = createListElementsPool(USE_DEFAULT_MAX_SLICES_COUNT);
    QVERIFY(m_Fixture.m_TempPool2);

    // batch 3
    const size_t batchSize3 = ELEMENTS_POOL_SLICE_SIZE - 1;
    ListElement** listElementRefs3 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs3, batchSize3, m_Fixture);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs3, batchSize3);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, ELEMENTS_POOL_SLICE_SIZE - 1, 1);

    // batch 4
    const size_t batchSize4 = ELEMENTS_POOL_SLICE_SIZE + 1;
    ListElement** listElementRefs4 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs4, batchSize4, m_Fixture);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs4, batchSize4);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 2 * ELEMENTS_POOL_SLICE_SIZE, 0);

    m_Fixture.m_List1 = createEmptyList(m_Fixture.m_TempPool2);
    QVERIFY(m_Fixture.m_List1);

    (void)createAndAppendToList(m_Fixture.m_List1, 5);
    (void)createAndPrependToList(m_Fixture.m_List1, 4);
    (void)createAndAppendToList(m_Fixture.m_List1, 2);
    (void)createAndAppendToList(m_Fixture.m_List1, 9);

    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 2 * ELEMENTS_POOL_SLICE_SIZE + 4, ELEMENTS_POOL_SLICE_SIZE - 4);

    // batch 5
    const size_t batchSize5 = ELEMENTS_POOL_SLICE_SIZE - 3;
    ListElement** listElementRefs5 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs5, batchSize5, m_Fixture);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs5, batchSize5);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 3 * ELEMENTS_POOL_SLICE_SIZE + 1, ELEMENTS_POOL_SLICE_SIZE - 1);

    const size_t prioritiesArray1[8] = {2, 4, 5, 8, 7, 0, 2, 3};
    m_Fixture.m_List2 = createListFromPrioritiesArray(prioritiesArray1, 8, m_Fixture.m_TempPool2);
    QVERIFY(m_Fixture.m_List2);

    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 3 * ELEMENTS_POOL_SLICE_SIZE + 9, ELEMENTS_POOL_SLICE_SIZE - 9);

    // batch 6
    const size_t batchSize6 = ELEMENTS_POOL_SLICE_SIZE - 4;
    ListElement** listElementRefs6 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs6, batchSize6, m_Fixture);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs6, batchSize6);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 4 * ELEMENTS_POOL_SLICE_SIZE + 5, ELEMENTS_POOL_SLICE_SIZE - 5);

    RELEASE_ELEMENTS(listElementRefs4, batchSize4, m_Fixture.m_TempPool2);
    RELEASE_ELEMENTS(listElementRefs6, batchSize6, m_Fixture.m_TempPool2);

    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 2 * ELEMENTS_POOL_SLICE_SIZE + 8, 3 * ELEMENTS_POOL_SLICE_SIZE - 8);

    shrinkPoolCapacity(m_Fixture.m_TempPool2);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 2 * ELEMENTS_POOL_SLICE_SIZE + 8, ELEMENTS_POOL_SLICE_SIZE - 8);

    sortDescendingByPriority(m_Fixture.m_List2);

    QVERIFY(getListSize(m_Fixture.m_List2) == 8);
    QVERIFY(m_Fixture.m_List2->first && m_Fixture.m_List2->first->priority == 8);
    QVERIFY(m_Fixture.m_List2->last && m_Fixture.m_List2->last->priority == 0);

    clearList(m_Fixture.m_List2, deleteObjectPayload);

    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 2 * ELEMENTS_POOL_SLICE_SIZE, ELEMENTS_POOL_SLICE_SIZE);

    shrinkPoolCapacity(m_Fixture.m_TempPool2);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 2 * ELEMENTS_POOL_SLICE_SIZE, ELEMENTS_POOL_SLICE_SIZE);

    RELEASE_ELEMENTS(listElementRefs3, batchSize3, m_Fixture.m_TempPool2);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, ELEMENTS_POOL_SLICE_SIZE + 1, 2 * ELEMENTS_POOL_SLICE_SIZE - 1);

    shrinkPoolCapacity(m_Fixture.m_TempPool2);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, ELEMENTS_POOL_SLICE_SIZE + 1, ELEMENTS_POOL_SLICE_SIZE - 1);

    // batch 7
    const size_t batchSize7 = ELEMENTS_POOL_SLICE_SIZE + 4;
    ListElement** listElementRefs7 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs7, batchSize7, m_Fixture);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs7, batchSize7);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs7, batchSize7);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 2 * ELEMENTS_POOL_SLICE_SIZE + 5, ELEMENTS_POOL_SLICE_SIZE - 5);

    shrinkPoolCapacity(m_Fixture.m_TempPool2);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 2 * ELEMENTS_POOL_SLICE_SIZE + 5, ELEMENTS_POOL_SLICE_SIZE - 5);

    QVERIFY(m_Fixture.m_List1->first && m_Fixture.m_List1->first->priority == 4);
    QVERIFY(m_Fixture.m_List1->last && m_Fixture.m_List1->last->priority == 9);

    RELEASE_ELEMENTS(listElementRefs5, batchSize5, m_Fixture.m_TempPool2);
    RELEASE_ELEMENTS(listElementRefs7, batchSize7, m_Fixture.m_TempPool2);

    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 4, 3 * ELEMENTS_POOL_SLICE_SIZE - 4);

    shrinkPoolCapacity(m_Fixture.m_TempPool2);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 4, ELEMENTS_POOL_SLICE_SIZE - 4);

    QVERIFY(getListSize(m_Fixture.m_List1) == 4);
    ListIterator it = lbegin(m_Fixture.m_List1);
    lnext(&it);

    QVERIFY(it.current && it.current->priority == 5);

    (void)createAndInsertBefore(it, 20);

    QVERIFY(getListSize(m_Fixture.m_List1) == 5);
    QVERIFY(m_Fixture.m_List1->first && m_Fixture.m_List1->first->next && m_Fixture.m_List1->first->next->priority == 20);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 5, ELEMENTS_POOL_SLICE_SIZE - 5);

    const size_t prioritiesArray2[4] = {9, 5, 12, 7};
    m_Fixture.m_List3 = createListFromPrioritiesArray(prioritiesArray2, 4, m_Fixture.m_TempPool2);
    QVERIFY(m_Fixture.m_List3);

    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 9, ELEMENTS_POOL_SLICE_SIZE - 9);

    sortAscendingByPriority(m_Fixture.m_List3);

    QVERIFY(getListSize(m_Fixture.m_List3) == 4);
    QVERIFY(m_Fixture.m_List3->first && m_Fixture.m_List3->first->priority == 5);
    QVERIFY(m_Fixture.m_List3->last && m_Fixture.m_List3->last->priority == 12);

    clearList(m_Fixture.m_List1, deleteObjectPayload);
    clearList(m_Fixture.m_List3, deleteObjectPayload);

    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 0, ELEMENTS_POOL_SLICE_SIZE);

    // batch 8
    const size_t batchSize8 = ELEMENTS_POOL_SLICE_SIZE + 1;
    ListElement** listElementRefs8 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs8, batchSize8, m_Fixture);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs8, batchSize8);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs8, batchSize8);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, ELEMENTS_POOL_SLICE_SIZE + 1, ELEMENTS_POOL_SLICE_SIZE - 1);

    RELEASE_ELEMENTS(listElementRefs8, batchSize8, m_Fixture.m_TempPool2);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 0, 2 * ELEMENTS_POOL_SLICE_SIZE);

    // batch 9
    const size_t batchSize9 = ELEMENTS_POOL_SLICE_SIZE;
    ListElement** listElementRefs9 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs9, batchSize9, m_Fixture);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs9, batchSize9);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, ELEMENTS_POOL_SLICE_SIZE, ELEMENTS_POOL_SLICE_SIZE);

    shrinkPoolCapacity(m_Fixture.m_TempPool2);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, ELEMENTS_POOL_SLICE_SIZE, ELEMENTS_POOL_SLICE_SIZE);

    RELEASE_ELEMENTS(listElementRefs9, batchSize9, m_Fixture.m_TempPool2);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 0, 2 * ELEMENTS_POOL_SLICE_SIZE);

    // batch 10
    const size_t batchSize10 = ELEMENTS_POOL_SLICE_SIZE - 1;
    ListElement** listElementRefs10 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs10, batchSize10, m_Fixture);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs10, batchSize10);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, ELEMENTS_POOL_SLICE_SIZE - 1, ELEMENTS_POOL_SLICE_SIZE + 1);

    shrinkPoolCapacity(m_Fixture.m_TempPool2);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, ELEMENTS_POOL_SLICE_SIZE - 1, 1);

    // batch 11
    const size_t batchSize11 = ELEMENTS_POOL_SLICE_SIZE + 1;
    ListElement** listElementRefs11 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs11, batchSize11, m_Fixture);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs11, batchSize11);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 2 * ELEMENTS_POOL_SLICE_SIZE, 0);

    shrinkPoolCapacity(m_Fixture.m_TempPool2);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 2 * ELEMENTS_POOL_SLICE_SIZE, 0);

    RELEASE_ELEMENTS(listElementRefs10, batchSize10, m_Fixture.m_TempPool2);
    RELEASE_ELEMENTS(listElementRefs11, batchSize11, m_Fixture.m_TempPool2);

    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 0, 2 * ELEMENTS_POOL_SLICE_SIZE);

    shrinkPoolCapacity(m_Fixture.m_TempPool2);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 0, ELEMENTS_POOL_SLICE_SIZE);
}

void ListElementTests::testAllPoolElementsAquired()
{
    /* First scenario: pool can be extended by adding slice */
    const size_t maxSlicesCount1{2};

    m_Fixture.m_TempPool1 = createListElementsPool(maxSlicesCount1);
    QVERIFY(m_Fixture.m_TempPool1);

    // batch 1
    const size_t batchSize1 = ELEMENTS_POOL_SLICE_SIZE;
    ListElement** listElementRefs1 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs1, batchSize1, m_Fixture);

    bool multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs1, batchSize1);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs1, batchSize1);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, ELEMENTS_POOL_SLICE_SIZE, 0);

    // batch 2
    const size_t batchSize2 = 2;
    ListElement** listElementRefs2 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs2, batchSize2, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs2, batchSize2);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs2, batchSize2);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, ELEMENTS_POOL_SLICE_SIZE + 2, ELEMENTS_POOL_SLICE_SIZE - 2);

    // batch 3
    const size_t batchSize3 = ELEMENTS_POOL_SLICE_SIZE - 1;
    ListElement** listElementRefs3 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs3, batchSize3, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs3, batchSize3);

    QVERIFY(!multipleElementsAquired);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, ELEMENTS_POOL_SLICE_SIZE + 2, ELEMENTS_POOL_SLICE_SIZE - 2);

    RELEASE_ELEMENTS(listElementRefs2, batchSize2, m_Fixture.m_TempPool1);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs3, batchSize3);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs3, batchSize3);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 2 * ELEMENTS_POOL_SLICE_SIZE - 1, 1);

    ListElement* element = aquireElement(m_Fixture.m_TempPool1);
    QVERIFY(element);
    CHECK_AQUIRED_ELEMENTS(&element, 1);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 2 * ELEMENTS_POOL_SLICE_SIZE, 0);

    ListElement* anotherElement = aquireElement(m_Fixture.m_TempPool1);
    QVERIFY(!anotherElement); // no more available elements, all elements aquired and maximum number of slices reached

    // batch 4
    const size_t batchSize4 = 1;
    ListElement** listElementRefs4 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs4, batchSize4, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs4, batchSize4);

    QVERIFY(!multipleElementsAquired); // no more available elements, all elements aquired and maximum number of slices reached
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 2 * ELEMENTS_POOL_SLICE_SIZE, 0);

    RELEASE_ELEMENTS(&element, 1, m_Fixture.m_TempPool1);
    element = nullptr;
    anotherElement = aquireElement(m_Fixture.m_TempPool1);

    QVERIFY(anotherElement);
    CHECK_AQUIRED_ELEMENTS(&anotherElement, 1);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 2 * ELEMENTS_POOL_SLICE_SIZE, 0);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs4, batchSize4);

    QVERIFY(!multipleElementsAquired); // no more available elements, all elements aquired and maximum number of slices reached
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 2 * ELEMENTS_POOL_SLICE_SIZE, 0);

    RELEASE_ELEMENTS(&anotherElement, 1, m_Fixture.m_TempPool1);
    anotherElement = nullptr;
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs4, batchSize4);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs4, batchSize4);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 2 * ELEMENTS_POOL_SLICE_SIZE, 0);

    RELEASE_ELEMENTS(listElementRefs1, batchSize1, m_Fixture.m_TempPool1);
    RELEASE_ELEMENTS(listElementRefs3, batchSize3, m_Fixture.m_TempPool1);
    RELEASE_ELEMENTS(listElementRefs4, batchSize4, m_Fixture.m_TempPool1);

    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 0, 2 * ELEMENTS_POOL_SLICE_SIZE);

    shrinkPoolCapacity(m_Fixture.m_TempPool1);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool1, 0, ELEMENTS_POOL_SLICE_SIZE);

    /* Second scenario: single slice pool */
    const size_t maxSlicesCount2{1};

    m_Fixture.m_TempPool2 = createListElementsPool(maxSlicesCount2);
    QVERIFY(m_Fixture.m_TempPool2);

    // batch 5
    const size_t batchSize5 = 1;
    ListElement** listElementRefs5 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs5, batchSize5, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs5, batchSize5);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs5, batchSize5);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 1, ELEMENTS_POOL_SLICE_SIZE - 1);

    const size_t batchSize6 = ELEMENTS_POOL_SLICE_SIZE - 1;
    ListElement** listElementRefs6 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs6, batchSize6, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs6, batchSize6);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs6, batchSize6);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, ELEMENTS_POOL_SLICE_SIZE, 0);

    element = aquireElement(m_Fixture.m_TempPool2);
    QVERIFY(!element); // no more available elements, slice is fully aquired

    RELEASE_ELEMENTS(listElementRefs5, batchSize5, m_Fixture.m_TempPool2);

    element = aquireElement(m_Fixture.m_TempPool2);

    QVERIFY(element);
    CHECK_AQUIRED_ELEMENTS(&element, 1);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, ELEMENTS_POOL_SLICE_SIZE, 0);

    RELEASE_ELEMENTS(&element, 1, m_Fixture.m_TempPool2);
    RELEASE_ELEMENTS(listElementRefs6, batchSize6, m_Fixture.m_TempPool2);

    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 0, ELEMENTS_POOL_SLICE_SIZE);

    shrinkPoolCapacity(m_Fixture.m_TempPool2);
    CHECK_AQUIRED_AND_AVAILABLE_ELEMENTS_COUNT(m_Fixture.m_TempPool2, 0, ELEMENTS_POOL_SLICE_SIZE);
}

void ListElementTests::testAssignRemoveObject()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Fixture.m_Pool);

    m_Fixture.m_List1 = createEmptyList(pool);

    // create elements and assign object content (type and payload)
    assignObjectContentToListElement(createAndAppendToList(m_Fixture.m_List1, 3), INTEGER, createIntegerPayload(5));
    assignObjectContentToListElement(createAndAppendToList(m_Fixture.m_List1, 1), DECIMAL, createDecimalPayload(1.25));
    assignObjectContentToListElement(createAndAppendToList(m_Fixture.m_List1, 2), POINT, createPointPayload(3, 4));
    assignObjectContentToListElement(createAndAppendToList(m_Fixture.m_List1, 4), SEGMENT, createSegmentPayload(-3, 5, 8, 9));
    assignObjectContentToListElement(createAndAppendToList(m_Fixture.m_List1, 4), LOCAL_CONDITIONS, createLocalConditionsPayload(4, -5, 32, 80.4));
    Q_UNUSED(createAndPrependToList(m_Fixture.m_List1, 10));

    QVERIFY(getListSize(m_Fixture.m_List1) == 6);

    ListIterator it = lbegin(m_Fixture.m_List1);
    QVERIFY2(it.current->object.type == -1 && !it.current->object.payload, "Default object is incorrect (should be empty)");

    lnext(&it);
    QVERIFY2(it.current->object.payload &&
             it.current->object.type == INTEGER &&
             *((int*)(it.current->object.payload)) == 5, "Object has been incorrectly assigned");

    lnext(&it);
    QVERIFY2(it.current->object.payload &&
             it.current->object.type == DECIMAL &&
             areDecimalNumbersEqual(*((double*)(it.current->object.payload)), 1.25), "Object has been incorrectly assigned");

    lnext(&it);
    QVERIFY2(it.current->object.payload &&
             it.current->object.type == POINT &&
             ((Point*)it.current->object.payload)->x == 3 &&
             ((Point*)it.current->object.payload)->y == 4, "Object has been incorrectly assigned");

    lnext(&it);
    QVERIFY2(it.current->object.payload &&
             it.current->object.type == SEGMENT &&
             ((Segment*)(it.current->object.payload))->start.x == -3 &&
             ((Segment*)(it.current->object.payload))->start.y == 5 &&
             ((Segment*)(it.current->object.payload))->stop.x == 8 &&
             ((Segment*)(it.current->object.payload))->stop.y == 9, "Object has been incorrectly assigned");

    lnext(&it);
    QVERIFY2(it.current->object.payload &&
             it.current->object.type == LOCAL_CONDITIONS &&
             ((LocalConditions*)(it.current->object.payload))->position.x == 4 &&
             ((LocalConditions*)(it.current->object.payload))->position.y == -5 &&
             ((LocalConditions*)(it.current->object.payload))->temperature == 32 &&
             areDecimalNumbersEqual(((LocalConditions*)(it.current->object.payload))->humidity, 80.4), "Object has been incorrectly assigned");

    QVERIFY(getListSize(m_Fixture.m_List1) == 6);

    // detach object content from each list element, delete payload
    it = lbegin(m_Fixture.m_List1);
    Object removedObject = detachContentFromListElement(it.current);

    QVERIFY(!removedObject.payload && removedObject.type == -1);

    lnext(&it);
    removedObject = detachContentFromListElement(it.current);

    QVERIFY2(removedObject.payload &&
             removedObject.type == INTEGER &&
             *((int*)(removedObject.payload)) == 5, "Incorrect object removal from list element");

    deleteObjectPayload(&removedObject);

    lnext(&it);
    removedObject = detachContentFromListElement(it.current);

    QVERIFY2(removedObject.payload &&
             removedObject.type == DECIMAL &&
             areDecimalNumbersEqual(*((double*)(removedObject.payload)), 1.25), "Incorrect object removal from list element");

    deleteObjectPayload(&removedObject);

    lnext(&it);
    removedObject = detachContentFromListElement(it.current);

    QVERIFY2(removedObject.payload &&
            removedObject.type == POINT &&
            ((Point*)removedObject.payload)->x == 3 &&
            ((Point*)removedObject.payload)->y == 4, "Incorrect object removal from list element");

    deleteObjectPayload(&removedObject);

    lnext(&it);
    removedObject = detachContentFromListElement(it.current);

    QVERIFY2(removedObject.payload &&
             removedObject.type == SEGMENT &&
             ((Segment*)(removedObject.payload))->start.x == -3 &&
             ((Segment*)(removedObject.payload))->start.y == 5 &&
             ((Segment*)(removedObject.payload))->stop.x == 8 &&
             ((Segment*)(removedObject.payload))->stop.y == 9, "Incorrect object removal from list element");

    deleteObjectPayload(&removedObject);

    lnext(&it);
    removedObject = detachContentFromListElement(it.current);

    QVERIFY2(removedObject.payload &&
             removedObject.type == LOCAL_CONDITIONS &&
             ((LocalConditions*)(removedObject.payload))->position.x == 4 &&
             ((LocalConditions*)(removedObject.payload))->position.y == -5 &&
             ((LocalConditions*)(removedObject.payload))->temperature == 32 &&
             areDecimalNumbersEqual(((LocalConditions*)(removedObject.payload))->humidity, 80.4), "Incorrect object removal from list element");

    deleteObjectPayload(&removedObject);

    QVERIFY(getListSize(m_Fixture.m_List1) == 6);
    it = lbegin(m_Fixture.m_List1);

    while(!areIteratorsEqual(it, lend(m_Fixture.m_List1)))
    {
        QVERIFY(!it.current->object.payload && it.current->object.type == -1);
        lnext(&it);
    }
}

void ListElementTests::testCustomCopyObject()
{
    const auto clearObject = [](ListElement* element) {
        Object removedObject = detachContentFromListElement(element);
        deleteObjectPayload(&removedObject);
    };

    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Fixture.m_Pool);

    ListElement* source = pool ? aquireElement(pool) : createListElement();
    ListElement* destination = pool ? aquireElement(pool) : createListElement();
    m_Fixture.markListElementForCleanup(source, pool != nullptr);
    m_Fixture.markListElementForCleanup(destination, pool != nullptr);

    QVERIFY(source && source->priority == 0 && !source->object.payload && source->object.type == -1);
    QVERIFY(destination && source->priority == 0 && !destination->object.payload && destination->object.type == -1);

    // integer
    assignObjectContentToListElement(source, INTEGER, createIntegerPayload(5));
    customCopyObject(source, destination);

    QVERIFY2(destination->object.payload &&
             destination->object.type == INTEGER &&
             *((int*)(destination->object.payload)) == 5, "Object content has been incorrectly copied to destination");

    clearObject(source);
    clearObject(destination);

    // decimal
    assignObjectContentToListElement(source, DECIMAL, createDecimalPayload(1.25));
    customCopyObject(source, destination);

    QVERIFY2(destination->object.payload &&
             destination->object.type == DECIMAL &&
             areDecimalNumbersEqual(*((double*)(destination->object.payload)), 1.25), "Object content has been incorrectly copied to destination");

    clearObject(source);
    clearObject(destination);

    // point
    assignObjectContentToListElement(source, POINT, createPointPayload(3, 4));
    customCopyObject(source, destination);

    QVERIFY2(destination->object.payload &&
             destination->object.type == POINT &&
             ((Point*)destination->object.payload)->x == 3 &&
             ((Point*)destination->object.payload)->y == 4, "Object content has been incorrectly copied to destination");

    clearObject(source);
    clearObject(destination);

    Object removedSourceObject = detachContentFromListElement(source);
    Object removedDestinationObject = detachContentFromListElement(destination);
    deleteObjectPayload(&removedSourceObject);
    deleteObjectPayload(&removedDestinationObject);

    // segment
    assignObjectContentToListElement(source, SEGMENT, createSegmentPayload(-3, 5, 8, 9));
    customCopyObject(source, destination);

    QVERIFY2(destination->object.payload &&
             destination->object.type == SEGMENT &&
             ((Segment*)(destination->object.payload))->start.x == -3 &&
             ((Segment*)(destination->object.payload))->start.y == 5 &&
             ((Segment*)(destination->object.payload))->stop.x == 8 &&
             ((Segment*)(destination->object.payload))->stop.y == 9, "Object content has been incorrectly copied to destination");

    clearObject(source);
    clearObject(destination);

    // local conditions
    assignObjectContentToListElement(source, LOCAL_CONDITIONS, createLocalConditionsPayload(4, -5, 32, 80.4));
    customCopyObject(source, destination);

    QVERIFY2(destination->object.payload &&
             destination->object.type == LOCAL_CONDITIONS &&
             ((LocalConditions*)(destination->object.payload))->position.x == 4 &&
             ((LocalConditions*)(destination->object.payload))->position.y == -5 &&
             ((LocalConditions*)(destination->object.payload))->temperature == 32 &&
             areDecimalNumbersEqual(((LocalConditions*)(destination->object.payload))->humidity, 80.4), "Object content has been incorrectly copied to destination");

    clearObject(source);
    clearObject(destination);
}

void ListElementTests::initTestCase_data()
{
    m_Fixture.init();

    ListElementsPool* p_NullPool{nullptr};

    QTest::addColumn<ListElementsPool*>("pool");

    QTest::newRow("allocation from pool") << m_Fixture.m_Pool;
    QTest::newRow("no pool allocation") << p_NullPool;
}

void ListElementTests::cleanupTestCase()
{
    QVERIFY(m_Fixture.hasInitialState());
    m_Fixture.destroy();
}

void ListElementTests::init()
{
    QVERIFY(m_Fixture.hasInitialState());
}

void ListElementTests::cleanup()
{
    m_Fixture.resetToInitialState();
}

QTEST_APPLESS_MAIN(ListElementTests)

#include "tst_listelementtests.moc"
