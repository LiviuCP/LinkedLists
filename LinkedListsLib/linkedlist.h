#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>

#include "../Utils/codeutils.h"

struct ListElement
{
    Object object;
    size_t priority;

    struct ListElement* next;
};

typedef struct ListElement ListElement;

typedef struct
{
    ListElement* first;
    ListElement* last; // required for constant time element appending
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

List* createEmptyList();                                                                                // don't use for stack created lists (heap-only)
List* createListFromPrioritiesArray(const size_t *priorityArray, const size_t arraySize);               // don't use for stack created lists (heap-only)
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
ListElement* getListElementAtIndex(const List* list, size_t index); // mainly for testing purposes, emulates the array indexing
ListElement* getPreviousListElement(ListIterator it);
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
