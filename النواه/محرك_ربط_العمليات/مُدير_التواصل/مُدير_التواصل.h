#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>




/* ========================================================= */
/* CONSOLE */
/* ========================================================= */

void sys_write(
    const char* str
);

char sys_read();

void sys_clear();

void sys_restart();

void sys_shutdown();

/* ========================================================= */
/* FILESYSTEM */
/* ========================================================= */

int sys_fs_create(
    const char* name
);

int sys_fs_write(
    const char* name,
    const char* data
);

const char* sys_fs_read(
    const char* name
);

void sys_fs_list(
    const char* path,
    void (*callback)(const char*)
);

int sys_fs_mkdir(
    const char* path
);

int sys_fs_delete(
    const char* path
);

int sys_fs_exists(
    const char* path
);

int sys_fs_copy(
    const char* src,
    const char* dest
);


#endif
