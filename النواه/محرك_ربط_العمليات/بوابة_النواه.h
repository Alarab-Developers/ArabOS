#ifndef KERNEL_GATEWAY_H
#define KERNEL_GATEWAY_H

#include <stdint.h>

#include "محرك_ربط_العمليات/محرك_ربط_العمليات.h"

typedef struct {

    /* ===================================================== */
    /* WINDOW API */
    /* ===================================================== */

    int (*window_create)(
        int x,
        int y,
        int w,
        int h,
        uint32_t color
    );

    void (*draw_pixel)(
        int x,
        int y,
        uint32_t color
    );

    void (*set_window_text)(
        int id,
        const char* text
    );

    void (*render)();

    void (*append_text)(
        int win,
        const char* text
    );

    /* ===================================================== */
    /* IPC */
    /* ===================================================== */

    int (*ipc_send)(
        int pid,
        Message* msg
    );

    int (*ipc_receive)(
        Message* msg
    );

    /* ===================================================== */
    /* PORT IO */
    /* ===================================================== */

    void (*outb)(
        uint16_t port,
        uint8_t value
    );

    uint8_t (*inb)(
        uint16_t port
    );
    
    uint64_t (*timer_ticks)();

    /* ===================================================== */
    /* VIDEO DIRECT ACCESS */
    /* ===================================================== */

    uint32_t* (*video_map)();

    uint32_t (*screen_width)();
    uint32_t (*screen_height)();
    uint32_t (*screen_pitch)();

    void (*video_present)();

} kernel_api_t;

extern kernel_api_t kapi;

#endif
