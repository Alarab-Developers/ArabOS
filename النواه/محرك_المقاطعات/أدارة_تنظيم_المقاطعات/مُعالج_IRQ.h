#ifndef IRQ_HANDLER_H
#define IRQ_HANDLER_H

#include <stdint.h>

typedef void (*irq_handler_t)(void);

// تسجيل handler
void irq_register(uint32_t irq, irq_handler_t handler);
void irq_handler(uint32_t irq);

// تهيئة (اختياري لكن مهم)
void irq_init();

#endif
