#pragma once
#include "util.h"
EXTERN_C_BEGIN
/// @file arguments.h
/// Very basic argument parsing, mostly to extract input and output filepaths.

#include <stdbool.h>

#include "int.h"

/// @struct flags
/// @brief Stores basic settings parsed from argument list.

/// If "--silent" is found, the @ref silent flag is set and logging via
/// @ref LOG_MSG is disabled.
///
/// Arguments that don't match any user-specified arguments (see
/// @ref parse_arguments) are assumed to be a filepath, and stored in
/// @ref input_path or @ref output_path.
/// When there are more than 2 paths found, the last 2 are saved as the input
/// and output paths. The input comes before the output on the command-line,
/// and it doesn't matter what arguments come before or after.
///
/// If there's only 1 path found, it's assumed to be the input.
typedef struct {
    /// The input filepath (may be NULL)
    char* input_path;

    /// The input filepath (may be NULL)
    char* output_path;

    /// @brief User-specified program mode.
    ///
    /// This is the index in the array of user-specified arguments that was
    /// seen last. This is intended for mutually-exclusive mode switches.
    u32 mode;

    /// @brief Whether the program has been set to silent mode (disables all
    /// output from @ref LOG_MSG).
    bool silent;
}flags;

/// Parses arguments from main() and returns a simple settings structure.
/// @param argc argc forwarded from main()
/// @param argv argv forwarded from main()
/// @param args an array of mutually exclusive flags to search for. See the
///             documentation on the return value for more details.
/// @param args_count size of the @p args array
///
/// @return Returns a @ref flags structure
flags parse_arguments(int argc, char** argv, const char* args[], u32 args_count);

/// @brief Parse Linux-style options with values
///
/// For example, if you want to specify a filepath, you might have an option
/// named "--path", with "-p" as a shorthand. This function will handle:
///   --path /path/to/file
///   --path "/path/to/file"
///   -p /path/to/file
///   -p "/path/to/file"
/// @param argc Your argc
/// @param argv Your argv
/// @param option Long form of your option name
/// @param shorthand Short form of your option name
/// @return String value, or NULL if not found
char* command_getoption(int argc, char** argv, const char* option, const char* shorthand);

/// @brief Parse Linux-style boolean flags
///
/// For example, you might have an option named "--skip", with "-s" as a
/// shorthand. This function will succeed if it finds either.
/// @param argc Your argc
/// @param argv Your argv
/// @param option Long form of your option name
/// @param shorthand Short form of your option name
/// @return Whether the flag was present
bool command_getflag(int argc, char** argv, const char* option, const char* shorthand);

/// @brief Get the argument at a specific index from the back.
///
/// e.g. idx 0 is the last argument, idx 1 is the second to last, etc.
/// @param argc Your argc
/// @param argv Your argv
/// @param idx The number of indices from the back
/// @return Argument text
char* args_get_from_back(int argc, char** argv, unsigned int idx);

EXTERN_C_END
