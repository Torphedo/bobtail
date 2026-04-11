#pragma once
#include <common/hashmap.h>
#include <common/crc32.h>
#include <common/logging.h>

namespace bob {

template<typename valT>
struct map {
    #define MAP_CHECK_DESTROYED() \
    do {                          \
        if (!buckets.buckets) {   \
            LOG_MSG(error, "You're attempting to use an already-destroyed bob::map (%p)!\n", this); \
        }                         \
    } while (0)

    hashbuckets_desc buckets;

    valT* at(const char* key) {
        MAP_CHECK_DESTROYED();
        return (valT*)hb_find_obj(&buckets, key);
    }
    
    valT* at(hashkey_t key) {
        MAP_CHECK_DESTROYED();
        return (valT*)hb_find_obj_direct(&buckets, key);
    }

    bool insert(const char* key, const valT& obj) {
        MAP_CHECK_DESTROYED();
        return hb_add_obj(&buckets, key, &obj);
    }

    bool insert(hashkey_t key, const valT& obj) {
        MAP_CHECK_DESTROYED();
        return hb_add_obj_direct(&buckets, key, &obj);
    }

    valT& operator[](hashkey_t key) {
        valT* obj = at(key);
        if (!obj) {
            // Reserve a slot if needed
            if (insert(key, valT{})) {
                obj = at(key);
            } else {
                assert(false && "Unable to insert key!");
            }
        }

        return *obj;
    }
    valT& operator[](const char* key) {
        const hashkey_t hash = crc32fast((const u8*)key, strlen(key));
        return operator[](hash);
    }

    map(u32 num_buckets = 10, u32 bucket_size = 10) {
        buckets = hb_create(num_buckets, bucket_size, sizeof(valT));
    }

    map(const map& o) {
        buckets = hb_clone(o.buckets);
    }

    map(map&& o) {
        buckets = o.buckets;
        o.buckets.buckets = nullptr;
    }

    ~map() {
        hb_destroy(&buckets);
    }
};

} // namespace bob
