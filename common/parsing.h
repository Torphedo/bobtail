#pragma once
#include <string.h>
#include "queue.h"

#include <common/int.h>

typedef struct {
    const char* data;
    u32 length;
}substr_t;

#define WHITESPACE "\t \n\v"

// Shatter text into a set of tokens
queue shatter_str(const char* text, s64 len, const char* reserved_chars);
