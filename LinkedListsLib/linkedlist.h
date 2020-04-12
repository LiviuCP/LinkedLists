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

void prependElementToList(List* list, ListElement* newElement);
void appendElementToList(List* list, ListElement* newElement);
ListElement* removeFirstListElement(List* list);
ListElement* removeLastListElement(List* list);

unsigned int getListSize(List* list);

void printList(List* list);

#endif // LINKEDLIST_H
