#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>

struct ListElement
{
    unsigned long priority;
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

void prependToList(List* list, ListElement* newElement);
void appendToList(List* list, ListElement* newElement);
ListElement* createAndPrependToList(List* list, size_t priority);
ListElement* createAndAppendToList(List* list, size_t priority);

ListElement* removeFirstListElement(List* list);
ListElement* removeLastListElement(List* list);
void clearList(List* list);

void reverseList(List* list);
void sortAscendingByPriority(List* list);

size_t getListSize(const List* list);
ListElement* getElementAtIndex(const List* list, size_t index); // mainly for testing purposes, emulates the array indexing

ListIterator lbegin(List* list);
ListIterator lend(List* list);
void lnext(ListIterator* iterator);
int areIteratorsEqual(ListIterator first, ListIterator second);

#ifdef __cplusplus
}
#endif


#endif // LINKEDLIST_H
