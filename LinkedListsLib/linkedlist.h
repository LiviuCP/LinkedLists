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
void deleteList(List* list);

ListElement* createListElement();

void prependToList(List* list, ListElement* newElement);
void appendToList(List* list, ListElement* newElement);
ListElement* createAndPrependToList(List* list, size_t priority);
ListElement* createAndAppendToList(List* list, size_t priority);

ListElement* createAndInsertAfter(ListIterator it, size_t priority);
void insertAfter(ListIterator it, ListElement* nextElement);
ListElement* createAndInsertBefore(ListIterator it, size_t priority);
void insertBefore(ListIterator it, ListElement* previousElement);

void assignObjectToListElement(ListElement* element, const char* objectType, void* objectPayload);
Object* removeObjectFromListElement(ListElement* element);

ListElement* removeFirstListElement(List* list);
ListElement* removeLastListElement(List* list);
ListElement* removePreviousListElement(ListIterator it);
ListElement* removeNextListElement(ListIterator it);
ListElement* removeCurrentListElement(ListIterator it);

void clearList(List* list);

void reverseList(List* list);
void sortAscendingByPriority(List* list);
void sortDescendingByPriority(List* list);

size_t getListSize(const List* list);
ListElement* getElementAtIndex(const List* list, size_t index); // mainly for testing purposes, emulates the array indexing
int isElementContained(const ListElement* element, const List* list);

ListIterator lbegin(List* list);
ListIterator lend(List* list);
void lnext(ListIterator* iterator);
int areIteratorsEqual(ListIterator first, ListIterator second);

#ifdef __cplusplus
}
#endif


#endif // LINKEDLIST_H
