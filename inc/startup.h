#ifndef STARTUP_H
#define STARTUP_H

#include <stdint.h>

/* if function not implemented, default to Handler()*/
#define WEAK __attribute__((weak, alias("Handler")))

/* union used for staring NVIC table values*/
typedef union tag_vector_table_element
{
    void (*function_handler)(void);
    void* address_pointer;
} vector_table_element;

/* array designating values loaded to NVIC*/
const vector_table_element vector_table[];

/**
 * Function run on device reset
*/
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

/**
 * main function ... 'nuff said
*/
int main(void);

/* symbols defined in ld script 
   used in initializing system data*/
extern uint32_t _stack_ptr;

extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;

extern uint32_t _sbss;
extern uint32_t _ebss;

#endif