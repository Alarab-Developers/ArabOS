#ifndef LAPIC_H
#define LAPIC_H

#include <stdint.h>

void lapic_init();
void lapic_eoi();
void lapic_timer_init();

#endif
