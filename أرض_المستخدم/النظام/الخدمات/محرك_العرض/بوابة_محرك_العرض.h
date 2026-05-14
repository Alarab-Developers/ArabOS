#ifndef DISPLAY_API_H
#define DISPLAY_API_H

#include <stdint.h>

typedef struct {

    /* النظام */
    int  (*init)();
    void (*render)();
    void (*shutdown)();

    /* النوافذ */
    int (*create_window)(
        int x,
        int y,
        int width,
        int height,
        uint32_t color
    );

    void (*move_window)(
        int id,
        int dx,
        int dy
    );

    /* الرسم */
    void (*draw_rect)(
        int x,
        int y,
        int width,
        int height,
        uint32_t color
    );
    
    void (*set_window_text)(
        int id,
        const char* text
    );
    void (*set_window_color)(
        int win,
        uint32_t color
    );
    void (*append_text)(
        int win,
        const char* text
    );

} display_api_t;

extern display_api_t display_api;

#endif
