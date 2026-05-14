#include <stdint.h>
#include "أرض_المستخدم/النظام/التعريفات/المؤقت/المؤقت.h"
#include "محرك_الجدولة/محرك_الجدولة.h"
#include "محرك_الجدولة/مُدير_الأحداث/مُدير_الأحداث.h"
#include "محرك_المقاطعات/أدارة_تنظيم_المقاطعات/مكتبة_واجهة_الإدخال_والإخراج.h"

#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40

volatile int ticks = 0;



void timer_init() {
    uint32_t freq = 100;
    uint32_t divisor = 1193180 / freq;

    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

uint64_t timer_get_ticks()
{
    return (uint64_t)ticks;
}

volatile int need_schedule = 0;   
void timer_handler() {
    ticks++;
    scheduler_tick();

    events |= EVENT_SCHEDULE;   //  فقط طلب
}
