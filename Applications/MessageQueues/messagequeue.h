#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <stdlib.h>

// these are required for calculating the queue id
#define QUEUE_FILENAME "/tmp/createqueue"
#define BASE_ID 543

typedef struct
{
    long priorityType; // convention: 1 - low (one digit), 2 - medium (two digits), 3 - high (three digits)
    size_t priority;
} QueueMessage;

#endif // MESSAGEQUEUE_H
