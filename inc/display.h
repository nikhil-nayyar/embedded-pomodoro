#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "application.h"
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

// --- structs --- //

typedef struct{
    uint8_t xs;
    uint8_t xe;
    uint8_t ys;
    uint8_t ye;
} Area;

typedef struct{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Color;

typedef struct {
    int x; 
    int y; 
    short blink;
    short blink_state;
    Font *font;
} Cursor;

typedef struct{
    Color fg;
    Color bg;
} Settings;

// --- Initialization --- //
/*
 * Function: send ssi command
 * ----------------------------
 *   send command instruction via SSI
 *
 *   command: 8 bit command instruction
 *
 */
void send_lcd_command(uint8_t command);

/*
 * Function: send ssi data
 * ----------------------------
 *   send data instruction via ssi
 *
 *   command: 8 bit command instruction
 * 
 */
void send_lcd_data(uint8_t data);


/*
 * Function: initialize ssi
 * ----------------------------
 *   setup ssi module on MCU
 *
 * 
 */
void initialize_display();


/*
 * Function: configure display
 * ----------------------------
 *   send series of commands/data to setup display
 *
 * 
 */
void configure_display();


// --- Rendering --- //

/**
 * series of rendering functions to call
 * for any given event
*/
void render_display();


// --- Cursor --- //

void render_cursor();

// --- Fonts --- //
extern const Font bitbuntu;

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
void draw_v_line(uint8_t col, Color color);

/**
 * draw horizontal line on display
 * 
 * @param row
 *  row to hold line
 */
void draw_h_line(uint8_t row, Color color);


/**
 * fill entire display with designated rgb color
 * 
 * @param Color
 *  Color to fill display
*/
void fill_display(Color color);

/**
 * draw rectangle on display
 * 
 * @param area
 *  Area to draw rectangle over
 * @param color
 *  Color for rectangle
 */
void draw_rectangle(Area area, Color color);

#endif