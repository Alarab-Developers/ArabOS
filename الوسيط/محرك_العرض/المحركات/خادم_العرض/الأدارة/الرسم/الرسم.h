#ifndef الرسم_H
#define الرسم_H

#include <stdint.h>

void draw_pixel(
    uint32_t* fb,
    uint32_t pitch,
    int x,
    int y,
    uint32_t color
);

void draw_line(
    uint32_t* fb,
    uint32_t pitch,
    int x0,
    int y0,
    int x1,
    int y1,
    uint32_t color
);

void draw_rect(
    uint32_t* fb,
    uint32_t pitch,
    int x,
    int y,
    int w,
    int h,
    uint32_t color
);

void fill_rect(
    uint32_t* fb,
    uint32_t pitch,
    int x,
    int y,
    int w,
    int h,
    uint32_t color
);

void clear_screen(
    uint32_t* fb,
    uint32_t width,
    uint32_t height,
    uint32_t pitch,
    uint32_t color
);

#endif
