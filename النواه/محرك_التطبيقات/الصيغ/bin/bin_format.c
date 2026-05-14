#include "bin_format.h"

#include <string.h>
#include <stdint.h>

#include "محرك_العمليات/بوابة_العمليات.h"
#include "محرك_الجدولة/بوابة_الجدولة.h"
#include "محرك_الذاكرة/بوابة_الذاكرة.h"
#include "محرك_ربط_العمليات/بوابة_النواه.h"

#define HIGHER_HALF_BASE 0xFFFF800000000000ULL

typedef void (*app_entry_t)(
    kernel_api_t*
);

/* ========================================================= */
/* التطبيق الحالي */
/* ========================================================= */

static app_entry_t current_app = 0;

/* ========================================================= */
/* Wrapper */
/* ========================================================= */

static void app_launcher()
{
    if (!current_app) {

        while (1) {
            asm volatile("hlt");
        }
    }

    /*
     * تمرير API الحقيقي للتطبيق
     */

    current_app(&kapi);

    /*
     * لا نعود أبداً
     */

    while (1) {
        asm volatile("hlt");
    }
}

/* ========================================================= */
/* BIN RUN */
/* ========================================================= */

int bin_run(file_t* f)
{
    if (!f)
        return 0;

    if (!f->data)
        return 0;

    if (f->size == 0)
        return 0;

    /*
     * تخصيص صفحة
     */

    uint64_t phys =
        memory_api.alloc_page();

    if (!phys)
        return 0;

    /*
     * تحويل لعنوان virtual
     */

    void* app_mem =
        (void*)(HIGHER_HALF_BASE + phys);

    /*
     * نسخ التطبيق
     */

    memcpy(
        app_mem,
        f->data,
        f->size
    );

    /*
     * entry point
     */

    current_app =
        (app_entry_t)app_mem;

    /*
     * إنشاء العملية
     */

    process_t* p =
        process_api.create(
            app_launcher
        );

    if (!p)
        return 0;

    /*
     * إضافتها للجدولة
     */

    scheduler_api.add(p);

    return 1;
}
