#ifndef PATH_H
#define PATH_H
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
void path_truncate(char* path, u16 pos);

/// Check if a path has any forward or backslashes
bool path_has_slashes(const char* path);

/// @brief Isolate just the filename component of a path
///
/// To be safe, @p output should be the same size as @p path.
/// @param path The path to get the filename from
/// @param output Where to write the filename component
void path_get_filename(const char* path, char* output);

/// @brief Find the parent directory of this executable.
///
/// @param argv_0 your argv[0] from main(). This is only really needed on
/// OpenBSD, but you should provide it for consistency.
/// @note This allocates memory! Caller is responsible for freeing the output
/// string.
char* get_self_path(const char* argv_0);

#endif // #ifndef PATH_H
