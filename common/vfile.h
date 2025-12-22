#ifndef VFILE_H
#define VFILE_H
#ifdef __cplusplus
extern "C" {
#endif
/// @file vfile.h
/// @brief A stdio-style interface that can treat memory like a file stream

#include <stdbool.h>

#include "int.h"
#include "logging.h"

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

/// Check if a vfile has space for a read/write of some arbitrary size.
/// Automatically prints an error message on failure.
bool vfile_opcheck(vfile* file, u32 writesize);

/// Advance the file pointer by an amount
/// @param file The file to modify
/// @param size The number of bytes to advance
void vfile_seek(vfile* file, u32 size);

/// @brief Get the current position as a pointer
///
/// This basically just hides some pointer casts.
void* vfile_cur(vfile file);

/// @brief Read bytes into a buffer
/// @param file The file to read from
/// @param buf Buffer to copy data into
/// @param size The number of bytes to copy
/// @return The number of bytes that were copied
u64 vfile_read_bytes(vfile* file, void* buf, u32 size);

/// @brief Like @ref vfile_cur(), but advances [pos].
///
/// For example:
/// @code
/// vfile f = ...;
/// const u32 data = VFILE_READ(u32, f);
/// @endcode
///
/// @param T The data type to read
/// @param file a @ref vfile structure to read from
/// @return equivalent value to (T*)vfile_cur(file)
/// type @p T.
#define VFILE_READ_PTR(T, file) ((T*)(&(file)->ptr[((file)->pos += sizeof(T)) - sizeof(T)]))

/// @brief Read data from a virtual file.
///
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
#define VFILE_READ(T, file) (*VFILE_READ_PTR(T, file))

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
        if (vfile_opcheck(file, sizeof(T))) {             \
            *(T*)(&(file)->ptr[(file)->pos]) = (val);     \
            const u32 _newpos = (file)->pos + sizeof(T);  \
            (file)->pos = MIN(_newpos, (file)->size - 1); \
        } else {                                          \
            LOG_MSG(warning, "write @ 0x%x / 0x%x would be out of bounds [%d bytes]\n", (file)->pos, (file)->size, sizeof(T)); \
        }                                                 \
    } while (0)
// The MIN() keeps us from advancing past EOF
// The do-while loop forces you to use a semicolon when calling the macro

#ifdef __cplusplus
}
#endif
#endif // #ifndef VFILE_H
