#include "مُعالج_IRQ.h"
#include "محرك_المقاطعات/الأداره_المتقدمة_للمقاطعات/مُتحكم_المقاطعات.h"

#define MAX_IRQ 256

static irq_handler_t irq_handlers[MAX_IRQ];

/* ================= INIT ================= */

void irq_init() {
    for (uint32_t i = 0; i < MAX_IRQ; i++)
        irq_handlers[i] = 0;
}

/* ================= REGISTER ================= */

void irq_register(uint32_t irq, irq_handler_t handler) {
    if (irq >= MAX_IRQ)
        return;

    irq_handlers[irq] = handler;
}

/*
 * irq_handler — المعالج المركزي
 * يُستدعى من isr_timer/keyboard/mouse بعد SAVE_REGS
 *
 *  يُرسل EOI في النهاية دائماً
 *  لا يُجري context switch هنا
 */

void irq_handler(uint32_t irq) {

    /* حماية من القيم الخاطئة */
    if (irq >= MAX_IRQ) {
        lapic_eoi();
        return;
    }

    irq_handler_t handler = irq_handlers[irq];

    if (handler) {
        handler();
    }

    /* إرسال End Of Interrupt */
    lapic_eoi();
}
