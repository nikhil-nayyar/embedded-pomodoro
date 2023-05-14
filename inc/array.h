#ifndef ARRAY_H
#define ARRAY_H

// --- Initialization --- //
static int top = 0;

void initialize_array();

// --- Command --- // 

void write_array_command_all(uint16_t instruction);

void write_array_command(uint16_t instruction);

void write_array_line(uint8_t line, uint8_t pattern);

// --- Test --- //
void test_led_array();

void enable_test();

void disable_test();

void enable_shutdown();

void disable_shutdown();

void blank();

void increment_bar_test();

#endif