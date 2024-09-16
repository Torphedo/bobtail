#include <stdlib.h> // For NULL
#include "platform.h"
#include "int.h"

// Unix / POSIX implementation
#if defined(PLATFORM_UNIX) || defined(PLATFORM_APPLE)
#include <sys/mman.h>

void* virtual_reserve(u64 size) {
    return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}
int virtual_commit(void* addr, u64 size) {
    return 0;
}
int virtual_free(void* addr, u64 size) {
    return munmap(addr, size);
}
#endif

#ifdef PLATFORM_WINDOWS
#include <Windows.h>

void* virtual_reserve(u64 size) {
    return VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}
int virtual_commit(void* addr, u64 size) {
    // We reserve with MEM_RESERVE | MEM_COMMIT, so Windows will automatically
    // commit physical memory as needed when we write to the memory.
    return 0;
}
int virtual_free(void* addr, u64 size) {
    if (VirtualFree(addr, 0, MEM_RELEASE)) {
        return 0;
    }
    else {
	return -1;
    }
}
#endif

// Reserving / committing doesn't really exist as a kernel concept on HorizonOS
// (Switch). We'll probably have to fake this behaviour by doing re-allocation
// and copying behind the scenes to get it to work.
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

void* virtual_reserve(u64 size) {
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
int virtual_commit(void* addr, u64 size) {
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
int virtual_free(void* addr, u64 size) {
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

