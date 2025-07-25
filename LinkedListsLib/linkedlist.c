#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "linkedlist.h"
#include "listsortutils.h"

#include "../Utils/error.h"
#include "../Utils/testobjects.h"

List* createEmptyList()
{
    List* list = (List*)malloc(sizeof(List));

    if (list != NULL)
    {
        list->first = NULL;
        list->last = NULL;
    }

    return list;
}

List* createListFromPrioritiesArray(const size_t* priorityArray, const size_t arraySize)
{
    List* result = NULL;

    if (priorityArray != NULL && arraySize > 0)
    {
        List* list = createEmptyList();

        bool success = true;

        for (size_t index = 0; index < arraySize; ++index)
        {
            if (!createAndAppendToList(list, priorityArray[index]))
            {
                success = false;
                break;
            }
        }

        if (success)
        {
            result = list;
        }
        else
        {
            deleteList(list, deleteObjectPayload);
        }

        list = NULL;
    }

    return result;
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

ListElement* createListElement()
{
    ListElement* result = (ListElement*)malloc(sizeof(ListElement));

    if (result != NULL)
    {
        result->next = NULL;
        result->priority = 0;

        // an empty object should have NULL payload and a negative type
        result->object.type = -1;
        result->object.payload = NULL;
    }

    return result;
}

ListElement* createAndAppendToList(List* list, size_t priority)
{
    ListElement* element = (ListElement*)malloc(sizeof(ListElement));

    if (element != NULL)
    {
        element->next = NULL;
        element->priority = priority;

        // empty object
        element->object.type = -1;
        element->object.payload = NULL;

        appendToList(list, element);
    }

    return element;
}

void appendToList(List* list, ListElement* newElement)
{
    if (list != NULL && newElement != NULL)
    {
        if (list->first == NULL)
        {
            ASSERT_CONDITION(list->last == NULL, "Non-null pointer detected for last list element")

            list->first = newElement;
            list->last = newElement;
        }
        else
        {
            ASSERT_CONDITION(list->last != NULL, "Null pointer detected for last list element")

            list->last->next = newElement;
            list->last = newElement;
        }
    }
}

ListElement* createAndPrependToList(List* list, size_t priority)
{
    ListElement* element = (ListElement*)malloc(sizeof(ListElement));

    if (element != NULL)
    {
        element->next = NULL;
        element->priority = priority;

        // empty object
        element->object.type = -1;
        element->object.payload = NULL;

        prependToList(list, element);
    }

    return element;
}

void prependToList(List* list, ListElement* newElement)
{
    if (list != NULL && newElement != NULL)
    {
        ASSERT_CONDITION(newElement->next == NULL, "Element to be inserted is linked to another element!");

        if (list->first == NULL)
        {
            list->last = newElement;
        }

        newElement->next = list->first;
        list->first = newElement;
    }
}

ListElement* createAndInsertAfter(ListIterator it, size_t priority)
{
    ASSERT_CONDITION(it.list != NULL, "Iterator points to NULL list")

    ListElement* result = NULL;
    ListElement* nextElement = createListElement();

    if (nextElement != NULL)
    {
        result = nextElement;
        nextElement->priority = priority;
        insertAfter(it, nextElement);
    }

    return result;
}

void insertAfter(ListIterator it, ListElement* nextElement)
{
    ASSERT_CONDITION(it.list != NULL, "Iterator points to NULL list")

    if (nextElement != NULL)
    {
        ASSERT_CONDITION(nextElement->next == NULL, "Element to be inserted is linked to another element!");

        if (it.current != NULL)
        {
            ASSERT_CONDITION(it.list->last != NULL, "Null pointer detected for last list element")

            if (it.current->next != NULL)
            {
                ListElement* temp = it.current->next;
                it.current->next = nextElement;
                nextElement->next = temp;
            }
            else
            {
                it.current->next = nextElement;
                it.list->last = nextElement;
            }
        }
        else if (it.list->first == NULL)
        {
            ASSERT_CONDITION(it.list->last == NULL, "Non-null pointer detected for last list element")

            it.list->first = nextElement;
            it.list->last = nextElement;
        }
    }
}

ListElement* createAndInsertBefore(ListIterator it, size_t priority)
{
    ASSERT_CONDITION(it.list != NULL, "Iterator points to NULL list");

    ListElement* result = NULL;
    ListElement* previousElement = createListElement();

    if (previousElement != NULL)
    {
        previousElement->priority = priority;
        result = previousElement;
        insertBefore(it, previousElement);
    }

    return result;
}

void insertBefore(ListIterator it, ListElement* previousElement)
{
    ASSERT_CONDITION(it.list != NULL, "Iterator points to NULL list");
    if (previousElement != NULL)
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
            ASSERT_CONDITION(list->last != NULL, "Null pointer detected for last list element")

            list->first = NULL;
            list->last = NULL;
        }
    }

    return removedElement;
}

ListElement* removeLastListElement(List* list)
{
    ListElement* removedElement = NULL;

    if (list != NULL && list->first != NULL)
    {
        ASSERT_CONDITION(list->last != NULL, "Null pointer detected for last list element")

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
            list->last = currentElement;
        }
        else
        {
            removedElement = currentElement;
            list->first = NULL;
            list->last = NULL;
        }
    }

    return removedElement;
}

ListElement* removePreviousListElement(ListIterator it)
{
    ASSERT_CONDITION(it.list != NULL, "Iterator points to NULL list");
    ListElement* result = NULL;

    if (it.list->first != NULL && it.current != it.list->first)
    {
        ASSERT_CONDITION(it.list->last != NULL, "Null pointer detected for last list element")

        ListElement* currentElement = it.list->first;
        ListElement* previousElement = it.list->first;

        if (currentElement->next == it.current)
        {
            if (it.list->first == it.list->last)
            {
                it.list->last = NULL; // one element list, the only element is removed, becomes NULL
            }

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

            if (currentElement == it.list->last)
            {
                it.list->last = previousElement;
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
    ASSERT_CONDITION(it.list != NULL, "Iterator points to NULL list");
    ListElement* result = NULL;

    if (it.current != NULL && it.current->next != NULL)
    {
        ASSERT_CONDITION(it.list->last != NULL, "Null pointer detected for last list element")

        result = it.current->next;
        it.current->next = result->next;
        result->next = NULL;

        if (result == it.list->last)
        {
            it.list->last = it.current->next;
        }
    }

    return result;
}

ListElement* removeCurrentListElement(ListIterator it)
{
    ASSERT_CONDITION(it.list != NULL, "Iterator points to NULL list");
    ListElement* result = NULL;

    if (it.current != NULL)
    {
        lnext(&it);
        result = removePreviousListElement(it);
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
        list->last = NULL;

        while (currentElement != NULL)
        {
            deallocObject(&currentElement->object);
            ListElement* elementToDelete = currentElement;
            currentElement = currentElement->next;
            free(elementToDelete);
            elementToDelete = NULL;
        }
    }
}

void swapListElements(ListIterator firstIt, ListIterator secondIt)
{
    ASSERT_CONDITION(firstIt.list != NULL && secondIt.list != NULL, "At least one iterator points to a NULL list");
    ASSERT_CONDITION(firstIt.list == secondIt.list, "Iterators belong to different lists");

    if (firstIt.current != NULL && secondIt.current != NULL && firstIt.current != secondIt.current)
    {
        ListElement* firstPrevious = getPreviousListElement(firstIt);
        ListElement* secondPrevious = getPreviousListElement(secondIt);
        ListElement* firstNext = firstIt.current->next;
        ListElement* secondNext = secondIt.current->next;

        if (firstPrevious != NULL)
        {
            firstPrevious->next = secondIt.current;
        }
        if (secondPrevious != NULL)
        {
            secondPrevious->next = firstIt.current;
        }

        firstIt.current->next = secondNext;
        secondIt.current->next = firstNext;
    }
}

void reverseList(List* list)
{
    if (list != NULL && list->first != NULL)
    {
        ASSERT_CONDITION(list->last != NULL, "Null pointer detected for last list element")

        ListElement* previousElement = list->first;
        ListElement* currentElement = previousElement->next;
        previousElement->next = NULL;

        while(currentElement != NULL)
        {
            ListElement* nextElement = currentElement->next;
            currentElement->next = previousElement;
            previousElement = currentElement;
            currentElement = nextElement;
        }

        ListElement* lastElement = list->first;
        list->first = list->last;
        list->last = lastElement;
    }
}

/* The batch size should be greater than 1 and smaller or equal to the list size, otherwise nothing gets reversed
   Return value: last element from the last reversed batch after reversing finished (NULL if no reversing applied)
 */
ListElement* batchReverseList(List* list, size_t batchSize)
{
    ListElement* lastReversedElement = NULL;
    const size_t nrOfElements = getListSize(list);

    size_t nrOfBatchesToReverse = 0;
    size_t nrOfUnreversableElements = nrOfElements;

    if (nrOfElements > 0 && batchSize > 1 && batchSize <= nrOfElements)
    {
        nrOfBatchesToReverse = nrOfElements / batchSize;
        nrOfUnreversableElements = nrOfElements % batchSize;
    }

    ListElement* firstElementInBatch = list->first;
    ListElement* elementToLinkToNextBatch = NULL; // connection point to the next batch (once this has been reversed) / the unchanged elements
    ListElement* previousElement = NULL;
    ListElement* currentElement = NULL;
    size_t nrOfBatchesLeftToReverse = nrOfBatchesToReverse;

    while (nrOfBatchesLeftToReverse > 0)
    {
        previousElement = firstElementInBatch;
        currentElement = previousElement->next;
        previousElement->next = NULL; // break the connection between the two nodes, they will be connected the other way around (reversed)

        // elements in batch are reversed 2 at a time (0 - 1, 1 -2, ...), hence the decrease by 1 (for 2 elements in batch only 1 reversal etc)
        size_t reversalsCountInBatch = batchSize - 1;

        while (reversalsCountInBatch > 0)
        {
            ListElement* nextElement = currentElement->next;
            currentElement->next = previousElement;
            previousElement = currentElement;
            currentElement = nextElement;
            --reversalsCountInBatch;
        }

        if (nrOfBatchesLeftToReverse == nrOfBatchesToReverse)
        {
            list->first = previousElement; // reference to first element to be updated during first batch
        }

        if (0 == nrOfUnreversableElements && 1 == nrOfBatchesLeftToReverse)
        {
            list->last = firstElementInBatch; // reference to last element to be updated during last batch if no unreversable elements exist after
        }

        if (elementToLinkToNextBatch != NULL)
        {
            elementToLinkToNextBatch->next = previousElement; // this is applicable starting with second batch (there is no previous batch to be linked to first batch)
        }

        firstElementInBatch->next = currentElement; // link the reversed batch to the elements to follow (firstElementInBatch is actually the last element after reversal)
        elementToLinkToNextBatch = firstElementInBatch; // this is for later relinking if the elements to follow are from a batch to be reversed (once this has been done)
        firstElementInBatch = currentElement; // finalize move to next batch (if any)

        if (1 == nrOfBatchesLeftToReverse)
        {
            lastReversedElement = elementToLinkToNextBatch;
        }

        --nrOfBatchesLeftToReverse;
    }

    return lastReversedElement;
}

void sortAscendingByPriority(List* list)
{
    BUBBLE_SORT(ASCENDING , priority)
}

void sortDescendingByPriority(List* list)
{
    BUBBLE_SORT(DESCENDING , priority)
}

bool sortByRandomAccess(List* list, void (*sortingAlgorithm)(ListElement** array, const size_t arraySize))
{
    bool result = false;

    if (list != NULL && list->first != NULL)
    {
        size_t arraySize = 0;
        ListElement** array = moveListToArray(list, &arraySize);

        if (array != NULL)
        {
            sortingAlgorithm(array, arraySize);
            moveArrayToList(array, arraySize, list);
            result = true;
        }

        free(array);
        array = NULL;
    }

    return result;
}

void moveContentToList(List* source, List* destination)
{
    if (source != NULL && destination != NULL && source != destination && source->first != NULL)
    {
        ASSERT_CONDITION(source->last != NULL, "Null pointer detected for last source list element")

        if (destination->first != NULL)
        {
            ASSERT_CONDITION(destination->last != NULL, "Null pointer detected for last destination list element")
            destination->last->next = source->first;
        }
        else
        {
            ASSERT_CONDITION(destination->last == NULL, "Non-null pointer detected for last destination list element")
            destination->first = source->first;
        }

        destination->last = source->last;
        source->first = NULL;
        source->last = NULL;
    }
}

/* The objects of the source list will not be copied into the destination list unless a proper custom copier function is provided (like customCopyObject()).
   This is because without knowing the object types the only way to achieve this is by shallow copying, which is not safe (two pointers would indicate to the same object).
   For this reason, by default (with copyObjectPlaceholder()) the objects will not be copied at all so the new elements of the destination list will contain NULL objects.
*/
ListElement* copyContentToList(const List* source, List* destination, bool (*copyObjectToElement)(const ListElement* source, ListElement* destination),
                               void (*deallocObject)(Object* object))
{
    ListElement* result = NULL;

    if (source != NULL && destination != NULL && source != destination && source->first != NULL)
    {
        ASSERT_CONDITION(source->last != NULL, "Null pointer detected for last source list element")

        List* temp = createEmptyList();

        if (temp != NULL)
        {
            ListElement* currentSourceElement = source->first;

            while (currentSourceElement != NULL)
            {
                ListElement* elementToAppend = createAndAppendToList(temp, currentSourceElement->priority);

                if (elementToAppend != NULL && copyObjectToElement(currentSourceElement, elementToAppend))
                {
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
    }

    return result;
}

ListElement** moveListToArray(List* list, size_t* arraySize)
{
    ListElement** result = NULL;
    *arraySize = 0;

    if (list != NULL && list->first != NULL)
    {
        ASSERT_CONDITION(list->last != NULL, "Null pointer detected for last list element")

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
            list->last = NULL;
            *arraySize = nrOfElements;
            result = array;
            array = NULL;
        }
    }

    return result;
}

void moveArrayToList(ListElement** array, const size_t arraySize, List* list)
{
    if (array != NULL && arraySize > 0 && list != NULL)
    {
        ASSERT_CONDITION(list->first == NULL || list->last == NULL, "Attempt to move pointers array to unempty list")
        ASSERT_CONDITION(*array != NULL, "NULL value array element identified")

        ListElement** currentArrayElement = array;
        list->first = *currentArrayElement;
        ListElement* currentListElement = list->first;

        for (size_t index = 1; index < arraySize; ++index)
        {
            currentListElement->next = array[index];
            ASSERT_CONDITION(array[index] != NULL, "NULL value array element identified");
            currentListElement = currentListElement->next;
        }

        // ensure the list is correctly closed and the array loses ownership of the elements
        list->last = currentListElement;
        array[arraySize-1]->next = NULL;
        for (size_t index = 0; index < arraySize; ++index)
        {
            array[index] = NULL;
        }
    }
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

ListElement* getListElementAtIndex(const List* list, size_t index)
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

ListElement* getPreviousListElement(ListIterator it)
{
    ASSERT_CONDITION(it.list != NULL, "Iterator points to NULL list");
    ListElement* result = NULL;

    const List* list = it.list;

    if (list != NULL && list->first != NULL  && it.current != list->first)
    {
        ListElement* previousElement = list->first;

        while (previousElement->next != it.current)
        {
            previousElement = previousElement->next;
        }

        result = previousElement;
    }

    return result;
}

ListElement* getLastListElement(const List* list)
{
    ListElement* result = NULL;

    if (list != NULL && list->first != NULL)
    {
        result = list->last;
        ASSERT_CONDITION(result != NULL, "Null pointer detected for last list element")
    }

    return result;
}

bool isListElementContained(const ListElement* element, const List* list)
{
    bool result = false;

    if (element != NULL && list != NULL && list->first != NULL)
    {
        ListElement* currentElement = list->first;

        while (currentElement != NULL)
        {
            if (element == currentElement)
            {
                result = true;
                break;
            }
            currentElement = currentElement->next;
        }
    }

    return result;
}

bool isSortedAscendingByPriority(const List* list)
{
    CHECK_IF_SORTED(ASCENDING, priority);
}

bool isSortedDescendingByPriority(const List* list)
{
    CHECK_IF_SORTED(DESCENDING, priority);
}

ListIterator lbegin(List* list)
{
    ASSERT_CONDITION(list != NULL, "Attempt to get iterator from NULL list");

    ListIterator result;

    result.list = list;
    result.current = list->first;

    return result;
}

ListIterator lend(List* list)
{
    ASSERT_CONDITION(list != NULL, "Attempt to get iterator from NULL list")

    ListIterator result;

    result.list = list;
    result.current = NULL;

    return result;
}

void lnext(ListIterator* iterator)
{
    ASSERT_CONDITION(iterator != NULL, "Attempt to advance a NULL iterator")
    ASSERT_CONDITION(iterator -> list != NULL, "Iterator points to NULL list");

    if (iterator->current != NULL)
    {
        iterator->current = iterator->current->next;
    }
}

bool areIteratorsEqual(ListIterator first, ListIterator second)
{
    ASSERT_CONDITION(first.list != NULL && second.list != NULL, "At least one iterator points to a NULL list");
    ASSERT_CONDITION(first.list == second.list, "Iterators belong to different lists")

    return first.current == second.current;
}

void printListContentToFile(const List* list, const char* outFile, const char* header)
{
    if (list != NULL)
    {
        FILE* outputFile = fopen(outFile, "w");
        int errorNumber = 0;

        if (outputFile != NULL)
        {
            ListElement* currentElement = list->first;
            size_t elementIndex = 0;

            fprintf(outputFile, "%s", header);

            if (getListSize(list) == 0)
            {
                fprintf(outputFile, "EMPTY LIST");
            }

            while (currentElement != NULL)
            {
                fprintf(outputFile, "Element: %d\t", (int)elementIndex);
                fprintf(outputFile, "Priority: %d\t", (int)currentElement->priority);
                fprintf(outputFile, "Has empty Object: ");
                if (currentElement->object.type >= 0)
                {
                    fprintf(outputFile, "no\t");
                    fprintf(outputFile, "Object type: ");
                    fprintf(outputFile, "%s", getTestObjectTypeAsString(currentElement->object.type));
                }
                else
                {
                    fprintf(outputFile, "yes");
                }
                fprintf(outputFile, "\n");

                currentElement = currentElement->next;
                ++elementIndex;
            }
            fclose(outputFile);
            outputFile = NULL;
        }
        else
        {
            errorNumber = errno;
            fprintf(stderr, "A file opening error occurred!\n");
            fprintf(stderr, "Error number: %d\n", errno);
            fprintf(stderr, "Error description: %s\n", strerror(errorNumber));
        }
    }
}

void assignObjectContentToListElement(ListElement* element, const int objectType, void* const objectPayload)
{
    if (element != NULL && objectPayload != NULL && objectType >= 0)
    {
        ASSERT_CONDITION(element->object.payload == NULL, "Attempt to assign object without emptying the existing one first!");

        element->object.type = objectType;
        element->object.payload = objectPayload;
    }
}

Object* detachContentFromListElement(ListElement* element)
{
    Object* result = NULL;

    if (element != NULL)
    {
        ASSERT_CONDITION((element->object.type >= 0 && element->object.payload != NULL) || (element->object.type < 0 && element->object.payload == NULL), "Invalid object detected")

        Object* temp = createObject(element->object.type, element->object.payload);

        if (temp != NULL)
        {
            result = temp;
            element->object.type = -1;
            element->object.payload = NULL;
        }
    }

    return result;
}

// user should pass a custom deleter for objects with payloads containing pointers to allocated heap memory
void deleteObjectPayload(Object *object)
{
    if (object != NULL)
    {
        free(object->payload);
        object->payload = NULL;
        object->type = -1;
    }
}

/* This function is just added for having a default value to be passed to the copyContentToList() function as deep copying function pointer.
   User is responsible to pass a custom deep copying function with this signature if any list element contains an non-empty Object.

   ---> To be used for lists with EMPTY objects only!
*/
bool copyObjectPlaceholder(const ListElement* source, ListElement* destination)
{
    bool success = false;

    if (source != NULL && destination != NULL)
    {
        ASSERT_CONDITION(source->object.type < 0 && source->object.payload == NULL && destination->object.type < 0 && destination->object.payload == NULL,
                         "The source and/or destination element object is either invalid or non-empty")
        success = true;
    }

    return success;
}

/* This function is just for illustrating the creation of custom deep copy function
    ---> for test purposes only
*/
bool customCopyObject(const ListElement* source, ListElement* destination)
{
    bool success = false;

    /* copy a NON-EMPTY source object to EMPTY destination object to avoid any memory leaks */
    if (source != NULL && destination != NULL && source->object.payload != NULL && destination->object.payload == NULL)
    {
        ASSERT_CONDITION(source->object.type >= 0, "Invalid source object detected")

        if (source->object.type == SEGMENT)
        {
            Segment* destinationObjectPayload = (Segment*)malloc(sizeof(Segment));
            if (destinationObjectPayload != NULL)
            {
                destinationObjectPayload->start = (Point*)malloc(sizeof(Point));
                destinationObjectPayload->stop = (Point*)malloc(sizeof(Point));

                if (destinationObjectPayload->start != NULL && destinationObjectPayload->stop != NULL)
                {
                    destinationObjectPayload->start->x = ((Segment*)source->object.payload)->start->x;
                    destinationObjectPayload->start->y = ((Segment*)source->object.payload)->start->y;
                    destinationObjectPayload->stop->x = ((Segment*)source->object.payload)->stop->x;
                    destinationObjectPayload->stop->y = ((Segment*)source->object.payload)->stop->y;
                    destination->object.type = source->object.type;
                    destination->object.payload = (void*)destinationObjectPayload;
                    success = true;
                }

                if (!success) // ensure any allocated memory is freed in case deep copy failed
                {
                    free(destinationObjectPayload->start);
                    destinationObjectPayload->start = NULL;
                    free(destinationObjectPayload->stop);
                    destinationObjectPayload->stop = NULL;
                    free(destinationObjectPayload);
                    destinationObjectPayload = NULL;
                }
            }
        }
        else if (source->object.type == LOCAL_CONDITIONS)
        {
            LocalConditions* destinationObjectPayload = (LocalConditions*)malloc(sizeof(LocalConditions));
            if (destinationObjectPayload != NULL)
            {
                destinationObjectPayload->position = (Point*)malloc(sizeof(Point));
                if (destinationObjectPayload->position != NULL)
                {
                    destinationObjectPayload->position->x = ((LocalConditions*)source->object.payload)->position->x;
                    destinationObjectPayload->position->y = ((LocalConditions*)source->object.payload)->position->y;
                    destinationObjectPayload->humidity = ((LocalConditions*)source->object.payload)->humidity;
                    destinationObjectPayload->temperature = ((LocalConditions*)source->object.payload)->temperature;
                    destination->object.type = source->object.type;
                    destination->object.payload = (void*)destinationObjectPayload;
                    success = true;
                }

                if (!success) // ensure any allocated memory is freed in case deep copy failed
                {
                    free(destinationObjectPayload);
                    destinationObjectPayload = NULL;
                }
            }
        }
    }

    return success;
}
