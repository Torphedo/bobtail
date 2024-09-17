// Nintendo Switch implementation of:
//     vmem_reserve()
//     vmem_commit()
//     vmem_free()
// Author: Greenlord/S14L0R

#include "platform.h"

#ifdef PLATFORM_SWITCH
#include <switch/kernel/virtmem.h>
#include <stdlib.h> // For NULL
#include "int.h"

// Nintendo Switch implementation of vmem_reserve/commit/free
// Reserve / commit doesn't really exist as a kernel concept in HorizonOS.
// However, this behaviour can be emulated in userspace with the help of libnx.
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

#endif // PLATFORM_SWITCH
