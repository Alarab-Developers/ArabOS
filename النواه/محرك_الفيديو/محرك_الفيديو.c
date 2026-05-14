#include "محرك_الفيديو.h"

#include "محرك_الذاكرة/بوابة_الذاكرة.h"

#include <stddef.h>

static uint32_t* fb;
static uint32_t* back_buffer;

static uint32_t fb_width;
static uint32_t fb_height;
static uint32_t fb_pitch;

static uint32_t buffer_size;

/* ========================================================= */
/* FAST COPY */
/* ========================================================= */

static void fast_copy(
    uint32_t* dest,
    uint32_t* src,
    uint32_t count
) {

    for (
        uint32_t i = 0;
        i < count;
        i++
    ) {

        dest[i] = src[i];
    }
}

/* ========================================================= */
/* INIT */
/* ========================================================= */

void fb_init(
    framebuffer_info_t* fb_info
) {

    fb = (uint32_t*)fb_info->address;

    fb_width  = fb_info->width;
    fb_height = fb_info->height;

    fb_pitch = fb_info->pitch / 4;

    buffer_size =
        fb_pitch *
        fb_height;

    back_buffer =
        memory_api.alloc(
            buffer_size *
            sizeof(uint32_t)
        );

    if (!back_buffer) {

        while (1);
    }

    fb_clear(0x00000000);
}

/* ========================================================= */
/* DRAW */
/* ========================================================= */

void fb_putpixel(
    int x,
    int y,
    uint32_t color
) {

    if (x < 0 || y < 0)
        return;

    if ((uint32_t)x >= fb_width)
        return;

    if ((uint32_t)y >= fb_height)
        return;

    back_buffer[
        y * fb_pitch + x
    ] = color;
}

void fb_clear(
    uint32_t color
) {

    for (
        uint32_t i = 0;
        i < buffer_size;
        i++
    ) {

        back_buffer[i] = color;
    }
}

void fb_swap_buffers()
{
    fast_copy(
        fb,
        back_buffer,
        buffer_size
    );
}

/* ========================================================= */
/* GETTERS */
/* ========================================================= */

uint32_t* fb_get_back_buffer()
{
    return back_buffer;
}

/* NEW */

uint32_t* fb_get_front_buffer()
{
    return fb;
}

uint32_t fb_get_width()
{
    return fb_width;
}

uint32_t fb_get_height()
{
    return fb_height;
}

uint32_t fb_get_pitch()
{
    return fb_pitch;
}

uint32_t fb_get_buffer_size()
{
    return buffer_size;
}
