#ifndef LINKEDLIST_H
#define LINKEDLIST_H

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

void prependElementToList(List* list, ListElement* newElement);
void appendElementToList(List* list, ListElement* newElement);
ListElement* removeFirstListElement(List* list);
ListElement* removeLastListElement(List* list);
void sortAscendingByPriority(List* list);

unsigned int getListSize(List* list);

ListIterator lbegin(List* list);
ListIterator lend(List* list);
void next(ListIterator* iterator);
int areEqual(ListIterator first, ListIterator second);

#endif // LINKEDLIST_H
