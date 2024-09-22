#ifndef FILE_H
#define FILE_H
/// @file file.h
/// @brief Utilities for dealing with the filesystem

#include <stdbool.h>

#include "int.h"

/// Check if a path exists (doesn't necessarily mean it's a file)
bool file_exists(const char* path);

/// Check if a path exists and is a file
bool path_is_file(const char* path);

/// Check if a path exists and is a directory
bool path_is_dir(const char* path);

/// @brief Find the size of a file
/// @return Size of the file. A value of 0 could mean the path doesn't exist,
/// or that the file is empty.
u32 file_size(const char* path);

/// @brief Read an entire file into a buffer. Caller must free the resource.
/// @param path Filepath
/// @note This allocates memory!
/// @return Pointer to buffer, or NULL on failure.
u8* file_load(const char* path);

/// @brief Read an entire file into an existing buffer.
/// @param path Filepath
/// @param buf Buffer to read file into
/// @param size Size of buffer
/// @return Returns false if anything goes wrong (can't open file, buffer is
/// NULL, not enough space)
bool file_load_existing(const char* path, u8* buf, u32 size);

/// @brief Create a 32-bit "magic number" from 4 bytes (usually ASCII)
/// This is useful for parsing file formats, where files are often identified
/// by 4 ASCII bytes like 'DDS ' or 'SARC' or 'IWAD'
static inline u32 magic(char a, char b, char c, char d) {
    return a | (b << 8) | (c << 16) | (d << 24);
}

/// Same as @ref magic(), but can be used in constants (good for enums)
#define MAGIC(a, b, c, d) ((u32)a | ((u32)b << 8) | ((u32)c << 16) | ((u32)d << 24))

#endif // #ifndef FILE_H
