#pragma once
// Cross-platform virtual memory functions for reserving address space and
// committing physical memory. Contains implementations for Windows and Unix.
// This should also work on other Unix-based systems like macOS and FreeBSD.

#include "int.h"

// Reserve virtual memory without committing any physical RAM.
void* virtual_reserve(u64 size);
// Commit physical memory to virtual memory starting at a specified position.
// Returns 0 on success, -1 on failure.
int virtual_commit(void* addr, u64 size);
// Free a virtual memory buffer you reserved with virtual_reserve(). This also
// frees physical memory committed to the freed virtual memory.
int virtual_free(void* addr, u64 size);

