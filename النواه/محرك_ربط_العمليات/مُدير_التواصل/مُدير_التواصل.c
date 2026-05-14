#include <stdint.h>

#include "مُدير_التواصل.h"

#include "أرض_المستخدم/النظام/الخدمات/محرك_العرض/بوابة_محرك_العرض.h"

#include "أرض_المستخدم/النظام/التعريفات/أنظمة_الملفات/نظام_العرب/المسارات/المسارات.h"
#include "أرض_المستخدم/النظام/التعريفات/أنظمة_الملفات/نظام_العرب/الملفات/الملفات.h"
#include "أرض_المستخدم/النظام/التعريفات/أنظمة_الملفات/نظام_العرب/نظام_العرب.h"
#include "أرض_المستخدم/النظام/التعريفات/أنظمة_الملفات/نظام_العرب/المجلدات/المجلدات.h"

#include "أرض_المستخدم/النظام/التعريفات/لوحات_المفاتيح/لوحة_المفاتيح_الافتراضية/لوحة_المفاتيح_الافتراضية.h"

#include "أرض_المستخدم/النظام/الخدمات/محرك_التعريفات/محرك_التعريفات.h"

#include "أرض_المستخدم/النظام/الخدمات/محرك_انظمة_الملفات/محرك_انظمة_الملفات.h"
#include "أرض_المستخدم/النظام/الخدمات/محرك_انظمة_الملفات/file.h"

#include "محرك_التطبيقات/محرك_التطبيقات.h"

#include "محرك_ربط_العمليات/محرك_ربط_العمليات.h"
#include "محرك_العمليات/محرك_العمليات.h"

/* ========================================================= */
/* IMPORTS */
/* ========================================================= */

extern file_t* find_file(
    const char* name
);


/* ========================================================= */
/* CONSOLE */
/* ========================================================= */

void sys_write(
    const char* str
) {

    console_write(str);
}

char sys_read() {

    return keyboard_getchar();
}

void sys_clear() {

    console_clear();
}

/* ========================================================= */
/* PORT I/O */
/* ========================================================= */

static inline void outb(
    unsigned short port,
    unsigned char val
) {

    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(val),
          "Nd"(port)
    );
}

static inline void outw(
    unsigned short port,
    unsigned short val
) {

    __asm__ volatile (
        "outw %0, %1"
        :
        : "a"(val),
          "Nd"(port)
    );
}

/* ========================================================= */
/* POWER */
/* ========================================================= */

void sys_restart() {

    console_write(
        "Restarting...\n"
    );

    outb(0x64, 0xFE);

    while (1) {

    }
}

void sys_shutdown() {

    console_write(
        "Shutting down...\n"
    );

    outw(0x604, 0x2000);

    while (1) {

    }
}

/* ========================================================= */
/* FILESYSTEM */
/* ========================================================= */

int sys_fs_create(
    const char* name
) {

    diskfs_create(name, "");

    return 1;
}

int sys_fs_write(
    const char* name,
    const char* data
) {

    diskfs_create(name, data);

    return 1;
}

static char read_buffer[512];

const char* sys_fs_read(
    const char* name
) {

    diskfs_read(
        name,
        read_buffer
    );

    return read_buffer;
}

static void list_callback(
    const char* name
) {

    console_write(name);
    console_write("\n");
}

void sys_fs_list(
    const char* path,
    void (*callback)(const char*)
) {

    (void)callback;

    unsigned int id = 0;

    if (
        path &&
        path[0] != 0
    ) {

        id =
            arabfs_resolve(path);
    }

    arabfs_list_path(
        id,
        list_callback
    );
}

int sys_fs_mkdir(
    const char* path
) {

    return arabfs_mkdir(path);
}

int sys_fs_delete(
    const char* path
) {

    return fs_engine_delete(path);
}

int sys_fs_exists(
    const char* path
) {

    return fs_engine_exists(path);
}

int sys_fs_copy(
    const char* src,
    const char* dest
) {

    return fs_engine_copy(
        src,
        dest
    );
}
