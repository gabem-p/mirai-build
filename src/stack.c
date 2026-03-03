#include "src/common.h"

typedef struct {
    uint capacity;
    uint count;
    void** items;
} m_stack;

m_stack* m_stack_new(uint capacity) {
    m_stack* stack = malloc(sizeof(m_stack));
    stack->capacity = capacity;
    stack->count = 0;
    stack->items = calloc(capacity, sizeof(void*));
    return stack;
}

void m_stack_push(m_stack* stack, void* item) {
    stack->items[stack->count++] = item;
}

void* m_stack_pop(m_stack* stack) {
    void* item = stack->items[--stack->count];
    stack->items[stack->count] = null;
    return item;
}

void* m_stack_peek(m_stack* stack) {
    return stack->items[stack->count - 1];
}

void m_stack_cleanup(m_stack* stack) {
    free(stack->items);
    free(stack);
}