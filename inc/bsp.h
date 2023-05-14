/**
 *   Declarations for all functionality tied to the 
 *   specific board used in this project (TM4C123GH6PM).
 */

#ifndef BSP_H
#define BSP_H

#include <stdint.h>

// --- FPU --- // 

/**
 * initialize floating point unit on Cortex-M processor
*/
void initialize_fpu();

// --- Ports --- //

/* map port letter to number */
typedef enum{A,B,C,D,E,F} ports_t;

/**
 * activate clock for GPIO port
 *
 * @param port
 *  port enum to activate
 *
 */
void initialize_port_clock(ports_t port);





// --- System Clock --- //

typedef enum{MOSC, PIOSC, PIOSC_4, LFIOSC} OSCSRC_t;
typedef enum{N1,N2, N3, N4, N5, N6, M4, M4096, M49152, M5, M512, M6, M6144, M73728, 
            M8, M8192, M10, M12, M12288, M1356, M1421818, M16, 
            M16384, M18, M20, M24, M25
            } XTAL_t;

/* global variable holding clock frequency*/
static long system_clock_frequency; // MHz

/**
 * initialize system clock to given speed based on inputs
 * 
 * @param frequency
 *  desired system clock frequency 
 * @param source
 *  desired system clock source
 * @param xtal_frequency
 *  external clock source frequency (16MHz on TM4C123GH6PM)
*/
void initialize_system_clock(double frequency, OSCSRC_t source, XTAL_t xtal_frequency);

// --- Systick & Timing --- //

/**
 * initialize SysTick interrupt, uses clock frequency set in 
 * initialize_system_clock().
 * 
 * @param period
 *  time interval (in ms) for firing SysTick interrupt
*/
void initialize_systick(unsigned int period);

/**
 * delay n*10ms
 * 
 * @param n
 *  number of times to delay 10ms
*/
void delay_10ms(unsigned int n);

#endif