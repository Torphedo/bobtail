#include <string.h>
#include <stdbool.h>

#include "logging.h"
#include "queue.h"

u32 queue_maxidx(queue q) {
    const s32 num_elements = (s32)q.buf.alloc_size / q.element_size;
    return MAX(0, num_elements - 1);
}

bool queue_fullback(queue q) {
    return q.back_idx >= queue_maxidx(q) || queue_maxidx(q) < 0;
}

bool queue_fullfront(queue q) {
    return (q.front_idx == 0);
}

bool queue_empty(queue q) {
    return (q.front_idx == q.back_idx);
}

const void* queue_element_const(const queue* q, u16 idx) {
    return (const void*)((uintptr_t)q->buf.data + (idx * q->element_size));
}

void* queue_element(queue* q, u16 idx) {
    return (void*)((uintptr_t)q->buf.data + (idx * q->element_size));
}

bool queue_contains(const queue* q, void* val) {
    for (u32 i = q->front_idx; i < q->back_idx + 1; i++) {
        const void* element = queue_element_const(q, i);
        if (memcmp(element, val, q->element_size) == 0) {
            return true;
        }
    }

    return false;
}

queue queue_create(u32 init_size, u16 element_size) {
    return (queue) {
        .buf = buf_create(init_size * element_size, true),
        .element_size = element_size,
        .front_idx = 0,
        .back_idx = 0
    };
}

void queue_add(queue* q, const void* val) {
    // If there's no room in the back
    if (queue_fullback(*q)) {
        if (!queue_fullfront(*q) && q->buf.data != NULL) {
            // We have open space at the front, so it can be transparently
            // reclaimed to make room for the new value.
            const u32 size = (q->back_idx - q->front_idx) * q->element_size;
            memmove(q->buf.data, queue_element(q, q->front_idx), size);
            // Zero out the now unused memory
            memset(((u8*)q->buf.data) + size, 0x00, q->buf.alloc_size - size);

            // The old front idx is also the number of open spaces we just
            // re-used. So the new back idx is just moved back by that amount.
            q->back_idx -= q->front_idx;
            q->front_idx = 0;
        } else {
            // The buffer is completely full & needs a new allocation.
            // Grow by 50%, rounded up to the next multiple of our data size.
            const u32 newsize = q->buf.alloc_size * 1.5;
            if (!buf_resize(&q->buf, newsize, true)) {
                LOG_MSG(error, "Couldn't expand queue 0x%X -> 0x%X [alloc failure]\n", q->buf.alloc_size, newsize);
                return;
            }
        }
    }

    // Put value in the next slot
    void* dest = queue_element(q, q->back_idx++);
    memcpy(dest, val, q->element_size);
}

bool queue_peek(queue q, void* val_out) {
    if (queue_empty(q) || queue_maxidx(q) <= q.front_idx) {
        return false;
    }
    // We can't get a value if there's not enough buffer space for the read
    if (q.buf.alloc_size < q.element_size || q.buf.data == NULL) {
        return false;
    }

    memcpy(val_out, queue_element_const(&q, q.front_idx), q.element_size);
    return true;
}

bool queue_get(queue* q, void* val_out) {
    // Get top and increment idx
    if (!queue_peek(*q, val_out)) {
        return false;
    }
    q->front_idx++;

    // If this was the last item, reset so the entire buffer can be re-used.
    if (queue_empty(*q)) {
        q->front_idx = 0;
        q->back_idx = 0;
    }
    return true;
}

void queue_clear(queue* q) {
    buf_clear(&q->buf);
    q->front_idx = 0;
    q->back_idx = 0;
}

void queue_destroy(queue* q) {
    buf_destroy(&q->buf);
    *q = (queue){0};
}
