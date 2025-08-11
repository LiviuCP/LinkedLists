#include <stdio.h>
#include <string.h>

#include "priorityqueue.h"
#include "../LinkedListsLib/linkedlist.h"
#include "../Utils/error.h"

PriorityQueue* createPriorityQueue()
{
    PriorityQueue* result = NULL;
    PriorityQueue* queue = (PriorityQueue*)malloc(sizeof(PriorityQueue));

    if (queue != NULL)
    {
        List* queueContainer = createEmptyList();

        if (queueContainer != NULL)
        {
            queue->container = (void*)queueContainer;
            result = queue;
        }
        else
        {
            free(queue);
            queue = NULL;
        }
    }

    return result;
}

void deletePriorityQueue(PriorityQueue* queue, void (*deallocObject)(Object* object))
{
    if (queue != NULL)
    {
        deleteList((List*)queue->container, deallocObject);
        queue->container = NULL;
        free(queue);
        queue = NULL;
    }
}

bool insertIntoPriorityQueue(PriorityQueue* queue, const size_t priority, const int objectType, void* const objectPayload)
{
    bool success = false;

    if (queue != NULL && objectPayload != NULL)
    {
        ASSERT(objectType >= 0, "Attempt to insert an invalid object");

        ListElement* newElement = NULL;

        if (queue->container != NULL)
        {
            newElement = createListElement();
        }
        else
        {
            ASSERT(false, "NULL queue container detected");
        }

        if (newElement != NULL)
        {
            newElement->priority = priority;
            newElement->object.type = objectType;
            newElement->object.payload = objectPayload;

            ListElement* currentElement = getFirstListElement((List*)queue->container);

            if (currentElement == NULL)
            {
                appendToList((List*)queue->container, newElement);
            }
            else if (currentElement->priority < newElement->priority)
            {
                prependToList((List*)queue->container, newElement);
            }
            else
            {
                while (currentElement->next != NULL)
                {
                    if (currentElement->next->priority >= newElement->priority)
                    {
                        currentElement = currentElement->next;
                    }
                    else
                    {
                        newElement->next = currentElement->next;
                        currentElement->next = newElement;
                        success = true;
                        break;
                    }
                }
                if (!success) // append to list if all elements have lower priority
                {
                    appendToList((List*)queue->container, newElement);
                    success = true;
                }
            }
        }
    }

    return success;
}


Object* removeFromPriorityQueue(PriorityQueue* queue)
{
    Object* result = NULL;

    if (queue != NULL)
    {
        ListElement* removedElement = NULL;

        if (queue->container != NULL)
        {
            removedElement = removeFirstListElement((List*)(queue->container));
        }
        else
        {
            ASSERT(false, "NULL queue container detected");
        }

        if (removedElement != NULL)
        {
            Object* newObject = NULL;

            if (removedElement->object.type >= 0 && removedElement->object.payload != NULL)
            {
                newObject = createObject(removedElement->object.type, removedElement->object.payload);
            }
            else
            {
                ASSERT(false, "Invalid or empty queue element object detected");
            }

            // move object type and payload into a new object as the current one is deallocated together with the list element
            if (newObject != NULL)
            {
                result = newObject;
                removedElement->object.type = -1;
                removedElement->object.payload = NULL;
                free(removedElement);
                removedElement = NULL;
            }
        }
    }

    return result;
}

void clearPriorityQueue(PriorityQueue* queue, void (*deallocObject)(Object* object))
{
    if (queue != NULL)
    {
        if (queue->container != NULL)
        {
            clearList((List*)queue->container, deallocObject);
        }
        else
        {
            ASSERT(false, "NULL queue container detected");
        }
    }
}

bool isEmptyQueue(const PriorityQueue* queue)
{
    bool result = false;

    if (queue != NULL)
    {
        ASSERT(queue->container != NULL, "NULL queue container detected");

        if (queue->container != NULL)
        {
            result = isEmptyList((List*)queue->container);
        }
    }

    return result;
}

PriorityQueueIterator pqbegin(const PriorityQueue* queue)
{
    PriorityQueueIterator result;
    result.queueItem = NULL;

    if (queue != NULL)
    {
        if (queue->container != NULL)
        {
            List* list = (List*)(queue->container);
            result.queueItem = getFirstListElement(list);
            list = NULL;
        }
        else
        {
            ASSERT(false, "NULL queue container detected");
        }
    }

    return result;
}

void pqnext(PriorityQueueIterator* it)
{
    if (it != NULL && it->queueItem != NULL)
    {
        it->queueItem = ((ListElement*)(it->queueItem))->next;
    }
}

Object* getPriorityQueueObject(PriorityQueueIterator it)
{
    Object* object = NULL;

    if (it.queueItem != NULL)
    {
        object = &(((ListElement*)(it.queueItem))->object);
    }
    else
    {
        ASSERT(false, "Attempt to access a NULL priority queue item");
    }

    // every queue object must be valid; we must ensure this throughout the usage period of the queue (user can modify objects by using the priority queue iterator)
    ASSERT(object != NULL && object->payload != NULL, "Invalid queue element object detected");

    return object;
}

size_t getObjectPriority(PriorityQueueIterator it)
{
    Object* object = NULL;

    if (it.queueItem != NULL)
    {
        object = &(((ListElement*)(it.queueItem))->object);
    }
    else
    {
        ASSERT(false, "Attempt to access a NULL priority queue item");
    }

    size_t priority = 0;

    // we must still ensure the object is valid even if only the priority is requested
    if (object != NULL && object->payload != NULL)
    {
        priority = ((ListElement*)(it.queueItem))->priority;
    }
    else
    {
        ASSERT(false, "Invalid queue element object detected");
    }

    return priority;
}
