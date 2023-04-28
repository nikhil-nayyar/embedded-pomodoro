#include <stdint.h>

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

void initialize_port_d(){
}

int main(void){

    // Configure Register
    initialize_port_f();  
    initialize_system_clock(80, PIOSC, M16);
    initialize_systick();


    initialize_ssi();
    configure_display();
    fill_display(0,0,0);

    int count = 0;

    while(1){
        int current = read_reg_by_mask(&GPIO_PORTF_DATA_R, 0xFF);
        if(current){
            write_reg_mask(&GPIO_PORTF_DATA_R, 0x1 << 3, 0x1 << 3, BIT_OFF);
        } else{
            write_reg_mask(&GPIO_PORTF_DATA_R, 0x1 << 3, 0x1 << 3, BIT_ON);
        }

        write_character(count+'0');
        count = (count + 1) % 10;
        delay_10ms(10);
    }


}