#include "vfile.h"

vfile vfile_open(void* ptr, u32 size) {
    return (vfile) { .ptr = (u8*)ptr, .pos = 0, .size = size};
}

bool vfile_eof(vfile file) {
    return (file.pos >= file.size);
}

bool vfile_opcheck(vfile* file, u32 writesize) {
    if (file->pos + writesize > file->size) {
        return false;
    }
    return true;
}

void vfile_seek(vfile* file, u32 size) {
    // Advance the file ptr, but don't let it go over the size.
    file->pos = MIN(file->size, file->pos + size);
}

void* vfile_cur(vfile file) {
    return (u8*)((uintptr_t)file.ptr + file.pos);
}
