#ifndef VMEM_H
#define VMEM_H
/// @file vmem.h
/// @brief Cross-platform virtual memory utilities
/// @author Greenlord/S41L0R (Nintendo Switch implementation)
/// @author Torphedo (Windows & Unix implementations)
///
/// Lets you reserve virtual address space without committing physical memory.
/// Contains implementations for Windows, Unix, and Nintendo Switch. This should
/// also work on other Unix-based systems like macOS and FreeBSD.

#include "int.h"

enum {
    VMEM_PAGE_SIZE = 4 * 1024,
    VMEM_ALLOC_GRANULARITY = 64 * 1024,
};

void* vmem_create_repeat_mapping(u32 ring_width, u32 repeat_count);

void vmem_destroy_repeat_mapping(void* base_addr, u32 ring_width, u32 repeat_count);

/// @brief Reserve a virtual memory region without committing any physical RAM.
///
/// Only address space is reserved, and no space is reserved in the page table.
/// Before using any part of the memory, commit all or part of the region with
/// vmem_commit(). You may be able to use the region without errors on some
/// systems, but this isn't portable or guaranteed.
/// @return Pointer to reserved region, or NULL on failure
void* vmem_reserve(u64 size);

/// @brief Commit physical memory to a virtual memory region.
///
/// If successful, memory in the region becomes usable and space is reserved in
/// the page file. Actual physical pages are only allocated as needed when
/// parts of the committed region are accessed.
/// @return 0 on success, -1 on failure.
int vmem_commit(void* addr, u64 size);

/// @brief Free a virtual memory region reserved with @ref vmem_reserve().
///
/// This also frees physical memory committed to that region.
/// @note Something I noticed in testing is that it can crash if you
/// accidentally free a region larger than you reserved. When run under a
/// debugger, it was fine. If you see this happen, make sure your sizes match!
/// @return 0 on success, -1 on failure.
int vmem_free(void* addr, u64 size);

#endif // VMEM_H
