// Windows implementation of vmem.h interface

#include "platform.h"

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#include <stdlib.h> // For NULL
#include "int.h"

void* vmem_reserve(u64 size) {
    return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
}

int vmem_commit(void* addr, u64 size) {
    // We reserve with MEM_RESERVE | MEM_COMMIT, so Windows will automatically
    // commit physical memory as needed when we write to the memory.
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

