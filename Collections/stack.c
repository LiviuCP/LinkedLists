#include <string.h>

#include "stack.h"
#include "../LinkedListsLib/linkedlist.h"

#include "../Utils/error.h"

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

boolean pushToStack(Stack* stack, const int objectType, void* const objectPayload)
{
    boolean success = FALSE;

    if (stack != NULL && objectPayload != NULL)
    {
        ASSERT_CONDITION(objectType >= 0, "Attempt to push invalid object")
        ASSERT_CONDITION(stack->container != NULL, "NULL stack container detected")

        ListElement* newElement = createAndPrependToList((List*)(stack->container), 0); // all stack elements have priority 0 (priority is not relevant here)

        if (newElement != NULL)
        {
            newElement->object.type = objectType;
            newElement->object.payload = objectPayload;
        }
    }

    return success;
}

Object* popFromStack(Stack* stack)
{
    Object* result = NULL;

    if (stack != NULL)
    {
        ASSERT_CONDITION(stack->container != NULL, "NULL stack container detected")
        ListElement* topStackElement = removeFirstListElement((List*)(stack->container));

        if (topStackElement != NULL)
        {
            ASSERT_CONDITION(topStackElement->object.type >= 0 && topStackElement->object.payload != NULL, "Invalid or empty stack element object detected")

            Object* newObject = createObject(topStackElement->object.type, topStackElement->object.payload);

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
        ASSERT_CONDITION(stack->container != NULL, "NULL stack container detected")
        clearList((List*)stack->container, deallocObject);
    }
}

boolean isEmptyStack(const Stack* stack)
{
    boolean result = FALSE;

    if (stack != NULL)
    {
        ASSERT_CONDITION(stack->container != NULL, "NULL stack container detected")
        result = (((List*)(stack->container))->first == NULL);
    }

    return result;
}
