#ifndef TIMER_H
#define TIMER_H
#define TIMER_FREQ 100

#include <stdint.h>
extern volatile int ticks;
extern volatile int need_schedule;


void timer_init();
void timer_handler();
uint64_t timer_get_ticks();

#endif
