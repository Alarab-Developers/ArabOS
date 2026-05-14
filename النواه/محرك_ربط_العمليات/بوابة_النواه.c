#include "بوابة_النواه.h"
#include "محرك_ربط_العمليات/محرك_ربط_العمليات.h"

#include "أرض_المستخدم/النظام/الخدمات/محرك_العرض/بوابة_محرك_العرض.h"
#include "أرض_المستخدم/النظام/التعريفات/المؤقت/المؤقت.h"
#include "محرك_الفيديو/بوابة_الفيديو.h"
#include "محرك_المقاطعات/بوابة_المقاطعات.h"
/* ========================================================= */
/* WINDOW */
/* ========================================================= */

static int api_window_create(
    int x,
    int y,
    int w,
    int h,
    uint32_t color
) {

    return display_api.create_window(
        x,
        y,
        w,
        h,
        color
    );
}

static void api_draw_pixel(
    int x,
    int y,
    uint32_t color
) {

    display_api.draw_rect(
        x,
        y,
        1,
        1,
        color
    );
}

static void api_set_window_text(
    int id,
    const char* text
) {

    display_api.set_window_text(
        id,
        text
    );
}

static void api_render()
{
    display_api.render();
}

static void api_append_text(
    int win,
    const char* text
) {

    display_api.append_text(
        win,
        text
    );
}

/* ========================================================= */
/* PORT IO */
/* ========================================================= */



static void api_outb(
    uint16_t port,
    uint8_t value
)
{
    interrupt_api.outb(
        port,
        value
    );
}

static uint8_t api_inb(
    uint16_t port
)
{
    return interrupt_api.inb(
        port
    );
}

/* ========================================================= */
/* TIMER */
/* ========================================================= */



static uint64_t api_timer_ticks()
{
    return (uint64_t)ticks;
}

/* ========================================================= */
/* VIDEO */
/* ========================================================= */

static uint32_t* api_video_map()
{
    /* IMPORTANT:
       return REAL framebuffer
       not kernel backbuffer
    */

    return fb_get_front_buffer();
}

static uint32_t api_screen_width()
{
    return video_api.width();
}

static uint32_t api_screen_height()
{
    return video_api.height();
}

static uint32_t api_screen_pitch()
{
    return video_api.pitch();
}

/* DIRECT FRONTBUFFER:
   no swap needed
*/

static void api_video_present()
{
}

/* ========================================================= */
/* API TABLE */
/* ========================================================= */

kernel_api_t kapi = {

    /* window */
    .window_create = api_window_create,
    .draw_pixel = api_draw_pixel,
    .set_window_text = api_set_window_text,
    .render = api_render,
    .append_text = api_append_text,

    /* ipc */
    .ipc_send = ipc_send,
    .ipc_receive = ipc_receive,

    /* port io */
    .outb = api_outb,
    .inb  = api_inb,

    /* timer */
    .timer_ticks = api_timer_ticks,

    /* video */
    .video_map = api_video_map,
    .screen_width = api_screen_width,
    .screen_height = api_screen_height,
    .screen_pitch = api_screen_pitch,
    .video_present = api_video_present,
};
