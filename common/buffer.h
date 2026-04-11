#pragma once
#include "util.h"
EXTERN_C_BEGIN
/// @file buffer.h
/// @brief Resizable buffer
/// @warning Don't keep pointers to the contents of the buffer if you resize
/// it!
/// @sa list.h
/// @sa queue.h

#include <stddef.h>
#include <stdbool.h>
#include "int.h"

/// @brief A resizable buffer
typedef struct {
    void* data;

    /// Current buffer size
    u64 alloc_size;
}buffer_t;

/// Create a buffer.
/// @param init_size The allocation size in bytes.
/// @param wipe_on_alloc Whether to fill the buffer with 0 or leave it uninitialized.
///
/// @note This allocates memory!
/// @sa buffer_destroy
buffer_t buf_create(u64 init_size, bool wipe_on_alloc);

/// @brief Free buffer data & fill all fields with 0
/// @sa buffer_create
void buf_destroy(buffer_t* buf);

/// Fill buffer contents with 0. Does not free buffer.
void buf_clear(buffer_t* buf);

/// @brief Whether the buffer is empty
bool buf_empty(buffer_t buf);

/// @param new_size The allocation size in bytes.
/// @param wipe_on_alloc Whether to fill the buffer with 0 or leave it uninitialized.
bool buf_resize(buffer_t* buf, u64 new_size, bool wipe_on_alloc);

EXTERN_C_END