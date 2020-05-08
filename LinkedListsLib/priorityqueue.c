#include <string.h>

#include "priorityqueue.h"
#include "linkedlist.h"
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
