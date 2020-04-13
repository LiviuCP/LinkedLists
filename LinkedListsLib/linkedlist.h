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
    ListElement* current;
}
ListIterator;

#ifdef __cplusplus
extern "C"{
#endif

void prependToList(List* list, ListElement* newElement);
void appendToList(List* list, ListElement* newElement);
ListElement* createAndPrependToList(List* list, size_t priority);
ListElement* createAndAppendToList(List* list, size_t priority);

ListElement* removeFirstListElement(List* list);
ListElement* removeLastListElement(List* list);
void clearList(List* list);

void sortAscendingByPriority(List* list);

unsigned int getListSize(List* list);

ListIterator lbegin(List* list);
ListIterator lend(List* list);
void next(ListIterator* iterator);
int areEqual(ListIterator first, ListIterator second);

#ifdef __cplusplus
}
#endif


#endif // LINKEDLIST_H
