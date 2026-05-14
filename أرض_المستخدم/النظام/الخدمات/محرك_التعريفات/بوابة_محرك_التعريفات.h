#ifndef DRIVER_API_H
#define DRIVER_API_H

#include <stdint.h>

/* ================= Mouse Driver ================= */

typedef struct {

    int* x;

    int* y;

    void (*init)();

    void (*handler)();

    void (*draw_cursor)();

} mouse_driver_t;

/* ================= Keyboard Driver ================= */

typedef struct {

    void (*init)();

    void (*handler)();

    char (*read)();

    char (*getchar)();

} keyboard_driver_t;

/* ================= Timer Driver ================= */

typedef struct {

    volatile int* ticks;

    void (*init)();

    void (*handler)();

} timer_driver_t;

/* ================= ATA Driver ================= */

typedef struct {

    void (*read_sector)(
        uint32_t lba,
        uint8_t* buffer
    );

    void (*write_sector)(
        uint32_t lba,
        uint8_t* buffer
    );

} ata_driver_t;

/* ================= Driver API ================= */

typedef struct {

    void (*init)();

    int (*register_driver)(
        const char* name,
        void* driver
    );

    void* (*get_driver)(
        const char* name
    );

    /* ===== Mouse ===== */

    mouse_driver_t* mouse;

    /* ===== Keyboard ===== */

    keyboard_driver_t* keyboard;

    /* ===== Timer ===== */

    timer_driver_t* timer;
    /* ===== ATA ===== */
    ata_driver_t* ata;


} driver_api_t;


/* ===== البوابة العامة ===== */

extern driver_api_t driver_api;

#endif
