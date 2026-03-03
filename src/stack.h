#include "src/common.h"

typedef struct {
    uint capacity;
    uint count;
} m_stack;

m_stack* m_stack_new(uint capacity);
void m_stack_push(m_stack* stack, void* item);
void* m_stack_pop(m_stack* stack);
void* m_stack_peek(m_stack* stack);
void m_stack_cleanup(m_stack* stack);