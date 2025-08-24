#ifndef STACK_H
#define STACK_H

#include "../LinkedListsLib/listelement.h"

typedef struct
{
    void* container;
} Stack;

#ifdef __cplusplus
extern "C"{
#endif

Stack* createStack(ListElementsPool* elementsPool);
void deleteStack(Stack* stack, void (*deallocObject)(Object* object));

bool pushToStack(Stack* stack, const int objectType, void* const objectPayload);
Object* popFromStack(Stack* stack);

void clearStack(Stack* stack, void (*deallocObject)(Object* object));

bool isEmptyStack(const Stack* stack);

#ifdef __cplusplus
}
#endif

#endif // STACK_H
