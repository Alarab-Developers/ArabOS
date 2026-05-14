#include "مُتحكم_المقاطعات.h"

#define IA32_APIC_BASE_MSR  0x1B
#define MMIO_BASE           0xFFFF900000000000ULL
#define LAPIC_PHYS          0xFEE00000ULL

/*
 * ✅ العنوان الـ virtual للـ LAPIC
 * يجب أن يُعمَل له map في النواه قبل استدعاء lapic_init:
 *   memory_api.map(MMIO_BASE + 0xFEE00000, 0xFEE00000, PAGE_WRITE);
 */
static volatile uint32_t* lapic =
    (volatile uint32_t*)(MMIO_BASE + LAPIC_PHYS);

static inline uint64_t read_msr(uint32_t msr) {
    uint32_t low, high;
    asm volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((uint64_t)high << 32) | low;
}

static inline void write_msr(uint32_t msr, uint64_t value) {
    uint32_t low  = (uint32_t)(value & 0xFFFFFFFF);
    uint32_t high = (uint32_t)(value >> 32);
    asm volatile ("wrmsr" : : "c"(msr), "a"(low), "d"(high));
}

static inline void lapic_write(uint32_t reg, uint32_t value) {
    lapic[reg / 4] = value;
}

static inline uint32_t lapic_read(uint32_t reg) {
    return lapic[reg / 4];
}

void lapic_init() {
    /* تفعيل APIC عبر MSR */
    uint64_t apic = read_msr(IA32_APIC_BASE_MSR);
    apic |= (1 << 11);
    write_msr(IA32_APIC_BASE_MSR, apic);

    /* Spurious Interrupt Vector — تفعيل + vector 0xFF */
    lapic_write(0xF0, 0x100 | 0xFF);
}

void lapic_eoi() {
    /* إرسال End Of Interrupt */
    lapic_write(0xB0, 0);
}

void lapic_timer_init() {
    lapic_write(0x3E0, 0x3);            /* Divide by 16 */
    lapic_write(0x320, 32 | (1 << 17)); /* Periodic, vector 32 */
    lapic_write(0x380, 200000);         /* Initial count */
}
