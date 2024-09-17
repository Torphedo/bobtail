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

/// @brief Reserve virtual memory without committing any physical RAM.
///
/// @return Pointer to reserved region, or NULL on failure
void* vmem_reserve(u64 size);

/// @brief Commit physical memory to a virtual memory region. 
///
/// This does nothing on all platforms except Nintendo Switch, because writing
/// to a reserved region automatically commits physical memory as needed.
/// @return 0 on success, -1 on failure.
int vmem_commit(void* addr, u64 size);

/// @brief Free a virtual memory region reserved with @ref vmem_reserve().
///
/// This also frees physical memory committed to that region.
/// @return 0 on success, -1 on failure.
int vmem_free(void* addr, u64 size);

#endif // VMEM_H
