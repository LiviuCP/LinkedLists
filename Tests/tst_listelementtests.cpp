#include <QTest>

#include "linkedlist.h"
#include "codeutils.h"

#define ALLOC_LIST_ELEMENT_REFS(listElementRefs, count) \
    listElementRefs = (ListElement**)malloc(count * sizeof(ListElement*)); \
    _markListElementRefsForDeletion(listElementRefs); \
\
    QVERIFY(listElementRefs); \
\
    for (size_t index = 0; index < count; ++index) \
    { \
        listElementRefs[index] = nullptr; \
    } \

#define DELETE_LIST_ELEMENTS_POOL(elementsPool) \
    if (elementsPool) \
    { \
        deleteListElementsPool(elementsPool); \
        elementsPool = nullptr; \
    }

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

public:
    explicit ListElementTests();

private slots:
    void testAquiringSinglePoolElement();
    void testAquiringMultiplePoolElements();

    void cleanupTestCase();
    void init();
    void cleanup();

private:
    void _markListElementForDeletion(ListElement* element);
    void _markListElementRefsForDeletion(ListElement** elementRefs);

    // temporary pools, to be cleaned up (if necessary) after each test run
    ListElementsPool* m_TempPool1;
    ListElementsPool* m_TempPool2;

    List* m_List1;

    // lists that are not test class members marked for deletion (deleted when running cleanup()) - do not add members (e.g. m_pList1) here
    std::vector<List*> m_ListsMarkedForDeletion;

    // elements collected from test cases that need to be freed
    std::vector<ListElement*> m_ListElementsMarkedForDeletion;

    // arrays of element refs collected from test cases that need to be freed
    std::vector<ListElement**> m_ListElementRefsMarkedForDeletion;
};

ListElementTests::ListElementTests()
    : m_TempPool1{nullptr}
    , m_TempPool2{nullptr}
    , m_List1{nullptr}
{
}

void ListElementTests::testAquiringSinglePoolElement()
{
    m_TempPool1 = createListElementsPool();
    QVERIFY(m_TempPool1);

    // in this case the temp pool is assigned to m_List1 only to prevent elements deletion from list (in case of test fail); the aquiring/releasing of elements occurs externally
    m_List1 = createEmptyList(m_TempPool1);
    QVERIFY(m_List1);

    const size_t initialPooledElementsCount = getAvailableElementsCount(m_TempPool1);

    QVERIFY(initialPooledElementsCount == ELEMENTS_POOL_SLICE_SIZE);
    QVERIFY(initialPooledElementsCount >= 8); // safety net to prevent overflows, see below

    ListElement* element = aquireElement(m_TempPool1);

    CHECK_AQUIRED_ELEMENTS(&element, 1);
    QVERIFY(getAvailableElementsCount(m_TempPool1) == initialPooledElementsCount - 1);

    prependToList(m_List1, element);

    element = aquireElement(m_TempPool1);

    CHECK_AQUIRED_ELEMENTS(&element, 1);
    QVERIFY(getAvailableElementsCount(m_TempPool1) == initialPooledElementsCount - 2);

    appendToList(m_List1, element);

    QVERIFY(getListSize(m_List1) == 2);

    for (size_t index = 0; index < initialPooledElementsCount - 3; ++index)
    {
        element = aquireElement(m_TempPool1);
        QVERIFY(element);

        if (index % 2)
        {
            prependToList(m_List1, element);
        }
        else
        {
            appendToList(m_List1, element);
        }
    }

    QVERIFY(getAvailableElementsCount(m_TempPool1) == 1);
    QVERIFY(getListSize(m_List1) == initialPooledElementsCount - 1);

    element = aquireElement(m_TempPool1);

    CHECK_AQUIRED_ELEMENTS(&element, 1);
    QVERIFY(getAvailableElementsCount(m_TempPool1) == 0);

    appendToList(m_List1, element);
    QVERIFY(getListSize(m_List1) == initialPooledElementsCount);

    element = aquireElement(m_TempPool1);

    QVERIFY(element);
    QVERIFY(getAvailableElementsCount(m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE - 1);

    bool released = false;

    released = releaseElement(element, m_TempPool1);
    QVERIFY(released && getAvailableElementsCount(m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE);

    element = removeLastListElement(m_List1);
    QVERIFY(element);

    released = releaseElement(element, m_TempPool1);
    QVERIFY(released && getAvailableElementsCount(m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE + 1);

    element = removeFirstListElement(m_List1);
    QVERIFY(element);

    released = releaseElement(element, m_TempPool1);
    QVERIFY(released && getAvailableElementsCount(m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE + 2);

    // releasing an element twice is not allowed
    released = releaseElement(element, m_TempPool1);
    QVERIFY(!released);

    ListElement* newElement = createListElement();
    _markListElementForDeletion(newElement);

    // releasing an element that doesn't belong to pool is not allowed
    released = releaseElement(newElement, m_TempPool1);
    QVERIFY(!released && getAvailableElementsCount(m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE + 2);

    element = aquireElement(m_TempPool1);

    CHECK_AQUIRED_ELEMENTS(&element, 1);
    QVERIFY(getAvailableElementsCount(m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE + 1);

    released = releaseElement(element, m_TempPool1);
    QVERIFY(released && getAvailableElementsCount(m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE + 2);

    const size_t listElementsCount = getListSize(m_List1);
    QVERIFY(listElementsCount == initialPooledElementsCount - 2);

    for (size_t index = 0; index < listElementsCount; ++index)
    {
        element = index % 2 == 0 ? removeLastListElement(m_List1) : removeFirstListElement(m_List1);
        QVERIFY(element);
        released = releaseElement(element, m_TempPool1);
        QVERIFY(released);
        element = nullptr;
    }

    const size_t finalPooledElementsCount = initialPooledElementsCount + ELEMENTS_POOL_SLICE_SIZE;

    QVERIFY(getAvailableElementsCount(m_TempPool1) == finalPooledElementsCount);
    QVERIFY(isEmptyList(m_List1));

    released = releaseElement(nullptr, m_TempPool1);
    QVERIFY(!released && getAvailableElementsCount(m_TempPool1) == finalPooledElementsCount);
}

void ListElementTests::testAquiringMultiplePoolElements()
{
    m_TempPool1 = createListElementsPool();
    QVERIFY(m_TempPool1);

    // batch 1
    const size_t batchSize1 = 3;
    ListElement** listElementRefs1 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs1, batchSize1);

    bool multipleElementsAquired = aquireElements(m_TempPool1, listElementRefs1, batchSize1);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs1, batchSize1);
    QVERIFY(getAvailableElementsCount(m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE - 3);

    // batch 2
    const size_t batchSize2 = ELEMENTS_POOL_SLICE_SIZE - 2;
    ListElement** listElementRefs2 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs2, batchSize2);

    multipleElementsAquired = aquireElements(m_TempPool1, listElementRefs2, batchSize2);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs2, batchSize2);
    QVERIFY(getAvailableElementsCount(m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE - 1);

    RELEASE_ELEMENTS(listElementRefs2, batchSize2, m_TempPool1);
    QVERIFY(getAvailableElementsCount(m_TempPool1) == 2 * ELEMENTS_POOL_SLICE_SIZE - 3);

    RELEASE_ELEMENTS(listElementRefs1, batchSize1, m_TempPool1);
    QVERIFY(getAvailableElementsCount(m_TempPool1) == 2 * ELEMENTS_POOL_SLICE_SIZE);

    // batch 3
    const size_t batchSize3 = 2 * ELEMENTS_POOL_SLICE_SIZE;
    ListElement** listElementRefs3 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs3, batchSize3);

    multipleElementsAquired = aquireElements(m_TempPool1, listElementRefs3, batchSize3);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs3, batchSize3);
    QVERIFY(getAvailableElementsCount(m_TempPool1) == 0);

    RELEASE_ELEMENTS(listElementRefs3, batchSize3, m_TempPool1);
    QVERIFY(getAvailableElementsCount(m_TempPool1) == 2 * ELEMENTS_POOL_SLICE_SIZE);

    // batch 4
    const size_t batchSize4 = 1;
    ListElement** listElementRefs4 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs4, batchSize4);

    multipleElementsAquired = aquireElements(m_TempPool1, listElementRefs4, batchSize4);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs4, batchSize4);
    QVERIFY(getAvailableElementsCount(m_TempPool1) == 2 * ELEMENTS_POOL_SLICE_SIZE - 1);

    // batch 5
    size_t batchSize5 = 2 * ELEMENTS_POOL_SLICE_SIZE + 1;
    ListElement** listElementRefs5 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs5, batchSize5);

    multipleElementsAquired = aquireElements(m_TempPool1, listElementRefs5, batchSize5);
    QVERIFY(!multipleElementsAquired);

    --batchSize5;

    multipleElementsAquired = aquireElements(m_TempPool1, listElementRefs5, batchSize5);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs5, batchSize5);
    QVERIFY(getAvailableElementsCount(m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE - 1);

    RELEASE_ELEMENTS(listElementRefs4, batchSize4, m_TempPool1);
    RELEASE_ELEMENTS(listElementRefs5, batchSize5, m_TempPool1);

    QVERIFY(getAvailableElementsCount(m_TempPool1) == 3 * ELEMENTS_POOL_SLICE_SIZE);

    // batch 6
    const size_t batchSize6 = ELEMENTS_POOL_SLICE_SIZE + ELEMENTS_POOL_SLICE_SIZE / 2;
    ListElement** listElementRefs6 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs6, batchSize6);

    multipleElementsAquired = aquireElements(m_TempPool1, listElementRefs6, batchSize6);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs6, batchSize6);
    QVERIFY(getAvailableElementsCount(m_TempPool1) == batchSize6);

    // batch 7
    const size_t batchSize7 = ELEMENTS_POOL_SLICE_SIZE;
    ListElement** listElementRefs7 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs7, batchSize7);

    multipleElementsAquired = aquireElements(m_TempPool1, listElementRefs7, batchSize7);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs7, batchSize7);
    QVERIFY(getAvailableElementsCount(m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE / 2);

    // batch 8
    size_t batchSize8 = ELEMENTS_POOL_SLICE_SIZE + ELEMENTS_POOL_SLICE_SIZE / 2 + 1;
    ListElement** listElementRefs8 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs8, batchSize8);

    multipleElementsAquired = aquireElements(m_TempPool1, listElementRefs8, batchSize8);

    QVERIFY(!multipleElementsAquired);

    --batchSize8;

    multipleElementsAquired = aquireElements(m_TempPool1, listElementRefs8, batchSize8);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs8, batchSize8);
    QVERIFY(getAvailableElementsCount(m_TempPool1) == 0);

    // batch 9
    const size_t batchSize9 = 5;
    ListElement** listElementRefs9 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs9, batchSize9);

    multipleElementsAquired = aquireElements(m_TempPool1, listElementRefs9, batchSize9);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs9, batchSize9);
    QVERIFY(getAvailableElementsCount(m_TempPool1) == ELEMENTS_POOL_SLICE_SIZE - 5);

    RELEASE_ELEMENTS(listElementRefs6, batchSize6, m_TempPool1);
    RELEASE_ELEMENTS(listElementRefs7, batchSize7, m_TempPool1);
    RELEASE_ELEMENTS(listElementRefs8, batchSize8, m_TempPool1);
    RELEASE_ELEMENTS(listElementRefs9, batchSize9, m_TempPool1);

    QVERIFY(getAvailableElementsCount(m_TempPool1) == 5 * ELEMENTS_POOL_SLICE_SIZE);

    // combined test: aquire single element and aquire multiple elements
    m_TempPool2 = createListElementsPool();
    QVERIFY(m_TempPool2);

    ListElement* element = aquireElement(m_TempPool2);
    QVERIFY(element);
    CHECK_AQUIRED_ELEMENTS(&element, 1);

    const size_t batchSize10 = ELEMENTS_POOL_SLICE_SIZE - 1;
    ListElement** listElementRefs10 = nullptr;
    ALLOC_LIST_ELEMENT_REFS(listElementRefs10, batchSize10);

    multipleElementsAquired = aquireElements(m_TempPool2, listElementRefs10, batchSize10);

    QVERIFY(multipleElementsAquired);
    CHECK_AQUIRED_ELEMENTS(listElementRefs10, batchSize10);
    QVERIFY(getAvailableElementsCount(m_TempPool2) == 0);

    RELEASE_ELEMENTS(listElementRefs10, batchSize10, m_TempPool2);
    RELEASE_ELEMENTS(&element, 1, m_TempPool2);

    QVERIFY(getAvailableElementsCount(m_TempPool2) == ELEMENTS_POOL_SLICE_SIZE);

    // additional corner cases
    multipleElementsAquired = aquireElements(m_TempPool2, listElementRefs10, 0);
    QVERIFY(!multipleElementsAquired);

    multipleElementsAquired = aquireElements(nullptr, listElementRefs10, 1);
    QVERIFY(!multipleElementsAquired);

    multipleElementsAquired = aquireElements(m_TempPool2, nullptr, 1);
    QVERIFY(!multipleElementsAquired);
}

void ListElementTests::cleanupTestCase()
{
    QVERIFY(!m_TempPool1);
    QVERIFY(!m_TempPool2);
    QVERIFY(!m_List1);

    QVERIFY(m_ListElementsMarkedForDeletion.empty());
    QVERIFY(m_ListElementRefsMarkedForDeletion.empty());
}

void ListElementTests::init()
{
    QVERIFY(!m_TempPool1);
    QVERIFY(!m_TempPool2);
    QVERIFY(!m_List1);

    QVERIFY(m_ListElementsMarkedForDeletion.empty());
    QVERIFY(m_ListElementRefsMarkedForDeletion.empty());
}

void ListElementTests::cleanup()
{
    DELETE_LIST(m_List1, deleteObjectPayload);

    DELETE_LIST_ELEMENTS_POOL(m_TempPool1);
    DELETE_LIST_ELEMENTS_POOL(m_TempPool2);

    for (auto& element : m_ListElementsMarkedForDeletion)
    {
        FREE(element);
    }

    m_ListElementsMarkedForDeletion.clear();

    for (auto& elementRefs : m_ListElementRefsMarkedForDeletion)
    {
        FREE(elementRefs);
    }

    m_ListElementRefsMarkedForDeletion.clear();
}

void ListElementTests::_markListElementForDeletion(ListElement* element)
{
    if (element)
    {
        m_ListElementsMarkedForDeletion.push_back(element);
    }
}

void ListElementTests::_markListElementRefsForDeletion(ListElement** elementRefs)
{
    if (elementRefs)
    {
        m_ListElementRefsMarkedForDeletion.push_back(elementRefs);
    }
}

QTEST_APPLESS_MAIN(ListElementTests)

#include "tst_listelementtests.moc"
