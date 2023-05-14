
#include <stdint.h>
#include "array.h"
#include "bsp.h"
#include "tm4c123gh6pm.h"
#include "utility.h"

void initialize_array(){


    // --- enable modules --- //
    // SSI0

    SYSCTL_RCGCSSI_R |= 0x1; // enable SSI0 clock
    initialize_port_clock(A); // enable port a

    // assign SSI signals to GPIO pins
    const int size = 4;
    uint8_t indicies[4] = {2,3,4,5};
    write_reg_bits(&GPIO_PORTA_AFSEL_R, indicies, size, ON);
    write_reg_bits(&GPIO_PORTA_DEN_R, indicies, size, ON);
    write_reg_bits(&GPIO_PORTA_AMSEL_R, indicies, size, OFF);
    write_reg_bits(&GPIO_PORTA_DR4R_R, indicies, size, ON);
    GPIO_PORTA_PCTL_R =  GPIO_PCTL_PA2_SSI0CLK | GPIO_PCTL_PA3_SSI0FSS | GPIO_PCTL_PA4_SSI0RX | GPIO_PCTL_PA5_SSI0TX;

    // --- configure SSI --- //    
    SSI0_CR1_R = 0x0;   // disable loopback, disable SSI operation, configure as master

    // configure SPI clock to 10MHz
    SSI0_CC_R = 0x0; // use system clock
    SSI0_CPSR_R = 0x2; // set Clock Prescale to 2
    SSI0_CR0_R &= ~(SSI_CR0_SCR_M); // set Serial Clock Rate to 5
    SSI0_CR0_R |= (4 << SSI_CR0_SCR_S);

    // configure SPI options
    SSI0_CR0_R &= ~(SSI_CR0_SPH | SSI_CR0_SPO); // clock phase and polarity to 0
    SSI0_CR0_R &= ~(SSI_CR0_FRF_M); // freescale spi format
    SSI0_CR0_R |= SSI_CR0_FRF_MOTO;
    SSI0_CR0_R &= ~(SSI_CR0_DSS_M); // 16 bit data
    SSI0_CR0_R |= SSI_CR0_DSS_16;

    // 0000000000000000 00000101 0 0 00 1111

    // enable SPI
    SSI0_CR1_R |= SSI_CR1_SSE;

    // configure array operation
    write_array_command_all(0x0900); // set no BCD decode
    write_array_command_all(0x0F00); // disable test mode
    write_array_command_all(0x0B07); // display all digits
    write_array_command_all(0x0A07); // medium intensity
    write_array_command(0x0C01); // disable shutdown for first module
    write_array_command(0x0C01);
    write_array_command(0x0C01);
    write_array_command(0x0C01);
}

void write_array_command_all(uint16_t instruction){
    for(int i = 0; i < 4; i++)
        SSI0_DR_R = instruction;
}

void write_array_command(uint16_t instruction){
   SSI0_DR_R = instruction;
}

void enable_test(){
    write_array_command_all(0x0F01);// enable test mode
}

void disable_test(){
    write_array_command_all(0x0F00);// disable test mode
}

void enable_shutdown(){
    write_array_command_all(0x0C00); // enable shutdown mode
}

void disable_shutdown(){
    write_array_command_all(0x0C01); // enable shutdown mode
}

void blank(){
    for(int i = 0; i < 8; i++){
        uint16_t instr = 0x0000 | ((i+1) << 8) | 0x00;
        write_array_command_all(instr);
    }
}

void write_array_line(uint8_t line, uint8_t pattern){

    uint8_t block = (uint8_t) line / 8;
    uint8_t ind = line % 8;

    uint16_t instruction = 0x0 | ((ind+1) << 8 ) | pattern;

    /*
    for(int i = 0; i < 3; i++){
        if (i == block){
            SSI0_DR_R = instruction;
            break;
        }
        else{
            SSI0_DR_R = 0x0000;
        }
    }*/

    write_array_command(instruction);

}


void increment_bar_test(){
    if(top < 31){
        write_array_line(top, 0xFF);
        top++;
    } else{
        top = 0; 
        blank();
    }
}
