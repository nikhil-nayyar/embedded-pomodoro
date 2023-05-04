/**
 *   Implementation for all functionality tied to the board
 *   used in this project (TM4C123GH6PM).
 * 
 */

#include <math.h>
#include <stdint.h>
#include "bsp.h"
#include "tm4c123gh6pm.h"
#include "utility.h"

/*FPU*/

// assembly code taken from
// https://developer.arm.com/documentation/ddi0439/b/Floating-Point-Unit/FPU-Programmers-Model/Enabling-the-FPU
void initialize_fpu(){
        __asm__(
       // ; CPACR is located at address 0xE000ED88
        "LDR.W   %R0, = $0xE000ED88 \n\t"
        // ; Read CPACR
        "LDR     %R1, [%R0] \n\t"
        //; Set bits 20-23 to enable CP10 and CP11 coprocessors
        "ORR     %R1, %R1, $(0xF << 20) \n\t"
        //; Write back the modified value to the CPACR
        "STR     %R1, [%R0]\n\t"
    );
}

/* Ports */
void initialize_port_clock(ports_t port)    
{
    write_reg_mask(&SYSCTL_RCGCGPIO_R, 0x1 << port, 0x1 << port, BIT_ON);
}

/* System Clock */
void initialize_system_clock(double clock_speed, OSCSRC_t src, XTAL_t xtal)
{

    system_clock_frequency = 80; // Hardcode to 80MHz

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

/* SysTick & Timing */

volatile unsigned int count = 0;

void initialize_systick(unsigned int period)
{
    // disable counter
    write_reg_mask(&NVIC_ST_CTRL_R, NVIC_ST_CTRL_ENABLE, NVIC_ST_CTRL_ENABLE, BIT_OFF);

    // calculate and load reload value
    // unsigned int reload = ( (float) pow(10,-3) / ( 1 / (system_clock_frequency * (float) pow(10,6) ) ) ) - 1;
    unsigned int reload = 83332;
    write_reg_mask(&NVIC_ST_RELOAD_R, NVIC_ST_RELOAD_M, reload, VALUE);

    // write to current to force reload
    write_reg_mask(&NVIC_ST_CURRENT_R, NVIC_ST_CURRENT_S, 0x0, VALUE);

    // enable interrupt
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

void delay_10ms(unsigned int n)
{
    count = n;
    while (count != 0)
    {
    }
}
