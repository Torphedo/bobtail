// POSIX (Linux, BSD, Apple, generic Unix) implementation of vmem.h interface

#include "platform.h"

#if defined(PLATFORM_POSIX)
#include <stdlib.h> // For NULL
#include <stdio.h>
#include <stdbool.h>

#include <sys/mman.h>
#include <sys/syscall.h>
#include <linux/userfaultfd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/fs.h>

#include "int.h"
#include "vmem.h"
#include "file.h"
#include "logging.h"


// Write-tracking isn't part of POSIX, we have to rely on more OS-specific APIs
#ifdef PLATFORM_LINUX

enum {
    // https://www.man7.org/linux/man-pages/man5/proc_pid_pagemap.5.html
    PAGE_FLAG_UFFD_WRITE_PROTECTED = 57,
};

#define GET_BIT(x, y) ((x & ((uint64_t)1 << y)) >> y)

static int user_fault_fd = -1;

int vmem_get_uffd() {
    if (user_fault_fd >= 0) {
        return user_fault_fd;
    }

    user_fault_fd = syscall(SYS_userfaultfd, UFFD_USER_MODE_ONLY);
    if (user_fault_fd < 0) {
        return user_fault_fd;
    }

    struct uffdio_api api = {
        .api = UFFD_API,
        .features = UFFD_FEATURE_WP_ASYNC,
    };
    ioctl(user_fault_fd, UFFDIO_API, &api);
    if (!(api.features & UFFD_FEATURE_WP_ASYNC)) {
        LOG_MSG(warning, "Write tracking is not automatically handled by the kernel, there may be problems!\n");
    }

    return user_fault_fd;
}

bool vmem_reset_write_watching(const void* buf, u64 size) {
    const uintptr_t end = (uintptr_t)buf + size;

    int f = open("/proc/self/pagemap", O_RDONLY);
    if (f < 0) {
        return false;
    }

    // Write-protect pages that have been written to
    struct pm_scan_arg args = {
        .start = (uintptr_t)buf,
        .size = sizeof(args),
        .end = end,
        .flags = PM_SCAN_WP_MATCHING,
        .category_mask = PAGE_IS_WRITTEN,
    };
    if (ioctl(f, PAGEMAP_SCAN, &args) != 0) {
        return false;
    }

    close(f);
    return true;
}

void* vmem_alloc_watched(u64 num_pages) {
    // Since we can get dirty state of any page on Linux and reserved memory is
    // immediately usable, there's nothing special to do here. This function
    // exists just for consistency in user code.
    const int uffd = vmem_get_uffd();
    if (uffd < 0) {
        return NULL;
    }

    const u64 size = num_pages * VMEM_PAGE_SIZE;
    void* buf = vmem_reserve(size);
    if (!buf) {
        return buf;
    }

    struct uffdio_register reg = {
        .range = {
            .start = (uintptr_t) buf,
            .len = size,
        },
        .mode = UFFDIO_REGISTER_MODE_WP,
    };
    if (ioctl(uffd, UFFDIO_REGISTER, &reg) != 0) {
        LOG_MSG(error, "Failed to register %d-byte memory region!\n", size);
        vmem_free(buf, size);
        return NULL;
    }

    struct uffdio_writeprotect wp = {
        .range = {
            .start = (uintptr_t) buf,
            .len = size,
        },
        .mode = UFFDIO_WRITEPROTECT_MODE_WP,
    };
    if (ioctl(uffd, UFFDIO_WRITEPROTECT, &wp) != 0) {
        LOG_MSG(error, "Failed to register %d-byte memory region!\n", size);
        vmem_free(buf, size);
        return NULL;
    }

    vmem_reset_write_watching(buf, size);
    return buf;
}

bool vmem_get_dirty_pages(void* addr, u64 num_pages, void** dirty_out, u64 dirty_out_size, u64* num_dirty_out) {
    FILE* f = fopen("/proc/self/pagemap", "rb");
    const u64 page_num = ((uintptr_t)addr / getpagesize());
    if (fseek(f, page_num * sizeof(u64), SEEK_SET)) {
        // Some seek failure??
        return false;
    }
    *num_dirty_out = 0;

    // No bounds checking is needed inside the loop because our loop boundary
    // accounts for user array size.
    for (u64 i = 0; i < MIN(num_pages, dirty_out_size); i++) {
        // We assume the page is dirty until told otherwise, better safe than sorry.
        bool is_dirty = true;

        // Read page flags
        uint64_t flags = 0;
        if (fread(&flags, sizeof(flags), 1, f) == 1) {
            // Pages are write-protected until written to, at which point the
            // kernel marks it as written and removes the protection
            is_dirty = !GET_BIT(flags, PAGE_FLAG_UFFD_WRITE_PROTECTED);
        }

        if (is_dirty) {
            // Save the address to the output array
            void* dirty_addr = (u8*)addr + (i * VMEM_PAGE_SIZE);
            dirty_out[i] = dirty_addr;
            // I'm just not going to bother remembering the correct order of
            // parentheses needed to do this with a "++".
            *num_dirty_out = *num_dirty_out + 1;
        }
    }

    fclose(f);
    return true;
}

#endif

// After this point are only standard POSIX implementations.
#ifndef PLATFORM_ANDROID
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
#endif

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

void* vmem_map_file(const char* file) {
    const long page_size = sysconf(_SC_PAGE_SIZE);
    const s64 size = ALIGN_UP(file_size(file), page_size);
    const int prot = PROT_READ | PROT_WRITE;
    const int flags = MAP_PRIVATE;

    const int fd = open(file, O_RDONLY);
    if (fd == -1) {
        return NULL;
    }

    void* result = mmap(NULL, size, prot, flags, fd, 0);
    if (result == MAP_FAILED) {
        LOG_MSG(error, "Failed to map file '%s' because '%s'\n", file, strerror(errno));
        return NULL;
    }

    return result;
}

void vmem_unmap_file(void* addr, u64 size) {
    munmap(addr, size);
}
#endif
