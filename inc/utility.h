#ifndef UTILITY_H
#define UTILITY_H

#include <stdint.h>

// --- MACROS --- //
#define ADDRESS volatile uint32_t*

// --- REGISTER OPERATIONS --- //
typedef enum {BIT_OFF, BIT_ON, VALUE} write_reg_t;
typedef enum {OFF, ON} bit_t;

/*
 * Function: write register
 * ----------------------------
 *   write specified data to MCU register
 *   function overwrites all data
 *   use this function when writing to communication data register
 *
 *   reg: register address
 *   mask: bits to clear and write to
 *   data: value to write to register
 *   type: type of data to write
 *
 */
void write_reg(ADDRESS reg, uint32_t data);

/*
 * Function: write register by mask
 * ----------------------------
 *   write specified data to MCU register
 *   function first clears bits and then writes 
 *
 *   reg: register address
 *   mask: bits to clear and write to
 *   data: value to write to register
 *   type: type of data to write
 *
 */
void write_reg_mask(ADDRESS reg, uint32_t mask,  uint32_t data, write_reg_t type);

/*
 * Function: write reg bit
 * ----------------------------
 *   write bit value in register
 *
 *   reg: register address
 *   index: bit index to set
 *   value: disable/enable
 *
 */
void write_reg_bit(ADDRESS reg, uint8_t index, bit_t value);

/*
 * Function: write reg bits
 * ----------------------------
 *   write bit values in register
 *
 *   reg: register address
 *   index: array of bits to set
 *   value: disable/enable
 *
 */
void write_reg_bits(ADDRESS reg, uint8_t indices[], uint8_t size, bit_t value);

/*
 * Function: write data bit
 * ----------------------------
 *   write bit value in data register
 *
 *   reg: data register address
 *   index: bit index to set
 *   value: disable/enable
 *
 */
void write_data_bit(ADDRESS reg, uint8_t index, bit_t value);

/*
 * Function: read from register
 * ----------------------------
 *   read bits specified by mask from register
 *
 *   reg: register address
 *   mask: data bits to read
 *
 */
uint32_t read_reg_by_mask(ADDRESS reg, uint32_t mask);


// --- Utils --- //

/**
 * returns byte data reversed
 * uses lookup table
 * 
 * @param char
 * 
*/
unsigned char reverse_byte(unsigned char character);

/**
 * converts hex character into integer
 * returns 0 if invalid character not (0-9,A-F)
 * 
 * @param c
 *  character to convert to hex
 * 
 * @return 
 *  integer
*/
int chtoi(char c);

#endif 