#include <stdint.h>
#include "مُدير_الأحداث/مُدير_الأحداث.h"
#include "محرك_الجدولة.h"
#include "محرك_العمليات/محرك_العمليات.h"
#include "مكتبة_المحركات/مكتبة_معالجة_النصوص.h"
#include "محرك_الذاكرة/بوابة_الذاكرة.h"

extern void context_switch(uint64_t* old_rsp, uint64_t new_rsp);
extern void jump_to_first_task(uint64_t rsp);

/* ================= LISTS ================= */

static process_t* ready_list[MAX_PROCESSES];
static process_t* sleep_list[MAX_PROCESSES];
static int ready_count = 0;
static int sleep_count = 0;

extern process_t* current_process;

uint64_t global_min_vruntime = 0;

static int weight_from_nice(int nice) {
    return 1024 / (nice + 1);
}

/* ================= INIT ================= */

void scheduler_init() {
    ready_count     = 0;
    sleep_count     = 0;
    current_process = 0;
}

/* ================= VRUNTIME ================= */

static void update_min_vruntime() {
    if (ready_count == 0) return;
    uint64_t min = ready_list[0]->vruntime;
    for (int i = 1; i < ready_count; i++)
        if (ready_list[i]->vruntime < min)
            min = ready_list[i]->vruntime;
    global_min_vruntime = min;
}

/* ================= READY LIST ================= */

void scheduler_add(process_t* p) {
    if (ready_count >= MAX_PROCESSES) return;
    p->vruntime = global_min_vruntime;
    ready_list[ready_count++] = p;
}

void scheduler_remove(process_t* p) {
    for (int i = 0; i < ready_count; i++) {
        if (ready_list[i] == p) {
            ready_list[i] = ready_list[--ready_count];
            return;
        }
    }
}

/* ================= SLEEP ================= */

static void add_to_sleep(process_t* p) {
    if (sleep_count >= MAX_PROCESSES) return;
    sleep_list[sleep_count++] = p;
}

static void remove_from_sleep(int index) {
    sleep_list[index] = sleep_list[--sleep_count];
}

/* ================= NEXT ================= */

extern process_t* idle_p;

process_t* scheduler_next() {
    if (ready_count == 0)
        return idle_p;

    process_t* best = ready_list[0];
    for (int i = 1; i < ready_count; i++)
        if (ready_list[i]->vruntime < best->vruntime)
            best = ready_list[i];

    return best;
}

/* ================= SCHEDULE ================= */
/*
 * تُستخدم فقط للانطلاق الأول من kernel_main (قبل STI).
 * بعد تفعيل المقاطعات، isr_timer يتولى كل التبديل.
 */
void schedule() {
    process_t* next = scheduler_next();
    if (!next) return;

    if (!current_process) {
        current_process = next;
        memory_api.switch_address_space(next->pml4);
        jump_to_first_task(next->rsp);
        return;
    }

    if (current_process == next) return;

    process_t* prev = current_process;
    current_process = next;
    memory_api.switch_address_space(next->pml4);
    context_switch(&prev->rsp, next->rsp);
}

/* ================= YIELD ================= */

void yield() {
    /*
     * int 32 = vector مؤقت الـ LAPIC (isr_timer).
     * هذا يدخل isr_timer بنفس المسار الطبيعي تماماً:
     *   SAVE_REGS → حفظ p->rsp → scheduler_next() → تبديل → iretq
     */
    __asm__ volatile ("int $32");
}

/* ================= TICK ================= */

void scheduler_tick() {
    if (current_process && current_process != idle_p) {
        current_process->vruntime +=
            (1024 / weight_from_nice(current_process->nice));
        update_min_vruntime();
    }

    for (int i = 0; i < sleep_count; ) {
        process_t* p = sleep_list[i];
        if (--p->sleep_ticks <= 0) {
            remove_from_sleep(i);
            scheduler_add(p);
            continue;
        }
        i++;
    }
}

/* ================= SLEEP ================= */

void scheduler_sleep(process_t* p, int ticks) {
    p->sleep_ticks = ticks;
    scheduler_remove(p);   /* أزلها من ready_list أولاً */
    add_to_sleep(p);       /* أضفها لقائمة النوم */
    /* لا نستدعي yield() هنا — المستدعي (sleep_ms) يفعل ذلك */
}

/* ================= CURRENT ================= */

process_t* scheduler_current() { return current_process; }
void scheduler_run() {}
