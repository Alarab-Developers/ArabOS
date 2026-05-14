#include "بوابة_العمليات.h"

//  اربط مع المحركات الحقيقية
#include "محرك_العمليات/محرك_العمليات.h"



/* ================= Wrappers ================= */

static process_t* api_create(void (*entry)()) {
    return process_create(entry);
}

static void api_start(process_t* p) {
    process_start(p);
}

static process_t* api_current() {
    return get_current_process();
}

static uint64_t api_get_pid(process_t* p) {
    return p->pid;
}

static void api_init() {
    process_init();
}

static uint64_t* api_get_pml4(process_t* p) {
    return p->pml4;
}


/* ================= API ================= */

process_api_t process_api = {
    .create   = api_create,
    .start    = api_start,
    .current  = api_current,
    .get_pid  = api_get_pid,
    .get_pml4 = api_get_pml4,
    .init     = api_init
};
