#include <common/logging.h>
#include <common/int.h>
#include <common/list.h>

#include "testing.h"
#include "queue.h"
#include "parsing.h"

const char* sample_text = "#include <stdio.h>\n\nint main() {\nprintf(\"Hello world\");\n}\n";
const char* sample_tokens[] = {
    "#", "include", "<", "stdio", ".", "h", ">",
    "int", "main", "(", ")", "{",
    "printf", "(", "\"", "Hello", "world", "\"", ")", ";",
    "}",
};
const char* reserved = WHITESPACE "#(/*+-);{},<>.\"\"";

bool test_parsing() {
    bool result = true;
    queue sample_tok_q = queue_create(ARRAY_SIZE(sample_tokens), sizeof(substr_t));
    for (u32 i = 0; i < ARRAY_SIZE(sample_tokens); i++) {
        const substr_t tok = {sample_tokens[i], strlen(sample_tokens[i])};
        queue_add(&sample_tok_q, &tok);
    }

    queue tokens = shatter_str(sample_text, strlen(sample_text), reserved);
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

    REPORT_RESULT(result);
    return result;
}