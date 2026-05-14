#ifndef VIDEO_API_H
#define VIDEO_API_H

#include <stdint.h>

#include "محرك_الفيديو/محرك_الفيديو.h"

typedef struct {

    /* framebuffer */

    void (*init)(
        void* fb_info
    );

    /* drawing */

    void (*putpixel)(
        int x,
        int y,
        uint32_t color
    );

    void (*clear)(
        uint32_t color
    );

    void (*swap)();

    /* screen info */

    uint32_t (*width)();
    uint32_t (*height)();
    uint32_t (*pitch)();

    /* framebuffer access */

    uint32_t* (*get_back_buffer)();

} video_api_t;

extern video_api_t video_api;

#endif
