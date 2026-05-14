#include "محرك_ربط_العمليات.h"

#include "محرك_العمليات/محرك_العمليات.h"
#include "محرك_الجدولة/محرك_الجدولة.h"

#include "محرك_التطبيقات/محرك_التطبيقات.h"

#include "أرض_المستخدم/النظام/الخدمات/محرك_انظمة_الملفات/file.h"

/* ========================================================= */
/* external */
/* ========================================================= */

extern file_t* find_file(
    const char* name
);

extern void append_text(
    const char* str
);

/* ========================================================= */
/* INTERRUPTS */
/* ========================================================= */

static inline void disable_interrupts(void)
{
    __asm__ volatile("cli");
}

static inline void enable_interrupts(void)
{
    __asm__ volatile("sti");
}

/* ========================================================= */
/* INIT */
/* ========================================================= */

void ipc_init_process(
    process_t* p
) {

    p->mailbox.head  = 0;
    p->mailbox.tail  = 0;
    p->mailbox.count = 0;
}

/* ========================================================= */
/* SEND */
/* ========================================================= */

int ipc_send(
    int target_pid,
    Message* msg
) {

    append_text(
        "[IPC] ipc_send called\n"
    );

    /* ===================================================== */
    /* kernel launch request */
    /* ===================================================== */

    if (
        target_pid == 0 &&
        msg->type == IPC_LAUNCH_APP
    ) {

        append_text(
            "[IPC] kernel launch request\n"
        );

        file_t* f =
            find_file(
                (char*)msg->data
            );

        if (f) {

            append_text(
                "[IPC] app found\n"
            );

            app_run(f);

        } else {

            append_text(
                "[IPC] app not found\n"
            );
        }

        return 0;
    }

    /* ===================================================== */
    /* normal ipc */
    /* ===================================================== */

    process_t* target =
        get_process(target_pid);

    if (!target) {

        append_text(
            "[IPC] target not found\n"
        );

        return -1;
    }

    ipc_mailbox_t* box =
        &target->mailbox;

    disable_interrupts();

    if (
        box->count >=
        IPC_MAX_MESSAGES
    ) {

        enable_interrupts();

        append_text(
            "[IPC] mailbox full\n"
        );

        return -2;
    }

    box->messages[box->tail] =
        *msg;

    box->tail =
        (box->tail + 1) %
        IPC_MAX_MESSAGES;

    box->count++;

    enable_interrupts();

    append_text(
        "[IPC] message queued\n"
    );

    if (
        target->state ==
        PROCESS_BLOCKED_IPC
    ) {

        target->state =
            TASK_READY;

        scheduler_add(target);

        append_text(
            "[IPC] target awakened\n"
        );
    }

    return 0;
}

/* ========================================================= */
/* RECEIVE */
/* ========================================================= */

int ipc_receive(
    Message* out_msg
) {

    process_t* current =
        get_current_process();

    if (!current)
        return -1;

    ipc_mailbox_t* box =
        &current->mailbox;

    disable_interrupts();

    while (
        box->count == 0
    ) {

        current->state =
            PROCESS_BLOCKED_IPC;

        enable_interrupts();

        scheduler_remove(current);

        yield();

        disable_interrupts();
    }

    *out_msg =
        box->messages[box->head];

    box->head =
        (box->head + 1) %
        IPC_MAX_MESSAGES;

    box->count--;

    enable_interrupts();

    append_text(
        "[IPC] message received\n"
    );

    return 0;
}
