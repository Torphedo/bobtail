#include <common/logging.h>
#include <common/int.h>
#include <common/sha1.h>
#include <string.h>

#include "testing.h"

typedef struct {
    const char* data;
    u32 data_size;
    sha1_digest digest;
}sha1_testcase;

const char sha1_data1[] = "The answer to life, the universe, and everything";
const char sha1_data2[] = "The quick brown fox jumps over the lazy dog";

sha1_testcase sha1_test_cases[] = {
    {
        sha1_data1,
        sizeof(sha1_data1) - 1, // Subtract 1 to exclude null terminator
        {0x6F, 0xC7, 0xB0, 0xE0, 0x0B, 0x5A, 0x30, 0x2D, 0x11, 0x22, 0xB1, 0xC0, 0x25, 0x8E, 0xF4, 0xBB, 0x7B, 0x2F, 0x2A, 0x6F},
    },
    {
        sha1_data2,
        sizeof(sha1_data2) - 1, // Subtract 1 to exclude null terminator
        {0x2F, 0xD4, 0xE1, 0xC6, 0x7A, 0x2D, 0x28, 0xFC, 0xED, 0x84, 0x9E, 0xE1, 0xBB, 0x76, 0xE7, 0x39, 0x1B, 0x93, 0xEB, 0x12},
    },
};

bool test_sha1() {
    bool result = true;

    for (u32 i = 0; i < ARRAY_SIZE(sha1_test_cases); i++) {
        sha1_testcase test = sha1_test_cases[i];
        sha1_digest digest = SHA1_buf((u8*)test.data, test.data_size);
        const bool result_memcmp = memcmp(&test.digest, &digest, sizeof(test.digest)) == 0;
        const bool result_sha1cmp = SHA1_equal(test.digest, digest);

        if (!SHA1_equal(digest, digest)) {
            printf("SHA1_equal: something went wrong with the comparison function\n");
            result = false;
        }

        if (SHA1_blank(test.digest)) {
            printf("SHA1_blank: false positive on digest ");
            SHA1_print(test.digest);
            printf("!\n");
            result = false;
        }

        if (!SHA1_equal(digest, test.digest)) {
            printf("SHA1_buf: SHA1 calculation is wrong! [");
            SHA1_print(test.digest);
            printf(" vs. ");
            SHA1_print(digest);
            printf("]\n");
            result = false;
        }
    }

    sha1_digest empty = {0};
    if (!SHA1_blank(empty)) {
        printf("SHA1_blank: false negative!\n");
        result = false;
    }

    REPORT_RESULT(result);
    return result;
}
