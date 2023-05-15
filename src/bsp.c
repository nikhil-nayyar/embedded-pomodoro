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

// --- FPU --- //

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

// --- Ports --- //
void initialize_port_clock(ports_t port)    
{
    write_reg_mask(&SYSCTL_RCGCGPIO_R, 0x1 << port, 0x1 << port, BIT_ON);
}

// --- System Clock --- //

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

// --- Timer and Timing --- //

#define TIMER_1US 83

volatile uint8_t control_flag;

void initialize_wtimer0(){

    /* Configure System */
    SYSCTL_RCGCWTIMER_R |= 0x1; // Enable Timer 0 Clock

    /* Configure Timer */
    WTIMER0_CTL_R &= ~(TIMER_CTL_TAEN); // disable timer
    //WTIMER0_CTL_R |= TIMER_CTL_TASTALL; // disable timer while debugging
    WTIMER0_CFG_R = TIMER_CFG_16_BIT; // Set at 32 bit timer
    WTIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD; // set timer to fire periodically
    WTIMER0_IMR_R = TIMER_IMR_TATOIM; // set interrupt to trigger on end of count

    /* Configure NVIC */
    // vector 110 int 94
    NVIC_EN2_R |= (0x1 << (94-64)); // Enable interrupt 19
    NVIC_PRI23_R |= (0x0 << NVIC_PRI23_INTC_S);
 }

void WTimer0A_Handler(void){

    /* Disable Timer */
    WTIMER0_CTL_R &= ~(TIMER_CTL_TAEN); // disable timer

    /* Clear Flag */
    WTIMER0_ICR_R = TIMER_ICR_TATOCINT;

    /* Set control flag to false */
    control_flag = 0;
}

void delay_block_us(unsigned int n){

    uint32_t count = n * TIMER_1US;

    control_flag = 1;
    WTIMER0_TAILR_R = count;
    WTIMER0_CTL_R |= (TIMER_CTL_TAEN); // enable timer

    while(control_flag)
        ;

}

void delay_block_ms(unsigned int n){
    delay_block_us(1000*n);
}