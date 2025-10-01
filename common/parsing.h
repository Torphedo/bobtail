#pragma once
#include <string.h>
#include "queue.h"

#include <common/int.h>

typedef struct {
    const char* data;
    u32 length;
}substr_t;

/// @brief Shatter text into a set of tokens
///
/// Whitespace is automatically removed.
/// @param text The string to tokenize
/// @param len The length of the string
/// @param char_ops All of the single-character operators that should form their own tokens
/// @param operators All of the multi-character operators that should form their own tokens
/// @param num_operators The number of multi-character operators
/// @return A queue of tokens
queue shatter_str(const char* text, s64 len, const char* char_ops, const char* const* operators, u32 num_operators);
