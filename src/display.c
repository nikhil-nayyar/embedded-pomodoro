#include <math.h>
#include <stdlib.h>

#include "bdf_font.h"
#include "bsp.h"
#include "utility.h"
#include "display.h"
#include "tm4c123gh6pm.h"

// --- Initialization --- //

void initialize_display()
{

    // --- enable modules --- //
    // SSI1
    write_reg_mask(&SYSCTL_RCGCSSI_R, SYSCTL_RCGCSSI_R1,SYSCTL_RCGCSSI_R1, BIT_ON);

    // Port D
    initialize_port_clock(D);
    // Enable use of PD7
    write_reg_mask(&GPIO_PORTD_LOCK_R, GPIO_LOCK_M, GPIO_LOCK_KEY, VALUE);
    write_reg_bit(&GPIO_PORTD_CR_R, 7, ON);

    // reserve PD167 for display initialization
    const int size = 3;
    uint8_t indicies[3] = {1, 6, 7};
    write_reg_bits(&GPIO_PORTD_DIR_R, indicies, size, ON);
    write_reg_bits(&GPIO_PORTD_AFSEL_R, indicies, size, OFF);
    write_reg_bits(&GPIO_PORTD_DEN_R, indicies, size, ON);
    write_reg_bits(&GPIO_PORTD_AMSEL_R, indicies, size, OFF);
    GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0x00FFFF0F)+0x00000000;

    write_data_bit(GPIO_PORTD_DATA_BITS_R, 1, OFF);
    write_data_bit(GPIO_PORTD_DATA_BITS_R, 7, ON);
    delay_10ms(50);
    write_data_bit(GPIO_PORTD_DATA_BITS_R, 7, OFF);
    delay_10ms(50);
    write_data_bit(GPIO_PORTD_DATA_BITS_R, 7, ON);
    delay_10ms(50);

    // assign SSI signals to GPIO pins
    const int size2 = 3;
    uint8_t indicies2[3] = {0,1,3};

    write_reg_bits(&GPIO_PORTD_AFSEL_R, indicies2, size2, ON);
    write_reg_bits(&GPIO_PORTD_DEN_R, indicies2, size2, ON);
    write_reg_bits(&GPIO_PORTD_AMSEL_R, indicies2, size2, OFF);
    write_reg_bits(&GPIO_PORTD_DR2R_R, indicies2, size2, ON);
    write_reg_mask(
        &GPIO_PORTD_PCTL_R,
        GPIO_PCTL_PD0_M | GPIO_PCTL_PD1_M | GPIO_PCTL_PD2_M | GPIO_PCTL_PD3_M,
        GPIO_PCTL_PD0_SSI1CLK | GPIO_PCTL_PD1_SSI1FSS | GPIO_PCTL_PD2_SSI1RX | GPIO_PCTL_PD3_SSI1TX,
        VALUE
    );

    // --- configure SSI --- //
    // disable SPI
    write_reg_mask(&SSI1_CR1_R, SSI_CR1_LBM, SSI_CR1_LBM, BIT_OFF);
    write_reg_mask(&SSI1_CR1_R, SSI_CR1_SSE, SSI_CR1_SSE, BIT_OFF);
    write_reg_mask(&SSI1_CR1_R, SSI_CR1_MS, SSI_CR1_MS, BIT_OFF);

    // configure SPI clock
    write_reg_mask(&SSI1_CC_R, SSI_CC_CS_M, SSI_CC_CS_SYSPLL, VALUE);
    write_reg_mask(&SSI1_CPSR_R, SSI_CPSR_CPSDVSR_M, 2 << SSI_CPSR_CPSDVSR_S, VALUE);
    write_reg_mask(&SSI1_CR0_R, SSI_CR0_SCR_M, 4 << SSI_CR0_SCR_S, VALUE);

    // configure SPI options
    write_reg_mask(&SSI1_CR0_R, SSI_CR0_SPH | SSI_CR0_SPO, SSI_CR0_SPH | SSI_CR0_SPO, BIT_OFF);
    write_reg_mask(&SSI1_CR0_R, SSI_CR0_FRF_M, SSI_CR0_FRF_MOTO, VALUE);
    write_reg_mask(&SSI1_CR0_R, SSI_CR0_DSS_M, SSI_CR0_DSS_8, VALUE);

    // enable SPI
    write_reg_mask(&SSI1_CR1_R, SSI_CR1_SSE, SSI_CR1_SSE, BIT_ON);
}

void configure_display(){

    send_lcd_command(0x01); // sw reset
    delay_10ms(5);
    send_lcd_command(0x11); // turn off sleep
    delay_10ms(50);
    send_lcd_command(0x13); // turn on normal display mode
    delay_10ms(1);
    send_lcd_command(0x20); // turn off display inversion
    send_lcd_command(0x13); // turn off partial display

    // set FR to 60fps
    send_lcd_command(0xB1);
    send_lcd_data(0x01);
    send_lcd_data(0x2C);
    send_lcd_data(0x2D);

    // display refresh settings
    send_lcd_command(0xB6);
    send_lcd_data(0x15);
    send_lcd_data(0x02);

    // set display inversion control
    send_lcd_command(0xB4);
    send_lcd_data(0x0);

    // set memory data access control
    send_lcd_command(0x36);
    send_lcd_data(0x00);

    // set color mode to 18bit
    send_lcd_command(0x3A);
    send_lcd_data(0x05);

    send_lcd_command(0x29); // turn on display
    delay_10ms(100);

}

void send_lcd_command(uint8_t command)
{

    while ((read_reg_by_mask(&SSI1_SR_R, SSI_SR_BSY) & SSI_SR_BSY) == SSI_SR_BSY)
    ;
    // set a0 low
    write_data_bit(GPIO_PORTD_DATA_BITS_R, 6, OFF);

    // write to data register
    write_reg(&SSI1_DR_R, command);

    // wait until command sent
    while (read_reg_by_mask(&SSI1_SR_R, SSI_SR_BSY) & SSI_SR_BSY)
    ;

}


void send_lcd_data(uint8_t command)
{

    while (read_reg_by_mask(&SSI1_SR_R, SSI_SR_TNF)==0)
    ;

    // set a0 high
    write_data_bit(GPIO_PORTD_DATA_BITS_R, 6, ON);

    // write to data register
    write_reg(&SSI1_DR_R, command);

}


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
    send_lcd_command(CASET);
    send_lcd_data(0x0);
    send_lcd_data( (0xFF & start));
    send_lcd_data(0x0);
    send_lcd_data( (0xFF & end));
}

static void set_row_address(uint8_t start, uint8_t end){
    send_lcd_command(RASET);
    send_lcd_data(0x0);
    send_lcd_data( (0xFF & start));
    send_lcd_data(0x0);
    send_lcd_data( (0xFF & end));
}

static void set_address(uint8_t XS, uint8_t XE, uint8_t YS, uint8_t YE){
    set_row_address(YS,YE);
    set_col_address(XS,XE);
}

// --- Write Data Commands --- //
void send_data(){
    send_lcd_command(RAMWR);
}

void send_pixel(Color color){
    char byte1 = 0x00 | (color.red & 0x3F) << 3 | (color.green & 0x38) >> 3;
    char byte2 = 0x00 | (color.green & 0x07) << 5 | (color.blue & 0x3F) >> 0;

    send_lcd_data(byte1);
    send_lcd_data(byte2);
}

// --- Functional Commands --- //
void fill_display(Color color){
    set_col_address(0,127);
    set_row_address(0,159);

    send_lcd_command(0x2C);
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


