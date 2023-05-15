
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

    // assign SSI signals to GPIO pins, reserve A3 for GPIO chip select
    const int size = 3;
    uint8_t indicies[4] = {2,4,5};
    write_reg_bits(&GPIO_PORTA_AFSEL_R, indicies, size, ON);
    write_reg_bits(&GPIO_PORTA_DEN_R, indicies, size, ON);
    write_reg_bits(&GPIO_PORTA_DIR_R, indicies, size, ON);
    write_reg_bits(&GPIO_PORTA_AMSEL_R, indicies, size, OFF);
    write_reg_bits(&GPIO_PORTA_DR4R_R, indicies, size, ON);
    GPIO_PORTA_PCTL_R =  GPIO_PCTL_PA2_SSI0CLK  | GPIO_PCTL_PA4_SSI0RX | GPIO_PCTL_PA5_SSI0TX;

    GPIO_PORTA_DEN_R |= (0x1 << 3);
    GPIO_PORTA_DIR_R |= (0x1 << 3);
    GPIO_PORTA_DR4R_R |= (0x1 << 3);
    GPIO_PORTA_AFSEL_R &= ~(0x1 << 3);

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
    SSI0_CR1_R |= SSI_CR1_SSE | SSI_CR1_EOT;

    // configure array operation
    write_array_command_all(0x0900); // set no BCD decode
    write_array_command_all(0x0F00); // disable test mode
    write_array_command_all(0x0B07); // display all digits
    write_array_command_all(0x0A07); // medium intensity
    write_array_command_all(0x0C01); // disable shutdown mode

    // write_array_command_all(0x0108);
}

void write_array_command_all(uint16_t instruction){
    write_data_bit(GPIO_PORTA_DATA_BITS_R, 3, ON);
    for(int i = 0; i < 4; i++){
        SSI0_DR_R = instruction;

        while(!SSI0_RIS_R & 0x8)
            ;
    }
    write_data_bit(GPIO_PORTA_DATA_BITS_R, 3, OFF);    
    delay_block_ms(1);
    write_data_bit(GPIO_PORTA_DATA_BITS_R, 3, ON);

}

/**
 * Send series of 4 bursts of 16 bits, then latch
*/
void write_array_command(uint8_t block, uint16_t instruction){

    write_data_bit(GPIO_PORTA_DATA_BITS_R, 3, ON);
    for(int i = 3; i >= 0; i--){
        if(i == block){
            SSI0_DR_R = instruction;
        } else{
            SSI0_DR_R = 0x0000;
        }

        while(!SSI0_RIS_R & 0x8)
            ;
    }
    write_data_bit(GPIO_PORTA_DATA_BITS_R, 3, OFF);
    delay_block_ms(1);
    write_data_bit(GPIO_PORTA_DATA_BITS_R, 3, ON);
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

    write_array_command(block, instruction);

}


void increment_bar(){
    if(top <= 31){
        write_array_line(top++, 0xFF);
    } 
}

void decrement_bar(){
    if(top >= 0){
        write_array_line(top--, 0x00);
    } 
}

void fill_bar(){

    uint16_t instruction;
    for(int i = 1; i <= 8; i++){
        instruction = ((i) << 8) | 0xFF;
        write_array_command_all(instruction);
    }
    top = 31;
}

void clear_bar(){

    uint16_t instruction;
    for(int i = 1; i <= 8; i++){
        instruction = ((i) << 8) | 0x00;
        write_array_command_all(instruction);
    }
    top = 0;
}
