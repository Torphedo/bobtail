#ifndef INT_H
#define INT_H
/// @file int.h

#include <stdint.h>

/// Shorthand for unsigned 8-bit integer
typedef uint8_t u8;
/// Shorthand for unsigned 16-bit integer
typedef uint16_t u16;
/// Shorthand for unsigned 32-bit integer
typedef uint32_t u32;
/// Shorthand for unsigned 64-bit integer
typedef uint64_t u64;

/// 16-bit character type for reading wchar_t strings built on Windows
typedef u16 c16;

/// Shorthand for signed 8-bit integer
typedef int8_t s8;
/// Shorthand for signed 16-bit integer
typedef int16_t s16;
/// Shorthand for signed 32-bit integer
typedef int32_t s32;
/// Shorthand for signed 64-bit integer
typedef int64_t s64;

/// @brief Dedicated integer type for GL object IDs.
///
/// This is the same type as GLuint, to avoid lots of linter warnings
typedef unsigned int gl_obj;


/// Useful when handling 24-bit integers not covered by the standard
enum {
    INT24_MAX = 0xFFFFFF
};

/// Get a value from a tightly packed bitmask like it's an array
u8 mask_get(u8* mask, u8 idx);

/// Set a value in a tightly packed bitmask like it's an array
void mask_set(u8* mask, u8 idx, u8 val);

s32 s24_to_s32(u8* addr);

/// Raise a number to a power (val^pow)
u32 exponent(u32 val, u8 pow);

/// Print a 16-bit string as if it were 8-bit (portable, unlike %ws)
void print_c16s(const c16* str);

/// Round a number up to any boundary
#define ALIGN_UP(x, bound) ((x) + ((bound) - ((x) % (bound))))

// sys/param.h defines these on some platforms, (included in platform.h)
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
#endif // INT_H
