#include "لوحة_مفاتيح.h"

static int extended = 0;

static int keyboard_read(kernel_api_t* api)
{
    uint8_t sc =
        api->inb(0x60);

    if (sc == 0xE0)
    {
        extended = 1;
        return KEY_NONE;
    }

    if (extended)
    {
        extended = 0;

        switch (sc)
        {
            case 0x48:
                return KEY_UP;

            case 0x50:
                return KEY_DOWN;

            case 0x4B:
                return KEY_LEFT;

            case 0x4D:
                return KEY_RIGHT;
        }
    }

    return KEY_NONE;
}

void keyboard_run(kernel_api_t* api)
{
    while (1)
    {
        int key =
            keyboard_read(api);

        if (key == KEY_LEFT)
        {
            /* حدث سهم يسار */
        }

        if (key == KEY_RIGHT)
        {
            /* حدث سهم يمين */
        }

        if (key == KEY_UP)
        {
            /* حدث سهم أعلى */
        }

        if (key == KEY_DOWN)
        {
            /* حدث سهم أسفل */
        }
    }
}
