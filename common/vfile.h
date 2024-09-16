#ifndef VFILE_H
#define VFILE_H

// stdio-style API for writing to memory
#include <stdbool.h>

#include "int.h"

typedef struct {
    u8* ptr;
    uintptr_t pos;
    u32 size;
}vfile;

vfile vfile_open(void* ptr, u32 size);
bool vfile_eof(vfile* file);

// Check if a vfile has space for a write of some arbitrary size. Automatically 
// prints an error message on failure.
bool vfile_writecheck(vfile* file, u32 writesize);

// Read data from a virtual file. Result is returned as if this was a function.
// Any type that can be pointer-dereferenced works (should support structs)
#define VFILE_READ(T, file) (*(T*)(&file->ptr[(file->pos += sizeof(T)) - sizeof(T)]))

// Write data to a virtual file. Data type must be provided, because literals
// will have an uncertain size. Any type can be used, as long as it can be
// written to memory with a pointer dereference.
#define VFILE_WRITE(T, file, val)                \
    do {                                         \
        if (vfile_writecheck(file, sizeof(T))) { \
            *(T*)(&file->ptr[file->pos]) = val;  \
            file->pos += sizeof(T);              \
        }                                        \
    } while (false)

#endif // #ifndef VFILE_H

