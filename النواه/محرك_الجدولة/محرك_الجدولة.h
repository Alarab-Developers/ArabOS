#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <stdint.h>

typedef struct process process_t;


extern void context_switch(uint64_t* old_rsp, uint64_t new_rsp);
void scheduler_sleep(process_t* p, int ticks);
void scheduler_remove(process_t* p);
void scheduler_add(process_t* p);
process_t* scheduler_current();
process_t* scheduler_next();
void scheduler_init();
void scheduler_tick();
void scheduler_run();
void schedule();
void yield();


#endif
