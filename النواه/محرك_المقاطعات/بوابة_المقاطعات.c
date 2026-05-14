#include "بوابة_المقاطعات.h"

#include "محرك_المقاطعات/أدارة_تنظيم_المقاطعات/مُدير_جدول_المقاطعات.h"
#include "محرك_المقاطعات/أدارة_تنظيم_المقاطعات/مُعالج_IRQ.h"
#include "محرك_المقاطعات/أدارة_تنظيم_المقاطعات/مكتبة_واجهة_الإدخال_والإخراج.h"

/* ===== IDT Wrappers ===== */

static void api_init() {

    idt_init();
}

static void api_set_gate(
    int n,
    uint64_t handler
) {

    idt_set_gate(n, handler);
}

/* ===== IRQ Wrappers ===== */

static void api_irq_init() {

    irq_init();
}

static void api_irq_register(
    uint32_t irq,
    irq_handler_t handler
) {

    irq_register(irq, handler);
}

static void api_irq_handler(
    uint32_t irq
) {

    irq_handler(irq);
}


/* ===== PORT IO ===== */

static void api_outb(
    uint16_t port,
    uint8_t value
)
{
    outb(
        port,
        value
    );
}

static uint8_t api_inb(
    uint16_t port
)
{
    return inb(
        port
    );
}

/* ===== API ===== */

interrupt_api_t interrupt_api = {

    /* ===== IDT ===== */

    .init = api_init,

    .set_gate = api_set_gate,

    /* ===== IRQ ===== */

    .irq_init = api_irq_init,

    .irq_register = api_irq_register,

    .irq_handler = api_irq_handler,

    /* ===== PORT IO ===== */

    .outb = api_outb,

    .inb  = api_inb
};
