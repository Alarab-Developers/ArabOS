#ifndef PROCESS_API_H
#define PROCESS_API_H

#include <stdint.h>

typedef struct process process_t;

typedef struct {

    // إنشاء عملية
    process_t* (*create)(void (*entry)());

    // بدء عملية
    void (*start)(process_t* p);

    // الحصول على العملية الحالية
    process_t* (*current)();

    // PID
    uint64_t (*get_pid)(process_t* p);

    uint64_t* (*get_pml4)(process_t* p);
    
    void (*init)();
        process_t* (*create_ex)(
        void (*entry)(void*),
        void* arg
    );



} process_api_t;

// 🔥 البوابة
extern process_api_t process_api;

#endif
