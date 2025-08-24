#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>

#include "../Utils/codeutils.h"

#define MAX_POOL_ITEMS_COUNT 1024

struct ListElement
{
    Object object;
    size_t priority;

    struct ListElement* next;
};

typedef struct ListElement ListElement;

// TODO: move pool to separate file (along with ListElement)
typedef struct
{
    void* content;
}
ListElementsPool;

typedef struct
{
    ListElement* first;
    ListElement* last; // required for constant time element appending
    ListElementsPool* elementsPool;
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

ListElementsPool* createListElementsPool();
void deleteListElementsPool(ListElementsPool* elementsPool);
size_t getAvailableElementsCount(ListElementsPool* elementsPool);
ListElement* aquireElement(ListElementsPool* elementsPool);
bool releaseElement(ListElement* element, ListElementsPool* elementsPool);

List* createEmptyList(ListElementsPool* elementsPool);                                                  // don't use for stack created lists (heap-only)
List* createEmptyLists(size_t count, ListElementsPool* elementsPool);                                   // don't use for stack created lists (heap-only)
List* createListFromPrioritiesArray(const size_t* prioritiesArray,
                                    const size_t arraySize,
                                    ListElementsPool* elementsPool);                                    // don't use for stack created lists (heap-only)
void deleteList(List* list, void (*deallocObject)(Object* object));                                     // don't use for stack created lists (heap-only)

ListElement* createListElement();                                                                       // don't use for stack created lists (heap-only)

ListElement* createAndAppendToList(List* list, size_t priority);                                        // don't use for stack created lists (heap-only)
void appendToList(List* list, ListElement* newElement);
ListElement* createAndPrependToList(List* list, size_t priority);                                       // don't use for stack created lists (heap-only)
void prependToList(List* list, ListElement* newElement);
ListElement* createAndInsertAfter(ListIterator it, size_t priority);                                    // don't use for stack created lists (heap-only)
void insertAfter(ListIterator it, ListElement* nextElement);
ListElement* createAndInsertBefore(ListIterator it, size_t priority);                                   // don't use for stack created lists (heap-only)
void insertBefore(ListIterator it, ListElement* previousElement);

ListElement* removeFirstListElement(List* list);
ListElement* removeLastListElement(List* list);
ListElement* removePreviousListElement(ListIterator it);
ListElement* removeNextListElement(ListIterator it);
ListElement* removeCurrentListElement(ListIterator it);

void clearList(List* list, void (*deallocObject)(Object* object));                                      // don't use for stack created lists (heap-only)
void detachListElements(List* list);                                                                    // use this function instead of clearList() for stack created lists

void swapListElements(ListIterator firstIt, ListIterator secondIt);
void reverseList(List* list);
ListElement* batchReverseList(List* list, size_t batchSize);                                            // reverses the list in groups (batches), each batch having batchSize elements
void sortAscendingByPriority(List* list);
void sortDescendingByPriority(List* list);
bool sortByRandomAccess(List* list, void (*sortingAlgorithm)(ListElement** array, const size_t arraySize));

void moveContentToList(List* source, List* destination);

/* avoid using this function with stack lists/elements */
ListElement* copyContentToList(const List* source, List* destination, bool (*copyObjectToElement)(const ListElement* source, ListElement* destination), void (*deallocObject)(Object* object));

ListElement** moveListToArray(List* list, size_t* arraySize);                                           // don't use for stack created lists (heap-only)
void moveArrayToList(ListElement** array, const size_t arraySize, List* list);                          // don't use for stack created lists (heap-only)

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

void assignObjectContentToListElement(ListElement* element, const int objectType, void* const objectPayload);
Object* detachContentFromListElement(ListElement* element);


/* don't use these two for stack created lists (heap-only) */
void deleteObjectPayload(Object* object); // default object deallocator, only works for simple objects without associated payload heap memory (e.g. Point, primitive types payloads)
bool copyObjectPlaceholder(const ListElement* source, ListElement* destination); // default object copy function, does nothing but is required for passing a default function pointer

// for testing purposes only
bool customCopyObject(const ListElement* source, ListElement* destination);

#ifdef __cplusplus
}
#endif


#endif // LINKEDLIST_H
