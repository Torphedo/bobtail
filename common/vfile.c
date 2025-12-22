#include "vfile.h"
#include <string.h>

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

u64 vfile_read_bytes(vfile* file, void* buf, u32 size) {
    const s64 size_remaining = MAX(0, (s64)file->size - (s64)file->pos);
    const u64 read_size = MIN(size, size_remaining);
    memcpy(buf, vfile_cur(*file), read_size);
    vfile_seek(file, read_size);

    return read_size;
}
