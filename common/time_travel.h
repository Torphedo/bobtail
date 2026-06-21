#pragma once
#include "int.h"
#include "list.h"

/// @brief A memory region you can snapshot and restore to
///
/// There are 2 different sets of IDs you can use when restoring, *states* and
/// *snapshots*. The *state* ID increments whenever something changes.
/// The snapshot ID increments every time you take a snapshot, even if nothing
/// changed.
/// Restoring *states* is useful for an undo/redo system where you don't care
/// how long you went between changes. Restoring *snapshots* is useful in
/// replaying a simulation, when you need an accurate recreation of the state at
/// each time step.
typedef struct {
    void* buf;
    u64 size;
    void* state_base;
    s32 cur_state;
    list states;
}tt_region;

/// Allocate a time-travelable memory region
tt_region tt_allocate(u64 size);

/// Destroy a time-travelable memory region
void tt_destroy(tt_region* region);

/// Save a new snapshot of the region
/// @param region The region to snapshot
/// @return The ID of the state
/// (if nothing has changed, this may be the same as the last snapshot)
s32 tt_snapshot(tt_region* region);

/// Restore a specific state of memory
void tt_restore_state(tt_region* region, s32 state);

/// Revert or fast-foward by @ref diff steps from the current state
void tt_restore_state_relative(tt_region* region, s32 diff);

// Get the current state ID
s32 tt_current_state(const tt_region* region);

// Make the given state the last one, wipe all states that come after
void tt_make_last_state(tt_region* region, s32 state);

bool tt_is_state_first(tt_region* region, s32 state);
bool tt_is_state_last(tt_region* region, s32 state);

u32 tt_state_count(const tt_region* region);


// Snapshot ID increments on state changes and repeats
// Hard to know if a full replay is needed unless we track the starting
// snapshot ID at each state
void tt_restore_snapshot(tt_region* region, s32 snapshot);

u32 tt_snapshot_count(const tt_region* region);

