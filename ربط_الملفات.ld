/*
 * ربط_الملفات.ld
 *
 * التخطيط الفيزيائي (LMA) والوهمي (VMA):
 *
 *  Physical    Virtual                    Section
 *  ──────────  ─────────────────────────  ────────────────
 *  0x100000    0x100000                   .multiboot2
 *  0x100030+   0x100030+                  .boot32data  (جداول paging + GDT)
 *  after       after                      .boot32bss   (stack)
 *  after       after                      .boot32      (كود 32-bit)
 *  ──────────  ─────────────────────────
 *  0x???       0xFFFF800000???            .text        (كود 64-bit)
 *  ...         ...                        .rodata
 *  ...         ...                        .data
 *  ...         ...                        .bss
 *
 * القاعدة: AT(x) يحدد LMA، بينما . يحدد VMA
 * الـ bootstrap sections: VMA = LMA (فيزيائية)
 * الـ kernel sections:    VMA = LMA + 0xFFFF800000000000
 */

ENTRY(_start)

HIGHER_HALF = 0xFFFF800000000000;

SECTIONS
{
    . = 1M;

    .multiboot2 : { KEEP(*(.multiboot2)) }

    .boot32data : { *(.boot32data) }

    .boot32bss (NOLOAD) : { *(.boot32bss) }

    .boot32 : { *(.boot32) }

    /* ── Higher Half ── */
    . += HIGHER_HALF;
    . = ALIGN(4096);

    .text : AT(ADDR(.text) - HIGHER_HALF) {
        *(.text*)
    }

    . = ALIGN(4096);
    .rodata : AT(ADDR(.rodata) - HIGHER_HALF) {
        *(.rodata*)
    }

    . = ALIGN(4096);
    .data : AT(ADDR(.data) - HIGHER_HALF) {
        *(.data*)
    }

    . = ALIGN(4096);
    .bss : AT(ADDR(.bss) - HIGHER_HALF) {
        *(COMMON)
        *(.bss*)
    }

    _kernel_end = .;

    /DISCARD/ : {
        *(.eh_frame)
        *(.comment)
        *(.note*)
    }
}
