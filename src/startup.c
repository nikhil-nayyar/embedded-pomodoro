#include <stdint.h>
#include "startup.h"

const vector_table_element vector_table[] __attribute__((section(".vector_table"))) = {
    {.address_pointer=&_stack_ptr},
    Reset_Handler,
    NMI_Handler,                // 2    Non-Maskable Interrupt handler
    HardFault_Handler,          // 3    Hard Fault Handler
    MemManageFault_Handler,     // 4    Memory management fault Handler
    BusFault_Handler,           // 5    Bus Fault Handler
    UsageFault_Handler,         // 6    Usage Fault Handler
    0,                          // 7    Reserved
    0,                          // 8    Reserved
    0,                          // 9    Reserved
    0,                          // 10   Reserved
    SVC_Handler,                // 11   SuperVisor Call Handler
    DebugMonitor_Handler,       // 12   Debug Monitor Handler
    0,                          // 13   Reserved
    PendSV_Handler,             // 14   Pendeable interrupt driven request
    SysTick_Handler,            // 15   SysTick Timer handler
};


void Reset_Handler(void){


    // 1. copy data memory from flash to sram
    uint32_t* src =  &_etext;
    uint32_t* dst =  &_sdata;
    uint32_t* end =  &_edata;

    while(dst < end){
        *dst++ = *src++;
    }

    // 2. initialize bss values to 0
    dst =  &_sbss;
    end =  &_ebss;
    while(dst<end){
        *dst++ =0;
    }

    // 3. Call main
    main();

}

void Handler(void){
    while(1){

    }
}