#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "int.h"
#include "logging.h"
#include "hashmap.h"

// Each entry in the buckets stores the hash, as well as the actual object
#define HB_ENTRY_SIZE_BYTES(desc) ((desc)->entry_size + sizeof(hashkey_t))

void hb_key_pos(const hashbuckets_desc* desc, u32 key, u32* bucket_out, u32* idx_out) {
    *bucket_out = key % desc->num_buckets;
    *idx_out = key / desc->bucket_entries;
}

/// @brief Create a new set of hashbuckets
///
/// @param num_buckets The number of buckets in the set
/// @param bucket_entries The size (in entrys) of each bucket
/// @param entry_size The size of each object you'll be storing in the buckets.
hashbuckets_desc hb_create(u32 num_buckets, u32 bucket_entries, u32 entry_size) {
    const u32 bucket_size = bucket_entries * (entry_size + sizeof(hashkey_t));
    return (hashbuckets_desc) {
        .bucket_entries = bucket_entries,
        .num_buckets = num_buckets,
        .entry_size = entry_size,
        .buckets = calloc(num_buckets, bucket_size),
    };
}

void hb_destroy(hashbuckets_desc* desc) {
    free(desc->buckets);
    *desc = (hashbuckets_desc){0};
}

void* hb_find_slot(const hashbuckets_desc* desc, hashkey_t key) {
    uintptr_t buckets = (uintptr_t)desc->buckets;
    u32 bucket_idx = key % desc->num_buckets;

    const u32 bucket_size = desc->bucket_entries * HB_ENTRY_SIZE_BYTES(desc);
    const uintptr_t bucket = (buckets + (bucket_size * bucket_idx));

    // Buckets have an unlimited size, so we search linearly for an open slot.
    for (u32 j = 0; j < desc->bucket_entries; j++) {
        void* entry = (void*)(bucket + (j * desc->entry_size));
        const u32* hash = entry;
        if (*hash == 0 || *hash == key) {
            // Found open slot or existing entry.
            return entry;
        }
    }

    return NULL;
}

void* hb_find_obj_direct(hashbuckets_desc* desc, hashkey_t hashkey) {
    void* entry = hb_find_slot(desc, hashkey);
    if (entry) {
        const u32* hash = entry;
        void* obj = (void*)(&hash[1]);
        if (*hash == hashkey) {
            return obj;
        }
    }

    LOG_MSG(error, "Failed to find object with hash %d in buckets %p\n", hashkey, desc);
    return NULL;

}

bool hb_resize_buckets(hashbuckets_desc* desc, u32 bucket_entries) {
    if (desc->bucket_entries == bucket_entries) {
        return true;
    }

    hashbuckets_desc new_desc = hb_create(desc->num_buckets, bucket_entries, desc->entry_size);
    if (!new_desc.buckets) {
        LOG_MSG(error, "Failed to allocate new buckets when resizing %d -> %d\n", desc->bucket_entries, bucket_entries);
        return false;
    }

    const u32 old_bucket_size = desc->bucket_entries * HB_ENTRY_SIZE_BYTES(desc);
    const u32 new_bucket_size = new_desc.bucket_entries * HB_ENTRY_SIZE_BYTES(desc);
    const uintptr_t newbuf = (uintptr_t)new_desc.buckets;
    const uintptr_t oldbuf = (uintptr_t)desc->buckets;
    const bool shrinking = new_bucket_size < old_bucket_size;

    for (u32 i = 0; i < desc->num_buckets; i++) {
        const void* old_bucket = (void*)(oldbuf + old_bucket_size * i);
        void* new_bucket =       (void*)(newbuf + new_bucket_size * i);
        memcpy(new_bucket, old_bucket, MIN(old_bucket_size, new_bucket_size));

        // This will search for empty slots within this bucket.
        // We assume that there won't be any small hashes (< num_buckets).
        const s64 slot = (s64)hb_find_slot(desc, i);
        if (slot > 0) {
            const s64 slot_idx = (slot - (s64)old_bucket) / desc->entry_size;
            const s64 slots_left = (s64)new_desc.bucket_entries - slot_idx - 1;
            if (slots_left < 0) {
                LOG_MSG(error, "Cancelling %d -> %d resize because %d entries in bucket %d won't fit\n",
                        desc->num_buckets, new_desc.bucket_entries, -slots_left, i);
                hb_destroy(&new_desc);
                return false;
            }
        }
    }

    hb_destroy(desc);
    *desc = new_desc;
    return true;
}

bool hb_add_obj_direct(hashbuckets_desc* desc, hashkey_t hashkey, const void* obj) {
    void* entry = hb_find_slot(desc, hashkey);
    if (!entry) {
        hb_resize_buckets(desc, desc->bucket_entries * 1.5f);
        entry = hb_find_slot(desc, hashkey);
    }

    if (entry) {
        u32* hash = entry;
        void* obj_slot = (void*)(&hash[1]);

        // Copy object into next open slot
        memcpy(obj_slot, obj, desc->entry_size);
        *hash = hashkey;
        return true;
    }

    // TODO: Add a function to increase bucket size we can use in this case
    LOG_MSG(error, "Failed to insert object with hash %d into buckets %p\n", hashkey, desc);
    return false;
}
