#ifndef WAIT_QUEUE_H
#define WAIT_QUEUE_H
#include "محرك_العمليات/محرك_العمليات.h"
#include "محرك_الجدولة/محرك_الجدولة.h"

typedef struct process process_t;

#define WAIT_QUEUE_MAX 256

typedef struct {
    process_t* list[WAIT_QUEUE_MAX];
    int count;
} wait_queue_t;

void wait_queue_init(wait_queue_t* q);
void wake_up_one(wait_queue_t* q);
void wait(wait_queue_t* q);
void wake_up(wait_queue_t* q);

#endif
