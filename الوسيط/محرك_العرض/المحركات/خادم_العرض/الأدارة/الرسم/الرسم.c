#include "الرسم.h"

void draw_pixel(
    uint32_t* fb,
    uint32_t pitch,
    int x,
    int y,
    uint32_t color
) {
    fb[y * pitch + x] = color;
}

void clear_screen(
    uint32_t* fb,
    uint32_t width,
    uint32_t height,
    uint32_t pitch,
    uint32_t color
) {
    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            fb[y * pitch + x] = color;
        }
    }
}

void fill_rect(
    uint32_t* fb,
    uint32_t pitch,
    int x,
    int y,
    int w,
    int h,
    uint32_t color
) {
    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            fb[py * pitch + px] = color;
        }
    }
}

void draw_rect(
    uint32_t* fb,
    uint32_t pitch,
    int x,
    int y,
    int w,
    int h,
    uint32_t color
) {
    for (int i = 0; i < w; i++) {
        fb[y * pitch + (x + i)] = color;
        fb[(y + h - 1) * pitch + (x + i)] = color;
    }

    for (int i = 0; i < h; i++) {
        fb[(y + i) * pitch + x] = color;
        fb[(y + i) * pitch + (x + w - 1)] = color;
    }
}

void draw_line(
    uint32_t* fb,
    uint32_t pitch,
    int x0,
    int y0,
    int x1,
    int y1,
    uint32_t color
) {
    int dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int sx = (x0 < x1) ? 1 : -1;

    int dy = -((y1 > y0) ? (y1 - y0) : (y0 - y1));
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx + dy;

    while (1) {

        fb[y0 * pitch + x0] = color;

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = err * 2;

        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }

        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}
