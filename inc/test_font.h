#ifndef TEST_FONT_H
#define TEST_FONT_H

typedef struct {
    int width;
    int height;
    int offX;
    int offY;
    int dWidth;
    int dHeight;
    char bitmap[25];
} Character;

#endif