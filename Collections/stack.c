#include "stack.h"
#include "../LinkedListsLib/linkedlist.h"

#include "../Utils/error.h"

#include <stdio.h>

Stack* createStack()
{
    Stack* result = NULL;
    Stack* stack = (Stack*)malloc(sizeof(Stack));

    if (stack != NULL)
    {
        List* stackContainer = createEmptyList();

        if (stackContainer != NULL)
        {
            stack->container = (void*)stackContainer;
            result = stack;
        }
        else
        {
            free(stack);
            stack = NULL;
        }
    }

    return result;
}

void deleteStack(Stack* stack, void (*deallocObject)(Object* object))
{
    if (stack != NULL)
    {
        deleteList((List*)stack->container, deallocObject);
        stack->container = NULL;
        free(stack);
        stack = NULL;
    }
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

        ASSERT(stack->container != NULL, "NULL stack container detected");

        if (stack->container != NULL)
        {
            topStackElement = removeFirstListElement((List*)(stack->container));
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
                free(topStackElement);
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
