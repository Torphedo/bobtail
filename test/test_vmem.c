#include <common/logging.h>
#include <common/int.h>
#include <common/vmem.h>
#include <string.h>

#include "testing.h"

bool test_vmem() {
    bool result = true;

    // Create a ring/repeat mapping, where one buffer is repeated many
    // times in virtual memory. Modifying any of these "views" into the
    // underlying buffer reflects the change in all views.
    const u32 ring_size = 8 * VMEM_ALLOC_GRANULARITY;
    const u32 ring_count = 5;
    u8* ringmap = vmem_create_repeat_mapping(8, ring_count);

    if (ringmap == NULL) {
        printf("Failed to create ring mapping!\n");
        result = false;
    }
    ringmap[0] = 20;

    // Setting this value in 1 location updates every location
    const u8 sum = ringmap[0] + ringmap[ring_size] + ringmap[ring_size * 2] + ringmap[ring_size * 3] + ringmap[ring_size * 4];
    if (sum != 100) {
        printf("Ring mapping isn't working!\n");
        result = false;
    }
    vmem_destroy_repeat_mapping(ringmap, 8, 5);

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

    REPORT_RESULT(result);
    return result;
}
