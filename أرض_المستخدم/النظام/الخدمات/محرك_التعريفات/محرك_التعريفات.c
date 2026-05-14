#include "محرك_التعريفات.h"

#define MAX_DRIVERS 32

typedef struct {

    const char* name;
    void* driver;

} driver_entry_t;

static driver_entry_t drivers[MAX_DRIVERS];

static int driver_count = 0;

/* ================= INIT ================= */

void driver_manager_init() {

    driver_count = 0;
}

/* ================= REGISTER ================= */

int driver_register(
    const char* name,
    void* driver
) {
    if (driver_count >= MAX_DRIVERS)
        return -1;

    drivers[driver_count].name   = name;
    drivers[driver_count].driver = driver;

    driver_count++;

    return 0;
}

/* ================= GET ================= */

void* driver_get(
    const char* name
) {
    for (int i = 0; i < driver_count; i++) {

        const char* a = drivers[i].name;

        int j = 0;

        while (name[j] &&
               a[j] &&
               name[j] == a[j]) {
            j++;
        }

        if (name[j] == 0 &&
            a[j] == 0) {

            return drivers[i].driver;
        }
    }

    return 0;
}

/* ================= Console ================= */

void console_write(const char* str) {
    (void)str;
}

void console_clear() {

}
