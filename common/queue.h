#ifndef QUEUE_H
#define QUEUE_H
#include "util.h"
EXTERN_C_BEGIN
/// @file queue.h
/// @brief An auto-expanding dynamic queue implementation
///
/// Unlike the dynamic @ref list, this structure isn't really generic yet. Its
/// type can be changed by editing the typedef of @ref queue_element at compile
/// time, but there's no way to have multiple queues of multiple different
/// types in one program.
///
/// @warning Don't keep pointers / indices to elements of the queue for any
/// longer than necessary! They are liable to point to different data or
/// freed/invalid memory if the queue is modified. Any function taking a
/// pointer to the queue can and will modify any part of it.
/// @sa list.h

#include <stdbool.h>
#include "int.h"
#include "buffer.h"

/// @brief An automatically expanding dynamic queue
///
/// @warning Don't keep pointers / indices to elements of the queue for any
/// longer than necessary! They are liable to point to different data or
/// freed/invalid memory if the queue is modified. Any function taking a
/// pointer to the queue can and will modify any part of it.
/// @sa list
typedef struct {
    /// @brief Backing buffer
    buffer_t buf;

    u16 element_size;

    /// Index of the front of the queue
    u32 front_idx;
    /// Index of the back of the queue
    u32 back_idx;
} queue;

/// @brief Create a queue.
/// @param init_size Initial allocation size in bytes
/// @note This allocates memory!
queue queue_create(u32 init_size, u16 element_size);

/// @brief Destroy / free a queue
///
/// @param q The queue to destroy
/// This will replace the backing buffer pointer with NULL, and fill the entire
/// contents of the buffer with zeroes.
void queue_destroy(queue* q);

/// @brief Add an element to the back of the queue.
/// @note If the backing buffer is full, this can allocate memory.
void queue_add(queue* q, const void* val);

/// Return the element at the front of the queue.
bool queue_peek(queue q, void* val_out);

/// Same as @ref queue_peek, but removes the front element.
bool queue_get(queue* q, void* val_out);

/// Reset internal state and fill backing buffer with 0. Does not free buffer.
void queue_clear(queue* q);

/// Check whether the queue is empty
bool queue_empty(queue q);

EXTERN_C_END
#endif // #ifndef QUEUE_H