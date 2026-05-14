#include <stdint.h>

#include "بوابة_النظام.h"

void _start(kernel_api_t* api)
{
    uint32_t* fb =
        api->video_map();

    uint32_t width =
        api->screen_width();

    uint32_t height =
        api->screen_height();

    uint32_t pitch =
        api->screen_pitch();

    for (uint32_t y = 0; y < height; y++) {

        for (uint32_t x = 0; x < width; x++) {

            fb[
                y * pitch + x
            ] = 0x00202020;
        }
    }

    /* مربع أبيض */

    for (int y = 100; y < 300; y++) {

        for (int x = 100; x < 300; x++) {

            fb[
                y * pitch + x
            ] = 0x00FFFFFF;
        }
    }


    while (1) {

    uint64_t t =
        api->timer_ticks();

    uint32_t color;

    if (t == 0)
        color = 0x00FF0000; // أحمر
    else
        color = 0x0000FF00; // أخضر

    /* تنظيف الشاشة */

    for (uint32_t y = 0; y < height; y++) {

        for (uint32_t x = 0; x < width; x++) {

            fb[
                y * pitch + x
            ] = 0x00202020;
        }
    }

    /* مربع بالمنتصف */

    int size = 200;

    int start_x =
        (width  - size) / 2;

        int start_y =
            (height - size) / 2;

        for (int y = 0; y < size; y++) {

            for (int x = 0; x < size; x++) {

                fb[
                    (start_y + y) * pitch +
                    (start_x + x)
                ] = color;
            }
        }

        api->video_present();
    }

}
