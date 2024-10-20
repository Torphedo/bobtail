#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "logging.h"
#include "queue.h"

s32 queue_maxidx(queue q) {
    return (q.alloc_size / sizeof(*q.data)) - 1;
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

bool queue_contains(queue* q, queue_element val) {
    for (u32 i = q->front_idx; i < q->back_idx + 1; i++) {
        if (q->data[i] == val) {
            return true;
        }
    }

    return false;
}

queue queue_create(u32 init_size) {
    return (queue) {
        .data = calloc(1, init_size),
        .alloc_size = init_size,
        .front_idx = 0,
        .back_idx = 0
    };
}

void queue_add(queue* q, queue_element val) {
    // If there's no room in the back
    if (queue_fullback(*q)) {
        if (!queue_fullfront(*q) && q->data != NULL) {
            // We have open space at the front, so it can be transparently
            // reclaimed to make room for the new value.
            const u32 size = (q->back_idx - q->front_idx) * sizeof(*q->data);
            memmove(q->data, &q->data[q->front_idx], size);
            // Zero out the now unused memory
            memset(((u8*)q->data) + size, 0x00, q->alloc_size - size);

            // The old front idx is also the number of open spaces we just
            // re-used. So the new back idx is just moved back by that amount.
            q->back_idx -= q->front_idx;
            q->front_idx = 0;
        }
        else {
            // The buffer is completely full & needs a new allocation.
            // Grow by 50%, rounded up to the next multiple of our data size.
            const u32 newsize = ALIGN_UP((u32)(q->alloc_size * 1.5), sizeof(*q->data));
            void* newbuf = calloc(1, newsize);
            if (newbuf == NULL) {
                LOG_MSG(error, "Couldn't expand queue 0x%X -> 0x%X [alloc failure]\n", q->alloc_size, newsize);
                return;
            }

            // Copy data & update state
            memcpy(newbuf, q->data, q->alloc_size);
            free(q->data);
            q->data = newbuf;
            q->alloc_size = newsize;
        }
    }

    // Put value in the next slot
    q->data[q->back_idx++] = val;
}

queue_element queue_get(queue* q) {
    // We can't get a value if there's not enough buffer space for the read
    if (q->alloc_size < sizeof(queue_element) || q->data == NULL) {
        return 0;
    }

    const queue_element val = q->data[q->front_idx++];

    // If this was the last item, reset so the entire buffer can be re-used.
    if (q->front_idx == q->back_idx) {
        q->front_idx = 0;
        q->back_idx = 0;
    }
    return val;
}

void queue_clear(queue* q) {
    memset(q->data, 0x00, q->alloc_size);
    q->front_idx = 0;
    q->back_idx = 0;
}

