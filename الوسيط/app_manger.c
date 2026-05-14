#include <stdint.h>

#include "بوابة_النظام.h"

void _start(kernel_api_t* api)
{
    Message msg;

    msg.sender   = 1;
    msg.receiver = 0;

    msg.type = IPC_LAUNCH_APP;

    msg.size = 8;

    msg.data[0] = 'a';
    msg.data[1] = 'p';
    msg.data[2] = 'p';
    msg.data[3] = '.';
    msg.data[4] = 'b';
    msg.data[5] = 'i';
    msg.data[6] = 'n';
    msg.data[7] = 0;

    api->ipc_send(
        0,
        &msg
    );

    while (1) {
        asm volatile("hlt");
    }
}
