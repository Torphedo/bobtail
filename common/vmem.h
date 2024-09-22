#ifndef VMEM_H
#define VMEM_H
/// @file vmem.h
/// @brief Cross-platform virtual memory utilities
/// @author Greenlord/S41L0R (Nintendo Switch implementation)
/// @author Torphedo (Windows & Unix implementations)
///
/// Unified API wrapping platform-specific virtual memory tricks, normally not
/// available to portable code.
/// Contains implementations for Windows, Unix, and Nintendo Switch. This
/// should also work on other Unix-based systems like macOS and FreeBSD.

#include "int.h"

enum {
    VMEM_PAGE_SIZE = 4 * 1024,
    VMEM_ALLOC_GRANULARITY = 64 * 1024,
};

/// @brief Create a special mapping that looks like a large linear buffer but acts like a tiny circular buffer
///
/// This allocates a single backing buffer, then uses virtual memory tricks to
/// create a number of "views" into it. It will look like the same backing
/// buffer was just copied many times, but if you write to any view, they all
/// reflect the change immediately (because they all point to the same physical
/// memory).<br>
/// A ring/repeat mapping can be useful to pass/return a buffer that can handle
/// reading/writing a lot of data, without actually using much memory. If
/// circular buffers are useful to you, they also let you use a circular buffer
/// on functions that only expect a linear buffer.
///
/// @param ring_width The size of the backing buffer as a multiple of 64KiB. On
/// POSIX it's possible to acheive 4KiB page-level granularity, but for
/// consistency it matches the Windows 64KiB granularity.
/// @param repeat_count How many "views" of the backing buffer to create.
///
/// @return Pointer to the start of the mapping. The usable size is
/// (@p ring_width * VMEM_ALLOC_GRANULARITY * @p repeat_count).
///
/// @note This function is not available on Nintendo Switch.
void* vmem_create_repeat_mapping(u32 ring_width, u32 repeat_count);

/// @brief Delete a repeat mapping
///
/// @note This function is not available on Nintendo Switch.
void vmem_destroy_repeat_mapping(void* base_addr, u32 ring_width, u32 repeat_count);



/// @brief Reserve a virtual memory region without committing any physical RAM.
///
/// Only address space is reserved, and no space is reserved in the page file.
/// Before using any part of the memory, commit all or part of the region with
/// vmem_commit(). You may be able to use the region without errors on some
/// systems, but this isn't portable or guaranteed.
/// @return Pointer to reserved region, or NULL on failure
// TODO: Look into writing a page fault handler on Windows to auto-commit reserved pages as needed
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
