#ifndef UTILITY_H
#define UTILITY_H

// --- Macros --- //
#define ADDRESS volatile uint32_t*
#define LENGTH(arr) sizeof(arr)/sizeof(arr[0])

// --- Bit Manipulation --- //
typedef enum {OFF, ON} bit_status_t;
unsigned int get_bit(ADDRESS reg, int bit_number);
void set_bit(ADDRESS reg, int bit, bit_status_t value);
void set_bits(ADDRESS reg, int bits[], int num_bits, bit_status_t value);

// --- System --- //
typedef enum{A,B,C,D,E,F} ports_t;
void initialize_port_clock(ports_t port);

#endif 