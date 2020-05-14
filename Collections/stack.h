#ifndef STACK_H
#define STACK_H

#include "../Utils/codeutils.h"

typedef struct
{
    void* container;
} Stack;

#ifdef __cplusplus
extern "C"{
#endif

Stack* createStack();
void deleteStack(Stack* stack, void (*deallocObject)(Object* object));

boolean pushToStack(Stack* stack, const int objectType, void* const objectPayload);
Object* popFromStack(Stack* stack);

void clearStack(Stack* stack, void (*deallocObject)(Object* object));

boolean isEmptyStack(const Stack* stack);

#ifdef __cplusplus
}
#endif

#endif // STACK_H
