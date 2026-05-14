#ifndef IPC_H
#define IPC_H

#include <stdint.h>

typedef struct process process_t;

/* ================================================================
 * ✅ إصلاح حجم الـ mailbox:
 *
 * المشكلة القديمة:
 *   IPC_MAX_MESSAGES = 32, IPC_DATA_SIZE = 64
 *   → sizeof(Message) = 80 bytes
 *   → sizeof(ipc_mailbox_t) = 2572 bytes
 *   → sizeof(process_t) = 2632 bytes
 *   → p->pml4 في offset 2624 (قريب من حد الصفحة 4096)
 *   → أي كتابة في p->pid (offset 0) تُسبب page fault
 *     عند محاولة الوصول لـ p->pml4 لاحقاً
 *     لأن الصفحة الثانية غير مُعيَّنة بعد kmalloc
 *
 * الإصلاح:
 *   IPC_MAX_MESSAGES = 8, IPC_DATA_SIZE = 32
 *   → sizeof(Message) = 48 bytes
 *   → sizeof(ipc_mailbox_t) = 396 bytes
 *   → sizeof(process_t) = 464 bytes
 *   → كل process_t يتسع في صفحة واحدة ✅
 * ================================================================ */
#define IPC_MAX_MESSAGES 8
#define IPC_DATA_SIZE    32

typedef enum {
    IPC_START_PROCESS,
    IPC_SLEEP,
    IPC_LAUNCH_APP,
    IPC_EVENT
} IPC_Type;

typedef struct {
    int      sender;
    int      receiver;
    IPC_Type type;
    uint32_t size;
    uint8_t  data[IPC_DATA_SIZE];
} Message;

typedef struct {
    Message messages[IPC_MAX_MESSAGES];
    int head;
    int tail;
    int count;
} ipc_mailbox_t;

// IPC API
void ipc_init_process(process_t* p);
int  ipc_send(int target_pid, Message* msg);
int  ipc_receive(Message* out_msg);

#endif
