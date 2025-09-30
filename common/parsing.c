#include "parsing.h"
#include <ctype.h>

queue shatter_str(const char* text, s64 len, const char* reserved_chars) {
    // Remove trailing whitespace
    while (isspace(text[MAX(0, len - 1)]) && len > 0) {
        len--;
    }

    // Skip leading whitespace
    while (isspace(*text) && len > 0) {
        text++;
        len--;
    }

    queue out = queue_create(5, sizeof(substr_t));
    s32 last_token_end = 0;
    for (s32 i = 0; i < len; i++) {
        const u32 prev_pos = MAX(0, i - 1);
        const char cur_ch = text[i];
        const char prev_ch = text[prev_pos];

        const bool prev_reserved = strchr(reserved_chars, prev_ch);
        const bool prev_space = isspace(prev_ch);
        const bool cur_reserved = strchr(reserved_chars, cur_ch);
        const bool cur_space = isspace(cur_ch);
        bool is_token_end = false;
        if (prev_space) {
            last_token_end++; // Skip spaces
        }
        if (prev_reserved || cur_reserved || cur_space) {
            is_token_end = true; // End token when we hit reserved char
        }

        const char* token_begin = &text[last_token_end];
        const s32 token_len = i - last_token_end;
        if (is_token_end && token_len > 0) {
            const substr_t tok = {.data = token_begin, .length = token_len};
            queue_add(&out, &tok);
            last_token_end = i;
        }
    }

    // Make the rest of the string a token
    const u32 token_len = MAX(0, len - last_token_end);
    const char* token_begin = &text[last_token_end];
    if (token_len > 0) {
        const substr_t tok = {.data = token_begin, .length = token_len};
        queue_add(&out, &tok);
    }

    return out;
}
