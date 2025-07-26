#include <common/logging.h>
#include <common/int.h>
#include <common/crc32.h>

#include "testing.h"

typedef struct {
    const char* data;
    u32 data_size;
    u32 crc32_hash;
    u32 crc32c_hash;
}crc32_testcase;

const char crc_data1[] = "The answer to life, the universe, and everything";
const char crc_data2[] = "The quick brown fox jumps over the lazy dog";

const crc32_testcase crc_test_cases[] = {
    {
        crc_data1,
        sizeof(crc_data1) - 1, // Subtract 1 to exclude null terminator
        2507325350,
        0xE1BFE209
    },
    {
        crc_data2,
        sizeof(crc_data2) - 1, // Subtract 1 to exclude null terminator
        1095738169,
        0x22620404
    },
};

bool test_crc32() {
    bool result = true;

    for (u32 i = 0; i < ARRAY_SIZE(crc_test_cases); i++) {
        crc32_testcase test = crc_test_cases[i];
        const u32 hash = crc32buf((u8*)test.data, test.data_size);
        const u32 crc32c_sse_hash = sse_crc32c((u8*)test.data, test.data_size);
        const u32 crc32c_software_hash = software_crc32c((u8*)test.data, test.data_size);

        if (hash != test.crc32_hash) {
            printf("crc32buf: Hash calculation is wrong! [%u vs. %u]\n", hash, test.crc32_hash);
            result = false;
        }

        if (crc32c_sse_hash != crc32c_software_hash) {
            printf("crc32cbuf: Software and SSE results differ! [%u vs. %u]\n", crc32c_software_hash, crc32c_sse_hash);
            result = false;
        }

        if (crc32c_sse_hash != test.crc32c_hash) {
            printf("crc32cbuf: SSE hash is wrong! [%u vs. %u]\n", crc32c_sse_hash, test.crc32c_hash);
            result = false;
        }

        if (crc32c_software_hash != test.crc32c_hash) {
            printf("crc32cbuf: Software hash is wrong! [%u vs. %u]\n", crc32c_software_hash, test.crc32c_hash);
            result = false;
        }
    }

    REPORT_RESULT(result);
    return result;
}
