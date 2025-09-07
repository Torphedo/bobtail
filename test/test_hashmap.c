#include <string.h>

#include <common/logging.h>
#include <common/int.h>
#include <common/crc32.h>
#include <common/hashmap.h>

#include "testing.h"

bool test_hashmap() {
    bool result = true;

    hashbuckets_desc buckets = hb_create(4, 4, sizeof(u32));
    if (!buckets.buckets) {
        result = false;
        goto hb_end;
    }

    const char data_key[] = "a key";
    const u32 data_key_hash = crc32buf((const u8*)data_key, strlen(data_key));

    const u32 data2 = 7;
    const char data2_key[] = "another key";
    const u32 data2_key_hash = crc32buf((const u8*)data2_key, strlen(data2_key));

    HB_ADD_VAL(&buckets, data_key_hash, 42, u32);
    result &= hb_add_obj(&buckets, data2_key_hash, &data2);
    if (!result) {
        printf("Failed to add 2 basic objects to hash buckets.\n");
        goto hb_end;
    }


    const u32 fetch = HB_FIND_VAL(&buckets, data_key_hash, u32);
    const u32* fetch2 = hb_find_obj(&buckets, data2_key_hash);
    if (!fetch2) {
        printf("Failed to recall 2 values from hash buckets.\n");
        result = false;
        goto hb_end;
    }

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


hb_end:
    hb_destroy(&buckets);

    REPORT_RESULT(result);
    return result;
}
