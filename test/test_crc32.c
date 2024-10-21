#include <common/logging.h>
#include <common/int.h>
#include <common/crc32.h>

#include "testing.h"

typedef struct {
    const char* data;
    u32 data_size;
    u32 hash;
}crc32_testcase;

const char crc_data1[] = "The answer to life, the universe, and everything";
const char crc_data2[] = "The quick brown fox jumps over the lazy dog";

const crc32_testcase crc_test_cases[] = {
    {
        crc_data1,
        sizeof(crc_data1) - 1, // Subtract 1 to exclude null terminator
        2507325350,
    },
    {
        crc_data2,
        sizeof(crc_data2) - 1, // Subtract 1 to exclude null terminator
        1095738169,
    },
};

bool test_crc32() {
    bool result = true;

    for (u32 i = 0; i < ARRAY_SIZE(crc_test_cases); i++) {
        crc32_testcase test = crc_test_cases[i];
        const u32 hash = crc32buf(test.data, test.data_size);

        if (hash != test.hash) {
            printf("crc32buf: Hash calculation is wrong! [%u vs. %u]\n", hash, test.hash);
            result = false;
        }
    }

    REPORT_RESULT(result);
    return result;
}
