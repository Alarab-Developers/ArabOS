#include "بوابة_الفيديو.h"

#include "محرك_الفيديو/محرك_الفيديو.h"

/* ========================================================= */
/* WRAPPERS */
/* ========================================================= */

static void api_init(
    void* fb_info
) {

    fb_init(
        (framebuffer_info_t*)fb_info
    );
}

static void api_putpixel(
    int x,
    int y,
    uint32_t color
) {

    fb_putpixel(
        x,
        y,
        color
    );
}

static void api_clear(
    uint32_t color
) {

    fb_clear(color);
}

static void api_swap()
{
    fb_swap_buffers();
}

static uint32_t api_width()
{
    return fb_get_width();
}

static uint32_t api_height()
{
    return fb_get_height();
}

static uint32_t api_pitch()
{
    return fb_get_pitch();
}

static uint32_t* api_get_back_buffer()
{
    return fb_get_back_buffer();
}

/* ========================================================= */
/* API TABLE */
/* ========================================================= */

video_api_t video_api = {

    .init = api_init,

    .putpixel = api_putpixel,
    .clear = api_clear,
    .swap = api_swap,

    .width = api_width,
    .height = api_height,
    .pitch = api_pitch,

    .get_back_buffer = api_get_back_buffer
};
