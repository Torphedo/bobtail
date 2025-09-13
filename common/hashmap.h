#ifndef HASHMAP_H
#define HASHMAP_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "int.h"
#include "crc32.h"

/// @brief Storage with logarithmic search time
///
/// Internally there are multiple arrays ("buckets") an entry might fall into,
/// based on its hash. The more buckets there are, the fewer entries will be in
/// each bucket.
/// This interface requires you to manage hashing yourself.
typedef struct {
    void* buckets;
    u32 num_buckets;
    u32 bucket_entries;
    u32 entry_size;
}hashbuckets_desc;

typedef u32 hashkey_t;

/// @brief Create a new set of hashbuckets
///
/// @param num_buckets The number of buckets in the set. This can't be changed after creation.
/// @param bucket_entries The size (in entries) of each bucket
/// @param entry_size The size of each object you'll be storing in the buckets. This can't be changed after creation.
hashbuckets_desc hb_create(u32 num_buckets, u32 bucket_entries, u32 entry_size);

/// @brief Clone a set of hashbuckets
///
/// @param desc The hashbuckets to clone
hashbuckets_desc hb_clone(const hashbuckets_desc desc);

/// @brief Destroy a set of hashbuckets
void hb_destroy(hashbuckets_desc* desc);

/// @brief Find a stored object by its key
void* hb_find_obj_direct(hashbuckets_desc* desc, hashkey_t hashkey);

/// @brief Store an object and associate it with a key
///
/// @param key The key to associate with the object
/// @param obj The object to store. [desc->entry_size] bytes are copied from this pointer.
static void* hb_find_obj(hashbuckets_desc* desc, const char* key) {
    const u32 keyhash = crc32fast((const u8*)key, strlen(key));
    return hb_find_obj_direct(desc, keyhash);
}

bool hb_resize_buckets(hashbuckets_desc* desc, u32 bucket_entries);

/// @brief Store an object and associate it with a key hash
///
/// @param desc The hash buckets to store the object in
/// @param hashkey The key's hash
/// @param obj The object to store. [desc->entry_size] bytes are copied from this pointer.
bool hb_add_obj_direct(hashbuckets_desc* desc, hashkey_t hashkey, const void* obj);

/// @brief Store an object and associate it with a key
///
/// @param key The key to associate with the object
/// @param obj The object to store. [desc->entry_size] bytes are copied from this pointer.
static bool hb_add_obj(hashbuckets_desc* desc, const char* key, const void* obj) {
    const u32 keyhash = crc32fast((const u8*)key, strlen(key));
    return hb_add_obj_direct(desc, keyhash, obj);
}

/// @brief A helper to return the object directly instead of getting a pointer
/// @param key The hash of the key
/// @param T The type of the stored object
#define HB_FIND_VAL_DIRECT(desc, keyhash, T) *(T*)hb_find_obj_direct((desc), (keyhash))

/// @brief A helper to store an object with no address (aka. an r-value).
/// @param key The hash of the key
/// @param val The data to be stored
/// @param T The type of [val]. sizeof(T) should match the entry size.
#define HB_ADD_VAL_DIRECT(desc, key, val, T)          \
do {                                           \
    assert((desc)->entry_size == sizeof(T) && "The hashbucket doesn't have objects of this size!"); \
    const T __hb_temp = (val);                 \
    hb_add_obj_direct(desc, key, &__hb_temp);  \
} while (0)                                    \

/// @brief A helper to return the object directly instead of getting a pointer
/// @param key The key string (not its hash!). The hash function is crc32.
/// @param T The type of the stored object
#define HB_FIND_VAL(desc, key, T) HB_FIND_VAL_DIRECT(desc, crc32fast((const u8*)(key), strlen(key)), T)

/// @brief A helper to store an object with no address (aka. an r-value).
/// @param key The key string (not its hash!). The hash function is crc32.
/// @param val The data to be stored
/// @param T The type of [val]. sizeof(T) should match the entry size.
#define HB_ADD_VAL(desc, key, val, T) HB_ADD_VAL_DIRECT(desc, crc32fast((const u8*)(key), strlen(key)), val, T)

#ifdef __cplusplus
}
#endif
#endif // #ifndef HASHMAP_H
