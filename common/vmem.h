#ifndef VMEM_H
#define VMEM_H
#ifdef __cplusplus
extern "C" {
#endif
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
    // TODO: Add a function to get the page size and/or convert a size in bytes
    // to a number of pages. Everything's 4KB pages right now, but we really
    // ought to be asking the OS at runtime.
    VMEM_PAGE_SIZE = 4 * 1024,
    VMEM_ALLOC_GRANULARITY = 64 * 1024,
};

/// @brief Allocate a region of memory that can be watched for writes by the OS.
///
/// On Linux, any arbitrary region of pages can be watched. However, we're
/// locked into this allocation-based API because of Windows' implementation.
///
/// This function will try its best to use as little physical memory as
/// possible for the allocation, so your memory usage might not increase by the
/// allocated amount until you write to every page in the region at least once.
///
/// @param num_pages How many pages to allocate (size in bytes will be [num_pages] * @ref VMEM_PAGE_SIZE)
/// @return Address of the allocated region, or NULL on failure
void* vmem_alloc_watched(u64 num_pages);

/// @brief Find out which pages have changed (are "dirty") since the last time write-tracking was reset
///
/// This is essentially a more portable GetWriteWatch() API.
/// @param addr The base address of the region to check for changes in
/// @param num_pages The number of pages to check for changes
/// @param dirty_out A pointer to an array that receives dirty page pointers
/// @param dirty_out_size The size of the array to receive dirty page pointers
/// @param num_dirty_out Receives the number of dirty page pointers written to the output array
/// @return success/failure
bool vmem_get_dirty_pages(void* addr, u64 num_pages, void** dirty_out, u64 dirty_out_size, u64* num_dirty_out);

/// @brief Reset dirty page tracking for all pages in the current process.
///
/// Because of limitations on Linux, it's not possible to reset tracking for
/// just one region. You should check all of your monitored regions and ensure
/// no threads are about to write to them before calling this function,
/// otherwise you may miss a write.
// TODO: Is there even a practical way to clear all write watches for the
// process on Windows? Can we work around this on Linux and clear per region?
bool vmem_reset_write_watching();

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
// TODO: Add an option to make the entire region immediately usable, at the cost of reserving page file space
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

#ifdef __cplusplus
}
#endif
#endif // VMEM_H
