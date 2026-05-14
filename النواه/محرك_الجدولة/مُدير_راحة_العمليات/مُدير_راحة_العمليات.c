#include "مُدير_راحة_العمليات.h"
#include "محرك_الجدولة/محرك_الجدولة.h"
#include "أرض_المستخدم/النظام/التعريفات/المؤقت/المؤقت.h"

/*
 * ✅ sleep و sleep_ms صحيحتان بالفعل في منطقهما.
 *
 * المشكلة لم تكن هنا — بل في yield() التي كانت تستدعي
 * context_switch() مباشرة وتكسر المكدس.
 *
 * الآن بعد إصلاح yield() لتستخدم "int $32"، هذا الملف
 * يعمل بشكل صحيح بدون أي تغيير في المنطق.
 *
 * الترتيب الصحيح:
 *   1) scheduler_sleep() تُزيل العملية من ready_list وتضعها في sleep_list
 *   2) yield() تُطلق int $32
 *   3) isr_timer يرى أن العملية الحالية ليست في ready_list
 *   4) scheduler_next() تختار عملية أخرى → تبديل صحيح
 */

void sleep(int ticks) {
    if (ticks <= 0)
        return;

    process_t* p = scheduler_current();

    scheduler_sleep(p, ticks);  /* أزلها من ready + أضفها لـ sleep */

    yield();                    /* int $32 → isr_timer يبدّل للعملية التالية */
}

void sleep_ms(int ms) {
    if (ms <= 0)
        return;

    int ticks = (ms * TIMER_FREQ + 999) / 1000;

    process_t* p = scheduler_current();

    scheduler_sleep(p, ticks);

    yield();
}
