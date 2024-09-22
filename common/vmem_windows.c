// Windows implementation of vmem.h interface

#include "platform.h"

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#include <stdlib.h> // For NULL
#include "int.h"
#include "vmem.h"

// Non-thread-safe version that may have to be retried a few times if a race
// condition causes a mapping to fail. This is the only option if
// VirtualAlloc2() and MapViewOfFile3() aren't available. Fabian Giesen's post
// has more details:
// https://fgiesen.wordpress.com/2012/07/21/the-magic-ring-buffer/
void* repeat_mapping_fallback(u32 ring_width, u32 repeat_count) {
    const u64 ring_size = ring_width * VMEM_ALLOC_GRANULARITY;
    const u64 mapping_size = ring_size * repeat_count;
    // Create initial file mapping, backed only by the page file
    HANDLE file_mapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, mapping_size << 32, mapping_size & UINT32_MAX, NULL);
    if (file_mapping == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    // We use VirtualAlloc to find a usable base address that's both aligned to
    // the allocation granularity, and has enough unused address space to hold
    // our mapping
    void* base_addr = VirtualAlloc(NULL, mapping_size, MEM_RESERVE, PAGE_NOACCESS);
    if (base_addr == NULL) {
        return NULL;
    }
    VirtualFree(base_addr, 0, MEM_RELEASE);
    // Everything after this free call is a potential race condition

    for (u32 i = 0; i < repeat_count; i++) {
        const uintptr_t offset = ring_size * i;
        void* target_region = (void*)((uintptr_t)base_addr + offset);

        const void* mapping = MapViewOfFileEx(file_mapping, FILE_MAP_ALL_ACCESS, 0, 0, ring_size, target_region);
        if (mapping == NULL) {
            // Clean up
            vmem_destroy_repeat_mapping(base_addr, ring_width, repeat_count);
            CloseHandle(file_mapping);
            return NULL;
        }
    }

    // File has been mapped, return base pointer
    return base_addr;
}

void* vmem_create_repeat_mapping(u32 ring_width, u32 repeat_count) {
    u8 num_attempts = 42;
    void* addr = NULL;
    // Introducing our (ryg's) innovative solution to rare race conditions:
    // Just try again until it works!
    while (addr == NULL && num_attempts > 0) {
        num_attempts--;
        // TODO: Use the thread-safe, guaranteed method shown by Casey Muratori.
        // After all, it's not much of a "fallback" if it's the only method :P
        // https://www.computerenhance.com/p/powerful-page-mapping-techniques
        addr = repeat_mapping_fallback(ring_width, repeat_count);
    }
    return addr;
}

void vmem_destroy_repeat_mapping(void* base_addr, u32 ring_width, u32 repeat_count) {
    // If this is called to clean up after a mapping failure, a lot of these
    // calls will fail because those regions were never mapped. But that's "ok"
    // since it's not a fatal error.
    const u64 ring_size = ring_width * VMEM_ALLOC_GRANULARITY;
    for (u32 j = 0; j < repeat_count; j++) {
        const uintptr_t unmap_offset = ring_size * j;
        void* unmap_region = (void*) ((uintptr_t) base_addr + unmap_offset);
        UnmapViewOfFile(unmap_region);
    }
    // TODO: What do we do about the leftover mapped file? OS-specific context
    // struct? Maybe a 64-bit "mapping metadata" field to keep it around without
    // making people think the mapping goes to disk?
    // CloseHandle(file_mapping);
}

// This API mimics VirtualAlloc() so it's a thin wrapper, not much to say here.
void* vmem_reserve(u64 size) {
    return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
}

int vmem_commit(void* addr, u64 size) {
    if (VirtualAlloc(addr, size, MEM_COMMIT, PAGE_READWRITE) == NULL) {
	    return -1; // Failure :(
    }
    return 0;
}

int vmem_free(void* addr, u64 size) {
    if (VirtualFree(addr, 0, MEM_RELEASE)) {
        return 0;
    }
    return -1;
}
#endif
