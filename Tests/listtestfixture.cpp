#include <cassert>

#include "listtestfixture.h"
#include "codeutils.h"

#define DELETE_LIST_ELEMENTS_POOL(elementsPool) \
    if (elementsPool) \
    { \
        deleteListElementsPool(elementsPool); \
        elementsPool = nullptr; \
    }

ListTestFixture::ListTestFixture()
    : m_Pool{nullptr}
    , m_TempPool1{nullptr}
    , m_TempPool2{nullptr}
    , m_List1{nullptr}
    , m_List2{nullptr}
    , m_List3{nullptr}
    , m_List4{nullptr}
    , m_List5{nullptr}
    , m_List6{nullptr}
    , m_List7{nullptr}
    , m_List8{nullptr}
    , m_ListElementRefs{nullptr}
{
}

void ListTestFixture::init()
{
    if (!m_Pool)
    {
        m_Pool = createListElementsPool();
        assert(hasInitialState() && "Incorrect fixture initialization!");
    }
}

void ListTestFixture::destroy()
{
    assert(hasInitialState() && "Fixture can only be destroyed after bringing into initial state!");

    deleteListElementsPool(m_Pool);
    m_Pool = nullptr;
}

void ListTestFixture::resetToInitialState()
{
    assert(m_Pool);

    DELETE_LIST(m_List1, deleteObjectPayload);
    DELETE_LIST(m_List2, deleteObjectPayload);
    DELETE_LIST(m_List3, deleteObjectPayload);
    DELETE_LIST(m_List4, deleteObjectPayload);
    DELETE_LIST(m_List5, deleteObjectPayload);
    DELETE_LIST(m_List6, deleteObjectPayload);
    DELETE_LIST(m_List7, deleteObjectPayload);
    DELETE_LIST(m_List8, deleteObjectPayload);

    DELETE_LIST_ELEMENTS_POOL(m_TempPool1);
    DELETE_LIST_ELEMENTS_POOL(m_TempPool2);

    for (auto list : m_ListsMarkedForDeletion)
    {
        DELETE_LIST(list, deleteObjectPayload);
    }

    m_ListsMarkedForDeletion.clear();

    for (auto& element : m_ListElementsMarkedForRelease)
    {
        if (element)
        {
            releaseElement(element, m_Pool);
            element = nullptr;
        }
    }

    for (auto& element : m_ListElementsMarkedForDeletion)
    {
        FREE(element);
    }

    m_ListElementsMarkedForRelease.clear();
    m_ListElementsMarkedForDeletion.clear();

    // it is assumed that the actual elements have been previously cleaned up (deleted or released) during test run
    FREE(m_ListElementRefs);

    for (auto& elementRefs : m_ListElementRefGroupsMarkedForDeletion)
    {
        FREE(elementRefs);
    }

    m_ListElementRefGroupsMarkedForDeletion.clear();
}

bool ListTestFixture::hasInitialState() const
{
    return m_Pool && getAquiredElementsCount(m_Pool) == 0 &&
           !m_TempPool1 && !m_TempPool2 &&
           !m_List1 && !m_List2 && !m_List3 && !m_List4 && !m_List5 && !m_List6 && !m_List7 && !m_List8 &&
           !m_ListElementRefs &&
           m_ListsMarkedForDeletion.empty() &&
           m_ListElementsMarkedForRelease.empty() && m_ListElementsMarkedForDeletion.empty() &&
           m_ListElementRefGroupsMarkedForDeletion.empty();
}

void ListTestFixture::markListForDeletion(List* list)
{
    if (list)
    {
        m_ListsMarkedForDeletion.push_back(list);
    }
}

void ListTestFixture::markListElementForCleanup(ListElement* element, bool shouldRelease)
{
    if (element)
    {
        auto& freeElementsCollector = shouldRelease ? m_ListElementsMarkedForRelease : m_ListElementsMarkedForDeletion;
        freeElementsCollector.push_back(element);
    }
}

void ListTestFixture::markListElementRefsForDeletion(ListElement** elementRefs)
{
    if (elementRefs)
    {
        m_ListElementRefGroupsMarkedForDeletion.push_back(elementRefs);
    }
}

void ListTestFixture::clearListElementsMarkedForCleanup()
{
    m_ListElementsMarkedForRelease.clear();
    m_ListElementsMarkedForDeletion.clear();
}
