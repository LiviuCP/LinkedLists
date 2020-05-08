#include <string.h>

#include "stack.h"
#include "linkedlist.h"

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

boolean pushToStack(Stack* stack, Object* object)
{
    boolean success = FALSE;

    if (stack != NULL && object != NULL && object->type != NULL && strlen(object->type) != 0 && object->payload != NULL)
    {
        ASSERT_CONDITION(stack->container != NULL, "NULL stack container detected")

        ListElement* newElement = createAndPrependToList((List*)(stack->container), 0); // all stack elements have priority 0 (priority is not relevant here)

        if (newElement != NULL)
        {
            newElement->object = object;
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
            ASSERT_CONDITION(topStackElement->object != NULL &&
                             topStackElement->object->type != NULL &&
                             strlen(topStackElement->object->type) != 0 &&
                             topStackElement->object->payload != NULL,     "Invalid stack element object detected")

            result = topStackElement->object;
            free(topStackElement);
            topStackElement = NULL;
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
