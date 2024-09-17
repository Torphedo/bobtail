#ifndef QUEUE_H
#define QUEUE_H
/// @file queue.h
/// @brief an auto-expanding dynamic queue implementation

#include "int.h"

// TODO: Make this structure generic like the dynamic list.

/// The underlying data type used for the dynamic queue
typedef u64 queue_element;

/// @brief An automatically expanding dynamic queue
///
/// Unlike the dynamic @ref list, this structure isn't really generic yet. Its
/// type can be changed by editing the typedef of @ref queue_element at compile
/// time, but there's no way to have multiple queues of multiple different
/// types in one program.
typedef struct {
    /// @brief Backing buffer
    queue_element* data;
    /// Current buffer size
    u32 alloc_size;

    /// Index of the front of the queue
    u32 front_idx;
    /// Index of the back of the queue
    u32 back_idx;
}queue;

/// @brief Create a queue.
/// @param init_size Initial allocation size in bytes
/// @note This allocates memory!
queue queue_create(u32 init_size);

/// @brief Add an element to the back of the queue.
/// @note If the backing buffer is full, this can allocate memory
void queue_add(queue* q, queue_element val);

/// Remove the element at the front of the queue, and return it.
queue_element queue_get(queue* q);

/// Reset internal state and fill backing buffer with 0. Does not free buffer.
void queue_clear(queue* q);

/// Check whether the queue is empty
bool queue_empty(queue q);

#endif // #ifndef QUEUE_H
