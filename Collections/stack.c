#include "stack.h"

#include "../LinkedListsLib/linkedlist.h"
#include "../Utils/error.h"

#include <stdio.h>

#define STACK_OFFSET 4

Stack* createStack(ListElementsPool* elementsPool)
{
    Stack* stack = NULL;

    // the offset bytes are required in order to prevent de-allocating data by deleting pointer to the first category (stack object)
    void* data = malloc(STACK_OFFSET + sizeof(Stack) + sizeof(List));

    if (data)
    {
        stack = (Stack*)(data + STACK_OFFSET);

        List* stackContainer = (List*)(stack + 1);
        stackContainer->first = NULL;
        stackContainer->last = NULL;
        stackContainer->elementsPool = elementsPool;

        stack->container = stackContainer;
        stack->data = data;
    }

    return stack;
}

void deleteStack(Stack* stack, void (*deallocObject)(Object* object))
{
    void* data = stack != NULL ? stack->data : NULL;
    List* stackContainer = stack != NULL ? (List*)stack->container : NULL;

    ASSERT(stack == NULL || data != NULL && stackContainer != NULL, "Invalid stack!");

    if (stackContainer != NULL)
    {
        clearList(stackContainer, deallocObject);
        stackContainer = NULL;
    }

    FREE(data);
}

bool pushToStack(Stack* stack, const int objectType, void* const objectPayload)
{
    bool success = false;

    if (stack != NULL && objectPayload != NULL)
    {
        ASSERT(objectType >= 0, "Attempt to push invalid object");
        ASSERT(stack->container != NULL, "NULL stack container detected");

        if (objectType >= 0 && stack->container != NULL)
        {
            ListElement* newElement = createAndPrependToList((List*)(stack->container), 0); // all stack elements have priority 0 (priority is not relevant here)

            if (newElement != NULL)
            {
                newElement->object.type = objectType;
                newElement->object.payload = objectPayload;
            }
        }
    }

    return success;
}

Object* popFromStack(Stack* stack)
{
    Object* result = NULL;

    if (stack != NULL)
    {
        ListElement* topStackElement = NULL;
        List* stackContainer = (List*)stack->container;

        ASSERT(stackContainer != NULL, "NULL stack container detected");

        if (stackContainer != NULL)
        {
            topStackElement = removeFirstListElement(stackContainer);
        }

        if (topStackElement != NULL)
        {
            Object* newObject = NULL;

            if (topStackElement->object.type >= 0 && topStackElement->object.payload != NULL)
            {
                newObject = createObject(topStackElement->object.type, topStackElement->object.payload);
            }
            else
            {
                ASSERT(false, "Invalid or empty stack element object detected");
            }

            if (newObject != NULL)
            {
                *newObject = topStackElement->object;
                result = newObject;

                if (stackContainer->elementsPool != NULL)
                {
                    releaseElement(topStackElement, stackContainer->elementsPool);
                }
                else
                {
                    free(topStackElement);
                }

                topStackElement = NULL;
            }
        }
    }

    return result;
}

void clearStack(Stack* stack, void (*deallocObject)(Object* object))
{
    if (stack != NULL)
    {
        ASSERT(stack->container != NULL, "NULL stack container detected");

        if (stack->container != NULL)
        {
            clearList((List*)stack->container, deallocObject);
        }
    }
}

bool isEmptyStack(const Stack* stack)
{
    bool result = false;

    if (stack != NULL)
    {
        ASSERT(stack->container != NULL, "NULL stack container detected");

        if (stack->container != NULL)
        {
            result = isEmptyList((List*)stack->container);
        }
    }

    return result;
}
