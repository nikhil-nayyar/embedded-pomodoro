#include <stdint.h>
#include "bsp.h"
#include "tm4c123gh6pm.h"
#include "display.h"
#include "utility.h"

static int test = 42;

void initialize_port_f(){
    int temp = test;
    initialize_port_clock(F);
    int bits[] = {1,2,3};
    write_reg_mask(&GPIO_PORTF_DEN_R, 0xF, 0xF, VALUE);
    write_reg_mask(&GPIO_PORTF_DIR_R, 0xF, 0xF, VALUE);
}


int main(void){

    // Configure Register
    initialize_fpu();
    initialize_port_f();  
    initialize_system_clock(80, PIOSC, M16);
    initialize_systick(1);

    // Configure SSI
    initialize_ssi();
    configure_display();
    fill_display(0,0,0);

    int count = 0;

    // Infinite Loop
    while(1){
        write_character((count++)+'0');
        delay_10ms(10);
    }

}