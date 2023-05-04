#ifndef BDF_FONT_H
#define BDF_FONT_H

/* groups together character parameters per Glyph Bitmap Distribution Format (BDF) specification*/
typedef struct {
    int width; // width of BLACKSPACE
    int height; // height of BLACKSPACE
    int offX; // offset from ORIGIN to start bitmap
    int offY; // offset from ORIGIN to start bitmap
    int dWidth; // movement of origin x coordinate after character write
    int dHeight; // movement of origin y coordinate after character write
    char bitmap[25]; // bitmap defined as string of hex characters
} Character;

#endif