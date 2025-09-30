#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>

#include "listelementspoolproxy.h"

typedef struct
{
    ListElement* first;
    ListElement* last; // required for constant time element appending
    ListElementsPoolProxy elementsPoolProxy;
}
List;

typedef struct
{
    List* list;
    ListElement* current;
}
ListIterator;

#ifdef __cplusplus
extern "C"{
#endif

/* These functions should only be used for heap-based lists/elements */

List* createEmptyList(void* elementsPool);

List* createListFromPrioritiesArray(const size_t* prioritiesArray,
                                    const size_t arraySize,
                                    void* elementsPool);

void initEmptyList(List* list, void* elementsPool);

void deleteList(List* list, void (*deallocObject)(Object* object));
void clearList(List* list, void (*deallocObject)(Object* object));

ListElement* createAndPrependToList(List* list, size_t priority);
ListElement* createAndAppendToList(List* list, size_t priority);
ListElement* createAndInsertBefore(ListIterator it, size_t priority);
ListElement* createAndInsertAfter(ListIterator it, size_t priority);

void moveContentToList(List* source, List* destination);

ListElement* copyContentToList(const List* source,
                               List* destination,
                               bool (*copyObjectToElement)(const ListElement* source, ListElement* destination),
                               void (*deallocObject)(Object* object));

ListElement** moveListToArray(List* list, size_t* arraySize);
void moveArrayToList(ListElement** array, const size_t arraySize, List* list);


/* These functions can also be used for stack-based lists/elements too */

void prependToList(List* list, ListElement* newElement);
void appendToList(List* list, ListElement* newElement);
void insertBefore(ListIterator it, ListElement* previousElement);
void insertAfter(ListIterator it, ListElement* nextElement);

ListElement* removeFirstListElement(List* list);
ListElement* removeLastListElement(List* list);
ListElement* removePreviousListElement(ListIterator it);
ListElement* removeNextListElement(ListIterator it);
ListElement* removeCurrentListElement(ListIterator it);

void swapListElements(ListIterator firstIt, ListIterator secondIt);
void reverseList(List* list);
ListElement* batchReverseList(List* list, size_t batchSize); // reverses the list in groups (batches), each batch having batchSize elements
void sortAscendingByPriority(List* list);
void sortDescendingByPriority(List* list);
bool sortByRandomAccess(List* list, void (*sortingAlgorithm)(ListElement** array, const size_t arraySize));

size_t getListSize(const List* list);
bool isEmptyList(const List* list);

ListElement* getListElementAtIndex(const List* list, size_t index); // mainly for testing purposes, emulates the array indexing
ListElement* getPreviousListElement(ListIterator it);
ListElement* getFirstListElement(const List* list);
ListElement* getLastListElement(const List* list);

bool isListElementContained(const ListElement* element, const List* list);
bool isSortedAscendingByPriority(const List* list);
bool isSortedDescendingByPriority(const List* list);

ListIterator lbegin(List* list);
ListIterator lend(List* list);
void lnext(ListIterator* iterator);
bool areIteratorsEqual(ListIterator first, ListIterator second);

void printListContentToFile(const List* list, const char* outFile, const char* header);


/* This function should only be used for stack-based lists/elements (stack alternative to clearList()) */
void detachListElements(List* list);

#ifdef __cplusplus
}
#endif


#endif // LINKEDLIST_H
