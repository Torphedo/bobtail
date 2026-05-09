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

s64 vfile_remaining(vfile file) {
    return MAX(0, (s64)file.size - (s64)file.pos);
}

u64 vfile_read_bytes(vfile* file, void* buf, u32 size) {
    const s64 size_remaining = vfile_remaining(*file);
    const u64 read_size = MIN(size, size_remaining);
    memcpy(buf, vfile_cur(*file), read_size);
    vfile_seek(file, read_size);

    return read_size;
}

u64 vfile_write_bytes(vfile* file, const void* buf, u32 size) {
    const s64 size_remaining = vfile_remaining(*file);
    const u64 write_size = MIN(size, size_remaining);
    memcpy(vfile_cur(*file), buf, write_size);
    vfile_seek(file, write_size);

    return write_size;
}

u64 vfile_transfer(vfile* in, vfile* out, u32 size) {
    const s64 remaining_out = vfile_remaining(*out);
    const u32 transfer_size = MIN(remaining_out, size);

    vfile_read_bytes(in, vfile_cur(*out), transfer_size);
    vfile_seek(out, transfer_size);
    return transfer_size;
}
