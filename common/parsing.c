#include "parsing.h"
#include <ctype.h>
#include <assert.h>

const char* blank_ops[] = {0};

queue shatter_str(const char* text, s64 len, const char* char_ops, const char* const* operators, u32 num_operators) {
    if (!char_ops) {
        char_ops = "";
    }
    if (!operators) {
        operators = blank_ops;
        num_operators = 0;
    }

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
        const s32 prev_pos = MAX(0, i - 1);
        const char cur_ch = text[i];
        const char prev_ch = text[prev_pos];

        for (u32 j = 0; j < num_operators; j++) {
            const char* op = operators[j];
            assert(strlen(op) < INT16_MAX && "What on earth are you doing over there?");
            if (strncmp(&text[prev_pos], op, strlen(op)) == 0) {
                last_token_end = prev_pos;
                i += (s32)strlen(op) - 1;
                break;
            }
        }

        const bool prev_op = strchr(char_ops, prev_ch);
        const bool cur_op = strchr(char_ops, cur_ch);
        const bool prev_space = isspace(prev_ch);
        const bool cur_space = isspace(cur_ch);
        bool is_token_end = false;
        if (prev_space) {
            last_token_end++; // Skip spaces
        }
        if (prev_op || cur_op || cur_space) {
            is_token_end = true; // End token when we hit reserved char
        }

        const s32 token_len = i - last_token_end;
        if (is_token_end && token_len > 0) {
            const substr_t tok = {.offset = last_token_end, .length = token_len};
            queue_add(&out, &tok);
            last_token_end = i;
        }
    }

    // Make the rest of the string a token
    const u32 token_len = MAX(0, len - last_token_end);
    if (token_len > 0) {
        const substr_t tok = {.offset = last_token_end, .length = token_len};
        queue_add(&out, &tok);
    }

    return out;
}
