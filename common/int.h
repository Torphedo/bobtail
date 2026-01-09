#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;


/// @brief Dedicated integer type for GL object IDs.
///
/// This is the same type as GLuint, to avoid lots of linter warnings
typedef unsigned int gl_obj;

/// Useful when handling 24-bit integers not covered by the standard
enum {
    INT24_MAX = 0xFFFFFF
};

#ifdef __cplusplus
}
#endif
