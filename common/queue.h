#ifndef QUEUE_H
#define QUEUE_H

#include "int.h"

typedef struct {
    // Base allocation & size
    // TODO: Consider making this structure more generic
    u64* data;
    u32 alloc_size;

    u32 front_idx;
    u32 back_idx;
}queue;

// Create a queue. init_size is the initial allocation (in bytes)
queue queue_create(u32 init_size);

// Add an element to the back of the queue.
void queue_add(queue* q, u64 val);

// Remove the element at the front of the queue, and return it.
u64 queue_get(queue* q);

// Reset internal state and fill buffer with NULL. Does not free buffer.
void queue_clear(queue* q);

bool queue_empty(queue* q);

#endif // #ifndef QUEUE_H
