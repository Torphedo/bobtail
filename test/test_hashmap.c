#include <string.h>

#include <common/logging.h>
#include <common/int.h>
#include <common/crc32.h>
#include <common/hashmap.h>

#include "testing.h"

bool test_hashmap() {
    bool result = true;

    hashbuckets_desc buckets = hb_create(4, 2, sizeof(u32));
    if (!buckets.buckets) {
        result = false;
        goto hb_end;
    }

    const u32 data2 = 7;
    const char data2_key[] = "another key";
    const u32 data2_key_hash = crc32buf((const u8*)data2_key, strlen(data2_key));

    HB_ADD_VAL(&buckets, "a key", 42, u32);
    result &= hb_add_obj(&buckets, "another key", &data2);
    if (!result) {
        printf("Failed to add 2 basic objects to hash buckets.\n");
        goto hb_end;
    }

    const u32 fetch = HB_FIND_VAL(&buckets, "a key", u32);
    const u32* fetch2 = hb_find_obj(&buckets, "another key");
    if (!fetch2) {
        printf("Failed to recall 2 values from hash buckets.\n");
        result = false;
        goto hb_end;
    }

    // Everything should still work after changing bucket count
    hb_increase_buckets(&buckets, 20);

    if (fetch != 42) {
        printf("First recalled value %d doesn't match stored value %d\n", fetch, 42);
        result = false;
        goto hb_end;
    }

    if (*fetch2 != data2) {
        printf("Second recalled value %d doesn't match stored value %d\n", *fetch2, data2);
        result = false;
        goto hb_end;
    }
    // hb_resize_buckets(&buckets, 3);


hb_end:
    hb_destroy(&buckets);

    REPORT_RESULT(result);
    return result;
}
