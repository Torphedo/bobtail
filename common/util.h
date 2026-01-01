#pragma once

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


#ifdef __cplusplus
    #define EXTERN_C_BEGIN extern "C" {
    #define EXTERN_C_END }
#else
    #define EXTERN_C_BEGIN
    #define EXTERN_C_END
#endif