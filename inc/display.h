#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "test_font.h"

// --- Macro Definitions --- //
#define XMIN 0
#define XMAX 127
#define YMIN 0
#define YMAX 160

#define CASET 0x2A
#define RASET 0x2B
#define RAMWR 0x2C

// --- Fonts --- //
extern const uint8_t font[256][7][8];
extern const Character test_font[];

void draw_display(uint8_t red, uint8_t green, uint8_t blue);
void write_character(char input);
void write_test_character(char letter);

void draw_v_line(uint16_t index);
void draw_h_line(uint16_t index);
void draw_rectangle(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2);

#endif