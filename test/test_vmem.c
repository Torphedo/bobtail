#include <common/logging.h>
#include <common/int.h>
#include <common/vmem.h>
#include <string.h>

#include "testing.h"

bool test_vmem_ringmap() {
    // Create a ring/repeat mapping, where one buffer is repeated many
    // times in virtual memory. Modifying any of these "views" into the
    // underlying buffer reflects the change in all views.
    const u32 ring_size = 8 * VMEM_ALLOC_GRANULARITY;
    const u32 ring_count = 5;
    u8* ringmap = vmem_create_repeat_mapping(8, ring_count);

    if (ringmap == NULL) {
        LOG_MSG(error, "Failed to create ring mapping!\n");
        // There's nothing to clean up, so we can just return right away.
        return false;
    }
    ringmap[0] = 20;

    // Setting this value in 1 location updates every location
    bool result = true;
    for (u32 i = 0; i < ring_count; i++) {
        const u8 val = ringmap[ring_size * i];
        if (val != ringmap[0]) {
            LOG_MSG(error, "Ring mapping isn't working!\n");
            LOG_MSG(debug, "[0] = %d, [ring_size] = %d\n", ringmap[0], val);
            // We can't return right away because we need to clean up
            result = false;
        }
    }

    // Clean up
    vmem_destroy_repeat_mapping(ringmap, 8, ring_count);
    return result;
}

bool test_vmem_reserve_commit() {
    bool result = true;
    // A 35-bit region is 32 GiB of address space. It's easy to reserve much
    // larger areas (like 512GiB), but those calls will start failing under
    // valgrind somewhere between 2^35 and 2^36.
    const u64 region_size = exponent(2, 35);
    u8* region = vmem_reserve(region_size);
    if (region == NULL) {
        printf("Failed to reserve %lluGiB region!\n", region_size / exponent(1024, 3));
        result = false;
    }

    // This causes reservations in the pagefile, so we can't go too crazy with
    // committing.
    if (vmem_commit(region, VMEM_PAGE_SIZE * 5000) == -1) {
        printf("Failed to commit %lluGiB region!\n", region_size / exponent(1024, 3));
        result = false;
    }

    // Write to the region to show it's usable. These are all perfectly valid,
    // and only a small amount of memory is used (~7MiB in a 512GiB region).
    // (I think the OS allocates a bunch of pages at once, under the assumption
    // you'll use it sequentially. So there is overhead, but not a ton)
    if (region != NULL) {
        *region = 20;
        region[100000] = 20;
        region[1000000] = 20;
        region[10000000] = 20;
        region[20000000] = 20;

        for (u8 i = 0; i < 35; i++) {
            u8* addr = &region[exponent(2, i)];
            vmem_commit(addr, VMEM_PAGE_SIZE);
            *addr = 50;
        }
        vmem_free(region, region_size);
    }

    return result;
}

bool test_vmem_write_watch() {
    const u64 num_pages = 50;
    u8* watched = vmem_alloc_watched(num_pages);
    if (watched == NULL) {
        // There's nothing to clean up yet, just return right away
        return false;
    }

    bool result = true;
    void* dirty_pages[50] = {0};
    u64 num_dirty_pages = 0;
    vmem_get_dirty_pages(watched, num_pages, dirty_pages, ARRAY_SIZE(dirty_pages), &num_dirty_pages);
    if (num_dirty_pages > 0) {
        LOG_MSG(error, "The entire region should be considered clean immediately after allocation!\n");
        result = false;
    }

    watched[0] = 42;

    // After clearing, no pages should be considered dirty.
    vmem_reset_write_watching(watched, num_pages * VMEM_PAGE_SIZE);
    memset(dirty_pages, 0, sizeof(dirty_pages)); // Wipe pointers we just got
    num_dirty_pages = 0;
    vmem_get_dirty_pages(watched, num_pages, dirty_pages, ARRAY_SIZE(dirty_pages), &num_dirty_pages);
    if (num_dirty_pages > 0) {
        LOG_MSG(error, "No pages should be considered dirty after a reset! (got %d)\n", num_dirty_pages);
    }

    // Write a value to a random part of the region
    watched[27] = 42;
    memset(dirty_pages, 0, sizeof(dirty_pages)); // Wipe pointers we just got
    num_dirty_pages = 0;
    vmem_get_dirty_pages(watched, num_pages, dirty_pages, ARRAY_SIZE(dirty_pages), &num_dirty_pages);
    if (num_dirty_pages != 1) {
        LOG_MSG(error, "Exactly 1 page should be considered dirty! (got %d)\n", num_dirty_pages);
    }

    // The address we wrote to was in the first page, so the modified page
    // address should be the same as the base address.
    if (dirty_pages[0] != watched) {
        LOG_MSG(error, "A write was missed, or the wrong page was identified!\n");
    }


    // Cleanup
    vmem_free(watched, num_pages * VMEM_PAGE_SIZE);
    return result;
}

bool test_vmem() {
    bool result = true;

    result &= test_vmem_reserve_commit();
    result &= test_vmem_ringmap();
    result &= test_vmem_write_watch();

    REPORT_RESULT(result);
    return result;
}
