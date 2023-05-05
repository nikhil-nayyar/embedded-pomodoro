#include <math.h>
#include <stdlib.h>

#include "bdf_font.h"
#include "bsp.h"
#include "utility.h"
#include "display.h"


// --- Cursor --- //

#define H_PADDING 7
#define V_PADDING 15

// default to bitbuntu font
static Cursor cursor = {
    0, 
    0,
    1,
    0,
    &bitbuntu
};

// --- Rendering --- //

static Settings settings = {
    { 255,255,255 },
    { 0,0,0 }
};

void render_display(){
    render_cursor();
    write_character('a');
    delay_10ms(100);
}

void render_cursor(){

    if(cursor.blink){
        cursor.blink_state = ~cursor.blink_state;
        Color color = cursor.blink_state ? settings.fg : settings.bg;
        Area area = {cursor.x, cursor.x + bitbuntu.width, cursor.y, cursor.y+bitbuntu.height};
        draw_rectangle(area,color);
    }
    
}


/**
 * move cursor according to bdf specification
*/
void move_cursor(int offset_x, int offset_y, int width, int height){

    // move cursor to right by character width
    cursor.x += offset_x;

    // if cursor position exceeds width, move to next row
    if(cursor.x > XMAX - H_PADDING){
        cursor.x = H_PADDING;
        cursor.y+= height+offset_y;
    }

    // if cursor position exceeds height, wrap to origin
    if(cursor.y > YMAX - V_PADDING){
        cursor.y = V_PADDING;
    }
    
}

// --- Addressing Commands --- //
static void set_col_address(uint8_t start, uint8_t end){
    send_ssi_command(CASET);
    send_ssi_data(0x0);
    send_ssi_data( (0xFF & start));
    send_ssi_data(0x0);
    send_ssi_data( (0xFF & end));
}

static void set_row_address(uint8_t start, uint8_t end){
    send_ssi_command(RASET);
    send_ssi_data(0x0);
    send_ssi_data( (0xFF & start));
    send_ssi_data(0x0);
    send_ssi_data( (0xFF & end));
}

static void set_address(uint8_t XS, uint8_t XE, uint8_t YS, uint8_t YE){
    set_row_address(YS,YE);
    set_col_address(XS,XE);
}

// --- Write Data Commands --- //
void send_data(){
    send_ssi_command(RAMWR);
}

void send_pixel(Color color){
    char byte1 = 0x00 | (color.red & 0x3F) << 3 | (color.green & 0x38) >> 3;
    char byte2 = 0x00 | (color.green & 0x07) << 5 | (color.blue & 0x3F) >> 0;

    send_ssi_data(byte1);
    send_ssi_data(byte2);
}

// --- Functional Draw Commands --- //
void fill_display(Color color){
    set_col_address(0,127);
    set_row_address(0,159);

    send_ssi_command(0x2C);
    for(int r = 0; r <= 127; r++){
        for(int c = 0; c <= 159; c++){
           send_pixel(color);
        }
    }
}

void draw_rectangle(Area area, Color color){
    set_address(area.xs, area.xe, area.ys, area.ye);
    send_data();
    for(int r = area.xs; r <= area.xe-area.xs; r++){
        for(int c = area.ys; c <= area.ye-area.ys; c++){
            send_pixel(color);
        }
    }
}

void write_character(char letter){

    Character input = *get_character(letter-32);

    // Generate Grid Addresses
    // TODO: create struct to store area
    /*
        Area area = {
        cursor.x + input.dWidth,
        cursor.x + input.dWidth + (input.width - 1),
        cursor.y - input.dHeight - 1,
        cursor.y - input.dHeight + input.height,
    };
    */
    int xs = cursor.x + input.dWidth;
    int xe = xs + (input.width - 1);

    int ye = input.height + cursor.y - input.dHeight;
    int ys = ye - (input.height - 1) ;

    // add cursor check
    if(xs < XMAX && xe > XMAX){
        // move to next row
        xs = XMIN;
        xe = xs + (input.width-1);
        ys = ys + 10;
        ye = ye+40;
    }

    if (ys < YMAX && ye > YMAX){
        // move to beginning of page
    }

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
                (Color){
                    255*mask,
                    255*mask,
                    255*mask
                }
            );
            pixels_sent++;
            row>>=1;
        }
    }

    move_cursor(input.dWidth, input.dHeight, input.width, input.height);

}

void draw_v_line(uint8_t col, Color color){
    set_address(col, col, YMIN, YMAX);
    send_data();
    for(int i = YMIN; i < YMAX; i++){
        send_pixel(color);
    }
}

void draw_h_line(uint8_t row, Color color){
    set_address(row, row, row, row);
    send_data();
    for(int i = 0; i < XMAX; i++){
        send_pixel(color);
    }
}


