/// @author Torphedo
#include "time_travel.h"
#include <stdlib.h>
#include <assert.h>
#include "vmem.h"


enum {
    DIRTY_MAX = 256,
};

// A diff between 2 snapshots
typedef struct {
    void* pagebuf; // Buffer to store pages that changed between snapshots
    u32 page_count; // The number of changed pages
    // How many snapshots in a row the state remained unchanged
    u32 repeat_count;
    void* page_targets[]; // The addresses to store each page to apply the diff
}tt_state_diff;

/// Create a diff between states using a list of dirty pages
/// @param dirty An array of pointers to dirty pages
/// @param pages The number of dirty pages
tt_state_diff* tt_state_allocate(void** dirty, u32 pages) {
    tt_state_diff* out = calloc(1, sizeof(*out) + pages * sizeof(*out->page_targets));
    if (!out) {
        return NULL;
    }
    if (pages == 0) {
        return out;
    }
    out->repeat_count = 1;
    out->page_count = pages;
    out->pagebuf = malloc(pages * vmem_get_page_size());
    if (!out->pagebuf) {
        free(out);
        return NULL;
    }

    memcpy(out->page_targets, dirty, sizeof(*dirty) * pages);
    for (u32 i = 0; i < pages; i++) {
        uintptr_t dest = (uintptr_t)out->pagebuf + vmem_get_page_size() * i;
        memcpy((void*)dest, dirty[i], vmem_get_page_size());
    }

    return out;
}

void tt_state_destroy(tt_state_diff* diff) {
    free(diff->pagebuf);
    free(diff);
}

tt_state_diff* tt_get_state(tt_region* region, s32 id) {
    if (id == 0) {
        return NULL;
    }
    return *(tt_state_diff**)list_get_element(region->states, id - 1);
}

s32 tt_last_state(const tt_region* region) {
    return list_size(region->states);
}

/// Advance the state by 1
void tt_snapshot_advance(tt_region* region) {
    const s32 target = region->cur_state + 1;
    if (target >= tt_state_count(region)) {
        return; // Nothing to advance to
    }

    // Find the diff that goes from the current snapshot to the next one
    const tt_state_diff* diff = tt_get_state(region, target);

    // Apply the diff by copying each changed page
    for (u32 j = 0; j < diff->page_count; j++) {
        uintptr_t source = (uintptr_t)diff->pagebuf + (j * vmem_get_page_size());
        memcpy(diff->page_targets[j], (const void*)source, vmem_get_page_size());
    }
    region->cur_state = target;
}

s32 tt_snapshot_to_state(const tt_region* region, s32 snapshot) {
    if (snapshot == 0) {
        return 0;
    }

    for (s32 i = 0; i < list_size(region->states); i++) {
        const tt_state_diff* diff = tt_get_state((tt_region*)region, i);
        if (!diff) {
            continue;
        }
        if (snapshot <= diff->repeat_count) {
            return i;
        }
        snapshot -= diff->repeat_count;
    }
    return -1;
}

tt_region tt_allocate(u64 size) {
    const u64 num_pages = vmem_size_to_pages(size);
    tt_region out = {
        .size = size,
        .buf = vmem_alloc_watched(num_pages),
    };

    out.states = list_create(10, sizeof(tt_state_diff*));
    return out;
}

void tt_destroy(tt_region* region) {
    for (u32 i = 1; i < list_size(region->states); i++) {
        tt_state_diff* diff = tt_get_state(region, i);
        tt_state_destroy(diff);
    }
    list_destroy(&region->states);

    free(region->state_base);
    vmem_free(region->buf, region->size);
}

s32 tt_snapshot(tt_region* region) {
    if (!region->state_base) {
        assert(region->cur_state == 0);

        // Copy the entire region as an initial state we can diff against
        region->state_base = malloc(region->size);
        if (!region->state_base) {
            return -1;
        }
        memcpy(region->state_base, region->buf, region->size);
        vmem_reset_write_watching(region->buf, region->size);
        return region->cur_state++;
    }

    const u64 num_pages = vmem_size_to_pages(region->size);
    u64 num_dirty = 0;
    void* dirty_pages[DIRTY_MAX] = {};
    vmem_get_dirty_pages(region->buf, num_pages, dirty_pages, ARRAY_SIZE(dirty_pages), &num_dirty);
    if (num_dirty == 0) {
        // Nothing has changed.
        // If the initial state was just saved, there may be no state diffs yet.
        const s32 last = tt_last_state(region);
        if (last > 0) {
            tt_get_state(region, last)->repeat_count++;
        }
        return last;
    }


    const u32 idx = list_size(region->states);
    tt_state_diff* diff = tt_state_allocate(dirty_pages, num_dirty);
    list_add(&region->states, &diff);
    region->cur_state++;

    vmem_reset_write_watching(region->buf, region->size);
    return idx;
}

void tt_restore_state(tt_region* region, s32 state) {
    if (state == region->cur_state) {
        return; // Nothing to do
    }

    if (state < region->cur_state) {
        // We have to rewind, so revert to the initial state and then fast forward
        memcpy(region->buf, region->state_base, region->size);
        region->cur_state = 0;
    }

    // Fast forward each state until we reach the target
    for (u32 i = region->cur_state; i < state; i++) {
        tt_snapshot_advance(region);
    }
}

void tt_restore_state_relative(tt_region* region, s32 diff) {
    const s32 target = region->cur_state + diff;
    tt_restore_state(region, target);
}

s32 tt_current_state(const tt_region* region) {
    return region->cur_state;
}

void tt_make_last_state(tt_region* region, s32 state) {
    for (u32 i = state; i < list_size(region->states); i++) {
        tt_state_diff* diff = tt_get_state(region, i);
        if (diff) {
            tt_state_destroy(diff);
        }
        list_remove(&region->states, i);
    }

    region->cur_state = tt_last_state(region);
}

bool tt_is_state_first(tt_region* region, s32 state) {
    return state == 0;
}

bool tt_is_state_last(tt_region* region, s32 state) {
    return state == tt_last_state(region);
}

u32 tt_state_count(const tt_region* region) {
    return tt_last_state(region) + (region->state_base != NULL);
}

void tt_restore_snapshot(tt_region* region, s32 snapshot) {
    const s32 state = tt_snapshot_to_state(region, snapshot);
    if (state >= 0) {
        tt_restore_state(region, state);
    }
}

u32 tt_snapshot_count(const tt_region* region) {
    u32 count = (region->state_base != NULL);
    for (s32 i = 1; i < tt_state_count(region); i++) {
        const tt_state_diff* diff = tt_get_state((tt_region*)region, i);
        if (diff) {
            count += diff->repeat_count;
        }
    }
    return count;
}
