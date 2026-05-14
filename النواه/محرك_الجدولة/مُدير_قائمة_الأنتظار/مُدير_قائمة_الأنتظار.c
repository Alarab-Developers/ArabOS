#include "مُدير_قائمة_الأنتظار.h"

/* ================= INIT ================= */

void wait_queue_init(wait_queue_t* q) {
    q->count = 0;
}

/* ================= WAIT ================= */

/*
 * ✅ الترتيب الصحيح مهم جداً:
 *   1) احصل على العملية الحالية
 *   2) غيّر حالتها إلى WAITING
 *   3) أزلها من ready_list (قبل yield!)
 *   4) أضفها لقائمة الانتظار
 *   5) استدعِ yield() → int $32 → isr_timer يبدّل
 *
 * ⚠️ لا تستدعِ scheduler_remove بعد yield — لن تصل إليه أبداً
 *    حتى يستدعي أحدٌ wake_up() على هذه القائمة.
 */
void wait(wait_queue_t* q) {
    process_t* p = scheduler_current();

    p->state = TASK_WAITING;

    scheduler_remove(p);          /* أزلها من ready_list */

    if (q->count < WAIT_QUEUE_MAX)
        q->list[q->count++] = p;

    yield();                      /* int $32 → تبديل صحيح */
}

/* ================= WAKE ALL ================= */

void wake_up(wait_queue_t* q) {
    for (int i = 0; i < q->count; i++) {
        q->list[i]->state = TASK_READY;
        scheduler_add(q->list[i]);
    }
    q->count = 0;
}

/* ================= WAKE ONE ================= */

void wake_up_one(wait_queue_t* q) {
    if (q->count == 0)
        return;

    process_t* p = q->list[0];

    for (int i = 1; i < q->count; i++)
        q->list[i - 1] = q->list[i];

    q->count--;

    p->state = TASK_READY;
    scheduler_add(p);
}
