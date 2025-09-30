#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include "codeutils.h"

typedef struct
{
    void* container;
    void* data; // to be used for PriorityQueue deletion only

} PriorityQueue;

typedef struct
{
    void* queueItem;
} PriorityQueueIterator;

#ifdef __cplusplus
extern "C"{
#endif

PriorityQueue* createPriorityQueue(void* elementsPool);
void deletePriorityQueue(PriorityQueue* queue, void (*deallocObject)(Object* object));

bool insertIntoPriorityQueue(PriorityQueue* queue, const size_t priority, const int objectType, void* const objectPayload);
Object* removeFromPriorityQueue(PriorityQueue* queue);

void clearPriorityQueue(PriorityQueue* queue, void (*deallocObject)(Object* object));

bool isEmptyQueue(const PriorityQueue* queue);

PriorityQueueIterator pqbegin(const PriorityQueue* queue);
void pqnext(PriorityQueueIterator *it);
Object* getPriorityQueueObject(PriorityQueueIterator it);
size_t getObjectPriority(PriorityQueueIterator it);

#ifdef __cplusplus
}
#endif

#endif // PRIORITYQUEUE_H
