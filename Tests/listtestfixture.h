#ifndef LISTTESTFIXTURE_H
#define LISTTESTFIXTURE_H

#include <vector>
#include <cstdlib>

#include "linkedlist.h"
#include "listelementspool.h"

#define USE_DEFAULT_MAX_SLICES_COUNT 0

using Priorities = std::vector<Priority>;

struct ListTestFixture
{
    ListElementsPool* m_Pool;

    // temporary pools, to be cleaned up (if necessary) after each test run
    ListElementsPool* m_TempPool1;
    ListElementsPool* m_TempPool2;

    List* m_List1;
    List* m_List2;
    List* m_List3;

    // used for storing multiple individual list element addreses; to be cleaned up after each test run (elements to be cleaned up separately)
    ListElement** m_ListElementRefs;

    // lists that are not test class members marked for deletion (deleted when running cleanup()) - do not add members (e.g. m_List1) here
    std::vector<List*> m_ListsMarkedForDeletion;

    // elements collected from test cases that need to be discarded (released to pool or freed)
    std::vector<ListElement*> m_ListElementsMarkedForRelease;
    std::vector<ListElement*> m_ListElementsMarkedForDeletion;

    // arrays (groups) of list element addresses (refs) collected from test cases that need to be freed
    // - each ListElement** array of addresses had been used for aquiring multiple elements from list elements pool;
    // - it is assumed that the list elements had been released prior to freeing the groups
    std::vector<ListElement**> m_ListElementRefGroupsMarkedForDeletion;

    explicit ListTestFixture();

    void init(); // brings fixture into initial state
    void destroy();
    void resetToInitialState();
    bool hasInitialState() const;

    void markListForDeletion(List* list);
    void markListElementForCleanup(ListElement* element, bool shouldRelease);
    void markListElementRefsForDeletion(ListElement** elementRefs);
    void clearListElementsMarkedForCleanup();
};

#endif // LISTTESTFIXTURE_H
