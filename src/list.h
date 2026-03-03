#pragma once

#include "src/common.h"

typedef struct {
    uint length;
} m_list;

// Returns a newly created list
m_list* m_list_new();
// Returns the item found at `index` in `list`
void* m_list_get(m_list* list, uint index);
// Removes the item found at `index` from `list`
void m_list_remove(m_list* list, uint index);
// Adds `data` as a new item to `list`
void m_list_add(m_list* list, void* data);
// Sets the value of the item at `index` in `list` to `data`
void m_list_set(m_list* list, uint index, void* data);
// Returns the index of the first appearance of `data` in `list`, or -1 if it is not present
int m_list_index(m_list* list, void* data);
// Destroy `list` and free its resources
void m_list_cleanup(m_list* list);
// Destroy `list` and free it resources, including all items
void m_list_full_cleanup(m_list* list);

typedef struct {
} m_list_iterator;

m_list_iterator* m_list_iter_new(m_list* list);
void m_list_iter_next(m_list_iterator* iterator);
void* m_list_iter_get(m_list_iterator* iterator);
void m_list_iter_cleanup(m_list_iterator* iterator);