#include <stdint.h>

#include "utility.h"
#include "tm4c123gh6pm.h"

// --- Register Operations --- //

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

// --- System --- //
void initialize_port_clock(ports_t port)    
{
    write_reg_mask(&SYSCTL_RCGCGPIO_R, 0x1 << port, 0x1 << port, BIT_ON);
}

void initialize_system_clock(double clock_speed, OSCSRC_t src, XTAL_t xtal)
{

    // use RCC2, bypass PLL, and disable SYSDIV while configuring
    write_reg_mask(&SYSCTL_RCC2_R, SYSCTL_RCC2_USERCC2, SYSCTL_RCC2_USERCC2, BIT_ON);
    write_reg_mask(&SYSCTL_RCC2_R, SYSCTL_RCC2_BYPASS2, SYSCTL_RCC2_BYPASS2, BIT_ON);
    write_reg_mask(&SYSCTL_RCC_R, SYSCTL_RCC_USESYSDIV, SYSCTL_RCC_USESYSDIV, BIT_OFF);

    // configure oscillator source and external value
    const int osc_src2_off = 4;
    const int xtal_off = 6;
    uint32_t osc_src2 = src << osc_src2_off;
    xtal = xtal << xtal_off;
    write_reg_mask(&SYSCTL_RCC2_R, SYSCTL_RCC2_OSCSRC2_M, osc_src2, VALUE);
    write_reg_mask(&SYSCTL_RCC_R, SYSCTL_RCC_XTAL_M, xtal_off, VALUE);

    // enable PLL
    write_reg_mask(&SYSCTL_RCC2_R, SYSCTL_RCC2_PWRDN2, SYSCTL_RCC2_PWRDN2, BIT_OFF);

    // set system divider for PLL
    write_reg_mask(&SYSCTL_RCC2_R ,SYSCTL_RCC2_DIV400, SYSCTL_RCC2_DIV400, BIT_ON);
    write_reg_mask(&SYSCTL_RCC2_R, SYSCTL_RCC2_SYSDIV2_M, 0x2, VALUE);
    write_reg_mask(&SYSCTL_RCC2_R, SYSCTL_RCC2_SYSDIV2LSB, SYSCTL_RCC2_SYSDIV2LSB, BIT_OFF);

    // wait for PLL lock
    while (!read_reg_by_mask(&SYSCTL_RIS_R, SYSCTL_RIS_PLLLRIS))
        ;

    // enable PLL
    write_reg_mask(&SYSCTL_RCC2_R, SYSCTL_RCC2_BYPASS2, SYSCTL_RCC2_BYPASS2, BIT_OFF);
}

// --- Systick --- //

volatile int32_t count;

void initialize_systick()
{

    // disable counter
    write_reg_mask(&NVIC_ST_CTRL_R, NVIC_ST_CTRL_ENABLE, NVIC_ST_CTRL_ENABLE, BIT_OFF);

    write_reg_mask(&NVIC_ST_RELOAD_R, NVIC_ST_RELOAD_M, 833332, VALUE);
    write_reg_mask(&NVIC_ST_CURRENT_R, NVIC_ST_CURRENT_S, 0x0, VALUE);

    write_reg_mask(
        &NVIC_ST_CTRL_R, 
        NVIC_ST_CTRL_ENABLE | NVIC_ST_CTRL_INTEN | NVIC_ST_CTRL_CLK_SRC, 
        NVIC_ST_CTRL_ENABLE | NVIC_ST_CTRL_INTEN | NVIC_ST_CTRL_CLK_SRC, 
        BIT_ON
    );
}

void SysTick_Handler(void)
{
    if (count != 0)
    {
        count--;
    }
}

void delay_10ms(uint32_t time)
{
    count = time;
    while (count != 0)
    {
    }
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
