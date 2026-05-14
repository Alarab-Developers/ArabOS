#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

#define PAGE_PRESENT  1
#define PAGE_WRITABLE 2
#define PAGE_HUGE     (1 << 7)

#define PAGE_WRITE    0x2
#define PAGE_USER     0x4

#define ENTRIES 512

typedef uint64_t* page_table_t;

void paging_init(void);
uint64_t* paging_get_pml4();
extern uint64_t* kernel_pml4;

/*
 * load_cr3 — يقبل عنوان VIRTUAL لجداول الكيرنل الثابتة
 * يطرح HIGHER_HALF_BASE داخلياً للحصول على الفيزيائي
 */
void load_cr3(uint64_t* pml4);

/*
 * load_cr3_phys — يقبل عنوان فيزيائي مباشرةً
 * استخدمها مع p->pml4 وapi_switch_address_space
 */
void load_cr3_phys(uint64_t phys);

void map_page(uint64_t* target_pml4, uint64_t virt,
              uint64_t phys, uint64_t flags);

void copy_kernel_mappings(uint64_t* pml4);

#endif
