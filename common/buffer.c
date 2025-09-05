#include <string.h>
#include <stdlib.h>

#include "logging.h"
#include "buffer.h"

buffer_t buf_create(u64 init_size, bool wipe_on_alloc) {
    return (buffer_t) {
        .alloc_size = init_size,
        .data = wipe_on_alloc ? calloc(1, init_size) : malloc(init_size),
    };
}

void buf_destroy(buffer_t* buf) {
    free(buf->data);
    *buf = (buffer_t){0};
}

/// Fill buffer contents with 0. Does not free buffer.
void buf_clear(buffer_t* buf) {
    memset(buf->data, 0, buf->alloc_size);
}

bool buf_empty(buffer_t buf) {
    return buf.alloc_size == 0 || buf.data == NULL;
}

bool buf_resize(buffer_t* buf, u64 new_size, bool wipe_on_alloc) {
    if (new_size == buf->alloc_size) {
        return true;
    }

    buffer_t new_buf = buf_create(new_size, wipe_on_alloc);
    if (!new_buf.data) {
        LOG_MSG(error, "Failed to resize buffer 0x%x -> 0x%x bytes\n", buf->alloc_size, new_size);
        return false;
    }

    // Copy as much as possible
    memcpy(new_buf.data, buf->data, MIN(new_size, buf->alloc_size));

    buf_destroy(buf);
    *buf = new_buf;

    return true;
}
