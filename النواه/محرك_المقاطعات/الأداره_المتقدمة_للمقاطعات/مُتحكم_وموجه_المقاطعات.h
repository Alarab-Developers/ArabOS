#ifndef IOAPIC_H
#define IOAPIC_H

#include <stdint.h>

void ioapic_init();
void ioapic_set_entry(int irq, uint8_t vector);

#endif
