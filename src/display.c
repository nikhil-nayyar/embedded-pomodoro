#include <math.h>

#include "font.h"
#include "utility.h"
#include "display.h"


// Cursor Characteristics
typedef struct {
    int w;
    int h;
    int x; // start at origin
    int y; // start at origin
} Cursor;
static Cursor cursor = {8, 7, 0, 0};

void move_cursor(){

    // move cursor to right by character width
    cursor.x += cursor.w;

    // if cursor position exceeds width, move to next row
    if(cursor.x > XMAX){
        cursor.x = 0;
        cursor.y+= cursor.h;
    }

    // if cursor position exceeds height, wrap to origin
    if(cursor.y > YMAX){
        cursor.y = 0;
    }
}

// Write Addressing 
static void set_col_address(uint16_t start, uint16_t end){
    send_ssi_command(0x2A);
    send_ssi_data(0x0);
    send_ssi_data( (0xFF & start));
    send_ssi_data(0x0);
    send_ssi_data( (0xFF & end));
}

static void set_row_address(uint16_t start, uint16_t end){
    send_ssi_command(0x2B);
    send_ssi_data(0x0);
    send_ssi_data( (0xFF & start));
    send_ssi_data(0x0);
    send_ssi_data( (0xFF & end));
}

void set_address(uint16_t XS, uint16_t XE, uint16_t YS, uint16_t YE){
    set_row_address(YS,YE);
    set_col_address(XS,XE);
}

// Write Data

void send_data(){
    send_ssi_command(0x2C);
}

void send_pixel_color(uint8_t red, uint8_t green, uint8_t blue){
    char byte1 = 0x00 | (red & 0x3F) << 3 | (green & 0x38) >> 3;
    char byte2 = 0x00 | (green & 0x07) << 5 | (blue & 0x3F) >> 0;

    send_ssi_data(byte1);
    send_ssi_data(byte2);
}

void fill_display(uint8_t red, uint8_t green, uint8_t blue){

    set_col_address(0,127);
    set_row_address(0,159);

    send_ssi_command(0x2C);
    for(int r = 0; r <= 127; r++){
        for(int c = 0; c <= 159; c++){
           send_pixel_color(red, green, blue);
        }
    }

}

void write_character(char input){

    set_col_address(cursor.x, cursor.x+cursor.w-1);
    set_row_address(cursor.y, cursor.y+cursor.h-1);
    send_data();

    char byte1 = 0;
    char byte2 = 0;

    for(int r = 0; r < cursor.h; r++){
        for(int c = 0; c < cursor.w; c++){
           byte1 = ( 0x00 | (0xFF & 0x3F) << 3 | (0xFF & 0x38) >> 3 ) * font[input][r][c];
           byte2 = ( 0x00 | (0xFF & 0x07) << 5 | (0xFF & 0x3F) >> 0 ) * font[input][r][c];

            int fg = font[input][r][c];
            int red = 0 * fg;
            int blue = 0 * fg;
            int green = 255 * fg;

            send_pixel_color(red, green, blue);
        }
    }

    move_cursor();

}

void draw_v_line(uint16_t index){
    set_address(index, index, YMIN, YMAX);
    send_data();
    for(int i = YMIN; i < YMAX; i++){
        send_pixel_color(255,255,255);
    }
}

void draw_h_line(uint16_t index){
    set_address(XMIN, XMAX, index, index);
    send_data();
    for(int i = 0; i < XMAX; i++){
        send_pixel_color(255, 255, 255);
    }
}

void draw_rectangle(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2){
    set_address(x1,x2,y1,y2);
    send_data();
    for(int r = x1; r < x2-x1; r++){
        for(int c = y1; c < y2-y1; c++){
            send_pixel_color(255,255,0);
        }
    }
}
