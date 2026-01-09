#pragma once
#include "int.h"
#include <stdio.h>
#include <string.h>
#include <wchar.h>

/// Round a number down to any boundary
#define ALIGN_DOWN(x, bound) ((x) - ((x) % (bound)))

/// Round a number up to any boundary
#define ALIGN_UP(x, bound) ((x) + ((bound) - ((x) % (bound))))

// sys/param.h defines these on some platforms
#ifndef MAX
    /// Return the larger of 2 values
    #define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
    /// Return the smaller of 2 values
    #define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

/// Returns low or high bound if @p val is out of bounds, otherwise return @p val
#define CLAMP(low, val, high) (((val) < (low)) ? (low) : MIN((val), (high)))

/// Can only be used on arrays with compile-time known sizes
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*(arr)))


/// 16-bit character type for reading wchar_t strings built on Windows
typedef u16 c16;

/// Print a 16-bit string as if it were 8-bit (portable, unlike %ws)
static void print_c16s(const c16* str) {
    while (*str != 0) {
        wchar_t c = *str;
        printf("%lc", c);
        str++;
    }
}

static s32 s24_to_s32(u8* addr) {
    s32 val = 0;
    memcpy(&val, addr, 3);
    return val;
}

static u64 exponent(u32 val, u8 pow) {
    u64 out = val;
    for (u8 i = 1; i < pow; i++) {
        out *= val;
    }
    return out;
}

static float reflect(float val, float axis) {
    const float distance = axis - val;

    // Go towards the axis but "overshoot" by 2x, reflecting the value.
    return val + (distance * 2.0f);
}

#ifdef __cplusplus
    #define EXTERN_C_BEGIN extern "C" {
    #define EXTERN_C_END }
#else
    #define EXTERN_C_BEGIN
    #define EXTERN_C_END
#endif
