#include <common/logging.h>
#include <common/int.h>

#include "testing.h"
#include "queue.h"
#include "parsing.h"

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
        .char_operators = WHITESPACE "#()/*+-);{},<>.\"\"",
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
        .char_operators = WHITESPACE,
    },
};

bool test_parsing() {
    bool result = true;

    for (u32 k = 0; k < ARRAY_SIZE(cases); k++) {
        const parse_test_case c = cases[k];

        queue sample_tok_q = queue_create(c.num_tokens, sizeof(substr_t));
        for (u32 i = 0; i < c.num_tokens; i++) {
            const substr_t tok = {c.tokens[i], strlen(c.tokens[i])};
            queue_add(&sample_tok_q, &tok);
        }

        queue tokens = shatter_str(c.text, strlen(c.text), c.char_operators, c.operators, c.num_ops);
        while (!queue_empty(tokens)) {
            substr_t token = {0};
            substr_t sample_token = {0};
            queue_get(&tokens, &token);
            queue_get(&sample_tok_q, &sample_token);
            if (token.length != sample_token.length) {
                LOG_MSG(error, "Token length mismatch!\n");
                result = false;
                break;
            }

            if (strncmp(token.data, sample_token.data, token.length) != 0) {
                LOG_MSG(error, "Token mismatch!\n");
                result = false;
                break;
            }
        }
    }

    REPORT_RESULT(result);
    return result;
}