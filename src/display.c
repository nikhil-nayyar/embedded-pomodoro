#include <math.h>
#include <stdlib.h>

#include "font.h"
#include "utility.h"
#include "display.h"


// --- Cursor --- //

const int HBUFF = 15;
const int VBUFF = 15;

typedef struct {
    int x; // start at origin x = 4
    int y; // start at origin y = 8
} Cursor;
static Cursor cursor = {15, 15};

void move_cursor(int offset_x, int offset_y, int width, int height){

    // move cursor to right by character width
    cursor.x += offset_x;

    // if cursor position exceeds width, move to next row
    if(cursor.x > XMAX){
        cursor.x = 0;
        cursor.y+= height+offset_y;
    }

    // if cursor position exceeds height, wrap to origin
    if(cursor.y > YMAX){
        cursor.y = 0;
    }
    
}

// --- Addressing Commands --- //
static void set_col_address(uint16_t start, uint16_t end){
    send_ssi_command(CASET);
    send_ssi_data(0x0);
    send_ssi_data( (0xFF & start));
    send_ssi_data(0x0);
    send_ssi_data( (0xFF & end));
}

static void set_row_address(uint16_t start, uint16_t end){
    send_ssi_command(RASET);
    send_ssi_data(0x0);
    send_ssi_data( (0xFF & start));
    send_ssi_data(0x0);
    send_ssi_data( (0xFF & end));
}

static void set_address(uint16_t XS, uint16_t XE, uint16_t YS, uint16_t YE){
    set_row_address(YS,YE);
    set_col_address(XS,XE);
}

// --- Write Data Commands --- //
void send_data(){
    send_ssi_command(RAMWR);
}

void send_pixel(uint8_t red, uint8_t green, uint8_t blue){
    char byte1 = 0x00 | (red & 0x3F) << 3 | (green & 0x38) >> 3;
    char byte2 = 0x00 | (green & 0x07) << 5 | (blue & 0x3F) >> 0;

    send_ssi_data(byte1);
    send_ssi_data(byte2);
}

// --- Functional Draw Commands --- //
void draw_display(uint8_t red, uint8_t green, uint8_t blue){
    set_col_address(0,127);
    set_row_address(0,159);

    send_ssi_command(0x2C);
    for(int r = 0; r <= 127; r++){
        for(int c = 0; c <= 159; c++){
           send_pixel(red, green, blue);
        }
    }
}

void write_character(char input){

    set_col_address(cursor.x, cursor.x-1);
    set_row_address(cursor.y, cursor.y-1);
    send_data();

    char byte1 = 0;
    char byte2 = 0;

    for(int r = 0; r < 7; r++){
        for(int c = 0; c < 8; c++){

            int fg = font[input][r][c];
            int red = 0 * fg;
            int blue = 0 * fg;
            int green = 255 * fg;

            send_pixel(red, green, blue);
        }
    }

    move_cursor(7,8,7,8);

}

int chtoi(char c){
    int current = 0;
    if (c >= 'A' && c <= 'F'){
        current = c - 65 + 10;
    } else if (c >= '0' && c <= '9'){
        current = c - 48;
    } else{
        current = 0;
    }
    return current & 0xF;
}

void write_test_character(char letter){

    Character input = test_font[letter-32];

    // Generate Grid Addresses
    // TODO: create struct to store area
    int xs = cursor.x + input.dWidth;
    int xe = xs + (input.width - 1);

    int ye = cursor.y - input.dHeight;
    int ys = ye - (input.height - 1) ;

    set_address(xs, xe, ys, ye);
    send_data();

    // Generate Bitmap
    int width = input.width;
    int pow = 1;
    int temp = input.width;
    while(temp >>= 1){
        pow++;
    }
    int h = (0x1 << pow) >> 2; // divide by 4
    int padding = (0x1 << pow) - width; 

    // debug
    int pixels_sent = 0;

    for(int r = 0; r < input.height * 2; r+=2){

        unsigned char halfbyte1 = chtoi(input.bitmap[r]);
        unsigned char halfbyte2 = chtoi(input.bitmap[r+1]);

        uint16_t row = (halfbyte1 << 4) | (halfbyte2);
        row>>=padding;
        row = reverse_byte(row);
        for(int i = 0; i < width; i++){
            int mask = row & 0x1;
            send_pixel(
                255*mask,
                255*mask,
                255*mask
            );
            pixels_sent++;
            row>>=1;
        }
    }


    move_cursor(input.dWidth, input.dHeight, input.width, input.height);

}

void draw_v_line(uint16_t index){
    set_address(index, index, YMIN, YMAX);
    send_data();
    for(int i = YMIN; i < YMAX; i++){
        send_pixel(255,255,255);
    }
}

void draw_h_line(uint16_t index){
    set_address(XMIN, XMAX, index, index);
    send_data();
    for(int i = 0; i < XMAX; i++){
        send_pixel(255, 255, 255);
    }
}

void draw_rectangle(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2){
    set_address(x1,x2,y1,y2);
    send_data();
    for(int r = x1; r < x2-x1; r++){
        for(int c = y1; c < y2-y1; c++){
            send_pixel(255,255,0);
        }
    }
}
