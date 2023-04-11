#include <stdint.h>
#include "startup.h"

const vector_table_element vector_table[] __attribute__((section(".vector_table"))) = {
    {.address_pointer=&_stack_ptr},
    Reset_Handler,
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
