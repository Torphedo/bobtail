#ifndef LIST_H
#define LIST_H
/// @file list.h
/// @brief Automatically expanding dynamic list

#include <stddef.h>
#include <stdbool.h>
#include "int.h"

/// @brief An automatically expanding dynamic list
typedef struct {
    /// @brief Backing buffer
    ///
    /// We use a uintptr_t so we can have a typeless pointer that can't
    /// accidentally be dereferenced.
    uintptr_t data;
    /// Current buffer size
    u32 alloc_size;
    /// @brief Index of the next open slot in the array (not the last element!)
    ///
    /// @warning This isn't the index of the last element! It could be an
    /// invalid index, or depending on the circumstances, invalid memory.
    u32 end_idx;
    /// Size of each array element
    u32 element_size;
}list;

/// Create a list.
/// @param init_size The allocation size in bytes. Please try to align to
/// @p element_size
///
/// @param element_size Thinking of the list as an array, this is the expected
/// size of each element.<br>
/// A list of u16 has element_size 2, a list of vec3f has element_size 12, etc.
///
/// @return A newly initialized dynamic list.
/// @note This allocates memory!
/// @sa list_destroy
list list_create(u32 init_size, u32 element_size);

/// @brief Free list data & fill all fields with 0
///
/// @param l List to destroy
/// @sa list_create
void list_destroy(list* l);

/// @brief Append an element to the list.
/// @param l The list to modify
/// @param data The data to append. Must be at least @ref list.element_size
/// bytes
/// @note If the list is full, this can cause a re-allocation.
/// @sa list_create()
void list_add(list* l, const void* data);

/// @brief Retrieve an element from the list.
///
/// @return Generic pointer to the element. This is our only option in C, since
/// the @ref list structure is generic. You'll have to cast to the appropriate
/// pointer type.
void* list_get_element(list l, u32 idx);

/// Reset list to initial state and fill buffer with 0. Does not free buffer.
void list_clear(list* l);

/// @brief Remove an element from the list.
///
/// @param l List to modify
/// @param idx Index of element to remove
/// @note This doesn't shift the entire list over by one element, as you might
/// expect. Don't make any assumptions about element order.
void list_remove(list* l, u32 idx);

/// @brief Find and remove the first occurance of a value from the list.
///
/// @param l List to modify
/// @param data Element to remove (Must be at least @ref list.element_size)
/// @sa list_remove
/// @sa list_find
void list_remove_val(list* l, const void* data);

/// @brief Append every element of @p src onto @p dest (duplicates are allowed)
/// @param dest List to append to
/// @param src List to copy data from
/// @note If @p dest is full, this can cause a re-allocation.
void list_merge(list* dest, list src);

/// @brief Search for a value and return its index
/// @param l List to search
/// @param data Data to search for. Must be at least @ref list.element_size.
/// @return Index of the data, or -1 on failure.
s64 list_find(list l, const void* data);

/// @brief Whether the list contains a certain value.
/// @param l List to search
/// @param data Data to search for. Must be at least @ref list.element_size.
///
/// @sa list_find
bool list_contains(list l, const void* data);

/// @brief Whether the list is empty
bool list_empty(list l);

#endif // #ifndef LIST_H
