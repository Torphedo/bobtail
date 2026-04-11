#include <string.h>
#include <assert.h>

#include "logging.h"
#include "int.h"
#include "arguments.h"

flags parse_arguments(int argc, char** argv, const char* args[], u32 args_count) {
    flags output = {
        .mode = 0 // Default mode is the first option
    };

    // Use default when no arguments are provided
    if (argc == 1) {
        return output;
    }

    // Loop over every argument, skipping the first which is just our program
    // name.
    for (s32 i = 1; i < argc; i++) {
        // Check if first 2 characters are "--"
        if (argv[i][0] != '-' || argv[i][1] != '-') {
            // If it's not a flag, it must be a filename
            if (output.input_path != NULL) {
                if (output.output_path != NULL) {
                    output.input_path = output.output_path;
                }
                output.output_path = argv[i];
            } else {
                output.input_path = argv[i];
            }

            continue;
        }

        // Disable all output on silent mode
        if (strcmp(argv[i], "--silent") == 0) {
            logging_enabled = false;
            continue;
        }

        // Check for flags the caller asked us to look for
        for (u32 j = 0; j < args_count; j++) {
            if (strcmp(argv[i], args[j]) == 0) {
                output.mode = j;
                break;
            }
        }
    }

    return output;
}

char* args_getoption(int argc, char** argv, const char* option, const char* shorthand) {
    // Make our pointers always non-NULL
    option = (option) ? option : "";
    shorthand = (shorthand) ? shorthand : "";

    char* result = NULL;
    bool hit = false;
    for (u32 i = 0; i < argc; i++) {
        char* arg = argv[i];
        if (hit) {
            // Last arg was the flag, this must be the value
            result = arg;
            break;
        }

        // Skip up to 2 dashes to handle long and short hand flags
        for (u32 j = 0; j < 2; j++) {
            if (*arg == '-') {
                arg++;
            }
        }
        hit = (strcmp(arg, option) == 0) || (strcmp(arg, shorthand) == 0);
    }

    return result;
}

bool args_getflag(int argc, char** argv, const char* option, const char* shorthand) {
    // Make our pointers always non-NULL
    option = (option) ? option : "";
    shorthand = (shorthand) ? shorthand : "";

    char* result = NULL;
    for (u32 i = 0; i < argc; i++) {
        char* arg = argv[i];

        // Skip up to 2 dashes to handle long and short hand flags
        for (u32 j = 0; j < 2; j++) {
            if (*arg == '-') {
                arg++;
            }
        }
        bool hit = (strcmp(arg, option) == 0) || (strcmp(arg, shorthand) == 0);
        if (hit) {
            return true;
        }
    }

    return false;
}

char* args_get_from_back(int argc, char** argv, unsigned int idx) {
    const int i = MAX(0, argc - 1 - (int)idx);

    assert(i >= 0);
    assert(i < argc);
    return argv[i];
}
