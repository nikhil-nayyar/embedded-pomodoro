#ifndef STARTUP_H
#define STARTUP_H

#include <stdint.h>

typedef union tag_vector_table_element
{
    void (*function_handler)(void);
    void* address_pointer;
} vector_table_element;

const vector_table_element vector_table[];

void Reset_Handler(void);

// Startup Function
int main(void);

extern uint32_t _stack_ptr;

extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;

extern uint32_t _sbss;
extern uint32_t _ebss;

#endif