#include <stdint.h>
#include "startup.h"

/*NVIC Table taken from Table 2-8 (pg103) and Table 2-9 (pg104) 
  of TM4C123GH6PM Datasheet */
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
    GPIOA_Handler,              // 16   GPIO Port A Handler
    GPIOB_Handler,              // 17   GPIO Port B Handler
    GPIOC_Handler,              // 18   GPIO Port C Handler
    GPIOD_Handler,              // 19   GPIO Port D Handler
    GPIOE_Handler,              // 20   GPIO Port E Handler
    Placeholder, // 21 Placeholder
    Placeholder, // 22 Placeholder
    Placeholder, // 23 Placeholder
    Placeholder, // 24 Placeholder
    Placeholder, // 25 Placeholder
    Placeholder, // 26 Placeholder
    Placeholder, // 27 Placeholder
    Placeholder, // 28 Placeholder
    Placeholder, // 29 Placeholder
    Placeholder, // 30 Placeholder
    Placeholder, // 31 Placeholder
    Placeholder, // 32 Placeholder
    Placeholder, // 33 Placeholder
    Placeholder, // 34 Placeholder
    Timer0_Handler,            // 35 Timer0A_Handler
    Placeholder, // 36 Placeholder
    Placeholder, // 37 Placeholder
    Placeholder, // 38 Placeholder
    Placeholder, // 39 Placeholder
    Placeholder, // 40 Placeholder
    Placeholder, // 41 Placeholder
    Placeholder, // 42 Placeholder
    0,           // 43 Reserved
    Placeholder, // 44 Placeholder
    Placeholder, // 45 Placeholder
    Placeholder, // 46 Placeholder
    0,           // 47 Reserved
    0,           // 48 Reserved
    UART2_Handler,                // 49 UART2 Handler
    Placeholder, // 50 Placeholder
    Placeholder, // 51 Placeholder
    Placeholder, // 52 Placeholder
    Placeholder, // 53 Placeholder
    Placeholder, // 54 Placeholder
    Placeholder, // 55 Placeholder
    Placeholder, // 56 Placeholder
    0,           // 57 Reserved
    0,           // 58 Reserved
    Placeholder, // 59 Placeholder
    Placeholder, // 60 Placeholder
    Placeholder, // 61 Placeholder
    Placeholder, // 62 Placeholder
    Placeholder, // 63 Placeholder
    Placeholder, // 64 Placeholder
    Placeholder, // 65 Placeholder
    Placeholder, // 66 Placeholder
    Placeholder, // 67 Placeholder
    0,           // 68 Reserved
    0,           // 69 Reserved
    0,           // 70 Reserved
    0,           // 71 Reserved
    0,           // 72 Reserved
    Placeholder, // 73 Placeholder
    Placeholder, // 74 Placeholder
    Placeholder, // 75 Placeholder
    Placeholder, // 76 Placeholder
    Placeholder, // 77 Placeholder
    Placeholder, // 78 Placeholder
    Placeholder, // 79 Placeholder
    0,           // 80 Reserved
    0,           // 81 Reserved
    0,           // 82 Reserved
    0,           // 83 Reserved
    Placeholder, // 84 Placeholder
    Placeholder, // 85 Placeholder
    Placeholder, // 86 Placeholder
    Placeholder, // 87 Placeholder
    0,           // 88 Reserved
    0,           // 89 Reserved
    0,           // 90 Reserved
    0,           // 91 Reserved
    0,           // 92 Reserved
    0,           // 93 Reserved
    0,           // 94 Reserved
    0,           // 95 Reserved
    0,           // 96 Reserved
    0,           // 97 Reserved
    0,           // 98 Reserved
    0,           // 99 Reserved
    0,           // 100 Reserved
    0,           // 101 Reserved
    0,           // 102 Reserved
    0,           // 103 Reserved
    0,           // 104 Reserved
    0,           // 105 Reserved
    0,           // 106 Reserved
    0,           // 107 Reserved
    Placeholder, // 108 Placeholder
    Placeholder, // 109 Placeholder
    WTimer0A_Handler,           // 110 WTimer0A_Handler
    Placeholder, // 111 Placeholder
    WTimer1A_Handler,           // 112 WTimer1A Handler
};

/**
*  function run on device reset
*
*  performs 3 major steps:
*   1. copies .data segment from device flash to sram
*   2. initializes undefined globals to 0
*   3. calls main()
*/
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

/**
 * default handler when NVIC handler not defined
 * runs infinite loop
*/
void Handler(void){
    while(1)
        ;
}