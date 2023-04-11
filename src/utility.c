#include <stdint.h>

#include "utility.h"
#include "tm4c123gh6pm.h"

// --- Bit Manipulation --- //

/*
 * Function: get_bit
 * ----------------------------
 *   Returns bit value for a given register
 *
 *   reg: register address from which to get bit value 
 *   bit: bit value to get
 *
 *   returns: bit value from given register
 */
unsigned int get_bit(ADDRESS reg, int bit){
    return (*reg & (0x1 << bit));
}

/*
 * Function: set_bit
 * ----------------------------
 *   Set the bit with the specified setting for a given register
 *
 *   reg: register address for which to set bit value 
 *   bit: bit value to set
 *   value: setting for specified bit: OFF (0) or ON (1)
 *
 */
void set_bit(ADDRESS reg, int bit, bit_status_t value){
    if(value){
        *reg |= (0x1 << bit);
    }
    else{
        *reg &= !(0x1 << bit);
    }
}

/*
 * Function: set_bits
 * ----------------------------
 *   Set the bits with the specified setting for a given register
 *
 *   reg: register address for which to set bit value 
 *   bit: bit values to set
 *   value: setting for specified bit: OFF (0) or ON (1)
 *
 */
void set_bits(ADDRESS reg, int bits[], int num_bits, bit_status_t value){
    for(int i = 0; i < num_bits; i++){
        set_bit(reg, bits[i], value);
    }
}

// --- System --- //
void initialize_port_clock(ports_t port){
    set_bit(&SYSCTL_RCGCGPIO_R, port, ON);
}