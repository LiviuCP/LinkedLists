#include "stdio.h"
#include "assert.h"

#include "linkedlist.h"
#include "../Utils/codeutils.h"

void prependElementToList(List* list, ListElement* newElement)
{
    if (list != NULL && newElement != NULL)
    {
        newElement->next = list->first;
        list->first = newElement;
    }
}

void appendElementToList(List* list, ListElement* newElement)
{
    ListElement* currentElement;
    if (list != NULL && newElement != NULL)
    {
        if (list->first == NULL)
        {
            list->first = newElement;
        }
        else
        {
            currentElement = list->first;
            while (currentElement->next != NULL)
            {
                currentElement = currentElement->next;
            }
            currentElement->next = newElement;
        }
    }
}

ListElement* removeFirstListElement(List* list)
{
    ListElement* removedElement = NULL;

    if (list != NULL && list->first != NULL)
    {
        removedElement = list->first;
        if (removedElement->next != NULL)
        {
            list->first = removedElement->next;
        }
        else
        {
            list->first = NULL;
        }
    }

    return removedElement;
}

ListElement* removeLastListElement(List* list)
{
    ListElement* removedElement = NULL;

    if (list != NULL && list->first != NULL)
    {
        ListElement* currentElement = list->first;
        ListElement* nextElement = currentElement->next;
        if (nextElement != NULL)
        {
            while (nextElement->next != NULL)
            {
                currentElement = nextElement;
                nextElement = nextElement->next;
            }
            removedElement = nextElement;
            currentElement->next = NULL;
        }
        else
        {
            removedElement = currentElement;
            list->first = NULL;
        }
    }

    return removedElement;
}

unsigned int getListSize(List *list)
{
    unsigned int length = 0;

    if (list != NULL)
    {
        ListElement* currentElement = list->first;

        while (currentElement != NULL)
        {
            ++length;
            currentElement = currentElement->next;
        }
    }

    return length;
}

void printList(List* list)
{
    if (list != NULL)
    {
        int currentElementIndex = 0;
        for (ListIterator it = lbegin(list); !areEqual(it, lend(list)); next(&it))
        {
            printf("Index: %d\t Priority: %d\n", currentElementIndex+1, it.current->priority);
            ++currentElementIndex;
        }
        if (currentElementIndex == 0)
        {
            printf("Empty list!\n\n");
        }
        else
        {
            printf("\nThe list has %d elements\n\n", (int)getListSize(list));
        }
    }
    else
    {
        printf("The list does not exist!");
    }
}

ListIterator lbegin(List *list)
{
    ListIterator result;

    if (list == NULL)
    {
        fprintf(stderr, "ASSERT ERROR! Attempt to get iterator from NULL list");
        assert(0);
    }
    result.current = list->first;

    return result;
}

ListIterator lend(List *list)
{
    ListIterator result;

    if (list == NULL)
    {
        fprintf(stderr, "ASSERT ERROR! Attempt to get iterator from NULL list");
        assert(0);
    }
    result.current = NULL;

    return result;
}

void next(ListIterator* iterator)
{
    if (iterator == NULL)
    {
        fprintf(stderr, "ASSERT ERROR! Attempt to advance a NULL iterator");
        assert(0);
    }

    if (iterator->current != NULL)
    {
        iterator->current = iterator->current->next;
    }
}

int areEqual(ListIterator first, ListIterator second)
{
    return first.current == second.current;
}
