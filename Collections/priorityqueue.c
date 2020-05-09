#include <string.h>

#include "priorityqueue.h"
#include "../LinkedListsLib/linkedlist.h"
#include "../Utils/codeutils.h"
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

boolean insertIntoPriorityQueue(PriorityQueue* queue, const size_t priority, Object* object)
{
    boolean success = FALSE;

    if (queue != NULL && object != NULL && object->type != NULL && strlen(object->type) != 0 && object->payload != NULL)
    {
        ASSERT_CONDITION(queue->container != NULL, "NULL queue container detected");

        ListElement* newElement = createListElement();

        if (newElement != NULL)
        {
            newElement->priority = priority;
            newElement->object = object;

            ListElement* currentElement = ((List*)(queue->container))->first;

            if (currentElement == NULL)
            {
                ((List*)(queue->container))->first = newElement;
            }
            else if (currentElement->priority < newElement->priority)
            {
                ((List*)(queue->container))->first = newElement;
                newElement->next = currentElement;
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
                        success = TRUE;
                        break;
                    }
                }
                if (!success) // append to list if all elements have lower priority
                {
                    currentElement->next = newElement;
                    success = TRUE;
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
        ASSERT_CONDITION(queue->container != NULL, "NULL queue container detected")
        ListElement* removedElement = removeFirstListElement((List*)(queue->container));

        if (removedElement != NULL)
        {
            ASSERT_CONDITION(removedElement->object != NULL &&
                             removedElement->object->type != NULL &&
                             strlen(removedElement->object->type) != 0 &&
                             removedElement->object->payload != NULL,     "Invalid queue element object detected")

            result = removedElement->object;
            free(removedElement);
            removedElement = NULL;
        }
    }

    return result;
}

void clearPriorityQueue(PriorityQueue* queue, void (*deallocObject)(Object* object))
{
    if (queue != NULL)
    {
        ASSERT_CONDITION(queue->container != NULL, "NULL queue container detected");
        clearList((List*)queue->container, deallocObject);
    }
}

boolean isEmptyQueue(const PriorityQueue* queue)
{
    boolean result = FALSE;

    if (queue != NULL)
    {
        ASSERT_CONDITION(queue->container != NULL, "NULL queue container detected");
        result = (((List*)(queue->container))->first == NULL);
    }

    return result;
}

PriorityQueueIterator pqbegin(const PriorityQueue* queue)
{
    PriorityQueueIterator result;
    result.queueItem = NULL;

    if (queue != NULL)
    {
        ASSERT_CONDITION(queue->container != NULL, "NULL queue container detected")

        List* list = (List*)(queue->container);
        result.queueItem = (void*)(list->first);
        list = NULL;
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
    ASSERT_CONDITION(it.queueItem != NULL, "Attempt to access a NULL priority queue item")

    Object* object = ((ListElement*)(it.queueItem))->object;

    // every queue object must be valid; we must ensure this throughout the usage period of the queue (user can modify objects by using the priority queue iterator)
    ASSERT_CONDITION(object != NULL &&
                     object->type != NULL &&
                     strlen(object->type) != 0 &&
                     object->payload != NULL,     "Invalid queue element object detected")

    return object;
}

size_t getObjectPriority(PriorityQueueIterator it)
{
    ASSERT_CONDITION(it.queueItem != NULL, "Attempt to access a NULL priority queue item")

    Object* object = ((ListElement*)(it.queueItem))->object;

    // we must still ensure the object is valid even if only the priority is requested
    ASSERT_CONDITION(object != NULL &&
                     object->type != NULL &&
                     strlen(object->type) != 0 &&
                     object->payload != NULL,     "Invalid queue element object detected")

    return ((ListElement*)(it.queueItem))->priority;
}
