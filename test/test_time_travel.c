#include <common/logging.h>
#include <common/time_travel.h>
#include <common/vmem.h>

#include "testing.h"

bool test_state_restore() {
    const u8 states[] = {1, 4, 3, 5, 4, 2, 8};
    bool result = true;

    tt_region reg = tt_allocate(vmem_get_page_size());
    if (!reg.buf) {
        LOG_MSG(error, "Failed to allocate test time travel region!\n");
        return false;
    }
    u8* buf = reg.buf;
    for (u32 i = 0; i < ARRAY_SIZE(states); i++) {
        *buf = states[i];
        tt_snapshot(&reg);
        const u32 state_count = tt_state_count(&reg);
        result &= state_count == i + 1;
        result &= tt_current_state(&reg) == i + 1;
    }

    for (u32 i = 0; i < ARRAY_SIZE(states); i++) {
        tt_restore_state(&reg, i);
        result &= tt_current_state(&reg) == i;
        const u8 new_state = *buf;
        result &= (new_state == states[i]);
    }

    tt_restore_state(&reg, 0);
    for (u32 i = 1; i < ARRAY_SIZE(states); i++) {
        tt_restore_state_relative(&reg, 1);
        result &= tt_current_state(&reg) == i;
        const u8 new_state = *buf;
        result &= (new_state == states[i]);
    }

    const u32 snapshot_count = tt_snapshot_count(&reg);
    const u32 state_count = tt_state_count(&reg);
    const u32 expected_state_count = ARRAY_SIZE(states);
    result &= state_count == expected_state_count;
    // There were no duplicate states, so these should match
    result &= state_count == snapshot_count;

    result &= tt_is_state_first(&reg, 0);
    result &= !tt_is_state_first(&reg, 1);
    result &= tt_is_state_last(&reg, state_count - 1);
    result &= !tt_is_state_last(&reg, 0);

    tt_destroy(&reg);
    return result;
}

bool test_time_travel() {
    bool result = true;

    result &= test_state_restore();

    REPORT_RESULT(result);
    return result;
}
