#ifndef SIMD_H_
#define SIMD_H_
#include <stdbool.h>
#include "int.h"

#ifdef _MSC_VER
/// MSVC CPUID
#define cpuid(info, x) __cpuidex(info, x, 0)
#else
//  GCC Intrinsics
#include <cpuid.h>
/// gcc / clang CPUID
#define cpuid(info, x) __cpuid_count(x, 0, info[0], info[1], info[2], info[3])
#endif

static bool sse4_available() {
    u32 info[4] = {0};
    cpuid(info, 1);
    u32 extended_features = info[3];
    return (extended_features & bit_SSE4_2) != 0;
}

#endif // SIMD_H_
