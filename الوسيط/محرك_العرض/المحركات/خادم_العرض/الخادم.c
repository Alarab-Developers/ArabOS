#include <stdint.h>

#include "الخادم.h"
#include "الأدارة/الرسم/الرسم.h"


void server_run(kernel_api_t* api)
{
    uint32_t* fb =
        api->video_map();

    uint32_t width =
        api->screen_width();

    uint32_t height =
        api->screen_height();

    uint32_t pitch =
        api->screen_pitch();

    while (1) {

        int mx =
            api->mouse_get_x();

        int my =
            api->mouse_get_y();

        /* تنظيف الشاشة باللون الأزرق */
        for (uint32_t y = 0; y < height; y++) {

            for (uint32_t x = 0; x < width; x++) {

                fb[
                    y * pitch + x
                ] = 0x000000FF;
            }
        }

        /* إبقاء المؤشر داخل الشاشة */
        if (mx < 0)
            mx = 0;

        if (my < 0)
            my = 0;

        if (mx > (int)width - 16)
            mx = width - 16;

        if (my > (int)height - 16)
            my = height - 16;


        fill_rect(
            fb,
            pitch,
            mx,
            my,
            16,
            16,
            0x00FFFFFF
        );


            
        
    

        api->video_present();
        api->swap();
    }
}
