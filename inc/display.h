#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "bdf_font.h"

// --- Macro Definitions --- //

/* Resolution for 128x160 SPI display */
#define XMIN 0
#define XMAX 127
#define YMIN 0
#define YMAX 160

/* SPI display instructions */
#define CASET 0x2A
#define RASET 0x2B
#define RAMWR 0x2C

// --- Fonts --- //
extern const Character bdf_font[];

// --- Display Functions --- //

/**
 * write character to display
 * 
 * @param character
 *  character to display
*/
void write_character(char character);

/**
 * draw vertical line on display
 * 
 * @param col
 *  column to hold line
 */
void draw_v_line(uint16_t col);

/**
 * draw horizontal line on display
 * 
 * @param row
 *  row to hold line
 */
void draw_h_line(uint16_t row);

/**
 * draw rectange on display
 * 
 * @param xs
 *  starting x position
 * @param xe
 *  ending x position
 * @param ys
 *  starting y position
 * @param ye
 *  ending y position
 */
void draw_rectangle(uint16_t xs, uint16_t xe, uint16_t ys, uint16_t ye);

/**
 * fill entire display with designated rgb color
 * 
 * @param red
 *  0-255 r value
 * @param green
 *  0-255 g value
 * @param blue
 *  0-255 b value
*/
void fill_display(uint8_t red, uint8_t green, uint8_t blue);

#endif