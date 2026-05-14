#include "بوابة_محرك_العرض.h"

#include "أرض_المستخدم/النظام/الخدمات/محرك_العرض/محرك_العرض.h"

#include "أرض_المستخدم/النظام/الخدمات/محرك_العرض/مدير_النوافذ/مدير_النوافذ.h"

#include "أرض_المستخدم/النظام/الخدمات/محرك_العرض/مدير_الرسم/مدير_الرسم.h"



/* ========================================================= */
/* النظام */
/* ========================================================= */

static int api_init() {

    window_manager_init();

    return 1;
}

static void api_render() {

    display_render();
}

static void api_shutdown() {

}



/* ========================================================= */
/* النوافذ */
/* ========================================================= */

static int api_create_window(
    int x,
    int y,
    int width,
    int height,
    uint32_t color
) {

    return window_create(
        x,
        y,
        width,
        height,
        color
    );
}

static void api_move_window(
    int id,
    int dx,
    int dy
) {

    window_move(id, dx, dy);
}



/* ========================================================= */
/* الرسم */
/* ========================================================= */

static void api_draw_rect(
    int x,
    int y,
    int width,
    int height,
    uint32_t color
) {

    draw_rect(
        x,
        y,
        width,
        height,
        color
    );
}

static void api_set_window_text(
    int id,
    const char* text
) {

    window_set_text(id, text);
}

static void api_set_window_color(
    int win,
    uint32_t color
) {
    window_set_color(win, color);
}

static void api_append_text(
    int win,
    const char* text
) {

    append_text_to_window(
        win,
        text
    );
}

/* ========================================================= */
/* API TABLE */
/* ========================================================= */

display_api_t display_api = {

    /* النظام */
    .init = api_init,
    .render = api_render,
    .shutdown = api_shutdown,

    /* النوافذ */
    .create_window = api_create_window,
    .move_window = api_move_window,

    /* الرسم */
    .append_text = api_append_text,
    .draw_rect = api_draw_rect,
    .set_window_text = api_set_window_text,
    .set_window_color = api_set_window_color,
};
