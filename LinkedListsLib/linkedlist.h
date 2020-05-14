#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>

#include "../Utils/codeutils.h"

struct ListElement
{
    Object* object;
    size_t priority;

    struct ListElement* next;
};

typedef struct ListElement ListElement;

typedef struct
{
    ListElement* first;
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

List* createEmptyList();
List* createListFromPrioritiesArray(const size_t *priorityArray, const size_t arraySize);
void deleteList(List* list, void (*deallocObject)(Object* object));

ListElement* createListElement();

ListElement* createAndAppendToList(List* list, size_t priority);
void appendToList(List* list, ListElement* newElement);
ListElement* createAndPrependToList(List* list, size_t priority);
void prependToList(List* list, ListElement* newElement);
ListElement* createAndInsertAfter(ListIterator it, size_t priority);
void insertAfter(ListIterator it, ListElement* nextElement);
ListElement* createAndInsertBefore(ListIterator it, size_t priority);
void insertBefore(ListIterator it, ListElement* previousElement);

ListElement* removeFirstListElement(List* list);
ListElement* removeLastListElement(List* list);
ListElement* removePreviousListElement(ListIterator it);
ListElement* removeNextListElement(ListIterator it);
ListElement* removeCurrentListElement(ListIterator it);
void clearList(List* list, void (*deallocObject)(Object* object));

void swapListElements(ListIterator firstIt, ListIterator secondIt);
void reverseList(List* list);
void sortAscendingByPriority(List* list);
void sortDescendingByPriority(List* list);
boolean sortByRandomAccess(List* list, void (*sortingAlgorithm)(ListElement** array, const size_t arraySize));

void moveContentToList(List* source, List* destination);
ListElement* copyContentToList(const List* source, List* destination, boolean (*copyObjectToElement)(const ListElement* source, ListElement* destination), void (*deallocObject)(Object* object));
ListElement** moveListToArray(List* list, size_t* arraySize);
void moveArrayToList(ListElement** array, const size_t arraySize, List* list);

size_t getListSize(const List* list);
ListElement* getListElementAtIndex(const List* list, size_t index); // mainly for testing purposes, emulates the array indexing
ListElement* getPreviousListElement(ListIterator it);
ListElement* getLastListElement(const List* list);
boolean isListElementContained(const ListElement* element, const List* list);
boolean isSortedAscendingByPriority(const List* list);
boolean isSortedDescendingByPriority(const List* list);

ListIterator lbegin(List* list);
ListIterator lend(List* list);
void lnext(ListIterator* iterator);
boolean areIteratorsEqual(ListIterator first, ListIterator second);

void printListContentToFile(const List* list, const char* outFile, const char* header);

void assignObjectToListElement(ListElement* element, int objectType, void* objectPayload);
Object* removeObjectFromListElement(ListElement* element);
void deleteObject(Object* object); // default object deallocator, only works for simple objects without associated payload heap memory (e.g. Point, primitive types payloads)
boolean copyObject(const ListElement* source, ListElement* destination); // default object copy function, does nothing but is required for passing a default function pointer

// for testing purposes only
boolean customCopyObject(const ListElement* source, ListElement* destination);

#ifdef __cplusplus
}
#endif


#endif // LINKEDLIST_H
