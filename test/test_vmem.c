#include <common/logging.h>
#include <common/int.h>
#include <common/vmem.h>
#include <string.h>

#include "testing.h"

static const u64 gigabytes_500 = 0x7D00000000;

bool test_vmem() {
    bool result = true;

    // Create a ring/repeat mapping, where one 3-page buffer is repeated 5
    // times in virtual memory. Modifying any of these 5 "views" into the
    // underlying buffer reflects the change in all views.
    const u32 ring_size = 3 * VMEM_PAGE_SIZE;
    u8* ringmap = vmem_create_repeat_mapping(3, 5);
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

    void* region = vmem_reserve(gigabytes_500);
    if (region == NULL) {
        printf("Failed to reserve 500GiB region!\n");
        result = false;
    }
    if (vmem_commit(region, VMEM_PAGE_SIZE * 5000) == -1) {
        printf("Failed to commit 500GiB region!\n");
        result = false;
    }

    u8* ptr = (u8*)region;

    // Write to the region to show it's usable. These are all perfectly valid,
    // and only ~6MiB of memory is used in total (on my system).
    // (I think mostly the OS allocates a bunch of pages at once, under the
    // assumption you'll use it sequentially. So there is overhead, but not a
    // ton)
    *ptr = 20;
    ptr[100000] = 20;
    ptr[1000000] = 20;
    ptr[10000000] = 20;
    ptr[20000000] = 20;

    vmem_free(region, gigabytes_500);
    REPORT_RESULT(result);
    return result;
}
