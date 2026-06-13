#ifndef IPC_H
#define IPC_H

#include <stdint.h>

#define IPC_DATA_SIZE 32

typedef enum {
    IPC_START_PROCESS,
    IPC_SLEEP,
    IPC_LAUNCH_APP,
    IPC_EVENT,
    IPC_VIDEO_CLEAR
} IPC_Type;

typedef struct Message {

    int sender;
    int receiver;

    IPC_Type type;

    uint32_t size;

    uint8_t data[IPC_DATA_SIZE];

} Message;

#endif
