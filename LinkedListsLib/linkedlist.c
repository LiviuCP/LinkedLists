#include <stdio.h>
#include <string.h>

#include "linkedlist.h"
#include "../Utils/codeutils.h"
#include "../Utils/error.h"

List* createList()
{
    List* list = (List*)malloc(sizeof(List));

    if (list != NULL)
    {
        list->first = NULL;
    }

    return list;
}

// user is responsible for de-allocating the Object of each element prior to erasing the list
void deleteList(List* list, void (*deallocObject)(Object* object))
{
    if (list != NULL)
    {
        if (list->first != NULL)
        {
            clearList(list, deallocObject);
        }
        free(list);
    }
}

// user should pass a custom deleter for objects with payloads containing pointers to allocated heap memory
void deleteObject(Object *object)
{
    if (object != NULL)
    {
        if (object->type != NULL)
        {
            free(object->type);
            object->type = NULL;
        }
        if (object->payload != NULL)
        {
            free(object->payload);
            object->payload = NULL;
        }
    }
    free(object);
    object = NULL;
}

/* This function is just added for having a default value to be passed to the copyContentToList() function as deep copying function pointer.
   User is responsible to pass a custom deep copying function with this signature if any list element contains an non-NULL Object.
*/
int copyObject(const ListElement* source, ListElement* destination)
{
    int success = 0;

    if (source != NULL && destination != NULL)
    {
        success = 1;
    }

    return success;
}

ListElement* createListElement()
{
    ListElement* result = (ListElement*)malloc(sizeof(ListElement));

    if (result != NULL)
    {
        result->next = NULL;
        result->priority = 0;
        result->object = NULL;
    }

    return result;
}

void prependToList(List* list, ListElement* newElement)
{
    if (list != NULL && newElement != NULL)
    {
        ASSERT_CONDITION(newElement->next == NULL, "Element to be inserted is linked to another element!");

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

ListElement* createAndPrependToList(List* list, size_t priority)
{
    ListElement* element = (ListElement*)malloc(sizeof(ListElement));

    if (element != NULL)
    {
        element->object = NULL;
        element->priority = priority;
        element->next = NULL;

        prependToList(list, element);
    }

    return element;
}

ListElement* createAndAppendToList(List* list, size_t priority)
{
    ListElement* element = (ListElement*)malloc(sizeof(ListElement));

    if (element != NULL)
    {
        element->object = NULL;
        element->priority = priority;
        element->next = NULL;

        appendToList(list, element);
    }

    return element;
}

ListElement* createAndInsertAfter(ListIterator it, size_t priority)
{
    ListElement* result = NULL;

    if (it.list != NULL)
    {
        ListElement* nextElement = createListElement();
        nextElement->priority = priority;

        if (it.current != NULL)
        {

            if (nextElement != NULL)
            {
                result = nextElement;
                insertAfter(it, nextElement);
            }
        }
        else if (it.list->first == NULL)
        {
            it.list->first = nextElement;
        }
    }

    return result;
}

void insertAfter(ListIterator it, ListElement* nextElement)
{
    if (it.list != NULL && nextElement != NULL)
    {
        ASSERT_CONDITION(nextElement->next == NULL, "Element to be inserted is linked to another element!");

        if (it.current != NULL)
        {
            if (it.current->next != NULL)
            {
                ListElement* temp = it.current->next;
                it.current->next = nextElement;
                nextElement->next = temp;
            }
            else
            {
                it.current->next = nextElement;
            }
        }
        else if (it.list->first == NULL)
        {
            it.list->first = nextElement;
        }
    }
}

ListElement* createAndInsertBefore(ListIterator it, size_t priority)
{
    ListElement* result = NULL;

    if (it.list != NULL)
    {
        ListElement* previousElement = createListElement();
        previousElement->priority = priority;

        if (previousElement != NULL)
        {
            result = previousElement;
            insertBefore(it, previousElement);
        }
    }

    return result;
}

void insertBefore(ListIterator it, ListElement* previousElement)
{
    if (it.list != NULL && previousElement != NULL)
    {
        ASSERT_CONDITION(previousElement->next == NULL, "Element to be inserted is linked to another element!");

        if (it.current != NULL)
        {
            if (it.current == it.list->first)
            {
                previousElement->next = it.list->first;
                it.list->first = previousElement;
            }
            else
            {
                ListElement* currentElement = it.list->first;
                while (currentElement->next != it.current)
                {
                    currentElement = currentElement->next;
                }
                currentElement->next = previousElement;
                previousElement->next = it.current;
            }
        }
        else
        {
            appendToList(it.list, previousElement);
        }
    }
}

void moveContentToList(List* source, List* destination)
{
    if (source != NULL && destination != NULL && source != destination && source->first != NULL)
    {
        appendToList(destination, source->first);
        source->first = NULL;
    }
}

/* The objects of the source list will not be copied into the destination list. This is because without knowing the object types the only way to achieve this is by
   shallow copying, which is not safe (two pointers would indicate to the same object). For this reason, the objects will not be copied at all so the new elements of the
   destination list will contain NULL objects
*/
ListElement* copyContentToList(const List* source, List* destination, int (*copyObjectToElement)(const ListElement* source, ListElement* destination),
                               void (*deallocObject)(Object* object))
{
    ListElement* result = NULL;

    if (source != NULL && destination != NULL && source != destination && source->first != NULL)
    {
        List* temp = createList();

        if (temp != NULL)
        {
            ListElement* currentSourceElement = source->first;
            // use createAndAppendToList() only once to avoid overhead of finding last list element
            ListElement* lastAppendedElement = createAndAppendToList(temp, currentSourceElement->priority);

            if (lastAppendedElement != NULL && copyObjectToElement(currentSourceElement, lastAppendedElement))
            {
                currentSourceElement = currentSourceElement->next;

                while (currentSourceElement != NULL)
                {
                    ListElement* elementToAppend = createListElement();

                    if (elementToAppend != NULL && copyObjectToElement(currentSourceElement, elementToAppend))
                    {
                        elementToAppend->priority = currentSourceElement->priority;
                        lastAppendedElement->next = elementToAppend;
                        lastAppendedElement = elementToAppend;
                        currentSourceElement = currentSourceElement->next;
                    }
                    else
                    {
                        // abort operation if a single additional element cannot be created
                        deleteList(temp, deallocObject);
                        temp = NULL;
                        break;
                    }
                }

                if (temp != NULL)
                {
                    result = temp->first;
                    moveContentToList(temp, destination);
                    deleteList(temp, deallocObject);
                    temp = NULL;
                }
            }
            else {
                // abort operation if the first copied element cannot be created
                deleteList(temp, deallocObject);
                temp = NULL;
            }
        }
    }

    return result;
}

void assignObjectToListElement(ListElement* element, const char* objectType, void* objectPayload)
{
    if (element != NULL && objectPayload != NULL && objectType != NULL)
    {
        ASSERT_CONDITION(element->object == NULL, "Attempt to assign object without freeing the existing one first!");

        element->object = (Object*)malloc(sizeof (Object));

        if (element->object != NULL)
        {
            element->object->type = (char*)malloc(strlen(objectType) + 1);

            if (element->object->type == NULL)
            {
                fprintf(stderr, "Unable to allocate memory for object type. Object cannot be assigned!");
                free(element->object);
                element->object = NULL;
            }
            else
            {
                strcpy(element->object->type, objectType);
                element->object->payload= objectPayload;
            }
        }
        else
        {
            fprintf(stderr, "Unable to allocate memory for object. Object cannot be assigned!");
        }
    }
}

Object* removeObjectFromListElement(ListElement* element)
{
    Object* result = NULL;

    if (element != NULL && element->object != NULL)
    {
        result  = element->object;
        element->object = NULL;
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

ListElement* removePreviousListElement(ListIterator it)
{
    ListElement* result = NULL;

    if (it.list != NULL && it.list->first != NULL && it.current != it.list->first)
    {
        ListElement* currentElement = it.list->first;
        ListElement* previousElement = it.list->first;

        if (currentElement->next == it.current)
        {
            it.list->first = it.current;
        }
        else
        {
            currentElement = currentElement->next;

            while (currentElement->next != it.current)
            {
                previousElement = currentElement;
                currentElement = currentElement->next;
            }

            previousElement->next = it.current;
        }

        currentElement->next = NULL;
        result = currentElement;
    }

    return result;
}

ListElement* removeNextListElement(ListIterator it)
{
    ListElement* result = NULL;

    if (it.list != NULL && it.current != NULL && it.current->next != NULL)
    {
        result = it.current->next;
        it.current->next = result->next;
        result->next = NULL;
    }

    return result;
}

ListElement* removeCurrentListElement(ListIterator it)
{
    ListElement* result = NULL;

    if (it.list != NULL && it.current != NULL)
    {
        lnext(&it);
        result =removePreviousListElement(it);
    }

    return result;
}

// user is responsible for de-allocating the Object of each element prior to erasing elements from the list
void clearList(List *list, void (*deallocObject)(Object* object))
{
    if (list != NULL)
    {
        ListElement* currentElement = list->first;
        list->first = NULL;

        while (currentElement != NULL)
        {
            deallocObject(currentElement->object);
            currentElement->object = NULL;
            ListElement* elementToDelete = currentElement;
            currentElement = currentElement->next;
            free(elementToDelete);
            elementToDelete = NULL;
        }
    }
}

void reverseList(List* list)
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

void sortDescendingByPriority(List* list)
{
    SORT_LIST(DESCENDING , priority)
}

size_t getListSize(const List* list)
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

ListElement* getLastElement(const List* list)
{
    ListElement* result = NULL;

    if (list != NULL && list->first != NULL)
    {
        ListElement* currentElement = list->first;

        while (currentElement->next != NULL)
        {
            currentElement = currentElement->next;
        }

        result = currentElement;
    }

    return result;
}

int isElementContained(const ListElement* element, const List* list)
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

ListIterator lbegin(List* list)
{
    ListIterator result;

    ASSERT_CONDITION(list != NULL, "Attempt to get iterator from NULL list");

    result.list = list;
    result.current = list->first;

    return result;
}

ListIterator lend(List* list)
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

ListElement** moveListToArray(List* list, size_t* arraySize)
{
    ListElement** result = NULL;
    *arraySize = 0;

    if (list != NULL && list->first != NULL)
    {
        const size_t nrOfElements = getListSize(list);

        ListElement** array = (ListElement**)calloc(nrOfElements, sizeof(ListElement*));

        if (array != NULL)
        {
            ListElement* currentElement = list->first;
            ListElement** currentArrayElement = array;

            while (currentElement != NULL)
            {
                *currentArrayElement = currentElement;
                currentElement = currentElement->next;
                (*currentArrayElement)->next = NULL; // decouple the list elements, array will now maintain cohesion and order
                ++currentArrayElement;
            }

            list->first = NULL;
            *arraySize = nrOfElements;
            result = array;
            array = NULL;
        }
    }

    return result;
}

List* moveArrayToList(ListElement** array, const size_t arraySize)
{
    List* result = NULL;

    if (array != NULL && arraySize > 0)
    {
        List* list = createList();

        if (list != NULL)
        {
            ListElement** currentArrayElement = array;
            list->first = *currentArrayElement;
            *currentArrayElement = NULL;
            ListElement* currentListElement = list->first;

            for (size_t index = 1; index < arraySize; ++index)
            {
                currentListElement->next = array[index];
                currentListElement = currentListElement->next;
                array[index] = NULL;
            }

            result = list;
        }
    }

    return result;
}

/* These two functions are just for illustrating the creation of custom deallocator and custom deep copy function */
void customDeleteObject(Object* object)
{
    if (object != NULL)
    {
        if (object->payload != NULL)
        {
            ASSERT_CONDITION(object->type != NULL, "Null object type when payload is not null"); // object should have both type and payload
            if (strcmp(object->type,"Segment") == 0)
            {
                Segment* segment = (Segment*)object->payload;
                free(segment->start);
                segment->start = NULL;
                free(segment->stop);
                segment->stop = NULL;
            }
            else if (strcmp(object->type, "LocalConditions") == 0)
            {
                LocalConditions* conditions = (LocalConditions*)object->payload;
                free(conditions->position);
                conditions->position = NULL;
            }
        }

        free(object->payload);
        object->payload = NULL;
        free(object->type);
        object->type = NULL;
        free(object);
        object = NULL;
    }
}

int customCopyObject(const ListElement* source, ListElement* destination)
{
    int success = 0;

    if (source != NULL && destination != NULL && source->object != NULL && source->object->type != NULL && source->object->payload != NULL)
    {
        char* destinationObjectType = (char*)malloc(strlen(source->object->type)+1);
        if (destinationObjectType != NULL)
        {
            strcpy(destinationObjectType, source->object->type);

            if (strcmp(destinationObjectType, "Segment") == 0)
            {
                Segment* destinationObjectPayload = (Segment*)malloc(sizeof(Segment));
                if (destinationObjectPayload != NULL)
                {
                    destinationObjectPayload->start = (Point*)malloc(sizeof(Point));
                    destinationObjectPayload->stop = (Point*)malloc(sizeof(Point));
                    Object* destinationObject = (Object*)malloc(sizeof(Object));

                    if (destinationObjectPayload->start != NULL && destinationObjectPayload->stop != NULL && destinationObject != NULL)
                    {
                        const Segment* sourceObjectPayload = (Segment*)source->object->payload;
                        destinationObjectPayload->start->x = sourceObjectPayload->start->x;
                        destinationObjectPayload->start->y = sourceObjectPayload->start->y;
                        destinationObjectPayload->stop->x = sourceObjectPayload->stop->x;
                        destinationObjectPayload->stop->y = sourceObjectPayload->stop->y;
                        destinationObject->type = destinationObjectType;
                        destinationObject->payload = (void*)destinationObjectPayload;
                        destination->object = destinationObject;
                        success = 1;
                    }
                    else
                    {
                        free(destinationObjectPayload->start);
                        destinationObjectPayload->start = NULL;
                        free(destinationObjectPayload->stop);
                        destinationObjectPayload->stop = NULL;
                        free(destinationObjectPayload);
                        destinationObjectPayload = NULL;
                        free(destinationObject);
                        destinationObject = NULL;
                    }
                }

            }
            else if (strcmp(destinationObjectType, "LocalConditions") == 0)
            {
                LocalConditions* destinationObjectPayload = (LocalConditions*)malloc(sizeof(LocalConditions));
                if (destinationObjectPayload != NULL)
                {
                    destinationObjectPayload->position = (Point*)malloc(sizeof(Point));
                    Object* destinationObject = (Object*)malloc(sizeof(Object));
                    if (destinationObjectPayload->position != NULL && destinationObject != NULL)
                    {
                        const LocalConditions* sourceObjectPayload = (LocalConditions*)source->object->payload;
                        destinationObjectPayload->position->x = sourceObjectPayload->position->x;
                        destinationObjectPayload->position->y = sourceObjectPayload->position->y;
                        destinationObjectPayload->humidity = sourceObjectPayload->humidity;
                        destinationObjectPayload->temperature = sourceObjectPayload->temperature;
                        destinationObject->type = destinationObjectType;
                        destinationObject->payload = (void*)destinationObjectPayload;
                        destination->object = destinationObject;
                        success = 1;
                    }
                    else
                    {
                        free(destinationObjectPayload->position);
                        destinationObjectPayload->position = NULL;
                        free(destinationObjectPayload);
                        destinationObjectPayload = NULL;
                        free(destinationObject);
                        destinationObject = NULL;
                    }
                }
            }
        }

        if (!success)
        {
            // no copy action to be performed if object type is unknown
            free(destinationObjectType);
            destinationObjectType = NULL;
        }
    }

    return success;
}
