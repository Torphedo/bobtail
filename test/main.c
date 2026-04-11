#include <stdbool.h>

#include <common/int.h>
#include <common/logging.h>

bool test_crc32();
bool test_queue();
bool test_list();
bool test_hashmap();
bool test_hashmap_cpp();
bool test_vmem();
bool test_sha1();
bool test_parsing();

typedef bool (*testproc)();
testproc tests[] = {
    test_crc32,
    test_queue,
    test_list,
    test_hashmap,
    test_hashmap_cpp,
    test_vmem,
    test_sha1,
    test_parsing,
};

int main() {
    enable_win_ansi();

    LOG_MSG(info, "Running %d tests\n", ARRAY_SIZE(tests));
    u32 passed_count = 0;
    for (u32 i = 0; i < ARRAY_SIZE(tests); i++) {
        passed_count += tests[i]();
    }

    float passed_ratio = ((float)passed_count / (float)ARRAY_SIZE(tests)) * 100;
    LOG_MSG(info, "%.1f%% passed.\n", passed_ratio);

    // Communicate overall pass/fail via process exit code
    if (passed_count != ARRAY_SIZE(tests)) {
        return 1;
    }
    return 0;
}
