#ifndef VFILE_H
#define VFILE_H
/// @file vfile.h
/// @brief A stdio-style interface that can treat memory like a file stream

#include <stdbool.h>

#include "int.h"

/// Virtual file context/state (like stdio FILE*)
typedef struct {
    /// The start of the memory region
    u8* ptr;
    /// Current offset from the base pointer
    uintptr_t pos;
    /// Size of the memory region
    u32 size;
}vfile;

/// @brief Create a virtual file from a sized memory region
/// @param ptr The start of the memory region to treat as a virtual file
/// @param size The size of the memory region
vfile vfile_open(void* ptr, u32 size);

/// Whether this file has reached the end of the allowed memory region
bool vfile_eof(vfile file);

/// Check if a vfile has space for a write of some arbitrary size. Automatically 
/// prints an error message on failure.
bool vfile_writecheck(vfile* file, u32 writesize);

/// @brief Read data from a virtual file.
///
/// Any type that can be pointer-dereferenced works (should support structs)
/// For example:
/// @code
/// vfile f = ...;
/// const u32 data = VFILE_READ(u32, f);
/// @endcode
///
/// @param T The data type to read
/// @param file a @ref vfile structure to read from
/// @return The requested data is returned as if this was a function of return
/// type @p T.
// TODO: Use MIN() here to avoid reading out of bounds
#define VFILE_READ(T, file) (*((T)*)(&(file)->ptr[((file)->pos += sizeof(T)) - sizeof(T)]))

/// @brief Write data to a virtual file.
///
/// For example:
/// @code
/// vfile f = ...;
/// const u32 data = 10;
/// VFILE_WRITE(u32, f, data); // Write a 32-bit variable
/// VFILE_WRITE(u64, f, 20); // Write a literal as an 8-byte int
/// @endcode
///
/// @param T The data type to write to memory. Any type can be used, as long as
/// it can be written to memory with a pointer dereference.
/// @param file a @ref vfile structure to write to
/// @param val value to write
// TODO: If we ever get C23, use typeof() so it can just be VFILE_WRITE(file, val)
#define VFILE_WRITE(T, file, val)                         \
    do {                                                  \
        if (vfile_writecheck(file, sizeof(T))) {          \
            *((T)*)(&(file)->ptr[(file)->pos]) = (val);   \
            const u32 _newpos = (file)->pos + sizeof(T);  \
            (file)->pos = MIN(_newpos, (file)->size - 1); \
        }                                                 \
    } while (0)
// The MIN() keeps us from advancing past EOF
// The do-while loop forces you to use a semicolon when calling the macro

#endif // #ifndef VFILE_H
