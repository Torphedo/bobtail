#include <stdlib.h>

#include <common/logging.h>
#include <common/int.h>
#include <common/queue.h>

#include "testing.h"

bool test_queue() {
    bool result = true;

    // Test basic queue creation
    const u32 alloc_size = 2;
    queue q = queue_create(alloc_size, sizeof(u64));
    if (q.buf.data == NULL) {
        printf("CREATE: Initial alloc failed!\n");
        result = false;
    }
    if (q.buf.alloc_size != alloc_size * q.element_size) {
        printf("CREATE: Initial alloc has the wrong size!\n");
        result = false;
    }
    if (q.front_idx != 0) {
        printf("CREATE: Front index didn't start at 0!\n");
        result = false;
    }
    if (q.back_idx != 0) {
        printf("CREATE: Back index didn't start at 0!\n");
        result = false;
    }
    if (q.front_idx != q.back_idx) {
        printf("CREATE: Front & back index don't match!\n");
        result = false;
    }

    // Retreiving from an empty queue should fail
    u64 element = 0;
    queue_get(&q, &element);
    if (element != 0) {
        printf("GET: Queue allowed out-of-bounds read on empty queue!\n");
        result = false;
    }

    const u64 temp = 42;
    queue_add(&q, &temp);
    queue_add(&q, &temp);
    if (q.buf.alloc_size < sizeof(temp) && q.buf.data != NULL) {
        printf("ADD: Resizing doesn't ensure enough space for an element!\n");
        result = false;
    }
    if (q.back_idx != 2) {
        printf("ADD: Back index didn't move!\n");
        result = false;
    }

    u64 read_val = 0;
    queue_get(&q, &read_val);
    if (read_val != temp) {
        printf("GET: Failed to read element that should exist!\n");
        result = false;
    }
    if (q.front_idx == 0) {
        printf("GET: Didn't move front idx correctly!\n");
        result = false;
    }
    if (q.back_idx == 0) {
        printf("GET: Reset indices at the wrong time!\n");
        result = false;
    }

    // After getting the last element in the queue, it should reset
    u64 last_element = 0;
    queue_get(&q, &last_element);
    if (q.back_idx != 0 || q.front_idx != 0) {
        printf("GET: Indices weren't reset on empty queue!\n");
        result = false;
    }

    queue empty = queue_create(0, sizeof(u64));
    if (!queue_empty(empty)) {
        printf("EMPTY: False negative!\n");
        result = false;
    }

    // This will just crash if NULL isn't handled
    queue_add(&empty, &temp);

    // We have to clear it again in case NULL is handled correctly
    free(empty.buf.data);
    empty = (queue){0};
    u64 empty_read = 0;
    queue_get(&empty, &empty_read);

    // Add some elements back to test clearing
    queue_add(&q, &temp);
    queue_add(&q, &temp);
    if (queue_empty(q)) {
        printf("EMPTY: False positive!\n");
        result = false;
    }
    queue_clear(&q);
    if (q.front_idx != 0 || q.back_idx != 0 || q.buf.data == NULL || q.buf.alloc_size == 0) {
        printf("CLEAR: Clear doesn't act as expected!\n");
        result = false;
    }

    queue_destroy(&q);
    queue_destroy(&empty);

    REPORT_RESULT(result);
    return result;
}
