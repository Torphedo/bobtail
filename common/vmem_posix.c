// POSIX (Linux, BSD, Apple, generic Unix) implementation of vmem.h interface

#include "platform.h"

#ifdef PLATFORM_POSIX
#include <stdlib.h> // For NULL
#include "int.h"
#include "vmem.h"
#include <sys/mman.h>
#include <fcntl.h>

void* vmem_create_repeat_mapping(u32 ring_width, u32 repeat_count) {
    // To trick mmap() into mapping the same region to consecutive virtual
    // regions, we create a virtual (in-memory) file as a backing buffer.
    const char* vfile_name = "_repeatmap_vfile";
    // If we crashed during this function or something, the file could be left
    // behind in memory from a previous run, so we delete it first.
    shm_unlink(vfile_name);
    int ramfile = shm_open(vfile_name, O_RDWR | O_CREAT, 0);
    ftruncate(ramfile, VMEM_ALLOC_GRANULARITY * ring_width); // Extend the file to this size

    // Reserve enough virtual address space to hold the whole repeat mapping
    const u64 mapping_size = (u64)VMEM_ALLOC_GRANULARITY * (u64)ring_width * (u64)repeat_count;
    if (mapping_size == 0) {
        printf("Likely integer overflow or bad user input!\n");
    }
    void* mapbase = mmap(NULL, mapping_size, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (mapbase == NULL) {
        return NULL;
    }

    // Map the same virtual file multiple times into adjacent virtual pages,
    // so that writing to one mapping affects all of them
    for (u32 i = 0; i < repeat_count; i++) {
        const uintptr_t offset = (u64)i * (u64)VMEM_ALLOC_GRANULARITY * (u64)ring_width;
        void* cur_map_pos = (void*)((uintptr_t)mapbase + offset);
        void* mapping = mmap(cur_map_pos, ring_width * VMEM_ALLOC_GRANULARITY, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED, ramfile, 0);
        if (mapping == MAP_FAILED) {
            return NULL;
        }
    }

    // Once the file is mapped, we can delete it from the virtual filesystem
    // and our mappings aren't affected.
    shm_unlink(vfile_name);
    return mapbase;
}

void vmem_destroy_repeat_mapping(void* base_addr, u32 ring_width, u32 repeat_count) {
    u64 size = ring_width * VMEM_ALLOC_GRANULARITY * repeat_count;
    munmap(base_addr, size);
}

void* vmem_reserve(u64 size) {
    // MAP_ANONYMOUS tells it not to try to map a file into memory
    // MAP_NORESERVE tells it not to reserve space in the page file
    // (allows for larger-than-physical-memory reserved regions, of which only
    // small parts are used)
    void *retval = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
    if (retval == MAP_FAILED) {
        // We need all implementations to return NULL on error for consistency
        return NULL;
    }
    return retval;
}

int vmem_commit(void* addr, u64 size) {
    // The kernel will automatically commit physical memory as needed when we
    // write to the region. However, here we edit the existing mapping to let
    // it reserve space in the page file.
    void* retval = mmap(addr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (retval != MAP_FAILED) {
	    return 0;
    }
    return -1;
}

int vmem_free(void* addr, u64 size) {
    return munmap(addr, size);
}
#endif

