#ifndef FONT
#define FONT

#include <stdint.h>

typedef struct
{
    uint8_t WIDTH;
    uint8_t HEIGHT;
    uint8_t *font;
} Font;

extern uint8_t d_font_8x6[];
extern uint8_t d_font_32x16[];

#endif
