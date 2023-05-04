#ifndef UTILITY_H
#define UTILITY_H

#include <stdint.h>
#include "font.h"

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



// --- SPI OPERATIONS --- //
/*
 * Function: send ssi command
 * ----------------------------
 *   send command instruction via SSI
 *
 *   command: 8 bit command instruction
 *
 */
void send_ssi_command(uint8_t command);

/*
 * Function: send ssi data
 * ----------------------------
 *   send data instruction via ssi
 *
 *   command: 8 bit command instruction
 * 
 */
void send_ssi_data(uint8_t data);


/*
 * Function: initialize ssi
 * ----------------------------
 *   setup ssi module on MCU
 *
 * 
 */
void initialize_ssi();


/*
 * Function: configure display
 * ----------------------------
 *   send series of commands/data to setup display
 *
 * 
 */
void configure_display();




// --- Utils --- //
unsigned char reverse_byte(unsigned char);

#endif 