#include <stdlib.h> // For NULL
#include "platform.h"
#include "int.h"

// Unix / POSIX implementation
#if defined(PLATFORM_UNIX) || defined(PLATFORM_APPLE)
#include <sys/mman.h>

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

#ifdef PLATFORM_WINDOWS
#include <Windows.h>

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

// Reserve / commit doesn't really exist as a kernel concept in HorizonOS
// (Switch). However, this behaviour can be emulated in userspace with the help
// of libnx.
#ifdef PLATFORM_SWITCH
#include <switch/kernel/virtmem.h>

typedef struct ReservationMapping ReservationMapping;

struct ReservationMapping {
	ReservationMapping* prev;
	ReservationMapping* next;
	void* addr;
	u64 size;
	VirtmemReservation* reservation;
};

static ReservationMapping* g_ReservationMappings;

void* vmem_reserve(u64 size) {
	virtmemLock();
	void* addr = virtmemFindAslr(size, 0);
	VirtmemReservation* reservation = virtmemAddReservation(addr, size);
	virtmemUnlock();

	ReservationMapping* reservationMapping = malloc(sizeof(ReservationMapping));
	reservationMapping->prev = NULL;
	reservationMapping->next = g_ReservationMappings;
	reservationMapping->addr = addr;
	reservationMapping->size = size;
	reservationMapping->reservation = reservation;
	g_ReservationMappings->prev = reservationMapping;
	g_ReservationMappings = reservationMapping;

	return addr;
}
int vmem_commit(void* addr, u64 size) {
	virtmemLock();
	VirtmemReservation* reservation = virtmemAddReservation(addr, size);
	virtmemUnlock();

	ReservationMapping* reservationMapping = malloc(sizeof(ReservationMapping));
	reservationMapping->prev = NULL;
	reservationMapping->next = g_ReservationMappings;
	reservationMapping->addr = addr;
	reservationMapping->size = size;
	reservationMapping->reservation = reservation;
	g_ReservationMappings->prev = reservationMapping;
	g_ReservationMappings = reservationMapping;

	if (reservation)
		return 0;
	else
		return -1;
}
int vmem_free(void* addr, u64 size) {
	VirtmemReservation* reservation;

	for (ReservationMapping* reservationMapping = g_ReservationMappings; reservationMapping; reservationMapping = reservationMapping->next) {
		if (reservationMapping->addr < (char*)addr + size && (char*)reservationMapping->addr + reservationMapping->size > addr) {
			virtmemLock();

			virtmemRemoveReservation(reservationMapping->reservation);

			// It's possible that we are freeing just a subsection of what was reserved.
			// Because of this we may need to keep a segment at the end or beginning reserved, or even both.
			if (reservationMapping->addr != addr) {
				VirtmemReservation* leftoverReservation = virtmemAddReservation(reservationMapping->addr, (u64)addr - (u64)reservationMapping->addr);
				ReservationMapping* leftoverReservationMapping = malloc(sizeof(ReservationMapping));
				leftoverReservationMapping->prev = NULL;
				leftoverReservationMapping->next = g_ReservationMappings;
				leftoverReservationMapping->addr = addr;
				leftoverReservationMapping->size = size;
				leftoverReservationMapping->reservation = leftoverReservation;
				g_ReservationMappings->prev = reservationMapping;
				g_ReservationMappings = leftoverReservationMapping;
			}
			if ((char*)reservationMapping->addr + reservationMapping->size != (char*)addr + size) {
				VirtmemReservation* leftoverReservation = virtmemAddReservation((char*)addr + size, ((char*)reservationMapping->addr + reservationMapping->size) - ((char*)addr + size));
				ReservationMapping* leftoverReservationMapping = malloc(sizeof(ReservationMapping));
				leftoverReservationMapping->prev = NULL;
				leftoverReservationMapping->next = g_ReservationMappings;
				leftoverReservationMapping->addr = addr;
				leftoverReservationMapping->size = size;
				leftoverReservationMapping->reservation = leftoverReservation;
				g_ReservationMappings->prev = reservationMapping;
				g_ReservationMappings = leftoverReservationMapping;
			}

			// Remove this reservation from the list.
			if (reservationMapping->next)
				reservationMapping->next->prev = reservationMapping->prev;
			if (reservationMapping->prev)
				reservationMapping->prev->next = reservationMapping->next;

			free(reservationMapping);

			virtmemUnlock();
			return 0;
		}
	}

	return 0; // What was freed was never reserved, so I guess it's a success.
}
#endif

