#include <stdint.h>
#include "محرك_المقاطعات/بوابة_المقاطعات.h"
#include "محرك_المقاطعات/أدارة_تنظيم_المقاطعات/مكتبة_واجهة_الإدخال_والإخراج.h"
#include "أرض_المستخدم/النظام/الخدمات/محرك_التعريفات/بوابة_محرك_التعريفات.h"
#include "محرك_الفيديو/بوابة_الفيديو.h"
#include "مكتبة_المحركات/مكتبة_معالجة_النصوص.h"
#include "القلب/الجوهرة.h"
#include "محرك_الذاكرة/بوابة_الذاكرة.h"
#include "محرك_العمليات/بوابة_العمليات.h"
#include "محرك_الجدولة/بوابة_الجدولة.h"
#include "أرض_المستخدم/النظام/الخدمات/محرك_العرض/بوابة_محرك_العرض.h"
#include "أرض_المستخدم/النظام/الخدمات/محرك_انظمة_الملفات/file.h"
#include "محرك_التطبيقات/محرك_التطبيقات.h"
#include "محرك_المقاطعات/الأداره_المتقدمة_للمقاطعات/مُتحكم_المقاطعات.h"
#include "محرك_المقاطعات/الأداره_المتقدمة_للمقاطعات/مُتحكم_وموجه_المقاطعات.h"
#include "محرك_الجدولة/مُدير_الأحداث/مُدير_الأحداث.h"
#include "محرك_ربط_العمليات/محرك_ربط_العمليات.h"
#include "محرك_ربط_العمليات/مُدير_التواصل/مُدير_التواصل.h"



#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36d76289
#define MMIO_BASE 0xFFFF900000000000ULL
#define USER_ADDR 0x0000000000400000


extern void isr_timer();
extern void isr_keyboard();
extern void isr_mouse();
extern volatile int need_schedule;


void append_text(const char* str);
void append_hex(uint64_t val);

extern void init_apps();

process_t* idle_p = 0;

/* ================= MULTIBOOT ================= */

struct multiboot2_tag {
    uint32_t type;
    uint32_t size;
};

struct multiboot2_tag_framebuffer {
    uint32_t type;
    uint32_t size;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    uint16_t reserved;
};

struct multiboot2_tag_module {
    uint32_t type;
    uint32_t size;
    uint32_t mod_start;
    uint32_t mod_end;
};

/* ================= الوسيط ================= */

file_t files[16];
int file_count = 0;

file_t* init_file = 0;
file_t* txt_file  = 0;

int hex_to_int(char* str, int len) {
    int val = 0;
    for (int i = 0; i < len; i++) {
        val <<= 4;
        char c = str[i];
        if (c >= '0' && c <= '9') val |= (c - '0');
        else if (c >= 'A' && c <= 'F') val |= (c - 'A' + 10);
    }
    return val;
}

void parse_الوسيط(uint8_t* start, uint32_t size) {
    uint8_t* p   = start;
    uint8_t* end = start + size;

    while (p < end) {
        if (!(p[0]=='0' && p[1]=='7' && p[2]=='0' && p[3]=='7')) break;

        int namesize = hex_to_int((char*)(p + 94), 8);
        int filesize = hex_to_int((char*)(p + 54), 8);

        char*    name = (char*)(p + 110);
        uint8_t* data = (uint8_t*)(p + 110 + namesize);

        if ((uintptr_t)data % 4)
            data += 4 - ((uintptr_t)data % 4);

        if (name[0] && file_count < 16) {
            file_t* f = &files[file_count++];
            int i = 0;
            while (name[i] && i < 127) { f->name[i] = name[i]; i++; }
            f->name[i] = 0;
            f->data = data;
            f->size = filesize;
        }

        if (name[0] == 'T') break;

        p = data + filesize;
        if ((uintptr_t)p % 4)
            p += 4 - ((uintptr_t)p % 4);
    }
}

file_t* find_file(const char* name) {
    for (int i = 0; i < file_count; i++) {
        char* a = files[i].name;
        int j = 0;
        while (name[j] && a[j] && name[j] == a[j]) j++;
        if (name[j] == 0 && a[j] == 0) return &files[i];
    }
    return 0;
}

/* ================= طرفية ================= */

int win;
char command[128];
int cmd_len = 0;
char terminal_buffer[4096];

int starts_with(const char* str, const char* prefix) {
    int i = 0;
    while (prefix[i]) {
        if (str[i] != prefix[i]) return 0;
        i++;
    }
    return 1;
}

void append_text(const char* str) {
    int len = 0;
    while (terminal_buffer[len]) len++;
    int i = 0;
    while (str[i] && len < 4094) {
        terminal_buffer[len++] = str[i++];
    }
    terminal_buffer[len] = 0;
}

void execute_command(const char* cmd) {
    append_text("> ");
    append_text(cmd);
    append_text("\n");

    if (strcmp(cmd, "clear") == 0) { terminal_buffer[0] = 0; return; }

    if (strcmp(cmd, "help") == 0) {
        append_text("Commands:\n");
        append_text("help clear ls cat echo touch mkdir rm\n");
        append_text("pwd write read\n");
        return;
    }

    if (strcmp(cmd, "ls") == 0) {
        for (int i = 0; i < file_count; i++) {
            append_text(files[i].name);
            append_text("\n");
        }
        return;
    }

    if (starts_with(cmd, "cat ")) {
        const char* data = sys_fs_read(cmd + 4);
        if (data) { append_text(data); append_text("\n"); }
        else append_text("File not found\n");
        return;
    }

    if (starts_with(cmd, "echo ")) { append_text(cmd + 5); append_text("\n"); return; }

    if (starts_with(cmd, "touch ")) {
        if (sys_fs_create(cmd + 6)) append_text("File created\n");
        else append_text("Error\n");
        return;
    }

    if (starts_with(cmd, "mkdir ")) {
        if (sys_fs_mkdir(cmd + 6)) append_text("Directory created\n");
        else append_text("Error\n");
        return;
    }

    if (starts_with(cmd, "rm ")) {
        if (sys_fs_delete(cmd + 3)) append_text("Deleted\n");
        else append_text("Error deleting\n");
        return;
    }

    if (starts_with(cmd, "write ")) {
        const char* p = cmd + 6;
        char name[64];
        int i = 0;
        while (*p && *p != ' ') { name[i++] = *p++; }
        name[i] = 0;
        if (*p == ' ') p++;
        if (sys_fs_write(name, p)) append_text("Written\n");
        else append_text("Write error\n");
        return;
    }

    if (starts_with(cmd, "run ")) {
        file_t* f = find_file(cmd + 4);
        if (f) { app_run(f); append_text("App started\n"); }
        else append_text("File not found\n");
        return;
    }

    if (starts_with(cmd, "read ")) {
        const char* data = sys_fs_read(cmd + 5);
        if (data) { append_text(data); append_text("\n"); }
        else append_text("Not found\n");
        return;
    }

    if (strcmp(cmd, "pwd") == 0) { append_text("/\n"); return; }

    append_text("Unknown command\n");
}

void update_display() {

    char buffer[256];

    int i = 0;

    while (
        terminal_buffer[i] &&
        i < 253
    ) {

        buffer[i] = terminal_buffer[i];
        i++;
    }

    buffer[i++] = '>';
    buffer[i++] = ' ';

    for (
        int j = 0;
        j < cmd_len && i < 255;
        j++
    ) {

        buffer[i++] = command[j];
    }

    buffer[i] = 0;

    display_api.set_window_text(
        win,
        buffer
    );
}

/* ================= المهام ================= */

void gui_task() {
    win = display_api.create_window(100, 100, 400, 300, 0x334455);
    terminal_buffer[0] = 0;
    append_text("ArabOS Terminal\n");
    append_text("type help\n\n");

    while (1) {
        char c = driver_api.keyboard->read();
        if (c) {
            if (c == '\n') {
                command[cmd_len] = 0;
                execute_command(command);
                cmd_len = 0;
            } else if (c == '\b') {
                if (cmd_len > 0) cmd_len--;
            } else if (c >= 32 && cmd_len < 127) {
                command[cmd_len++] = c;
            }
        }
        update_display();
        display_api.render();;
    }
}

void test_task() {

    while (1) {
        scheduler_api.sleep(
            scheduler_api.current(),
            3000
        );
        display_api.set_window_color(
            win,
            0xFF0000
        );
        scheduler_api.sleep(
            scheduler_api.current(),
            3000    
        );
        display_api.set_window_color(
            win,
            0x00FF00
        );
    }
}

void task_entry() {
    update_display();
    display_api.render();;
    append_text("\n");
    update_display();
    display_api.render();;
    while (1) {
        asm volatile("hlt");
    }
}

void vm_task_a() {

    uint64_t* p =
        (uint64_t*)USER_ADDR;

    *p = 0xAAAAAAAAAAAAAAAAULL;

    append_text("[A] write = ");
    append_hex(*p);
    append_text("\n");

    while (1) {

        append_text("[A] read = ");
        append_hex(*p);
        append_text("\n");

        for (volatile int i=0;i<100000000;i++);
    }
}

void vm_task_b() {

    uint64_t* p =
        (uint64_t*)USER_ADDR;

    *p = 0xBBBBBBBBBBBBBBBBULL;

    append_text("[B] write = ");
    append_hex(*p);
    append_text("\n");

    while (1) {

        append_text("[B] read = ");
        append_hex(*p);
        append_text("\n");

        for (volatile int i=0;i<100000000;i++);
    }
}

/* ================= اختبار IPC ================= */

/*
 * receiver_pid: يُحفظ قبل بدء الجدولة حتى يعرف المُرسِل
 * إلى أين يرسل الرسالة.
 */
static int receiver_pid = 0;

void task_sender() {

    append_text("[IPC] sender: start\n");

    /* انتظر قليلاً */
    for (volatile int i = 0;
         i < 3000000;
         i++);

    append_text("[IPC] sender: after delay\n");

    Message msg;

    memset(&msg, 0, sizeof(Message));


    append_text("[IPC] receiver_pid=");
    append_hex((uint64_t)receiver_pid);
    append_text("\n");

    msg.sender   = 1;

    append_text("[IPC] receiver_pid=");
    append_hex((uint64_t)receiver_pid);
    append_text("\n");

    msg.receiver = receiver_pid;
    msg.type     = IPC_EVENT;
    msg.size     = 4;

    append_text("[IPC] sender: header OK\n");

    /* بيانات الاختبار */
    msg.data[0] = 0xAB;
    msg.data[1] = 0xCD;
    msg.data[2] = 0x12;
    msg.data[3] = 0x34;

    append_text("[IPC] sender: data OK\n");

    append_text("[IPC] sender: call ipc_send...\n");

    int result =
        ipc_send(receiver_pid, &msg);

    append_text("[IPC] sender: ipc_send returned\n");

    append_text("[IPC] send result=");
    append_hex((uint64_t)(int64_t)result);
    append_text("\n");

    if (result == 0) {

        append_text("[IPC] send: OK\n");

    } else {

        append_text("[IPC] send: FAIL\n");
    }

    while (1) {
        __asm__("hlt");
    }
}

void task_receiver() {

    append_text("[IPC] receiver: start\n");

    Message msg;

    memset(&msg, 0, sizeof(Message));

    append_text("[IPC] receiver: waiting...\n");

    int result = ipc_receive(&msg);

    append_text("[IPC] receiver: ipc_receive returned\n");

    append_text("[IPC] receive result=");
    append_hex((uint64_t)(int64_t)result);
    append_text("\n");

    if (result != 0) {

        append_text("[IPC] receive: FAIL\n");

        while (1) {
            __asm__("hlt");
        }
    }

    append_text("[IPC] ===================\n");

    append_text("[IPC] send in!\n");

    append_text("[IPC] sender PID : ");
    append_hex((uint64_t)msg.sender);
    append_text("\n");

    append_text("[IPC] type : ");
    append_hex((uint64_t)msg.type);
    append_text("\n");

    append_text("[IPC] size : ");
    append_hex((uint64_t)msg.size);
    append_text("\n");

    append_text("[IPC] data : ");

    append_hex((uint64_t)msg.data[0]);
    append_text(" ");

    append_hex((uint64_t)msg.data[1]);
    append_text(" ");

    append_hex((uint64_t)msg.data[2]);
    append_text(" ");

    append_hex((uint64_t)msg.data[3]);

    append_text("\n");

    append_text("[IPC] ===================\n");

    while (1) {
        __asm__("hlt");
    }
}

/* ================= ================= */

void idle() {
    while (1) {
        __asm__("hlt");
    }
}

/* ================= TAR ================= */

typedef struct {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char typeflag;
} tar_header_t;

int oct_to_int(char* str, int len) {
    int val = 0;
    for (int i = 0; i < len; i++) {
        if (str[i] == 0) break;
        val = val * 8 + (str[i] - '0');
    }
    return val;
}

file_t* parse_tar_and_find(uint8_t* data, uint32_t size, const char* target) {
    static file_t result;
    uint8_t* p   = data;
    uint8_t* end = data + size;

    while (p < end) {
        tar_header_t* h = (tar_header_t*)p;
        if (h->name[0] == 0) break;

        int      filesize  = oct_to_int(h->size, 12);
        uint8_t* file_data = p + 512;

        int i = 0;
        while (target[i] && h->name[i] && target[i] == h->name[i]) i++;

        if (target[i] == 0 && h->name[i] == 0) {
            result.data = file_data;
            result.size = filesize;
            int j = 0;
            while (h->name[j] && j < 127) { result.name[j] = h->name[j]; j++; }
            result.name[j] = 0;
            return &result;
        }

        int jump = ((filesize + 511) / 512) * 512;
        p += 512 + jump;
    }
    return 0;
}


void append_hex(uint64_t val) {
    char hex[] = "0123456789ABCDEF";
    char buf[17];
    for (int i = 0; i < 16; i++) {
        buf[15 - i] = hex[val & 0xF];
        val >>= 4;
    }
    buf[16] = 0;
    append_text(buf);
}

void page_fault_handler_c(uint64_t* stack) {
    (void)stack;

    uint64_t addr;
    asm("mov %%cr2, %0" : "=r"(addr));

    append_text("PAGE FAULT!\n");
    append_text("Address: 0x");
    append_hex(addr);
    append_text("\n");

    while (1);
}


/* ================= KERNEL MAIN ================= */

void kernel_main(uint64_t magic, uint64_t addr) {

    if ((uint32_t)magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        while (1);
    }
    

    uint8_t* ptr        = (uint8_t*)addr;
    uint32_t total_size = *(uint32_t*)ptr;
    ptr += 8;

    framebuffer_info_t fb_info;
    int fb_found = 0;

    uint8_t* الوسيط_start = 0;
    uint32_t الوسيط_size  = 0;

    while (ptr < (uint8_t*)addr + total_size) {
        struct multiboot2_tag* tag = (struct multiboot2_tag*)ptr;
        if (tag->type == 0) break;

        if (tag->type == 8) {
            struct multiboot2_tag_framebuffer* fb = (void*)tag;
            fb_info.address = fb->framebuffer_addr;
            fb_info.width   = fb->framebuffer_width;
            fb_info.height  = fb->framebuffer_height;
            fb_info.pitch   = fb->framebuffer_pitch;
            fb_info.bpp     = fb->framebuffer_bpp;
            fb_found        = 1;
        }

        if (tag->type == 3) {
            struct multiboot2_tag_module* mod = (void*)tag;
            الوسيط_start = (uint8_t*)(uintptr_t)mod->mod_start;
            الوسيط_size  = mod->mod_end - mod->mod_start;
        }

        ptr += (tag->size + 7) & ~7;
    }

    if (!fb_found) {
        while (1);
    }

    memory_api.init();

    memory_api.map(MMIO_BASE + 0xFEE00000, 0xFEE00000, MEM_WRITE); // LAPIC
    memory_api.map(MMIO_BASE + 0xFEC00000, 0xFEC00000, MEM_WRITE); // IOAPIC

    uint64_t frame = memory_api.alloc_page();
    uint64_t test_addr = 0xFFFF800000400000ULL;
    memory_api.map(test_addr, frame, MEM_WRITE);
    char* test_mem = (char*)test_addr;
    *test_mem = 0x42;

    video_api.init(&fb_info);
    //video_api.clear(0x00224488);
    //video_api.swap();

    if (الوسيط_start) {

        parse_الوسيط(
            الوسيط_start,
            الوسيط_size
        );

        init_file =
            find_file("init.txt");

        file_t* rootfs =
            find_file("rootfs.tar");

        if (rootfs) {

            txt_file =
                parse_tar_and_find(
                    rootfs->data,
                    rootfs->size,
                    "./txt.txt"
                );
                //file_t* app =
               // find_file("app.bin");
    
                //if (app) {
            
                   // append_text(
                    //"[KERNEL] starting app.bin\n"
                   // );

                    //app_run(app);
                }
            
    }
    update_display();
    display_api.render();;
    display_api.init();
    interrupt_api.init();

    interrupt_api.set_gate(
        32,
        (uint64_t)isr_timer
    );
    interrupt_api.set_gate(
        33,
        (uint64_t)isr_keyboard
    );
    interrupt_api.set_gate(
        44,
        (uint64_t)isr_mouse
    );

    extern void isr_page_fault();

    interrupt_api.set_gate(
        14,
        (uint64_t)isr_page_fault
    );



    lapic_init();
    ioapic_init();
    lapic_timer_init();

    interrupt_api.irq_register(0,  driver_api.timer->handler);
    interrupt_api.irq_register(1, driver_api.keyboard->handler);
    interrupt_api.irq_register(12, driver_api.mouse->handler);

    driver_api.mouse->init();
    driver_api.keyboard->init();

    /* ── idle ── */
    idle_p = process_api.create(idle);

    /* ── task_entry + IPC_START_PROCESS (كما كان) ── */


    process_t* p = process_api.create(task_entry);
    scheduler_api.add(p);

    Message msg;
    msg.sender   = 0;
    msg.receiver = process_api.get_pid(p);
    msg.type     = IPC_START_PROCESS;
    msg.size     = 0;
    ipc_send(process_api.get_pid(p), &msg);
    process_t* gui  = process_api.create(gui_task);
    scheduler_api.add(gui);
/*
    process_t* a =
        process_api.create(vm_task_a);

    scheduler_api.add(a);

    process_t* b =
        process_api.create(vm_task_b);

    scheduler_api.add(b);
*/

    /* ── GUI + test ── */

    //process_t* test = process_api.create(test_task, 0);

    //scheduler_api.add(test);

    /* ── اختبار IPC ──
     *
     * المستقبِل يُنشأ أولاً ونحفظ PID-ه في receiver_pid
     * حتى يعرف المُرسِل إلى أين يرسل.
     * نستخدم process_api.start بدلاً من scheduler_api.add
     * لأن start تضيف للجدولة بنفسها.
     */
    //process_t* p_recv = process_api.create(task_receiver, 0);
   // receiver_pid = (int)process_api.get_pid(p_recv);   /* احفظ قبل add */
    //process_api.start(p_recv);

   // process_t* p_send = process_api.create(task_sender, 0);
   // process_api.start(p_send);

    asm volatile("sti");
    scheduler_api.schedule();

    while (1) {

        asm volatile("hlt");
    }
}
