#include <stdio.h>
#include <assert.h>

#include "linkedlist.h"
#include "../Utils/codeutils.h"

void prependToList(List* list, ListElement* newElement)
{
    if (list != NULL && newElement != NULL)
    {
        newElement->next = list->first;
        list->first = newElement;
    }
}

void appendToList(List* list, ListElement* newElement)
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

ListElement* createAndPrependToList(List *list, size_t priority)
{
    ListElement* element = (ListElement*)malloc(sizeof(ListElement));

    if (element != NULL)
    {
        element->priority = priority;
        element->next = NULL;
        prependToList(list, element);
    }

    return element;
}

ListElement* createAndAppendToList(List *list, size_t priority)
{
    ListElement* element = (ListElement*)malloc(sizeof(ListElement));

    if (element != NULL)
    {
        element->priority = priority;
        element->next = NULL;
        appendToList(list, element);
    }

    return element;
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
            removedElement->next = NULL;
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

void clearList(List *list)
{
    if (list != NULL)
    {
        ListElement* currentElement = list->first;
        list->first = NULL;

        while (currentElement != NULL)
        {
            ListElement* elementToDelete = currentElement;
            currentElement = currentElement->next;
            free(elementToDelete);
            elementToDelete = NULL;
        }
    }
}

size_t getListSize(const List *list)
{
    size_t length = 0;

    if (list != NULL && list->first != NULL)
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

ListElement* getElementAtIndex(const List* list, size_t index)
{
    ListElement* result = NULL;

    if (list != NULL)
    {
        if (index >= getListSize(list))
        {
            fprintf(stderr, "ASSERTION ERROR! The index is out of bounds!");
            assert(0);
        }

        ListElement* currentElement = list->first;

        for (size_t currentIndex = 0; currentIndex < index; ++currentIndex)
        {
            currentElement = currentElement->next;
        }

        result = currentElement;
    }

    return result;
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

void sortAscendingByPriority(List* list)
{
    if (list != NULL)
    {
        // bubble sort used due to the access model of the list elements (non-constant, element after element)
        for (;;)
        {
            ListElement* currentElement = list->first;
            ListElement* previousElement = list->first;

            int wasSortingPerformed = 0;

            if (currentElement->next != NULL && currentElement->priority > currentElement->next->priority)
            {
                previousElement = list->first->next;
                list->first = currentElement->next;
                ListElement* temp = currentElement->next->next;
                currentElement->next->next = currentElement;
                currentElement->next = temp;
                wasSortingPerformed = 1;
            }

            while (currentElement != NULL)
            {
                if (currentElement->next != NULL && currentElement->priority > currentElement->next->priority)
                {
                    previousElement->next = currentElement->next;
                    ListElement* temp = currentElement->next->next;
                    currentElement->next->next = currentElement;
                    currentElement->next = temp;
                    wasSortingPerformed = 1;
                    previousElement = previousElement->next;
                }
                else
                {
                    previousElement = currentElement;
                    currentElement = currentElement->next;
                }
            }

            if (!wasSortingPerformed)
            {
                break;
            }
        }
    }
}
