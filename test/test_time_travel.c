#include <common/logging.h>
#include <common/time_travel.h>
#include <common/vmem.h>

#include "testing.h"

bool test_state_restore() {
    const u8 states[] = {1, 4, 3, 5, 4, 2, 8};
    bool result = true;

    tt_region reg = tt_allocate(vmem_get_page_size());
    TEST_ASSERT(reg.buf, "Failed to allocate test time travel region!\n");
    u8* buf = reg.buf;
    for (u32 i = 0; i < ARRAY_SIZE(states); i++) {
        *buf = states[i];
        tt_snapshot(&reg);
        const u32 state_count = tt_state_count(&reg);
        TEST_ASSERT(state_count == i + 1, "State count wasn't updated after snapshot!");
        TEST_ASSERT(tt_current_state(&reg) == i + 1, "Current state wasn't updated after snapshot!");
    }

    for (u32 i = 0; i < ARRAY_SIZE(states); i++) {
        tt_restore_state(&reg, i);
        const u8 new_state = *buf;
        TEST_ASSERT(tt_current_state(&reg) == i, "Current state wasn't updated after restore!");
        TEST_ASSERT(new_state == states[i], "Data wasn't restored correctly!");
    }

    tt_restore_state(&reg, 0);
    for (u32 i = 1; i < ARRAY_SIZE(states); i++) {
        tt_restore_state_relative(&reg, 1);
        const u8 new_state = *buf;
        TEST_ASSERT(tt_current_state(&reg) == i, "Current state wasn't updated after incremental restore!");
        TEST_ASSERT(new_state == states[i], "Data wasn't restored correctly during incremental restore!");
    }
    TEST_ASSERT(tt_current_state(&reg) == tt_last_state(&reg), "Should have fast forwarded to the last state!");

    for (s32 i = ARRAY_SIZE(states) - 1; i >= 0; i--) {
        const u8 new_state = *buf;
        TEST_ASSERT(tt_current_state(&reg) == i, "Current state wasn't updated after incremental rewind!");
        TEST_ASSERT(new_state == states[i], "Data wasn't restored correctly during incremental rewind!");
        tt_restore_state_relative(&reg, -1);
    }

    const u32 snapshot_count = tt_snapshot_count(&reg);
    const u32 state_count = tt_state_count(&reg);
    const u32 expected_state_count = ARRAY_SIZE(states);
    TEST_ASSERT(state_count == expected_state_count, "State count is wrong!");
    TEST_ASSERT(state_count == snapshot_count, "State count should match snapshot count when there are no duplicates!");

    TEST_ASSERT(tt_is_state_first(&reg, 0), "First state detection is broken!");
    TEST_ASSERT(!tt_is_state_first(&reg, 1), "First state detection is broken!");
    TEST_ASSERT(tt_is_state_last(&reg, state_count - 1), "Last state detection is broken!");
    TEST_ASSERT(!tt_is_state_last(&reg, 0), "Last state detection is broken!");

    tt_destroy(&reg);
    return result;
}

bool test_time_travel() {
    bool result = true;

    result &= test_state_restore();

    REPORT_RESULT(result);
    return result;
}
