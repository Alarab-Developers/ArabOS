#ifndef INTERRUPT_API_H
#define INTERRUPT_API_H

#include <stdint.h>

/* ===== IRQ Handler Type ===== */

typedef void (*irq_handler_t)(void);

/* ===== API ===== */

typedef struct {

    /* ===== IDT ===== */

    void (*init)();

    void (*set_gate)(
        int n,
        uint64_t handler
    );

    /* ===== IRQ ===== */

    void (*irq_init)();

    void (*irq_register)(
        uint32_t irq,
        irq_handler_t handler
    );

    void (*irq_handler)(
        uint32_t irq
    );

    /* ===== PORT IO ===== */

    void (*outb)(
        uint16_t port,
        uint8_t value
    );

    uint8_t (*inb)(
        uint16_t port
    );

} interrupt_api_t;

/* ===== Global API ===== */

extern interrupt_api_t interrupt_api;

#endif
