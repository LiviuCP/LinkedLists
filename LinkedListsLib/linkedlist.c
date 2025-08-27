#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "linkedlist.h"
#include "listsortutils.h"
#include "testobjects.h"

/* This function is for private use only; it erases the list elements without deallocating the payload of the Object
   Should be used with caution (might cause memory leaks) and only when sure that references to each Object data (type + payload) are contained within another list
*/
static void clearListWithoutObjectsDeallocation(List* list);

List* createEmptyList(ListElementsPool* elementsPool)
{
    List* list = (List*)malloc(sizeof(List));

    if (list != NULL)
    {
        list->first = NULL;
        list->last = NULL;
        list->elementsPool = elementsPool;
    }

    return list;
}

List* createEmptyLists(size_t count, ListElementsPool* elementsPool)
{
    List* first = NULL;

    if (count > 0)
    {
        first = (List*)calloc(count, sizeof(List));
    }

    if (first)
    {
        for (size_t index = 0; index < count; ++index)
        {
            List* current = &first[index];
            current->first = NULL;
            current->last = NULL;
            current->elementsPool = elementsPool;
        }
    }

    return first;
}

List* createListFromPrioritiesArray(const size_t* prioritiesArray, const size_t arraySize, ListElementsPool* elementsPool)
{
    List* list = NULL;

    if (prioritiesArray != NULL && arraySize > 0)
    {
        list = createEmptyList(elementsPool);
    }

    if (list != NULL)
    {
        bool success = false;

        if (elementsPool != NULL)
        {
            ListElement** elementsToAquire = (ListElement**)malloc(arraySize * sizeof(ListElement*));

            if (elementsToAquire != NULL)
            {
                success = aquireElements(elementsPool, elementsToAquire, arraySize);
                const size_t elementsCountToAppend = success ? arraySize : 0;

                for (size_t index = 0; index < elementsCountToAppend; ++index)
                {
                    ListElement* element = elementsToAquire[index];
                    ASSERT(element != NULL, "NULL element aquired from elements pool!");

                    if (element != NULL)
                    {
                        initListElement(element);
                        element->priority = prioritiesArray[index];
                        appendToList(list, element);
                    }
                }

                free(elementsToAquire); // no longer needed, elements got appended to list
                elementsToAquire = NULL;
            }
        }
        else
        {
            success = true;

            for (size_t index = 0; index < arraySize; ++index)
            {
                ListElement* listElement = createAndAppendToList(list, prioritiesArray[index]);

                if (!listElement)
                {
                    success = false;
                    break;
                }
            }
        }

        if (!success)
        {
            deleteList(list, deleteObjectPayload);
            list = NULL;
        }
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
            list->elementsPool = NULL; // elementsPool is not owned by list, hence removing any reference to it is sufficient
        }

        free(list);
    }
}

ListElement* createAndAppendToList(List* list, size_t priority)
{
    ListElement* element = NULL;

    if (list != NULL)
    {
        element = list->elementsPool != NULL ? aquireElement(list->elementsPool) : (ListElement*)malloc(sizeof(ListElement));

        if (element != NULL)
        {
            element->next = NULL;
            element->priority = priority;

            // empty object
            element->object.type = -1;
            element->object.payload = NULL;

            appendToList(list, element);
        }
    }

    return element;
}

void appendToList(List* list, ListElement* newElement)
{
    if (list != NULL && newElement != NULL)
    {
        if (list->first == NULL)
        {
            ASSERT(list->last == NULL, "Non-null pointer detected for last list element");

            list->first = newElement;
            list->last = newElement;
        }
        else
        {
            ASSERT(list->last != NULL, "Null pointer detected for last list element");

            if (list->last != NULL)
            {
                list->last->next = newElement;
                list->last = newElement;
            }
        }
    }
}

ListElement* createAndPrependToList(List* list, size_t priority)
{
    ListElement* element = NULL;

    if (list != NULL)
    {
        element = list->elementsPool != NULL ? aquireElement(list->elementsPool) : (ListElement*)malloc(sizeof(ListElement));

        if (element != NULL)
        {
            element->next = NULL;
            element->priority = priority;

            // empty object
            element->object.type = -1;
            element->object.payload = NULL;

            prependToList(list, element);
        }
    }

    return element;
}

void prependToList(List* list, ListElement* newElement)
{
    if (list != NULL && newElement != NULL)
    {
        ASSERT(newElement->next == NULL, "Element to be inserted is linked to another element!");

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
    ListElement* result = NULL;
    ASSERT(it.list != NULL, "Iterator points to NULL list");

    if (it.list != NULL)
    {
        ListElement* const nextElement = it.list->elementsPool != NULL ? aquireElement(it.list->elementsPool) : createListElement();

        if (nextElement != NULL)
        {
            result = nextElement;
            nextElement->priority = priority;
            insertAfter(it, nextElement);
        }
    }

    return result;
}

void insertAfter(ListIterator it, ListElement* nextElement)
{
    ASSERT(it.list != NULL, "Iterator points to NULL list");

    if (it.list != NULL && nextElement != NULL)
    {
        ASSERT(nextElement->next == NULL, "Element to be inserted is linked to another element!");

        if (it.current != NULL)
        {
            ASSERT(it.list->last != NULL, "Null pointer detected for last list element");

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
            ASSERT(it.list->last == NULL, "Non-null pointer detected for last list element");

            it.list->first = nextElement;
            it.list->last = nextElement;
        }
    }
}

ListElement* createAndInsertBefore(ListIterator it, size_t priority)
{
    ListElement* result = NULL;
    ASSERT(it.list != NULL, "Iterator points to NULL list");

    if (it.list != NULL)
    {
        ListElement* const previousElement = it.list->elementsPool != NULL ? aquireElement(it.list->elementsPool) : createListElement();

        if (previousElement != NULL)
        {
            previousElement->priority = priority;
            result = previousElement;
            insertBefore(it, previousElement);
        }
    }

    return result;
}

void insertBefore(ListIterator it, ListElement* previousElement)
{
    ASSERT(it.list != NULL, "Iterator points to NULL list");

    if (it.list != NULL && previousElement != NULL)
    {
        ASSERT(previousElement->next == NULL, "Element to be inserted is linked to another element!");

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
            ASSERT(list->last != NULL, "Null pointer detected for last list element");

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
        ASSERT(list->last != NULL, "Null pointer detected for last list element");

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
    ListElement* result = NULL;

    ASSERT(it.list != NULL, "Iterator points to NULL list");

    if (it.list != NULL && it.list->first != NULL && it.current != it.list->first)
    {
        ASSERT(it.list->last != NULL, "Null pointer detected for last list element");

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
    ListElement* result = NULL;

    ASSERT(it.list != NULL, "Iterator points to NULL list");

    if (it.list != NULL && it.current != NULL && it.current->next != NULL)
    {
        ASSERT(it.list->last != NULL, "Null pointer detected for last list element");

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
    ASSERT(it.list != NULL, "Iterator points to NULL list");

    ListElement* result = NULL;

    if (it.current != NULL)
    {
        lnext(&it);
        result = removePreviousListElement(it);
    }

    return result;
}

// user is responsible for de-allocating the Object of each element prior to erasing elements from the list
void clearList(List* list, void (*deallocObject)(Object* object))
{
    if (list != NULL)
    {
        ListElement* currentElement = list->first;
        list->first = NULL;
        list->last = NULL;

        if (list->elementsPool != NULL)
        {
            while (currentElement != NULL)
            {
                deallocObject(&currentElement->object);
                ListElement* elementToDelete = currentElement;
                currentElement = currentElement->next;
                const bool released = releaseElement(elementToDelete, list->elementsPool);
                ASSERT(released, "Element not contained in elementsPool!");
                elementToDelete = NULL;
            }
        }
        else
        {
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
}

// not to be used for heap created lists unless the references to first and last have been previously stored (memory leaks might occur)
void detachListElements(List* list)
{
    list->first = NULL;
    list->last = NULL;
}

void swapListElements(ListIterator firstIt, ListIterator secondIt)
{
    ASSERT(firstIt.list != NULL && secondIt.list != NULL, "At least one iterator points to a NULL list");
    ASSERT(firstIt.list == secondIt.list, "Iterators belong to different lists");

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
        ASSERT(list->last != NULL, "Null pointer detected for last list element");

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
        ASSERT(source->last != NULL, "Null pointer detected for last source list element");

        if (destination->last != NULL)
        {
            ASSERT(destination->first != NULL, "Null pointer detected for first destination list element");
            List* temp = createEmptyList(destination->elementsPool);
            bool unsuccessfulElementAllocationOccurred = false;

            if (temp != NULL)
            {
                ListElement* currentSourceElement = source->first;

                while (currentSourceElement != NULL && !unsuccessfulElementAllocationOccurred)
                {
                    ListElement* elementToAppend = createAndAppendToList(temp, currentSourceElement->priority);
                    unsuccessfulElementAllocationOccurred = elementToAppend == NULL;
                    elementToAppend->object.type = currentSourceElement->object.type;
                    elementToAppend->object.payload = currentSourceElement->object.payload;
                    currentSourceElement = currentSourceElement->next;
                }
            }

            if (!unsuccessfulElementAllocationOccurred)
            {
                destination->last->next = temp->first;
                destination->last = temp->last;
                temp->first = NULL;
                temp->last = NULL;
                free(temp);
                temp = NULL;

                clearListWithoutObjectsDeallocation(source); // reference of elements Object data (type + payload) kept by destination list
            }
            else
            {
                clearListWithoutObjectsDeallocation(temp); // reference of elements Object data (type + payload) kept by source list

                free(temp);
                temp = NULL;
            }
        }
        else
        {
            destination->first = source->first;
            destination->elementsPool = source->elementsPool;
            destination->last = source->last;
            source->first = NULL;
            source->last = NULL;
        }
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
        ASSERT(source->last != NULL, "Null pointer detected for last source list element");

        List* temp = createEmptyList(destination->elementsPool);

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

                if (destination->last != NULL)
                {
                    destination->last->next = temp->first;
                }
                else
                {
                    destination->first = temp->first;
                }

                destination->last = temp->last;

                temp->first = NULL;
                temp->last = NULL;
                free(temp);
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
        ASSERT(list->last != NULL, "Null pointer detected for last list element");

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
        ASSERT(list->first == NULL && list->last == NULL, "Attempt to move pointers array to unempty list");
        ASSERT(*array != NULL, "NULL value array element identified");

        if (*array != NULL)
        {
            ListElement** currentArrayElement = array;
            list->first = *currentArrayElement;
            ListElement* currentListElement = list->first;

            for (size_t index = 1; index < arraySize; ++index)
            {
                currentListElement->next = array[index];

                if (array[index] == NULL)
                {
                    ASSERT(false, "NULL value array element identified");
                    break;
                }

                currentListElement = currentListElement->next;
            }

            // ensure the list is correctly closed and the array loses ownership of the elements
            list->last = currentListElement;
            array[arraySize - 1]->next = NULL;

            for (size_t index = 0; index < arraySize; ++index)
            {
                array[index] = NULL;
            }
        }
    }
}

size_t getListSize(const List* list)
{
    size_t length = 0;

    if (!isEmptyList(list))
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

bool isEmptyList(const List* list)
{
    bool isEmpty = true;

    if (list != NULL)
    {
        isEmpty = list->first == NULL;
        ASSERT((isEmpty && list->last == NULL) || (!isEmpty && list->last != NULL), "Last element should be NULL for an empty list and NOT NULL for a unempty list");
    }

    return isEmpty;
}

ListElement* getListElementAtIndex(const List* list, size_t index)
{
    ListElement* result = NULL;

    if (list != NULL)
    {
        if (index < getListSize(list))
        {
            ListElement* currentElement = list->first;

            for (size_t currentIndex = 0; currentIndex < index; ++currentIndex)
            {
                currentElement = currentElement->next;
            }

            result = currentElement;
        }
        else
        {
            ASSERT(false, "The index is out of bounds!");
        }
    }

    return result;
}

ListElement* getPreviousListElement(ListIterator it)
{
    ListElement* result = NULL;

    ASSERT(it.list != NULL, "Iterator points to NULL list");

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

ListElement* getFirstListElement(const List* list)
{
    ListElement* result = NULL;

    if (list != NULL)
    {
        result = list->first;

        ASSERT((result != NULL && list->last != NULL) || (result == NULL && list->last == NULL),
               "Both first and last element should either be NULL or NOT NULL!");
    }

    return result;
}

ListElement* getLastListElement(const List* list)
{
    ListElement* result = NULL;

    if (list != NULL)
    {
        result = list->last;

        ASSERT((result != NULL && list->first != NULL) || (result == NULL && list->first == NULL),
               "Both first and last element should either be NULL or NOT NULL!");
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
    ListIterator result;

    result.list = NULL;
    result.current = NULL;

    ASSERT(list != NULL, "Attempt to get iterator from NULL list");

    if (list != NULL)
    {
        result.list = list;
        result.current = list->first;
    }

    return result;
}

ListIterator lend(List* list)
{
    ListIterator result;

    result.list = NULL;
    result.current = NULL;

    ASSERT(list != NULL, "Attempt to get iterator from NULL list");

    if (list != NULL)
    {
        result.list = list;
    }

    return result;
}

void lnext(ListIterator* iterator)
{
    ASSERT(iterator != NULL, "Attempt to advance a NULL iterator");
    ASSERT(iterator->list != NULL, "Iterator points to NULL list");

    if (iterator != NULL && iterator->current != NULL)
    {
        iterator->current = iterator->current->next;
    }
}

bool areIteratorsEqual(ListIterator first, ListIterator second)
{
    ASSERT(first.list != NULL && second.list != NULL, "At least one iterator points to a NULL list");
    ASSERT(first.list == second.list, "Iterators belong to different lists");

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

            fputs(header, outputFile);

            if (isEmptyList(list))
            {
                fputs("EMPTY LIST", outputFile);
            }

            while (currentElement != NULL)
            {
                char stringToDisplay[12];

                convertIntToString((int)elementIndex, stringToDisplay, sizeof(stringToDisplay));
                fputs("Element: ", outputFile);
                fputs(stringToDisplay, outputFile);
                fputs("\t", outputFile);

                convertIntToString((int)currentElement->priority, stringToDisplay, sizeof(stringToDisplay));
                fputs("Priority: ", outputFile);
                fputs(stringToDisplay, outputFile);
                fputs("\t", outputFile);

                fputs("Has empty Object: ", outputFile);

                if (currentElement->object.type >= 0)
                {
                    fputs("no\t", outputFile);
                    fputs("Object type: ", outputFile);
                    fputs(getTestObjectTypeAsString(currentElement->object.type), outputFile);
                }
                else
                {
                    fputs("yes", outputFile);
                }

                fputs("\n", outputFile);

                currentElement = currentElement->next;
                ++elementIndex;
            }

            fclose(outputFile);
            outputFile = NULL;
        }
        else
        {
            errorNumber = errno;
            printf("A file opening error occurred!\n");
            printf("Error number: %d\n", errno);
            printf("Error description: %s\n", strerror(errorNumber));
        }
    }
}

static void clearListWithoutObjectsDeallocation(List* list)
{
    ASSERT(list != NULL, "Invalid list (NULL)!");

    if (list != NULL)
    {
        ListElement* currentElementToDelete = list->first;

        while (currentElementToDelete != NULL)
        {
            ListElement* elementToDelete = currentElementToDelete;
            currentElementToDelete = currentElementToDelete->next;
            elementToDelete->next = NULL;

            if (list->elementsPool != NULL)
            {
                releaseElement(elementToDelete, list->elementsPool);
            }
            else
            {
                free(elementToDelete);
            }

            elementToDelete = NULL;
        }

        list->first = NULL;
        list->last = NULL;
    }
}
