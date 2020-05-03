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

boolean pushToStack(Stack* stack, Object* object);
Object* popFromStack(Stack* stack);

boolean isEmptyStack(const Stack* stack);

#ifdef __cplusplus
}
#endif

#endif // STACK_H
