#include <common/logging.h>
#include <common/int.h>
#include <common/queue.h>
#include <common/parsing.h>

#include "testing.h"

typedef struct {
    const char* text;
    const char** tokens;
    const char* char_operators;
    const char** operators;
    u32 num_tokens;
    u32 num_ops;
}parse_test_case;

const parse_test_case cases[] = {
    {
        .text = "#include <stdio.h>\n\nint main() {\n printf(\"Hello world\");\n struct->field }\n",
        .tokens = (const char* []) {
            "#", "include", "<", "stdio", ".", "h", ">",
            "int", "main", "(", ")", "{",
            "printf", "(", "\"", "Hello", "world", "\"", ")", ";",
            "struct", "->", "field",
            "}",
        },
        .num_tokens = 24,
        .char_operators = "#()/*+-);{},<>.\"\"",
        .operators = (const char* []) {
            "->",
        },
        .num_ops = 1,
    },
    {
        .text = "a simpler example",
        .tokens = (const char* []) {
            "a", "simpler", "example",
        },
        .num_tokens = 3,
    },
};

bool test_parsing() {
    bool result = true;

    for (u32 k = 0; k < ARRAY_SIZE(cases); k++) {
        const parse_test_case c = cases[k];

        queue tokens = shatter_str(c.text, strlen(c.text), c.char_operators, c.operators, c.num_ops);
        u32 i = 0;
        while (!queue_empty(tokens)) {
            substr_t token = {0};
            queue_get(&tokens, &token);
            const char* expected_token = c.tokens[i++];
            if (token.length != strlen(expected_token)) {
                LOG_MSG(error, "Token length mismatch (expected %d, got %d)!\n", strlen(expected_token), token.length);
                result = false;
                break;
            }

            const char* actual_token = c.text + token.offset;
            if (strncmp(actual_token, expected_token, token.length) != 0) {
                LOG_MSG(error, "Token mismatch (expected '%s', got '%.*s')!\n", expected_token, token.length, actual_token);
                result = false;
                break;
            }
        }
    }

    REPORT_RESULT(result);
    return result;
}
