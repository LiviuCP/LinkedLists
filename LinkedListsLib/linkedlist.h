#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>

#include "listutils.h"

typedef struct
{
    char* type;
    void* payload;
}
Object;

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

List* createList();
void deleteList(List* list, void (*deallocObject)(Object* object));
void deleteObject(Object* object); // default object deallocator, only works for simple objects without associated payload heap memory (e.g. Point, primitive types payloads)

int copyObject(const ListElement* source, ListElement* destination); // default object copy function, does nothing but is required for passing a default function pointer

ListElement* createListElement();

void prependToList(List* list, ListElement* newElement);
void appendToList(List* list, ListElement* newElement);
ListElement* createAndPrependToList(List* list, size_t priority);
ListElement* createAndAppendToList(List* list, size_t priority);

ListElement* createAndInsertAfter(ListIterator it, size_t priority);
void insertAfter(ListIterator it, ListElement* nextElement);
ListElement* createAndInsertBefore(ListIterator it, size_t priority);
void insertBefore(ListIterator it, ListElement* previousElement);

void moveContentToList(List* source, List* destination);
ListElement* copyContentToList(const List* source, List* destination, int (*copyObjectToElement)(const ListElement* source, ListElement* destination),
                               void (*deallocObject)(Object* object));

void assignObjectToListElement(ListElement* element, const char* objectType, void* objectPayload);
Object* removeObjectFromListElement(ListElement* element);

ListElement* removeFirstListElement(List* list);
ListElement* removeLastListElement(List* list);
ListElement* removePreviousListElement(ListIterator it);
ListElement* removeNextListElement(ListIterator it);
ListElement* removeCurrentListElement(ListIterator it);

void clearList(List* list, void (*deallocObject)(Object* object));

void reverseList(List* list);
void sortAscendingByPriority(List* list);
void sortDescendingByPriority(List* list);

size_t getListSize(const List* list);
ListElement* getElementAtIndex(const List* list, size_t index); // mainly for testing purposes, emulates the array indexing
ListElement* getLastElement(const List* list);
int isElementContained(const ListElement* element, const List* list);

ListIterator lbegin(List* list);
ListIterator lend(List* list);
void lnext(ListIterator* iterator);
int areIteratorsEqual(ListIterator first, ListIterator second);

ListElement** moveListToArray(List* list, size_t* arraySize);
void moveArrayToList(ListElement** array, const size_t arraySize, List* list);


// for testing purposes only
void customDeleteObject(Object* object);
int customCopyObject(const ListElement* source, ListElement* destination);

#ifdef __cplusplus
}
#endif


#endif // LINKEDLIST_H
