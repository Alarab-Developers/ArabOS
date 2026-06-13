#include <stdint.h>

#include "بوابة_النظام.h"

void _start(kernel_api_t* api)
{
    Message msg = {0};

    msg.type = IPC_LAUNCH_APP;

    const char* name = "app.ت";

    int i = 0;

    while (name[i])
    {
        msg.data[i] = name[i];
        i++;
    }

    msg.data[i] = 0;

    msg.size = i + 1;

    api->ipc_send(
        0,
        &msg
    );

    while (1)
    {
    }
}
