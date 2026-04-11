#pragma once
#include "int.h"
#include <stdio.h>
#include <string.h>
#include <wchar.h>

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
