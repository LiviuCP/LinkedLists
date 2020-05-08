#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include "../Utils/codeutils.h"

typedef struct
{
    void* container;

} PriorityQueue;

#ifdef __cplusplus
extern "C"{
#endif

PriorityQueue* createPriorityQueue();
void deletePriorityQueue(PriorityQueue* queue, void (*deallocObject)(Object* object));

boolean insertIntoPriorityQueue(PriorityQueue* queue, const size_t priority, Object* object);
Object* removeFromPriorityQueue(PriorityQueue* queue);

void clearPriorityQueue(PriorityQueue* queue, void (*deallocObject)(Object* object));

boolean isEmptyQueue(const PriorityQueue* queue);

#ifdef __cplusplus
}
#endif

#endif // PRIORITYQUEUE_H
