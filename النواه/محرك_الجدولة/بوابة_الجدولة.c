#include "بوابة_الجدولة.h"
#include "محرك_الجدولة/محرك_الجدولة.h"

// ================= Wrappers =================

static void api_init() {
    scheduler_init();
}

static void api_add(process_t* p) {
    scheduler_add(p);
}

static void api_remove(process_t* p) {
    scheduler_remove(p);
}

static process_t* api_next() {
    return scheduler_next();
}

static void api_schedule() {
    schedule();
}

static void api_sleep(process_t* p, int ticks) {
    scheduler_sleep(p, ticks);
}

static process_t* api_current() {
    return scheduler_current();
}

// ================= API =================

scheduler_api_t scheduler_api = {
    .init     = api_init,
    .add      = api_add,
    .remove   = api_remove,
    .next     = api_next,
    .schedule = api_schedule,
    .sleep    = api_sleep,
    .current  = api_current
};
