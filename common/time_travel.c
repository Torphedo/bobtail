/// @author Torphedo
#include "time_travel.h"
#include <stdlib.h>
#include <common/vmem.h>

enum {
    DIRTY_MAX = 256,
};

// A diff between 2 snapshots
typedef struct {
    void* pagebuf; // Buffer to store pages that changed between snapshots
    u32 page_count; // The number of changed pages
    void* page_targets[]; // The addresses to store each page to apply the diff
}tt_snapshot_record;

/// Create a diff between snapshots using a list of dirty pages
/// @param dirty An array of pointers to dirty pages
/// @param pages The number of dirty pages
/// @return The snapshot diff
tt_snapshot_record* tt_allocate_snapshot_record(void** dirty, u32 pages) {
    tt_snapshot_record* out = calloc(1, sizeof(*out) + pages * sizeof(*out->page_targets));
    if (!out) {
        return NULL;
    }
    if (pages == 0) {
        return out;
    }
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

void tt_destroy_snapshot_record(tt_snapshot_record* snap) {
    free(snap->pagebuf);
    free(snap);
}

/// Get a snapshot diff at a specific index
tt_snapshot_record* tt_get_snapshot(tt_region* region, s32 id) {
    return *(tt_snapshot_record**)list_get_element(region->snapshots, id);
}

/// Advance the state by 1 snapshot
void tt_snapshot_advance(tt_region* region) {
    const s32 target = region->cur_snapshot + 1;
    if (target >= list_size(region->snapshots)) {
        return; // Nothing to advance to
    }

    // Find the diff that goes from the current snapshot to the next one
    const tt_snapshot_record* snap = tt_get_snapshot(region, target);

    // Apply the diff by copying each changed page
    for (u32 j = 0; j < snap->page_count; j++) {
        uintptr_t source = (uintptr_t)snap->pagebuf + (j * vmem_get_page_size());
        memcpy(snap->page_targets[j], (const void*)source, vmem_get_page_size());
    }
}

tt_region tt_allocate(u64 size) {
    const u64 num_pages = vmem_size_to_pages(size);
    tt_region out = {
        .size = size,
        .buf = vmem_alloc_watched(num_pages),
    };

    list_create(10, sizeof(tt_snapshot_record*));
}

void tt_destroy(tt_region* region) {
    for (u32 i = 0; i < list_size(region->snapshots); i++) {
        tt_snapshot_record* snap = tt_get_snapshot(region, i);
        tt_destroy_snapshot_record(snap);
    }
    list_destroy(&region->snapshots);

    free(region->snapshot_base);
    vmem_free(region->buf, region->size);
}

s32 tt_snapshot(tt_region* region) {
    if (!region->snapshot_base) {
        // Copy the entire region as an initial state we can diff against
        region->snapshot_base = malloc(region->size);
        if (!region->snapshot_base) {
            return -1;
        }
        memcpy(region->snapshot_base, region->buf, region->size);
        vmem_reset_write_watching(region->buf, region->size);
        return 0;
    }

    const u64 num_pages = vmem_size_to_pages(region->size);
    u64 num_dirty = 0;
    void* dirty_pages[DIRTY_MAX] = {};
    vmem_get_dirty_pages(region->buf, num_pages, dirty_pages, ARRAY_SIZE(dirty_pages), &num_dirty);

    const u32 idx = list_size(region->snapshots);
    tt_snapshot_record* snap = tt_allocate_snapshot_record(dirty_pages, num_dirty);
    list_add(&region->snapshots, &snap);

    vmem_reset_write_watching(region->buf, region->size);
    return idx;
}

void tt_revert(tt_region* region, s32 snapshot_id) {
    if (snapshot_id == region->cur_snapshot) {
        return; // Nothing to do
    }

    if (snapshot_id < region->cur_snapshot) {
        // We have to rewind, so revert to the initial state and then fast forward
        memcpy(region->buf, region->snapshot_base, region->size);
        region->cur_snapshot = 0;
    }

    // Fast forward each snapshot until we reach the target
    for (u32 i = region->cur_snapshot; i < snapshot_id; i++) {
        tt_snapshot_advance(region);
    }
}

void tt_revert_relative(tt_region* region, s32 diff) {
    const s32 target = region->cur_snapshot + diff;
    tt_revert(region, target);
}
