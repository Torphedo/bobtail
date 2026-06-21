#pragma once
#include "int.h"
#include "list.h"

// A region of memory which can be snapshotted and reverted to previous states
typedef struct {
    void* buf;
    u64 size;
    void* snapshot_base;
    s32 cur_snapshot;
    list snapshots;
}tt_region;

/// Allocate a time-travelable memory region
tt_region tt_allocate(u64 size);

/// Destroy a time-travelable memory region
void tt_destroy(tt_region* region);

/// Save a new snapshot of the region
/// @param region The region to snapshot
/// @return The ID of the snapshot
s32 tt_snapshot(tt_region* region);

// Revert to a specific snapshot
void tt_revert(tt_region* region, s32 snapshot_id);

// Revert or fast-foward by @ref diff steps from the current state
void tt_revert_relative(tt_region* region, s32 diff);

// Get the snapshot ID of the current region state
s32 tt_current_snapshot(const tt_region* region);