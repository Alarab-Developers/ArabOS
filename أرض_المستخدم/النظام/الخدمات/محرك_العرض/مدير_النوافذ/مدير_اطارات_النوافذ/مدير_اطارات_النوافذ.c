#include "مدير_اطارات_النوافذ.h"
#include "أرض_المستخدم/النظام/الخدمات/محرك_العرض/مدير_الرسم/مدير_الرسم.h"

#define FRAME_COLOR    0x222222
#define TITLE_COLOR    0x4444AA
#define TITLE_LIGHT    0x6666CC
#define CLOSE_COLOR    0xFF4444
#define MAXIMIZE_COLOR 0x44FF44

#define RADIUS 6
#define TRANSPARENT 0xFFFFFFFF

// دمج لونين
static uint32_t blend(uint32_t c1, uint32_t c2, float t) {
    uint8_t r1 = (c1 >> 16) & 0xFF;
    uint8_t g1 = (c1 >> 8) & 0xFF;
    uint8_t b1 = c1 & 0xFF;

    uint8_t r2 = (c2 >> 16) & 0xFF;
    uint8_t g2 = (c2 >> 8) & 0xFF;
    uint8_t b2 = c2 & 0xFF;

    uint8_t r = r1 + (r2 - r1) * t;
    uint8_t g = g1 + (g2 - g1) * t;
    uint8_t b = b1 + (b2 - b1) * t;

    return (r << 16) | (g << 8) | b;
}

//  دالة موحدة لقص الزوايا (زوايا النوافذ)
static int inside_rounded(window_t* win, int x, int y) {

    if (win->is_maximized)
        return 1;


    if (x < RADIUS && y < RADIUS) {
        int dx = RADIUS - x;
        int dy = RADIUS - y;
        if (dx*dx + dy*dy > RADIUS*RADIUS) return 0;
    }


    if (x >= win->w - RADIUS && y < RADIUS) {
        int dx = x - (win->w - RADIUS - 1);
        int dy = RADIUS - y;
        if (dx*dx + dy*dy > RADIUS*RADIUS) return 0;
    }


    if (x < RADIUS && y >= win->h - RADIUS) {
        int dx = RADIUS - x;
        int dy = y - (win->h - RADIUS - 1);
        if (dx*dx + dy*dy > RADIUS*RADIUS) return 0;
    }


    if (x >= win->w - RADIUS && y >= win->h - RADIUS) {
        int dx = x - (win->w - RADIUS - 1);
        int dy = y - (win->h - RADIUS - 1);
        if (dx*dx + dy*dy > RADIUS*RADIUS) return 0;
    }

    return 1;
}

void window_draw_frame(window_t* win) {
    if (!win->buffer) return;
    if (win->w < 30 || win->h < 30) return;

    /* ================= TITLE BAR ================= */

    for (int y = 0; y < 18 && y < win->h; y++) {


        //  نفس لون الخلفية مع تفتيح خفيف (Gradient احترافي)
        uint32_t color = win->color;

        //  خط علوي خفيف (شريط العنوان)
        if (y == 0)
            color = blend(win->color, 0x000000, 0.25f);

        for (int x = 0; x < win->w; x++) {

            // عدم الرسم خارج الزوايا الدائرية
            if (!inside_rounded(win, x, y)) {
                win->buffer[y * win->w + x] = TRANSPARENT;
                continue;
            }

            win->buffer[y * win->w + x] = color;
        }
    }

    /* ================= BUTTONS ================= */

    int close_x = win->w - 14;
    int close_y = 3;

    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {

            int px = close_x + x;
            int py = close_y + y;

            if (!inside_rounded(win, px, py)) continue;

            win->buffer[py * win->w + px] =
                blend(CLOSE_COLOR, 0x000000, (float)y / 10);
        }
    }

    int max_x = win->w - 28;
    int max_y = 3;

    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {

            int px = max_x + x;
            int py = max_y + y;

            if (!inside_rounded(win, px, py)) continue;

            win->buffer[py * win->w + px] =
                blend(MAXIMIZE_COLOR, 0x000000, (float)y / 10);
        }
    }

    /* ================= FRAME ================= */

    for (int x = RADIUS; x < win->w - RADIUS; x++) {
        win->buffer[x] = FRAME_COLOR;
        win->buffer[(win->h - 1) * win->w + x] = FRAME_COLOR;
    }

    for (int y = RADIUS; y < win->h - RADIUS; y++) {
        win->buffer[y * win->w] = FRAME_COLOR;
        win->buffer[y * win->w + (win->w - 1)] = FRAME_COLOR;
    }
}
