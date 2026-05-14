#include "مدير_التركيب_الرسومي.h"
#include "محرك_الفيديو/بوابة_الفيديو.h"
#include "أرض_المستخدم/النظام/الخدمات/محرك_العرض/مدير_الرسم/مدير_الرسم.h"
#include "أرض_المستخدم/النظام/الخدمات/محرك_العرض/اداره_محرك_العرض/مدير_الخلفية/مدير_الخلفية.h"

#define TRANSPARENT 0xFFFFFFFF  //  لون خاص للشفافية

extern window_t windows[];
extern int window_count;

// رسم نافذة داخل back buffer
static void blit_window(window_t* win) {

    uint32_t screen_w = video_api.width();
    uint32_t screen_h = video_api.height();
    uint32_t pitch    = video_api.pitch();

    uint32_t* back = video_api.get_back_buffer();

    if (!win->buffer) return;

    for (int y = 0; y < win->h; y++) {

        int screen_y = win->y + y;
        if (screen_y < 0 || screen_y >= (int)screen_h)
            continue;

        for (int x = 0; x < win->w; x++) {

            int screen_x = win->x + x;
            if (screen_x < 0 || screen_x >= (int)screen_w)
                continue;

            uint32_t color = win->buffer[y * win->w + x];

            //  فقط تجاهل الشفافية الحقيقية
            if (color != TRANSPARENT) {
                back[screen_y * pitch + screen_x] = color;
            }
        }
    }
}

void compositor_render() {

    uint32_t screen_w = video_api.width();
    uint32_t screen_h = video_api.height();
    uint32_t pitch    = video_api.pitch();

    uint32_t* back = video_api.get_back_buffer();

    if (!back) return;

    //  مسح الشاشة
    for (uint32_t y = 0; y < screen_h; y++) {
        for (uint32_t x = 0; x < screen_w; x++) {
            back[y * pitch + x] = 0x000000;
        }
    }

    //  الخلفية
    background_render();

    //  النوافذ
    for (int i = 0; i < window_count; i++) {

        if (!windows[i].visible)
            continue;

        blit_window(&windows[i]);
    }
}
