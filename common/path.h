#pragma once
#include "util.h"
EXTERN_C_BEGIN
/// @file path.h
/// @brief Utilities for working with filepaths

#include <stdbool.h>
#include "int.h"

/// @brief Check that a path has a file extension.
/// @param path Path to check
/// @param extension File extension to check for
///
/// The extension actually doesn't need to be a normal extension (with a ".").
/// All it actually does is check that the end of @p path matches @p extension.
bool path_has_extension(const char* path, const char* extension);

/// @brief Get the file extension from a path
/// @param path The input filepath
///
/// @return The last '.' in the path (outer extension), or an empty string if
/// there was no extension. The returned pointer is to the middle of the given
/// string, so its lifetime is tied to your string's lifetime.
const char* path_get_extension(const char* path);

/// @brief Replace all backslashes in a string with forward slashes.
/// @param path String to edit.
void path_fix_backslashes(char* path);

/// @brief Replace all forward slashes in a string with backslashes.
/// @param path String to edit.
void path_fix_forward_slashes(char* path);

/// @brief Truncate a filename or folder name from a path, leaving a trailing
/// "\\" or "/".
/// @param path The path to truncate (this string will be edited). If the string
/// does not contain any slashes or backslashes, it will be completely filled
/// with null characters.
/// @param pos The position to start searching for directory separators. This
/// should usually be the string's length + 1.
void path_truncate(char* path, u64 pos);

/// @brief Truncate a filename or folder name from a path, leaving a trailing
/// "\\" or "/". This variant creates a new string instead of modifying the input.
/// @param path The path to truncate. If the string does not contain any slashes
/// or backslashes, it will be completely filled with null characters.
const char* path_truncate_clone(const char* path);

/// Check if a path has any forward or backslashes
bool path_has_slashes(const char* path);

/// @brief Isolate just the filename component of a path
///
/// @p output should be at least as big as @p path.
/// @param path The path to get the filename from
/// @param output Where to write the filename component
void path_get_filename(const char* path, char* output);

/// @brief Find the parent directory of this executable.
///
/// @param argv_0 your argv[0] from main(). This is only really needed on
/// OpenBSD, but you should always provide it for consistency.
/// @note This allocates memory! Caller is responsible for freeing the output
/// string.
char* get_self_path(const char* argv_0);

EXTERN_C_END
#endif // #ifndef PATH_H