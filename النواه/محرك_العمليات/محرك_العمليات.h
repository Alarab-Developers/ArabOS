#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "محرك_ربط_العمليات/محرك_ربط_العمليات.h"

#define MAX_PROCESSES 256

typedef struct process {
    int      pid;          /* offset 0  */
    void   (*entry)();     /* offset 8  */
    uint64_t rsp;          /* offset 16 ← isr_timer يستخدم هذا */
    void*    stack;        /* offset 24 */
    int      sleep_ticks;  /* offset 32 */
    int      state;        /* offset 36 */
    uint64_t vruntime;     /* offset 40 */
    int      nice;         /* offset 48 */
    ipc_mailbox_t mailbox; /* offset 52 (حجمه 396 بعد التصغير) */
    uint64_t* pml4;        /* offset 448 */
    void* arg;
} process_t;

enum {
    TASK_READY,
    TASK_WAITING,
    TASK_SLEEPING,
    PROCESS_BLOCKED_IPC
};

void process_init();
process_t* process_create(void (*entry)());
void process_start(process_t* p);
void process_sleep(process_t* p);
process_t* get_process(int pid);
process_t* get_current_process();

#endif
