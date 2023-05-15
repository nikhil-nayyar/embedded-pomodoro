#include "stdint.h"

#include "bsp.h"
#include "input.h"
#include "tm4c123gh6pm.h"


/**
 * Pin mappings
 * IDX0: PF4
 * PhA0 PF0
 * PhB0 PF1
*/
void initialize_input(){

    // enable QEI0 clock
    SYSCTL_RCGCQEI_R |= 0x1;

    // enable Port F clock
    initialize_port_clock(F);

    /* configure PORTF GPIO Pins */
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTF_CR_R |= (0x13); 
    GPIO_PORTF_DEN_R |= (0x13); // set as digital
    GPIO_PORTF_DIR_R &= ~(0x13); // set as inputf
    GPIO_PORTF_AFSEL_R |= (0x13);
    GPIO_PORTF_PCTL_R |= GPIO_PCTL_PF0_PHA0 | GPIO_PCTL_PF1_PHB0 | GPIO_PCTL_PF4_IDX0;

    /* configure QEI */
    QEI0_CTL_R |=  (20) << QEI_CTL_FILTCNT_S | QEI_CTL_STALLEN | QEI_CTL_FILTEN; //| QEI_CTL_SIGMODE;
    QEI0_MAXPOS_R = 9;
    QEI0_CTL_R |= 0x1;

}

int read_encoder(){
    return QEI0_POS_R;
}