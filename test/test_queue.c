#include <stdlib.h>

#include <common/logging.h>
#include <common/int.h>
#include <common/queue.h>

#include "testing.h"

bool test_queue() {
    bool result = true;

    // Test basic queue creation
    const u32 alloc_size = 2;
    queue empty = {0}; // For testing reaction to empty queue
    queue q = queue_create(alloc_size);
    if (q.data == NULL) {
        printf("CREATE: Initial alloc failed!\n");
        result = false;
    }
    if (q.alloc_size != alloc_size) {
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
    const queue_element element = queue_get(&q);
    if (element != 0) {
        printf("GET: Queue allowed out-of-bounds read on empty queue!\n");
        result = false;
    }

    const queue_element temp = 42;
    queue_add(&q, temp);
    if (q.alloc_size < sizeof(queue_element) && q.data != NULL) {
        printf("ADD: Resizing doesn't ensure enough space for an element!\n");
        result = false;
    }
    if (q.back_idx != 1) {
        printf("ADD: Back index didn't move!\n");
        result = false;
    }
    if (q.data[0] != temp) {
        printf("ADD: Failed to add element or added the wrong data!\n");
        result = false;
    }
    queue_add(&q, temp);

    if (queue_get(&q) != temp) {
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
    queue_get(&q);
    if (q.back_idx != 0 || q.front_idx != 0) {
        printf("GET: Indices weren't reset on empty queue!\n");
        result = false;
    }

    if (!queue_empty(empty)) {
        printf("EMPTY: False negative!\n");
        result = false;
    }

    // This will just crash if NULL isn't handled
    queue_add(&empty, 42);

    // We have to clear it again in case NULL is handled correctly
    free(empty.data);
    empty = (queue){0};
    queue_get(&empty);

    // Add some elements back to test clearing
    queue_add(&q, temp);
    queue_add(&q, temp);
    if (queue_empty(q)) {
        printf("EMPTY: False positive!\n");
        result = false;
    }
    queue_clear(&q);
    if (q.front_idx != 0 || q.back_idx != 0 || q.data == NULL || q.alloc_size == 0) {
        printf("CLEAR: Clear doesn't act as expected!\n");
        result = false;
    }

    REPORT_RESULT(result);
    return result;
}
