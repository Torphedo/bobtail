#include <algorithm>

#include <cpp/hashmap.hxx>
#include <int.h>
#include "testing.h"

extern "C" bool test_hashmap_cpp() {
    bool result = true;
    bob::map<u32> map;

    map["testkey"] = 42;
    map[72] = 5;

    result &= (map["testkey"] == 42);
    result &= (map[72] == 5);

    bob::map<u32> map2 = map;

    result &= (map2["testkey"] == 42);
    result &= (map2[72] == 5);

    bob::map<u32> map3 = std::move(map2);
    // Old instance should be destroyed
    result &= map2.buckets.buckets == nullptr;

    // The destroyed instance can be used, it'll automatically reallocate
    map2["testkey"] = 42;

    REPORT_RESULT(result);
    return result;
}
