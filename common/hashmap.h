#ifndef HASHMAP_H
#define HASHMAP_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include "int.h"

#define ARRAY_2D_ADDR(arr, i, j, dim1, entry_type) (entry_type*)(((uintptr_t)arr) + (sizeof(entry_type) * ((i) * (dim1) + (j))))

typedef struct {
    u32 hash;
    void* ptr;
}hashbucket_entry;

typedef struct {
    u32 num_buckets;
    u32 bucket_size;
    hashbucket_entry** buckets;
}hashbuckets_desc;

void* hashbucket_find_ptr(u32 hashkey, const hashbuckets_desc* desc) {
    hashbucket_entry* const* buckets = desc->buckets;
    const u32 i = hashkey % desc->num_buckets; // Get the bucket number

    // Buckets have an unlimited size, so we search them linearly.
    const hashbucket_entry* bucket = buckets[desc->bucket_size * i];
    for (u32 j = 0; j < desc->bucket_size; j++) {
        if (bucket[j].hash == hashkey) {
            return bucket[j].ptr;
        }
    }
    return NULL;
}

bool hashbucket_add_ptr(u32 hashkey, void* ptr, hashbuckets_desc* desc) {
    hashbucket_entry** buckets = desc->buckets;
    const u32 i = hashkey % desc->num_buckets; // Get the bucket number

    // Buckets have an unlimited size, so we search for an open slot
    hashbucket_entry* bucket = buckets[desc->bucket_size * i];
    for (u32 j = 0; j < desc->bucket_size; j++) {
        if (bucket[j].hash == 0) {
            continue;
        }
        bucket[j].hash = hashkey;
        bucket[j].ptr = ptr;
        return true;
    }

    return false;
}

#ifdef __cplusplus
}
#endif
#endif // #ifndef HASHMAP_H
