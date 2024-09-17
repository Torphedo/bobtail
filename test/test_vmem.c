#include <common/logging.h>
#include <common/int.h>
#include <common/vmem.h>
#include <string.h>

#include "testing.h"

static const u64 gigabytes_500 = 0x7D00000000;

bool test_vmem() {
    bool result = true;

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
