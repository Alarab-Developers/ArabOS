#ifndef SCHEDULER_API_H
#define SCHEDULER_API_H

#include <stdint.h>

// forward declaration
typedef struct process process_t;

typedef struct {

    void (*init)();

    void (*add)(process_t* p);

    void (*remove)(process_t* p);

    process_t* (*next)();

    void (*schedule)();

    void (*sleep)(process_t* p, int ticks);
    process_t* (*current)();

} scheduler_api_t;

// 🔥 البوابة العامة
extern scheduler_api_t scheduler_api;

#endif
