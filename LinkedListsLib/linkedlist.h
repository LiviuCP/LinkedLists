#ifndef LINKEDLIST_H
#define LINKEDLIST_H

struct ListElement
{
    unsigned int id;
    struct ListElement* next;
};

typedef struct ListElement ListElement;

typedef struct
{
    ListElement* first;
} List;

void prependElement(List* list, ListElement* newElement);
void appendElement(List* list, ListElement* newElement);
ListElement* removeFirstElement(List* list);
ListElement* removeLastElement(List* list);
void printList(List* list);
unsigned int size(List* list);

#endif // LINKEDLIST_H
