#include <stdint.h>

#include "tm4c123gh6pm.h"
#include "utility.h"

static int test = 42;

void initialize_port_f(){
    int temp = test;
    initialize_port_clock(F);
    int bits[] = {1,2,3};
    initialize_port_clock(A);
    set_bits(&GPIO_PORTF_DEN_R, bits, LENGTH(bits), ON);
    set_bits(&GPIO_PORTF_DIR_R, bits, LENGTH(bits), ON);
}

void Delay(void){
  unsigned long volatile time;
  time = 800000;
  while(time){
		time--;
  }
}

int main(void){

    // Configure Register
    initialize_port_f();    

    while(1){
        GPIO_PORTF_DATA_R = 0x2;
        Delay();
        GPIO_PORTF_DATA_R = 0x0;
        Delay();
    }


}