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

/// Round a number down to any boundary
#define ALIGN_DOWN(x, bound) ((x) - ((x) % (bound)))

/// Round a number up to any boundary
#define ALIGN_UP(x, bound) ((x) + ((bound) - ((x) % (bound))))

/// @brief Create a 32-bit "magic number" from 4 bytes (usually ASCII)
/// This is useful for parsing file formats, where files are often identified
/// by 4 ASCII bytes like 'DDS ' or 'SARC' or 'IWAD'
#define MAGIC(a, b, c, d) ((u32)a | ((u32)b << 8) | ((u32)c << 16) | ((u32)d << 24))

#ifdef __cplusplus
}
#endif
