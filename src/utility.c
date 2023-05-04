#include <stdint.h>

#include "bsp.h"
#include "utility.h"
#include "tm4c123gh6pm.h"

/* Register Operations */

void write_reg(ADDRESS reg, uint32_t data){
    *reg = data;
}

void write_reg_mask(ADDRESS reg, uint32_t mask, uint32_t data, write_reg_t type)
{

    // clear bits before write
    *reg &= ~(mask);

    // manipulate data based on type of write if necessary
    if (type == BIT_OFF)
    {
        data = ~(data)&mask;
    }

    // write data
    *reg |= data;
}

void write_reg_bit(ADDRESS reg, uint8_t index, bit_t value){
    if(value)
        *reg |= 0x1 << index;
    else
        *reg &= ~(0x1 << index);
}

void write_reg_bits(ADDRESS reg, uint8_t indices[], uint8_t size, bit_t value){

    uint16_t data = 0x0;
    for(int i = 0; i < size; i++)
        data |= (0x1 << indices[i]);

    write_reg_mask(reg, data, data, value);

}


void write_data_bit(ADDRESS reg, uint8_t index, bit_t value){

    // account for data register write access (see p. 662 of data sheet)
    uint16_t address_operand = (0x1 << (index));
    reg = reg + address_operand;

    // enable/disable
    uint16_t val = 0xFFF;
    if(value)
        *reg |= (val);
    else
        *reg &= ~(val);
}

uint32_t read_reg_by_mask(ADDRESS reg, uint32_t mask)
{

    // read data
    return *reg &= mask;
}

// --- SSI --- //


void initialize_ssi()
{

    // --- enable modules --- //
    // SSI1
    write_reg_mask(&SYSCTL_RCGCSSI_R, SYSCTL_RCGCSSI_R1,SYSCTL_RCGCSSI_R1, BIT_ON);

    // Port D
    initialize_port_clock(D);
    // Enable use of PD7
    write_reg_mask(&GPIO_PORTD_LOCK_R, GPIO_LOCK_M, GPIO_LOCK_KEY, VALUE);
    write_reg_bit(&GPIO_PORTD_CR_R, 7, ON);

    // reserve PD167 for display initialization
    const int size = 3;
    uint8_t indicies[3] = {1, 6, 7};
    write_reg_bits(&GPIO_PORTD_DIR_R, indicies, size, ON);
    write_reg_bits(&GPIO_PORTD_AFSEL_R, indicies, size, OFF);
    write_reg_bits(&GPIO_PORTD_DEN_R, indicies, size, ON);
    write_reg_bits(&GPIO_PORTD_AMSEL_R, indicies, size, OFF);
    GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0x00FFFF0F)+0x00000000;

    write_data_bit(GPIO_PORTD_DATA_BITS_R, 1, OFF);
    write_data_bit(GPIO_PORTD_DATA_BITS_R, 7, ON);
    delay_10ms(50);
    write_data_bit(GPIO_PORTD_DATA_BITS_R, 7, OFF);
    delay_10ms(50);
    write_data_bit(GPIO_PORTD_DATA_BITS_R, 7, ON);
    delay_10ms(50);

    // assign SSI signals to GPIO pins
    const int size2 = 3;
    uint8_t indicies2[3] = {0,1,3};

    write_reg_bits(&GPIO_PORTD_AFSEL_R, indicies2, size2, ON);
    write_reg_bits(&GPIO_PORTD_DEN_R, indicies2, size2, ON);
    write_reg_bits(&GPIO_PORTD_AMSEL_R, indicies2, size2, OFF);
    write_reg_bits(&GPIO_PORTD_DR2R_R, indicies2, size2, ON);
    write_reg_mask(
        &GPIO_PORTD_PCTL_R,
        GPIO_PCTL_PD0_M | GPIO_PCTL_PD1_M | GPIO_PCTL_PD2_M | GPIO_PCTL_PD3_M,
        GPIO_PCTL_PD0_SSI1CLK | GPIO_PCTL_PD1_SSI1FSS | GPIO_PCTL_PD2_SSI1RX | GPIO_PCTL_PD3_SSI1TX,
        VALUE
    );

    // --- configure SSI --- //
    // disable SPI
    write_reg_mask(&SSI1_CR1_R, SSI_CR1_LBM, SSI_CR1_LBM, BIT_OFF);
    write_reg_mask(&SSI1_CR1_R, SSI_CR1_SSE, SSI_CR1_SSE, BIT_OFF);
    write_reg_mask(&SSI1_CR1_R, SSI_CR1_MS, SSI_CR1_MS, BIT_OFF);

    // configure SPI clock
    write_reg_mask(&SSI1_CC_R, SSI_CC_CS_M, SSI_CC_CS_SYSPLL, VALUE);
    write_reg_mask(&SSI1_CPSR_R, SSI_CPSR_CPSDVSR_M, 2 << SSI_CPSR_CPSDVSR_S, VALUE);
    write_reg_mask(&SSI1_CR0_R, SSI_CR0_SCR_M, 4 << SSI_CR0_SCR_S, VALUE);

    // configure SPI options
    write_reg_mask(&SSI1_CR0_R, SSI_CR0_SPH | SSI_CR0_SPO, SSI_CR0_SPH | SSI_CR0_SPO, BIT_OFF);
    write_reg_mask(&SSI1_CR0_R, SSI_CR0_FRF_M, SSI_CR0_FRF_MOTO, VALUE);
    write_reg_mask(&SSI1_CR0_R, SSI_CR0_DSS_M, SSI_CR0_DSS_8, VALUE);

    // enable SPI
    write_reg_mask(&SSI1_CR1_R, SSI_CR1_SSE, SSI_CR1_SSE, BIT_ON);
}

void configure_display(){

    send_ssi_command(0x01); // sw reset
    delay_10ms(5);
    send_ssi_command(0x11); // turn off sleep
    delay_10ms(50);
    send_ssi_command(0x13); // turn on normal display mode
    delay_10ms(1);
    send_ssi_command(0x20); // turn off display inversion
    send_ssi_command(0x13); // turn off partial display

    // set FR to 60fps
    send_ssi_command(0xB1);
    send_ssi_data(0x01);
    send_ssi_data(0x2C);
    send_ssi_data(0x2D);

    // display refresh settings
    send_ssi_command(0xB6);
    send_ssi_data(0x15);
    send_ssi_data(0x02);

    // set display inversion control
    send_ssi_command(0xB4);
    send_ssi_data(0x0);

    // set memory data access control
    send_ssi_command(0x36);
    send_ssi_data(0x00);

    // set color mode to 18bit
    send_ssi_command(0x3A);
    send_ssi_data(0x05);

    send_ssi_command(0x29); // turn on display
    delay_10ms(100);

}

void send_ssi_command(uint8_t command)
{

    while ((read_reg_by_mask(&SSI1_SR_R, SSI_SR_BSY) & SSI_SR_BSY) == SSI_SR_BSY)
    ;
    // set a0 low
    write_data_bit(GPIO_PORTD_DATA_BITS_R, 6, OFF);

    // write to data register
    write_reg(&SSI1_DR_R, command);

    // wait until command sent
    while (read_reg_by_mask(&SSI1_SR_R, SSI_SR_BSY) & SSI_SR_BSY)
    ;

}


void send_ssi_data(uint8_t command)
{

    while (read_reg_by_mask(&SSI1_SR_R, SSI_SR_TNF)==0)
    ;

    // set a0 high
    write_data_bit(GPIO_PORTD_DATA_BITS_R, 6, ON);

    // write to data register
    write_reg(&SSI1_DR_R, command);

}

// --- Utils --- //
static const unsigned char reverse_table[] = {
    0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
    0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
    0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
    0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
    0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
    0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
    0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
    0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
    0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
    0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
    0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
    0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
    0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
    0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
    0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
    0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
    0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
    0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
    0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
    0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
    0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
    0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
    0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
    0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
    0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
    0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
    0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
    0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
    0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
    0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
    0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
    0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
};

unsigned char reverse_byte(unsigned char x){
    return reverse_table[x];
}