#include <stdio.h>
#include <string.h>

#include "linkedlist.h"
#include "../Utils/codeutils.h"
#include "../Utils/error.h"

List *createList()
{
    List* list = (List*)malloc(sizeof(List));

    if (list != NULL)
    {
        list->first = NULL;
    }

    return list;
}

void deleteList(List *list)
{
    if (list != NULL)
    {
        if (list->first != NULL)
        {
            clearList(list);
        }
        free(list);
    }
}

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
        element->object = NULL;
        element->objectType = NULL;
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
        element->object = NULL;
        element->objectType = NULL;
        element->priority = priority;
        element->next = NULL;

        appendToList(list, element);
    }

    return element;
}

void assignObjectToListElement(ListElement* element, void* object, const char* objectType)
{
    if (element != NULL && object != NULL && objectType != NULL)
    {
        ASSERT_CONDITION(element->object == NULL && element->objectType == NULL, "Attempt to assign object without freeing the existing one first!");

        element->objectType = (char*)malloc(strlen(objectType) + 1);

        if (element->objectType == NULL)
        {
            fprintf(stderr, "Unable to allocate memory for object type. Object cannot be assigned!");
        }
        else
        {
            strcpy(element->objectType, objectType);
            element->object = object;
        }
    }
}

void* removeObjectFromListElement(ListElement* element)
{
    ListElement* result = NULL;

    if (element != NULL && element->object != NULL)
    {
        ASSERT_CONDITION(element->objectType != NULL, "Object type description referring to NULL object!");

        result  = element->object;
        element->object = NULL;
        free(element->objectType);
        element->objectType = NULL;
    }

    return result;
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
            removeObjectFromListElement(elementToDelete);
            free(elementToDelete);
            elementToDelete = NULL;
        }
    }
}

void reverseList(List *list)
{
    if (list != NULL && list->first != NULL)
    {
        ListElement* firstElement = list->first;
        ListElement* currentElement = firstElement->next;
        firstElement->next = NULL;

        while(currentElement != NULL)
        {
            ListElement* nextElement = currentElement->next;
            currentElement->next = firstElement;
            firstElement = currentElement;
            currentElement = nextElement;
        }

        list->first = firstElement;
    }
}

void sortAscendingByPriority(List* list)
{
    SORT_LIST(ASCENDING , priority)
}

void sortDescendingByPriority(List *list)
{
    SORT_LIST(DESCENDING , priority)
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
        ASSERT_CONDITION(index < getListSize(list), "The index is out of bounds!")

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

    ASSERT_CONDITION(list != NULL, "Attempt to get iterator from NULL list");

    result.list = list;
    result.current = list->first;

    return result;
}

ListIterator lend(List *list)
{
    ListIterator result;

    ASSERT_CONDITION(list != NULL, "Attempt to get iterator from NULL list")

    result.list = list;
    result.current = NULL;

    return result;
}

void lnext(ListIterator* iterator)
{
    ASSERT_CONDITION(iterator != NULL, "Attempt to advance a NULL iterator")

    if (iterator->current != NULL)
    {
        iterator->current = iterator->current->next;
    }
}

int areIteratorsEqual(ListIterator first, ListIterator second)
{
    ASSERT_CONDITION(first.list == second.list, "Iterators belong to different lists")
    return first.current == second.current;
}

int isElementContained(const ListElement* element, const List *list)
{
    int result = 0;

    if (element != NULL && list != NULL && list->first != NULL)
    {
        ListElement* currentElement = list->first;

        while (currentElement != NULL)
        {
            if (element == currentElement)
            {
                result = 1;
                break;
            }
            currentElement = currentElement->next;
        }
    }

    return result;
}

ListElement* createListElement()
{
    ListElement* result = (ListElement*)malloc(sizeof(ListElement));

    if (result != NULL)
    {
        result->next = NULL;
        result->priority = 0;
        result->object = NULL;
        result->objectType = NULL;
    }

    return result;
}
