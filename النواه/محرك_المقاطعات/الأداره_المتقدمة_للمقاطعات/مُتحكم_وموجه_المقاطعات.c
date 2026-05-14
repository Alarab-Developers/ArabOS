#include "مُتحكم_وموجه_المقاطعات.h"

#define MMIO_BASE    0xFFFF900000000000ULL
#define IOAPIC_PHYS  0xFEC00000ULL

/*
 * ✅ العنوان الـ virtual للـ IOAPIC
 * يجب أن يُعمَل له map في النواه قبل استدعاء ioapic_init:
 *   memory_api.map(MMIO_BASE + 0xFEC00000, 0xFEC00000, PAGE_WRITE);
 */
static volatile uint32_t* ioapic =
    (volatile uint32_t*)(MMIO_BASE + IOAPIC_PHYS);

#define IOREGSEL  0x00
#define IOWIN     0x10

static inline void ioapic_write(uint32_t reg, uint32_t value) {
    ioapic[IOREGSEL / 4] = reg;
    ioapic[IOWIN    / 4] = value;
}

static inline uint32_t ioapic_read(uint32_t reg) {
    ioapic[IOREGSEL / 4] = reg;
    return ioapic[IOWIN / 4];
}

void ioapic_set_entry(int irq, uint8_t vector) {
    uint32_t reg = 0x10 + irq * 2;

    uint32_t low = 0;
    low |= vector;   /* vector */
    /* delivery=fixed(000), dest=physical, polarity=high, trigger=edge, mask=0 */

    ioapic_write(reg,     low);
    ioapic_write(reg + 1, 0 << 24);  /* destination: CPU 0 */
}

void ioapic_init() {
    ioapic_set_entry(1,  33);   /* keyboard → vector 33 */
    ioapic_set_entry(12, 44);   /* mouse    → vector 44 */
}
