#ifndef STARTUP_H
#define STARTUP_H

#include <stdint.h>

#define WEAK __attribute__((weak, alias("Handler")))

typedef union tag_vector_table_element
{
    void (*function_handler)(void);
    void* address_pointer;
} vector_table_element;

const vector_table_element vector_table[];

void Reset_Handler(void);
WEAK void NMI_Handler(void);
WEAK void SVC_Handler(void);
WEAK void DebugMonitor_Handler(void);
WEAK void PendSV_Handler(void);
WEAK void SysTick_Handler(void);
WEAK void HardFault_Handler(void);
WEAK void MemManageFault_Handler(void);
WEAK void BusFault_Handler(void);
WEAK void UsageFault_Handler(void);

// Startup Function
int main(void);

extern uint32_t _stack_ptr;

extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;

extern uint32_t _sbss;
extern uint32_t _ebss;

#endif