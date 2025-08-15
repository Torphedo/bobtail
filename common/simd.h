#ifndef SIMD_H_
#define SIMD_H_
#include <stdbool.h>
#include "int.h"

#ifdef _MSC_VER
/// MSVC CPUID
#include <immintrin.h>
#define cpuid(info, x) __cpuidex(info, x, 0)
#define bit_SSE4_2 0x00100000
#define simd_crc32c_u8(crc, val) _mm_crc32_u8(crc, val)
#define simd_crc32c_u16(crc, val) _mm_crc32_u16(crc, val)
#define simd_crc32c_u32(crc, val) _mm_crc32_u32(crc, val)
#define simd_crc32c_u64(crc, val) _mm_crc32_u64(crc, val)
#else
//  GCC Intrinsics
#include <cpuid.h>
/// gcc / clang CPUID
#define cpuid(info, x) __cpuid_count(x, 0, info[0], info[1], info[2], info[3])
#define simd_crc32c_u8(crc, val) __builtin_ia32_crc32qi(crc, val)
#define simd_crc32c_u16(crc, val) __builtin_ia32_crc32hi(crc, val)
#define simd_crc32c_u32(crc, val) __builtin_ia32_crc32si(crc, val)
#define simd_crc32c_u64(crc, val) __builtin_ia32_crc32di(crc, val)
#endif

static bool sse4_available() {
    u32 info[4] = {0};
    cpuid(info, 1);
    u32 extended_features = info[3];
    return (extended_features & bit_SSE4_2) != 0;
}

#endif // SIMD_H_
