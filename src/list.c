#include "src/common.h"

typedef struct list_node list_node;
struct list_node {
    list_node* next;
    void* data;
};

typedef struct {
    uint length;
    list_node* first;
    list_node* last;
} m_list;

m_list* m_list_new() {
    m_list* list = malloc(sizeof(m_list));
    list->length = 0;
    return list;
}

void* m_list_get(m_list* list, uint index) {
    if (index >= list->length)
        return null;
    list_node* node = list->first;
    int i = 0;
    while (i++ < index)
        node = node->next;
    return node->data;
}

void m_list_remove(m_list* list, uint index) {
    if (index >= list->length)
        return;
    list_node* prev;
    list_node* node = list->first;
    int i = 0;
    while (i++ < index) {
        prev = node;
        node = node->next;
    }
    if (index == 0)
        list->first = node->next;
    else if (index == list->length - 1)
        list->last = prev;
    else
        prev->next = node->next;
    free(node);
    list->length--;
}

void m_list_add(m_list* list, void* data) {
    list_node* node = malloc(sizeof(list_node));
    node->data = data;
    node->next = null;
    if (list->length == 0)
        list->first = node;
    else
        list->last->next = node;
    list->last = node;

    list->length++;
}

void m_list_set(m_list* list, uint index, void* data) {
    if (index >= list->length)
        return;
    list_node* node = list->first;
    int i = 0;
    while (i++ < index)
        node = node->next;
    node->data = data;
}

int m_list_index(m_list* list, void* data) {
    list_node* node = list->first;
    int i = 0;
    while (i++ < list->length) {
        if (node->data == data)
            return i - 1;
        node = node->next;
    }
    return -1;
}

void m_list_cleanup(m_list* list) {
    list_node* node = list->first;
    int i = 0;
    while (i++ < list->length) {
        list_node* next = node->next;
        free(node);
        node = next;
    }
    free(list);
}

void m_list_full_cleanup(m_list* list) {
    list_node* node = list->first;
    int i = 0;
    while (i++ < list->length) {
        list_node* next = node->next;
        if (node->data != null)
            free(node->data);
        free(node);
        node = next;
    }
    free(list);
}

typedef struct {
    m_list* list;
    list_node* current;
    int index;
} m_list_iterator;

m_list_iterator* m_list_iter_new(m_list* list) {
    m_list_iterator* iterator = malloc(sizeof(m_list_iterator));
    iterator->list = list;
    iterator->current = list->first;
    iterator->index = 0;
    return iterator;
}

void m_list_iter_next(m_list_iterator* iterator) {
    iterator->current = iterator->current->next;
    iterator->index++;
}

void* m_list_iter_get(m_list_iterator* iterator) { return iterator->current->data; }

void m_list_iter_cleanup(m_list_iterator* iterator) { free(iterator); }