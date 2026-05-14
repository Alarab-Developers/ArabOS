#include "بوابة_محرك_التعريفات.h"
#include "محرك_التعريفات.h"

#include "أرض_المستخدم/النظام/التعريفات/مؤشرات_الفأرة/مؤشر_الفأرة_الافتراضي/مؤشر_الفأرة_الافتراضية.h"
#include "أرض_المستخدم/النظام/التعريفات/لوحات_المفاتيح/لوحة_المفاتيح_الافتراضية/لوحة_المفاتيح_الافتراضية.h"
#include "أرض_المستخدم/النظام/التعريفات/المؤقت/المؤقت.h"
#include "أرض_المستخدم/النظام/التعريفات/التخزين/ATA/ata.h"

/* ===== Mouse Driver ===== */

static mouse_driver_t mouse_driver = {

    .x = &mouse_x,

    .y = &mouse_y,

    .init = mouse_init,

    .handler = mouse_handler,

    .draw_cursor = draw_cursor
};
/* ===== Keyboard Driver ===== */

static keyboard_driver_t keyboard_driver = {

    .init = keyboard_init,

    .handler = keyboard_handler,

    .read = keyboard_read,

    .getchar = keyboard_getchar
};

/* ================= Timer Driver ================= */

static timer_driver_t timer_driver = {

    .ticks = &ticks,

    .init = timer_init,

    .handler = timer_handler
};

/* ===== ATA Driver ===== */

static ata_driver_t ata_driver = {

    .read_sector = ata_read_sector,

    .write_sector = ata_write_sector
};

/* ===== Wrappers ===== */

static void api_init() {

    driver_manager_init();
}

static int api_register_driver(
    const char* name,
    void* driver
) {

    return driver_register(
        name,
        driver
    );
}

static void* api_get_driver(
    const char* name
) {

    return driver_get(name);
}

/* ===== API ===== */

driver_api_t driver_api = {

    .init = api_init,

    .register_driver = api_register_driver,

    .get_driver = api_get_driver,

    .mouse = &mouse_driver,
    .keyboard = &keyboard_driver,
    .timer = &timer_driver,
    .ata = &ata_driver,
};
