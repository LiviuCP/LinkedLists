#include <QTest>

#include "listtestfixture.h"
#include "linkedlist.h"
#include "listelementspool.h"
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
    void testAssignRemoveObject();

    void initTestCase_data();
    void cleanupTestCase();
    void init();
    void cleanup();

private:
    ListTestFixture m_Fixture;
};

void ListElementTests::testAquiringSinglePoolElement()
{
    m_Fixture.m_TempPool1 = createListElementsPool();
    QVERIFY(m_Fixture.m_TempPool1);

    // in this case the temp pool is assigned to m_Fixture.m_List1 only to prevent elements deletion from list (in case of test fail); the aquiring/releasing of elements occurs externally
    m_Fixture.m_List1 = createEmptyList(m_Fixture.m_TempPool1);
    QVERIFY(m_Fixture.m_List1);

    const size_t initialPooledElementsCount = getAvailableElementsCount(m_Fixture.m_TempPool1);

    QVERIFY(initialPooledElementsCount == ELEMENTS_POOL_SLICE_SIZE);
    QVERIFY(initialPooledElementsCount >= 8); // safety net to prevent overflows, see below

    ListElement* element = aquireElement(m_Fixture.m_TempPool1);

    CHECK_AQUIRED_ELEMENTS(&element, 1);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool1) == initialPooledElementsCount - 1);

    prependToList(m_Fixture.m_List1, element);

    element = aquireElement(m_Fixture.m_TempPool1);

    CHECK_AQUIRED_ELEMENTS(&element, 1);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool1) == initialPooledElementsCount - 2);

    appendToList(m_Fixture.m_List1, element);

    QVERIFY(getListSize(m_Fixture.m_List1) == 2);

    for (size_t index = 0; index < initialPooledElementsCount - 3; ++index)
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

    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool1) == 1);
    QVERIFY(getListSize(m_Fixture.m_List1) == initialPooledElementsCount - 1);

    element = aquireElement(m_Fixture.m_TempPool1);

    CHECK_AQUIRED_ELEMENTS(&element, 1);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool1) == 0);

    appendToList(m_Fixture.m_List1, element);
    QVERIFY(getListSize(m_Fixture.m_List1) == initialPooledElementsCount);

    element = aquireElement(m_Fixture.m_TempPool1);

    QVERIFY(element);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE - 1);

    bool released = false;

    released = releaseElement(element, m_Fixture.m_TempPool1);
    QVERIFY(released && getAvailableElementsCount(m_Fixture.m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE);

    element = removeLastListElement(m_Fixture.m_List1);
    QVERIFY(element);

    released = releaseElement(element, m_Fixture.m_TempPool1);
    QVERIFY(released && getAvailableElementsCount(m_Fixture.m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE + 1);

    element = removeFirstListElement(m_Fixture.m_List1);
    QVERIFY(element);

    released = releaseElement(element, m_Fixture.m_TempPool1);
    QVERIFY(released && getAvailableElementsCount(m_Fixture.m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE + 2);

    // releasing an element twice is not allowed
    released = releaseElement(element, m_Fixture.m_TempPool1);
    QVERIFY(!released);

    ListElement* newElement = createListElement();
    m_Fixture.markListElementForCleanup(newElement, false);

    // releasing an element that doesn't belong to pool is not allowed
    released = releaseElement(newElement, m_Fixture.m_TempPool1);
    QVERIFY(!released && getAvailableElementsCount(m_Fixture.m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE + 2);

    element = aquireElement(m_Fixture.m_TempPool1);

    CHECK_AQUIRED_ELEMENTS(&element, 1);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE + 1);

    released = releaseElement(element, m_Fixture.m_TempPool1);
    QVERIFY(released && getAvailableElementsCount(m_Fixture.m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE + 2);

    const size_t listElementsCount = getListSize(m_Fixture.m_List1);
    QVERIFY(listElementsCount == initialPooledElementsCount - 2);

    for (size_t index = 0; index < listElementsCount; ++index)
    {
        element = index % 2 == 0 ? removeLastListElement(m_Fixture.m_List1) : removeFirstListElement(m_Fixture.m_List1);
        QVERIFY(element);
        released = releaseElement(element, m_Fixture.m_TempPool1);
        QVERIFY(released);
        element = nullptr;
    }

    const size_t finalPooledElementsCount = initialPooledElementsCount + ELEMENTS_POOL_SLICE_SIZE;

    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool1) == finalPooledElementsCount);
    QVERIFY(isEmptyList(m_Fixture.m_List1));

    released = releaseElement(nullptr, m_Fixture.m_TempPool1);
    QVERIFY(!released && getAvailableElementsCount(m_Fixture.m_TempPool1) == finalPooledElementsCount);
}

void ListElementTests::testAquiringMultiplePoolElements()
{
    m_Fixture.m_TempPool1 = createListElementsPool();
    QVERIFY(m_Fixture.m_TempPool1);

    // batch 1
    const size_t batchSize1 = 3;
    ListElement** listElementRefs1 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs1, batchSize1, m_Fixture);

    bool multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs1, batchSize1);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE - 3);
    CHECK_AQUIRED_ELEMENTS(listElementRefs1, batchSize1);

    // batch 2
    const size_t batchSize2 = ELEMENTS_POOL_SLICE_SIZE - 2;
    ListElement** listElementRefs2 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs2, batchSize2, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs2, batchSize2);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE - 1);
    CHECK_AQUIRED_ELEMENTS(listElementRefs2, batchSize2);

    RELEASE_ELEMENTS(listElementRefs2, batchSize2, m_Fixture.m_TempPool1);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool1) == 2 * ELEMENTS_POOL_SLICE_SIZE - 3);

    RELEASE_ELEMENTS(listElementRefs1, batchSize1, m_Fixture.m_TempPool1);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool1) == 2 * ELEMENTS_POOL_SLICE_SIZE);

    // batch 3
    const size_t batchSize3 = 2 * ELEMENTS_POOL_SLICE_SIZE;
    ListElement** listElementRefs3 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs3, batchSize3, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs3, batchSize3);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool1) == 0);
    CHECK_AQUIRED_ELEMENTS(listElementRefs3, batchSize3);

    RELEASE_ELEMENTS(listElementRefs3, batchSize3, m_Fixture.m_TempPool1);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool1) == 2 * ELEMENTS_POOL_SLICE_SIZE);

    // batch 4
    const size_t batchSize4 = 1;
    ListElement** listElementRefs4 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs4, batchSize4, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs4, batchSize4);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool1) == 2 * ELEMENTS_POOL_SLICE_SIZE - 1);
    CHECK_AQUIRED_ELEMENTS(listElementRefs4, batchSize4);

    // batch 5
    size_t batchSize5 = 2 * ELEMENTS_POOL_SLICE_SIZE + 1;
    ListElement** listElementRefs5 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs5, batchSize5, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs5, batchSize5);
    QVERIFY(!multipleElementsAquired);

    --batchSize5;

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs5, batchSize5);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE - 1);
    CHECK_AQUIRED_ELEMENTS(listElementRefs5, batchSize5);

    RELEASE_ELEMENTS(listElementRefs4, batchSize4, m_Fixture.m_TempPool1);
    RELEASE_ELEMENTS(listElementRefs5, batchSize5, m_Fixture.m_TempPool1);

    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool1) == 3 * ELEMENTS_POOL_SLICE_SIZE);

    // batch 6
    const size_t batchSize6 = ELEMENTS_POOL_SLICE_SIZE + ELEMENTS_POOL_SLICE_SIZE / 2;
    ListElement** listElementRefs6 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs6, batchSize6, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs6, batchSize6);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool1) == batchSize6);
    CHECK_AQUIRED_ELEMENTS(listElementRefs6, batchSize6);

    // batch 7
    const size_t batchSize7 = ELEMENTS_POOL_SLICE_SIZE;
    ListElement** listElementRefs7 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs7, batchSize7, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs7, batchSize7);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE / 2);
    CHECK_AQUIRED_ELEMENTS(listElementRefs7, batchSize7);

    // batch 8
    size_t batchSize8 = ELEMENTS_POOL_SLICE_SIZE + ELEMENTS_POOL_SLICE_SIZE / 2 + 1;
    ListElement** listElementRefs8 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs8, batchSize8, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs8, batchSize8);

    QVERIFY(!multipleElementsAquired);

    --batchSize8;

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs8, batchSize8);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool1) == 0);
    CHECK_AQUIRED_ELEMENTS(listElementRefs8, batchSize8);

    // batch 9
    const size_t batchSize9 = 5;
    ListElement** listElementRefs9 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs9, batchSize9, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs9, batchSize9);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE - 5);
    CHECK_AQUIRED_ELEMENTS(listElementRefs9, batchSize9);

    RELEASE_ELEMENTS(listElementRefs6, batchSize6, m_Fixture.m_TempPool1);
    RELEASE_ELEMENTS(listElementRefs7, batchSize7, m_Fixture.m_TempPool1);
    RELEASE_ELEMENTS(listElementRefs8, batchSize8, m_Fixture.m_TempPool1);
    RELEASE_ELEMENTS(listElementRefs9, batchSize9, m_Fixture.m_TempPool1);

    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool1) == 5 * ELEMENTS_POOL_SLICE_SIZE);

    // combined test: aquire single element and aquire multiple elements
    m_Fixture.m_TempPool2 = createListElementsPool();
    QVERIFY(m_Fixture.m_TempPool2);

    ListElement* element = aquireElement(m_Fixture.m_TempPool2);
    QVERIFY(element);
    CHECK_AQUIRED_ELEMENTS(&element, 1);

    const size_t batchSize10 = ELEMENTS_POOL_SLICE_SIZE - 1;
    ListElement** listElementRefs10 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs10, batchSize10, m_Fixture);

    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs10, batchSize10);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool2) == 0);
    CHECK_AQUIRED_ELEMENTS(listElementRefs10, batchSize10);

    RELEASE_ELEMENTS(listElementRefs10, batchSize10, m_Fixture.m_TempPool2);
    RELEASE_ELEMENTS(&element, 1, m_Fixture.m_TempPool2);

    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE);

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
    // first pool: simple test for optimizing capacity
    m_Fixture.m_TempPool1 = createListElementsPool();
    QVERIFY(m_Fixture.m_TempPool1);

    const size_t batchSize1 = ELEMENTS_POOL_SLICE_SIZE + 1;
    ListElement** listElementRefs1 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs1, batchSize1, m_Fixture);
    bool multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs1, batchSize1);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE - 1);

    const size_t batchSize2 = 5;
    ListElement** listElementRefs2 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs2, batchSize2, m_Fixture);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool1, listElementRefs2, batchSize2);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE - 6);

    RELEASE_ELEMENTS(listElementRefs1, batchSize1, m_Fixture.m_TempPool1);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool1) == 2 * ELEMENTS_POOL_SLICE_SIZE - 5);

    shrinkPoolCapacity(m_Fixture.m_TempPool1);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE - 5);

    RELEASE_ELEMENTS(listElementRefs2, batchSize2, m_Fixture.m_TempPool1);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE);

    shrinkPoolCapacity(m_Fixture.m_TempPool1);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE);

    // second pool: test that the integrity of the aquired elements is preserved when optimization is performed
    m_Fixture.m_TempPool2 = createListElementsPool();
    QVERIFY(m_Fixture.m_TempPool2);

    const size_t batchSize3 = ELEMENTS_POOL_SLICE_SIZE - 1;
    ListElement** listElementRefs3 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs3, batchSize3, m_Fixture);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs3, batchSize3);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool2) == 1);

    const size_t batchSize4 = ELEMENTS_POOL_SLICE_SIZE + 1;
    ListElement** listElementRefs4 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs4, batchSize4, m_Fixture);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs4, batchSize4);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool2) == 0);

    m_Fixture.m_List1 = createEmptyList(m_Fixture.m_TempPool2);
    QVERIFY(m_Fixture.m_List1);

    (void)createAndAppendToList(m_Fixture.m_List1, 5);
    (void)createAndPrependToList(m_Fixture.m_List1, 4);
    (void)createAndAppendToList(m_Fixture.m_List1, 2);
    (void)createAndAppendToList(m_Fixture.m_List1, 9);

    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE - 4);

    const size_t batchSize5 = ELEMENTS_POOL_SLICE_SIZE - 3;
    ListElement** listElementRefs5 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs5, batchSize5, m_Fixture);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs5, batchSize5);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE - 1);

    const size_t prioritiesArray1[8] = {2, 4, 5, 8, 7, 0, 2, 3};
    m_Fixture.m_List2 = createListFromPrioritiesArray(prioritiesArray1, 8, m_Fixture.m_TempPool2);
    QVERIFY(m_Fixture.m_List2);

    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE - 9);

    const size_t batchSize6 = ELEMENTS_POOL_SLICE_SIZE - 4;
    ListElement** listElementRefs6 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs6, batchSize6, m_Fixture);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs6, batchSize6);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE - 5);

    RELEASE_ELEMENTS(listElementRefs4, batchSize4, m_Fixture.m_TempPool2);
    RELEASE_ELEMENTS(listElementRefs6, batchSize6, m_Fixture.m_TempPool2);

    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == 3 * ELEMENTS_POOL_SLICE_SIZE - 8);

    shrinkPoolCapacity(m_Fixture.m_TempPool2);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE - 8);

    sortDescendingByPriority(m_Fixture.m_List2);

    QVERIFY(getListSize(m_Fixture.m_List2) == 8);
    QVERIFY(m_Fixture.m_List2->first && m_Fixture.m_List2->first->priority == 8);
    QVERIFY(m_Fixture.m_List2->last && m_Fixture.m_List2->last->priority == 0);

    clearList(m_Fixture.m_List2, deleteObjectPayload);

    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE);

    shrinkPoolCapacity(m_Fixture.m_TempPool2);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE);

    RELEASE_ELEMENTS(listElementRefs3, batchSize3, m_Fixture.m_TempPool2);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == 2 * ELEMENTS_POOL_SLICE_SIZE - 1);

    shrinkPoolCapacity(m_Fixture.m_TempPool2);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE - 1);

    const size_t batchSize7 = ELEMENTS_POOL_SLICE_SIZE + 4;
    ListElement** listElementRefs7 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs7, batchSize7, m_Fixture);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs7, batchSize7);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE - 5);
    CHECK_AQUIRED_ELEMENTS(listElementRefs7, batchSize7);

    shrinkPoolCapacity(m_Fixture.m_TempPool2);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE - 5);

    QVERIFY(m_Fixture.m_List1->first && m_Fixture.m_List1->first->priority == 4);
    QVERIFY(m_Fixture.m_List1->last && m_Fixture.m_List1->last->priority == 9);

    RELEASE_ELEMENTS(listElementRefs5, batchSize5, m_Fixture.m_TempPool2);
    RELEASE_ELEMENTS(listElementRefs7, batchSize7, m_Fixture.m_TempPool2);

    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == 3 * ELEMENTS_POOL_SLICE_SIZE - 4);

    shrinkPoolCapacity(m_Fixture.m_TempPool2);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE - 4);

    QVERIFY(getListSize(m_Fixture.m_List1) == 4);
    ListIterator it = lbegin(m_Fixture.m_List1);
    lnext(&it);

    QVERIFY(it.current && it.current->priority == 5);

    (void)createAndInsertBefore(it, 20);

    QVERIFY(getListSize(m_Fixture.m_List1) == 5);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE - 5);
    QVERIFY(m_Fixture.m_List1->first && m_Fixture.m_List1->first->next && m_Fixture.m_List1->first->next->priority == 20);

    const size_t prioritiesArray2[4] = {9, 5, 12, 7};
    m_Fixture.m_List3 = createListFromPrioritiesArray(prioritiesArray2, 4, m_Fixture.m_TempPool2);
    QVERIFY(m_Fixture.m_List3);

    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE - 9);

    sortAscendingByPriority(m_Fixture.m_List3);

    QVERIFY(getListSize(m_Fixture.m_List3) == 4);
    QVERIFY(m_Fixture.m_List3->first && m_Fixture.m_List3->first->priority == 5);
    QVERIFY(m_Fixture.m_List3->last && m_Fixture.m_List3->last->priority == 12);

    clearList(m_Fixture.m_List1, deleteObjectPayload);
    clearList(m_Fixture.m_List3, deleteObjectPayload);

    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE);

    const size_t batchSize8 = ELEMENTS_POOL_SLICE_SIZE + 1;
    ListElement** listElementRefs8 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs8, batchSize8, m_Fixture);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs8, batchSize8);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE - 1);
    CHECK_AQUIRED_ELEMENTS(listElementRefs8, batchSize8);

    RELEASE_ELEMENTS(listElementRefs8, batchSize8, m_Fixture.m_TempPool2);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == 2 * ELEMENTS_POOL_SLICE_SIZE);

    const size_t batchSize9 = ELEMENTS_POOL_SLICE_SIZE;
    ListElement** listElementRefs9 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs9, batchSize9, m_Fixture);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs9, batchSize9);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE);

    shrinkPoolCapacity(m_Fixture.m_TempPool2);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE);

    RELEASE_ELEMENTS(listElementRefs9, batchSize9, m_Fixture.m_TempPool2);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == 2 * ELEMENTS_POOL_SLICE_SIZE);

    const size_t batchSize10 = ELEMENTS_POOL_SLICE_SIZE - 1;
    ListElement** listElementRefs10 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs10, batchSize10, m_Fixture);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs10, batchSize10);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE + 1);

    shrinkPoolCapacity(m_Fixture.m_TempPool2);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == 1);

    const size_t batchSize11 = ELEMENTS_POOL_SLICE_SIZE + 1;
    ListElement** listElementRefs11 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs11, batchSize11, m_Fixture);
    multipleElementsAquired = aquireElements(m_Fixture.m_TempPool2, listElementRefs11, batchSize11);

    QVERIFY(multipleElementsAquired && getAvailableElementsCount(m_Fixture.m_TempPool2) == 0);

    shrinkPoolCapacity(m_Fixture.m_TempPool2);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == 0);

    RELEASE_ELEMENTS(listElementRefs10, batchSize10, m_Fixture.m_TempPool2);
    RELEASE_ELEMENTS(listElementRefs11, batchSize11, m_Fixture.m_TempPool2);

    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == 2 * ELEMENTS_POOL_SLICE_SIZE);

    shrinkPoolCapacity(m_Fixture.m_TempPool2);
    QVERIFY(getAvailableElementsCount(m_Fixture.m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE);
}

void ListElementTests::testAssignRemoveObject()
{
    QFETCH_GLOBAL(ListElementsPool*, pool);
    QVERIFY(!pool || pool == m_Fixture.m_Pool);

    m_Fixture.m_List1 = createEmptyList(pool);

    // Point
    Point* point = static_cast<Point*>(malloc(sizeof(Point)));
    point->x = 3;
    point->y = 4;
    assignObjectContentToListElement(createAndAppendToList(m_Fixture.m_List1, 2), POINT, static_cast<void*>(point));
    point = nullptr;
    // int
    int* distance = static_cast<int*>(malloc(sizeof(int)));
    *distance = 5;
    assignObjectContentToListElement(createAndAppendToList(m_Fixture.m_List1, 3), INTEGER, static_cast<void*>(distance));
    distance = nullptr;
    // float
    double* angle = static_cast<double*>(malloc(sizeof(double)));
    *angle = 1.25;
    assignObjectContentToListElement(createAndAppendToList(m_Fixture.m_List1, 1), DECIMAL, static_cast<void*>(angle));
    angle = nullptr;
    // no object
    Q_UNUSED(createAndPrependToList(m_Fixture.m_List1, 10));

    QVERIFY(getListSize(m_Fixture.m_List1) == 4);

    ListIterator it = lbegin(m_Fixture.m_List1);
    QVERIFY2(it.current->object.type == -1 && it.current->object.payload == nullptr, "Default object is incorrect (should be empty)");
    lnext(&it);
    QVERIFY2(it.current->object.type == POINT && (static_cast<Point*>(it.current->object.payload))->x == 3
             && (static_cast<Point*>(it.current->object.payload))->y == 4, "Object has been incorrectly assigned");
    lnext(&it);
    QVERIFY2(it.current->object.type == INTEGER && *(static_cast<int*>(it.current->object.payload)) == 5, "Object has been incorrectly assigned");
    lnext(&it);
    QVERIFY2(it.current->object.type == DECIMAL && areDecimalNumbersEqual(*(static_cast<double*>(it.current->object.payload)), 1.25), "Object has been incorrectly assigned");

    Object* removedObject = static_cast<Object*>(detachContentFromListElement(getListElementAtIndex(m_Fixture.m_List1, 2)));
    QVERIFY2(getListElementAtIndex(m_Fixture.m_List1, 2)->object.type == -1 &&
             getListElementAtIndex(m_Fixture.m_List1, 2)->object.payload == nullptr &&
             removedObject->type == INTEGER &&
             (*(static_cast<int*>(removedObject->payload)) == 5), "Incorrect object removal from list element");

    deleteObjectPayload(removedObject);
    free(removedObject);
    removedObject = nullptr;

    QVERIFY(getListSize(m_Fixture.m_List1) == 4);
}

void ListElementTests::initTestCase_data()
{
    m_Fixture.init();
    QVERIFY(m_Fixture.hasInitialState());

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
    QVERIFY(m_Fixture.hasInitialState());
}

QTEST_APPLESS_MAIN(ListElementTests)

#include "tst_listelementtests.moc"
